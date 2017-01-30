/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   I/O routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef NETBSD
#include <soundcard.h>
#else
#include <sys/soundcard.h>
#endif
#ifdef LINUX
#include <linux/rtc.h>
#endif

#include "types.h"
#include "io.h"
#include "cwirc.h"
#include "keyer.h"
#include "cwsound.h"
#include "cwdecoder.h"
#include "propagation.h"
#include "extension.h"
#include "ipc.h"



/* Definitions */
#define DSR_LINE_CLOSED_KEY		1
#define CTS_LINE_CLOSED_KEY		1
#define DTR_LINE_SET			0
#define RTS_LINE_SET			0
#define BASE_BEEP_FREQ			1100	/* Hz */
#define DSP_SAMPLE_FREQ			44100
#define DSP_STEREO			1
#define DSP_16BITS			1
#define DSP_FRAG_SIZE  			8	/* bytes, in power of 2 */
#define DSP_FRAG_BUFFER			25	/* Prebuffered fradments */
#define BYTES_PER_FRAG			(1<<DSP_FRAG_SIZE)
#define SAMPLES_PER_TICK		(BYTES_PER_FRAG /		\
					((DSP_STEREO+1)*(DSP_16BITS+1)))
#define RTC_RATE			1024	/* IRQs per second */
#define SENDER_SIGNAL_FADEOUT_DELAY	1000	/* ms */



/* Global variables */
static int fddsp=-1;
static int fdser=-1;
static int fdrtc=-1;
static char errmsg[FILENAME_MAX+32];
static struct cwirc_extension_api *ext_sharedmem;



/* Prototypes */
static int io_process(int shmid,int ext_shmid);
char *open_snd_dev(char *snddev);
char *open_serial_dev(char *serialdev);
char *open_rtc_dev(void);
void close_snd_dev(void);
void close_serial_dev(void);
void close_rtc_dev(void);
static T_BOOL read_straight_key_serial(void);
static T_BOOL read_iambic_key_serial(double ticklen);
static T_BOOL read_straight_key_mouse(void);
static T_BOOL read_iambic_key_mouse(double ticklen);
static int set_sounder_line(T_BOOL state);
static int enable_rtc_interrupts(void);
static void send_errmsg(char const *fmt,...);
static void cleanup(void);



/* Spawn the I/O process. */
int cwirc_spawn_io_process(int shmid,int ext_shmid)
{
  int iopid=-1;			/* PID of the I/O process */

  switch(iopid=fork())
  {
  case -1:
    return(-1);
    break;
  case 0:
    _exit(io_process(shmid,ext_shmid));
    break;
  }

  return(iopid);
}



/* I/O process : output morse signals coming from the irc channel as sound,
   read the morse keys and outputs the corresponding signal to the irc server.
   The function is passed the id of the CWirc shared memory block, and the id of
   the CWirc extension API's shared memory block. */
static int io_process(int shmid,int ext_shmid)
{
  double ticklen;	/* Time between 2 write calls to the soundcard, in ms */
  double real_samples_per_tick=0;
  int rounded_samples_per_tick=0;
  double samples_per_tick_fraction=0;
  double samples_per_tick_drift=0;
  int samples_per_tick=0;
  int bytes_per_frag;
  double sender_signal_strength;
  double total_signal_strength;
  double sndbuf[SAMPLES_PER_TICK];
  double remote_signals_sndbuf[SAMPLES_PER_TICK];
  double remote_signals_snd_total_weight=0;
  double local_signal_sndbuf[SAMPLES_PER_TICK];
  T_BOOL local_signal_present=0;
  T_BOOL local_signal_present_prev=0;
  T_S16 frag[SAMPLES_PER_TICK*(DSP_STEREO+1)];
  int beep_freq;
  unsigned long rtc_data;
  T_BOOL remote_keys;
  T_BOOL serialkey=0;
  T_BOOL mousekey=0;
  T_BOOL key=0;
  T_BOOL key_prev=0;
  T_BOOL key_evts_send=0;
  T_BOOL key_evts_send_prev=0;
  unsigned long long local_keydown_tickcnt=0;
  unsigned long long local_keyup_tickcnt=999999;	/* Start quietly */
  int local_xmit_buf[XMIT_BUF_MAX_SIZE];
  int xmit_buf_i=0;
  double xmit_ms_count=0,xmit_ms_count_prev=0;
  double xmit_drift=0;
  T_BOOL do_xmit=0;
  int i,j,k;
  double l;
  char *errormsg;
  int ext_lock_semno=0;
  T_BOOL process_in_error=0;
  
  bytes_per_frag=BYTES_PER_FRAG;

  /* Attach to the shared memory block */
  if((sharedmem=(struct cwirc_shm_block *)cwirc_shm_attach(shmid))
	==(struct cwirc_shm_block *)-1)
  {
    printf("CWirc (I/O) : error : can't attach to shared memory.\n");
    return(-1);
  }

  /* Attach to the extension API's shared memory block */
  if((ext_sharedmem=(struct cwirc_extension_api *)cwirc_shm_attach(ext_shmid))
	==(struct cwirc_extension_api *)-1)
  {
    printf("CWirc (I/O) : error : can't attach to the extension API's "
    		"shared memory.\n");
    cwirc_shm_detach(sharedmem);
    return(-1);
  }

  /* Run till we're told to stop */
  while(!sharedmem->stop_frontend)
  {
    /* If we're in error, twiddle our thumbs until we're told to reconfigure */
    if(process_in_error && !sharedmem->reconfigure_io_process)
    {
      /* Clear the reception buffer (don't allow it to fill up since we're not
         emptying it) */
      if(!cwirc_sem_P(sharedmem->semid,SEM_ST))	/* Acquire the semaphore */
      {
        for(i=0;i<MAX_SENDERS;i++)
        sharedmem->sender[i].name[0]=0;

        /* Release the semaphore */
        cwirc_sem_V(sharedmem->semid,SEM_ST);
      }

      usleep(200000);
      continue;
    }

    process_in_error=0;
    sharedmem->reconfigure_io_process=0;

    /* Do we output to a sound device ? */
    if(sharedmem->do_snddev_output)
    {
      /* Open the sound device */
      if((errormsg=open_snd_dev(sharedmem->snddev))!=NULL)
      {
        send_errmsg("Error : %s\n",errormsg);
        cleanup();
        process_in_error=1;
        continue;
      }

      ticklen=((double)1000*(double)SAMPLES_PER_TICK)/(double)DSP_SAMPLE_FREQ;
      samples_per_tick=SAMPLES_PER_TICK;
    }
    else
    {
      /* Open the rtc instead, to give us a nice fast timing device */
      if((errormsg=open_rtc_dev())!=NULL)
      {
        send_errmsg("Error : %s\n",errormsg);
        cleanup();
        process_in_error=1;
        continue;
      }
      /* Enable the RTC periodic interrupt */
      if(!sharedmem->do_snddev_output && enable_rtc_interrupts())
      {
        close_rtc_dev();
        send_errmsg("Error enabling real-time clock periodic interrupts.\n");
        process_in_error=1;
        continue;
      }

      ticklen=(double)1000/(double)RTC_RATE;
      real_samples_per_tick=(double)DSP_SAMPLE_FREQ/(double)RTC_RATE;
      rounded_samples_per_tick=real_samples_per_tick;
      samples_per_tick_fraction=real_samples_per_tick-
		(double)rounded_samples_per_tick;
      samples_per_tick_drift=0;
    }

    /* Open the serial port if we need to */
    if((sharedmem->do_key_input || sharedmem->do_sounder_output) &&
	(errormsg=open_serial_dev(sharedmem->serialdev))!=NULL)
    {
      send_errmsg("Error : %s\n",errormsg);
      cleanup();
      process_in_error=1;
      continue;
    }
  
    /* Main loop */
    while(!sharedmem->reconfigure_io_process && !process_in_error &&
		!sharedmem->stop_frontend)
    {
      /* Are we paced only by the RTC device ? */
      if(!sharedmem->do_snddev_output)
      {
        /* Recalculate the apparent samples per tick value to compensate for the
           fact that the real number of samples per tick isn't exact, so that
           we generate sound samples for an extension program that doesn't lag
           over time */
        if(samples_per_tick_drift>=1)
        {
          samples_per_tick_drift-=1;
          samples_per_tick=rounded_samples_per_tick+1;
        }
        else
          samples_per_tick=rounded_samples_per_tick;
        samples_per_tick_drift+=samples_per_tick_fraction;
      }

      /* Initialize the sound buffer */
      remote_signals_snd_total_weight=0;
      for(i=0;i<samples_per_tick;i++)
        remote_signals_sndbuf[i]=0;

      remote_keys=0;
      total_signal_strength=0;

      /* Acquire the semaphore to prevent being reconfigured while we work */
      if(!cwirc_sem_P(sharedmem->semid,SEM_IO_PROCESS_WORKING))
      {
        /* Acquire the semaphore to access the senders table */
        if(!cwirc_sem_P(sharedmem->semid,SEM_ST))
        {
          for(i=0;i<MAX_SENDERS;i++)
          {
            if(sharedmem->sender[i].name[0])
            {
              /* Calculate the sender's signal strength */
              sender_signal_strength=(sharedmem->simulate_signal_strength &&
			sharedmem->gridsquare[0])?
			(sharedmem->sender[i].signal_strength>=0?
			sharedmem->sender[i].signal_strength:
			(double)sharedmem->default_signal_strength/100):1;

              /* Simulate sporadic-E */
              if(sharedmem->simulate_sporadicE)
                cwirc_simulate_sporadicE(&sender_signal_strength,ticklen);

              /* If playback start timeout hasn't reached 0 yet, decrease it. */
              if(sharedmem->sender[i].playback_start_timeout>0)
              {
                sharedmem->sender[i].playback_start_timeout-=ticklen;

                if(sharedmem->sender[i].playback_start_timeout>0)
                {
                  /* Maintain the sender's signal strength to 0 or whatever it
                     was when it started to fade out so we don't get a sudden
                     rise of background signal if we have a short buffer
                     underrun for that sender */
                  remote_signals_snd_total_weight+=sender_signal_strength *
			(sharedmem->sender[i].playback_stop_timeout/
			SENDER_SIGNAL_FADEOUT_DELAY);
                }
                else
                  sharedmem->sender[i].playback_stop_timeout=0;
              }

              /* If the playback stop timeout hasn't reached 0 yet, decrease it,
                 fade out the sender's signal and remove the sender when the
                 timeout reaches 0 */
              else if(sharedmem->sender[i].playback_stop_timeout>0)
              {
                sharedmem->sender[i].playback_stop_timeout-=ticklen;
                if(sharedmem->sender[i].playback_stop_timeout<=0)
                  sharedmem->sender[i].name[0]=0;	/* Remove sender */

                remote_signals_snd_total_weight+=sender_signal_strength *
			(sharedmem->sender[i].playback_stop_timeout/
			SENDER_SIGNAL_FADEOUT_DELAY);
              }

              else		/* We can consume this sender's events */
              {
                /* Bias the beep frequency according to the RX pitch, and the
                   sender's name and so the listener can discriminate between
                   several senders.*/
                for(j=k=0;j<strlen(sharedmem->sender[i].name);j++)
                  k+=sharedmem->sender[i].name[j];
                k/=strlen(sharedmem->sender[i].name);
                beep_freq=BASE_BEEP_FREQ+(sharedmem->cwrxpitch*20)-
			(((signed int)sharedmem->sender[i].name[0])-80)*5;

                /* If the sender's key changed state, reset the current
                   keyup or keydown tick counter */
                if(sharedmem->sender[i].keystate[sharedmem->sender[i].
			buf_head] && !sharedmem->sender[i].keystate_prev)
                  sharedmem->sender[i].keydown_tickcnt=0;
                if(!sharedmem->sender[i].keystate[sharedmem->sender[i].
			buf_head] && sharedmem->sender[i].keystate_prev)
                  sharedmem->sender[i].keyup_tickcnt=0;

                /* Add the sender's signal to the existing ones */
                generate_cw_sound_fragment(sharedmem->cwsound,
			sharedmem->sender[i].keystate_prev,sharedmem->
			sender[i].keystate[sharedmem->sender[i].buf_head],
			DSP_SAMPLE_FREQ,samples_per_tick,beep_freq,
			sender_signal_strength*100,sharedmem->sender[i].
			keyup_tickcnt,sharedmem->sender[i].keydown_tickcnt,
			sndbuf);
                for(j=0;j<samples_per_tick;j++)
                  remote_signals_sndbuf[j]+=sndbuf[j];

                remote_signals_snd_total_weight+=sender_signal_strength;

                /* Increment the keyup and keydown tick counters */
                sharedmem->sender[i].keyup_tickcnt+=samples_per_tick;
                sharedmem->sender[i].keydown_tickcnt+=samples_per_tick;

                sharedmem->sender[i].keystate_prev=sharedmem->sender[i].
			keystate[sharedmem->sender[i].buf_head];

                /* If the sender's key if down, add the sender's signal strength
                   to the total signal strength  */
                if(sharedmem->sender[i].keystate[sharedmem->sender[i].buf_head])
                {
                  total_signal_strength+=sender_signal_strength;

                  /* Only register this remote key if the sender's signal is
                     at least S+1 above S0 or the QRN floor */
                  if(sender_signal_strength*100-(sharedmem->simulate_qrn?
			sharedmem->qrnlevel:0) > 5)
                    remote_keys=1;
                }

                /* Decrease the delay counter of the sender */
                sharedmem->sender[i].kcdelay[sharedmem->sender[i].buf_head]-=
			ticklen;
                while(sharedmem->sender[i].kcdelay[sharedmem->sender[i].
			buf_head]<=0)
                {
                  /* Move the sender's buffer head to the next delay counter*/
                  j=sharedmem->sender[i].buf_head;
                  if((++sharedmem->sender[i].buf_head)==MAX_EVT_BUFFER)
                    sharedmem->sender[i].buf_head=0;

                  /* If buffer is empty, start sender playback stop timeout */
                  if(sharedmem->sender[i].kcdelay[sharedmem->sender[i].
			buf_head]<=0)
                  {
                    sharedmem->sender[i].playback_stop_timeout=
				SENDER_SIGNAL_FADEOUT_DELAY;
                    break;
                  }
                  else
                    /* Compensate the time drift between the sender and us */
                    sharedmem->sender[i].kcdelay[sharedmem->sender[i].
				buf_head]+=sharedmem->sender[i].kcdelay[j];
                }
              }
            }
          }

          /* Release the semaphore to access the senders table */
          cwirc_sem_V(sharedmem->semid,SEM_ST);
        }

        /* Cap the total received signal strength */
        if(total_signal_strength>1)
          total_signal_strength=1;

        /* Read the key connected to the serial port if we need to */
        if(sharedmem->do_key_input)
          serialkey=sharedmem->doiambic?read_iambic_key_serial(ticklen):
				read_straight_key_serial();

        /* Read the mouse "key" if we need to */
        if(sharedmem->do_mouse_input)
          mousekey=sharedmem->doiambic?read_iambic_key_mouse(ticklen):
				read_straight_key_mouse();

        /* Combine all the key inputs and the "key" from the extension API */
        key_prev=key;
        key=serialkey || mousekey || ext_sharedmem->in_key;

        /* If we're in sidetone mode, silence the key input flags used to
           determine the events to send. Otherwise, make them follow the real
           key input states */
        key_evts_send_prev=key_evts_send;
        key_evts_send=sharedmem->sidetone_mode?0:key;

        /* Send the remote keys and local key states to the morse decoder */
        cwirc_decode_cw(remote_keys || key,ticklen,sharedmem->cwcodeset);

        /* If the local key is down, its signal drowns out all the incoming
           signal */
        if(key)
          total_signal_strength=1;

        /* If the local key changed state, reset the current keyup or keydown
           tick counter */
        if(key && !key_prev)
          local_keydown_tickcnt=0;
        if(!key && key_prev)
          local_keyup_tickcnt=0;

        /* Bias the beep frequency according to the TX pitch */
        beep_freq=BASE_BEEP_FREQ+(sharedmem->cwtxpitch*20);

        /* generate the local signal */
        local_signal_present_prev=local_signal_present;
        local_signal_present=generate_cw_sound_fragment(
		sharedmem->cwsound,key_prev,key,DSP_SAMPLE_FREQ,
		samples_per_tick,beep_freq,100,local_keyup_tickcnt,
		local_keydown_tickcnt,local_signal_sndbuf);

        /* Increment the keyup and keydown tick counters */
        local_keyup_tickcnt+=samples_per_tick;
        local_keydown_tickcnt+=samples_per_tick;

        /* If we have no countdown going and the key goes down, start counter
           and enable transmission to the irc server. Save current cw channel
           so we don't end up emitting on another one if user changes channel
           before we flush the buffer. */
        if(xmit_ms_count<=0 && xmit_ms_count_prev<=0 &&
		key_evts_send && !key_evts_send_prev)
        {
          xmit_ms_count=xmit_ms_count_prev=XMIT_BUF_DELAY;
          xmit_drift=0;
          key_evts_send_prev=key_evts_send;
          do_xmit=1;
        }

        /* Detect changes of the key, store then in the buffer. If the countdown
           has reached 0 and the key still hasn't changed, artificially close
           the event */
        if((key_evts_send_prev!=key_evts_send ||
		(xmit_ms_count<=0 && xmit_ms_count_prev>0 &&
		key_evts_send_prev==key_evts_send)) &&
		xmit_buf_i<XMIT_BUF_MAX_SIZE)
        {
          /* Store the duration and type of the event */
          l=xmit_ms_count_prev-xmit_ms_count;
          local_xmit_buf[xmit_buf_i]=l;
          xmit_drift+=l-(double)local_xmit_buf[xmit_buf_i];

          /* Compensate the drift due to the float -> integer conversion */
          if(xmit_drift>=1)
          {
            local_xmit_buf[xmit_buf_i]++;
            xmit_drift-=1;
          }

          /* Key up or down ? */
          if(!key_evts_send_prev)
            local_xmit_buf[xmit_buf_i]=-local_xmit_buf[xmit_buf_i];

          /* Only store the event if its duration isn't zero (case when we have
             a sub-millisecond event, rounded to int, without a drift
             compensation added afterward) */
          if(local_xmit_buf[xmit_buf_i])
            xmit_buf_i++;

          xmit_ms_count_prev=xmit_ms_count;
        }

        /* Do we need to transmit the buffer to the irc server ? */
        if(do_xmit && xmit_ms_count<=0)
        {
          /* If there's nothing to transmit or just a single silence, disable
             the transmission */
          if(!xmit_buf_i || (xmit_buf_i==1 && local_xmit_buf[0]<0))
          {
            do_xmit=0;
            xmit_ms_count=xmit_ms_count_prev=0;
          }
          else
          {
            /* Transmit all the timings to the IRC server : */
            /* Acquire the semaphore to acess the xmit buffer */
            if(!cwirc_sem_P(sharedmem->semid,SEM_XMIT_BUF))
            {
              /* Copy the local xmit buffer to the shared memory block */
              for(i=0;i<xmit_buf_i;i++)
                sharedmem->xmit_buf[i]=local_xmit_buf[i];

              /* Indicate how many events have to be flushed */
              sharedmem->xmit_buf_flush_nb_evts=xmit_buf_i;

              /* Release the semaphore */
              cwirc_sem_V(sharedmem->semid,SEM_XMIT_BUF);
            }

            /* Restart the counter */
            xmit_ms_count+=XMIT_BUF_DELAY;
            xmit_ms_count_prev+=XMIT_BUF_DELAY;
          }

          xmit_buf_i=0;
        }

        /* Do the xmit buffer timeout countdown */
        xmit_ms_count-=xmit_ms_count>0?ticklen:0;

        /* Add QRN to the local signal */
        if(sharedmem->simulate_qrn)
        {
          generate_qrn_sound_fragment(DSP_SAMPLE_FREQ,samples_per_tick,100,
			sndbuf);
          for(i=0;i<samples_per_tick;i++)
            remote_signals_sndbuf[i]=(remote_signals_sndbuf[i]*(100-
		sharedmem->qrnlevel) + (sndbuf[i]*sharedmem->qrnlevel))/100;
        }

        /* Finish mixing all the incoming signals, if we have no local
           signal */
        if(!local_signal_present || !local_signal_present_prev)
          for(i=0;i<samples_per_tick;i++)
            remote_signals_sndbuf[i]/=remote_signals_snd_total_weight+1;

        /* If there's a local signal, it drowns out all the incoming signals,
           but we make sure it's faded in and out softly so it doesn't click*/
        if(local_signal_present)
          if(!local_signal_present_prev)
            for(i=0;i<samples_per_tick;i++)
              sndbuf[i]=remote_signals_sndbuf[i]*
			((double)(samples_per_tick-i)/(double)samples_per_tick)
			+ .5*local_signal_sndbuf[i]*((double)i/
			(double)samples_per_tick);
          else
            for(i=0;i<samples_per_tick;i++)
              sndbuf[i]=.5*local_signal_sndbuf[i];
        else
          if(local_signal_present_prev)
            for(i=0;i<samples_per_tick;i++)
              sndbuf[i]=remote_signals_sndbuf[i]*
			((double)i/(double)samples_per_tick) +
			.5*local_signal_sndbuf[i]*((double)(samples_per_tick-i)/
			(double)samples_per_tick);
          else
            for(i=0;i<samples_per_tick;i++)
              sndbuf[i]=remote_signals_sndbuf[i];

        /* Apply the squelch */
        squelch(sharedmem->squelch,sndbuf,samples_per_tick,ticklen);

        /* Append the sound fragment to the extension API's audio ring buffer */
        for(i=0;i<samples_per_tick;i++)
        {
          ext_sharedmem->out_audiobuf[ext_sharedmem->out_audiobuf_end++]=
		sndbuf[i]*32767;
          if(ext_sharedmem->out_audiobuf_end>=AUDIOBUF_SIZE)
            ext_sharedmem->out_audiobuf_end-=AUDIOBUF_SIZE;
          if(ext_sharedmem->out_audiobuf_end==ext_sharedmem->out_audiobuf_start)
            if(++ext_sharedmem->out_audiobuf_start>=AUDIOBUF_SIZE)
              ext_sharedmem->out_audiobuf_start-=AUDIOBUF_SIZE;
        }

        /* Release one of the extension API's semaphores */
        cwirc_sem_V(ext_sharedmem->semid,ext_lock_semno);

        /* Apply the volume */
        for(i=0;i<samples_per_tick;i++)
          sndbuf[i]=(sndbuf[i]*sharedmem->volume)/100;

        /* Create the final sound fragment */
        for(i=0;i<samples_per_tick;i++)
          frag[i*2]=frag[i*2+1]=sndbuf[i]*32767;

        /* Make the QRN signal the total signal strength's floor value */
        if(sharedmem->simulate_qrn &&
		sharedmem->qrnlevel>total_signal_strength*100)
          total_signal_strength=(double)sharedmem->qrnlevel/100;

        /* Assert this for the gui's S-meter */
        sharedmem->recv_signal=total_signal_strength*100;

        /* Play the morse code on the external sounder if we need to */
        if(sharedmem->do_sounder_output)
          set_sounder_line(remote_keys | key);

        /* Do we output to a sound device ? */
        if(sharedmem->do_snddev_output)
        {
          /* Release the semaphore to prevent being reconfigured while we work*/
          cwirc_sem_V(sharedmem->semid,SEM_IO_PROCESS_WORKING);

          /* Write the sound fragment through the sound device */
          i=0;
          j=bytes_per_frag;
          do
          {
            i=write(fddsp,frag+i,j);
            j-=i;
          }
          while(i>0 && j>0);

          /* Did we encounter a write error ? */
          if(i<=0)
          {
            send_errmsg("Error writing to the sound device.\n");
            process_in_error=1;
            cleanup();

            /* Re-acquire the extension API's semaphore and switch semaphore */
            cwirc_sem_P(ext_sharedmem->semid,ext_lock_semno);
            ext_lock_semno=ext_lock_semno?0:1;

            continue;
          }
        }
        else	/* No sound card to sync us : get our timing from the rtc */
        {
          /* Release the semaphore to prevent being reconfigured while we work*/
          cwirc_sem_V(sharedmem->semid,SEM_IO_PROCESS_WORKING);

          /* Wait for the next periodic interrupt */
          if(read(fdrtc,&rtc_data,sizeof(rtc_data))==-1)
          {
            process_in_error=1;
            cleanup();

            /* Re-acquire the extension API's semaphore and switch semaphore */
            cwirc_sem_P(ext_sharedmem->semid,ext_lock_semno);
            ext_lock_semno=ext_lock_semno?0:1;

            continue;
          }
        }

        /* Re-acquire the extension API's semaphore and switch semaphore */
        cwirc_sem_P(ext_sharedmem->semid,ext_lock_semno);
        ext_lock_semno=ext_lock_semno?0:1;
      }
    }

    /* Make sure the sounder is inactive */
    if(sharedmem->do_sounder_output)
      set_sounder_line(0);

    cleanup();
  }

  cwirc_shm_detach(ext_sharedmem);
  cwirc_shm_detach(sharedmem);

  return(0);
}



/* Read a straight key connected to a serial port, do debouncing, then return
   the key state */
static T_BOOL read_straight_key_serial(void)
{
  int serstatus;
  static int debounce_buf_i=0;
  static int debounce_buf[DEBOUNCE_BUF_MAX_SIZE];
  static T_BOOL keystate=0;
  int i,j;

  /* Read the key state */
  if(ioctl(fdser,TIOCMGET,&serstatus)!=-1)
  {
    debounce_buf[debounce_buf_i]=(serstatus & (TIOCM_DSR|TIOCM_CTS))?
    		DSR_LINE_CLOSED_KEY:!DSR_LINE_CLOSED_KEY;

    debounce_buf_i++;
  }

  /* If the debounce buffer is full, determine the state of the key */
  if(debounce_buf_i>=sharedmem->debounce)
  {
    debounce_buf_i=0;

    j=0;
    for(i=0;i<sharedmem->debounce;i++)
      if(debounce_buf[i])
        j++;
    keystate=(j>sharedmem->debounce/2)?1:0;
  }

  return(keystate);
}



/* Read an iambic key connected to a serial port, do debouncing, emulate a
   straight key, then return the emulated key state */
static T_BOOL read_iambic_key_serial(double ticklen)
{
  int serstatus;
  static T_BOOL dah_debounce_buf[DEBOUNCE_BUF_MAX_SIZE];
  static T_BOOL dit_debounce_buf[DEBOUNCE_BUF_MAX_SIZE];
  static int debounce_buf_i=0;
  static int dah=0,dit=0;
  static struct cwirc_keyer_state is={0};
  int i,j;

  /* Read the key states */
  if(ioctl(fdser,TIOCMGET,&serstatus)!=-1)
  {
    if(sharedmem->invertpaddles)
    {
      dah_debounce_buf[debounce_buf_i]=
	(serstatus & TIOCM_DSR)?DSR_LINE_CLOSED_KEY:!DSR_LINE_CLOSED_KEY;
      dit_debounce_buf[debounce_buf_i]=
	(serstatus & TIOCM_CTS)?CTS_LINE_CLOSED_KEY:!CTS_LINE_CLOSED_KEY;
    }
    else
    {
      dit_debounce_buf[debounce_buf_i]=
	(serstatus & TIOCM_DSR)?DSR_LINE_CLOSED_KEY:!DSR_LINE_CLOSED_KEY;
      dah_debounce_buf[debounce_buf_i]=
	(serstatus & TIOCM_CTS)?CTS_LINE_CLOSED_KEY:!CTS_LINE_CLOSED_KEY;
    }

    debounce_buf_i++;
  }

  /* If the debounce buffer is full, determine the state of the keys */
  if(debounce_buf_i>=sharedmem->debounce)
  {
    debounce_buf_i=0;

    j=0;
    for(i=0;i<sharedmem->debounce;i++)
      if(dah_debounce_buf[i])
        j++;
    dah=(j>sharedmem->debounce/2)?1:0;

    j=0;
    for(i=0;i<sharedmem->debounce;i++)
      if(dit_debounce_buf[i])
        j++;
    dit=(j>sharedmem->debounce/2)?1:0;
  }

  return(cwirc_run_keyer(&is,dit,dah,sharedmem->wpm,sharedmem->iambicmode,
	sharedmem->do_midelementmodeB,sharedmem->do_ditmemory,
	sharedmem->do_dahmemory,sharedmem->do_autocharspacing,
	sharedmem->do_autowordspacing,sharedmem->dit_weight,ticklen));
}



/* Read the mouse buttons as a straight key */
static T_BOOL read_straight_key_mouse(void)
{
  return(sharedmem->mouseinputbutton0 || sharedmem->mouseinputbutton1);
}



/* Read the mouse buttons as an iambic key, emulate a straight key, then return
   the emulated key state */
static T_BOOL read_iambic_key_mouse(double ticklen)
{
  static int dah=0,dit=0;
  static struct cwirc_keyer_state is={0};

  if(sharedmem->invertpaddles)
  {
    dah=sharedmem->mouseinputbutton0;
    dit=sharedmem->mouseinputbutton1;
  }
  else
  {
    dit=sharedmem->mouseinputbutton0;
    dah=sharedmem->mouseinputbutton1;
  }

  return(cwirc_run_keyer(&is,dit,dah,sharedmem->wpm,sharedmem->iambicmode,
	sharedmem->do_midelementmodeB,sharedmem->do_ditmemory,
	sharedmem->do_dahmemory,sharedmem->do_autocharspacing,
	sharedmem->do_autowordspacing,sharedmem->dit_weight,ticklen));
}



/* Open the sound device. Return codes :
    NULL : no error
    error message */
char *open_snd_dev(char *snddev)
{
  int i;
  int little_endian=0;

  /* Do the endianness test here, so we don't have to force the user to change
     compilation options */
  i=1;
  if(((char *)&i)[0])
    little_endian=1;

  /* Open the sound device */
  if((fddsp=open(snddev,O_WRONLY | O_NONBLOCK))==-1)
  {
    sprintf(errmsg,"cannot open sound device %s",snddev);
    return(errmsg);
  }

  /* Remove the non-blocking flag */
  if(fcntl(fddsp,F_SETFL,0)==-1)
  {
    sprintf(errmsg,"cannot make sound device %s blocking",snddev);
    return(errmsg);
  }

  /* Configure the sound device */
  i=DSP_FRAG_SIZE+(DSP_FRAG_BUFFER<<16);
  if(ioctl(fddsp,SNDCTL_DSP_SETFRAGMENT,&i)<0)
  {
    close(fddsp);
    fddsp=-1;
    sprintf(errmsg,"cannot configure buffer in sound device %s",snddev);
    return(errmsg);
  }

  i=DSP_STEREO;
  if(ioctl(fddsp,SNDCTL_DSP_STEREO,&i)<0)
  {
    close(fddsp);
    fddsp=-1;
    sprintf(errmsg,"cannot configure sound device %s in %s.",snddev,
	i?"stereo":"mono");
    return(errmsg);
  }

  i=DSP_16BITS?(little_endian?AFMT_S16_LE:AFMT_S16_BE):AFMT_S8;
  if(ioctl(fddsp,SNDCTL_DSP_SETFMT,&i)<0)
  {
    close(fddsp);
    fddsp=-1;
    sprintf(errmsg,"cannot configure sound device %s in signed %d bits.",snddev,
	i==AFMT_S8?8:16);
    return(errmsg);
  }

  i=DSP_SAMPLE_FREQ;
  if(ioctl(fddsp,SNDCTL_DSP_SPEED,&i)<0)
  {
    close(fddsp);
    fddsp=-1;
    sprintf(errmsg,"cannot configure sound device %s at %dHz.",snddev,i);
    return(errmsg);
  }

  return(NULL);
}



/* Open the serial device and perform initial setup. */
char *open_serial_dev(char *serialdev)
{
  int serstatus;

  if((fdser=open(serialdev,O_WRONLY))==-1)
  {
    sprintf(errmsg,"cannot open serial device %s",serialdev);
    return(errmsg);
  }

  /* Ensure DTR is down, so we can read the key's contact(s) */
  if(ioctl(fdser,TIOCMGET,&serstatus)==-1)
  {
    close(fdser);
    fdser=-1;
    sprintf(errmsg,"cannot get serial device status");
    return(errmsg);
  }

  if(DTR_LINE_SET)
    serstatus&=~TIOCM_DTR;
  else
    serstatus|=TIOCM_DTR;
  
  if(ioctl(fdser,TIOCMSET,&serstatus)==-1)
  {
    close(fdser);
    fdser=-1;
    sprintf(errmsg,"cannot set serial device status");
    return(errmsg);
  }

  return(NULL);
}



/* Open and set /dev/rtc */
char *open_rtc_dev(void)
{
#ifdef LINUX
  /* Open the rtc device */
  if((fdrtc=open("/dev/rtc",O_RDONLY))==-1)
  {
    sprintf(errmsg,"cannot open /dev/rtc.");
    return(errmsg);
  }

  /* Configure the rtc for periodic interrupts */
  if(ioctl(fdrtc,RTC_IRQP_SET,RTC_RATE)==-1)
  {
    close(fdrtc);
    fdrtc=-1;
    sprintf(errmsg,"cannot set /dev/rtc to generate %d interupts per second.",
	RTC_RATE);
    return(errmsg);
  }

  return(NULL);
#else
  sprintf(errmsg,"operation without a sound device isn't implemented.");
  return(errmsg);
#endif
}



/* Enable periodic interrupts from /dev/rtc */
static int enable_rtc_interrupts(void)
{
#ifdef LINUX
  return(ioctl(fdrtc,RTC_PIE_ON,0)==-1?-1:0);
#else
  return(-1);
#endif
}



/* Close the sound device */
void close_snd_dev(void)
{
  /* Make sure the sound device is closed */
  if(fddsp!=-1)
  {
    close(fddsp);
    fddsp=-1;
  }
}



/* Close the serial device */
void close_serial_dev(void)
{
  /* Make sure the sound device is closed */
  if(fdser!=-1)
  {
    close(fdser);
    fdser=-1;
  }
}



/* Reset and close /dev/rtc */
void close_rtc_dev(void)
{
#ifdef LINUX
  /* Make sure the rtc device file is closed */
  if(fdrtc!=-1)
  {
    /* Disable the periodic interrupt */
    ioctl(fdrtc,RTC_PIE_OFF,0);

    close(fdrtc);
    fdrtc=-1;
  }
#endif
}



/* Set the external sounder line */
static int set_sounder_line(T_BOOL state)
{
  int serstatus;

  if(ioctl(fdser,TIOCMGET,&serstatus)==-1)
    return(-1);

  if(RTS_LINE_SET)
    serstatus=state?serstatus | TIOCM_RTS:serstatus & (~TIOCM_RTS);
  else
    serstatus=state?serstatus & (~TIOCM_RTS):serstatus | TIOCM_RTS;
  
  if(ioctl(fdser,TIOCMSET,&serstatus)==-1)
    return(-1);

  return(0);
}



/* Send an error message to the gui */
static void send_errmsg(char const *fmt,...)
{
  va_list ap;

  va_start(ap,fmt);

  /* Acquire the semaphore */
  if(!cwirc_sem_P(sharedmem->semid,SEM_IO_PROCESS_MSG))
  {
    vsprintf(sharedmem->io_process_msg,fmt,ap);

    /* Release the semaphore */
    cwirc_sem_V(sharedmem->semid,SEM_IO_PROCESS_MSG);
  }

  va_end(ap);
}



/* Clean up our stuff : close files, free memory ... */
static void cleanup(void)
{
  close_rtc_dev();
  close_snd_dev();
  close_serial_dev();
}

/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   CW sound generation routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "cwsound.h"
#include "sounder_down.h"
#include "sounder_up.h"



/* Definitions */
#define SOUNDER_UP_TO_DOWN_DELAY	10	/* ms */
#define SOUNDER_DOWN_TO_UP_DELAY	60	/* ms */
#define QRN_AMPLIFICATION		2
#define QRN_CRACKLING_DURATION		2	/* ms */
#define QRN_CRACKLING_OCCUR_PROB	.005
#define SQUELCH_TIMEOUT			3000	/* ms */



/* Generate a sound fragment that is "nbsamples" long at an audio frequency
   "freq", at an amplitude "amplitude" (between 0 and 100), in the form of a
    serie of samples betweem -1 and 1, at a sampling rate of "samplerate", at
    an offset of "offset_keyup" or "offset_keydown" sample ticks, corresponding
    to a morse key state that is either up or down. There are 2 types of sounds:

    0 --> beep : simple sine wave at "freq" Hz that's either present or absent
          depending on the state of the key.
    1 --> sounder clicks : 2 sounds, one is a sharp click when the key goes up
          and the other is a duller click when the key goes down. In this case,
          "freq" is ignored.

   Also, make sure the sound changes "softly" when the key changes state, so it
   doesn't make nasty clicks.

   Return code: 0 --> all samples are null, 1 --> sound was generated
*/
int generate_cw_sound_fragment(int sndtype,int keystate_prev,int keystate,
	int samplerate,int nbsamples,int freq,int amplitude,
	unsigned long long offset_keyup,unsigned long long offset_keydown,
	double *samplebuf)
{
  static T_BOOL firstcall=1;
  static double sinetable[1000];
  unsigned long long sine_i;
  int retcode;
  int i;

  retcode=0;
  switch(sndtype)
  {
  case 0:	/* beeping sound */
    if(keystate || keystate_prev)
    {
      /* Is it the first time we're called ? */
      if(firstcall)
      {
        /* Generate the sine table. The 1000 samples contain exactly one period.
           All sine values are pre-divided by 100 so we can multiply by the
           amplitude in the sample generation code blow without having to do an
           extra division. */
        for(i=0;i<1000;i++)
          sinetable[i]=sin(((double)i*M_PI*2)/1000)/100;
        firstcall=0;
      }

      /* Make sure the sound frequency is above 75Hz so it's always audible */
      if(freq<75)
        freq=75;

      /* Generate the samples */
      for(i=0;i<nbsamples;i++)
      {
        sine_i=((double)((offset_keydown+i)*freq)*1000)/(double)samplerate;
        if(sine_i>=0)
          samplebuf[i]=(double)amplitude*sinetable[sine_i%1000];
        else
          samplebuf[i]=(double)amplitude*-sinetable[(-sine_i)%1000];

        /* Make the beeps fade in and out so they don't generate nasty clicks */
        if(!keystate_prev)
          samplebuf[i]*=(double)i/(double)nbsamples;
        else if(!keystate)
          samplebuf[i]*=(double)(nbsamples-i)/(double)nbsamples;

        if(samplebuf[i]!=0)
          retcode=1;
      }
    }
    else
      for(i=0;i<nbsamples;i++)
        samplebuf[i]=0;
    break;

  case 1:	/* sounder clicking sound */
    if(keystate)
    {
      offset_keyup-=(samplerate*SOUNDER_UP_TO_DOWN_DELAY)/1000;
      for(i=0;i<nbsamples;i++)
        if(offset_keyup>=0 && offset_keydown+i<sounder_down_nbsamples)
        {
          samplebuf[i]=(double)amplitude*((double)sounder_down[offset_keydown+i]
			/32768)/100;

          if(samplebuf[i]!=0)
            retcode=1;
        }
        else
          samplebuf[i]=0;
    }
    else
    {
      offset_keyup-=(samplerate*SOUNDER_DOWN_TO_UP_DELAY)/1000;
      for(i=0;i<nbsamples;i++)
        if(offset_keyup>=0 && offset_keyup+i<sounder_up_nbsamples)
        {
          samplebuf[i]=(double)amplitude*((double)sounder_up[offset_keyup+i]
			/32768)/100;

          if(samplebuf[i]!=0)
            retcode=1;
        }
        else
          samplebuf[i]=0;
    }
    break;
  }

  return(retcode);
}



/* Generate a QRN sound fragment that is "nbsamples" long, at an amplitude
   "amplitude" (between 0 and 100), in the form of a serie of samples betweem
   -1 and 1. */
void generate_qrn_sound_fragment(int samplerate,int nbsamples,int amplitude,
				double *samplebuf)
{
  static int div;
  static double rndval=0,rndval2;
  static double sample=0;
  static int getrndval_cnt=0;
  static int cracklingcnt=0;
  int i;

  div=samplerate/5513;

  /* Generate crackling */
  rndval2=(double)rand()/RAND_MAX;
  if(rndval2<QRN_CRACKLING_OCCUR_PROB && !cracklingcnt)
    cracklingcnt=(samplerate*QRN_CRACKLING_DURATION)/1000;

  /* Generate fake pink noise, inserting cracklings of white noise when needed*/
  for(i=0;i<nbsamples;i++)
  {
    if(!getrndval_cnt)
      rndval=((((double)rand()*2)/RAND_MAX-1)*amplitude)/100;
    sample=((sample+rndval)/div)*QRN_AMPLIFICATION;
    if(sample>1)
      sample=1;
    else if(sample<-1)
      sample=-1;

    if(cracklingcnt)
    {
      cracklingcnt--;
      samplebuf[i]=(((double)rand()/RAND_MAX-1)*amplitude)/100;
    }
    else
      samplebuf[i]=sample;

    getrndval_cnt=(getrndval_cnt+1)%div;
  }
}



/* Squelch sounds under a certain threshold */
void squelch(int squelch_level,double *sndbuf,int nbsamples,double ticklen)
{
  static double squelch_timeout=0;
  static int fadecnt=0;
  int sample;
  int i;

  for(i=0;i<nbsamples;i++)
  {
    if(squelch_timeout<=0)
    {
      sample=sndbuf[i]*100;
      if(sample>squelch_level || sample<-squelch_level)
        squelch_timeout=SQUELCH_TIMEOUT;

      /* Squelch the sound, do a fadeout */
      fadecnt+=fadecnt<nbsamples?1:0;
    }
    else
      /* Unsquelch the sound, do a fadein */
      fadecnt-=fadecnt?1:0;

    sndbuf[i]*=((double)nbsamples-(double)fadecnt)/(double)nbsamples;
  }

  if(squelch_timeout>0)
    squelch_timeout-=ticklen;
}

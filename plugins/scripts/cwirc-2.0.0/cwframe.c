/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   CW frames encoding/decoding routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "cwframe.h"
#include "cwirc.h"
#include "grid.h"
#include "propagation.h"
#include "io.h"
#include "ipc.h"



/* Prototypes */
static void rot46_enc_dec(char *msg);
static int decoded_number_basefmt(char **buf);
static int decoded_number_xfmt(char **buf);
static char *encoded_number_basefmt(int number);
static char *encoded_number_xfmt(int number);



/* Encode a cw frame. A frame has the following format : 

[de=<encrypted callsign>,][at=<encrypted grid square,]<cw=|cx=><channel>
	<delay in ms><delay in ms><delay in ms> ...

- The channel is a positive integer. I suggest using 1000 a general CQ channel.

- Each delay is a positive or negative number. If it's positive, the sender's
  key is down (beeping). If it's negative, the key is up (silence). The absolute
  value of the number is the number of ms the event lasts

The channel is always encoded in base format (2-letter block of printable
characters, see below). If the format is the base format ("cw=" header),
subsequent event delay values are also encoded in base format. If the format is
the extended format ("cx=" header), subsequent event delay values are encoded
in extended format (1- or 3-letter blocks of printable characters, see below).

  Return value : NULL --> the frame wasn't encoded
                 pointer to the encoded frame
*/
char *cwirc_encode_cw_frame(void)
{
  static char cwframe[3+MAX_NICK_SIZE+1+3+MAX_GRIDSQUARE_SIZE+1+3+2+
			XMIT_BUF_MAX_SIZE*3+1];
  char encoded_callsign[MAX_NICK_SIZE];
  char encoded_gridsquare[MAX_GRIDSQUARE_SIZE];
  T_BOOL send_callsign;
  T_BOOL send_gridsquare;
  char enc_evts_basefmt[XMIT_BUF_MAX_SIZE*2+1];
  char enc_evts_xfmt[XMIT_BUF_MAX_SIZE*3+1];
  T_U8 evts_fmt;	/* 0 -> evts in base fmt, 1 --> evts in extended fmt */
  int i;

  if(!sharedmem->xmit_buf_flush_nb_evts)
    return(NULL);

  send_callsign=sharedmem->send_callsign_with_cw && sharedmem->callsign[0];
  send_gridsquare=sharedmem->send_gridsquare_with_cw &&sharedmem->gridsquare[0];

  if(send_callsign)
  {
    strcpy(encoded_callsign,sharedmem->callsign);
    rot46_enc_dec(encoded_callsign);
  }

  if(send_gridsquare)
  {
    strcpy(encoded_gridsquare,sharedmem->gridsquare);
    rot46_enc_dec(encoded_gridsquare);
  }

  /* Create a list of events encoded in base format */
  enc_evts_basefmt[0]=0;
  for(i=0;i<sharedmem->xmit_buf_flush_nb_evts;i++)
    strcat(enc_evts_basefmt,encoded_number_basefmt(sharedmem->xmit_buf[i]));

  /* Create a list of events encoded in extended format */
  enc_evts_xfmt[0]=0;
  for(i=0;i<sharedmem->xmit_buf_flush_nb_evts;i++)
    strcat(enc_evts_xfmt,encoded_number_xfmt(sharedmem->xmit_buf[i]));

  /* Use whatever format makes a shorter frame */
  evts_fmt=(strlen(enc_evts_xfmt)<strlen(enc_evts_basefmt))?1:0;

  sprintf(cwframe,"%s%s%s%s%s%s%s%s%s",
		send_callsign?EXPLICIT_CALLSIGN_HEADER:"",
		send_callsign?encoded_callsign:"",
		send_callsign?",":"",
		send_gridsquare?GRID_SQUARE_HEADER:"",
		send_gridsquare?encoded_gridsquare:"",
		send_gridsquare?",":"",
		evts_fmt==0?CW_FRAME_HEADER_BASEFMT:CW_FRAME_HEADER_XFMT,
		encoded_number_basefmt(sharedmem->cwchannel[sharedmem->
			currcwchannel]),
		evts_fmt==0?enc_evts_basefmt:enc_evts_xfmt);

  return(cwframe);
}



/* Decode a cw frame and insert it in the senders table. A frame has the
following format : 

[de=<encrypted callsign>,][at=<encrypted grid square,]<cw=|cx=><channel>
	<delay in ms><delay in ms><delay in ms> ...

- The channel is a positive integer.

- Each delay is a positive or negative number. If it's positive, the sender's
  key is down (beeping). If it's negative, the key is up (silence). The absolute
  value of the number is the number of ms the event lasts

The channel is always encoded in base format (2-letter block of printable
characters, see below). If the format is the base format ("cw=" header),
subsequent event delay values are also encoded in base format. If the format is
the extended format ("cx=" header), subsequent event delay values are encoded
in extended format (1- or 3-letter blocks of printable characters, see below).

  Return value : 0 --> frame is cw frame, but not on our cw chan, or we drop it
                 1 --> frame is a cw frame on our channel from a new sender,
                       and is decoded
                 2 --> frame is a cw frame on our channel from an already known
                       sender and is decoded

  If a callsign was found in the frame, callsign points to it. Otherwise,
  callsign is NULL.
*/
int cwirc_decode_cw_frame(char *sender_name,char *frame,char **callsign)
{
  static char decoded_callsign[MAX_NICK_SIZE];
  static char decoded_gridsquare[MAX_GRIDSQUARE_SIZE];
  int distance_from_receiver;		/* In Km */
  int i,j;
  char *ptr,*ptr2;
  int new_sender;
  T_U8 evts_fmt;	/* 0 -> evts in base fmt, 1 --> evts in extended fmt */

  new_sender=0;
  ptr=frame;

  /* Does the message start with the explicit callsign header ? */
  *callsign=NULL;
  if(!strncmp(ptr,EXPLICIT_CALLSIGN_HEADER,strlen(EXPLICIT_CALLSIGN_HEADER)))
  {
    /* Extract and decrypt the callsign */
    ptr+=strlen(EXPLICIT_CALLSIGN_HEADER);
    ptr2=strchr(ptr,',');
    i=ptr2-ptr;
    i=i>=MAX_NICK_SIZE?MAX_NICK_SIZE-1:i;
    strncpy(decoded_callsign,ptr,i);
    decoded_callsign[i]=0;
    rot46_enc_dec(decoded_callsign);
    if(decoded_callsign[0])
    {
      *callsign=decoded_callsign;
      sender_name=decoded_callsign;
    }
    ptr=ptr2+1;
  }

  /* Is there a grid square header next ? */
  decoded_gridsquare[0]=0;
  if(!strncmp(ptr,GRID_SQUARE_HEADER,strlen(GRID_SQUARE_HEADER)))
  {
    /* Extract and decrypt the grid square */
    ptr+=strlen(GRID_SQUARE_HEADER);
    ptr2=strchr(ptr,',');
    i=ptr2-ptr;
    i=i>=MAX_GRIDSQUARE_SIZE?MAX_GRIDSQUARE_SIZE-1:i;
    strncpy(decoded_gridsquare,ptr,i);
    decoded_gridsquare[i]=0;
    rot46_enc_dec(decoded_gridsquare);
    ptr=ptr2+1;
  }

  /* Are events in extended format ?*/
  if(ptr[1]=='w')
  {
    evts_fmt=0;
    ptr+=strlen(CW_FRAME_HEADER_BASEFMT);
  }
  else
  {
    evts_fmt=1;
    ptr+=strlen(CW_FRAME_HEADER_XFMT);
  }

  /* Is the sender on our channel ? */
  if(decoded_number_basefmt(&ptr)!=sharedmem->cwchannel[sharedmem->
	currcwchannel])
    return(0);	/* Not our channel : ignore the frame */

  /* Acquire the semaphore */
  if(!cwirc_sem_P(sharedmem->semid,SEM_ST))
  {
    /* Check if we already know the sender */
    for(i=0;i<MAX_SENDERS && strcmp(sender_name,sharedmem->sender[i].name);i++);

    /* If the sender isn't known, or is known but is currently being timed out
       for removal, treat it as a new sender */
    if(i==MAX_SENDERS || (sharedmem->sender[i].playback_stop_timeout>0 &&
			sharedmem->sender[i].playback_start_timeout<=0))
    {
      new_sender=1;

      /* Find a free slot if the sender is new */
      if(i==MAX_SENDERS)
      {
        for(i=0;i<MAX_SENDERS && sharedmem->sender[i].name[0];i++);

        if(i==MAX_SENDERS)	/* No free slot : */
          return(0);	/* just drop the frame */

        sharedmem->sender[i].playback_stop_timeout=0;
      }

      /* Initialize the slot for the sender */
      for(j=0;j<MAX_EVT_BUFFER;j++)
      {
        sharedmem->sender[i].kcdelay[j]=0;
        sharedmem->sender[i].keystate[j]=0;
      }
      sharedmem->sender[i].buf_head=0;
      sharedmem->sender[i].keyup_tickcnt=0;
      sharedmem->sender[i].keydown_tickcnt=0;
      sharedmem->sender[i].keystate_prev=0;
      strncpy(sharedmem->sender[i].name,sender_name,MAX_NICK_SIZE);
      sharedmem->sender[i].name[MAX_NICK_SIZE-1]=0;

      /* Give sender a chance to send more events before our buffer underruns */
      sharedmem->sender[i].playback_start_timeout=sharedmem->recv_buffering;
    }

    /* Append the frame events to the sender's ring buffer */
    j=sharedmem->sender[i].buf_head;
    do
    {
      if(sharedmem->sender[i].kcdelay[j]<=0)
      {
        sharedmem->sender[i].kcdelay[j]=evts_fmt==0?
		decoded_number_basefmt(&ptr):decoded_number_xfmt(&ptr);
        if(sharedmem->sender[i].kcdelay[j]>0)
          sharedmem->sender[i].keystate[j]=1;
        else
        {
          sharedmem->sender[i].keystate[j]=0;
          sharedmem->sender[i].kcdelay[j]=-sharedmem->sender[i].kcdelay[j];
        }
      }

      if((++j)==MAX_EVT_BUFFER)
        j=0;
    }
    while(j!=sharedmem->sender[i].buf_head && ptr[0]);

    /* If the sender has sent a grid square and ours is defined too, calculate
       how far the sender is from us and make up a signal strength*/
    if(sharedmem->gridsquare[0] && decoded_gridsquare[0])
    {
      distance_from_receiver=cwirc_great_circle_path(
				sharedmem->gridsquare,decoded_gridsquare);
      sharedmem->sender[i].signal_strength=cwirc_determine_signal_strength(
      				distance_from_receiver);
    }
    else
      sharedmem->sender[i].signal_strength=-1;
  
    /* Release the semaphore */
    cwirc_sem_V(sharedmem->semid,SEM_ST);
  }

  return(new_sender?1:2);
}



/* Check that a string looks like a valid cw frame */
int cwirc_is_cw_frame(char *frame)
{
  char *ptr,*ptr2;
  char buf[7];
  T_U8 evts_fmt;	/* 0 -> evts in base fmt, 1 --> evts in extended fmt */
  int i,j,k;

  ptr=frame;

  /* Is there an explicit header ? */
  if(!strncmp(ptr,EXPLICIT_CALLSIGN_HEADER,strlen(EXPLICIT_CALLSIGN_HEADER)))
  {
    /* Yes: can we find a ',' after the header ? */
    if((ptr=strchr(ptr,','))==NULL)
      return(0);	/* No ','. Drop the frame */
    else
      ptr++;
  }
  
  /* Is there a grid square header ? */
  if(!strncmp(ptr,GRID_SQUARE_HEADER,strlen(GRID_SQUARE_HEADER)))
  {
    /* Yes: can we find a ',' after the header ? */
    if((ptr2=strchr(ptr,','))==NULL)
      return(0);	/* No ','. Drop the frame */
    else
    {
      ptr+=strlen(GRID_SQUARE_HEADER);

      /* Check that the grid square is 4 or 6 characters long */
      if(ptr2-ptr!=4 && ptr2-ptr!=6)
        /* Invalid grid square. Drop the frame */
        return(0);

      /* Check that the grid square is valid */
      strncpy(buf,ptr,ptr2-ptr);
      buf[ptr2-ptr]=0;
      rot46_enc_dec(buf);
      if(!cwirc_is_grid_square(buf))
        /* Invalid grid square. Drop the frame */
        return(0);

      ptr=ptr2+1;
    }
  }

  /* Is there a morse frame header ? */
  if(strncmp(ptr,CW_FRAME_HEADER_BASEFMT,strlen(CW_FRAME_HEADER_BASEFMT)) &&
	strncmp(ptr,CW_FRAME_HEADER_XFMT,strlen(CW_FRAME_HEADER_XFMT)))
    return(0);	/* Header not found. Drop the frame */

  /* Are events in extended format ?*/
  if(ptr[1]=='w')
  {
    evts_fmt=0;
    ptr+=strlen(CW_FRAME_HEADER_BASEFMT);

    /* Are there are least 4 chars (channel number+one delay) and is
       the number of chars a multiple of 2 after the morse header ? */
    if(strlen(ptr)<4 || strlen(ptr)%2)
      return(0);	/* Not a valid frame length. Drop the frame */
  }
  else
  {
    evts_fmt=1;
    ptr+=strlen(CW_FRAME_HEADER_XFMT);

    /* Are there are least 3 chars (channel number+one delay) ? */
    if(strlen(ptr)<3)
      return(0);	/* Not a valid frame length. Drop the frame */
  }

  /* Are the characters only composed of printable characters between '!' (33)
     and '~' (126) included ? */
  for(i=0;i<strlen(ptr);i++)
    if(ptr[i]<'!' || ptr[i]>'~')
      return(0);/* Impossible character in an encoded delay. Drop the frame */

  /* Check that all the delays following the channel number have reasonable
     values individually, i.e. less than 1.5x our own xmit delay and not null,
     and the sum of all delays is less than 1.5x our own xmit delay.*/
  ptr+=2;
  k=0;
  while(k<XMIT_BUF_DELAY*1.5 && ptr[0])
  {
    j=evts_fmt==0?decoded_number_basefmt(&ptr):decoded_number_xfmt(&ptr);
    if(!j || (j<0?-j:j)>=XMIT_BUF_DELAY*1.5)
      return(0);	/* Suspicious delay. Drop the frame */
    k+=j<0?-j:j;
  }
  if(k>=XMIT_BUF_DELAY*1.5)
    return(0);		/* Suspicious sum of delays. Drop the frame */

  return(1);
}



/* Encrypt/decrypt a string with ROT46, using printable characters between
   '!' (33) and '}' (125) included, but excluding ','. Any character outside
   this set is silently discarded. This isn't much of an encryption, but it's
   good enough to scramble things so they're not too easily readable. */
static void rot46_enc_dec(char *msg)
{
  int i,j,k;
  unsigned char c;

  k=strlen(msg);

  /* Remove unwanted characters from the string */
  i=0;
  while(i<k)
  {
    if(msg[i]<'!' || msg[i]==',' || msg[i]>'}')
    {
      for(j=i;j<k;j++)
        msg[j]=msg[j+1];
      k--;
    }
    else
      i++;
  }

  /* ROT46 "encrypt"/"decrypt" the string */
  for(i=0;i<k;i++)
  {
    c=(msg[i]>=','?msg[i]-1:msg[i])+46;
    if(c>'}'-1)
      c='!'+c-'}';
    msg[i]=c>=','?c+1:c;
  }
}



/* Decode a signed number encoded into a string of 2 characters composed only of
   printable characters between '!' (33) and '~' (126) included, and do so in an
   endian-independant fashion.

   If an invalid character (including the string terminator) is encountered in
   the 2 characters needed, the function returns -32768 and *buf points to the
   offending character. If the call is successful, *buf points to the next
   character after the encoded number. */
static int decoded_number_basefmt(char **buf)
{
  unsigned char c1,c2;

  c1=(*buf)[0];
  if(c1<'!' || c1>'~')
    return(-32768);
  (*buf)++;

  c2=(*buf)[0];
  if(c2<'!' || c2>'~')
    return(-32768);
  (*buf)++;

  return(((c1-'!')*94 + (c2-'!'))-4418);
}



/* Decode a signed number encoded into a string of 1 or 3 characters composed
   only of printable characters between '!' (33) and '~' (126) included, and do
   so in an endian-independant fashion.

   The format is as follow:

   - If a character is between '!' and '~' excluded, it directly encodes the
     number.

   - If a character is '~', the 2 characters that follow encode the number in
     base format fashion (see above).

   If an invalid character (including the string terminator) is encountered in
   the 1 or 3 characters needed, or if the format is invalid, the function
   returns -32768 and *buf points to the offending character. If the call is
   successful, *buf points to the next character after the encoded number. */
static int decoded_number_xfmt(char **buf)
{
  unsigned char c1;

  c1=(*buf)[0];
  if(c1<'!' || c1>'~')
    return(-32768);
  (*buf)++;

  return(c1=='~'?decoded_number_basefmt(buf):(c1-'!')-46);
}



/* Encode a signed number into a string of 2 characters composed only of
   printable characters between '!' (33) and '~' (126) included, and do so in an
   endian-independant fashion */
static char *encoded_number_basefmt(int number)
{
  static char buf[3]={0,0,0};

  if(number<-4418) number=-4418;
  if(number>4417) number=4417;
  number+=4418;

  buf[0]='!'+number/94;
  buf[1]='!'+number%94;

  return(buf);
}



/* Encode a signed number into a string of 1, 2 or 3 characters composed only of
   printable characters between '!' (33) and '~' (126) included, and do so in an
   endian-independant fashion.

   The format is as follow:

   - If a number is between -46 and 46 included, it encodes into a single
     characters.

   - If a number is between -92 and -47 included, or between 47 and 92 included,
     it is encoded into 2 characters, which, decoded and summed up, reconstitute
     the number.

   - If a number is less than -92 or greater than 92, it is encoded into 3
     characters, the first one being '~' and the 2 others being the number
     encoded in base format (see above).
*/
static char *encoded_number_xfmt(int number)
{
  static char buf[4];

  if(number>=-46 && number<=46)
  {
    buf[0]='!'+number+46;
    buf[1]=0;
    return(buf);
  }

  if(number>=-92 && number<=92)
  {
    buf[0]=number<0?'!':'}';
    buf[1]='!'+(number<0?number+92:number);
    buf[2]=0;
    return(buf);
  }

  buf[0]='~';
  strcpy(buf+1,encoded_number_basefmt(number));
  return(buf);
}

/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   Morse decoder

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "cwdecoder.h"
#include "morsecodes.h"
#include "cwirc.h"



/* Definitions */
#define CW_BUF_MAX_SIZE	CW_SEQUENCE_MAX+2	/* Max. dits or dahs we buffer*/
#define DECODER_INERTIA	4			/* How slowly the decoder adapts
						   to a new keying speed */



/* Global variables */
struct cwcodeset cwirc_cw_table[NB_CW_CODE_SETS]=MORSE_CODES;



/* Prototypes */
static void decode_morse_code(T_BOOL key,double ticklen,int cwcodeset,
				T_BOOL reset_decoder);
static void decode_dot_code(T_BOOL key,double ticklen,int cwcodeset,
				T_BOOL reset_decoder);
static char *decode_cw_sequence(int cwcodeset,char *cwbuf,char *decoded_char);
static void insert_character_in_decoder_buffer(char *c,T_BOOL delete_spaces);



/* Functions */
void cwirc_decode_cw(T_BOOL key,double ticklen,int cwcodeset)
{
  /* Is the decoder disabled ? */
  if(cwirc_cw_table[sharedmem->cwcodeset].cwcodetype==NOCODE)
    if(sharedmem->decoded_msg_wpm!=WPM_DECODER_DISABLED)
      sharedmem->reset_decoder=1;
    else
      return;
  else
    if(sharedmem->decoded_msg_wpm==WPM_DECODER_DISABLED)
      sharedmem->decoded_msg_wpm=WPM_UNKNOWN_WPM;
  
  /* Have we been asked to reset the decoder ? */
  if(sharedmem->reset_decoder)
  {
    decode_morse_code(key,ticklen,cwcodeset,1);
    decode_dot_code(key,ticklen,cwcodeset,1);
    sharedmem->decoded_msg_buf[0]=0;
    sharedmem->decoded_msg_buf_char_markers[0]=0;
    sharedmem->decoded_msg_wpm=cwirc_cw_table[sharedmem->cwcodeset].cwcodetype
				==NOCODE?WPM_DECODER_DISABLED:WPM_UNKNOWN_WPM;
    sharedmem->decoded_msg_updated=1;
    sharedmem->reset_decoder=0;
  }

  /* Is the chosen code set in Morse code ? */
  if(cwirc_cw_table[cwcodeset].cwcodetype==MORSE)
  {
    decode_morse_code(key,ticklen,cwcodeset,0);
    return;
  }

  /* Is the chosen code set in DOT code ? */
  if(cwirc_cw_table[cwcodeset].cwcodetype==DOT)
  {
    decode_dot_code(key,ticklen,cwcodeset,0);
    return;
  }
}



/* Decode international Morse code */
void decode_morse_code(T_BOOL key,double ticklen,int cwcodeset,
			T_BOOL reset_decoder)
{
  static double evtlen=0;
  static double beeplenprev=0,lastshortbeeplen=0,lastlongbeeplen=0;
  static double lastbeep_too_short_by=0;	/* in ms */
  static double ditlen=0;
  static T_BOOL keyprev=0;
  static char cwbuf[CW_BUF_MAX_SIZE]="";
  static T_BOOL wait_for_end_of_word=0;
  static char newbeep=0;
  char decoded_char[CW_SYMBOL_MAX+1]="";
  int i;

  /* Should we reset the decoder's internal states? */
  if(reset_decoder)
  {
    evtlen=0;
    beeplenprev=0;
    lastshortbeeplen=0;
    lastlongbeeplen=0;
    lastbeep_too_short_by=0;
    ditlen=0;
    keyprev=0;
    cwbuf[0]=0;
    wait_for_end_of_word=0;
    newbeep=0;
    decoded_char[0]=0;

    return;
  }

  /* Did the key change state ? */
  if(key!=keyprev)
  {
    wait_for_end_of_word=0;
    
    if(!key)	/* Was the key released ? */
    {
      if(evtlen<beeplenprev/2)
        newbeep='.';
      else if(evtlen>beeplenprev*2)
        newbeep='-';

      if((i=strlen(cwbuf))<CW_BUF_MAX_SIZE-1)
      {
        cwbuf[i]=newbeep;
        cwbuf[i+1]=0;
      }

      if(newbeep=='.')
        lastshortbeeplen=evtlen;
      else
        lastlongbeeplen=evtlen;

      /* Progressively deviate the "official" dit length toward the last short
         beep length, so the decoder isn't thrown off track immediately when the
         operator made a temporary timing mistake */
      if(lastlongbeeplen!=0 && lastshortbeeplen!=0)
        ditlen=(ditlen*(DECODER_INERTIA-1)+
		((3*lastshortbeeplen+lastlongbeeplen)/6))/DECODER_INERTIA;
      else
        if(lastshortbeeplen!=0)
          ditlen=(ditlen*(DECODER_INERTIA-1)+lastshortbeeplen)/DECODER_INERTIA;
        else
          ditlen=(ditlen*(DECODER_INERTIA-1)+lastlongbeeplen/3)/DECODER_INERTIA;

      if(newbeep=='.')
        lastbeep_too_short_by=ditlen>evtlen?ditlen-evtlen:0;
      else
        lastbeep_too_short_by=ditlen*3>evtlen?ditlen*3-evtlen:0;

      beeplenprev=evtlen;
    }
    evtlen=0;
  }

  /* If the key has been released for over a dit length * 2, decode the
     character. Allow for timing imprecision if the last beep was too short */
  if(!key && !keyprev && (evtlen-lastbeep_too_short_by)>ditlen*2 && cwbuf[0])
  {
    /* Decode the cw character */
    decode_cw_sequence(cwcodeset,cwbuf,decoded_char);

    /* Empty the buffer */
    cwbuf[0]=0;

    wait_for_end_of_word=1;
  }

  /* If the key has been released for over a dit length * 4, send a space.
     Allow for timing imprecision if the last beep was too short */
  if((evtlen-lastbeep_too_short_by)>ditlen*4 && wait_for_end_of_word)
  {
    strcpy(decoded_char," ");
    wait_for_end_of_word=0;
  }

  /* Is there a new decoded character ? */
  if(decoded_char[0])
  {
    /* Add the character in the buffer, or remove the last character */
    insert_character_in_decoder_buffer(decoded_char,1);
    decoded_char[0]=0;

    /* Report the current decoder's speed */
    if(ditlen>0)
      sharedmem->decoded_msg_wpm=1200/ditlen;
    else
      sharedmem->decoded_msg_wpm=WPM_UNKNOWN_WPM;

    /* Assert this for the display routine */
    sharedmem->decoded_msg_updated=1;
  }

  keyprev=key;
  evtlen+=ticklen;
}



/* Decode DOT code */
void decode_dot_code(T_BOOL key,double ticklen,int cwcodeset,
			T_BOOL reset_decoder)
{
  static double evtlen=0;
  static double ditlen=0;
  static T_BOOL keyprev=0;
  static int dotcount=0;
  static char cwbuf[CW_BUF_MAX_SIZE]="";
  char decoded_char[CW_SYMBOL_MAX+1]="";
  int i;

  /* Should we reset the decoder's internal states? */
  if(reset_decoder)
  {
    evtlen=0;
    ditlen=0;
    keyprev=0;
    dotcount=0;
    cwbuf[0]=0;
    decoded_char[0]=0;

    return;
  }

  /* Did the key change state ? */
  if(key!=keyprev)
  {
    if(!key)	/* Was the key released ? */
    {
      if(dotcount<9)
        dotcount++;	/* Count dots in the current character element */

      /* Progressively deviate the "official" dit length toward the last short
         beep length, so the decoder isn't thrown off track immediately when the
         operator made a temporary timing mistake */
      ditlen=(ditlen*(DECODER_INERTIA-1)+evtlen)/DECODER_INERTIA;
    }
    evtlen=0;
  }

  /* If the key has been released for over a dit length * 2 and there were dots
     counted, add the current character element to the buffer. */
  if(!key && !keyprev && evtlen>ditlen*2 && dotcount &&
	(i=strlen(cwbuf))<CW_BUF_MAX_SIZE-1)
  {
    cwbuf[i]='0'+dotcount;
    cwbuf[i+1]=0;
    dotcount=0;
  }

  /* If the key has been released for over a dit length * 5 and there are
     character elements to decode, decode the character */
  if(evtlen>ditlen*5 && cwbuf[0])
  {
    /* Decode the cw character */
    decode_cw_sequence(cwcodeset,cwbuf,decoded_char);

    /* Empty the buffer */
    cwbuf[0]=0;
  }

  /* Is there a new decoded character ? */
  if(decoded_char[0])
  {
    /* Add the character in the buffer, or remove the last character */
    insert_character_in_decoder_buffer(decoded_char,0);
    decoded_char[0]=0;

    /* Report the current decoder's speed */
    if(ditlen>0)
      sharedmem->decoded_msg_wpm=600/ditlen;	/* Calculated on the basis of
      						   100 dits per PARIS word in
      						   DOT code */
    else
      sharedmem->decoded_msg_wpm=WPM_UNKNOWN_WPM;

    /* Assert this for the display routine */
    sharedmem->decoded_msg_updated=1;
  }

  keyprev=key;
  evtlen+=ticklen;
}



/* Find a CW sequence in the right table and return it */
static char *decode_cw_sequence(int cwcodeset,char *cwbuf,char *decoded_char)
{
  int i;
  
  /* Look for a matching sequence */
  for(i=0;cwirc_cw_table[cwcodeset].cwcode[i].sequence[0] &&
	strcmp(cwbuf,cwirc_cw_table[cwcodeset].cwcode[i].sequence);i++);

  /* Did we find a sequence ? */
  if(cwirc_cw_table[cwcodeset].cwcode[i].sequence[0])
    strcpy(decoded_char,cwirc_cw_table[cwcodeset].cwcode[i].symbol);
  else
  {
    /* Is it a continuous stream of dits ? */
    if(strchr(cwbuf,'-')==NULL)
      strcpy(decoded_char,"\b");	/* It's an "error" morse code */
    else
      strcpy(decoded_char,UNKNOWN_CHARACTER_SIGN);/* We don't know what it is */
  }

  return(decoded_char);
}



/* Insert a decoded character in the decoder buffer, or remove the last one
   if the character is a backspace */
static void insert_character_in_decoder_buffer(char *c,T_BOOL delete_spaces)
{
  int i,j,k;
  
  k=strlen(sharedmem->decoded_msg_buf);

  /* Is the character a BACKSPACE ? */
  if(c[0]=='\b')
  {
    if(delete_spaces)
    {
      /* Zap the last spaces in the decoded message line */
      while(k && sharedmem->decoded_msg_buf[k-1]==' ')
      {
        sharedmem->decoded_msg_buf[--k]=0;
        sharedmem->decoded_msg_buf_char_markers[k]=0;
      }
    }

    /* Zap the last character in the decoded message line */
    while(k && sharedmem->decoded_msg_buf_char_markers[k-1]==' ')
    {
      sharedmem->decoded_msg_buf[--k]=0;
      sharedmem->decoded_msg_buf_char_markers[k]=0;
    }
    if(k)
    {
      sharedmem->decoded_msg_buf[--k]=0;
      sharedmem->decoded_msg_buf_char_markers[k]=0;
    }
  }
  else
  {
    /* Do we need to make room in the decoded message buffer for the new
       character ? */
    j=strlen(c);
    if(j+k>=DECODED_MSG_SIZE)
    {
      /* Find out the position of the character that'll become the new
         leading character of the decoded message buffer */
      for(i=j+k+1-DECODED_MSG_SIZE;
		sharedmem->decoded_msg_buf_char_markers[i]==' ';i++);

      /* Shift the decoded message left accordingly */
      for(k=i;sharedmem->decoded_msg_buf[k];k++)
      {
        sharedmem->decoded_msg_buf[k-i]=sharedmem->decoded_msg_buf[k];
        sharedmem->decoded_msg_buf_char_markers[k-i]=
		sharedmem->decoded_msg_buf_char_markers[k];
      }
      sharedmem->decoded_msg_buf[k-i]=0;
      sharedmem->decoded_msg_buf_char_markers[k-i]=0;
    }

    /* Append the new character in the buffer */
    strcat(sharedmem->decoded_msg_buf,c);

    /* Append the new character's start marker in the markers buffer */
    for(i=0;i<j;i++)
      strcat(sharedmem->decoded_msg_buf_char_markers,i?" ":"|");
  }
}

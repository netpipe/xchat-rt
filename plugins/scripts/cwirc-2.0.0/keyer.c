/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   Iambic keyer implementation

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include "types.h"
#include "keyer.h"



/* Definitions */
#define NO_TIMEOUTS_SCHED	-2
#define NO_ELEMENT		-1
#define DIT			0
#define DAH			1
#define MODE_A			0
#define MODE_B			1
#define NO_PADDLE_SQUEEZE	0
#define PADDLES_SQUEEZED	1
#define PADDLES_RELEASED	2
#define NO_DELAY		0
#define CHAR_SPACING_DELAY	1
#define WORD_SPACING_DELAY	2



/* Implement the keyer. Ticklen is the time interval in ms between 2 calls to
   the function. Return the state of the keyer-generated "straight key" */
T_BOOL cwirc_run_keyer(struct cwirc_keyer_state *is,T_BOOL dit,T_BOOL dah,
	int wpm,int iambicmode,T_BOOL midelementmodeB,T_BOOL ditmemory,
	T_BOOL dahmemory,T_BOOL autocharspacing,T_BOOL autowordspacing,
	int weight,double ticklen)
{
  double ditlen=1200/(double)wpm;
  int set_element_timeouts=NO_TIMEOUTS_SCHED;

  /* Do we need to initialize the keyer ? */
  if(!is->keyer_initialized)
  {
    is->prev_dit=dit;
    is->prev_dah=dah;
    is->last_element=is->current_element=NO_ELEMENT;
    is->iambic_in_element=NO_PADDLE_SQUEEZE;
    is->paddles_squeezed_after_mid_element=0;
    is->insert_inverted_element=0;
    is->mid_element_timeout=is->beep_timeout=is->element_timeout=0;
    is->delay_timeout=0;
    is->delay_type=NO_DELAY;
    is->keyer_initialized=1;
  }

  /* Decrement the timeouts */
  is->delay_timeout-=is->delay_timeout>0?ticklen:0;
  if(is->delay_timeout<=0)
  {
    /* If nothing is scheduled to play, and we just did a character spacing
       delay, and we do auto word spacing, wait for a word spacing delay,
       otherwise resume the normal element timeout countdowns */
    if(is->element_timeout<=0 && is->delay_type==CHAR_SPACING_DELAY &&
	autowordspacing)
    {
      is->delay_timeout=ditlen*4;
      is->delay_type=WORD_SPACING_DELAY;
    }
    else
    {
      is->delay_type=NO_DELAY;
      is->mid_element_timeout-=is->mid_element_timeout>0?ticklen:0;
      is->beep_timeout-=is->beep_timeout>0?ticklen:0;
      is->element_timeout-=is->element_timeout>0?ticklen:0;
    }
  }

  /* Are both paddles squeezed ? */
  if(dit && dah)
  {
    is->iambic_in_element=PADDLES_SQUEEZED;

    /* Are the paddles squeezed past the middle of the element ? */
    if(is->mid_element_timeout<=0)
      is->paddles_squeezed_after_mid_element=1;
  }
  else
    /* Are both paddles released and we had gotten a squeeze in this element ?*/
    if(!dit && !dah && is->iambic_in_element==PADDLES_SQUEEZED)
      is->iambic_in_element=PADDLES_RELEASED;

  /* Is the current element finished ? */
  if(is->element_timeout<=0 && is->current_element!=NO_ELEMENT)
  {
    is->last_element=is->current_element;
    
    /* Should we insert an inverted element ? */
    if(((dit && dah) ||
	(is->insert_inverted_element && is->iambic_in_element!=
							PADDLES_RELEASED) ||
	(is->iambic_in_element==PADDLES_RELEASED && iambicmode==MODE_B &&
	(!midelementmodeB || is->paddles_squeezed_after_mid_element)) ) )
    {
      if(is->last_element==DAH)
        set_element_timeouts=is->current_element=DIT;
       else
        set_element_timeouts=is->current_element=DAH;
    }
    else
    {
      /* No more element */
      is->current_element=NO_ELEMENT;

      /* Do we do automatic character spacing ? */
      if(autocharspacing && !dit && !dah)
      {
        is->delay_timeout=ditlen*2;
        is->delay_type=CHAR_SPACING_DELAY;
      }
    }

    is->insert_inverted_element=0;
    is->iambic_in_element=NO_PADDLE_SQUEEZE;
    is->paddles_squeezed_after_mid_element=0;
  }

  /* Is an element currently being played ? */
  if(is->current_element==NO_ELEMENT)
  {
    if(dah)			/* Dah paddle down ? */
      set_element_timeouts=is->current_element=DAH;
    else if(dit)		/* Dit paddle down ? */
      set_element_timeouts=is->current_element=DIT;
  }

  /* Do the dah memory */
  if(is->current_element==DIT && !is->prev_dah && dah && dahmemory)
    is->insert_inverted_element=1;
      
  /* Do the dit memory */
  if(is->current_element==DAH && !is->prev_dit && dit && ditmemory)
    is->insert_inverted_element=1;
      
  /* If we had a dit (or dah) scheduled to be played after a delay, and the
     operator lifted both paddles before the end of the delay, and we have no
     dit (or dah) memory, forget it */
  if(is->delay_timeout>0 && !dit && !dah && (
		(is->current_element==DIT && !ditmemory) ||
		(is->current_element==DAH && !dahmemory)
	))
    set_element_timeouts=is->current_element=NO_ELEMENT;

  /* Do we need to set the playing timeouts of an element? */
  switch(set_element_timeouts)
  {
  case NO_ELEMENT:		/* Cancel any dit or dah */
    is->beep_timeout=0;
    is->mid_element_timeout=0;
    is->element_timeout=0;
    break;

  case DIT:			/* Schedule a dit ? */
    is->beep_timeout=(ditlen*(double)weight)/50;
    is->mid_element_timeout=is->beep_timeout/2;
    is->element_timeout=ditlen*2;
    break;

  case DAH:			/* Schedule a dah ? */
    is->beep_timeout=(ditlen*(double)weight)/50 + ditlen*2;
    is->mid_element_timeout=is->beep_timeout/2;
    is->element_timeout=ditlen*4;
    break;
  }

  is->prev_dit=dit;
  is->prev_dah=dah;

  return(is->beep_timeout>0 && is->delay_timeout<=0?1:0);
}

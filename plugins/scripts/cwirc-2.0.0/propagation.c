/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   Simulated propagation / signal strength evaluation routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdlib.h>

#include "propagation.h"



/* Definitions */
#define HALF_SIGNAL_DROP_DISTANCE	200	/* km */
#define SPORADICE_UPDATE_PERIOD		500	/* ms */



/* Make up a signal strength (0 -> 100) from the distance in Km between sender
   and receiver. The current simulation doesn't correspond to anything real but
   allows us to not really exclude anybody anywhere in the world from being
   received. */
double cwirc_determine_signal_strength(int distance)
{
  return(1/(1+((double)distance/HALF_SIGNAL_DROP_DISTANCE)));
}



/* Simulate sporadic-E. Here, we do it solely by attenuating the signal at
   random */
void cwirc_simulate_sporadicE(double *signal_strength,double ticklen)
{
  static double rnd_update_timeout=0;
  static double dir=1;
  static double attn=1;

  if(rnd_update_timeout<=0)
  {
    dir=rand()>RAND_MAX/2?ticklen/300:0;
    rnd_update_timeout=SPORADICE_UPDATE_PERIOD;
  }
  rnd_update_timeout-=ticklen;

  attn=(attn+dir)/(1+ticklen/300);

  if(*signal_strength<0.2)
    *signal_strength*=attn;
}

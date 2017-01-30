/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   Grid squares distance routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "grid.h"



/* Definitions */
#define EARTH_RADIUS	6367		/* km, average */



/* Prototypes */
static void gridsquare_to_latlon(char *gs,double *lat,double *lon);



/* Check if a string is a valid 4- or 6-character grid square */
int cwirc_is_grid_square(char *gs)
{
  int i;
  
  /* Test the length of the string */
  i=strlen(gs);
  if(i!=4 && i!=6)
    return(0);

  /* Test its format */
  if(toupper(gs[0])<'A' || toupper(gs[0])>'R' ||
	toupper(gs[1])<'A' || toupper(gs[1])>'R' ||
	!isdigit(gs[2]) ||
	!isdigit(gs[3]) ||
	(i==6 && (toupper(gs[4])<'A' || toupper(gs[4])>'X' ||
		toupper(gs[5])<'A' || toupper(gs[5])>'X')))
    return(0);

  return(1);
}



/* Calculate the great circle path between 2 grid squares' centers in Km */
int cwirc_great_circle_path(char *gs1,char *gs2)
{
  double lat1,lon1;
  double lat2,lon2;
  double a,d;

  /* Convert the grid squares to latitude/longitude */
  gridsquare_to_latlon(gs1,&lat1,&lon1);
  gridsquare_to_latlon(gs2,&lat2,&lon2);

  /* Calculate the great circle path */
  a=pow(sin((lat2-lat1)/2),2) + cos(lat1)*cos(lat2)*pow(sin((lon2-lon1)/2),2);
  d=EARTH_RADIUS * 2*atan2(sqrt(a),sqrt(1-a));

  return(d);
}



/* Calculate the latitude/longitude (in rads) of the center of a grid square */
static void gridsquare_to_latlon(char *gs,double *lat,double *lon)
{
  /* Calculate the base coordinates of the corner of the grid square */
  *lon=-M_PI   +(toupper(gs[0])-'A')*((M_PI/180)*20)+(gs[2]-'0')*((M_PI/180)*2);
  *lat=-M_PI/2 +(toupper(gs[1])-'A')*((M_PI/180)*10)+(gs[3]-'0')*((M_PI/180)*1);

  /* Is it a short (4 character) or long (6 character) grid square ? */
  if(strlen(gs)==4)
  {
    /* Calculate the center of the grid square */
    *lon+=(M_PI/180)*1;
    *lat+=(M_PI/180)*.5;
  }
  else
  {
    /* Increase precision of the coordinates of the corner of the grid square */
    *lon+=(toupper(gs[4])-'A')*(((M_PI/180)/60)*5);
    *lat+=(toupper(gs[5])-'A')*(((M_PI/180)/60)*2.5);

    /* Calculate the center of the grid square */
    *lon+=((M_PI/180)/60)*2.5;
    *lat+=((M_PI/180)/60)*1.25;
  }
}

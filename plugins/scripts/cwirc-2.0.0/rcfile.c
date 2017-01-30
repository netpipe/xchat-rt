/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   Configuration file reading, parsing and checking routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "types.h"
#include "rcfile.h"
#include "cwirc.h"
#include "grid.h"
#include "io.h"
#include "cwdecoder.h"



/* Definitions */
#define LINE_MAX_SIZE				256
#define DEFAULT_CWINPUT				"mouse"
#define DEFAULT_CWOUTPUT			"soundcard"
#define DEFAULT_CWSOUND				"beeps"
#ifdef LINUX
#define DEFAULT_SNDDEV				"/dev/dsp"
#define DEFAULT_SERIALDEV			"/dev/ttyS0"
#endif
#ifdef FREEBSD
#define DEFAULT_SNDDEV				"/dev/dsp0.0"
#define DEFAULT_SERIALDEV			"/dev/cuaa0"
#endif
#ifdef NETBSD
#define DEFAULT_SNDDEV				"/dev/audio0"
#define DEFAULT_SERIALDEV			"/dev/tty00"
#endif
#define DEFAULT_KEYTYPE				"straight"
#define DEFAULT_IAMBICMODE			"B"
#define DEFAULT_MIDELEMENTMODEB			0
#define DEFAULT_DITMEMORY			1
#define DEFAULT_DAHMEMORY			1
#define DEFAULT_AUTOCHARSPACING			0
#define DEFAULT_AUTOWORDSPACING			0
#define DEFAULT_INVERTPADDLES			0
#define DEFAULT_DITWEIGHT			50
#define DEFAULT_SIDETONE_MODE			0
#define DEFAULT_CWCHANNEL1			1000
#define DEFAULT_CWCHANNEL2			1000
#define DEFAULT_CWCHANNEL3			1000
#define DEFAULT_CWCHANNEL4			1000
#define DEFAULT_CWCHANNEL5			1000
#define DEFAULT_CURRENTCWCHANNEL		1
#define DEFAULT_CWRXPITCH			0
#define DEFAULT_CWTXPITCH			0
#define DEFAULT_SQUELCH				0
#define DEFAULT_VOLUME				100
#define DEFAULT_WPM				10
#define DEFAULT_DEBOUNCE			1
#define DEFAULT_RECV_BUFFERING			1000
#define DEFAULT_CALLSIGN			""
#define DEFAULT_GRIDSQUARE			""
#define DEFAULT_SEND_CALLSIGN_WITH_CW		0
#define DEFAULT_SEND_GRIDSQUARE_WITH_CW		0
#define DEFAULT_REPLY_TO_CTCP			1
#define DEFAULT_GIVE_CALLSIGN_IN_CTCP_REPLY	0
#define DEFAULT_GIVE_GRIDSQUARE_IN_CTCP_REPLY	0
#define DEFAULT_GIVE_CWCHANNEL_IN_CTCP_REPLY	1
#define DEFAULT_SIMULATE_QRN			0
#define DEFAULT_QRNLEVEL			10
#define DEFAULT_SIMULATE_SIGNAL_STRENGTH	0
#define DEFAULT_SIMULATE_SPORADICE		0
#define DEFAULT_DEFAULT_SIGNAL_STRENGTH		50
#define DEFAULT_CW_DECODER_LANGUAGE		"english"



/* Global variables */
static char errmsg[LINE_MAX_SIZE];
static char cwinput[LINE_MAX_SIZE];
static char cwoutput[LINE_MAX_SIZE];
static char cwsound[LINE_MAX_SIZE];
static char snddev[LINE_MAX_SIZE];
static char serialdev[LINE_MAX_SIZE];
static char keytype[LINE_MAX_SIZE];
static char iambicmode[LINE_MAX_SIZE];
static int midelementmodeB;
static int ditmemory;
static int dahmemory;
static int autocharspacing;
static int autowordspacing;
static int invertpaddles;
static int ditweight;
static int sidetone_mode;
static int cwrxpitch;
static int cwtxpitch;
static int squelch;
static int volume;
static int cwchannel[5];
static int currentcwchannel;
static int wpm;
static int debounce;
static int recv_buffering;
static char callsign[LINE_MAX_SIZE];
static char gridsquare[LINE_MAX_SIZE];
static int send_callsign_with_cw;
static int send_gridsquare_with_cw;
static int reply_to_ctcp;
static int give_callsign_in_ctcp_reply;
static int give_gridsquare_in_ctcp_reply;
static int give_cwchannel_in_ctcp_reply;
static int simulate_qrn;
static int qrnlevel;
static int simulate_signal_strength;
static int simulate_sporadicE;
static int default_signal_strength;
static char cw_decoder_language[LINE_MAX_SIZE];



/* Prototypes */
void restore_default(void);
static char *check_parameters(void);
void load_internal_vars_from_rcfile_values(void);



/* Load default values into the variables */
void restore_default_settings(void)
{
  strcpy(cwinput,DEFAULT_CWINPUT);
  strcpy(cwoutput,DEFAULT_CWOUTPUT);
  strcpy(cwsound,DEFAULT_CWSOUND);
  strcpy(snddev,DEFAULT_SNDDEV);
  strcpy(serialdev,DEFAULT_SERIALDEV);
  strcpy(keytype,DEFAULT_KEYTYPE);
  strcpy(iambicmode,DEFAULT_IAMBICMODE);
  midelementmodeB=DEFAULT_MIDELEMENTMODEB;
  ditmemory=DEFAULT_DITMEMORY;
  dahmemory=DEFAULT_DAHMEMORY;
  autocharspacing=DEFAULT_AUTOCHARSPACING;
  autowordspacing=DEFAULT_AUTOWORDSPACING;
  invertpaddles=DEFAULT_INVERTPADDLES;
  ditweight=DEFAULT_DITWEIGHT;
  sidetone_mode=DEFAULT_SIDETONE_MODE;
  cwrxpitch=DEFAULT_CWRXPITCH;
  cwtxpitch=DEFAULT_CWTXPITCH;
  squelch=DEFAULT_SQUELCH;
  volume=DEFAULT_VOLUME;
  cwchannel[0]=DEFAULT_CWCHANNEL1;
  cwchannel[1]=DEFAULT_CWCHANNEL2;
  cwchannel[2]=DEFAULT_CWCHANNEL3;
  cwchannel[3]=DEFAULT_CWCHANNEL4;
  cwchannel[4]=DEFAULT_CWCHANNEL5;
  currentcwchannel=DEFAULT_CURRENTCWCHANNEL;
  wpm=DEFAULT_WPM;
  debounce=DEFAULT_DEBOUNCE;
  recv_buffering=DEFAULT_RECV_BUFFERING;
  strcpy(callsign,DEFAULT_CALLSIGN);
  strcpy(gridsquare,DEFAULT_GRIDSQUARE);
  send_callsign_with_cw=DEFAULT_SEND_CALLSIGN_WITH_CW;
  send_gridsquare_with_cw=DEFAULT_SEND_GRIDSQUARE_WITH_CW;
  reply_to_ctcp=DEFAULT_REPLY_TO_CTCP;
  give_callsign_in_ctcp_reply=DEFAULT_GIVE_CALLSIGN_IN_CTCP_REPLY;
  give_gridsquare_in_ctcp_reply=DEFAULT_GIVE_GRIDSQUARE_IN_CTCP_REPLY;
  give_cwchannel_in_ctcp_reply=DEFAULT_GIVE_CWCHANNEL_IN_CTCP_REPLY;
  simulate_qrn=DEFAULT_SIMULATE_QRN;
  qrnlevel=DEFAULT_QRNLEVEL;
  simulate_signal_strength=DEFAULT_SIMULATE_SIGNAL_STRENGTH;
  simulate_sporadicE=DEFAULT_SIMULATE_SPORADICE;
  default_signal_strength=DEFAULT_DEFAULT_SIGNAL_STRENGTH;
  strcpy(cw_decoder_language,DEFAULT_CW_DECODER_LANGUAGE);

  load_internal_vars_from_rcfile_values();
}



/* Read the config file and set options accordingly. The config files is
   composed of "var=val" lines. Return codes :
   NULL --> no error
   error message
*/
char *cwirc_parse_rcfile(char *rcfile)
{
  char filename[FILENAME_MAX];
  char *homedir;
  FILE *fd;
  char buf[LINE_MAX_SIZE];
  char *var,*val;
  char *strptr;
  int *intptr;
  int lineno;
  int i,j;
  
  strncpy(filename,rcfile,FILENAME_MAX);
  filename[FILENAME_MAX-1]=0;

  /* Expand tildas by the HOME variable */
  homedir=getenv("HOME");
  for(i=0;i<strlen(filename);i++)
    if(filename[i]=='~')
    {
      for(j=strlen(filename);j>i;j--)
        filename[j+strlen(homedir)-1]=filename[j];
      for(j=0;j<strlen(homedir);j++)
        filename[i+j]=homedir[j];
    }

  /* Restore the default values */
  restore_default_settings();

  /* Open the config file for reading */
  if((fd=fopen(filename,"r"))==NULL)
  {
    /* Was the file non-existant ? */
    if(errno==ENOENT)
    {
      /* Try to create the file first */
      if(cwirc_save_rcfile(rcfile)!=NULL)
        return(errmsg);

      /* Now reopen it */
      if((fd=fopen(filename,"r"))==NULL)
      {
        sprintf(errmsg,"cannot open %s : %s",rcfile,strerror(errno));
        return(errmsg);
      }
    }
    else
    {
      /* Some other open error. We give up. */
      sprintf(errmsg,"cannot open %s : %s",rcfile,strerror(errno));
      return(errmsg);
    }
  }


  /* Read the file line by line */
  for(lineno=1;fgets(buf,LINE_MAX_SIZE,fd)!=NULL;lineno++)
  {
    /* Zap comments */
    if((strptr=strchr(buf,';'))!=NULL)
      strptr[0]=0;

    /* Zap any trailing CR, LF, spaces or tabs */
    i=strlen(buf);
    while(i-- && isspace(buf[i]))
      buf[i]=0;

    /* Ignore empty lines */
    if(buf[0])
    {
      /* If we don't have '=' somewhere in the line, it's a syntax error */
      if((strptr=strchr(buf,'='))==NULL)
      {
        sprintf(errmsg,"syntax error in %s, line %d : \"%s\"",
		rcfile,lineno,buf);
        fclose(fd);
        return(errmsg);
      }

      /* Separate var name and value */
      strptr[0]=0;
      val=strptr+1;
      var=buf;

      /* Remove leading and trailing spaces or tabs in the variable name */
      i=strlen(var);
      while(i-- && isspace(var[i]))
        var[i]=0;
      while(i-- && isspace(var[i]))
        var++;

      /* Remove leading and trailing spaces or tabs in the value */
      i=strlen(val);
      while(i-- && isspace(val[i]))
        val[i]=0;
      while(i-- && isspace(val[0]))
        val++;

      /* If we have empty var name, we have a syntax error */
      if(!var[0])
      {
        sprintf(errmsg,"syntax error in %s, line %d : %s",rcfile,lineno,buf);
        fclose(fd);
        return(errmsg);
      }

      /* Fill out the proper variables */
      strptr=NULL;
      intptr=NULL;
      if(!strcmp(var,"cwinput")) strptr=cwinput;
      else if(!strcmp(var,"cwoutput")) strptr=cwoutput;
      else if(!strcmp(var,"cwsound")) strptr=cwsound;
      else if(!strcmp(var,"snddev")) strptr=snddev;
      else if(!strcmp(var,"serialdev")) strptr=serialdev;
      else if(!strcmp(var,"keytype")) strptr=keytype;
      else if(!strcmp(var,"iambicmode")) strptr=iambicmode;
      else if(!strcmp(var,"midelementmodeB")) intptr=&midelementmodeB;
      else if(!strcmp(var,"ditmemory")) intptr=&ditmemory;
      else if(!strcmp(var,"dahmemory")) intptr=&dahmemory;
      else if(!strcmp(var,"autocharspacing")) intptr=&autocharspacing;
      else if(!strcmp(var,"autowordspacing")) intptr=&autowordspacing;
      else if(!strcmp(var,"invertpaddles")) intptr=&invertpaddles;
      else if(!strcmp(var,"ditweight")) intptr=&ditweight;
      else if(!strcmp(var,"sidetone_mode")) intptr=&sidetone_mode;
      else if(!strcmp(var,"cwrxpitch")) intptr=&cwrxpitch;
      else if(!strcmp(var,"cwtxpitch")) intptr=&cwtxpitch;
      else if(!strcmp(var,"squelch")) intptr=&squelch;
      else if(!strcmp(var,"volume")) intptr=&volume;
      else if(!strcmp(var,"cwchannel1")) intptr=&cwchannel[0];
      else if(!strcmp(var,"cwchannel2")) intptr=&cwchannel[1];
      else if(!strcmp(var,"cwchannel3")) intptr=&cwchannel[2];
      else if(!strcmp(var,"cwchannel4")) intptr=&cwchannel[3];
      else if(!strcmp(var,"cwchannel5")) intptr=&cwchannel[4];
      else if(!strcmp(var,"currentcwchannel")) intptr=&currentcwchannel;
      else if(!strcmp(var,"wpm")) intptr=&wpm;
      else if(!strcmp(var,"debounce")) intptr=&debounce;
      else if(!strcmp(var,"recv_buffering")) intptr=&recv_buffering;
      else if(!strcmp(var,"callsign")) strptr=callsign;
      else if(!strcmp(var,"gridsquare")) strptr=gridsquare;
      else if(!strcmp(var,"send_callsign_with_cw"))intptr=
		&send_callsign_with_cw;
      else if(!strcmp(var,"send_gridsquare_with_cw"))intptr=
		&send_gridsquare_with_cw;
      else if(!strcmp(var,"reply_to_ctcp")) intptr=&reply_to_ctcp;
      else if(!strcmp(var,"give_callsign_in_ctcp_reply"))
		intptr=&give_callsign_in_ctcp_reply;
      else if(!strcmp(var,"give_gridsquare_in_ctcp_reply"))
		intptr=&give_gridsquare_in_ctcp_reply;
      else if(!strcmp(var,"give_cwchannel_in_ctcp_reply"))
		intptr=&give_cwchannel_in_ctcp_reply;
      else if(!strcmp(var,"simulate_qrn")
			|| !strcmp(var,"simulate_qrm"))	/* Legacy option */
		intptr=&simulate_qrn;
      else if(!strcmp(var,"qrnlevel")
			|| !strcmp(var,"qrmlevel"))	/* Legacy option */
		intptr=&qrnlevel;
      else if(!strcmp(var,"simulate_signal_strength"))
		intptr=&simulate_signal_strength;
      else if(!strcmp(var,"simulate_sporadicE"))
		intptr=&simulate_sporadicE;
      else if(!strcmp(var,"default_signal_strength"))
		intptr=&default_signal_strength;
      else if(!strcmp(var,"cw_decoder_language"))
		strptr=cw_decoder_language;
      else
      {
        sprintf(errmsg,"syntax error in %s, line %d : unknown variable \"%s\".",
		rcfile,lineno,var);
        fclose(fd);
        return(errmsg);
      }

      if(strptr!=NULL)
      {
        strncpy(strptr,val,LINE_MAX_SIZE);
        strptr[LINE_MAX_SIZE-1]=0;
      }
      else if(intptr!=NULL)
      {
        if(val[0])
          *intptr=strtol(val,(char **)NULL,0);
        else
        {
          sprintf(errmsg,"syntax error in %s, line %d : no value for %s",
			rcfile,lineno,var);
          fclose(fd);
          return(errmsg);
        }
      }
    }
  }
  fclose(fd);

  if(check_parameters()!=NULL)
    return(errmsg);

  load_internal_vars_from_rcfile_values();

  return(NULL);
}



/* Check the parameters. Return codes :
   NULL --> no error
   error message
*/
static char *check_parameters(void)
{
  int i;

  /* Check that the variables have correct values */
  if(strcmp(cwinput,"key") && strcmp(cwinput,"mouse") &&
	strcmp(cwinput,"both"))
  {
    sprintf(errmsg,"invalid CW input source \"%s\".",cwinput);
    return(errmsg);
  }
  if(strcmp(cwoutput,"soundcard") && strcmp(cwoutput,"sounder") &&
	strcmp(cwoutput,"both"))
  {
    sprintf(errmsg,"invalid CW output source \"%s\".",cwoutput);
    return(errmsg);
  }
  if((!strcmp(cwinput,"key") || !strcmp(cwinput,"both") ||
	!strcmp(cwoutput,"sounder") || !strcmp(cwoutput,"both")) &&
	!serialdev[0])
  {
    sprintf(errmsg,"serial device not defined.");
    return(errmsg);
  }
  if((!strcmp(cwoutput,"soundcard") || !strcmp(cwoutput,"both")) &&
	!snddev[0])
  {
    sprintf(errmsg,"sound device not defined.");
    return(errmsg);
  }
  if(strcmp(cwsound,"beeps") && strcmp(cwsound,"sounder_clicks"))
  {
    sprintf(errmsg,"invalid CW sound type \"%s\".",cwsound);
    return(errmsg);
  }
  if(cwrxpitch<-50 || cwrxpitch>50)
  {
    sprintf(errmsg,"invalid CW RX pitch setting %d.",cwrxpitch);
    return(errmsg);
  }
  if(cwtxpitch<-50 || cwtxpitch>50)
  {
    sprintf(errmsg,"invalid CW TX pitch setting %d.",cwtxpitch);
    return(errmsg);
  }
  if(squelch<0 || squelch>100)
  {
    sprintf(errmsg,"invalid squelch setting %d.",squelch);
    return(errmsg);
  }
  if(volume<0 || volume>100)
  {
    sprintf(errmsg,"invalid volume setting %d.",volume);
    return(errmsg);
  }
  for(i=0;i<5;i++)
    if(cwchannel[i]<0 || cwchannel[i]>3999)
    {
      sprintf(errmsg,"invalid morse channel #%d %d.",i+1,cwchannel[i]);
      return(errmsg);
    }
  if(currentcwchannel<1 || currentcwchannel>5)
  {
    sprintf(errmsg,"invalid current morse channel %d.",currentcwchannel);
    return(errmsg);
  }
  if(wpm<1 || wpm>60)
  {
    sprintf(errmsg,"%d wpm is invalid.",wpm);
    return(errmsg);
  }
  if(debounce<1 || debounce>DEBOUNCE_BUF_MAX_SIZE)
  {
    sprintf(errmsg,"debounce value of %d is invalid.",debounce);
    return(errmsg);
  }
  if(strcmp(keytype,"straight") && strcmp(keytype,"iambic"))
  {
    sprintf(errmsg,"invalid key type \"%s\".",keytype);
    return(errmsg);
  }
  if(strcmp(iambicmode,"A") && strcmp(iambicmode,"B"))
  {
    sprintf(errmsg,"iambic mode must be A or B.");
    return(errmsg);
  }
  if(midelementmodeB!=0 && midelementmodeB!=1)
  {
    sprintf(errmsg,"midelementmodeB must be 0 or 1.");
    return(errmsg);
  }
  if(ditmemory!=0 && ditmemory!=1)
  {
    sprintf(errmsg,"ditmemory must be 0 or 1.");
    return(errmsg);
  }
  if(dahmemory!=0 && dahmemory!=1)
  {
    sprintf(errmsg,"dahmemory must be 0 or 1.");
    return(errmsg);
  }
  if(autocharspacing!=0 && autocharspacing!=1)
  {
    sprintf(errmsg,"autocharspacing must be 0 or 1.");
    return(errmsg);
  }
  if(autowordspacing!=0 && autowordspacing!=1)
  {
    sprintf(errmsg,"autowordspacing must be 0 or 1.");
    return(errmsg);
  }
  if(invertpaddles!=0 && invertpaddles!=1)
  {
    sprintf(errmsg,"invertpaddles must be 0 or 1.");
    return(errmsg);
  }
  if(ditweight<15 || ditweight>85)
  {
    sprintf(errmsg,"invalid dit weight \"%d\".",ditweight);
    return(errmsg);
  }
  if(sidetone_mode!=0 && sidetone_mode!=1)
  {
    sprintf(errmsg,"sidetone_mode must be 0 or 1.");
    return(errmsg);
  }
  if(recv_buffering<100 || recv_buffering>3000)
  {
    sprintf(errmsg,"invalid receive buffering %d ms.",recv_buffering);
    return(errmsg);
  }
  for(i=0;i<strlen(callsign);i++)
    if(callsign[i]<'!' || callsign[i]==',' || callsign[i]>'}')
    {
      sprintf(errmsg,"invalid character \"%c\" in callsign \"%s\".",
		callsign[i],callsign);
      return(errmsg);
    }
  if(gridsquare[0] && !cwirc_is_grid_square(gridsquare))
  {
    sprintf(errmsg,"invalid grid square \"%s\".",gridsquare);
    return(errmsg);
  }
  if(send_callsign_with_cw!=0 && send_callsign_with_cw!=1)
  {
    sprintf(errmsg,"send_callsign_with_cw must be 0 or 1.");
    return(errmsg);
  }
  if(send_gridsquare_with_cw!=0 && send_gridsquare_with_cw!=1)
  {
    sprintf(errmsg,"send_gridsquare_with_cw must be 0 or 1.");
    return(errmsg);
  }
  if(reply_to_ctcp!=0 && reply_to_ctcp!=1)
  {
    sprintf(errmsg,"reply_to_ctcp must be 0 or 1.");
    return(errmsg);
  }
  if(give_callsign_in_ctcp_reply!=0 && give_callsign_in_ctcp_reply!=1)
  {
    sprintf(errmsg,"give_callsign_in_ctcp_reply must be 0 or 1.");
    return(errmsg);
  }
  if(give_gridsquare_in_ctcp_reply!=0 && give_gridsquare_in_ctcp_reply!=1)
  {
    sprintf(errmsg,"give_gridsquare_in_ctcp_reply must be 0 or 1.");
    return(errmsg);
  }
  if(give_cwchannel_in_ctcp_reply!=0 && give_cwchannel_in_ctcp_reply!=1)
  {
    sprintf(errmsg,"give_cwchannel_in_ctcp_reply must be 0 or 1.");
    return(errmsg);
  }
  if(simulate_qrn!=0 && simulate_qrn!=1)
  {
    sprintf(errmsg,"simulate_qrn must be 0 or 1.");
    return(errmsg);
  }
  if(qrnlevel<0 || qrnlevel>100)
  {
    sprintf(errmsg,"invalid qrn level %d.",qrnlevel);
    return(errmsg);
  }
  if(simulate_signal_strength!=0 && simulate_signal_strength!=1)
  {
    sprintf(errmsg,"simulate_signal_strength must be 0 or 1.");
    return(errmsg);
  }
  if(simulate_sporadicE!=0 && simulate_sporadicE!=1)
  {
    sprintf(errmsg,"simulate_sporadicE must be 0 or 1.");
    return(errmsg);
  }
  if(default_signal_strength<0 || default_signal_strength>100)
  {
    sprintf(errmsg,"invalid default signal strength %d.",
	default_signal_strength);
    return(errmsg);
  }
  for(i=0;i<NB_CW_CODE_SETS && strcmp(cw_decoder_language,
	cwirc_cw_table[i].lang);i++);
  if(i==NB_CW_CODE_SETS)
  {
    sprintf(errmsg,"invalid CW decoder language \"%s\".,",cw_decoder_language);
    return(errmsg);
  }

  return(NULL);
}



/* Convert all the values from the rcfile into CWirc variables usable by the
   other sections of the program. The rcfile values need to be valid before
   calling this function. */
void load_internal_vars_from_rcfile_values(void)
{
  int i;
  
  sharedmem->do_mouse_input=!strcmp(cwinput,"mouse") || !strcmp(cwinput,"both");
  sharedmem->do_key_input=!strcmp(cwinput,"key") || !strcmp(cwinput,"both");
  sharedmem->do_snddev_output=!strcmp(cwoutput,"soundcard") ||
			!strcmp(cwoutput,"both");
  sharedmem->do_sounder_output=!strcmp(cwoutput,"sounder") ||
			!strcmp(cwoutput,"both");
  sharedmem->cwsound=!strcmp(cwsound,"beeps")?0:1;
  strncpy(sharedmem->snddev,snddev,FILENAME_MAX);
  sharedmem->snddev[FILENAME_MAX-1]=0;
  strncpy(sharedmem->serialdev,serialdev,FILENAME_MAX);
  sharedmem->serialdev[FILENAME_MAX-1]=0;
  sharedmem->doiambic=(keytype[0]=='i');
  sharedmem->iambicmode=(iambicmode[0]=='A'?0:iambicmode[0]=='B'?1:
			iambicmode[3]=='m'?2:3);
  sharedmem->do_midelementmodeB=midelementmodeB;
  sharedmem->do_ditmemory=ditmemory;
  sharedmem->do_dahmemory=dahmemory;
  sharedmem->do_autocharspacing=autocharspacing;
  sharedmem->do_autowordspacing=autowordspacing;
  sharedmem->invertpaddles=invertpaddles;
  sharedmem->dit_weight=ditweight;
  sharedmem->sidetone_mode=sidetone_mode;
  sharedmem->cwrxpitch=cwrxpitch;
  sharedmem->cwtxpitch=cwtxpitch;
  sharedmem->squelch=squelch;
  sharedmem->volume=volume;
  for(i=0;i<5;i++)
    sharedmem->cwchannel[i]=cwchannel[i];
  sharedmem->currcwchannel=currentcwchannel-1;
  sharedmem->wpm=wpm;
  sharedmem->debounce=debounce;
  sharedmem->recv_buffering=recv_buffering;
  strncpy(sharedmem->callsign,callsign,MAX_NICK_SIZE);
  sharedmem->callsign[MAX_NICK_SIZE-1]=0;
  strncpy(sharedmem->gridsquare,gridsquare,MAX_GRIDSQUARE_SIZE);
  sharedmem->gridsquare[MAX_GRIDSQUARE_SIZE-1]=0;
  sharedmem->send_callsign_with_cw=send_callsign_with_cw;
  sharedmem->send_gridsquare_with_cw=send_gridsquare_with_cw;
  sharedmem->reply_to_ctcp=reply_to_ctcp;
  sharedmem->give_callsign_in_ctcp_reply=give_callsign_in_ctcp_reply;
  sharedmem->give_gridsquare_in_ctcp_reply=give_gridsquare_in_ctcp_reply;
  sharedmem->give_cwchannel_in_ctcp_reply=give_cwchannel_in_ctcp_reply;
  sharedmem->simulate_qrn=simulate_qrn;
  sharedmem->qrnlevel=qrnlevel;
  sharedmem->simulate_signal_strength=simulate_signal_strength;
  sharedmem->simulate_sporadicE=simulate_sporadicE;
  sharedmem->default_signal_strength=default_signal_strength;
  for(sharedmem->cwcodeset=0;strcmp(cw_decoder_language,
	cwirc_cw_table[sharedmem->cwcodeset].lang);
	sharedmem->cwcodeset++);
}



/* Save the internal parameters into a config file */
char *cwirc_save_rcfile(char *rcfile)
{
  char filename[FILENAME_MAX];
  char *homedir;
  FILE *fd;
  int i,j;

  strncpy(filename,rcfile,FILENAME_MAX);
  filename[FILENAME_MAX-1]=0;

  /* Expand tildas by the HOME variable */
  homedir=getenv("HOME");
  for(i=0;i<strlen(filename);i++)
    if(filename[i]=='~')
    {
      for(j=strlen(filename);j>i;j--)
        filename[j+strlen(homedir)-1]=filename[j];
      for(j=0;j<strlen(homedir);j++)
        filename[i+j]=homedir[j];
    }

  /* Open the config file for writing */
  if((fd=fopen(filename,"w"))==NULL)
  {
    sprintf(errmsg,"cannot open %s for writing : %s",rcfile,strerror(errno));
    return(errmsg);
  }

  fprintf(fd,"cwinput=%s\n",sharedmem->do_key_input?
	sharedmem->do_mouse_input?"both":"key":"mouse");
  fprintf(fd,"cwoutput=%s\n",sharedmem->do_snddev_output?
	sharedmem->do_sounder_output?"both":"soundcard":"sounder");
  fprintf(fd,"cwsound=%s\n",sharedmem->cwsound==0?"beeps":"sounder_clicks");
  fprintf(fd,"snddev=%s\n",sharedmem->snddev);
  fprintf(fd,"serialdev=%s\n",sharedmem->serialdev);
  fprintf(fd,"debounce=%d\n",sharedmem->debounce);
  fprintf(fd,"keytype=%s\n",sharedmem->doiambic?"iambic":"straight");
  fprintf(fd,"iambicmode=%s\n",sharedmem->iambicmode==0?"A":
	sharedmem->iambicmode==1?"B":sharedmem->iambicmode==2?"ditmem":
	"ditdahmem");
  fprintf(fd,"midelementmodeB=%d\n",sharedmem->do_midelementmodeB?1:0);
  fprintf(fd,"ditmemory=%d\n",sharedmem->do_ditmemory?1:0);
  fprintf(fd,"dahmemory=%d\n",sharedmem->do_dahmemory?1:0);
  fprintf(fd,"autocharspacing=%d\n",sharedmem->do_autocharspacing?1:0);
  fprintf(fd,"autowordspacing=%d\n",sharedmem->do_autowordspacing?1:0);
  fprintf(fd,"invertpaddles=%d\n",sharedmem->invertpaddles?1:0);
  fprintf(fd,"ditweight=%d\n",sharedmem->dit_weight);
  fprintf(fd,"sidetone_mode=%d\n",sharedmem->sidetone_mode);
  fprintf(fd,"wpm=%d\n",sharedmem->wpm);
  fprintf(fd,"cwrxpitch=%d\n",sharedmem->cwrxpitch);
  fprintf(fd,"cwtxpitch=%d\n",sharedmem->cwtxpitch);
  fprintf(fd,"squelch=%d\n",sharedmem->squelch);
  fprintf(fd,"volume=%d\n",sharedmem->volume);
  for(i=0;i<5;i++)
    fprintf(fd,"cwchannel%d=%d\n",i+1,sharedmem->cwchannel[i]);
  fprintf(fd,"currentcwchannel=%d\n",sharedmem->currcwchannel+1);
  fprintf(fd,"recv_buffering=%d\n",sharedmem->recv_buffering);
  fprintf(fd,"callsign=%s\n",sharedmem->callsign);
  fprintf(fd,"gridsquare=%s\n",sharedmem->gridsquare);
  fprintf(fd,"send_callsign_with_cw=%d\n",sharedmem->send_callsign_with_cw);
  fprintf(fd,"send_gridsquare_with_cw=%d\n",sharedmem->send_gridsquare_with_cw);
  fprintf(fd,"reply_to_ctcp=%d\n",sharedmem->reply_to_ctcp);
  fprintf(fd,"give_callsign_in_ctcp_reply=%d\n",
	sharedmem->give_callsign_in_ctcp_reply);
  fprintf(fd,"give_gridsquare_in_ctcp_reply=%d\n",
	sharedmem->give_gridsquare_in_ctcp_reply);
  fprintf(fd,"give_cwchannel_in_ctcp_reply=%d\n",
	sharedmem->give_cwchannel_in_ctcp_reply);
  fprintf(fd,"simulate_qrn=%d\n",sharedmem->simulate_qrn);
  fprintf(fd,"qrnlevel=%d\n",sharedmem->qrnlevel);
  fprintf(fd,"simulate_signal_strength=%d\n",
	sharedmem->simulate_signal_strength);
  fprintf(fd,"simulate_sporadicE=%d\n",sharedmem->simulate_sporadicE);
  fprintf(fd,"default_signal_strength=%d\n",sharedmem->default_signal_strength);
  fprintf(fd,"cw_decoder_language=%s\n",
	cwirc_cw_table[sharedmem->cwcodeset].lang);

  fclose(fd);

  return(NULL);
}

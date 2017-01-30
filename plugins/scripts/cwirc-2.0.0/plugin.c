/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   X-Chat plugin stub.

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <xchat-plugin.h>

#include "types.h"
#include "common.h"
#include "cwirc.h"
#include "cwframe.h"
#include "ipc.h"



/* Global variables */
struct cwirc_shm_block *sharedmem;
static xchat_plugin *ph;		/* plugin handle */
static xchat_hook *xc_hook[12];
static T_BOOL plugin_enabled=0;
static pid_t watchdog_pid;
static pid_t frontend_pid;
static char ctcp_reply[128+MAX_NICK_SIZE]="";
static char locked_channel[MAX_CHANNEL_NAME_SIZE]="";
static char locked_server[MAX_SERVER_NAME_SIZE]="";
static int shmid=-1;



/* Prototypes */
int xchat_plugin_init(xchat_plugin *plugin_handle, char **plugin_name,
			char **plugin_desc, char **plugin_version, char *arg);
int xchat_plugin_deinit(void);
static int enable_cb(char *word[], char *word_eol[], void *userdata);
static void disable_plugin(void);
static int msg_receive_cb(char *word[], void *userdata);
static int msg_send_cb(void *userdata);
static int ctcp_query_cb(char *word[],void *userdata);
static int sent_msgs_filter_cb(char *word[],void *userdata);
static int sent_notices_filter_cb(char *word[],void *userdata);
static int cwlock_cb(char *word[], char *word_eol[], void *userdata);
static int cwunlock_cb(char *word[], char *word_eol[], void *userdata);
static void clean_exit_hdlr(int nothing);



/* Register the plugin */
int xchat_plugin_init(xchat_plugin *plugin_handle, char **plugin_name,
			char **plugin_desc, char **plugin_version, char *arg)
{
  plugin_enabled=0;

  /* we need to save this for use with any xchat_* functions */
  ph=plugin_handle;

  *plugin_name="CWirc";
  *plugin_desc="Send and receive raw morse code over IRC";
  *plugin_version=VERSION;

  /* Hook the message receive callback that'll filter out incoming cwirc frames
     even when the plugin is disabled */
  xc_hook[1]=xchat_hook_print(ph,"Channel Message",XCHAT_PRI_NORM,
	msg_receive_cb,0);
  xc_hook[2]=xchat_hook_print(ph,"Private Message",XCHAT_PRI_NORM,
	msg_receive_cb,0);
  xc_hook[3]=xchat_hook_print(ph,"Private Message to Dialog",XCHAT_PRI_NORM,
	msg_receive_cb,0);
  xc_hook[4]=xchat_hook_print(ph,"Notice",XCHAT_PRI_NORM,
	msg_receive_cb,0);

  /* Add the "/CW" command */
  xc_hook[0]=xchat_hook_command(ph,"CW",XCHAT_PRI_NORM,enable_cb,
			"Usage: CW, Turns ON/OFF morse coding/decoding",0);

  /* Add a "CWirc" button */
  xchat_commandf(ph,"ADDBUTTON CWirc CW");

  /* Inform the user we're loaded */
  xchat_printf(ph,"CWirc loaded successfully!\n");

  /* Seed the RNG with something vaguely random */
  srand(time(NULL));

  return(1);
}



/* Deregister the plugin */
int xchat_plugin_deinit(void)
{
  int i;

  /* Disable the plugin if it's already enabled */
  if(plugin_enabled)
  {
    /* Stop the frontend */
    sharedmem->stop_frontend=1;

    /* Disable the plugin */
    disable_plugin();
  }

  /* Remove the "CWirc" button */
  xchat_commandf(ph,"DELBUTTON CWirc CW");

  /* Unhook all the remaining callbacks */
  for(i=0;i<5;i++)
    xchat_unhook(ph,xc_hook[i]);

  xchat_printf(ph, "CWirc unloaded successfully!\n");
  return(1);
}



/* Enable the plugin */
static int enable_cb(char *word[], char *word_eol[], void *userdata)
{
  char shmid_string[32];
  char frontend_msg[FRONTEND_MSG_SIZE];
  int frontend_stdout_pipe[2];
  FILE *frontend_stdout;
  int caught_sigs_not_sigchld[]={SIGHUP,SIGINT,SIGKILL,SIGPIPE,SIGIO,	\
		SIGPROF,SIGTERM,SIGUSR1,SIGUSR2,SIGVTALRM,SIGURG,-1};
  struct sigaction sigact;
  int i;

  /* Disable the plugin if it's already enabled */
  if(plugin_enabled)
  {
    /* Stop the frontend */
    sharedmem->stop_frontend=1;
    return(XCHAT_EAT_ALL);
  }

  /* Enable the plugin : */
  /* Create the shared memory block */
  if((shmid=cwirc_shm_alloc(rand(),sizeof(struct cwirc_shm_block)))==-1)
  {
    xchat_printf(ph,"CWirc : error : can't create shared memory.\n");
    return(XCHAT_EAT_ALL);
   }

  /* Attach to the shared memory block */
  if((sharedmem=(struct cwirc_shm_block *)cwirc_shm_attach(shmid))
	==(struct cwirc_shm_block *)-1)
  {
    cwirc_shm_detach(sharedmem);
    cwirc_shm_free(shmid);
    xchat_printf(ph,"CWirc : error : can't attach to the shared memory.\n");
    return(XCHAT_EAT_ALL);
  }

  /* Store the version number known to us (the plugin) in the shared memory
     block, so the frontend will be able to detect a version mismatch */
  strcpy(sharedmem->version,VERSION);

  /* Initialize the senders table */
  for(i=0;i<MAX_SENDERS;i++)
    sharedmem->sender[i].name[0]=0;

  /* Zero the xmit buffer */
  for(i=0;i<XMIT_BUF_MAX_SIZE;i++)
    sharedmem->xmit_buf[i]=0;
  sharedmem->xmit_buf_flush_nb_evts=0;

  /* Make sure we have some values set up now because the config file isn't
     loaded yet by the frontend, but we start using the value within the plugin
     before spawning the frontend */
  sharedmem->recv_buffering=1000;
  for(i=0;i<5;i++)
    sharedmem->cwchannel[i]=1000;
  sharedmem->currcwchannel=0;
  sharedmem->reply_to_ctcp=0;
  sharedmem->give_callsign_in_ctcp_reply=0;
  sharedmem->give_gridsquare_in_ctcp_reply=0;
  sharedmem->give_cwchannel_in_ctcp_reply=0;

  /* Make sure the plugin isn't locked on any channel */
  locked_channel[0]=0;

  /* Create the semaphore */
  if((sharedmem->semid=cwirc_sem_create(rand(),NB_SEMAPHORES))==-1)
  {
    xchat_printf(ph,"CWirc : error : can't create semaphore.\n");
    cwirc_shm_detach(sharedmem);
    cwirc_shm_free(shmid);
    return(XCHAT_EAT_ALL);
  }

  /* Spawn the frontend */
  sharedmem->stop_frontend=0;
  sharedmem->frontend_stopped=0;
  switch((watchdog_pid=fork()))	/* Escape from the X-Chat execution context */
  {
  case -1:			/* Error fork()ing */
    cwirc_sem_destroy(sharedmem->semid);
    cwirc_shm_detach(sharedmem);
    cwirc_shm_free(shmid);
    xchat_printf(ph,"CWirc : error : can't spawn frontend watchdog process.\n");
    return(XCHAT_EAT_ALL);
    break;

  case 0:			/* I'm the 1st child */
    /* Make sure we catch all signals other than SIGCHLD to exit cleanly */
    for(i=0;caught_sigs_not_sigchld[i]!=-1;i++)
      signal(caught_sigs_not_sigchld[i],&clean_exit_hdlr);

    /* Make sure we catch the frontend process' death (but not its stopping) */
    sigact.sa_handler=clean_exit_hdlr;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags=SA_NOCLDSTOP;
    sigaction(SIGCHLD,&sigact,NULL);

    /* Make a pipe to catch the frontend's stdout */
    if(pipe(frontend_stdout_pipe))
    {
      printf("CWirc : error : cannot create unnamed pipe.\n");
      fflush(stdout);
      _exit(0);
    }

    switch((frontend_pid=fork()))	/* Spawn the frontend process proper */
    {
    case -1:			/* Error fork()ing */
      printf("CWirc : error : cannot spawn frontend process.\n");
      fflush(stdout);
      _exit(0);
      break;

    case 0:			/* I'm the 2nd child */
      /* Connect my stdout to the pipe and close the reading end of the pipe */
      close(1);
      dup(frontend_stdout_pipe[1]);
      close(frontend_stdout_pipe[0]);

      /* Spawn the frontend and pass it the id of the shared memory block */
      sprintf(shmid_string,"0x%0x",shmid);
      execlp(FRONTEND,FRONTEND,shmid_string,NULL);
      printf("CWirc : error : cannot execute \"%s\".\n",FRONTEND);
      fflush(stdout);
      sharedmem->frontend_stopped=1;
      _exit(0);
      break;

    default:
      /* Close the writing end of the pipe */
      close(frontend_stdout_pipe[1]);
      frontend_stdout=fdopen(frontend_stdout_pipe[0],"r");

      /* Capture what the frontend says. If the pipe closes, the frontend has
         died. */
      while(fgets(frontend_msg,FRONTEND_MSG_SIZE,frontend_stdout)!=NULL)
      {
        /* Zap trailing CR or LFs */
        i=strlen(frontend_msg);
        while(i && (frontend_msg[i-1]=='\n' || frontend_msg[i-1]=='\r'))
          frontend_msg[--i]=0;

        /* Send the message for display in the X-Chat window : */
        i=1;
        do
        {
          if(!cwirc_sem_P(sharedmem->semid,SEM_FRONTEND_MSG))	/* Acquire sem*/
          {
            if(!sharedmem->frontend_msg[0])
            {
              strncpy(sharedmem->frontend_msg,frontend_msg,FRONTEND_MSG_SIZE);
              sharedmem->frontend_msg[FRONTEND_MSG_SIZE-1]=0;
              i=0;
            }
            /* Release the semaphore */
            cwirc_sem_V(sharedmem->semid,SEM_FRONTEND_MSG);
          }
          /* If we couldn't send the message, sleep a bit */
          if(i)
            usleep(10000);
        }
        while(i);
      }

      sharedmem->stop_frontend=1;

      /* Reap the frontend's process */
      waitpid(frontend_pid,NULL,0);

      /* Mark the frontend as stopped */
      sharedmem->frontend_stopped=1;

      _exit(0);
      break;
    }
    break;
  }

  /* Hook the additional callbacks we need when the plugin is enabled */
  xc_hook[5]=xchat_hook_timer(ph,10,msg_send_cb,NULL);
  xc_hook[6]=xchat_hook_print(ph,"Your Message",XCHAT_PRI_NORM,
	sent_msgs_filter_cb,0);
  xc_hook[7]=xchat_hook_print(ph,"CTCP Generic",XCHAT_PRI_NORM,
	ctcp_query_cb,0);
  xc_hook[8]=xchat_hook_print(ph,"CTCP Generic to Channel",XCHAT_PRI_NORM,
	ctcp_query_cb,0);
  xc_hook[9]=xchat_hook_print(ph,"Notice Send",XCHAT_PRI_NORM,
	sent_notices_filter_cb,0);
  xc_hook[10]=xchat_hook_command(ph,"CWLOCK",XCHAT_PRI_NORM,cwlock_cb,
		"Usage: CWLOCK, Locks CWirc onto the current chat window",0);
  xc_hook[11]=xchat_hook_command(ph,"CWUNLOCK",XCHAT_PRI_NORM,cwunlock_cb,
		"Usage: CWUNLOCK, Release CWirc from any chat window lock.",0);

  /* All good */
  plugin_enabled=1;

  return(XCHAT_EAT_ALL);
}



/* Disable the plugin */
void disable_plugin(void)
{
  int i;
  
  /* Reap the frontend watchdog process */
  waitpid(watchdog_pid,NULL,0);

  /* Unhook the callbacks we don't need when the plugin is disabled */
  for(i=5;i<12;i++)
    xchat_unhook(ph,xc_hook[i]);

  plugin_enabled=0;

  /* Detach/free the shared memory block, destroy the semaphore */
  cwirc_sem_destroy(sharedmem->semid);
  cwirc_shm_detach(sharedmem);
  cwirc_shm_free(shmid);

  xchat_printf(ph, "CWirc disabled!\n");
}



/* Receive messages from the IRC channel */
static int msg_receive_cb(char *word[],void *userdata)
{
  xchat_context *curr_ctx,*ctx;
  const char *ircchannel,*ircserver;
  char src_ircchannel[MAX_CHANNEL_NAME_SIZE];
  char src_ircserver[MAX_SERVER_NAME_SIZE];
  char cmp_ircchannel[MAX_CHANNEL_NAME_SIZE];
  char *nickptr;
  char *callsign;

  /* Is the line a cw frame ? */
  if(cwirc_is_cw_frame(word[2]))
  {
    /* Is the plugin enabled ? */
    if(plugin_enabled)
    {
      /* Find the originating channel namee */
      if((ircchannel=xchat_get_info(ph,"channel"))==NULL)
        return(XCHAT_EAT_ALL);
      strncpy(src_ircchannel,ircchannel,MAX_CHANNEL_NAME_SIZE);
      src_ircchannel[MAX_CHANNEL_NAME_SIZE-1]=0;
    
      /* Find the originating server namee */
      if((ircserver=xchat_get_info(ph,"server"))==NULL)
        return(XCHAT_EAT_ALL);
      strncpy(src_ircserver,ircserver,MAX_SERVER_NAME_SIZE);
      src_ircserver[MAX_SERVER_NAME_SIZE-1]=0;
    
      /* Are we not locked on a particular channel/server ? */
      if(!locked_channel[0])
      {
        /* Save the current xchat context */
        if((curr_ctx=xchat_get_context(ph))==NULL)
          return(XCHAT_EAT_ALL);
    
        /* Find the context of the window currently in focus and switch to it */
        if((ctx=xchat_find_context(ph,NULL,NULL))==NULL)
          return(XCHAT_EAT_ALL);
        if(!xchat_set_context(ph,ctx))
          return(XCHAT_EAT_ALL);
    
        /* Find the current channel name */
        if((ircchannel=xchat_get_info(ph,"channel"))==NULL)
          return(XCHAT_EAT_ALL);
        strncpy(cmp_ircchannel,ircchannel,MAX_CHANNEL_NAME_SIZE);
        cmp_ircchannel[MAX_CHANNEL_NAME_SIZE-1]=0;
        ircchannel=cmp_ircchannel;
    
        /* Find the current server name */
        if((ircserver=xchat_get_info(ph,"server"))==NULL)
          return(XCHAT_EAT_ALL);
    
        /* Restore the current context (source channel context) */
        if(!xchat_set_context(ph,curr_ctx))
          return(XCHAT_EAT_ALL);
      }
      else
      {
        ircchannel=locked_channel;
        ircserver=locked_server;
      }
    
      /* Remove possible X-Chat color codes from the nick */
      nickptr=word[1];
      if(nickptr[0]==3)	/* CTRL-C indicates a color code follows */
        while(isdigit((++nickptr)[0]));
  
      /* Did the line come from the channel currently in focus or the one we're
         locked on ? */
      if(!strcmp(src_ircchannel,ircchannel) && !strcmp(src_ircserver,ircserver))
      {
        /* Decode the frame */
        if(cwirc_decode_cw_frame(nickptr,word[2],&callsign)==1)
        {
          if(callsign!=NULL)
            xchat_printf(ph, "Receiving cw from %s [from %s] ...\n",callsign,
  		nickptr);
          else
            xchat_printf(ph, "Receiving cw from %s ...\n",nickptr);
        }
      }
    }

    /* Filter out the cw frame from the channel window */
    return(XCHAT_EAT_ALL);
  }

  return(XCHAT_EAT_NONE);
}



/* This routine gets called regularly by xchat to poll and check if a message
   needs to be sent, and also take care of disabling the plugin if the frontend
   has terminated. It's butt-ugly but there's no other mechanism. */
static int msg_send_cb(void *userdata)
{
  const char *dst;
  xchat_context *curr_ctx=NULL,*ctx;
  xchat_list *dcclist;
  T_BOOL do_dccchat;
  char *msgptr;
  static time_t last_error_tstamp=0,curtime;
  int i;

  /* Check if we have a message from the frontend */
  if(!cwirc_sem_P(sharedmem->semid,SEM_FRONTEND_MSG))	/* Acquire semaphore */
  {
    /* Any error message from the frontend ? */
    if(sharedmem->frontend_msg[0])
    {
      xchat_printf(ph,"%s\n",sharedmem->frontend_msg);
      sharedmem->frontend_msg[0]=0;
    }

    /* Release the semaphore */
    cwirc_sem_V(sharedmem->semid,SEM_FRONTEND_MSG);	/* Release semaphore */
  }

  /* Check if we have a morse message to send to the irc server */
  if(!cwirc_sem_P(sharedmem->semid,SEM_XMIT_BUF))	/* Acquire semaphore */
  {
    /* Any CW message to transmit ? */
    if(sharedmem->xmit_buf_flush_nb_evts)
    {
      dst=NULL;

      /* Save the current xchat context */
      if((curr_ctx=xchat_get_context(ph))!=NULL)
      {
        /* Find the context of the window currently in focus, or the locked
           channel/server */
        if((ctx=xchat_find_context(ph,locked_channel[0]?locked_server:NULL,
		locked_channel[0]?locked_channel:NULL))!=NULL)
        {
          /* Switch to that context */
          if(xchat_set_context(ph,ctx))
          {        
            /* If we're not locked on a channel/server, find the current
               channel name */
            if(locked_channel[0])
              dst=locked_channel;
            else
              dst=xchat_get_info(ph,"channel");
          }
        }
        else	/* We failed to find the context */
        {
          /* Is the channel/server locked ? */
          if(locked_channel[0])
          {
            /* If it is more than 3s since we last sent the following error msg,
               change context to the window currently in focus and send it
               again */
            curtime=time(NULL);
            if(curtime-last_error_tstamp>3 &&
		(ctx=xchat_find_context(ph,NULL,NULL))!=NULL &&
		xchat_set_context(ph,ctx))
              xchat_printf(ph,"WARNING: can't send cw to \"%s\" (%s) : stale "
				"lock.\n",locked_channel,locked_server);

            last_error_tstamp=curtime;
          }
        }
      }

      /* Do we have something to send to ? */
      if(dst!=NULL)
      {
        /* Check if we have a DCC CHAT connection to destination if it's a
           nick */
        do_dccchat=0;
        if(dst[0]!='#' && (dcclist=xchat_list_get(ph,"dcc")))
        {
          while(!do_dccchat && xchat_list_next(ph,dcclist))
          {
            i=xchat_list_int(ph,dcclist,"type");
            if(!strcmp(dst,xchat_list_str(ph,dcclist,"nick")) &&
		xchat_list_int(ph,dcclist,"status")==1 && (i==2 || i==3))
              do_dccchat=1;
          }
        }

        /* Acquire the semaphore to access the personal information */
        if(!cwirc_sem_P(sharedmem->semid,SEM_PERSONAL_INFO))
        {
          if((msgptr=cwirc_encode_cw_frame())!=NULL)
            xchat_commandf(ph,"MSG %s%s %s",do_dccchat?"=":"",dst,msgptr);

          /* Release the semaphore */
          cwirc_sem_V(sharedmem->semid,SEM_PERSONAL_INFO);
        }
      }

      sharedmem->xmit_buf_flush_nb_evts=0;
    }

    /* Restore the current context */
    if(curr_ctx!=NULL)
      xchat_set_context(ph,curr_ctx);

    /* Release the semaphore */
    cwirc_sem_V(sharedmem->semid,SEM_XMIT_BUF);
  }

  /* Do we need to disable the plugin because the frontend has stopped ? */
  if(sharedmem->frontend_stopped)
    disable_plugin();

  return(1);
}



/* Watch incoming CTCP queries, intercept "CTCP CWIRC" and repy, or not. */
static int ctcp_query_cb(char *word[],void *userdata)
{
  T_BOOL report_callsign,report_gridsquare;
  
  /* Do we reply to CTCP CWIRC queries, and is this one ? */
  if(sharedmem->reply_to_ctcp && !strcasecmp(word[1],"CWIRC"))
  {
    sprintf(ctcp_reply,"CWIRC CWirc version %s",VERSION);

    if(sharedmem->give_callsign_in_ctcp_reply ||
	sharedmem->give_gridsquare_in_ctcp_reply ||
	sharedmem->give_cwchannel_in_ctcp_reply)
    {
      strcat(ctcp_reply," (");

      /* Should we give the callsign or the grid square in the reply ? */
      report_callsign=sharedmem->give_callsign_in_ctcp_reply &&
			sharedmem->callsign[0];
      report_gridsquare=sharedmem->give_gridsquare_in_ctcp_reply &&
			sharedmem->gridsquare[0];
      if(report_callsign || report_gridsquare)
      {
        /* Acquire the semaphore to access the personal information */
        if(!cwirc_sem_P(sharedmem->semid,SEM_PERSONAL_INFO))
        {
          sprintf(ctcp_reply+strlen(ctcp_reply),"user ");

          if(report_callsign)
            sprintf(ctcp_reply+strlen(ctcp_reply),"callsign \"%s\"%s",
			sharedmem->callsign,report_gridsquare ||
			sharedmem->give_cwchannel_in_ctcp_reply?", ":")");

          if(report_gridsquare)
            sprintf(ctcp_reply+strlen(ctcp_reply),"grid location \"%s\"%s",
			sharedmem->gridsquare,
			sharedmem->give_cwchannel_in_ctcp_reply?", ":")");

          /* Release the semaphore */
          cwirc_sem_V(sharedmem->semid,SEM_PERSONAL_INFO);
        }
      }

      /* Should we give the current CW channel in the reply ? */
      if(sharedmem->give_cwchannel_in_ctcp_reply)
        sprintf(ctcp_reply+strlen(ctcp_reply),"currently on channel %d)",
			sharedmem->cwchannel[sharedmem->currcwchannel]);
    }

    /* Send the reply */
    xchat_commandf(ph,"NOTICE %s %s",word[2],ctcp_reply);
  }

  return(XCHAT_EAT_NONE);
}



/* Watch what we send to the server and trap our own cw frames */
static int sent_msgs_filter_cb(char *word[],void *userdata)
{
  static time_t last_send_tstamp=0,curtime;
  
  /* Is the message one of our cw frames ? */
  if(cwirc_is_cw_frame(word[2]))
  {
    /* Is it more than 3s since we sent the last cw frame ? */
    curtime=time(NULL);
    if(curtime-last_send_tstamp>3)
      xchat_printf(ph,"%s sending cw ...\n",word[1]);

    last_send_tstamp=curtime;

    return(XCHAT_EAT_ALL);
  }

  return(XCHAT_EAT_NONE);
}



/* Watch the notices we send to the server and trap our own CTCP replies */
static int sent_notices_filter_cb(char *word[],void *userdata)
{
  if(!strcmp(word[2],ctcp_reply))
    return(XCHAT_EAT_ALL);

  return(XCHAT_EAT_NONE);
}



/* Locks CWirc onto the current chat window, i.e. make sure it'll always send
   and receive from the corresponding channel/nick from now on, even if the user
   changes chat window afterward */
static int cwlock_cb(char *word[], char *word_eol[], void *userdata)
{
  xchat_context *curr_ctx,*ctx;
  const char *irc_info;

  /* Save the current xchat context */
  if((curr_ctx=xchat_get_context(ph))==NULL)
    return(XCHAT_EAT_ALL);

  /* Find the context of the window currently in focus and switch to it */
  if((ctx=xchat_find_context(ph,NULL,NULL))==NULL)
    return(XCHAT_EAT_ALL);
  if(!xchat_set_context(ph,ctx))
    return(XCHAT_EAT_ALL);

  /* Find the current channel name */
  if((irc_info=xchat_get_info(ph,"channel"))==NULL || !irc_info[0])
  {
    locked_channel[0]=locked_server[0]=0;
    return(XCHAT_EAT_ALL);
  }
  strncpy(locked_channel,irc_info,MAX_CHANNEL_NAME_SIZE);
  locked_channel[MAX_CHANNEL_NAME_SIZE-1]=0;

  /* Find the current server name */
  if((irc_info=xchat_get_info(ph,"server"))==NULL || !irc_info[0])
  {
    locked_channel[0]=locked_server[0]=0;
    return(XCHAT_EAT_ALL);
  }
  strncpy(locked_server,irc_info,MAX_SERVER_NAME_SIZE);
  locked_server[MAX_SERVER_NAME_SIZE-1]=0;

  /* Restore the current context */
  if(!xchat_set_context(ph,curr_ctx))
  {
    locked_channel[0]=locked_server[0]=0;
    return(XCHAT_EAT_ALL);
  }

  /* Tell the user what CWirc is locked on now */
  xchat_printf(ph, "CWirc locked onto \"%s\" (%s).\n",locked_channel,
		locked_server);

  return(XCHAT_EAT_ALL);
}



/* Release CWirc from any chat window it might be locked onto. */
static int cwunlock_cb(char *word[], char *word_eol[], void *userdata)
{
  /* Tell the user what CWirc has been unlocked from if we can. */
  if(locked_channel[0])
    xchat_printf(ph, "CWirc released from \"%s\" (%s).\n",locked_channel,
		locked_server);
  else
    xchat_printf(ph, "CWirc is not locked.\n");

  locked_channel[0]=locked_server[0]=0;

  return(XCHAT_EAT_ALL);
}



/* Signal handler to make sure we terminate the frontend if we get any signal
   in the frontend watch process . */
static void clean_exit_hdlr(int nothing)
{
  /* Terminate the frontend. */
  sharedmem->stop_frontend=1;
}

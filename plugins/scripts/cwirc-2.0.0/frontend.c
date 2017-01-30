/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   Frontend application.

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "types.h"
#include "cwirc.h"
#include "common.h"
#include "rcfile.h"
#include "io.h"
#include "gui.h"
#include "extension.h"
#include "ipc.h"



/* Global variables */
struct cwirc_shm_block *sharedmem;
int shmid;
int ext_shmid;		/* Extension API's shared memory id */
struct cwirc_extension_api *ext_sharedmem;
int io_process_pid=-1;



/* Prototypes */
static void sigchld_hdlr(int nothing);
static void clean_exit_hdlr(int nothing);



/* Main Function */
int main(int argc,char *argv[])
{
  char *errmsg;
  int caught_sigs_not_sigchld[]={SIGHUP,SIGINT,SIGKILL,SIGPIPE,SIGIO,	\
		SIGPROF,SIGTERM,SIGUSR1,SIGUSR2,SIGVTALRM,SIGURG,-1};
  struct sigaction sigact;
  int normal_priority;
  int i;

  /* Find out our current priority */
  errno=0;
  normal_priority=getpriority(PRIO_PROCESS,0);
  if(errno)			/* Error finding out the priority ? */
    normal_priority=-99;	/* Forget about the whole priority thing */

  /* Try to renice ourselves, then immediately drop root privileges. If this
     fails, it's not an error as the user would only install the frontend
     binary suid root to allow it to renice itself, to cure "scratchy" sound
     with low-end soundcards. If the user is happy with I/O performances
     without renicing, so much the better. */
  if(normal_priority!=-99)
    setpriority(PRIO_PROCESS,0,-20);
  setreuid(getuid(),getuid());
  setregid(getgid(),getgid());

  /* We need one argument only, and it needs to be the shared memory id */
  if(argc!=2 || (shmid=strtol(argv[1],NULL,0))==0)
  {
    printf("Error: %s is a frontend for the CWirc X-Chat plugin.\n",argv[0]);
    printf("       It cannot be used as a standalone application.\n");
    usleep(250000);
    return(-1);
  }

  /* Attach to the shared memory block */
  if((sharedmem=(struct cwirc_shm_block *)cwirc_shm_attach(shmid))
	==(struct cwirc_shm_block *)-1)
  {
    printf("CWirc (frontend) : error : can't attach to the shared memory.\n");
    usleep(250000);
    return(-1);
  }

  /* Check the plugin's version number against ours */
  if(strncmp(sharedmem->version,VERSION,strlen(VERSION)))
  {
    printf("CWirc (frontend) : error : plugin/frontend version mismatch.\n");
    usleep(250000);
    return(-1);
  }

  sharedmem->mouseinputbutton0=0;
  sharedmem->mouseinputbutton1=0;
  sharedmem->cwcodeset=0;
  sharedmem->decoded_msg_buf[0]=0;
  sharedmem->decoded_msg_buf_char_markers[0]=0;
  sharedmem->decoded_msg_wpm=-1;
  sharedmem->decoded_msg_updated=0;
  sharedmem->reset_decoder=0;
  sharedmem->sidetone_mode=0;

  /* Read/check the config file */
  if((errmsg=cwirc_parse_rcfile(RCFILE))!=NULL)
  {
    printf("CWirc : error : %s",errmsg);
    cwirc_shm_detach(sharedmem);
    usleep(250000);
    return(-1);
  }

  /* Seed the RNG with something vaguely random */
  srand(time(NULL));

  /* Create a shared memory block for the extension API */
  while((i=rand())==shmid);
  if((ext_shmid=cwirc_shm_alloc(i,sizeof(struct cwirc_extension_api)))==-1)
  {
    printf("CWirc : error : can't create shared memory for the extension API");
    cwirc_shm_detach(sharedmem);
    usleep(250000);
    return(-1);
  }

  /* Attach to the extension API's shared memory block */
  if((ext_sharedmem=(struct cwirc_extension_api *)cwirc_shm_attach(ext_shmid))
	==(struct cwirc_extension_api *)-1)
  {
    printf("CWirc (frontend) : error : can't attach to the extension API's "
    		"shared memory\n");
    cwirc_shm_free(ext_shmid);
    cwirc_shm_detach(sharedmem);
    usleep(250000);
    return(-1);
  }

  /* Create 2 semaphores to sync an extension program and allow it to safely
     get audio out of the extension API's audio buffer */
  while((i=rand())==sharedmem->semid);
  if((ext_sharedmem->semid=cwirc_sem_create(i,2))==-1)
  {
    printf("CWirc (frontend) : error : can't create sync semaphores for the "
		"extension API\n");
    cwirc_shm_detach(ext_sharedmem);
    cwirc_shm_free(ext_shmid);
    cwirc_shm_detach(sharedmem);
    usleep(250000);
    return(-1);
  }

  /* Acquire the 2 semaphores before any extension program has a chance to
     start */
  for(i=0;i<2;i++)
    if(cwirc_sem_P(ext_sharedmem->semid,i)!=0)
    {
      printf("CWirc (frontend) : error : can't acquire sync semaphore #%d for "
      		"the extension API\n",i);
      cwirc_shm_detach(ext_sharedmem);
      cwirc_shm_free(ext_shmid);
      cwirc_shm_detach(sharedmem);
      usleep(250000);
      return(-1);
    }

  /* Initialize the extension API */
  ext_sharedmem->out_audiobuf_start=0;
  ext_sharedmem->out_audiobuf_end=0;
  ext_sharedmem->in_key=0;
  ext_sharedmem->pid=-1;

  /* Make sure we catch all signals other than SIGCHLD to exit cleanly */
  for(i=0;caught_sigs_not_sigchld[i]!=-1;i++)
    signal(caught_sigs_not_sigchld[i],&clean_exit_hdlr);

  /* Make sure we catch the I/O process' death (but not its stopping) */
  sigact.sa_handler=sigchld_hdlr;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags=SA_NOCLDSTOP;
  sigaction(SIGCHLD,&sigact,NULL);

  /* Spawn the I/O process */
  if((io_process_pid=cwirc_spawn_io_process(shmid,ext_shmid))==-1)
  {
    printf("CWirc : error : can't spawn I/O process.\n");
    cwirc_sem_destroy(ext_sharedmem->semid);
    cwirc_shm_detach(ext_sharedmem);
    cwirc_shm_free(ext_shmid);
    cwirc_shm_detach(sharedmem);
    usleep(250000);
    return(-1);
  }

  /* The I/O process is running, possibly with nicer priority. The user
     interface doesn't need that however, so renice ourselves down now. */
  if(normal_priority!=-99)
    setpriority(PRIO_PROCESS,0,normal_priority);

  printf("CWirc enabled!\n");
  fflush(stdout);

  /* Run the user interface */
  cwirc_ui(ext_shmid);

  /* If an extension program is running, kill it */
  if(ext_sharedmem->pid!=-1)
  {
    /* Try nicely at first */
    kill(ext_sharedmem->pid,SIGHUP);
    sleep(1);

    /* If the process still isn't dead and reaped by now, be a little more
       persuasive */
    if(ext_sharedmem->pid!=-1)
    {
      kill(ext_sharedmem->pid,SIGKILL);
      sleep(1);
    }
  }

  /* Clean things up */
  cwirc_sem_destroy(ext_sharedmem->semid);
  cwirc_shm_detach(ext_sharedmem);
  cwirc_shm_free(ext_shmid);
  cwirc_shm_detach(sharedmem);

  return(0);
}



/* SIGCHLD signal handler */
static void sigchld_hdlr(int nothing)
{
  int pid;
  
  /* Reap the child process that died */
  pid=wait(NULL);

  if(pid==io_process_pid)	/* I/O process died */
    sharedmem->stop_frontend=1; /* Terminate the entire frontend. */
  else
    if(pid==ext_sharedmem->pid)	/* Extension process died */
      ext_sharedmem->pid=-1;
}



/* Signal handler to make sure we reap the I/O process before dying. */
static void clean_exit_hdlr(int nothing)
{
  /* Terminate the I/O process. */
  sharedmem->stop_frontend=1;
}

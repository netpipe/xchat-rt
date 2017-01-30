/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   CWirc extensions routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "types.h"
#include "cwirc.h"
#include "extension.h"
#include "ipc.h"
#include "common.h"



/* Variables */
char cwirc_extensions[MAX_CWIRC_EXTENSIONS][FILENAME_MAX];



/* Browse the CWirc extensions directory and store filenames present there */
void get_available_cwirc_extensions(void)
{
  DIR *extdir;
  struct dirent *dirent;
  struct stat statbuf;
  char fullpathname[FILENAME_MAX];
  int i;
  
  /* Initialize the extensions table */
  for(i=0;i<MAX_CWIRC_EXTENSIONS;i++)
    cwirc_extensions[i][0]=0;

  /* Open the extensions directory */
  if((extdir=opendir(EXTENSIONS_DIR))==NULL)
    return;

  /* Browse the content of the directory */
  for(i=0;i<MAX_CWIRC_EXTENSIONS && (dirent=readdir(extdir))!=NULL;)
  {
    /* Is the directory entry an executable file ? */
    sprintf(fullpathname,"%s/%s",EXTENSIONS_DIR,dirent->d_name);
    if(stat(fullpathname,&statbuf)!=-1 && S_ISREG(statbuf.st_mode) &&
	!access(fullpathname,X_OK))
    {
      strncpy(cwirc_extensions[i],dirent->d_name,FILENAME_MAX-1);
      cwirc_extensions[i][FILENAME_MAX-1]=0;
      i++;
    }
  }

  /* Close the directory */
  closedir(extdir);
}



/* Execute an extension program, passing it "--cwirc" as first argument, and
   the extension API's shared memory id as second argument. Return NULL or
   an error message. */
char *exec_extension_program(char *fname,int ext_shmid)
{
  static char errmsg[64];
  struct cwirc_extension_api *ext_sharedmem;
  char fullpathname[FILENAME_MAX];
  int pid;

  /* Attach to the extension API's shared memory block */
  if((ext_sharedmem=(struct cwirc_extension_api *)cwirc_shm_attach(ext_shmid))
	==(struct cwirc_extension_api *)-1)
  {
    strcpy(errmsg,"Error : can't attach to the extension API's shared memory.");
    return(errmsg);
  }

  /* If an extension process is already running, abort */
  if(ext_sharedmem->pid!=-1)
  {
    cwirc_shm_detach(ext_sharedmem);
    strcpy(errmsg,"An extension program is already running.");
    return(errmsg);
  }

  /* Spawn the extension program */
  switch((pid=fork()))
  {
  case -1:
    strcpy(errmsg,"Error : fork() : Cannot spawn the extension process.");
    return(errmsg);
    break;

  case 0:		/* I'm the child */
    /* Close the extension program's stdout so it doesn't trickle down to the
       X-Chat window */
    close(1);

    /* Detach from the extension API's shared memory block */
    cwirc_shm_detach(ext_sharedmem);

    /* Execute the extension program */  
    sprintf(errmsg,"0x%0x",ext_shmid);	/* Reuse errmsg to store the shm id */
    sprintf(fullpathname,"%s/%s",EXTENSIONS_DIR,fname);
    execl(fullpathname,fname,"--cwirc",errmsg,NULL);
    fprintf(stderr,"Error : cannot execute \"%s\".\n",fullpathname);
    fflush(stderr);
    sleep(1);	/* Give the parent a change to catch us dying prematurely */
    _exit(0);
    break;
  }

  ext_sharedmem->pid=pid;
  
  /* Detach from the extension API's shared memory block */
  cwirc_shm_detach(ext_sharedmem);

  return(NULL);
}

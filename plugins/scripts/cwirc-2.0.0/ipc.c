/* CWirc - X-Chat plugin for sending and receiving raw morse code over IRC
   (c) Pierre-Philippe Coupard - 18/06/2003

   IPC routines

   This program is distributed under the terms of the GNU General Public License
   See the COPYING file for details
*/
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "ipc.h"



/* Create a semaphore set. */
int cwirc_sem_create(int key,int nb_sems)
{
  struct sembuf sops;
  int semid;
  int i;

  /* Create the semaphore set */
  if((semid=semget(key,nb_sems,IPC_CREAT | 0600))==-1)
    return(-1);

  /* Wait for the semaphore to reach 1 */
  for(i=0;i<nb_sems;i++)
  {
    sops.sem_num=i;
    sops.sem_op=1;		/* wait for semaphore flag to become 1 */
    sops.sem_flg=SEM_UNDO;
    if(semop(semid,&sops,1)==-1)
    {
      semctl(semid,0,IPC_RMID,0);
      semid=-1;
      return(-1);
    }
  }

  return(semid);
}



/* Decrement a semaphore */
int cwirc_sem_dec(int semid,int semnum)
{
  struct sembuf sops;

  sops.sem_num=semnum;
  sops.sem_op=-1;		/* decrement semaphore == P() */
  sops.sem_flg=SEM_UNDO;
  if(semop(semid,&sops,1)==-1)
    return(-1);

  return(0);
}



/* Increment a semaphore */
int cwirc_sem_inc(int semid,int semnum)
{
  struct sembuf sops;

  sops.sem_num=semnum;
  sops.sem_op=1;		/* increment semaphore == V() */
  sops.sem_flg=SEM_UNDO;
  if(semop(semid,&sops,1)==-1)
    return(-1);

  return(0);
}



/* Destroy the semaphore */
int cwirc_sem_destroy(int semid)
{
  if(semid<0 || semctl(semid,0,IPC_RMID,0)==-1)
    return(-1);

  return(0);
}



/* Allocate a shared memory block. Return code :
   shared memory id
   -1 : error allocating the shared memory */
int cwirc_shm_alloc(int key,int size)
{
  return(shmget(key,size,IPC_CREAT | 0600));
}



/* Attach the shared memory block. Return code :
   pointer to the shared memory block
   -1 : error attaching to the shared memory block */
void *cwirc_shm_attach(int shmid)
{
  return(shmat(shmid,NULL,0));
}



/* Detach the shared memory block. */
int cwirc_shm_detach(void *shm)
{
  return(shmdt(shm));
}



/* Free the shared memory block */
int cwirc_shm_free(int shmid)
{
  if(shmid<0 || shmctl(shmid,IPC_RMID,0)==-1)
    return(-1);

  return(0);
}

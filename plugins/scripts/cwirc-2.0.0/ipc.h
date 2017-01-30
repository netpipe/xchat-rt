/* Definitions */
#define SEM_ST			0	/* To access the senders table */
#define SEM_FRONTEND_MSG	1	/* To access the frontend message */
#define SEM_XMIT_BUF		2	/* To access the xmit buffer */
#define SEM_IO_PROCESS_MSG	3	/* To access the I/O process message */
#define SEM_IO_PROCESS_WORKING	4	/* To reconfigure the I/O process
					   safely */
#define SEM_PERSONAL_INFO	5	/* To access the pers. info settings */

#define NB_SEMAPHORES		6

#define cwirc_sem_P		cwirc_sem_dec
#define cwirc_sem_V		cwirc_sem_inc



/* Prototypes */
int cwirc_sem_create(int key,int nb_sems);
int cwirc_sem_dec(int semid,int semnum);
int cwirc_sem_inc(int semid,int semnum);
int cwirc_sem_destroy(int semid);
int cwirc_shm_alloc(int key,int size);
void *cwirc_shm_attach(int shmid);
int cwirc_shm_detach(void *shm);
int cwirc_shm_free(int shmid);

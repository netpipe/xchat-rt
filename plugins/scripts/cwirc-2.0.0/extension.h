/* CWirc extension API */

/* Definitions */
#define MAX_CWIRC_EXTENSIONS	10
#define AUDIOBUF_SIZE		22050	/* Samples */



/* Types */

/* Extension programs must use the API as follows :

   - An extension program synchronizes itself with CWirc by getting audio
     samples from the it. Audio samples are always 44100Hz/16bit/mono.

   - To get audio samples, the program acquires one of the 2 semaphores in the
     semphore set alternatively, read audio samples from the ring buffer,
     adjust the ring buffer's pointers accordingly, then release the semaphore
     it had acquired as quickly as possible. The ring buffer can contain up to
     .5s worth of audio. Once a semaphore is acquired, the extension program is
     guaranteed to have samples to read : CWirc won't release any of the 2
     semaphore unless there's something in the buffer.

   - To provoke a key-down in CWirc, the extension sets in_key to 1. To
     provoke a key-up, the extension sets in_key to 0.

   - The extension may be killed at any time with SIGHUP.
*/

struct cwirc_extension_api
{
  int semid;			/* 2-semaphore set to synchronize extension */
  T_S16 out_audiobuf[AUDIOBUF_SIZE];/* Audio buffer */
  int out_audiobuf_start;
  int out_audiobuf_end;
  int in_key;			/* Input source for extension to key CWirc*/
  int pid;			/* Pid of the extension process. For internal
  				   use only. */
};



/* Variables */
extern char cwirc_extensions[MAX_CWIRC_EXTENSIONS][FILENAME_MAX];



/* Prototypes */
void get_available_cwirc_extensions(void);
char *exec_extension_program(char *fname,int ext_shmid);

/* Definitions */
#define RCFILE			"~/.cwircrc"
#define	MAX_NICK_SIZE		64	/* That should be more than enough */
#define	MAX_GRIDSQUARE_SIZE	7
#define	MAX_CHANNEL_NAME_SIZE	128	/* That should be more than enough */
#define	MAX_SERVER_NAME_SIZE	256	/* That should be more than enough */
#define MAX_EVT_BUFFER		500	/* Nb events we buffer per sender */
#define MAX_SENDERS		10	/* Max. senders we listen to */
#define XMIT_BUF_MAX_SIZE	2048	/* Max. events we send to irc server */
#define DECODED_MSG_SIZE	256	/* Max. chars in the cw decoder buf */
#define FRONTEND_MSG_SIZE	512	/* Max. chars in a frontend err msg */



/* Types */
struct cwirc_cw_sender
{
  char name[MAX_NICK_SIZE];
  double kcdelay[MAX_EVT_BUFFER];
  T_BOOL keystate[MAX_EVT_BUFFER];
  T_BOOL keystate_prev;
  unsigned long long keyup_tickcnt;
  unsigned long long keydown_tickcnt;
  double playback_start_timeout;
  double playback_stop_timeout;
  T_U16 buf_head;
  double signal_strength;		/* 0 -> 1. <1 == undefined */
};

struct cwirc_shm_block
{
  char version[10];		/* Used to detect plugin/frontend mismatch */
  int semid;
  T_BOOL stop_frontend;
  T_BOOL frontend_stopped;
  char frontend_msg[FRONTEND_MSG_SIZE];
  T_BOOL reconfigure_io_process;
  char io_process_msg[FRONTEND_MSG_SIZE];

  char snddev[FILENAME_MAX];
  char serialdev[FILENAME_MAX];

  T_BOOL sidetone_mode;		/* 0==send cw normally, 1==sidetone mode */

  T_BOOL do_mouse_input;
  T_BOOL do_key_input;
  T_BOOL do_snddev_output;
  T_BOOL do_sounder_output;
  T_U8 cwsound;			/* 0==beeps, 1==sounder clicks */
  
  T_U16 cwchannel[5];		/* The preset channels */
  T_U8 currcwchannel;		/* The currently active preset channel (0->4)*/
  T_S8 cwrxpitch;		/* Received CW sound pitch */
  T_S8 cwtxpitch;		/* Transmitted CW sound pitch */
  T_U8 squelch;			/* Squelch level */
  T_U8 volume;			/* Sound volume */

  T_U8 wpm;			/* Words per minute for the iambic keyer */
  T_BOOL doiambic;		/* Iambic or straight key mode */
  T_U8 iambicmode;		/* 0 -> mode-A, 1 -> mode-B */
  T_BOOL do_midelementmodeB;
  T_BOOL do_ditmemory;
  T_BOOL do_dahmemory;
  T_BOOL do_autocharspacing;
  T_BOOL do_autowordspacing;
  T_U8 dit_weight;		/* 15 -> 85 % */
  T_BOOL invertpaddles;		/* 0 = left paddle is dit, right paddle is dah*/

  T_U8 debounce;		/* nb samples to take to read paddles */
  T_S16 recv_buffering;		/* Number of millisec. we buffer incoming cw */

  char callsign[MAX_NICK_SIZE];		/* Operator's callsign */
  char gridsquare[MAX_GRIDSQUARE_SIZE];	/* Operator's location */
  T_BOOL send_callsign_with_cw;	/* 1 = send the callsign along with cw frames */
  T_BOOL send_gridsquare_with_cw;/* 1 = send the location along with cw frames*/

  struct cwirc_cw_sender sender[MAX_SENDERS];
  T_S16 xmit_buf[XMIT_BUF_MAX_SIZE];
  T_U16 xmit_buf_flush_nb_evts;
  T_BOOL mouseinputbutton0;
  T_BOOL mouseinputbutton1;
  T_U8 recv_signal;

  T_U8 cwcodeset;
  char decoded_msg_buf[DECODED_MSG_SIZE];
  char decoded_msg_buf_char_markers[DECODED_MSG_SIZE];
  double decoded_msg_wpm;
  T_BOOL decoded_msg_updated;
  T_BOOL reset_decoder;

  T_BOOL reply_to_ctcp;
  T_BOOL give_callsign_in_ctcp_reply;
  T_BOOL give_gridsquare_in_ctcp_reply;
  T_BOOL give_cwchannel_in_ctcp_reply;

  T_BOOL simulate_qrn;
  T_U8 qrnlevel;

  T_BOOL simulate_signal_strength;
  T_BOOL simulate_sporadicE;
  T_U8 default_signal_strength;
};



/* Variables */
extern struct cwirc_shm_block *sharedmem;

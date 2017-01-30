/* Definitions */
#define EXPLICIT_CALLSIGN_HEADER	"de="
#define GRID_SQUARE_HEADER		"at="
#define CW_FRAME_HEADER_BASEFMT		"cw="
#define CW_FRAME_HEADER_XFMT		"cx="



/* Prototypes */
char *cwirc_encode_cw_frame(void);
int cwirc_decode_cw_frame(char *sender_name,char *frame,char **callsign);
int cwirc_is_cw_frame(char *frame);
int cwirc_decode_encrypted_callsign_in_msg(char **callsign,char **msg);

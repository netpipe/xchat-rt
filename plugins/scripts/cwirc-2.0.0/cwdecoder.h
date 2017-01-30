/* Definitions */
#define UNKNOWN_CHARACTER_SIGN	"_"
#define NB_CW_CODE_SETS		6
#define CW_CODESET_SIZE_MAX	94
#define CW_SEQUENCE_MAX		9
#define CW_SYMBOL_MAX		5
#define LANG_NAME_SIZE_MAX	8
#define MENU_ENTRY_SIZE_MAX	14
/* Code types */
#define NOCODE			0
#define MORSE			1
#define DOT			2
/* Special meanings of the decoded_msg_wpm variable */
#define WPM_UNKNOWN_WPM		-1
#define WPM_DECODER_DISABLED	-2



/* Types */
struct cwsymbol
{
  char sequence[CW_SEQUENCE_MAX+1];
  char symbol[CW_SYMBOL_MAX+1];
};
struct cwcodeset
{
  char lang[LANG_NAME_SIZE_MAX+1];
  char lang_menu_entry[MENU_ENTRY_SIZE_MAX+1];
  char cwcodetype;
  struct cwsymbol cwcode[CW_CODESET_SIZE_MAX];
};



/* Global variables */
extern struct cwcodeset cwirc_cw_table[];



/* Prototypes */
void cwirc_decode_cw(T_BOOL key,double ticklen,int cwcodeset);

#ifdef WIN32
	#define _WIN32_WINNT 0x0400
	#include "resrc1.h"
	#include <windows.h>
	#include <stdio.h>
	BOOL CALLBACK PassFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	#define snprintf _snprintf
#else
	#include "cfgopts.h"
	#include <stdlib.h>
	#include <unistd.h>
	#define ZeroMemory(x,y) memset( &x, 0, y );
	#define stricmp strcasecmp
	#define strnicmp strncasecmp
#endif

#include "DH1080.h"


extern unsigned char iniPath[255], rndBuf[160], rndPath[255];


void initb64();
int decrypt_string(char *key, char *str, char *dest, int len);
int encrypt_string(char *key, char *str, char *dest, int len);

int decrypt_key(char *theData);
int encrypt_key(char *theData);


extern unsigned char B64[];

int b64toh(char *b, char *d);
int htob64(char *h, char *d, unsigned int l);

void SHA256_memory(unsigned char *buf, int len, unsigned char *hash);

//int ExtractRhost(char *Rhost, char *incoming_msg);
int ExtractRnick(char *Rnick, char *incoming_msg);
void FixContactName(char *contactName);

char *strcasestr (const char *, const char *);

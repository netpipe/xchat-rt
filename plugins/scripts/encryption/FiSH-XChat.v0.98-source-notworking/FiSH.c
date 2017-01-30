///****************************
// *** FiSH v0.98 for XChat ***
///****************************


#include "xchat-plugin.h"
#include "FiSH.h"

static xchat_plugin *ph=0;   // plugin handle

unsigned char iniKey[100], default_iniKey[]="blowinikey\0ADDITIONAL SPACE FOR CUSTOM BLOW.INI PASSWORD";
unsigned char g_myPrivKey[300], g_myPubKey[300];
unsigned char iniPath[255], rndBuf[160], rndPath[255], tempPath[255];
int g_doEncrypt=1, g_doDecrypt=1, g_noFormatting=0;

miracl *mip;

#define DEFAULT_FORMAT "\002<\002%s\002>\002\t%s"		// <nick>	message
#define FORMAT_MSG_SEND "\002*\002%s\002*\002\t%s"		// *nick*	message
#define FORMAT_NOTICE_SEND "\002]\002%s\002[\002\t%s"	// ]nick[	notice
#define	unsetiniFlag (void *)0xBEEF



#ifdef WIN32
HINSTANCE g_hInstance;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hInstance=hModule;
			break;
    }
    return TRUE;
}
#endif



// encrypt a message (using key for target contactName)
int FiSH_encrypt(char *msg_ptr, char *target, char *bf_dest)
{
	unsigned char contactName[100]="", theKey[500]="";


	if(msg_ptr==NULL || *msg_ptr=='\0' || target==NULL || *target=='\0' || bf_dest==NULL) return 0;

	if(strlen(target) >= sizeof(contactName)) return 0;
	strcpy(contactName, target);
	FixContactName(contactName);	// replace '[' and ']' with '~' in contact name

	GetPrivateProfileString(contactName, "key", "", theKey, sizeof(theKey), iniPath);
	if(*theKey==0 || strlen(theKey)<4) return 0;		// don't process, key not found in ini

	if(strncmp(theKey, "+OK ", 4)==0)
	{	// key is encrypted, lets decrypt
		decrypt_string((char *)iniKey, theKey+4, theKey, strlen(theKey+4));
		if(*theKey=='\0')
		{	// don't process, decrypted key is bad
			ZeroMemory(theKey, sizeof(theKey));
			return 0;
		}
	}

	encrypt_string(theKey, msg_ptr, bf_dest, strlen(msg_ptr));
	ZeroMemory(theKey, sizeof(theKey));

	bf_dest[512]=0;

	return 166;
}



// decrypt a base64 cipher text (using key for target contactName)
int FiSH_decrypt(char *msg_ptr, char *contactName)
{
	unsigned char theKey[500]="", bf_dest[1500]="", myMark[20]="";
	unsigned int msg_len, i, mark_broken_block=0;


	if(msg_ptr==NULL || *msg_ptr=='\0' || contactName==NULL || *contactName=='\0') return 0;

	FixContactName(contactName);	// replace '[' and ']' with '~' in contact name

	GetPrivateProfileString(contactName, "key", "", theKey, sizeof(theKey), iniPath);
	if(*theKey==0 || strlen(theKey)<4) return 0;	// don't process, key not found in ini
	if(strncmp(theKey, "+OK ", 4)==0)
	{	// key is encrypted, lets decrypt
		decrypt_string((char *)iniKey, theKey+4, theKey, strlen(theKey+4));
		if(*theKey=='\0')
		{	// don't process, decrypted key is bad
			ZeroMemory(theKey, sizeof(theKey));
			return 0;
		}
	}

	// Verify base64 string
	msg_len=strlen(msg_ptr);
	if((strspn(msg_ptr, B64) != msg_len) || (msg_len < 12)) return 0;

	// block-align blowcrypt strings if truncated by IRC server (each block is 12 chars long)
	// such a truncated block is destroyed and not needed anymore
	if(msg_len != (msg_len/12)*12)
	{
		msg_len=(msg_len/12)*12;
		msg_ptr[msg_len]='\0';
		GetPrivateProfileString("FiSH", "mark_broken_block", " \002&\002", myMark, sizeof(myMark), iniPath);
		if(*myMark=='\0' || *myMark=='0' || *myMark=='n' || *myMark=='N') mark_broken_block=0;
		else mark_broken_block=1;
	}

	decrypt_string(theKey, msg_ptr, bf_dest, msg_len);
	ZeroMemory(theKey, sizeof(theKey));
	if(*bf_dest=='\0') return 0;	// don't process, decrypted msg is bad

	i=0;
	while(bf_dest[i] != 0x0A && bf_dest[i] != 0x0D && bf_dest[i] != 0x00) i++;
	bf_dest[i]=0x00;	// in case of wrong key, decrypted message might have control characters -> cut message

	// append broken-block-mark?
	if(mark_broken_block) strcat(bf_dest, myMark);

	strcpy(msg_ptr, bf_dest);	// copy decrypted message back (overwriting the base64 cipher text)
	ZeroMemory(bf_dest, sizeof(bf_dest));

	return 166;
}



int decrypt_incoming(char *word[], char *word_eol[], void *userdata)
{
	unsigned char *msg_ptr, contactName[100]="", from_nick[50], msg_event[100]="", psyNetwork[12];
	unsigned int psylog_found=0, i, u=0;
	xchat_context *find_query_ctx;

	// word[1] = :fromnick!ident@host
	// word[2] = PRIVMSG/NOTICE/TOPIC
	// word[3] = target nick/#channel
	// word[4] = :+OK or :mcps
	// word[5] = BASe.64/STRinG

	if(g_doDecrypt==0 || word[5]==0 || word[5][0]==0) return XCHAT_EAT_NONE;
	if( (strcmp(word[4], ":+OK")!=0) &&
		(strcmp(word[4], ":mcps")!=0) &&
		(strncmp(word[1], ":-psyBNC!", 9)!=0))
		return XCHAT_EAT_NONE;		// prefix/psyBNC not found, don't process

	if(word[1][0] == ':') ExtractRnick(from_nick, word[1]);
	else *from_nick=0;

	msg_ptr = word[5];

	if(word[3][0]=='#' || word[3][0]=='&') strcpy(contactName, word[3]);
	else if(strcmp(from_nick, "-psyBNC")==0)
	{
		// word[8] = :(nick!ident@host.org)
		// word[9] = +OK or mcps
		// word[10] = BASe.64/STRinG
		if(word[10]==0) return XCHAT_EAT_NONE;

		if( (strcmp(word[9], "+OK")!=0) &&
			(strcmp(word[9], "mcps")!=0))
			return XCHAT_EAT_NONE;		// prefix not found, don't process

		// find psyBNC network string
		i=1;
		while(word[4][i]!='~' && word[4][i]!=0 && word[4][i]!=' ') i++;
		ZeroMemory(psyNetwork, sizeof(psyNetwork));
		if(word[4][i]=='~' && i < sizeof(psyNetwork)) strncpy(psyNetwork, word[4]+1, i);

		if(*psyNetwork)
		{
			strcpy(contactName, psyNetwork);
			u = strlen(psyNetwork);
		}

		if(strncmp(word[8], ":(", 2)==0) ExtractRnick(contactName+u, word[8]+2);
		else return XCHAT_EAT_NONE;

		msg_ptr = word[10];
		word[10] = 0;
		word_eol[10] = 0;
		psylog_found=1;
	}
	else strcpy(contactName, from_nick);

	// decrypt the base64 cipher text (using key for target contactName)
	if(FiSH_decrypt(msg_ptr, contactName) == 0) return XCHAT_EAT_NONE;

	// move message pointer to previous argument ('+OK' not needed)
	if(psylog_found)
	{
		word_eol[9] = msg_ptr;
		strcpy(strstr(word_eol[4], "+OK "), msg_ptr);
	}
	else word_eol[4] = msg_ptr;


	if(g_noFormatting==0)
	{
		// let xchat display ACTION/NOTICE/TOPIC messages (without crypt-mark ...)
		if (strncmp(msg_ptr, "\001ACTION ", 8)==0 ||
			strcmp(word[2], "TOPIC")==0 ||
			strcmp(word[2], "NOTICE")==0) return XCHAT_EAT_NONE;

		if(*contactName=='#' || *contactName=='&')
		{
			if(strcasestr(word_eol[4], xchat_get_info(ph, "nick"))!=NULL) return XCHAT_EAT_NONE;	// workaround for hilight
			GetPrivateProfileString("incoming_format", "crypted_chanmsg", DEFAULT_FORMAT, msg_event, sizeof(msg_event), iniPath);
		}
		else
		{
			find_query_ctx = xchat_find_context(ph, NULL, from_nick);
			if(find_query_ctx==NULL)
			{	// no query window yet, lets open one
				xchat_commandf(ph, "query %s", from_nick);
				find_query_ctx = xchat_find_context(ph, NULL, from_nick);
			}
			xchat_set_context(ph, find_query_ctx);

			GetPrivateProfileString("incoming_format", "crypted_privmsg", DEFAULT_FORMAT, msg_event, sizeof(msg_event), iniPath);
		}

		// display formatted nick and decrypted message
		xchat_printf(ph, msg_event, from_nick, word_eol[4]+psylog_found);

		return XCHAT_EAT_XCHAT;
	}
	else return XCHAT_EAT_NONE;
}



int encrypt_outgoing(char *word[], char *word_eol[], void *userdata)
{
	unsigned char bf_dest[2000]="", new_msg[600]="";
	unsigned char ini_tmp_buf[20]="";
	unsigned int pp;
	const char *contactPtr, *own_nick;


	if (g_doEncrypt==0 || word_eol[1]==0 || word_eol[1][0]==0) return XCHAT_EAT_NONE;		// don't process

	contactPtr = xchat_get_info(ph, "channel");
	own_nick = xchat_get_info(ph, "nick");

	// plain-prefix in msg found?
	GetPrivateProfileString("FiSH", "plain_prefix", "+p ", ini_tmp_buf, sizeof(ini_tmp_buf), iniPath);
	pp=strlen(ini_tmp_buf);
	if(strnicmp(word_eol[1], ini_tmp_buf, pp)==0)
	{
		snprintf(new_msg, 511, "PRIVMSG %s :%s", contactPtr, word_eol[1]+pp);
	}
	else
	{
		pp=0;

		// encrypt a message (using key for target)
		if(FiSH_encrypt(word_eol[1], (char *)contactPtr, bf_dest) == 0) return XCHAT_EAT_NONE;

		snprintf(new_msg, 511, "PRIVMSG %s :+OK %s\n", contactPtr, bf_dest);
	}

	// display formatted (plain-text) message in local IRC client
	if(pp) xchat_emit_print(ph, "Your Message", own_nick, word_eol[1]+pp, NULL, NULL);
	else
	{
		if(*contactPtr=='#' || *contactPtr=='&') GetPrivateProfileString("outgoing_format", "crypted_chanmsg", DEFAULT_FORMAT, bf_dest, sizeof(bf_dest), iniPath);
		else GetPrivateProfileString("outgoing_format", "crypted_privmsg", DEFAULT_FORMAT, bf_dest, sizeof(bf_dest), iniPath);

		xchat_printf(ph, bf_dest, own_nick, word_eol[1]);
	}

	ZeroMemory(bf_dest, sizeof(bf_dest));

	xchat_command(ph, new_msg);	// send message to IRC server
	return XCHAT_EAT_XCHAT;
}



int decrypt_topic_332(char *word[], char *word_eol[], void *userdata)
{
	unsigned char contactName[100]="";
	unsigned char *msg_ptr;

	// word[1] = :irc.server.com
	// word[2] = 332
	// word[3] = own_nick
	// word[4] = target #channel
	// word[5] = :+OK or :mcps
	// word[6] = BASe.64/T0PiC.STRinG

	if(g_doDecrypt==0 || word[6]==0 || word[6][0]==0) return XCHAT_EAT_NONE;
	if( (strcmp(word[5], ":+OK")!=0) &&
		(strcmp(word[5], ":mcps")!=0) &&
		(word[4][0] != '#') &&
		(word[4][0] != '&'))
		return XCHAT_EAT_NONE;		// prefix not found/not a channel, don't process

	msg_ptr = word[6];

	strcpy(contactName, word[4]);

	// decrypt the base64 cipher text (using key for target contactName)
	if(FiSH_decrypt(msg_ptr, contactName) == 0) return XCHAT_EAT_NONE;

	// move message pointer to previous argument ('+OK' not needed)
	word_eol[5] = msg_ptr;

	return XCHAT_EAT_NONE;
}



// copy key for old nick to use with the new one
int nick_changed(char *word[], char *word_eol[], void *userdata)
{
	unsigned char contactName[100]="", theKey[500]="", ini_nicktracker[10];

	// word[1] = :user@host.com
	// word[2] = NICK
	// word[3] = :newNick

	if(word[3]==0 || word[3][1]=='\0') return XCHAT_EAT_NONE;

	GetPrivateProfileString("FiSH", "nicktracker", "1", ini_nicktracker, sizeof(ini_nicktracker), iniPath);

	if(	*ini_nicktracker=='0' || *ini_nicktracker=='N' || *ini_nicktracker=='n' ||
		(ExtractRnick(contactName, word[1])==0) ||
		(stricmp(contactName, word[3]+1)==0))
		return XCHAT_EAT_NONE;

	// process only if a query is open
	if(xchat_find_context(ph, NULL, contactName)==0) return XCHAT_EAT_NONE;

	FixContactName(contactName);	// replace '[' and ']' with '~' in contact name

	GetPrivateProfileString(contactName, "key", "", theKey, sizeof(theKey), iniPath);
	if(strlen(theKey) < 4) return XCHAT_EAT_NONE;	// don't process, key not found in ini

	strcpy(contactName, word[3]+1);
	FixContactName(contactName);

	WritePrivateProfileString(contactName, "key", theKey, iniPath);

	ZeroMemory(theKey, sizeof(theKey));

	return XCHAT_EAT_NONE;
}




// Display on channel-join if chat will be encrypted or not
int print_onjoin(char *word[], void *userdata)
{
	unsigned char contactName[100]="", theKey[500]="";
	const char *target=word[2];

	if(target==0 || *target==0) return XCHAT_EAT_NONE;

	if(strlen(target) >= sizeof(contactName)) return XCHAT_EAT_NONE;
	strcpy(contactName, target);
	FixContactName(contactName); // replace '[' and ']' with '~' in contact name

	GetPrivateProfileString(contactName, "key", "", theKey, sizeof(theKey), iniPath);

	if(*theKey==0 || strlen(theKey)<4)
		xchat_printf(ph,"\002FiSH:\002 Chat in %s will \002NOT\002 be encrypted (no key found)", target);
	else
		xchat_printf(ph,"\002FiSH:\002 Chat in %s will be encrypted", target);

	ZeroMemory(theKey, sizeof(theKey));
	return XCHAT_EAT_NONE;
}



// Set a custom blow.ini password, xchat syntax: /setinipw <sekure_blow.ini_password>
int command_setinipw(char *word[], char *word_eol[], void *userdata)
{
	unsigned int i=0, pw_len, re_enc=0;
	unsigned char B64digest[50], SHA256digest[35];
	unsigned char bfKey[500], old_iniKey[100], *fptr, *ok_ptr, line_buf[1000], iniPath_new[300];
	FILE *h_ini=NULL, *h_ini_new=NULL;

	//char *cmd = word[1];
	char *iniPW = word[2];


	if(*iniKey==0)
    {
		xchat_printf(ph, "\002FiSH:\002 You didn't set your current blow.ini password. Use \002/fishpw <password>\002 or \002/load /path/xfish.so <password>\002 first.");
		return XCHAT_EAT_ALL;
    }

	pw_len=strlen(iniPW);
	if(pw_len < 7)
    {
		xchat_printf(ph, "\002FiSH:\002 Password too short, at least 7 characters needed! Usage: /setinipw <sekure_blow.ini_password>");
		return XCHAT_EAT_ALL;
    }

	SHA256_memory(iniPW, pw_len, SHA256digest);
	for(i=0;i<40872;i++) SHA256_memory(SHA256digest, 32, SHA256digest);
	htob64(SHA256digest, B64digest, 32);
	strcpy(old_iniKey, iniKey);
	if(userdata == unsetiniFlag) strcpy(iniKey, default_iniKey);	// unsetinipw
	else
	{
		memset(iniPW, ' ', pw_len);
		strcpy(iniKey, B64digest);	// this is used for encrypting blow.ini
	}
	for(i=0;i<30752;i++) SHA256_memory(SHA256digest, 32, SHA256digest);
	htob64(SHA256digest, B64digest, 32);	// this is used to verify the entered password
	ZeroMemory(SHA256digest, sizeof(SHA256digest));


	// re-encrypt blow.ini with new password
	strcpy(iniPath_new, iniPath);
	strcat(iniPath_new, "_new");
	h_ini=fopen(iniPath,"r");
	if(h_ini) h_ini_new=fopen(iniPath_new, "w");
	if(h_ini_new)
	{
		do
		{
			fptr=fgets(line_buf,sizeof(line_buf)-2,h_ini);
			if(fptr)
			{
				ok_ptr=strstr(line_buf, "+OK ");
				if(ok_ptr)
				{
					re_enc=1;
					strtok(ok_ptr+4," \n\r");
					decrypt_string(old_iniKey, ok_ptr+4, bfKey, strlen(ok_ptr+4));
					memset(ok_ptr+4, 0, strlen(ok_ptr+4)+1);
					encrypt_string(iniKey, bfKey, ok_ptr+4, strlen(bfKey));
					strcat(line_buf, "\n");
				}
				fprintf(h_ini_new,"%s",line_buf);
			}
		}
		while (!feof(h_ini));

		ZeroMemory(bfKey, sizeof(bfKey));
		ZeroMemory(line_buf, sizeof(line_buf));
		ZeroMemory(old_iniKey, sizeof(old_iniKey));
		fclose(h_ini);
		fclose(h_ini_new);
		remove(iniPath);
		rename(iniPath_new, iniPath);
	}
	else
	{
		ZeroMemory(bfKey, sizeof(bfKey));
		ZeroMemory(old_iniKey, sizeof(old_iniKey));
		return XCHAT_EAT_ALL;
	}

	WritePrivateProfileString("FiSH", "ini_password_Hash", B64digest, iniPath);

	ZeroMemory(B64digest, sizeof(B64digest));

	if(re_enc) xchat_printf(ph, "\002FiSH: Re-encrypted blow.ini\002 with new password.");

	if(userdata != unsetiniFlag) xchat_printf(ph, "\002FiSH:\002 blow.ini password hash saved.");
#ifndef WIN32
	if(userdata != unsetiniFlag)
	{
		xchat_print(ph, "\002FiSH:\002 You could use \002/load /path/to/fish.so <password>\002 next time to load FiSH and forward your blow.ini password as argument.");
		xchat_print(ph, "\002FiSH:\002 Or you enter your blow.ini password using \002/fishpw <password>\002 after loading FiSH...");
	}
#endif

	return XCHAT_EAT_ALL;
}



// Change back to default blow.ini password, xchat syntax: /unsetinipw
int command_unsetinipw(char *word[], char *word_eol[], void *userdata)
{
	word[2] = "Some_boogie_dummy_key";

	command_setinipw(word, word_eol, unsetiniFlag);

	if(*iniKey)
	{
		WritePrivateProfileString("FiSH", "ini_password_Hash", "0", iniPath);
		xchat_print(ph, "\002FiSH:\002 Changed back to default blow.ini password, you won't have to enter it on start-up anymore.");
	}

	return XCHAT_EAT_ALL;
}



#ifndef WIN32
int command_fishpw(char *word[], char *word_eol[], void *userdata)
{
	unsigned int i=0, pw_len;
	unsigned char iniPasswordHash[50], B64digest[50], SHA256digest[35];

	//char *cmd = word[1];
	char *iniPW = word[2];

	pw_len=strlen(iniPW);
	if(pw_len < 7)
    {
		xchat_printf(ph, "\002FiSH:\002 Password too short, at least 7 characters needed! Usage: /fishpw <password>");
		return XCHAT_EAT_ALL;
    }

	GetPrivateProfileString("FiSH", "ini_Password_hash", "0", iniPasswordHash, sizeof(iniPasswordHash), iniPath);
	if(strlen(iniPasswordHash) == 43)
	{
		SHA256_memory(iniPW, pw_len, SHA256digest);
		memset(iniPW, ' ', pw_len);
		for(i=0;i<40872;i++) SHA256_memory(SHA256digest, 32, SHA256digest);
		htob64(SHA256digest, B64digest, 32);
		strcpy(iniKey, B64digest);      // this is used for encrypting blow.ini
		for(i=0;i<30752;i++) SHA256_memory(SHA256digest, 32, SHA256digest);
		htob64(SHA256digest, B64digest, 32);	// this is used to verify the entered password
		if(strcmp(B64digest, iniPasswordHash) != 0)
		{
			xchat_print(ph, "\002FiSH:\002 Wrong blow.ini password entered, try again...\n");
			*iniKey=0;
			return XCHAT_EAT_ALL;
		}
		xchat_print(ph, "\002FiSH:\002 Correct blow.ini password entered, lets go!\n");

		ZeroMemory(SHA256digest, sizeof(SHA256digest));
		ZeroMemory(B64digest, sizeof(B64digest));
	}
	else xchat_print(ph, "\002FiSH:\002 ERROR: Invalid ini_Password_hash in blow.ini found!\n");

	return XCHAT_EAT_ALL;
}
#endif



// xchat syntax: /setkey [<nick/#channel>] <sekure_key>
int command_setkey(char *word[], char *word_eol[], void *userdata)
{
	unsigned char contactName[100]="", encryptedKey[500]="";
	const char *getinfoPtr;

	//char *cmd = word[1];
	char *target = word[2];
	char *key = word[3];


	if(target==0 || *target==0)
	{
		xchat_printf(ph, "\002FiSH:\002 No parameters. Usage: /setkey [<nick/#channel>] <sekure_key>");
		return XCHAT_EAT_ALL;
	}

	if (key==0 || *key==0)
	{
		// only one paramter given - it's the key
		key = target;
		target = (char *)xchat_get_info(ph, "channel");
		getinfoPtr=xchat_get_info(ph, "network");
		if(target==0 || (getinfoPtr!=0 && stricmp(target, getinfoPtr)==0))
		{
			xchat_printf(ph, "\002FiSH:\002 Please define nick/#channel. Usage: /setkey [<nick/#channel>] <sekure_key>");
			return XCHAT_EAT_ALL;
		}
	}

	if(strlen(target) >= sizeof(contactName)) return XCHAT_EAT_NONE;

	strcpy(contactName, target);
	FixContactName(contactName);	// replace '[' and ']' with '~' in contact name

	strcpy(encryptedKey, key);
	memset(key, ' ', strlen(key));
	encrypt_key(encryptedKey);

	WritePrivateProfileString(contactName, "key", encryptedKey, iniPath);

	ZeroMemory(encryptedKey, sizeof(encryptedKey));

	xchat_printf(ph, "\002FiSH:\002 Key for %s successfully set!", target);
	return XCHAT_EAT_ALL;
}



// xchat syntax: /delkey <nick/#channel
int command_delkey(char *word[], char *word_eol[], void *userdata)
{
	unsigned char contactName[100]="";

	//char *cmd = word[1];
	char *target = word[2];


	if(target==0 || *target==0)
	{
		xchat_printf(ph, "\002FiSH:\002 No parameters. Usage: /delkey <nick/#channel>");
		return XCHAT_EAT_ALL;
	}

	if(strlen(target) >= sizeof(contactName)) return XCHAT_EAT_NONE;

	strcpy(contactName, target);
	FixContactName(contactName);	// replace '[' and ']' with '~' in contact name

	WritePrivateProfileString(contactName, "key", "0\r\n", iniPath);

	xchat_printf(ph, "\002FiSH:\002 Key for %s successfully removed!", target);
	return XCHAT_EAT_ALL;
}



// display key, xchat syntax: /key [<nick/#channel>]
int command_key(char *word[], char *word_eol[], void *userdata)
{
	unsigned char contactName[100]="", theKey[500]="";
	const char *getinfoPtr;

	//char *cmd = word[1];
	char *target = word[2];


	if(target==0 || *target==0)
	{
		// no paramter given - try current window
		target = (char *)xchat_get_info(ph, "channel");
		getinfoPtr=xchat_get_info(ph, "network");
		if(target==0 || (getinfoPtr!=0 && stricmp(target, getinfoPtr)==0))
		{
			xchat_printf(ph, "\002FiSH:\002 Please define nick/#channel. Usage: /key <nick/#channel>");
			return XCHAT_EAT_ALL;
		}
	}

	if(strlen(target) >= sizeof(contactName)) return XCHAT_EAT_NONE;

	strcpy(contactName, target);
	FixContactName(contactName);	// replace '[' and ']' with '~' in contact name

	GetPrivateProfileString(contactName, "key", "", theKey, sizeof(theKey), iniPath);
	if(*theKey=='\0' || strlen(theKey)<4)
	{	// don't process, key not found in ini
		xchat_printf(ph, "\002FiSH:\002 Key for %s not found!", target);
		return XCHAT_EAT_ALL;
	}

	if(strncmp(theKey, "+OK ", 4)==0)
	{       // key is encrypted, lets decrypt
		decrypt_string((char *)iniKey, theKey+4, theKey, strlen(theKey+4));
		if(*theKey=='\0')
		{	// don't process, decrypted key is bad
			xchat_printf(ph, "\002FiSH:\002 Key for %s invalid!", target);
			return XCHAT_EAT_ALL;
		}
	}

	xchat_printf(ph, "\002FiSH:\002 Key for %s: %s", target, theKey);
	ZeroMemory(theKey, sizeof(theKey));
	return XCHAT_EAT_ALL;
}




// xchat syntax: /encrypt [<1/on|0/off>]
int command_encrypt(char *word[], char *word_eol[], void *userdata)
{
	char *param=word[2];


	if(param==0 || *param==0)
	{
		if(g_doEncrypt==1) xchat_printf(ph, "\002FiSH:\002 Encryption is \002ON\002");
		else xchat_printf(ph, "\002FiSH:\002 Encryption is \002OFF\002");
	}
	else if(stricmp(param,"on")==0 || *param=='1' || *param=='y' || *param=='Y')
	{
		xchat_printf(ph, "\002FiSH:\002 Set encryption to \002ON\002.");
		g_doEncrypt=1;
		WritePrivateProfileString("FiSH", "process_outgoing", "1", iniPath);
	}
	else if(stricmp(param,"off")==0 || *param=='0' || *param=='n' || *param=='N')
	{
		xchat_printf(ph, "\002FiSH:\002 Set encryption to \002OFF\002.");
		g_doEncrypt=0;
		WritePrivateProfileString("FiSH", "process_outgoing", "0", iniPath);
	}

	return XCHAT_EAT_ALL;
}



// xchat syntax: /decrypt [<1/on|0/off>]
int command_decrypt(char *word[], char *word_eol[], void *userdata)
{
	char *param=word[2];


	if(param==0 || *param==0)
	{
		if(g_doDecrypt==1) xchat_printf(ph, "\002FiSH:\002 Decryption is \002ON\002");
		else xchat_printf(ph, "\002FiSH:\002 Decryption is \002OFF\002");
	}
	else if(stricmp(param,"on")==0 || *param=='1' || *param=='y' || *param=='Y')
	{
		xchat_printf(ph, "\002FiSH:\002 Set decryption to \002ON\002.");
		g_doDecrypt=1;
		WritePrivateProfileString("FiSH", "process_incoming", "1", iniPath);
	}
	else if(stricmp(param,"off")==0 || *param=='0' || *param=='n' || *param=='N')
	{
		xchat_printf(ph, "\002FiSH:\002 Set decryption to \002OFF\002.");
		g_doDecrypt=0;
		WritePrivateProfileString("FiSH", "process_incoming", "0", iniPath);
	}

	return XCHAT_EAT_ALL;
}




// set encrypted topic for current channel, xchat syntax: /topic+ <your topic>
int command_crypt_TOPIC(char *word[], char *word_eol[], void *userdata)
{
	unsigned char *target, bf_dest[2000]="";

	//char *cmd = word[1];
	char *topic = word_eol[2];


	if(topic==0 || *topic==0)
	{
		xchat_printf(ph, "\002FiSH:\002 No parameters. Usage: /topic+ <your new topic>");
		return XCHAT_EAT_ALL;
	}

	target = (char *)xchat_get_info(ph, "channel");
	if(target==0 || (*target!='#' && *target!='&'))
	{
		xchat_printf(ph, "\002FiSH:\002 Please change to the channel window where you want to set the topic!");
		return XCHAT_EAT_ALL;
	}

	// encrypt a message (using key for target)
	if(FiSH_encrypt(topic, target, bf_dest) == 0)
	{
		xchat_printf(ph, "\002FiSH:\002 /topic+ error, no key found for %s. Usage: /topic+ <your new topic>", target);
		return XCHAT_EAT_ALL;
	}

	xchat_commandf(ph, "TOPIC %s +OK %s\n", target, bf_dest);
	ZeroMemory(bf_dest, sizeof(bf_dest));

	return XCHAT_EAT_ALL;
}



// send an encrypted NOTICE message, xchat syntax: /notice+ <nick/#channel> <your notice>
int command_crypt_NOTICE(char *word[], char *word_eol[], void *userdata)
{
	unsigned char bf_dest[2000]="";

	//char *cmd = word[1];
	char *target = word[2];
	char *notice = word_eol[3];


	if (target==0 || *target==0 || notice==0 || *notice==0)
	{
		xchat_printf(ph, "\002FiSH:\002 Bad parameters. Usage: /notice+ <nick/#channel> <your notice>");
		return XCHAT_EAT_ALL;
	}

	// encrypt a message (using key for target)
	if(FiSH_encrypt(notice, target, bf_dest) == 0)
	{
		xchat_printf(ph, "\002FiSH:\002 /notice+ error, no key found for %s. Usage: /notice+ <nick/#channel> <your notice>", target);
		return XCHAT_EAT_ALL;
	}

	xchat_commandf(ph, "quote NOTICE %s :+OK %s", target, bf_dest);
	ZeroMemory(bf_dest, sizeof(bf_dest));

	xchat_printf(ph, FORMAT_NOTICE_SEND, target, notice);	// display notice in current window
	return XCHAT_EAT_ALL;
}



// send an encrypted message, xchat syntax: /msg+ <nick/#channel> <your message>
int command_crypt_MSG(char *word[], char *word_eol[], void *userdata)
{
	unsigned char bf_dest[2000]="";
	xchat_context *find_query_ctx;

	char *target = word[2];
	char *message = word_eol[3];

	if (target==0 || *target==0 || message==0 || *message==0)
	{
		xchat_printf(ph, "\002FiSH:\002 Bad parameters. Usage: /msg+ <nick/#channel> <your message>");
		return XCHAT_EAT_ALL;
	}

	// encrypt a message (using key for target)
	if(FiSH_encrypt(message, target, bf_dest) == 0)
	{
		xchat_printf(ph, "\002FiSH:\002 /msg+ error, no key found for %s. Usage: /msg+ <nick/#channel> <your message>", target);
		return XCHAT_EAT_ALL;
	}

	xchat_commandf(ph, "PRIVMSG %s :+OK %s", target, bf_dest);

	find_query_ctx = xchat_find_context(ph, NULL, target);
	if(find_query_ctx)
	{	// open query/channel window found, display the event there
		xchat_set_context(ph, find_query_ctx);
		if(*target!='#' && *target!='&') GetPrivateProfileString("outgoing_format", "crypted_privmsg", DEFAULT_FORMAT, bf_dest, sizeof(bf_dest), iniPath);
		else GetPrivateProfileString("outgoing_format", "crypted_chanmsg", DEFAULT_FORMAT, bf_dest, sizeof(bf_dest), iniPath);
		xchat_printf(ph, bf_dest, xchat_get_info(ph, "nick"), message);
	}
	else xchat_printf(ph, FORMAT_MSG_SEND, target, message);	// no open window found, display in current window

	ZeroMemory(bf_dest, sizeof(bf_dest));
	return XCHAT_EAT_ALL;
}



// Start a DH1080 key-exchange, xchat syntax: /keyx [<nick>]
int command_keyx(char *word[], char *word_eol[], void *userdata)
{
	//char *cmd = word[1];
	char *getinfoPtr, *target = word[2];
	xchat_context *find_query_ctx;


	if(target==0 || *target==0)
	{
		// no paramter given - try current window
		target = (char *)xchat_get_info(ph, "channel");
		getinfoPtr=xchat_get_info(ph, "network");
		if(target==0 || (getinfoPtr!=0 && stricmp(target, getinfoPtr)==0))
		{
			xchat_printf(ph, "\002FiSH:\002 Please define target nick. Usage: /keyx <nick>");
			return XCHAT_EAT_ALL;
		}
	}

	if(*target=='#' || *target=='&')
	{
		xchat_printf(ph, "\002FiSH:\002 KeyXchange does not work for channels!");
		return XCHAT_EAT_ALL;
	}


	if(DH1080_gen(g_myPrivKey, g_myPubKey) != 1)
	{
		xchat_printf(ph, "\002FiSH:\002 KeyXchange \002failed\002! Error in DH1080_gen :(");
		return XCHAT_EAT_ALL;
	}


	xchat_commandf(ph, "quote NOTICE %s :DH1080_INIT %s", target, g_myPubKey);	// send DH1080 init to target

	// if a query is already open, display info text there
	find_query_ctx = xchat_find_context(ph, NULL, target);
	if(find_query_ctx != 0) xchat_set_context(ph, find_query_ctx);

	xchat_printf(ph, "\002FiSH:\002 Sent my DH1080 public key to %s, waiting for reply ...", target);
	return XCHAT_EAT_ALL;
}



// DH1080 notice handling
int notice_received(char *word[], char *word_eol[], void *userdata)
{
	unsigned int i;
	unsigned char hisPubKey[300], contactName[25]="", from_nick[25]="";
	xchat_context *find_query_ctx;

	//word[1];	// :nick!ident@host.com
	//char *cmd = word[2];
	//const char *target = word[3];	// target nick or #channel
	char *DH_msg = word[4];
	char *DH_pubkey = word[5];

	if(	word[5]==NULL || *word[5]=='\0' || *word[4]=='\0' ||
		*word[3]=='\0' || *word[1]=='\0') return XCHAT_EAT_NONE;

	if(strcmp(word[4], ":+OK")==0 || strcmp(word[4], ":mcps")==0)
		// encrypted notice message?
		return decrypt_incoming(word, word_eol, userdata);

	if(ExtractRnick(from_nick, word[1])==0) return XCHAT_EAT_NONE;
	i=strlen(DH_pubkey);
	if(i<179 || i>181) return XCHAT_EAT_NONE;


	// if a query is already open, display info text there
	find_query_ctx = xchat_find_context(ph, NULL, from_nick);
	if(find_query_ctx) xchat_set_context(ph, find_query_ctx);

	if(strncmp(DH_msg, ":DH1080_INIT", 12)==0)
	{
		xchat_printf(ph, "\002FiSH:\002 Received DH1080 public key from %s, sending mine...", from_nick);

		if(DH1080_gen(g_myPrivKey, g_myPubKey) != 1)
		{
			xchat_printf(ph, "\002FiSH:\002 KeyXchange \002failed\002! Error in DH1080_gen :(");
			return XCHAT_EAT_ALL;
		}
		xchat_commandf(ph, "quote NOTICE %s :DH1080_FINISH %s", from_nick, g_myPubKey);	// send DH1080_FINISH (own pubkey) to from_nick
	}
	else if(strncmp(DH_msg, ":DH1080_FINISH", 14)!=0) return XCHAT_EAT_NONE;

	strcpy(hisPubKey, DH_pubkey);
	if(DH1080_comp(g_myPrivKey, hisPubKey)==0) return XCHAT_EAT_NONE;

	strcpy(contactName, from_nick);
	FixContactName(contactName);	// replace '[' and ']' with '~' in contact name
	encrypt_key(hisPubKey);
	WritePrivateProfileString(contactName, "key", hisPubKey, iniPath);

	ZeroMemory(hisPubKey, sizeof(hisPubKey));

	xchat_printf(ph, "\002FiSH:\002 Key for %s successfully set!", from_nick);
	return XCHAT_EAT_ALL;
}



int xchat_plugin_init(xchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg)
{
	unsigned char iniPasswordHash[50], SHA256digest[35], tmpBuf[50];
	const char *getinfoPtr;
	unsigned int i;
	FILE *hRnd;


	if(ph != 0)
	{
		xchat_print (ph, "\002FiSH is already loaded!\002\n");
		return FALSE;
	}

	// we need to save this for use with any xchat_* functions
	ph = plugin_handle;

	*plugin_name = "FiSH";
	*plugin_desc = "Blowfish IRC encryption, including secure Diffie-Hellman 1080 bit key-exchange";
	*plugin_version = "0.98";

	initb64();
	mip=mirsys(256, 0);

	// path to xchat config file
#ifdef WIN32
	if((getinfoPtr=xchat_get_info(ph, "xchatdirfs"))==NULL) getinfoPtr=xchat_get_info(ph, "xchatdir");
#else
	getinfoPtr=getenv("HOME");
#endif
	if(getinfoPtr==NULL || mip==NULL) return FALSE;

	strcpy(iniPath, getinfoPtr);
	strcpy(rndPath, iniPath);
#ifdef WIN32
	strcat(iniPath, "\\blow.ini");
	strcat(rndPath, "\\random.ECL");
	hCryptProv=0;
#else
	strcat(rndPath, "/.xchat2/random.ECL");
	strcat(iniPath, "/.xchat2/blow.ini");
	strcpy(tempPath, getinfoPtr);
	strcat(tempPath, "/.xchat2/temp_FiSH.$$$");
#endif


	GetPrivateProfileString("FiSH", "ini_Password_hash", "0", iniPasswordHash, sizeof(iniPasswordHash), iniPath);
	if(strlen(iniPasswordHash) == 43)
	{

#ifdef WIN32
		if(DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_PASS), NULL, (DLGPROC)PassFunc)==0)
		{
			xchat_print(ph, "\002FiSH:\002 No blow.ini password entered, try again...\n");
			xchat_print(ph, "\002FiSH not loaded.\002\n");
			return 0;
		}
#else
		if(arg==0 || *arg==0)
		{
			xchat_print(ph, "\002FiSH:\002 Please enter your blow.ini password using \002/fishpw <password>\002\n");
			xchat_print(ph, "\002FiSH:\002 You could also use \002/load /path/to/xfish.so <password>\002 to load FiSH and forward your blow.ini password as argument.\n");
			*iniKey=0;
		}
		else strcpy(iniKey, arg);	// blow.ini password forwarded as argument
#endif

		if(*iniKey)
		{
			SHA256_memory(iniKey, strlen(iniKey), SHA256digest);
			for(i=0;i<40872;i++) SHA256_memory(SHA256digest, 32, SHA256digest);
			htob64(SHA256digest, tmpBuf, 32);
			strcpy(iniKey, tmpBuf);      // this is used for encrypting blow.ini
			for(i=0;i<30752;i++) SHA256_memory(SHA256digest, 32, SHA256digest);
			htob64(SHA256digest, tmpBuf, 32);	// this is used to verify the entered password
			if(strcmp(tmpBuf, iniPasswordHash) != 0)
			{
				xchat_print(ph, "\002FiSH:\002 Wrong blow.ini password entered, try again...\n");
				xchat_print(ph, "\002FiSH not loaded.\002\n");
				return 0;
			}
			xchat_print(ph, "\002FiSH:\002 Correct blow.ini password entered, lets go!\n");
		}
	}
	else
	{
		strcpy(iniKey, default_iniKey);
		xchat_print(ph, "\002FiSH:\002 Using default password to decrypt blow.ini... Try /setinipw to set a custom password.\n");
	}


	if(hRnd=fopen(rndPath, "rb"))
	{
		fread(rndBuf, sizeof(rndBuf), 1, hRnd);
		fclose(hRnd);
	}


	GetPrivateProfileString("FiSH", "process_incoming", "1", tmpBuf, sizeof(tmpBuf), iniPath);
	if(*tmpBuf=='0' || *tmpBuf=='n' || *tmpBuf=='N') g_doDecrypt=0;

	GetPrivateProfileString("FiSH", "process_outgoing", "1", tmpBuf, sizeof(tmpBuf), iniPath);
	if(*tmpBuf=='0' || *tmpBuf=='n' || *tmpBuf=='N') g_doEncrypt=0;

	GetPrivateProfileString("FiSH", "no_formatting", "0", tmpBuf, sizeof(tmpBuf), iniPath);
	if(*tmpBuf=='1' || *tmpBuf=='y' || *tmpBuf=='Y') g_noFormatting=1;


	xchat_hook_server(ph, "PRIVMSG", XCHAT_PRI_NORM, decrypt_incoming, 0);
	xchat_hook_server(ph, "NOTICE", XCHAT_PRI_NORM, notice_received, 0);
	xchat_hook_server(ph, "TOPIC", XCHAT_PRI_NORM, decrypt_incoming, 0);
	xchat_hook_server(ph, "NICK", XCHAT_PRI_NORM, nick_changed, 0);
	xchat_hook_server(ph, "332", XCHAT_PRI_NORM, decrypt_topic_332, 0);
	xchat_hook_command(ph, "", XCHAT_PRI_NORM, encrypt_outgoing, 0, 0);

	xchat_hook_command(ph, "setkey", XCHAT_PRI_NORM, command_setkey, "Set key for target to sekure_key. If no target specified, the key for current window will be set to sekure_key. Usage: /setkey [<nick/#channel>] <sekure_key>", NULL);
	xchat_hook_command(ph, "delkey", XCHAT_PRI_NORM, command_delkey, "Delete key for target. You have to specify the target. Usage: /delkey <nick/#channel>", 0);
	xchat_hook_command(ph, "key", XCHAT_PRI_NORM, command_key, "Show key for target. If no target specified, the key for current window will be shown.\nUsage: /key [<nick/#channel>]", 0);
	xchat_hook_command(ph, "keyx", XCHAT_PRI_NORM, command_keyx, "Perform DH1080 KeyXchange with target. If no target specified, the KeyXchange takes place with the current query window. Usage: /keyx [<nick>]", 0);
	xchat_hook_command(ph, "setinipw", XCHAT_PRI_NORM, command_setinipw, "Set a custom password to encrypt your key-container (blow.ini) - you will be forced to enter it each time you load the module.\nUsage: /setinipw <sekure_blow.ini_password>", 0);
	xchat_hook_command(ph, "unsetinipw", XCHAT_PRI_NORM, command_unsetinipw, "Change back to default blow.ini password. Usage: /unsetinipw", 0);
	xchat_hook_command(ph, "topic+", XCHAT_PRI_NORM, command_crypt_TOPIC, "Set a new encrypted topic for the current channel. Usage: /topic+ <your topic>", 0);
	xchat_hook_command(ph, "notice+", XCHAT_PRI_NORM, command_crypt_NOTICE, "Send an encrypted notice. Usage: /notice+ <nick/#channel> <your notice>", 0);
	xchat_hook_command(ph, "msg+", XCHAT_PRI_NORM, command_crypt_MSG, "Send an encrypted message. Usage: /msg+ <nick/#channel> <your message>", 0);
	xchat_hook_command(ph, "encrypt", XCHAT_PRI_NORM, command_encrypt, "Enable or disable FiSH encryption. Usage: /encrypt [< 1/y/on | 0/n/off >]", 0);
	xchat_hook_command(ph, "decrypt", XCHAT_PRI_NORM, command_decrypt, "Enable or disable FiSH decryption. Usage: /decrypt [< 1/y/on | 0/n/off >]", 0);

	GetPrivateProfileString("FiSH", "print_onjoin", "1", tmpBuf, sizeof(tmpBuf), iniPath);
	if(*tmpBuf=='1' || *tmpBuf=='y' || *tmpBuf=='Y') xchat_hook_print(ph, "You Join", XCHAT_PRI_NORM, print_onjoin, 0);

#ifndef WIN32
	// only needed if blow.ini password was not forwarded using /load command
	if(*iniKey==0) xchat_hook_command(ph, "fishpw", XCHAT_PRI_NORM, command_fishpw, "Set FiSH password to decrypt your key-container (blow.ini).\nUsage: /fishpw <password>", 0);
#endif

	xchat_print(ph, "\002FiSH v0.98\002 - encryption plugin for XChat \002loaded!\002 URL: http://fish.sekure.us\n");

	return 1;	// return 1 for success
}



int xchat_plugin_deinit(xchat_plugin *plugin_handle)
{
	FILE *hRnd;

	xchat_set_context(ph, xchat_find_context(ph, NULL, NULL));	// set context to current window

	xchat_print(ph, "\002FiSH unloaded.\002\n");
	ph=0;

	if(mip) mirexit();

	if(hRnd=fopen(rndPath, "wb"))
	{	// write random buffer to file
		fwrite(rndBuf, sizeof(rndBuf), 1, hRnd);
		fclose(hRnd);
#ifdef WIN32
		if(hCryptProv) CryptReleaseContext(hCryptProv, 0);
#endif
	}

	return 1;
}



// replace '[' and ']' from nick/channel with '~' (else problems with .ini files)
void FixContactName(char *contactName)
{
	while(*contactName != 0)
	{
		if((*contactName == '[') || (*contactName == ']')) *contactName='~';
		contactName++;
	}
}



// :somenick!ident@host.net PRIVMSG leetguy :Some Text -> Result: Rnick="somenick"
int ExtractRnick(char *Rnick, char *incoming_msg)		// needs pointer to "nick@host" or ":nick@host"
{
	int k=0;

	if(*incoming_msg == ':') incoming_msg++;

	while(*incoming_msg!='!' && *incoming_msg!=0) {
		Rnick[k]=*incoming_msg;
		incoming_msg++;
		k++;
	}
	Rnick[k]=0;

	if (*Rnick < '0') return FALSE;
	else return TRUE;
}



void memXOR(unsigned char *s1, const unsigned char *s2, int n)
{
	while(n--) *s1++ ^= *s2++;
}



// Find the first occurrence of NEEDLE in HAYSTACK, using case-insensitive  comparison.
char * strcasestr (const char *haystack, const char *needle)
{
	// Be careful not to look at the entire extent of haystack or needle
	// until needed.  This is useful because of these two cases:
	//   - haystack may be very long, and a match of needle found early,
	//   - needle may be very long, and not even a short initial segment of
	//     needle may be found in haystack.

	if (*needle != '\0')
	{	// Speed up the following searches of needle by caching its first character
		unsigned char b = tolower ((unsigned char) *needle);

		needle++;
		for (;; haystack++)
		{
			if (*haystack == '\0') return NULL;	// no match
			if (tolower ((unsigned char) *haystack) == b)
			{	// The first character matches
				const char *rhaystack = haystack + 1;
				const char *rneedle = needle;

				for (;; rhaystack++, rneedle++)
				{
                  if (*rneedle == '\0') return (char *) haystack;	// Found a match
                  if (*rhaystack == '\0') return NULL;	// no match
                  if (tolower ((unsigned char) *rhaystack)
                      != tolower ((unsigned char) *rneedle))
                    /* Nothing in this round.  */
                    break;
				}
			}
		}
	}
	else return (char *) haystack;
}



#ifdef WIN32
BOOL CALLBACK PassFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HICON hicon;

	switch (msg)
	{
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hwndDlg, IDC_PASS));
			hicon=LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_KEY));
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (long)hicon);
			SendDlgItemMessage(hwndDlg, IDC_PASS, EM_LIMITTEXT, 99, 0);		// Allow only 99 chars for IDC_PASS editbox
			break;
		case WM_COMMAND:
			if(LOWORD(wParam)==IDOK)
			{
				*iniKey=0;
				if(GetDlgItemText(hwndDlg, IDC_PASS, iniKey, 99)==0)
				{
					EndDialog(hwndDlg, 0);	// quit password dialog and return FALSE (error)
					return 0;
				}
				EndDialog(hwndDlg, 1);	// quit password dialog and return TRUE (success)
				break;
			}
			return TRUE;
		case WM_DESTROY:
		case WM_CLOSE:
			EndDialog(hwndDlg, 0);
			return TRUE;
	}
	return FALSE;
}
#endif

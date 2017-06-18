/*
 * Off-the-Record Messaging (OTR) modules for IRC
 * Copyright (C) 2009  Uli Meis <a.sporto+bee@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,USA
 */

#include "otr.h"

WEECHAT_PLUGIN_NAME("irc-otr");
WEECHAT_PLUGIN_DESCRIPTION("Off-The-Record Messaging for WeeChat");
WEECHAT_PLUGIN_AUTHOR("Uli Meis <a.sporto+bee@gmail.com>");
WEECHAT_PLUGIN_VERSION(IRCOTR_VERSION);
WEECHAT_PLUGIN_WEECHAT_VERSION("unknown");
WEECHAT_PLUGIN_LICENSE("GPL3");

struct t_weechat_plugin *weechat_otr_plugin = NULL;
struct t_gui_bar_item *otr_statusbar;

int debug = 0;

#ifdef HAVE_GREGEX_H
GRegex *regex_nickignore = NULL;
#endif

static IOUSTATE *ioustate;

void printformatva(IRC_CTX *ircctx, const char *nick, const char *format, va_list params)
{
	char msg[LOGMAX], *s = msg;
	char *server = NULL;
	struct t_gui_buffer *buffer = NULL;

	if (ircctx)
		server = ircctx->address;

	if (server&&nick) {
		char s[256];
		sprintf(s,"%s.%s",ircctx->address,nick);
		buffer = weechat_buffer_search("irc",s);
		if (!buffer) {
			char cmd[256];
			sprintf(cmd,"/query -server %s %s",ircctx->address,nick);
			weechat_command(NULL,cmd);
			buffer = weechat_buffer_search("irc",s);
			if (!buffer)
				weechat_printf(NULL,"OTR:\tFailed to create "
					       "a buffer for the following "
					       "message! server=%s,nick=%s",
					       ircctx->address,nick);
		}
	}

	if( vsnprintf( s, LOGMAX, format, params ) < 0 )
		sprintf( s, "internal error parsing error string (BUG)" );
	va_end( params );

	weechat_printf(buffer,"OTR:\t%s",s);
}

void printformat(IRC_CTX *ircctx, const char *nick, int lvl, int fnum, ...)
{
	va_list params;
	va_start( params, fnum );

	printformatva(ircctx,nick,formats[fnum].def,params);
}

void otr_log(IRC_CTX *ircctx, const char *nick, int level, const char *format, ...)
{
	va_list params;
	va_start( params, format );

	printformatva(ircctx,nick,format,params);
}

#define wc_printf(server,nick,format,...) \
	otr_log(server,nick,0,format, ## __VA_ARGS__)

#define wc_debug(server,nick,format,...) { \
	if (debug) \
		wc_printf(server,nick, \
			    format, ## __VA_ARGS__); \
}

void irc_send_message(IRC_CTX *ircctx, const char *recipient, char *msg) {
	char nmsg[512];
	
	wc_debug(ircctx,recipient,"OTR injection %s.%s: %s",ircctx->address,recipient,msg);
	sprintf(nmsg,"/quote -server %s PRIVMSG %s :%s",ircctx->address,recipient,msg);
	weechat_command(NULL,nmsg);
}

IRC_CTX *ircctx_by_peername(const char *peername, char *nick)
{
	char *address;
	static IRC_CTX ircctx;
	static char pname[256];

	strcpy(pname,peername);

	address = strchr(pname,'@');

	if (!address)
		return NULL;

	*address = '\0';
	strcpy(nick,pname);
	*address++ = '@';

	ircctx.address = address;
	ircctx.nick = pname;

        return &ircctx;
}

char *wc_modifier_privmsg_in(void *data, const char *modifier,
			  const char *modifier_data, const char *string)
{
	int argc;
	char **argv, **argv_eol;
	char nick[256];
	char *newmsg,*msg;
	IRC_CTX ircctx;
	char cmsg[512];

	string = strdup(string);

	argv = weechat_string_explode (string, " ", 0, 0, &argc);
	argv_eol = weechat_string_explode (string, " ", 1, 0, NULL);

	if (!extract_nick(nick,argv[0]))
		goto done;

	if ((*argv[2]=='&')||(*argv[2]=='#'))
		goto done;

#ifdef HAVE_GREGEX_H
	if (g_regex_match(regex_nickignore,nick,0,NULL))
		goto done;
#endif

	ircctx.address = (char*)modifier_data;
	ircctx.nick = argv[2];

	msg = argv_eol[3]+1;
	wc_debug(&ircctx,nick,"otr receive own %s, server %s, nick %s, msg %s",
		       ircctx.nick,ircctx.address,nick,msg);
	newmsg = otr_receive(&ircctx,msg,nick);

	if (!newmsg) {
		string = strdup("");
		goto done;
	}

	if (newmsg==msg) {
		goto done;
	}

	snprintf(cmsg, 511, "%s %s %s :%s",argv[0],argv[1],argv[2],newmsg);

	otrl_message_free(newmsg);

	string = strdup(cmsg);
done:
	weechat_string_free_exploded(argv);
	weechat_string_free_exploded(argv_eol);

	return (char*)string;
}

char *wc_modifier_privmsg_out(void *data, const char *modifier,
			  const char *modifier_data, const char *string)
{
	int argc;
	char **argv, **argv_eol;
	IRC_CTX ircctx;
	char newmsg[512];
	char *otrmsg;
	char s[256];
	char *msg;

	argv = weechat_string_explode (string, " ", 0, 0, &argc);
	argv_eol = weechat_string_explode (string, " ", 1, 0, NULL);

	string = strdup(string);
	
	if ((*argv[1]=='&')||(*argv[1]=='#'))
		goto done;

	msg = argv_eol[2]+1;

#ifdef HAVE_GREGEX_H
	if (g_regex_match(regex_nickignore,argv[1],0,NULL))
		goto done;
#endif

	/* we're unfortunately fed back stuff from irc_send_message above */
	if (strncmp(msg,"?OTR",4)==0)
		goto done;

	ircctx.address = (char*)modifier_data;
	sprintf(s,"%s.%s",ircctx.address,argv[1]);

	ircctx.nick = (char*)weechat_info_get("irc_nick",ircctx.address);

	wc_debug(&ircctx,argv[1],"otr send own %s, server %s, nick %s, msg %s",
		       ircctx.nick,ircctx.address,argv[1],msg);
	otrmsg = otr_send(&ircctx,msg,argv[1]);

	if (otrmsg==msg)
		goto done;

	if (!otrmsg) {
		wc_debug(&ircctx,argv[1],"OTR send NULL");
		free((char*)string);
		string = strdup("");
		goto done;
	}

	wc_debug(&ircctx,argv[1],"NEWMSG");
	snprintf(newmsg, 511, "PRIVMSG %s :%s", argv[1], otrmsg);

	otrl_message_free(otrmsg);
	
	free((char*)string);
	string = newmsg;

done:
	weechat_string_free_exploded(argv);
	weechat_string_free_exploded(argv_eol);

	return (char*)string;
}

int cmd_otr(void *data, struct t_gui_buffer *buffer, int argc, char **word, char **word_eol)
{
	const char *own_nick = weechat_buffer_get_string(buffer,"localvar_nick");
	const char *server = weechat_buffer_get_string(buffer,"localvar_server");
	char *target = (char*)weechat_buffer_get_string(buffer,"short_name");
	IRC_CTX ircctxs = {
		.nick = (char*)own_nick,
		.address = (char*)server },
		*ircctx = &ircctxs;

	word++;
	word_eol++;
	argc--;

	cmd_generic(ioustate,ircctx,argc,word,word_eol,target);

	return WEECHAT_RC_OK;
}

/*
 * otr status bar
 */
char* otr_statusbar_callback (void *data,
			      struct t_gui_bar_item *item,
			      struct t_gui_window *window)
{
	const char *target;
	IRC_CTX ircctx;
	int formatnum;
	struct t_gui_buffer *buffer = 
		weechat_window_get_pointer(window, "buffer");
	const char *type = weechat_buffer_get_string (buffer, "localvar_type");

	if (!type || strcmp(type, "private") != 0) 
		return strdup("");
	
	ircctx.nick = (char*)weechat_buffer_get_string(buffer,"localvar_nick");
	ircctx.address =  (char*)weechat_buffer_get_string(buffer,"localvar_server");
	target = weechat_buffer_get_string(buffer,"short_name");

	formatnum = otr_getstatus_format(&ircctx, target);

	return strdup (formats[formatnum].def);
}


void otr_status_change(IRC_CTX *ircctx, const char *nick, int event)
{
	char signalname[128];
	char servernick[256];

	sprintf(signalname,"OTR_%s",otr_status_txt[event]);
	sprintf(servernick,"%s,%s",ircctx->address,nick);

	weechat_hook_signal_send(signalname,
				 WEECHAT_HOOK_SIGNAL_STRING,
				 servernick);

	weechat_bar_item_update("otr");
}

int weechat_plugin_init (struct t_weechat_plugin *plugin, int argc, char *argv[])
{

	weechat_plugin = plugin;

	weechat_hook_modifier("irc_in_privmsg", &wc_modifier_privmsg_in, NULL);
	weechat_hook_modifier("irc_out_privmsg", &wc_modifier_privmsg_out, NULL);

	if (otrlib_init())
		return WEECHAT_RC_ERROR;

	ioustate = otr_init_user("one to rule them all");

	otr_setpolicies(ioustate,IO_DEFAULT_POLICY,FALSE);
	otr_setpolicies(ioustate,IO_DEFAULT_POLICY_KNOWN,TRUE);

#ifdef HAVE_GREGEX_H
	if (regex_nickignore)
		g_regex_unref(regex_nickignore);
	regex_nickignore = g_regex_new(IO_DEFAULT_IGNORE,0,0,NULL);
#endif

	weechat_hook_command ("otr",
			      N_("Control the OTR module"),
			      N_("[text]"),
			      N_("text: write this text"),
			      "",
			      &cmd_otr, NULL);

	cmds[CMDCOUNT].name = "set";
	cmds[CMDCOUNT].cmdfunc = cmd_set;

	otr_statusbar = weechat_bar_item_new ("otr", &otr_statusbar_callback, NULL);
	weechat_bar_item_update ("otr");

	return WEECHAT_RC_OK;
}

int weechat_plugin_end (struct t_weechat_plugin *plugin)
{
	weechat_bar_item_remove(otr_statusbar);

	otr_deinit_user(ioustate);

	otrlib_deinit();

	return WEECHAT_RC_OK;
}

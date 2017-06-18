/*
 *  Off-the-Record Messaging Proxy
 *  Copyright (C) 2004-2005  Nikita Borisov and Ian Goldberg
 *                           <otr@cypherpunks.ca>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <sys/select.h>
#endif

#include <privkey.h>
#include <proto.h>
#include <userstate.h>

#include "sockdef.h"
#include "otrproxy.h"
#include "oscarproxy.h"
#include "buffer.h"
#include "netstate.h"
#include "servsock.h"
#include "util.h"
#include "proxyevent.h"

/* The common UserState */
OtrlUserState otrproxy_userstate = NULL;

/* Start the OTR proxy main loop. */
void otrproxy_mainloop(void)
{
    while(1) {
	fd_set rfds, wfds;
	int blockms;
	int maxfd = -1, res;
	struct timeval block;

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);

	/* See who is waiting */
	blockms = netstate_fdsets(&rfds, &wfds, &maxfd);
	if (blockms >= 0) {
	    block.tv_sec = blockms / 1000;
	    block.tv_usec = (blockms % 1000) * 1000;
	}

#ifdef DEBUG
	/* Flush stderr, since that seems to be important on Win32. */
	fprintf(stderr, "Select... ");
	fflush(stderr);
#endif
	/* Do the select.  We should be blocking here almost all of the
	 * time. */
	res = select(maxfd + 1, &rfds, &wfds, NULL,
		blockms >= 0 ? &block : NULL);

#ifdef DEBUG
	fprintf(stderr, "%d\n", res);
#endif
	/* Who is ready? */
	netstate_handle(&rfds, &wfds);
    }
}

/* Initialize the proxy.  Use the given userdir, if non-NULL. */
void otrproxy_init(const char *custom_userdir)
{
    char *userdir, *privkeyfile, *storefile;

#ifdef WIN32
    /* Win32 doesn't send a SIGPIPE if you write to a socket that the
     * other end has closed, so we don't need to deal with it. */
#else
    /* Ignore SIGPIPEs; we'll gracefully handle it if write() returns an
     * error. */
    signal(SIGPIPE, SIG_IGN);
#endif

    /* Set the custom user dir, if present */
    util_userdir_set(custom_userdir);

    /* Make sure we have a user dir */
    userdir = util_userdir_file(NULL);
#ifdef WIN32
    mkdir(userdir);
#else
    mkdir(userdir, 0755);
#endif
    free(userdir);

    /* Initialize the OTR library */
    OTRL_INIT;

    /* Create a single UserState */
    otrproxy_userstate = otrl_userstate_create();
    if (!otrproxy_userstate) {
	fprintf(stderr, "Could not create User State!\n");
	exit(1);
    }

    /* Read the private keys and fingerprint store */
    privkeyfile = util_userdir_file(PRIVKEYFNAME);
    storefile = util_userdir_file(STOREFNAME);
    if (!privkeyfile || !storefile) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }
    otrl_privkey_read(otrproxy_userstate, privkeyfile);
    otrl_privkey_read_fingerprints(otrproxy_userstate, storefile, NULL, NULL);
    free(privkeyfile);
    free(storefile);
}

/* Set up the libotr UI ops */
static OtrlPolicy (*ui_policy_cb)(ConnContext *) = NULL;

static OtrlPolicy policy_cb(void *opdata, ConnContext *context)
{
    if (ui_policy_cb) {
	return ui_policy_cb(context);
    } else {
	return OTRL_POLICY_DEFAULT;
    }
}

static const char *protocol_name_lookup(const char *protocol)
{
    if (!strcmp(protocol, OSCAR_PROTOCOL_ID)) {
	return "AIM/ICQ";
    } else {
	return protocol;
    }
}

static const char *protocol_name_cb(void *opdata, const char *protocol)
{
    return protocol_name_lookup(protocol);
}

static void protocol_name_free_cb(void *opdata, const char *protocol_name)
{
    /* Do nothing, since we didn't actually allocate any memory in
     * protocol_name_cb. */
}

static void create_privkey_cb(void *opdata, const char *accountname,
	const char *protocol)
{
    unsigned int dialogid = proxyevent_generating_privkey(accountname,
	    protocol);

    otrproxy_generate_privkey(accountname, protocol);

    proxyevent_done_generating_privkey(dialogid);
}

static void notify_cb(void *opdata, OtrlNotifyLevel level,
	const char *accountname, const char *protocol, const char *username,
	const char *title, const char *primary, const char *secondary)
{
    proxyevent_generic_dialog(level, title, primary, secondary);
}

static int display_otr_message_cb(void *opdata, const char *accountname,
	const char *protocol, const char *username, const char *msg)
{
    return -1;
}

static void update_context_list_cb(void *opdata)
{
    proxyevent_context_state();
}

static void new_fingerprint_cb(void *opdata, OtrlUserState us,
	const char *accountname, const char *protocol, const char *username,
	unsigned char fingerprint[20])
{
    proxyevent_new_fingerprint(accountname, protocol, username, fingerprint);
}


static void write_fingerprints_cb(void *opdata)
{
    char *storefile = util_userdir_file(STOREFNAME);
    otrl_privkey_write_fingerprints(otrproxy_userstate, storefile);
    free(storefile);
}

static void gone_secure_cb(void *opdata, ConnContext *context,
	int protocol_version)
{
    proxyevent_gone_secure(context, protocol_version);
}

static void gone_insecure_cb(void *opdata, ConnContext *context)
{
    proxyevent_gone_insecure(context);
}

static void still_secure_cb(void *opdata, ConnContext *context, int is_reply,
	int protocol_version)
{
    if (is_reply == 0) {
	proxyevent_still_secure(context, protocol_version);
    }
}

static void log_message_cb(void *opdata, const char *message)
{
    /* message already ends in \n */
    proxyevent_log_message(message);
}

static int is_online_cb(void *opdata, const char *accountname,
	const char *protocol, const char *recipient)
{
    return -1;
}

static void inject_message_cb(void *opdata, const char *accountname,
	const char *protocol, const char *recipient, const char *message)
{
    NetState *ns = opdata;

    if (!strcmp(protocol, OSCAR_PROTOCOL_ID)) {
	oscarproxy_inject_message(ns, recipient, message);
    }
}

OtrlMessageAppOps otrproxy_ui_ops = {
    policy_cb,
    create_privkey_cb,
    is_online_cb,
    inject_message_cb,
    notify_cb,
    display_otr_message_cb,
    update_context_list_cb,
    protocol_name_cb,
    protocol_name_free_cb,
    new_fingerprint_cb,
    write_fingerprints_cb,
    gone_secure_cb,
    gone_insecure_cb,
    still_secure_cb,
    log_message_cb
};

/* Disconnect a private connection, and let the other user know. */
void otrproxy_disconnect(ConnContext *context)
{
    otrl_message_disconnect(otrproxy_userstate, &otrproxy_ui_ops, NULL,
	    context->accountname, context->protocol, context->username);
    proxyevent_gone_insecure(context);
}

/* Inject a message, looking up the account as appropriate */
void otrproxy_inject_message(const char *accountname,
	const char *protocol, const char *recipient, const char *message)
{
    if (!strcmp(protocol, OSCAR_PROTOCOL_ID)) {
	/* Find the right NetState */
	NetState *ns = oscarproxy_find_netstate(accountname);

	if (!ns) {
	    const char *protoname = protocol_name_lookup(protocol);
	    const char *fmt = "You are not currently connected to account "
		"%s (%s).";
	    char *msg = malloc(strlen(fmt) + strlen(accountname) +
		    strlen(protoname) - 3);
	    if (msg) {
		sprintf(msg, fmt, accountname, protoname);
		proxyevent_generic_dialog(OTRL_NOTIFY_ERROR, "Not connected",
			msg, NULL);
		free(msg);
	    }
	} else {
	    oscarproxy_inject_message(ns, recipient, message);
	}
    }
}

/* Attempt to start a private connection. */
void otrproxy_connect(ConnContext *context)
{
    char *msg = otrl_proto_default_query_msg(context->accountname,
	    policy_cb(NULL, context));
    otrproxy_inject_message(context->accountname, context->protocol,
	    context->username, msg ? msg : "?OTRv2?");
    free(msg);
}

/* Forget a fingerprint */
void otrproxy_forget_fingerprint(Fingerprint *fingerprint)
{
    ConnContext *context;
    char *storefile;

    if (fingerprint == NULL) return;

    context = fingerprint->context;
    if (context == NULL) return;

    if (context->msgstate == OTRL_MSGSTATE_ENCRYPTED &&
	    context->active_fingerprint == fingerprint) return;

    otrl_context_forget_fingerprint(fingerprint, 1);
    storefile = util_userdir_file(STOREFNAME);
    otrl_privkey_write_fingerprints(otrproxy_userstate, storefile);
    free(storefile);

    proxyevent_context_state();
}

/* Set the policy callback */
void otrproxy_set_policy_cb(OtrlPolicy (*cb)(ConnContext *))
{
    ui_policy_cb = cb;
}

/* Generate a private key */
void otrproxy_generate_privkey(const char *accountname, const char *protocol)
{
    char *privkeyfile = util_userdir_file(PRIVKEYFNAME);

    /* Generate the key */
    otrl_privkey_generate(otrproxy_userstate, privkeyfile, accountname,
	    protocol);
    free(privkeyfile);

    proxyevent_privkey_state();
}

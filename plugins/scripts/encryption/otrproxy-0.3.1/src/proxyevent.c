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
#include <assert.h>

#include "proxyevent.h"

static void (*event_handler)(ProxyEvent *event, void *data) = NULL;
static void *event_handler_data = NULL;

static unsigned int last_dialog_id = 0;

/* A linked list of open dialog boxes */
typedef struct s_OpenDialog {
    unsigned int dialogid;
    void (*response_cb)(int resp, void *response_data);
    void *response_data;

    struct s_OpenDialog *next;
    struct s_OpenDialog **tous;
} OpenDialog;

static OpenDialog *proxyevent_open_dialogs = NULL;

/* Register a handler for proxy Events */
void proxyevent_register(void (*handler)(ProxyEvent *event, void *data),
	void *data)
{
    event_handler = handler;
    event_handler_data = data;
}

static void init_event(ProxyEvent *evp, ProxyEventType type)
{
    memset(evp, 0, sizeof(ProxyEvent));
    evp->type = type;
}

/* Call this when the proxy's socket state changes; for example, when a
 * new connection (inbound or outbound) is established. */
void proxyevent_socket_state(void)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_SOCKET_STATE);

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this when the proxy's context state changes; for example, when a
 * new ConnContext is added, or when an existing one changes to
 * CONN_CONNECTED. */
void proxyevent_context_state(void)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_CONTEXT_STATE);

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this when the proxy's privkey state changes.  This should only
 * be when a new private key is created. */
void proxyevent_privkey_state(void)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_PRIVKEY_STATE);

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this when a ConnContext enters the CONN_CONNECTED state. */
void proxyevent_gone_secure(ConnContext *context, int protocol_version)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_GONE_SECURE);
    ev.dialogid = ++last_dialog_id;
    ev.account = context->accountname;
    ev.protocol = context->protocol;
    ev.username = context->username;
    ev.fingerprint = context->active_fingerprint->fingerprint;
    ev.sessionid = context->sessionid;
    ev.sessionid_len = context->sessionid_len;
    ev.boldhalf = context->sessionid_half;
    ev.protocol_version = protocol_version;

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this when a ConnContext leaves the CONN_CONNECTED state */
void proxyevent_gone_insecure(ConnContext *context)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_GONE_INSECURE);
    ev.dialogid = ++last_dialog_id;
    ev.account = context->accountname;
    ev.protocol = context->protocol;
    ev.username = context->username;

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this when a ConnContext receives a Key Exchange Message it
 * already knew (but only if it's not a reply to one we sent). */
void proxyevent_still_secure(ConnContext *context, int protocol_version)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_STILL_SECURE);
    ev.dialogid = ++last_dialog_id;
    ev.account = context->accountname;
    ev.protocol = context->protocol;
    ev.username = context->username;
    ev.protocol_version = protocol_version;

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this when we're about to generate a private key.  It will return
 * the dialog id, for passing to proxyevent_done_generating_privkey. */
unsigned int proxyevent_generating_privkey(const char *account,
	const char *protocol)
{
    unsigned int dialogid;
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_GENERATING_PRIVKEY);
    dialogid = ev.dialogid = ++last_dialog_id;
    ev.account = account;
    ev.protocol = protocol;

    if (event_handler) event_handler(&ev, event_handler_data);

    return dialogid;
}

/* Call this when we're done generating the private key.  Pass the
 * dialogid returned from proxyevent_generating_privkey. */
void proxyevent_done_generating_privkey(unsigned int dialogid)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_DONE_GENERATING_PRIVKEY);
    ev.dialogid = dialogid;

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this when we're to write a log/debug message.  The message
 * should already end in "\n". */
void proxyevent_log_message(const char *message)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_LOG_MESSAGE);
    ev.primary = message;

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this to display a free-form dialog to the user, which will have
 * a single "OK" button. */
void proxyevent_generic_dialog(OtrlNotifyLevel level, const char *title,
	const char *primary, const char *secondary)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_GENERIC_DIALOG);
    ev.dialogid = ++last_dialog_id;
    ev.level = level;
    ev.title = title;
    ev.primary = primary;
    ev.secondary = secondary;

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* Call this to inform the user that the other side has closed his end
 * of the private connection. */
void proxyevent_disconnected(const char *accountname, const char *protocol,
	const char *username)
{
    const char *fmt = "%s has closed his private connection to you.";
    char *primary = malloc(strlen(fmt) + strlen(username) - 1);
    if (primary) {
	sprintf(primary, fmt, username);
	proxyevent_generic_dialog(OTRL_NOTIFY_INFO, "Connection closed",
		primary, "You should do the same.");
	free(primary);
    }
}

/* Call this to inform the user of a hitherto unknown fingerprint. */
void proxyevent_new_fingerprint(const char *accountname,
	const char *protocol, const char *username,
	const unsigned char *fingerprint)
{
    ProxyEvent ev;
    init_event(&ev, PROXYEVENT_CONFIRM_FINGERPRINT);
    ev.dialogid = ++last_dialog_id;
    ev.account = accountname;
    ev.username = username;
    ev.protocol = protocol;
    ev.fingerprint = fingerprint;

    if (event_handler) event_handler(&ev, event_handler_data);
}

/* The UI calls this (after calling uisync_lock()) to indicate that a
 * given dialog box has been closed.  resp is how it was closed:
 * 1 = yes/OK, 0 = no/cancel, -1 = dialog closed without answering. */
void proxyevent_ui_closed_dialog(unsigned int dialogid, int resp)
{
    OpenDialog *od = proxyevent_open_dialogs;

    /* Find the OpenDialog for this dialogid */
    while (od) {
	if (od->dialogid == dialogid) {
	    if (od->response_cb) {
		od->response_cb(resp, od->response_data);
	    }
	    /* Delete the OpenDialog entry */
	    *(od->tous) = od->next;
	    if (od->next) {
		od->next->tous = od->tous;
	    }
	    free(od);
	    return;
	}
	od = od->next;
    }
}

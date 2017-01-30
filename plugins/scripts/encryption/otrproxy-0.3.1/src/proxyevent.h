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

#ifndef __PROXYEVENT_H__
#define __PROXYEVENT_H__

#include <gcrypt.h>
#include <dh.h>
#include <proto.h>
#include <message.h>

typedef enum {
    PROXYEVENT_SOCKET_STATE,
    PROXYEVENT_CONTEXT_STATE,
    PROXYEVENT_PRIVKEY_STATE,
    PROXYEVENT_LOG_MESSAGE,
    PROXYEVENT_CLOSE_DIALOG,
    PROXYEVENT_GENERIC_DIALOG,
    PROXYEVENT_CONFIRM_FINGERPRINT,
    PROXYEVENT_GENERATING_PRIVKEY,
    PROXYEVENT_DONE_GENERATING_PRIVKEY,
    PROXYEVENT_GONE_SECURE,
    PROXYEVENT_GONE_INSECURE,
    PROXYEVENT_STILL_SECURE
} ProxyEventType;

typedef struct {
    ProxyEventType type;
    unsigned int dialogid;
    const char *account, *protocol, *username;
    OtrlNotifyLevel level;
    const char *title, *primary, *secondary;
    const unsigned char *fingerprint, *sessionid;
    size_t sessionid_len;
    OtrlSessionIdHalf boldhalf;
    int protocol_version;
} ProxyEvent;

/* Register a handler for proxy Events */
void proxyevent_register(void (*handler)(ProxyEvent *event, void *data),
	void *data);

/* Call this when the proxy's socket state changes; for example, when a
 * new connection (inbound or outbound) is established. */
void proxyevent_socket_state(void);

/* Call this when the proxy's context state changes; for example, when a
 * new ConnContext is added, or when an existing one changes to
 * CONN_CONNECTED. */
void proxyevent_context_state(void);

/* Call this when the proxy's privkey state changes.  This should only
 * be when a new private key is created. */
void proxyevent_privkey_state(void);

/* Call this when a ConnContext enters the CONN_CONNECTED state. */
void proxyevent_gone_secure(ConnContext *context, int protocol_version);

/* Call this when a ConnContext leaves the CONN_CONNECTED state */
void proxyevent_gone_insecure(ConnContext *context);

/* Call this when a ConnContext receives a Key Exchange Message it
 * already knew (but only if it's not a reply to one we sent). */
void proxyevent_still_secure(ConnContext *context, int protocol_version);

/* Call this when we're about to generate a private key.  It will return
 * the dialog id, for passing to proxyevent_done_generating_privkey. */
unsigned int proxyevent_generating_privkey(const char *account,
	const char *protocol);

/* Call this when we're done generating the private key.  Pass the
 * dialogid returned from proxyevent_generating_privkey. */
void proxyevent_done_generating_privkey(unsigned int dialogid);

/* Call this when we're to write a log/debug message.  The message
 * should already end in "\n". */
void proxyevent_log_message(const char *message);

/* Call this to display a free-form dialog to the user, which will have
 * a single "OK" button. */
void proxyevent_generic_dialog(OtrlNotifyLevel level, const char *title,
	const char *primary, const char *secondary);

/* Call this to inform the user that the other side has closed his end
 * of the private connection. */
void proxyevent_disconnected(const char *accountname, const char *protocol,
	const char *username);

/* Call this to inform the user of a hitherto unknown fingerprint. */
void proxyevent_new_fingerprint(const char *accountname,
	const char *protocol, const char *username,
	const unsigned char *fingerprint);

/* The UI calls this (after calling uisync_lock()) to indicate that a
 * given dialog box has been closed.  resp is how it was closed:
 * 1 = yes/OK, 0 = no/cancel, -1 = dialog closed without answering. */
void proxyevent_ui_closed_dialog(unsigned int dialogid, int resp);

#endif

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

#ifndef __OTRPROXY_H__
#define __OTRPROXY_H__

#include <proto.h>
#include <userstate.h>
#include <message.h>

#define OTRPROXY_VERSION "0.3.1"

/* What internal ID should we use for the OSCAR protocol? */
#define OSCAR_PROTOCOL_ID "prpl-oscar"

/* The common UserState */
extern OtrlUserState otrproxy_userstate;

/* The common ui ops */
extern OtrlMessageAppOps otrproxy_ui_ops;

/* Start the OTR proxy main loop. */
void otrproxy_mainloop(void);

/* Initialize the proxy.  Use the given userdir, if non-NULL. */
void otrproxy_init(const char *custom_userdir);

/* Disconnect a private connection, and let the other user know. */
void otrproxy_disconnect(ConnContext *context);

/* Inject a message, looking up the account as appropriate */
void otrproxy_inject_message(const char *accountname,
	const char *protocol, const char *recipient, const char *message);

/* Attempt to start a private connection. */
void otrproxy_connect(ConnContext *context);

/* Forget a fingerprint */
void otrproxy_forget_fingerprint(Fingerprint *fingerprint);

/* Set the policy callback */
void otrproxy_set_policy_cb(OtrlPolicy (*cb)(ConnContext *));

/* Generate a private key */
void otrproxy_generate_privkey(const char *accountname, const char *protocol);

#endif

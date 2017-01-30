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

#ifndef __OSCARPROXY_H__
#define __OSCARPROXY_H__

#include <userstate.h>

#include "sockdef.h"
#include "buffer.h"
#include "netstate.h"

struct oscarproxy_data {
    SOCKET cfd;              /* fd to the client */
    SOCKET sfd;              /* fd to the server */
    int clientdone;          /* Has the client stopped sending? */
    int serverdone;          /* Has the server stopped sending? */
    Buffer crbuf;            /* read buffer from client */
    Buffer cwbuf;            /* write buffer to client */
    Buffer srbuf;            /* read buffer from server */
    Buffer swbuf;            /* write buffer to server */

                             /* Functions to write data to the client
			      * and server */
    void (*cwbuf_write)(NetState *, unsigned char *, size_t);
    void (*swbuf_write)(NetState *, unsigned char *, size_t);
    void *cwbuf_write_data;
    void *swbuf_write_data;

    int have_crdata;         /* If we were given leftover data read from
				the client in oscarproxy_enter, set this
				value to 1. */
    unsigned short cseqno;   /* The current FLAP sequence number to the
				client */
    unsigned short sseqno;   /* The current FLAP sequence number to the
				server */
    char *username;          /* The username for this connection */
    int logged_in;           /* Have we yet authenticated to the server? */
    void (*atexit)(void *);  /* Call this just before we clean up */
    void *atexit_data;       /* Pass this to the above call */
};

typedef struct s_cookie {
    unsigned char *cookie;
    size_t cookie_len;
    char *username;
    char *boshost;
    time_t when;
    struct s_cookie *next;
    struct s_cookie **tous;
} Cookie;

/* Find the NetState corresponding to the OSCAR connection with the
 * given accountname. */
NetState *oscarproxy_find_netstate(const char *accountname);

/* Inject an OSCAR message */
void oscarproxy_inject_message(NetState *ns, const char *who, const char *msg);

/* Change a NetState to act as an OSCAR proxy between the given client
 * and server fds.  Copy any data in the given buffer to the client read
 * buffer, which we'll create. */
void oscarproxy_enter(NetState *ns, SOCKET cfd, SOCKET sfd, Buffer *crbuf);

#endif

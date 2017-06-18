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

#ifndef __HTTPPROXY_H__
#define __HTTPPROXY_H__

#include "sockdef.h"
#include "buffer.h"

/* Flags to control the HTTP proxy */
#define HTTPPROXY_ALLOW_HTTP  1
#define HTTPPROXY_ALLOW_HTTPS 2

typedef enum {
    HTTPPROXY_READING_HEADERS,
    HTTPPROXY_WRITECLOSE,
    HTTPPROXY_WRITEHEADERS,
    HTTPPROXY_WAITING_DNS,
    HTTPPROXY_WAITING_CONNECT
} HttpProxyState;

struct httpproxy_data {
    HttpProxyState state;    /* The proxying state */
    int flags;               /* Flags to control the HTTP proxy */
    SOCKET cfd;              /* fd to the client */
    Buffer crbuf;            /* read buffer from the client */
    Buffer cwbuf;            /* write buffer to the client */

    char *auth64;            /* The expected value of the Proxy-Authorization
				header (NULL if no auth is needed) */
    int auth64len;           /* The length of the above header, or -1 if
				no auth is _allowed_ */

    char *host;              /* The hostname we're trying to CONNECT to */
    unsigned short port;     /* The port we're trying to CONNECT to */
#ifdef WIN32
    in_addr_t ipaddr;        /* The resolved address */
#else
    int dnsfd;               /* The fd to the DNS resolver */
#endif
    struct sockaddr_in sin;  /* The resolved address/port */

    SOCKET sfd;              /* fd to the server */

    time_t timeout;          /* When to stop waiting for HTTP OSCAR data */
};

/* Start a DNS lookup for ns->data->host. */
void httpproxy_startdns(NetState *ns);

/* Add a new NetState for acting as an HTTP proxy for the given fd */
void httpproxy_add(SOCKET cfd, int flags, const char *username,
	const char *password);

#endif

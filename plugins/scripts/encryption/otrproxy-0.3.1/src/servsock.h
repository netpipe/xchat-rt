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

#ifndef __SERVSOCK_H__
#define __SERVSOCK_H__

#include "sockdef.h"

typedef enum {
#ifdef WIN32
    SERVSOCK_PROXY_UISYNC,  /* Not actually a proxy type */
#endif
    SERVSOCK_PROXY_SOCKS5,
    SERVSOCK_PROXY_HTTPS,
    SERVSOCK_PROXY_HTTP
} ProxyType;

typedef enum {
    SERVSOCK_MASTER_NONE,
#ifdef WIN32
    SERVSOCK_MASTER_UISYNC,
#endif
    SERVSOCK_MASTER_SOCKS,
    SERVSOCK_MASTER_HTTP
} MasterType;

/* Start listening on the given port with the given proxytype.  Return
 * the port number we bound to (in case we passed port = 0). */
unsigned short servsock_add(in_addr_t bindaddr, unsigned short port,
	ProxyType proxytype, const char *username, const char *password,
	int *errnop);

/* Set a simple proxy config: at most one SOCKS5 proxy, at most one
 * HTTP(S) proxy, with the same username/password.  Set the appropriate
 * port to 0 if you don't want that kind of proxy; set both the username
 * and password to NULL if you don't want authentication.  *sockserrp
 * and *httperrp will be set to the appropriate errnos if binding to
 * those sockets fails.  Return 0 on total success, -1 otherwise. */
int servsock_start_simple(in_addr_t bindaddr, unsigned short socksport,
	unsigned short httpport, const char *username,
	const char *password, int *sockserrp, int *httperrp);

#endif

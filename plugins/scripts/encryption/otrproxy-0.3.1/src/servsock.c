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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#ifndef WIN32
#include <errno.h>
#endif

#include "sockdef.h"
#include "netstate.h"
#include "servsock.h"
#include "httpproxy.h"
#include "socksproxy.h"
#ifdef WIN32
#include "uisync.h"
#endif
#include "proxyevent.h"

struct servsock_data {
    SOCKET servfd;
    unsigned short port;
    char *username;
    char *password;
    MasterType mastertype;
    unsigned int flags;
};

static void set_errno(int *errnop)
{
#ifdef WIN32
    *errnop = WSAGetLastError();
#else
    *errnop = errno;
#endif
}

static SOCKET make_server_socket(in_addr_t bindaddr, unsigned short localport,
	int *errnop)
{
    SOCKET fd;
    struct sockaddr_in sin;
    int reuse = 1;
    *errnop = 0;
    
    fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) {
	set_errno(errnop);
	return -1;
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(bindaddr);
    sin.sin_port = htons(localport);

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse,
		sizeof(reuse)) < 0) {
	closesocket(fd);
	set_errno(errnop);
	return -1;
    }

    if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
	closesocket(fd);
	set_errno(errnop);
	return -1;
    }

    if (listen(fd, 5) < 0) {
	closesocket(fd);
	set_errno(errnop);
	return -1;
    }

    return fd;
}

static int servsock_fdset(NetState *ns, fd_set *rfdp, fd_set *wfdp,
	int *maxfdp)
{
    struct servsock_data *sdata = ns->data;
    FD_SET(sdata->servfd, rfdp);
    if (*maxfdp < sdata->servfd) *maxfdp = sdata->servfd;
    return -1;
}

static void servsock_handle(NetState *ns, fd_set *rfdp, fd_set *wfdp)
{
    struct servsock_data *sdata = ns->data;
    if (FD_ISSET(sdata->servfd, rfdp)) {
	SOCKET cfd;
	struct sockaddr_in sin;
	socklen_t sinlen = sizeof(sin);

	cfd = accept(sdata->servfd, (struct sockaddr *)&sin, &sinlen);
	if (cfd < 0) {
	    perror("accept");
	    return;
	}
#ifndef WIN32
	/* Set it to be non-blocking */
	fcntl(cfd, F_SETFL, O_NONBLOCK);
#endif

	switch (sdata->mastertype) {
	    case SERVSOCK_MASTER_NONE:
		/* Huh? */
		closesocket(cfd);
		proxyevent_socket_state();
		break;
#ifdef WIN32
	    case SERVSOCK_MASTER_UISYNC:
		/* Only accept a single UI connection */
		uisync_add(cfd, sdata->flags);
		netstate_del(ns);
		return;
#endif
	    case SERVSOCK_MASTER_SOCKS:
		socksproxy_add(cfd, sdata->flags, sdata->username,
			sdata->password);
		break;
	    case SERVSOCK_MASTER_HTTP:
		httpproxy_add(cfd, sdata->flags, sdata->username,
			sdata->password);
		break;
	}
    }
}

static void servsock_free_data(void *data)
{
    struct servsock_data *sdata = data;

    free(sdata->username);
    free(sdata->password);
    closesocket(sdata->servfd);
    free(sdata);
}

/* Start listening on the given port with the given proxytype.  Return
 * the port number we bound to (in case we passed port = 0). */
unsigned short servsock_add(in_addr_t bindaddr, unsigned short port,
	ProxyType proxytype, const char *username, const char *password,
	int *errnop)
{
    SOCKET servfd;
    MasterType mastertype = SERVSOCK_MASTER_NONE;
    struct servsock_data *sdata;
    NetState *ns;
    *errnop = 0;

    /* What is the "master" type for this proxy type? */
    switch (proxytype) {
#ifdef WIN32
	case SERVSOCK_PROXY_UISYNC:
	    mastertype = SERVSOCK_MASTER_UISYNC;
	    break;
#endif
	case SERVSOCK_PROXY_SOCKS5:
	    mastertype = SERVSOCK_MASTER_SOCKS;
	    break;
	case SERVSOCK_PROXY_HTTP:
	case SERVSOCK_PROXY_HTTPS:
	    mastertype = SERVSOCK_MASTER_HTTP;
	    break;
    }

    /* First see if there's already something listening on that port */
    for (ns = netstate_first(); ns; ns = netstate_next(ns)) {
	if (ns->type == NETSTATE_SERVSOCK) {
	    sdata = ns->data;
	    if (sdata->port == port) {
		break;
	    }
	}
    }

    if (ns == NULL) {
	/* There isn't yet a socket listening on that port; make one */
	servfd = make_server_socket(bindaddr, port, errnop);
	if (servfd == -1) {
	    return 0;
	}
	if (port == 0) {
	    /* Get the real port number */
	    struct sockaddr_in sin;
	    socklen_t sinlen = sizeof(sin);
	    int res = getsockname(servfd, (struct sockaddr *)&sin, &sinlen);
	    assert(res == 0);
	    port = ntohs(sin.sin_port);
	}
	sdata = calloc(1, sizeof(struct servsock_data));
	assert(sdata != NULL);
	sdata->username = username ? strdup(username) : NULL;
	sdata->password = password ? strdup(password) : NULL;
	sdata->port = port;
	sdata->servfd = servfd;
	sdata->mastertype = mastertype;
	sdata->flags = 0;

	ns = netstate_add(NETSTATE_SERVSOCK, servsock_fdset, servsock_handle,
		servsock_free_data, sdata);
    }

    if (ns) {
	/* There is; is is compatible with the requested type? */
	sdata = ns->data;
	if (sdata->mastertype != mastertype) {
	    /* We were asked to add, for example, a SOCKS proxy on a
	     * port for which we're listening as an HTTP proxy */
#ifdef WIN32
	    *errnop = WSAEADDRINUSE;
#else
	    *errnop = EADDRINUSE;
#endif
	    return 0;
	} else {
	    switch (proxytype) {
		case SERVSOCK_PROXY_SOCKS5:
		    sdata->flags |= SOCKSPROXY_ALLOW_SOCKS5;
		    break;
		case SERVSOCK_PROXY_HTTP:
		    sdata->flags |= HTTPPROXY_ALLOW_HTTP;
		    break;
		case SERVSOCK_PROXY_HTTPS:
		    sdata->flags |= HTTPPROXY_ALLOW_HTTPS;
		    break;
#ifdef WIN32
		case SERVSOCK_PROXY_UISYNC:
		    break;
#endif
	    }
	}
    }
    return sdata->port;
}

/* Set the username/password fields in the given sdata to the given
 * values (safely managing memory, of course) */
static void set_user_pass(struct servsock_data *sdata, const char *username,
	const char *password) {
    if (username == NULL) {
	free(sdata->username);
	sdata->username = NULL;
    } else if (!(sdata->username) || strcmp(username, sdata->username)) {
	free(sdata->username);
	sdata->username = strdup(username);
    }
    if (password == NULL) {
	free(sdata->password);
	sdata->password = NULL;
    } else if (!(sdata->password) || strcmp(password, sdata->password)) {
	free(sdata->password);
	sdata->password = strdup(password);
    }
}

/* Set a simple proxy config: at most one SOCKS5 proxy, at most one
 * HTTP(S) proxy, with the same username/password.  Set the appropriate
 * port to 0 if you don't want that kind of proxy; set both the username
 * and password to NULL if you don't want authentication.  *sockserrp
 * and *httperrp will be set to the appropriate errnos if binding to
 * those sockets fails.  Return 0 on total success, -1 otherwise. */
int servsock_start_simple(in_addr_t bindaddr, unsigned short socksport,
	unsigned short httpport, const char *username,
	const char *password, int *sockserrp, int *httperrp)
{
    NetState *ns;
    int ret = 0;
    int found_socks5 = 0;
    int found_http = 0;

    *sockserrp = 0;
    *httperrp = 0;

    /* First see if there are already proxies listening. */
    ns = netstate_first();
    while (ns) {
	NetState *nextns = netstate_next(ns);
	if (ns->type == NETSTATE_SERVSOCK) {
	    struct servsock_data *sdata = ns->data;
	    if (sdata->mastertype == SERVSOCK_MASTER_SOCKS) {
		/* There's a SOCKS5 proxy.  See if it's already on the
		 * right port. */
		if (sdata->port == socksport) {
		    /* It is.  Make sure it's got the right
		     * username/password and flags */
		    set_user_pass(sdata, username, password);
		    sdata->flags = SOCKSPROXY_ALLOW_SOCKS5;
		    found_socks5 = 1;
		} else {
		    /* An extraneous proxy; kill it */
		    netstate_del(ns);
		}
	    } else if (sdata->mastertype == SERVSOCK_MASTER_HTTP) {
		/* There's an HTTP proxy.  See if it's already on the
		 * right port. */
		if (sdata->port == httpport) {
		    /* It is.  Make sure it's got the right
		     * username/password and flags */
		    set_user_pass(sdata, username, password);
		    sdata->flags = HTTPPROXY_ALLOW_HTTP |
			HTTPPROXY_ALLOW_HTTPS;
		    found_http = 1;
		} else {
		    /* An extraneous proxy; kill it */
		    netstate_del(ns);
		}
	    }
	}
	ns = nextns;
    }

    /* Now start any proxies that weren't already listening */
    if (!found_socks5 && socksport) {
	unsigned short res = servsock_add(bindaddr, socksport,
		SERVSOCK_PROXY_SOCKS5, username, password, sockserrp);
	if (res == 0) ret = -1;
    }
    if (!found_http && httpport) {
	unsigned short res = servsock_add(bindaddr, httpport,
		SERVSOCK_PROXY_HTTP, username, password, httperrp);
	if (res == 0) {
	    ret = -1;
	} else {
	    res = servsock_add(bindaddr, httpport, SERVSOCK_PROXY_HTTPS,
		    username, password, httperrp);
	    if (res == 0) ret = -1;
	}
    }
    return ret;
}

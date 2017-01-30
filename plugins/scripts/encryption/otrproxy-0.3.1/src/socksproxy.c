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
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

#include "sockdef.h"
#include "netstate.h"
#include "netutil.h"
#include "buffer.h"
#include "oscarproxy.h"

typedef enum {
    SOCKSPROXY_READING_METHODS,
    SOCKSPROXY_WRITING_METHOD,
    SOCKSPROXY_READING_AUTH,
    SOCKSPROXY_WRITING_AUTHRESULT,
    SOCKSPROXY_READING_REQUEST,
    SOCKSPROXY_WAITING_DNS,
    SOCKSPROXY_WAITING_CONNECT,
    SOCKSPROXY_WRITERESP,
    SOCKSPROXY_WRITECLOSE,
} SocksProxyState;

struct socksproxy_data {
    SocksProxyState state;   /* The proxying state */
    SOCKET cfd;              /* fd to the client */
    Buffer crbuf;            /* read buffer from the client */
    Buffer cwbuf;            /* write buffer to the client */

    char *username;          /* The username for client authentication */
    char *password;          /* The password for client authentication */
                             /* If _both_ of the above fields are "",
			      * authentication is disabled. */
    unsigned char auth_method; /* The SOCKS5 authentication method to use */
    unsigned char auth_result; /* The SOCKS5 authentication result */

    char *host;              /* The hostname we're trying to CONNECT to */
    unsigned short port;     /* The port we're trying to CONNECT to */
#ifdef WIN32
    in_addr_t ipaddr;        /* The resolved address */
#else
    int dnsfd;               /* The fd to the DNS resolver */
#endif
    struct sockaddr_in sin;  /* The resolved address/port */

    SOCKET sfd;              /* fd to the server */
    int flags;               /* Flags to control the SOCKS proxy */
};

static void socksproxy_free_data(void *data)
{
    struct socksproxy_data *sdata = data;

    if (sdata->cfd >= 0) closesocket(sdata->cfd);
    buffer_zero(&(sdata->crbuf));
    buffer_zero(&(sdata->cwbuf));
    free(sdata->username);
    free(sdata->password);
    free(sdata->host);
#ifndef WIN32
    if (sdata->dnsfd >= 0) close(sdata->dnsfd);
#endif
    if (sdata->sfd >= 0) closesocket(sdata->sfd);
    free(sdata);
}

static void socksproxy_result(NetState *ns, unsigned char code)
{
    struct socksproxy_data *sdata = ns->data;
    unsigned char resp[10] = { 0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00 };

    if (code) {
	resp[1] = code;
	buffer_append(&(sdata->cwbuf), resp, 10);
	sdata->state = SOCKSPROXY_WRITECLOSE;
    } else {
	/* Get the local address of the server socket */
	struct sockaddr_in sin;
	socklen_t sinlen = sizeof(sin);
	int res = getsockname(sdata->sfd, (struct sockaddr *)&sin, &sinlen);
	if (!res) {
	    memmove(resp + 4, &(sin.sin_addr.s_addr), 4);
	    memmove(resp + 8, &(sin.sin_port), 2);
	}
	buffer_append(&(sdata->cwbuf), resp, 10);
	sdata->state = SOCKSPROXY_WRITERESP;
    }
}

static int socksproxy_connection_allowed(NetState *ns)
{
    struct socksproxy_data *sdata = ns->data;

    /* Only allow SOCKS connections to port 5190 */
    if (sdata->port == 5190) return 1;

    return 0;
}

static void socksproxy_start_connection(NetState *ns)
{
    struct socksproxy_data *sdata = ns->data;
    int res;

    /* Make a new non-blocking socket and start the connect */
    sdata->sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sdata->sfd < 0) {
	socksproxy_result(ns, 1);
	return;
    }
#ifndef WIN32
    fcntl(sdata->sfd, F_SETFL, O_NONBLOCK);
#endif

#ifdef DEBUG
    fprintf(stderr, "Trying to connect... ");
    fflush(stderr);
#endif

    res = connect(sdata->sfd, (struct sockaddr *)&(sdata->sin),
	    sizeof(sdata->sin));
    if (res == 0) {
	/* It's already connected. */
	socksproxy_result(ns, 0);
#ifdef DEBUG
	fprintf(stderr, "success\n");
	fflush(stderr);
#endif
#ifndef WIN32
    } else if (errno == EINPROGRESS) {
	/* We need to wait for the connect to finish */
	sdata->state = SOCKSPROXY_WAITING_CONNECT;
#ifdef DEBUG
	fprintf(stderr, "in progress\n");
	fflush(stderr);
#endif
#endif
    } else {
	/* Some other failure */
	socksproxy_result(ns, 1);
#ifdef DEBUG
	fprintf(stderr, "failed\n");
	fflush(stderr);
#endif
    }
}

static int socksproxy_fdset(NetState *ns, fd_set *rfdp, fd_set *wfdp,
	int *maxfdp)
{
    struct socksproxy_data *sdata = ns->data;
    int res = -1;

    switch(sdata->state) {
	case SOCKSPROXY_READING_METHODS:
	case SOCKSPROXY_READING_AUTH:
	case SOCKSPROXY_READING_REQUEST:
	    FD_SET(sdata->cfd, rfdp);
	    if (*maxfdp < sdata->cfd) *maxfdp = sdata->cfd;
	    break;
	case SOCKSPROXY_WRITING_METHOD:
	case SOCKSPROXY_WRITING_AUTHRESULT:
	case SOCKSPROXY_WRITERESP:
	case SOCKSPROXY_WRITECLOSE:
	    FD_SET(sdata->cfd, wfdp);
	    if (*maxfdp < sdata->cfd) *maxfdp = sdata->cfd;
	    break;
	case SOCKSPROXY_WAITING_DNS:
#ifdef WIN32
	    res = 0;
#else
	    FD_SET(sdata->dnsfd, rfdp);
	    if (*maxfdp < sdata->dnsfd) *maxfdp = sdata->dnsfd;
#endif
	    break;
	case SOCKSPROXY_WAITING_CONNECT:
	    FD_SET(sdata->sfd, wfdp);
	    if (*maxfdp < sdata->sfd) *maxfdp = sdata->sfd;
	    break;
    }
    return res;
}

static void socksproxy_handle(NetState *ns, fd_set *rfdp, fd_set *wfdp)
{
    struct socksproxy_data *sdata = ns->data;

    switch(sdata->state) {
	case SOCKSPROXY_READING_METHODS:
	    if (FD_ISSET(sdata->cfd, rfdp)) {
		int res;
		unsigned char nmethods;
		unsigned char *methods;
		int need_auth = 1;
		unsigned char authbuf[2];

		Buffer *b = &(sdata->crbuf);
		res = buffer_readfd(sdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}

		/* Have we read the methods? */
		if (b->bufsize < 1) break;
		if (b->buf[0] != 0x05) {
		    /* Wrong version! */
		    netstate_del(ns);
		    return;
		}
		if (b->bufsize < 2) break;
		nmethods = b->buf[1];
		if (b->bufsize < 2 + nmethods) break;
		methods = b->buf + 2;

		/* Do we need to authenticate? */
		if (sdata->username[0] == '\0' && sdata->password[0] == '\0') {
		    need_auth = 0;
		}

		if (need_auth == 0) {
		    /* No authentication needed */
		    sdata->auth_method = 0x00;
		} else {
		    /* See if username/password (0x02) was one of the
		     * methods offered. */
		    int i;
		    int found_pw = 0;
		    for (i=0;i<nmethods;++i) {
			if (methods[i] == 0x02) {
			    found_pw = 1;
			    break;
			}
		    }
		    if (found_pw) {
			sdata->auth_method = 0x02;
		    } else {
			sdata->auth_method = 0xff;
		    }
		}
		authbuf[0] = 0x05;
		authbuf[1] = sdata->auth_method;
		buffer_append(&(sdata->cwbuf), authbuf, 2);
		buffer_discard(b, 2 + nmethods);
		sdata->state = SOCKSPROXY_WRITING_METHOD;
	    }
	    break;
	case SOCKSPROXY_WRITING_METHOD:
	    if (FD_ISSET(sdata->cfd, wfdp)) {
		int res;

		Buffer *b = &(sdata->cwbuf);
		res = buffer_writefd(sdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}

		if (b->bufsize == 0) {
		    switch (sdata->auth_method) {
			case 0x00:
			    sdata->state = SOCKSPROXY_READING_REQUEST;
			    break;
			case 0x02:
			    sdata->state = SOCKSPROXY_READING_AUTH;
			    break;
			default:
			    netstate_del(ns);
			    return;
		    }
		}
	    }
	    break;
	case SOCKSPROXY_READING_AUTH:
	    if (FD_ISSET(sdata->cfd, rfdp)) {
		int res;
		unsigned char authresbuf[2];
		unsigned char *username = NULL;
		unsigned char *password = NULL;
		unsigned char userlen = 0;
		unsigned char passlen = 0;

		Buffer *b = &(sdata->crbuf);
		res = buffer_readfd(sdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}

		if (b->bufsize < 2) break;
		if (b->buf[0] != 0x01) {
		    /* Wrong version! */
		    authresbuf[0] = 0x01;
		    authresbuf[1] = sdata->auth_result;
		    buffer_append(&(sdata->cwbuf), authresbuf, 2);
		    sdata->state = SOCKSPROXY_WRITING_AUTHRESULT;
		    break;
		}
		userlen = b->buf[1];
		if (b->bufsize < 3 + userlen) break;
		passlen = b->buf[2 + userlen];
		if (b->bufsize < 3 + userlen + passlen) break;

		/* Check the supplied username and password */
		username = b->buf + 2;
		password = b->buf + 3 + userlen;

		if (userlen == strlen(sdata->username) &&
			passlen == strlen(sdata->password) &&
			!strncmp(sdata->username, username, userlen) &&
			!strncmp(sdata->password, password, passlen)) {
		    /* Successful authentication */
		    sdata->auth_result = 0x00;
		}
		authresbuf[0] = 0x01;
		authresbuf[1] = sdata->auth_result;
		buffer_append(&(sdata->cwbuf), authresbuf, 2);
		buffer_discard(b, 3 + userlen + passlen);
		sdata->state = SOCKSPROXY_WRITING_AUTHRESULT;
	    }
	    break;
	case SOCKSPROXY_WRITING_AUTHRESULT:
	    if (FD_ISSET(sdata->cfd, wfdp)) {
		int res;

		Buffer *b = &(sdata->cwbuf);
		res = buffer_writefd(sdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}

		if (b->bufsize == 0) {
		    switch (sdata->auth_result) {
			case 0x00:
			    sdata->state = SOCKSPROXY_READING_REQUEST;
			    break;
			default:
			    netstate_del(ns);
			    return;
		    }
		}
	    }
	    break;
	case SOCKSPROXY_READING_REQUEST:
	    if (FD_ISSET(sdata->cfd, rfdp)) {
		int res;

		Buffer *b = &(sdata->crbuf);
		res = buffer_readfd(sdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}

		if (b->bufsize < 4) break;
		if (b->buf[0] != 0x05) {
		    /* Wrong version! */
		    netstate_del(ns);
		    return;
		}
		if (b->buf[1] != 0x01) {
		    /* Bad command */
		    socksproxy_result(ns, 7);
		    break;
		}
		if (b->buf[2] != 0x00) {
		    /* Badly formatted command */
		    socksproxy_result(ns, 1);
		    break;
		}

		/* Extract the host and port */
		if (b->buf[3] == 0x01) {
		    /* A 4-byte IP address follows */
		    if (b->bufsize < 10) break;
		    sdata->host = malloc(16);  /* Big enough for a
						  dotted quad IP address
						  and a NUL */
		    assert(sdata->host != NULL);
		    sprintf(sdata->host, "%u.%u.%u.%u",
			    b->buf[4], b->buf[5], b->buf[6], b->buf[7]);
		    sdata->port = (b->buf[8] << 8) + b->buf[9];
		    memset(&(sdata->sin), 0, sizeof(sdata->sin));
		    sdata->sin.sin_family = AF_INET;
		    memmove(&(sdata->sin.sin_addr.s_addr), b->buf + 4, 4);
		    memmove(&(sdata->sin.sin_port), b->buf + 8, 2);
		    buffer_discard(b, 10);
		    if (!socksproxy_connection_allowed(ns)) {
			socksproxy_result(ns, 2);
			break;
		    }
		    socksproxy_start_connection(ns);
		    break;
		} else if (b->buf[3] == 0x03) {
		    unsigned char hostlen;
		    if (b->bufsize < 5) break;
		    hostlen = b->buf[4];
		    if (b->bufsize < 7 + hostlen) break;
		    sdata->host = malloc(hostlen+1);
		    assert(sdata->host != NULL);
		    memmove(sdata->host, b->buf + 5, hostlen);
		    sdata->host[hostlen] = '\0';
		    sdata->port = (b->buf[5 + hostlen] << 8) +
			b->buf[6 + hostlen];
		    buffer_discard(b, 7 + hostlen);
		    if (!socksproxy_connection_allowed(ns)) {
			socksproxy_result(ns, 2);
			break;
		    }
#ifdef WIN32
		    netutil_start_dns_win32(sdata->host, &(sdata->ipaddr));
		    sdata->state = SOCKSPROXY_WAITING_DNS;
#else
		    if (netutil_start_dns(sdata->host, &(sdata->dnsfd))) {
			socksproxy_result(ns, 1);
		    } else {
			sdata->state = SOCKSPROXY_WAITING_DNS;
		    }
#endif
		    break;
		} else {
		    /* Bad address type */
		    socksproxy_result(ns, 8);
		    break;
		}
	    }
	    break;
	case SOCKSPROXY_WAITING_DNS:
#ifdef WIN32
	    if (1) {
#else
	    if (FD_ISSET(sdata->dnsfd, rfdp)) {
#endif
		/* Read the 4-byte address from the fd */
		in_addr_t ipaddr;

#ifdef WIN32
		ipaddr = sdata->ipaddr;
#else
		int res;
		res = read(sdata->dnsfd, &ipaddr, 4);

		/* Close the pipe, and check for error */
		close(sdata->dnsfd);
		sdata->dnsfd = -1;
		if (res < 4 || ipaddr == htonl(-1)) {
		    socksproxy_result(ns, 4);
		    return;
		}
#endif

		/* Construct the sockaddr_in */
		memset(&(sdata->sin), 0, sizeof(sdata->sin));
		sdata->sin.sin_family = AF_INET;
		sdata->sin.sin_port = htons(sdata->port);
		sdata->sin.sin_addr.s_addr = ipaddr;

		socksproxy_start_connection(ns);
#ifdef WIN32  /* This is just so braces will match up */
	    }
#else
	    }
#endif
	    break;
	case SOCKSPROXY_WAITING_CONNECT:
	    if (FD_ISSET(sdata->sfd, wfdp)) {
		/* The connect completed.  But was it successful? */
		struct sockaddr_in sin;
		socklen_t sinlen = sizeof(sin);
		int res = getpeername(sdata->sfd, (struct sockaddr *)&sin,
			&sinlen);
		if (res == 0) {
		    /* Success! */
		    socksproxy_result(ns, 0);
		} else {
		    /* Failure */
		    socksproxy_result(ns, 5);
		}
	    }
	    break;
	case SOCKSPROXY_WRITECLOSE:
	case SOCKSPROXY_WRITERESP:
	    if (FD_ISSET(sdata->cfd, wfdp)) {
		int res;
		Buffer *b = &(sdata->cwbuf);
		res = buffer_writefd(sdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}
		if (b->bufsize == 0) {
		    if (sdata->state == SOCKSPROXY_WRITECLOSE) {
			/* We've written everything we're supposed to.
			 * Close the socket. */
			netstate_del(ns);
			return;
		    } else {
			/* We're done writing the headers back to the
			 * client; start proxying. */
			SOCKET cfd = sdata->cfd;
			SOCKET sfd = sdata->sfd;
			/* Mark these as -1 so they're not closed when
			 * we change to oscarproxy. */
			sdata->cfd = -1;
			sdata->sfd = -1;
			oscarproxy_enter(ns, cfd, sfd, &(sdata->crbuf));
			return;
		    }
		}
	    }
	    break;
    }
}

/* Add a new NetState for acting as a SOCKS5 proxy for the given fd */
void socksproxy_add(SOCKET cfd, int flags, const char *username,
	const char *password)
{
    struct socksproxy_data *sdata;

    sdata = calloc(1, sizeof(struct socksproxy_data));
    assert(sdata != NULL);
    sdata->state = SOCKSPROXY_READING_METHODS;
    sdata->cfd = cfd;
    buffer_new(&(sdata->crbuf));
    buffer_new(&(sdata->cwbuf));
    sdata->username = strdup (username ? username : "");
    sdata->password = strdup (password ? password : "");
    sdata->auth_method = 0xff;
    sdata->auth_result = 0xff;
    sdata->host = NULL;
    sdata->port = 0;
#ifndef WIN32
    sdata->dnsfd = -1;
#endif
    sdata->sfd = -1;
    sdata->flags = flags;

    netstate_add(NETSTATE_SOCKSPROXY, socksproxy_fdset, socksproxy_handle,
	    socksproxy_free_data, sdata);
}

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
#include <time.h>

#include <b64.h>

#include "sockdef.h"
#include "netstate.h"
#include "netutil.h"
#include "buffer.h"
#include "httposcar.h"
#include "httpproxy.h"
#include "oscarproxy.h"

static void httpproxy_free_data(void *data)
{
    struct httpproxy_data *hdata = data;

    if (hdata->cfd >= 0) closesocket(hdata->cfd);
    buffer_zero(&(hdata->crbuf));
    buffer_zero(&(hdata->cwbuf));
    free(hdata->auth64);
    free(hdata->host);
#ifndef WIN32
    if (hdata->dnsfd >= 0) close(hdata->dnsfd);
#endif
    if (hdata->sfd >= 0) closesocket(hdata->sfd);
    free(hdata);
}

/* Extract the pieces of the URI from an HTTP request.  Set *hostp to
 * the hostname, *portp to the port number, *pathp to the path.  *hostp
 * and *pathp must be free()d by the caller.  Return 0 on success, -1 on
 * error.  */
static int request_uri(NetState *ns, char **hostp, unsigned short *portp,
	char **pathp)
{
    struct httpproxy_data *hdata = ns->data;
    Buffer *b = &(hdata->crbuf);
    int uristart, uriend, scanend = -1;
    const char *pathstart, *pathend, *portstart, *hoststart;

    *hostp = NULL;
    *portp = 80;
    *pathp = NULL;

    sscanf(b->buf, "%*s %n%*s%n HTTP/1.%*[0-9]%n",
	    &uristart, &uriend, &scanend);
    /* You can't rely on the return value from scanf when you use %n;
     * the behaviour differs on various systems.  So we check if scanend
     * gets set. */
    if (scanend == -1) {
	return -1;
    }

    hoststart = b->buf + uristart;

    /* Skip the http:// if it's there */
    if (!strncasecmp(hoststart, "http://", 7)) {
	hoststart += 7;
    }

    /* Find the start of the path part */
    pathend = b->buf + uriend;
    pathstart = memchr(hoststart, '/', pathend - hoststart);
    if (pathstart == NULL) pathstart = pathend;

    /* Find the port if it's there */
    portstart = memchr(hoststart, ':', pathstart - hoststart);
    if (portstart) {
	int portsize = -1;
	sscanf(portstart, ":%hu%n", portp, &portsize);
	if (portsize != pathstart - portstart) {
	    return -1;
	}
    } else {
	portstart = pathstart;
    }

    *hostp = malloc(portstart - hoststart + 1);
    if (*hostp == NULL) return -1;
    *pathp = malloc(pathend - pathstart + 1);
    if (*pathp == NULL) {
	free(*hostp);
	*hostp = NULL;
	return -1;
    }
    memmove(*hostp, hoststart, portstart - hoststart);
    (*hostp)[portstart - hoststart] = '\0';
    memmove(*pathp, pathstart, pathend - pathstart);
    (*pathp)[pathend - pathstart] = '\0';

    return 0;
}

static void httpproxy_result(NetState *ns, int resultcode)
{
    struct httpproxy_data *hdata = ns->data;
    Buffer *b = &(hdata->cwbuf);

    if (resultcode == 403) {
	buffer_append(b, "HTTP/1.1 403 Not authorized\r\n\r\n", 31);
	hdata->state = HTTPPROXY_WRITECLOSE;
    } else if (resultcode == 404) {
	buffer_append(b, "HTTP/1.1 404 Not found\r\n\r\n", 26);
	hdata->state = HTTPPROXY_WRITECLOSE;
    } else if (resultcode == 500) {
	buffer_append(b, "HTTP/1.1 500 Server error\r\n\r\n", 29);
	hdata->state = HTTPPROXY_WRITECLOSE;
    } else if (resultcode == 200) {
	buffer_append(b, "HTTP/1.1 200 OK\r\n\r\n", 19);
	hdata->state = HTTPPROXY_WRITEHEADERS;
    }
}

/* Return a pointer to the value associated to the given header.  Note
 * that this points into the existing headers, and so will be terminated
 * by \r\n, not NUL. */
static const char *get_header_value(Buffer *b, const char *header)
{
    size_t headerlen = strlen(header);
    const char *headerp = b->buf;
    while (*headerp != '\r' && *headerp != '\n') {
	const char *nextheader;

	/* Is this the one we're looking for? */
	if (!strncasecmp(headerp, header, headerlen) &&
		headerp[headerlen] == ':') {
	    const char *value = headerp + headerlen + 1;
	    /* Skip leading whitespace */
	    while (*value == ' ' || *value == '\t') ++value;
	    return value;
	}
	/* Skip to the next header */
	nextheader = strchr(headerp, '\n');
	if (!nextheader) return NULL;
	headerp = nextheader + 1;
    }
    return NULL;
}

/* Extract the Content-Length: header from the given buffer */
static int get_content_length(Buffer *b, size_t *content_lengthp)
{
    unsigned int len;
    const char *val = get_header_value(b, "Content-Length");
    if (!val) return -1;
    if (sscanf(val, "%u", &len) == 1) {
	*content_lengthp = (size_t)len;
	return 0;
    }
    return -1;
}

/* Check the username/password given in the connection */
static int check_auth(NetState *ns)
{
    struct httpproxy_data *hdata = ns->data;
    Buffer *b = &(hdata->crbuf);
    const char *authval;

    if (hdata->auth64 == NULL) {
	/* No auth is needed */
	return 1;
    }
    if (hdata->auth64len < 0) {
	/* No auth is allowed */
	return 0;
    }

    authval = get_header_value(b, "Proxy-Authorization");
    /* Were any credentials supplied at all? */
    if (!authval) return 0;

    /* Did they match? */
    return (!strncmp(authval, hdata->auth64, hdata->auth64len) &&
	    (authval[hdata->auth64len] == '\r' ||
	     authval[hdata->auth64len] == '\n'));
}

static int httpproxy_fdset(NetState *ns, fd_set *rfdp, fd_set *wfdp,
	int *maxfdp)
{
    struct httpproxy_data *hdata = ns->data;
    int res = -1;

    switch(hdata->state) {
	Buffer *b;
	const unsigned char *eoh;
	case HTTPPROXY_READING_HEADERS:
	    FD_SET(hdata->cfd, rfdp);
	    if (*maxfdp < hdata->cfd) *maxfdp = hdata->cfd;
	    if (hdata->timeout > 0) {
		time_t now = time(NULL);
		res = (hdata->timeout - now) * 1000;
		if (res < 0) res = 0;
	    }

	    /* Have we read all the headers? */
	    b = &(hdata->crbuf);
	    eoh = buffer_contains(b, "\r\n\r\n", 4);
	    if (eoh) {
		eoh += 4;
	    } else {
		eoh = buffer_contains(b, "\n\n", 2);
		if (eoh) {
		    eoh += 2;
		}
	    }
	    if (eoh) {
		char *urihost = NULL, *uripath = NULL;
		unsigned short uriport;
		if (!check_auth(ns)) {
		    httpproxy_result(ns, 403);
		    FD_SET(hdata->cfd, wfdp);
		    if (*maxfdp < hdata->cfd) *maxfdp = hdata->cfd;
		    break;
		}
		if (b->bufsize >= 4 && !memcmp(b->buf, "GET ", 4)) {
		    if (request_uri(ns, &urihost, &uriport, &uripath) == 0) {
			if (!strncmp(uripath, "/monitor?", 9)) {
			    if (httposcar_datawaiting(ns, uripath)) {
				res = 0;
			    }
			}
			free(urihost);
			free(uripath);
		    }
		}
	    }
	    break;
	case HTTPPROXY_WRITECLOSE:
	case HTTPPROXY_WRITEHEADERS:
	    FD_SET(hdata->cfd, wfdp);
	    if (*maxfdp < hdata->cfd) *maxfdp = hdata->cfd;
	    break;
	case HTTPPROXY_WAITING_DNS:
#ifdef WIN32
	    res = 0;
#else
	    FD_SET(hdata->dnsfd, rfdp);
	    if (*maxfdp < hdata->dnsfd) *maxfdp = hdata->dnsfd;
#endif
	    break;
	case HTTPPROXY_WAITING_CONNECT:
	    FD_SET(hdata->sfd, wfdp);
	    if (*maxfdp < hdata->sfd) *maxfdp = hdata->sfd;
	    break;
    }
    return res;
}

static void httpproxy_handle(NetState *ns, fd_set *rfdp, fd_set *wfdp)
{
    struct httpproxy_data *hdata = ns->data;

    switch(hdata->state) {
	const unsigned char *eoh;
	Buffer *b;
	case HTTPPROXY_READING_HEADERS:
	    b = &(hdata->crbuf);
	    if (FD_ISSET(hdata->cfd, rfdp)) {
		int res;

		res = buffer_readfd(hdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}
	    }

	    /* Have we read all the headers? */
	    eoh = buffer_contains(b, "\r\n\r\n", 4);
	    if (eoh) {
		eoh += 4;
	    } else {
		eoh = buffer_contains(b, "\n\n", 2);
		if (eoh) {
		    eoh += 2;
		}
	    }
	    if (eoh) {
		char *urihost = NULL, *uripath = NULL;
		unsigned short uriport;
		int result = 403;
		if (!check_auth(ns)) {
		    result = 403;
		} else if ((hdata->flags & HTTPPROXY_ALLOW_HTTPS) &&
			b->bufsize >= 8 && !memcmp(b->buf, "CONNECT ", 8)) {
		    if (request_uri(ns, &(hdata->host), &(hdata->port),
				&uripath) == 0 && uripath[0] == '\0'
			    && hdata->port == 5190) {
#ifdef DEBUG
			fprintf(stderr, "%s\n", b->buf);
#endif
#ifdef WIN32
			netutil_start_dns_win32(hdata->host, &(hdata->ipaddr));
#else
			/* We've got a hostname and port.  We need to
			 * resolve the hostname in a non-blocking
			 * manner. */
			if (netutil_start_dns(hdata->host, &(hdata->dnsfd))) {
			    httpproxy_result(ns, 500);
			} else {
			    hdata->state = HTTPPROXY_WAITING_DNS;
			}
#endif
			result = 0;
		    }
		} else if ((hdata->flags & HTTPPROXY_ALLOW_HTTP) &&
			b->bufsize >= 4 && !memcmp(b->buf, "GET ", 4)) {
		    if (request_uri(ns, &urihost, &uriport, &uripath)
			    == 0) {
			if (!strcmp(uripath, "/hello")) {
			    /* HTTP OSCAR hello message */
			    httposcar_hello(ns);
			    result = 0;
			} else if (!strncmp(uripath, "/monitor?", 9)) {
			    /* HTTP OSCAR GET message */
			    int getres = httposcar_get(ns, uripath);
			    if (getres == 0) {
				result = 0;
			    } else if (getres == 1) {
				result = -1;
			    }
			}
		    }
		} else if ((hdata->flags & HTTPPROXY_ALLOW_HTTP) &&
			b->bufsize >= 5 && !memcmp(b->buf, "POST ", 4)) {
		    if (request_uri(ns, &urihost, &uriport, &uripath)
			    == 0) {
			if (!strncmp(uripath, "/data?", 6)) {
			    /* HTTP OSCAR POST message */
			    size_t content_length;

			    if (get_content_length(b, &content_length)
				    == 0) {
				if (b->buf + b->bufsize <
					eoh + content_length) {
				    /* We don't have it all yet */
				    break;
				}
				if (httposcar_post(ns, uripath,
					eoh, content_length) == 0) {
				    result = 0;
				}
			    }
			}
		    }
		}
		if (result > 0) {
		    httpproxy_result(ns, result);
		}
		free(urihost);
		free(uripath);
		if (result >= 0) {
		    buffer_discard(b, eoh - b->buf);
		}
	    }
	    break;
	case HTTPPROXY_WRITECLOSE:
	case HTTPPROXY_WRITEHEADERS:
	    if (FD_ISSET(hdata->cfd, wfdp)) {
		int res;
		Buffer *b = &(hdata->cwbuf);
		res = buffer_writefd(hdata->cfd, b);
		if (res <= 0) {
		    netstate_del(ns);
		    return;
		}
		if (b->bufsize == 0) {
		    if (hdata->state == HTTPPROXY_WRITECLOSE) {
			/* We've written everything we're supposed to.
			 * Close the socket. */
			netstate_del(ns);
			return;
		    } else {
			/* We're done writing the headers back to the
			 * client; start proxying. */
			SOCKET cfd = hdata->cfd;
			SOCKET sfd = hdata->sfd;
			/* Mark these as -1 so they're not closed when
			 * we change to oscarproxy. */
			hdata->cfd = -1;
			hdata->sfd = -1;
			oscarproxy_enter(ns, cfd, sfd, &(hdata->crbuf));
			return;
		    }
		}
	    }
	    break;
	case HTTPPROXY_WAITING_DNS:
#ifdef WIN32
	    if (1) {
#else
	    if (FD_ISSET(hdata->dnsfd, rfdp)) {
#endif
		/* Read the 4-byte address from the fd */
		in_addr_t ipaddr;
		int res;
#ifdef WIN32
		ipaddr = hdata->ipaddr;
#else

		res = read(hdata->dnsfd, &ipaddr, 4);

		/* Close the pipe, and check for error */
		close(hdata->dnsfd);
		hdata->dnsfd = -1;
		if (res < 4 || ipaddr == htonl(-1)) {
		    httpproxy_result(ns, 404);
		    return;
		}
#endif

		/* Construct the sockaddr_in */
		memset(&(hdata->sin), 0, sizeof(hdata->sin));
		hdata->sin.sin_family = AF_INET;
		hdata->sin.sin_port = htons(hdata->port);
		hdata->sin.sin_addr.s_addr = ipaddr;

		/* Make a new non-blocking socket and start the connect */
		hdata->sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (hdata->sfd < 0) {
		    httpproxy_result(ns, 500);
		    return;
		}
#ifndef WIN32
		fcntl(hdata->sfd, F_SETFL, O_NONBLOCK);
#endif

		res = connect(hdata->sfd, (struct sockaddr *)&(hdata->sin),
			sizeof(hdata->sin));
		if (res == 0) {
		    /* It's already connected. */
		    httpproxy_result(ns, 200);
#ifndef WIN32
		} else if (errno == EINPROGRESS) {
		    /* We need to wait for the connect to finish */
		    hdata->state = HTTPPROXY_WAITING_CONNECT;
#endif
		} else {
		    /* Some other failure */
		    httpproxy_result(ns, 500);
		}
#ifdef WIN32  /* Match the braces */
	    }
#else
	    }
#endif
	    break;
	case HTTPPROXY_WAITING_CONNECT:
	    if (FD_ISSET(hdata->sfd, wfdp)) {
		/* The connect completed.  But was it successful? */
		struct sockaddr_in sin;
		socklen_t sinlen = sizeof(sin);
		int res = getpeername(hdata->sfd, (struct sockaddr *)&sin,
			&sinlen);
		if (res == 0) {
		    /* Success! */
		    httpproxy_result(ns, 200);
		} else {
		    /* Failure */
		    httpproxy_result(ns, 404);
		}
	    }
	    break;
    }
}

/* Add a new NetState for acting as an HTTP proxy for the given fd */
void httpproxy_add(SOCKET cfd, int flags, const char *username,
	const char *password)
{
    struct httpproxy_data *hdata;

    hdata = calloc(1, sizeof(struct httpproxy_data));
    assert(hdata != NULL);
    hdata->state = HTTPPROXY_READING_HEADERS;
    hdata->flags = flags;
    hdata->cfd = cfd;
    buffer_new(&(hdata->crbuf));
    buffer_new(&(hdata->cwbuf));

    /* Construct the auth64 string */
    if (username == NULL) username = "";
    if (password == NULL) password = "";
    if (username[0] == '\0' && password[0] == '\0') {
	hdata->auth64 = NULL;
	hdata->auth64len = 0;
    } else {
	size_t userpasslen, maxb64len;
	char *userpass, *auth64;

	hdata->auth64 = NULL;
	hdata->auth64len = -1;

	userpasslen = strlen(username) + 1 + strlen(password);
	maxb64len = ((userpasslen + 2) / 3) * 4;
	userpass = malloc(userpasslen + 1);
	auth64 = malloc(7 + maxb64len);
	if (userpass && auth64) {
	    size_t b64res;
	    sprintf(userpass, "%s:%s", username, password);
	    strcpy(auth64, "Basic ");
	    b64res = otrl_base64_encode(auth64 + 6, userpass, userpasslen);
	    auth64[6 + b64res] = '\0';
	    free(userpass);
	    hdata->auth64 = auth64;
	    hdata->auth64len = 6 + b64res;
	} else {
	    free(userpass);
	    free(auth64);
	    fprintf(stderr, "Out of meemory!\n");
	}
    }
    hdata->host = NULL;
    hdata->port = 0;
#ifndef WIN32
    hdata->dnsfd = -1;
#endif
    hdata->sfd = -1;
    hdata->timeout = 0;

    netstate_add(NETSTATE_HTTPPROXY, httpproxy_fdset, httpproxy_handle,
	    httpproxy_free_data, hdata);
}

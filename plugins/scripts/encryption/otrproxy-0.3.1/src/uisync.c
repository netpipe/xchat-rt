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
#include <fcntl.h>
#include <assert.h>

#include "sockdef.h"
#include "netstate.h"
#include "uisync.h"
#ifdef WIN32
#include "servsock.h"
#endif
#include "proxyevent.h"

#ifdef WIN32
/* Windows, it seems, can't select() on a _pipe().  So we have to create
 * an out-and-out TCP socket for communications within a single process.
 * Ugh! */
#endif

/* UiSyncHandles point to this */
struct s_UiSync {
#ifdef WIN32
    SOCKET sfd;
#else
    int readfd, writefd;
#endif
};

/* NetState data fields point to this */
struct uisync_data {
    int initialized;
#ifdef WIN32
    SOCKET cfd;
#else
    int readfd, writefd;
#endif
};

static int uisync_fdset(NetState *ns, fd_set *rfdp, fd_set *wfdp,
	int *maxfdp)
{
    struct uisync_data *udata = ns->data;
#ifdef WIN32
    FD_SET(udata->cfd, rfdp);
    if (*maxfdp < udata->cfd) *maxfdp = udata->cfd;
#else
    FD_SET(udata->readfd, rfdp);
    if (*maxfdp < udata->readfd) *maxfdp = udata->readfd;
#endif
    return udata->initialized ? -1 : 0;
}

static void uisync_handle(NetState *ns, fd_set *rfdp, fd_set *wfdp)
{
    struct uisync_data *udata = ns->data;
#ifdef WIN32
    SOCKET fd = udata->cfd;
#else
    int fd = udata->readfd;
#endif
    /* See if we need to send the inital messages to the UI */
    if (udata->initialized == 0) {
	proxyevent_socket_state();
	proxyevent_context_state();
	proxyevent_privkey_state();
	udata->initialized = 1;
    }
    if (FD_ISSET(fd, rfdp)) {
	/* Read a command byte from the fd */
	unsigned char cmdbuf[1];
#ifdef WIN32
	int res = recv(fd, cmdbuf, 1, 0);
#else
	int res = read(fd, cmdbuf, 1);
#endif
	if (res < 1) {
	    netstate_del(ns);
	    return;
	}
	if (cmdbuf[0] == UISYNC_CMD_WAIT) {
	    unsigned int locklevel = 1;

#ifdef DEBUG
	    fprintf(stderr, "Proxy thread requested to wait.\n");
#endif
	    /* Reply with an acknowledgement. */
	    cmdbuf[0] = UISYNC_CMD_WAIT_ACK;
#ifdef WIN32
	    res = send(udata->cfd, cmdbuf, 1, 0);
#else
	    res = write(udata->writefd, cmdbuf, 1);
#endif
	    if (res < 1) {
		netstate_del(ns);
		return;
	    }

	    /* Now block until we're told to stop waiting.  If we're
	     * told anything else in the meantime, we ignore it. */
	    do {
#ifdef WIN32
		res = recv(udata->cfd, cmdbuf, 1, 0);
#else
		res = read(udata->readfd, cmdbuf, 1);
#endif
		if (res < 1) {
		    netstate_del(ns);
		    return;
		}
		if (cmdbuf[0] == UISYNC_CMD_STOP_WAITING) {
		    /* Decrement the lock level */
		    --locklevel;
		} else if (cmdbuf[0] == UISYNC_CMD_WAIT) {
		    /* Reply with an acknowledgement, and increment the
		     * lock level. */
		    cmdbuf[0] = UISYNC_CMD_WAIT_ACK;
#ifdef WIN32
		    res = send(udata->cfd, cmdbuf, 1, 0);
#else
		    res = write(udata->writefd, cmdbuf, 1);
#endif
		    if (res < 1) {
			netstate_del(ns);
			return;
		    }
		    ++locklevel;
		}
	    } while(locklevel > 0);
#ifdef DEBUG
	    fprintf(stderr, "Proxy thread done waiting.\n");
#endif
	}
    }
}

static void uisync_free_data(void *data)
{
    struct uisync_data *udata = data;

#ifdef WIN32
    closesocket(udata->cfd);
#else
    close(udata->readfd);
    close(udata->writefd);
#endif
    free(udata);
}

#ifdef WIN32
/* Listen for UI synchronization requests on this SOCKET.  Write our
 * acknowledgements to the same SOCKET. */
void uisync_add(SOCKET cfd, unsigned int flags)
#else
/* Listen for UI synchronization requests on this given readfd.  Write
 * our acknowledgements on the given writefd.  These fds are ints (from
 * pipe() for example), and not SOCKETs. */
static void uisync_add(int readfd, int writefd)
#endif
{
    struct uisync_data *udata;

    udata = calloc(1, sizeof(struct uisync_data));
    assert(udata != NULL);
#ifdef WIN32
    udata->cfd = cfd;
#else
    udata->readfd = readfd;
    udata->writefd = writefd;
#endif
    udata->initialized = 0;
    netstate_add(NETSTATE_UISYNC, uisync_fdset, uisync_handle,
	    uisync_free_data, udata);
}

/* If your program requires separate threads for the UI and the proxy,
 * call this before the proxy thread is created.  The resulting
 * UiSyncHandle can be passed to uisync_lock() and uisync_unlock(), and
 * released when the UI exits with uisync_release().  Pass this function
 * the UI event handler and its data. */
UiSyncHandle uisync_setup(void (*handler)(ProxyEvent *event, void *data),
	void *data)
{
    UiSyncHandle h;
    int res;

#ifdef WIN32
    unsigned short uisyncport;
    SOCKET uisyncsock;
    struct sockaddr_in sin;
    int errn;

    /* Create the UI sync server socket */
    uisyncport = servsock_add(INADDR_LOOPBACK, 0, SERVSOCK_PROXY_UISYNC,
	    NULL, NULL, &errn);
    if (uisyncport == 0) {
	return NULL;
    }

    /* Connect a socket to the sync server */
    uisyncsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (uisyncsock < 0) {
	return NULL;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sin.sin_port = htons(uisyncport);
    res = connect(uisyncsock, (struct sockaddr *)&sin, sizeof(sin));
    if (res < 0) {
	closesocket(uisyncsock);
	return NULL;
    }

    h = malloc(sizeof(struct s_UiSync));
    if (h == NULL) {
	closesocket(uisyncsock);
	return NULL;
    }
    h->sfd = uisyncsock;
#else
    int toproxy[2], fromproxy[2];

    /* Create two pipe pairs */
    res = pipe(toproxy);
    if (res < 0) {
	return NULL;
    }
    res = pipe(fromproxy);
    if (res < 0) {
	close(toproxy[0]);
	close(toproxy[1]);
	return NULL;
    }

    h = malloc(sizeof(struct s_UiSync));
    if (h == NULL) {
	close(toproxy[0]);
	close(toproxy[1]);
	close(fromproxy[0]);
	close(fromproxy[1]);
	return NULL;
    }

    h->writefd = toproxy[1];
    h->readfd = fromproxy[0];
    uisync_add(toproxy[0], fromproxy[1]);
#endif
    proxyevent_register(handler, data);
    return h;
}

/* Release a UiSyncHandle.  Note that you will not be able to
 * communicate with the proxy thread after you do this. */
void uisync_release(UiSyncHandle h)
{
#ifdef WIN32
    closesocket(h->sfd);
#else
    close(h->readfd);
    close(h->writefd);
#endif
    free(h);
}

/* Ask the proxy thread to block so that another thread can access proxy
 * or OTR data structures.   BE SURE to call uisync_unlock() soon after.
 * Returns 0 on success, -1 on failure. */
int uisync_lock(UiSyncHandle h)
{
    unsigned char cmdbuf[1];
    int res;

    /* Ask the proxy thread to wait */
    cmdbuf[0] = UISYNC_CMD_WAIT;
#ifdef WIN32
    res = send(h->sfd, cmdbuf, 1, 0);
#else
    res = write(h->writefd, cmdbuf, 1);
#endif
    if (res < 1) {
	return -1;
    }

    /* Wait for an acknowledgement.  Ignore everything but an
     * acknowledgement. */
    do {
#ifdef WIN32
	res = recv(h->sfd, cmdbuf, 1, 0);
#else
	res = read(h->readfd, cmdbuf, 1);
#endif
	if (res < 1) {
	    return -1;
	}
    } while (cmdbuf[0] != UISYNC_CMD_WAIT_ACK);

    return 0;
}

/* Unblock the proxy thread.  Be sure to call this soon after calling
 * uisync_lock(). */
void uisync_unlock(UiSyncHandle h)
{
    unsigned char cmdbuf[1];

    /* Ask the proxy thread to stop waiting */
    cmdbuf[0] = UISYNC_CMD_STOP_WAITING;
#ifdef WIN32
    send(h->sfd, cmdbuf, 1, 0);
#else
    write(h->writefd, cmdbuf, 1);
#endif
}

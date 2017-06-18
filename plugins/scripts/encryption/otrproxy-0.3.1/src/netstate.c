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
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#endif
#include "netstate.h"
#include "proxyevent.h"

static NetState *netstate_root = NULL;

/* Create a new NetState with the given properties */
NetState *netstate_add(NetStateType type,
	int (*set_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd,
			    int *maxfdp),
	void (*handle_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd),
	void (*freedata)(void *data),
	void *data)
{
    NetState **p;
    NetState *newh = calloc(1, sizeof(NetState));
#ifdef DEBUG
    fprintf(stderr, "Adding netstate %p\n", newh);
#endif
    assert(newh != NULL);
    newh->type = type;
    newh->handled = 0;
    newh->set_fdsets = set_fdsets;
    newh->handle_fdsets = handle_fdsets;
    newh->freedata = freedata;
    newh->data = data;

    /* Now link it to the end of the list */
    p = &netstate_root;
    while(*p) {
	p = &((*p)->next);
    }
    *p = newh;
    newh->next = NULL;
    newh->tous = p;
    proxyevent_socket_state();
    return newh;
}

/* Change a NetState to have the given properties */
void netstate_change(NetState *ns, NetStateType type,
	int (*set_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd,
			    int *maxfdp),
	void (*handle_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd),
	void (*freedata)(void *data),
	void *data)
{
    if (ns->data && ns->freedata) ns->freedata(ns->data);
    ns->type = type;
    ns->set_fdsets = set_fdsets;
    ns->handle_fdsets = handle_fdsets;
    ns->freedata = freedata;
    ns->data = data;
    proxyevent_socket_state();
}

/* Remove the given NetState */
void netstate_del(NetState *ns)
{
#ifdef DEBUG
    fprintf(stderr, "Deleting netstate %p\n", ns);
#endif
    if (ns->data && ns->freedata) ns->freedata(ns->data);
    *(ns->tous) = ns->next;
    if (ns->next) {
	ns->next->tous = ns->tous;
    }
    proxyevent_socket_state();
}

/* Set fd_sets on anything we're waiting for.  Return the number of
 * milliseconds we should block for, or -1 if forever. */
int netstate_fdsets(fd_set *rfdsp, fd_set *wfdsp, int *maxfdp)
{
    NetState *ns = netstate_root;
    int res = -1;

    while(ns) {
	int thisres = ns->set_fdsets(ns, rfdsp, wfdsp, maxfdp);
	if (thisres >= 0 && res == -1) res = thisres;
	if (thisres >= 0 && thisres < res) res = thisres;
	ns = ns->next;
    }

    return res;
}

/* Handle any available data */
void netstate_handle(fd_set *rfdsp, fd_set *wfdsp)
{
    NetState *ns;

    /* First set everyone's handled flag to 0 */
    for (ns = netstate_root; ns; ns = ns->next) {
	ns->handled = 0;
    }

    /* Now repeatedly find the first NetState in the list that hasn't
     * been handled yet, and handle it.  We need to do it this way,
     * because the act of handling a NetState can _remove_ that or other
     * NetStates from our list, and we don't want to end up holding a
     * bad pointer. */
    while (1) {
	for (ns = netstate_root; ns; ns = ns->next) {
	    if (ns->handled == 0) {
		ns->handled = 1;
		ns->handle_fdsets(ns, rfdsp, wfdsp);
		break;
	    }
	}
	if (ns == NULL) {
	    /* We went through the whole list, and didn't find an
	     * unhandled NetState, so we're done. */
	    break;
	}
    }
}

/* Get the first NetState for an iterator */
NetState *netstate_first(void)
{
    return netstate_root;
}

/* Get the next NetState for an interator */
NetState *netstate_next(NetState *ns)
{
    return ns->next;
}

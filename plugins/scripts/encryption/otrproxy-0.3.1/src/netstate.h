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

#ifndef __NETSTATE_H__
#define __NETSTATE_H__

typedef enum {
    NETSTATE_SERVSOCK,
    NETSTATE_SOCKSPROXY,
    NETSTATE_HTTPPROXY,
    NETSTATE_HTTPOSCAR,
    NETSTATE_OSCARPROXY,
    NETSTATE_UISYNC
} NetStateType;

typedef struct s_NetState {
    NetStateType type;
    int handled;
    int (*set_fdsets)(struct s_NetState *ns, fd_set *rfd, fd_set *wfd,
			int *maxfdp);
    void (*handle_fdsets)(struct s_NetState *ns, fd_set *rfd, fd_set *wfd);
    void (*freedata)(void *data);
    void *data;
    struct s_NetState *next;
    struct s_NetState **tous;
} NetState;

/* Create a new NetState with the given properties */
NetState *netstate_add(NetStateType type,
	int (*set_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd,
			    int *maxfdp),
	void (*handle_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd),
	void (*freedata)(void *data),
	void *data);

/* Change a NetState to have the given properties */
void netstate_change(NetState *ns, NetStateType type,
	int (*set_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd,
			    int *maxfdp),
	void (*handle_fdsets)(NetState *ns, fd_set *rfd, fd_set *wfd),
	void (*freedata)(void *data),
	void *data);

/* Remove the given NetState */
void netstate_del(NetState *nh);

/* Set fd_sets on anything we're waiting for.  Return 1 (indicating we
 * shouldn't block on the select()) if any set_fdset we call does so. */
int netstate_fdsets(fd_set *rfdsp, fd_set *wfdsp, int *maxfdp);

/* Handle any available data */
void netstate_handle(fd_set *rfdsp, fd_set *wfdsp);

/* Get the first NetState for an iterator */
NetState *netstate_first(void);

/* Get the next NetState for an interator */
NetState *netstate_next(NetState *ns);

#endif

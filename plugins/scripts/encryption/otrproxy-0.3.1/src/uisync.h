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

#ifndef __UISYNC_H__
#define __UISYNC_H__

#include "sockdef.h"
#include "proxyevent.h"

typedef enum {
    UISYNC_CMD_WAIT,
    UISYNC_CMD_WAIT_ACK,
    UISYNC_CMD_STOP_WAITING
} UiSyncCmdType;

typedef struct s_UiSync *UiSyncHandle;

#ifdef WIN32
/* Listen for UI synchronization requests on this SOCKET.  Write our
 * acknowledgements to the same SOCKET. */
void uisync_add(SOCKET cfd, unsigned int flags);
#endif

/* If your program requires separate threads for the UI and the proxy,
 * call this before the proxy thread is created.  The resulting
 * UiSyncHandle can be passed to uisync_lock() and uisync_unlock(), and
 * released when the UI exits with uisync_release().  Pass this function
 * the UI event handler and its data. */
UiSyncHandle uisync_setup(void (*handler)(ProxyEvent *event, void *data),
	void *data);

/* Ask the proxy thread to block so that another thread can access proxy
 * or OTR data structures.   BE SURE to call uisync_unlock() soon after.
 * Returns 0 on success, -1 on failure. */
int uisync_lock(UiSyncHandle h);

/* Unblock the proxy thread.  Be sure to call this soon after calling
 * uisync_lock(). */
void uisync_unlock(UiSyncHandle h);

#endif

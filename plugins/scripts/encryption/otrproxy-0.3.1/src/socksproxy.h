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

#ifndef __SOCKSPROXY_H__
#define __SOCKSPROXY_H__

#include "sockdef.h"

/* Flags to control the SOCKS proxy */
#define SOCKSPROXY_ALLOW_SOCKS5 1

/* Add a new NetState for acting as a SOCKS5 proxy for the given fd */
void socksproxy_add(SOCKET cfd, int flags, const char *username,
	const char *password);

#endif

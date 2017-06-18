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

#ifndef __SOCKDEF_H__
#define __SOCKDEF_H__

/* Make sure SOCKET, SHUT_WR, SHUT_RD, in_addr_t, and closesocket are
 * correctly defined on all platforms. */

#include "../config.h"    /* To get socklen_t, if necessary */

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef unsigned int in_addr_t;
#define SHUT_WR SD_SEND
#define SHUT_RD SD_RECEIVE
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define closesocket close
typedef int SOCKET;
#endif

#endif

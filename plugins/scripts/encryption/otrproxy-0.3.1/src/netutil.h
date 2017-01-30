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

#ifndef __NETUTIL_H__
#define __NETUTIL_H__

#ifdef WIN32
/* Do a blocking DNS lookup of the given host, and set *addrp to its IP
 * address (or 0xffffffff on error). */
int netutil_start_dns_win32(const char *host, in_addr_t *addrp);
#else
/* Start a non-blocking DNS lookup of the given host.  Set *fdp to a
 * file descriptor we can read() four bytes (the IP address) from.
 * Return -1 if we fail to start the DNS lookup, 0 otherwise.  If the
 * lookup itself fails, 0xffffffff will be written to the fd. */
int netutil_start_dns(const char *host, int *fdp);
#endif

#endif

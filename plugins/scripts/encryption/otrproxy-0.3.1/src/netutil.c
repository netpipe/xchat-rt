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
#ifdef WIN32
#define BLOCKING_DNS
#else
#include <netdb.h>
#include <sys/wait.h>
#endif

#include "sockdef.h"

#ifdef DEBUG
#define BLOCKING_DNS
#endif

#ifdef WIN32
/* Do a blocking DNS lookup of the given host, and set *addrp to its IP
 * address (or 0xffffffff on error). */
int netutil_start_dns_win32(const char *host, in_addr_t *addrp)
#else
/* Start a non-blocking DNS lookup of the given host.  Set *fdp to a
 * file descriptor we can read() four bytes (the IP address) from.
 * Return -1 if we fail to start the DNS lookup, 0 otherwise.  If the
 * lookup itself fails, 0xffffffff will be written to the fd. */
int netutil_start_dns(const char *host, int *fdp)
#endif
{
#ifndef WIN32
    int fds[2];
#endif
#ifndef BLOCKING_DNS
    pid_t childpid;
#else
    struct hostent *hent;
#endif

#ifndef WIN32
    if (pipe(fds) < 0) {
	return -1;
    }
#endif

#ifndef BLOCKING_DNS
    childpid = fork();
    if (childpid == -1) {
	/* We couldn't fork! */
	return -1;
    }
    if (childpid == 0) {
	struct hostent *hent;

	/* We're the child.  Fork again, and exit.  This makes our
	 * parent (the proxy) able to waitpid() for us right away. */
	if (fork()) exit(0);

	/* Close the half of the pipe we're not interested in. */
	close(fds[0]);
#endif
#ifdef DEBUG
	fprintf(stderr, "Looking up IP address for %s...", host);
	fflush(stderr);
#endif

	/* Resolve the IP address of sdata->host, and write it to fds[1] */
	hent = gethostbyname(host);

#ifdef WIN32
	if (hent == NULL) {
	    *addrp = (in_addr_t)(-1);
	} else {
	    memmove(addrp, hent->h_addr, 4);
	}
#else
	if (hent == NULL) {
	    in_addr_t error = htonl(-1);
	    write(fds[1], &error, 4);
	} else {
	    write(fds[1], hent->h_addr, 4);
	}
	close(fds[1]);
#endif
#ifdef DEBUG
	fprintf(stderr, " %s\n", hent ? "success" : "failed");
	fflush(stderr);
#endif

#ifndef BLOCKING_DNS
	exit(0);
    } else {
	/* We're the proxy.  Close the write half of the pipe, and
	 * Waitpid for the child (which exits immediately). */
	close(fds[1]);
	waitpid(childpid, NULL, 0);
#endif

#ifndef WIN32
	*fdp = fds[0];
#endif
	return 0;
#ifndef BLOCKING_DNS
    }
#endif
}

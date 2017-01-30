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
#include <string.h>
#include <unistd.h>

#include "sockdef.h"
#include "buffer.h"

/* Initialize an already-allocated Buffer structure */
void buffer_new(Buffer *b)
{
    b->buf = malloc(1024);
    assert(b->buf != NULL);
    b->buf[0] = '\0';
    b->bufsize = 0;
    b->bufalloc = 1024;
}

/* Deinitialize a Buffer structure */
void buffer_zero(Buffer *b)
{
    free(b->buf);
    b->buf = NULL;
    b->bufsize = 0;
    b->bufalloc = 0;
}

/* Append data to a Buffer */
void buffer_append(Buffer *b, const unsigned char *data, size_t len)
{
    size_t newsize = b->bufsize + len;
    if (newsize + 1 > b->bufalloc) {
	size_t newalloc = newsize + 1000;
	unsigned char *newbuf = realloc(b->buf, newalloc);
	assert(newbuf != NULL);
	b->buf = newbuf;
	b->bufalloc = newalloc;
    }
    memmove(b->buf + b->bufsize, data, len);
    b->bufsize = newsize;
    b->buf[b->bufsize] = '\0';
}

#ifdef DEBUG
static void debugdump(unsigned char *d, int amt)
{
    while (amt > 0) {
	int piece = 32;
	int i;
	if (piece > amt) piece = amt;
	for(i=0;i<piece;++i) {
	    fprintf(stderr, "%02x", d[i]);
	}
	fprintf(stderr, "\n");
	d += piece;
	amt -= piece;
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}
#endif

/* Read from a fd into a Buffer */
int buffer_readfd(SOCKET fd, Buffer *b)
{
    unsigned char d[2048];
    int res;
    
#ifdef WIN32
    res = recv(fd, d, sizeof(d), 0);
#else
    res = read(fd, d, sizeof(d));
#endif
#ifdef DEBUG
    fprintf(stderr, "Read %d from %d:\n", res, fd);
    debugdump(d, res);
#endif
    if (res > 0) {
	buffer_append(b, d, res);
    }
    return res;
}

/* Remove data from the head of a Buffer */
void buffer_discard(Buffer *b, size_t amt)
{
    size_t newsize;

    if (amt > b->bufsize) amt = b->bufsize;
    if (amt == 0) return;
    newsize = b->bufsize - amt;
    memmove(b->buf, b->buf + amt, newsize + 1);
    b->bufsize = newsize;
}

/* Write from a Buffer to an fd */
int buffer_writefd(SOCKET fd, Buffer *b)
{
    int res;
    
#ifdef WIN32
    res = send(fd, b->buf, b->bufsize, 0);
#else
    res = write(fd, b->buf, b->bufsize);
#endif
#ifdef DEBUG
    fprintf(stderr, "Wrote %d to %d:\n", res, fd);
    debugdump(b->buf, res);
#endif
    if (res <= 0) return res;
    buffer_discard(b, res);
    return res;
}

/* Does the buffer contain the given string?  Return a pointer to it, or
 * NULL. */
const unsigned char *buffer_contains(Buffer *b, const unsigned char *s,
	size_t slen)
{
    const unsigned char *p = b->buf;
    const unsigned char *bufend = b->buf + b->bufsize;
    while(p + slen <= bufend) {
	if (!memcmp(p, s, slen)) return p;
	++p;
    }
    return NULL;
}

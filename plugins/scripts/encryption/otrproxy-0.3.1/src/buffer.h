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

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "sockdef.h"

typedef struct s_Buffer {
    unsigned char *buf;
    size_t bufsize;
    size_t bufalloc;
} Buffer;

/* Initialize an already-allocated Buffer structure */
void buffer_new(Buffer *b);

/* Deinitialize a Buffer structure */
void buffer_zero(Buffer *b);

/* Append data to a Buffer */
void buffer_append(Buffer *b, const unsigned char *data, size_t len);

/* Read from a fd into a Buffer */
int buffer_readfd(SOCKET fd, Buffer *b);

/* Remove data from the head of a Buffer */
void buffer_discard(Buffer *b, size_t amt);

/* Write from a Buffer to an fd */
int buffer_writefd(SOCKET fd, Buffer *b);

/* Does the buffer contain the given string?  Return a pointer to it, or
 * NULL. */
const unsigned char *buffer_contains(Buffer *b, const unsigned char *s,
	size_t slen);

#endif

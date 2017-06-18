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

#ifndef __UTIL_H__
#define __UTIL_H__

/* The filenames for our private keys and the fingerprint store */
#define PRIVKEYFNAME "otr.private_key"
#define STOREFNAME "otr.fingerprints"

/* Set the userdir to the given string, if non-NULL. */
void util_userdir_set(const char *userdir);

/* Construct a full path for the file with the given name in the user dir.
 * Whoever calls this must free() the result.  Pass NULL to just get the
 * user dir name. */
char *util_userdir_file(const char *basename);

#endif

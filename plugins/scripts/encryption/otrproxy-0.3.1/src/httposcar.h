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

#ifndef __HTTPOSCAR_H__
#define __HTTPOSCAR_H__

void httposcar_hello(NetState *ns);

int httposcar_post(NetState *ns, const char *uripath,
	const unsigned char *postdata, size_t postlen);

/* Check to see if there is data waiting in a Session referenced by a
 * given sid */
int httposcar_datawaiting(NetState *ns, const char *uripath);

/* Get any waiting HTTP OSCAR messages.  Return -1 on error, 0 on
 * success, and 1 if there's no data for us yet. */
int httposcar_get(NetState *ns, const char *uripath);

#endif

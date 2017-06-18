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

#ifndef __CHARSET_H__
#define __CHARSET_H__

#define CHARSET_OSCAR_ASCII        0
#define CHARSET_OSCAR_UCS_2BE      2
#define CHARSET_OSCAR_ISO_8859_1   3

/* Convert the given string (of the given length) in the given charset
 * to UTF-8. */
char *charset_conv_given_to_utf8(const char *msg, size_t len,
	unsigned short charset);

/* Convert the given UTF-8 string to the best charset OSCAR supports.
 * Place the length of the resulting string into *lenp. */
char *charset_conv_utf8_to_oscar_best(const char *utf8,
	unsigned short *charsetp, size_t *lenp);

#endif

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
#include <string.h>

#include "charset.h"

static unsigned int next_utf8(const char **utf8p, size_t *lenp)
{
    unsigned int val = 0;

    if (*lenp == 0) return 0;
    if (((**utf8p) & 0x80) == 0) {
	val = **utf8p;
	*utf8p += 1;
	*lenp -= 1;
    } else if ( (((**utf8p) & 0xe0) == 0xc0) && (*lenp > 1) &&
	    (((*utf8p)[1] & 0xc0) == 0x80) ) {
	val = (((*utf8p)[0] & 0x1f) << 6) + ((*utf8p)[1] & 0x3f);
	if (val < 0x80) val = 0;
	*utf8p += 2;
	*lenp -= 2;
    } else if ( (((**utf8p) & 0xf0) == 0xe0) && (*lenp > 2) &&
	    (((*utf8p)[1] & 0xc0) == 0x80) &&
	    (((*utf8p)[2] & 0xc0) == 0x80) ) {
	val = (((*utf8p)[0] & 0x0f) << 12) +
	    (((*utf8p)[1] & 0x3f) << 6) + ((*utf8p)[2] & 0x3f);
	if (val < 0x800) val = 0;
	*utf8p += 3;
	*lenp -= 3;
    }

    return val;
}

static void charset_conv_utf8_to_iso_8859_1(char *iso, const char *utf8)
{
    const char *utf8cur = utf8;
    size_t utf8len = strlen(utf8);
    unsigned char *isocur = iso;

    while (utf8len) {
	unsigned int nextpt = next_utf8(&utf8cur, &utf8len);
	if (nextpt == 0)  {
	    break;
	} else if (nextpt < 0x100) {
	    *isocur = nextpt;
	    ++isocur;
	}
    }

    *isocur = '\0';
}

static void charset_conv_utf8_to_ucs_2be(char *ucs, const char *utf8)
{
    const char *utf8cur = utf8;
    size_t utf8len = strlen(utf8);
    unsigned char *ucscur = ucs;

    while (utf8len) {
	unsigned int nextpt = next_utf8(&utf8cur, &utf8len);
	if (nextpt == 0)  {
	    break;
	} else if (nextpt < 0x10000) {
	    ucscur[0] = (nextpt >> 8) & 0xff;
	    ucscur[1] = nextpt & 0xff;
	    ucscur += 2;
	}
    }

    *ucscur = '\0';
}

static char *charset_conv_iso_8859_1_to_utf8(const char *iso, size_t len)
{
    size_t utf8len = 0;
    const unsigned char *isoend = iso + len;
    const unsigned char *isocur = iso;
    char *utf8str = NULL;
    unsigned char *utf8cur = NULL;

    /* See how much room we'll need */
    while (isocur < isoend) {
	if ((*isocur) & 0x80) {
	    utf8len += 2;
	} else if (*isocur) {
	    utf8len += 1;
	} else {
	    break;
	}
	++isocur;
    }

    utf8str = malloc(utf8len + 1);
    if (utf8str == NULL) return NULL;

    utf8cur = utf8str;
    for (isocur = iso; isocur < isoend; ++isocur) {
	if ((*isocur) & 0x80) {
	    utf8cur[0] = 0xc0 + ((*isocur) >> 6);
	    utf8cur[1] = 0x80 + ((*isocur) & 0x3f);
	    utf8cur += 2;
	} else if (*isocur) {
	    *utf8cur = *isocur;
	    utf8cur += 1;
	} else {
	    break;
	}
    }
    *utf8cur = '\0';

    return utf8str;
}

static char *charset_conv_ucs_2be_to_utf8(const char *ucs, size_t len)
{
    size_t utf8len = 0;
    const unsigned char *ucsend = ucs + len;
    const unsigned char *ucscur = ucs;
    char *utf8str = NULL;
    unsigned char *utf8cur = NULL;

    /* See how much room we'll need */
    while (ucscur + 1 < ucsend) {
	int nextpt = ((ucscur[0]) << 8) + ucscur[1];
	if (nextpt == 0) {
	    break;
	} else if (nextpt < 0x80) {
	    utf8len += 1;
	} else if (nextpt < 0x800) {
	    utf8len += 2;
	} else {
	    utf8len += 3;
	}
	ucscur += 2;
    }

    utf8str = malloc(utf8len + 1);
    if (utf8str == NULL) return NULL;

    utf8cur = utf8str;
    for (ucscur = ucs; ucscur + 1 < ucsend; ucscur += 2) {
	int nextpt = ((ucscur[0]) << 8) + ucscur[1];
	if (nextpt == 0) {
	    break;
	} else if (nextpt < 0x80) {
	    utf8cur[0] = nextpt;
	    utf8cur += 1;
	} else if (nextpt < 0x800) {
	    utf8cur[0] = 0xc0 + (nextpt >> 6);
	    utf8cur[1] = 0x80 + (nextpt & 0x3f);
	    utf8cur += 2;
	} else {
	    utf8cur[0] = 0xe0 + (nextpt >> 12);
	    utf8cur[1] = 0x80 + ((nextpt >> 6) & 0x3f);
	    utf8cur[2] = 0x80 + (nextpt & 0x3f);
	    utf8cur += 3;
	}
    }
    *utf8cur = '\0';

    return utf8str;
}

/* Convert the given string (of the given length) in the given charset
 * to UTF-8. */
char *charset_conv_given_to_utf8(const char *msg, size_t len,
	unsigned short charset)
{
    char *retmsg = NULL;

    switch (charset) {
	case CHARSET_OSCAR_UCS_2BE:
	    retmsg = charset_conv_ucs_2be_to_utf8(msg, len);
	    break;
	case CHARSET_OSCAR_ISO_8859_1:
	    retmsg = charset_conv_iso_8859_1_to_utf8(msg, len);
	    break;
	default:
	    retmsg = strdup(msg);
	    break;
    }

    return retmsg;
}

/* Convert the given UTF-8 string to the best charset OSCAR supports.
 * Place the length of the resulting string into *lenp. */
char *charset_conv_utf8_to_oscar_best(const char *utf8,
	unsigned short *charsetp, size_t *lenp)
{
    char *retmsg = NULL;
    const char *utf8cur = utf8;
    size_t utf8len = strlen(utf8);
    int asciilen = 0;
    int isolen = 0;
    int ucslen = 0;

    /* Figure out the best charset for this UTF8. */
    while (utf8len) {
	unsigned int nextpt = next_utf8(&utf8cur, &utf8len);
	if (nextpt == 0)  {
	    break;
	} else if (nextpt < 0x80) {
	    if (asciilen >= 0) asciilen += 1;
	    if (isolen >= 0) isolen += 1;
	    if (ucslen >= 0) ucslen += 2;
	} else if (nextpt < 0x100) {
	    asciilen = -1;
	    if (isolen >= 0) isolen += 1;
	    if (ucslen >= 0) ucslen += 2;
	} else if (nextpt < 0x10000) {
	    asciilen = -1;
	    isolen = -1;
	    if (ucslen >= 0) ucslen += 2;
	} else {
	    /* There was a char > 0xffff in the input stream, but none
	     * of ASCII, ISO-8859-1, or UCS-2BE can handle such things. */
	    asciilen = -1;
	    isolen = -1;
	    ucslen = -1;
	}
    }

    if (asciilen >= 0) {
	retmsg = strdup(utf8);
	if (retmsg) {
	    *lenp = asciilen;
	} else {
	    *lenp = 0;
	}
	*charsetp = CHARSET_OSCAR_ASCII;
    } else if (isolen >= 0) {
	retmsg = malloc(isolen + 1);
	if (retmsg) {
	    charset_conv_utf8_to_iso_8859_1(retmsg, utf8);
	    *lenp = isolen;
	} else {
	    *lenp = 0;
	}
	*charsetp = CHARSET_OSCAR_ISO_8859_1;
    } else if (ucslen >= 0) {
	retmsg = malloc(ucslen + 1);
	if (retmsg) {
	    charset_conv_utf8_to_ucs_2be(retmsg, utf8);
	    *lenp = ucslen;
	} else {
	    *lenp = 0;
	}
	*charsetp = CHARSET_OSCAR_UCS_2BE;
    } else {
	retmsg = NULL;
	*lenp = 0;
	*charsetp = 0;
    }

    return retmsg;
}

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

#ifndef __ACCOUNTLIST_H__
#define __ACCOUNTLIST_H__

typedef struct s_AccountList {
    char *accountname;
    char *protocol;
    char *fingerprint_text;
    struct s_AccountList *next;
    struct s_AccountList **tous;
} *AccountList;

/* Generate the list of known accounts (with fingerprints, if present). */
AccountList accountlist_generate(void);

/* Free an AccountList */
void accountlist_free(AccountList al);

#endif

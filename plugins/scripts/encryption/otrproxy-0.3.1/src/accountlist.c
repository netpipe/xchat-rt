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

#include <stdlib.h>
#include <string.h>

#include "sockdef.h"
#include "otrproxy.h"
#include "netstate.h"
#include "oscarproxy.h"
#include "accountlist.h"

static void add(AccountList *alp, const char *accountname,
	const char *protocol, const char *fingerprint)
{
    /* Find the spot where we should insert this new record */
    AccountList r;
    AccountList *spot = alp;
    while (*spot) {
	int accountcmp = strcmp(accountname, (*spot)->accountname);
	if (accountcmp < 0) break;
	if (accountcmp == 0) {
	    int protocmp = strcmp(protocol, (*spot)->protocol);
	    if (protocmp < 0) break;
	    /* If we find an exact match, don't add a new record */
	    if (protocmp == 0) return;
	}
	spot = &((*spot)->next);
    }

    /* Make the new record */
    r = malloc(sizeof(struct s_AccountList));
    if (r == NULL) return;

    r->accountname = strdup(accountname);
    r->protocol = strdup(protocol);
    r->fingerprint_text = fingerprint ? strdup(fingerprint) : NULL;

    if (r->accountname == NULL || r->protocol == NULL ||
	    (fingerprint != NULL && r->fingerprint_text == NULL)) {
	free(r->accountname);
	free(r->protocol);
	free(r->fingerprint_text);
	free(r);
	return;
    }

    /* Link it in at this spot */
    r->next = *spot;
    if (r->next) {
	r->next->tous = &(r->next);
    }
    r->tous = spot;
    *spot = r;
}

/* Generate the list of known accounts (with fingerprints, if present). */
AccountList accountlist_generate(void)
{
    AccountList al = NULL;
    OtrlPrivKey *pk;
    NetState *ns;

    /* First add all of the accounts with fingerprints */
    for (pk = otrproxy_userstate->privkey_root; pk; pk = pk->next) {
	char fingerprint[45];
	otrl_privkey_fingerprint(otrproxy_userstate, fingerprint,
		pk->accountname, pk->protocol);
	add(&al, pk->accountname, pk->protocol, fingerprint);
    }

    /* Now add all of the accounts we've got open connections for */
    ns = netstate_first();
    while(ns) {
	if (ns->type == NETSTATE_OSCARPROXY) {
	    struct oscarproxy_data *odata = ns->data;
	    if (odata->logged_in) {
		add(&al, odata->username, OSCAR_PROTOCOL_ID, NULL);
	    }
	}
	ns = netstate_next(ns);
    }

    return al;
}

/* Free an AccountList */
void accountlist_free(AccountList al)
{
    while (al) {
	AccountList next = al->next;
	free(al->accountname);
	free(al->protocol);
	free(al->fingerprint_text);
	free(al);
	al = next;
    }
}

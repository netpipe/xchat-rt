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
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#ifdef DEBUG
#ifndef WIN32
#include <errno.h>
#endif
#endif

#include <gcrypt.h>
#include <proto.h>
#include <context.h>
#include <privkey.h>
#include <message.h>
#include <userstate.h>

#include "sockdef.h"
#include "netstate.h"
#include "buffer.h"
#include "flappacket.h"
#include "otrproxy.h"
#include "oscarproxy.h"
#include "util.h"
#include "proxyevent.h"
#include "charset.h"

/* Inject an OSCAR message */
void oscarproxy_inject_message(NetState *ns, const char *who, const char *msg)
{
    struct oscarproxy_data *odata = ns->data;
    unsigned char wholen = strlen(who);
    unsigned short msglen = strlen(msg);
    char *snacdata = malloc(22 + wholen);
    char *caplist = malloc(2);
    char *msgdata = malloc(msglen + 5);
    char *twodata = malloc(1);
    TLV *captlv = malloc(sizeof(TLV));
    TLV *msgtlv = malloc(sizeof(TLV));
    TLV *twotlv = malloc(sizeof(TLV));
    FlapPacket newfp = malloc(sizeof(struct s_FlapPacket));
    unsigned char *newflapdata;
    size_t newflaplen;

    if (!snacdata || !caplist || !msgdata || !twodata ||
	    !captlv || !msgtlv || !twotlv || !newfp) {
	free(snacdata);
	free(caplist);
	free(msgdata);
	free(twodata);
	free(captlv);
	free(msgtlv);
	free(twotlv);
	free(newfp);
	return;
    }
    memmove(snacdata, "\x00\x04\x00\x06\x00\x00\x00\x00\x00\x00", 10);
    gcry_create_nonce(snacdata + 10, 8);
    snacdata[18] = 0x00;
    snacdata[19] = 0x01;
    snacdata[20] = wholen;
    memmove(snacdata + 21, who, wholen);
    snacdata[21 + wholen] = '\0';

    memmove(msgdata, "\x00\x00\x00\x00", 4);
    memmove(msgdata + 4, msg, msglen);
    msgdata[4 + msglen] = '\0';

    msgtlv->type = 0x0101;
    msgtlv->datalen = 4 + msglen;
    msgtlv->data = msgdata;
    msgtlv->sub_tlv = NULL;
    msgtlv->next = NULL;

    memmove(caplist, "\x01\x00", 2);

    captlv->type = 0x0501;
    captlv->datalen = 1;
    captlv->data = caplist;
    captlv->sub_tlv = NULL;
    captlv->next = msgtlv;

    twodata[0] = '\0';

    twotlv->type = 0x02;
    twotlv->datalen = 0;
    twotlv->data = twodata;
    twotlv->sub_tlv = captlv;
    twotlv->next = NULL;

    newfp->channel = 2;
    newfp->seqno = 0;
    newfp->datalen = 21 + wholen;
    newfp->extralen = 0;
    newfp->data = snacdata;
    newfp->tlv = twotlv;

    flappacket_serialize(newfp, &(odata->sseqno), &newflapdata, &newflaplen);
    odata->swbuf_write(ns, newflapdata, newflaplen);
    free(newflapdata);

    flappacket_free(newfp);
}

#define COOKIE_EXPIRE_SECS 60

static Cookie *cookie_root = NULL;

static void add_cookie(time_t now, const unsigned char *cookie,
	size_t cookie_len, const char *username, const char *boshost)
{
    Cookie *ck = calloc(1, sizeof(Cookie));
    assert(ck != NULL);
    ck->cookie = malloc(cookie_len);
    assert(ck->cookie != NULL);
    memmove(ck->cookie, cookie, cookie_len);
    ck->cookie_len = cookie_len;
    ck->username = strdup(username);
    assert(ck->username != NULL);
    ck->boshost = strdup(boshost);
    assert(ck->boshost != NULL);
    ck->when = now;
    ck->next = cookie_root;
    if (ck->next) {
	ck->next->tous = &(ck->next);
    }
    cookie_root = ck;
    ck->tous = &cookie_root;
}

static void del_cookie(Cookie *ck)
{
    *(ck->tous) = ck->next;
    if (ck->next) {
	ck->next->tous = ck->tous;
    }
    free(ck->cookie);
    free(ck->username);
    free(ck->boshost);
    free(ck);
}

static Cookie *find_cookie(const unsigned char *cookie, size_t cookie_len)
{
    Cookie *ck;

    for (ck = cookie_root; ck; ck = ck->next) {
	if (ck->cookie_len == cookie_len &&
		!memcmp(ck->cookie, cookie, cookie_len)) {
	    return ck;
	}
    }
    return NULL;
}

static void expire_cookies(time_t now)
{
    time_t exptime = now - COOKIE_EXPIRE_SECS;
    Cookie *ck = cookie_root;

    while(ck) {
	Cookie *next = ck->next;
	if (ck->when <= exptime) {
	    del_cookie(ck);
	}
	ck = next;
    }
}

static void oscarproxy_free_data(void *data)
{
    struct oscarproxy_data *odata = data;

    if (odata->atexit) {
	odata->atexit(odata->atexit_data);
    }
    if (odata->cfd >= 0) closesocket(odata->cfd);
    if (odata->sfd >= 0) closesocket(odata->sfd);
    buffer_zero(&(odata->crbuf));
    buffer_zero(&(odata->cwbuf));
    buffer_zero(&(odata->srbuf));
    buffer_zero(&(odata->swbuf));
    free(odata->username);
    free(odata);
}

/* Find the next FLAP packet in the datastream.  Return a pointer to it,
 * and set *lenp to its length.  If we don't have a complete packet yet,
 * set *lenp to 0 (but return non-NULL).  If the data isn't a FLAP
 * packet, return NULL. */
static const unsigned char *flap_packet(Buffer *b, size_t *lenp)
{
    unsigned short payloadlen;
    size_t totlen;
    *lenp = 0;
    if (b->bufsize < 1) return b->buf ? b->buf : (const unsigned char *)"";
    if (b->buf[0] != 0x2a) return NULL;
    if (b->bufsize < 6) return b->buf;
    payloadlen = (b->buf[4] << 8) + b->buf[5];
    totlen = payloadlen + 6;
    if (b->bufsize >= totlen) *lenp = totlen;
    return b->buf;
}

static int oscarproxy_fdset(NetState *ns, fd_set *rfdp, fd_set *wfdp,
	int *maxfdp)
{
    struct oscarproxy_data *odata = ns->data;

    /* See if we've got anything to read */
    if (odata->cfd >= 0 && !odata->clientdone) {
	FD_SET(odata->cfd, rfdp);
#ifdef DEBUG
	fprintf(stderr, "Marking %d for reading\n", odata->cfd);
#endif
	if (*maxfdp < odata->cfd) *maxfdp = odata->cfd;
    }
    if (odata->sfd >= 0 && !odata->serverdone) {
	FD_SET(odata->sfd, rfdp);
#ifdef DEBUG
	fprintf(stderr, "Marking %d for reading\n", odata->sfd);
#endif
	if (*maxfdp < odata->sfd) *maxfdp = odata->sfd;
    }

    /* See if we've got anything to write */
    if (odata->cfd >= 0 && odata->cwbuf.bufsize > 0) {
	FD_SET(odata->cfd, wfdp);
#ifdef DEBUG
	fprintf(stderr, "Marking %d for writing\n", odata->cfd);
#endif
    }
    if (odata->sfd >= 0 && odata->swbuf.bufsize > 0) {
	FD_SET(odata->sfd, wfdp);
#ifdef DEBUG
	fprintf(stderr, "Marking %d for writing\n", odata->sfd);
#endif
    }

    return (odata->have_crdata ? 0 : -1);
}

typedef enum { CLIENT_TO_SERVER, SERVER_TO_CLIENT } CommDir;

static void swbuf_write(NetState *ns, unsigned char *data, size_t len)
{
    struct oscarproxy_data *odata = ns->data;
    Buffer *writebuf = &(odata->swbuf);
    buffer_append(writebuf, data, len);
}

static void cwbuf_write(NetState *ns, unsigned char *data, size_t len)
{
    struct oscarproxy_data *odata = ns->data;
    Buffer *writebuf = &(odata->cwbuf);
    buffer_append(writebuf, data, len);
}

/* OSCAR screennames are case- and space- insensitive.  Modify the given
 * screenname (in place) to turn it into all lowercase, and remove
 * spaces. */
static void oscar_normalize(char *who)
{
    char *s = who;
    char *d = who;

    while (*s) {
	if (isspace(*s)) {
	    ++s;
	} else {
	    *d = tolower(*s);
	    ++s;
	    ++d;
	}
    }
    *d = '\0';
}

static void flap_handle(NetState *ns, CommDir dir,
	const unsigned char *flapdata, size_t flaplen)
{
    struct oscarproxy_data *odata = ns->data;
    FlapPacket flappacket;
    unsigned char *newflapdata;
    size_t newflaplen;
    unsigned short *seqnop;
    void (*writeproc)(NetState *, unsigned char *, size_t);
    unsigned short maj, min;
    time_t now = time(NULL);
    int ignore_message = 0;

    /* Parse the FLAP data */
    flappacket = flappacket_new(flapdata, flaplen);

    if (dir == CLIENT_TO_SERVER) {
	writeproc = odata->swbuf_write;
	seqnop = &(odata->sseqno);
    } else {
	writeproc = odata->cwbuf_write;
	seqnop = &(odata->cseqno);
    }

#ifdef DEBUG
    if (dir == SERVER_TO_CLIENT) {
	flappacket_dump(stderr, "<- ", flappacket);
    }
#endif
#ifdef DEBUG
    if (dir == CLIENT_TO_SERVER) {
	flappacket_dump(stderr, ">- ", flappacket);
    }
#endif

    /* If there's an auth cookie returned to the client, keep a note of it. */
    if (dir == SERVER_TO_CLIENT &&
	    ( ( flappacket_snac(flappacket, &maj, &min) &&
		( (maj == 0x17 && min == 0x03) ||
		  (maj == 0x01 && min == 0x05) ) )
	      || flappacket->channel == 0x04 ) ) {
	TLV *auth_cookie, *username_tlv, *boshost;

	auth_cookie = flappacket_find_tlv(flappacket->tlv, 0x06);
	username_tlv = flappacket_find_tlv(flappacket->tlv, 0x01);
	boshost = flappacket_find_tlv(flappacket->tlv, 0x05);
	if (auth_cookie && boshost && (username_tlv || odata->username)) {
	    const char *username;
	    if (username_tlv) {
		username = username_tlv->data;
	    } else {
		username = odata->username;
	    }

	    add_cookie(now, auth_cookie->data, auth_cookie->datalen,
		    username, boshost->data);
	}
	if (username_tlv) {
	    free(odata->username);
	    odata->username = strdup(username_tlv->data);
	}
    }

    /* If we see it again (presumably on the BOS connection), look it up
     * to verify the username. */
    if (dir == CLIENT_TO_SERVER && flappacket->channel == 0x01) {
	TLV *auth_cookie = flappacket_find_tlv(flappacket->tlv, 0x06);
	if (auth_cookie) {
	    Cookie *ck = find_cookie(auth_cookie->data, auth_cookie->datalen);
	    if (ck) {
		free(odata->username);
		odata->username = ck->username;
		odata->logged_in = 1;
		ck->username = NULL;
		del_cookie(ck);
		proxyevent_socket_state();
	    } else {
		/* We didn't find the cookie.  Abort, so that we don't
		 * inadvertantly send unencrypted messages. */
#ifdef DEBUG
		fprintf(stderr, "Cookie not found!\n");
#endif
		netstate_del(ns);
		return;
	    }
	}
    }

    if ( ( dir == CLIENT_TO_SERVER && flappacket_snac(flappacket, &maj, &min)
	    && maj == 0x04 && min == 0x06) ||
         ( dir == SERVER_TO_CLIENT && flappacket_snac(flappacket, &maj, &min)
	    && maj == 0x04 && min == 0x07) ) {
	const unsigned char *msgdata =
	    flappacket->data + 18 + flappacket->extralen;
	unsigned short msgtype = (msgdata[0] << 8) + msgdata[1];
	unsigned char wholen = msgdata[2];
	char *who = malloc(wholen + 1);
	TLV *msgtlv, *subtlv;

	if (!who) goto out;
	memmove(who, msgdata + 3, wholen);
	who[wholen] = '\0';
	oscar_normalize(who);

	if (msgtype == 1) {
	    msgtlv = flappacket_find_tlv(flappacket->tlv, 0x02);
	    if (msgtlv) {
		subtlv = flappacket_find_tlv(msgtlv->sub_tlv, 0x101);
		if (subtlv && subtlv->datalen >= 4) {
		    unsigned short charset = (subtlv->data[0] << 8) +
			subtlv->data[1];
		    const char *msg = subtlv->data + 4;
		    if (dir == CLIENT_TO_SERVER) {
			gcry_error_t err;
			char *newmessage = NULL;
			char *utf8msg = NULL;
#ifdef DEBUG
			fprintf(stderr, "OUTGOING message type 1 from ``%s'' "
				"to ``%s'': ``%s''\n\n",
				odata->username, who, msg);
#endif

			utf8msg = charset_conv_given_to_utf8(msg,
				subtlv->datalen - 4, charset);

			err = otrl_message_sending(otrproxy_userstate,
				&otrproxy_ui_ops, ns, odata->username,
				OSCAR_PROTOCOL_ID, who,
				utf8msg ? utf8msg : "", NULL,
				&newmessage, NULL, NULL);
			free(utf8msg);
			/* Be super-careful not to send out the
			 * plaintext in the event of an error. */
			if (err && !newmessage) {
			    subtlv->datalen = 4;
			} else if (newmessage) {
			    size_t msglen = strlen(newmessage);
			    char *newdata = malloc(5 + msglen);
			    if (newdata) {
				/* Set the outgoing charset to ASCII,
				 * since that's what the _encoded_ OTR
				 * message is. */
				memset(newdata, 0, 4);
				strcpy(newdata + 4, newmessage);
				subtlv->datalen = 4 + msglen;
				free(subtlv->data);
				subtlv->data = newdata;
			    } else {
				subtlv->datalen = 4;
			    }
			    otrl_message_free(newmessage);
			}
		    } else {
			char *newmessage = NULL;
			OtrlTLV *tlvs = NULL;
#ifdef DEBUG
			fprintf(stderr, "INCOMING message type 1 to ``%s'' "
				"from ``%s'': ``%s''\n\n",
				odata->username, who, msg);
#endif
			ignore_message = otrl_message_receiving(
				otrproxy_userstate, &otrproxy_ui_ops, ns,
				odata->username, OSCAR_PROTOCOL_ID, who,
				msg, &newmessage, &tlvs, NULL, NULL);

			if (newmessage) {
			    size_t convlen;
			    char *newdata;
			    char *convmsg;
			    unsigned short charset;

			    /* We need to convert the UTF-8 message we've
			     * got now into something OSCAR can support. */
			    convmsg = charset_conv_utf8_to_oscar_best(
				    newmessage, &charset, &convlen);

			    newdata = malloc(5 + convlen);
			    if (newdata) {
				newdata[0] = (charset >> 8) & 0xff;
				newdata[1] = charset & 0xff;
				newdata[2] = 0x00;
				newdata[3] = 0x00;
				memmove(newdata + 4, convmsg, convlen);
				newdata[4 + convlen] = '\0';
				subtlv->datalen = 4 + convlen;
				free(subtlv->data);
				subtlv->data = newdata;
			    } else {
				subtlv->datalen = 4;
			    }
			    free(convmsg);
			    otrl_message_free(newmessage);
			}

			if (otrl_tlv_find(tlvs, OTRL_TLV_DISCONNECTED)) {
			    /* Let the user know the other side has
			     * disconnected. */
			    proxyevent_disconnected(odata->username,
				    OSCAR_PROTOCOL_ID, who);
			}

			otrl_tlv_free(tlvs);
		    }
		}
	    }
	} else if (msgtype == 4) {
	    msgtlv = flappacket_find_tlv(flappacket->tlv, 0x05);
	    /* Only handle MTYPE_PLAIN (0x01) messages in the old ICQ
	     * protocol. */
	    if (msgtlv && msgtlv->datalen >= 9 && msgtlv->data[4] == 0x01) {
		const char *msg = msgtlv->data + 8;
		if (dir == CLIENT_TO_SERVER) {
		    gcry_error_t err;
		    char *newmessage = NULL;
#ifdef DEBUG
		    fprintf(stderr, "OUTGOING message type 4 from ``%s'' "
			    "to ``%s'': ``%s''\n\n",
			    odata->username, who, msg);
#endif
		    err = otrl_message_sending(otrproxy_userstate,
			    &otrproxy_ui_ops, ns, odata->username,
			    OSCAR_PROTOCOL_ID, who, msg, NULL,
			    &newmessage, NULL, NULL);
		    /* Be super-careful not to send out the plaintext in
		     * the event of an error. */
		    if (err && !newmessage) {
			msgtlv->datalen = 9;
			msgtlv->data[6] = '\0';
			msgtlv->data[7] = '\0';
			msgtlv->data[8] = '\0';
		    } else if (newmessage) {
			size_t msglen = strlen(newmessage);
			char *newdata = malloc(9 + msglen);
			if (newdata) {
			    memmove(newdata, msgtlv->data, 6);
			    msgtlv->data[6] = msglen & 0xff;
			    msgtlv->data[7] = (msglen >> 8) & 0xff;
			    strcpy(newdata + 8, newmessage);
			    msgtlv->datalen = 9 + msglen;
			    free(msgtlv->data);
			    msgtlv->data = newdata;
			} else {
			    msgtlv->datalen = 9;
			    msgtlv->data[6] = '\0';
			    msgtlv->data[7] = '\0';
			    msgtlv->data[8] = '\0';
			}
			otrl_message_free(newmessage);
		    }
		} else {
		    char *newmessage = NULL;
		    OtrlTLV *tlvs = NULL;
#ifdef DEBUG
		    fprintf(stderr, "INCOMING message type 4 to ``%s'' "
			    "from ``%s'': ``%s''\n\n",
			    odata->username, who, msg);
#endif
		    ignore_message = otrl_message_receiving(
			    otrproxy_userstate, &otrproxy_ui_ops, ns,
			    odata->username, OSCAR_PROTOCOL_ID, who,
			    msg, &newmessage, &tlvs, NULL, NULL);

		    if (newmessage) {
			size_t msglen = strlen(newmessage);
			char *newdata = malloc(5 + msglen);
			if (newdata) {
			    memmove(newdata, msgtlv->data, 4);
			    strcpy(newdata + 4, newmessage);
			    msgtlv->datalen = 4 + msglen;
			    free(msgtlv->data);
			    msgtlv->data = newdata;
			} else {
			    msgtlv->datalen = 4;
			}
			otrl_message_free(newmessage);
		    }

		    if (otrl_tlv_find(tlvs, OTRL_TLV_DISCONNECTED)) {
			/* Let the user know the other side has
			 * disconnected. */
			proxyevent_disconnected(odata->username,
				OSCAR_PROTOCOL_ID, who);
		    }

		    otrl_tlv_free(tlvs);
		}
	    } else if (msgtlv && msgtlv->datalen >= 9 &&
		    msgtlv->data[4] == 0x02) {
		proxyevent_generic_dialog(OTRL_NOTIFY_WARNING,
			"Unhandled type 4 URL message.",
			"Please report this to "
			"otr-users@lists.cypherpunks.ca", NULL);
	    }
	}
	free(who);
    }

#ifdef DEBUG
    if (dir == SERVER_TO_CLIENT) {
	flappacket_dump(stderr, "-< ", flappacket);
    }
#endif
#ifdef DEBUG
    if (dir == CLIENT_TO_SERVER) {
	flappacket_dump(stderr, "-> ", flappacket);
    }
#endif

    /* Serialize */
    if (!ignore_message) {
	flappacket_serialize(flappacket, seqnop, &newflapdata, &newflaplen);
	writeproc(ns, newflapdata, newflaplen);
	free(newflapdata);
    }

out:
    flappacket_free(flappacket);

    /* Expire any old cookie we may have */
    expire_cookies(now);
}

static void oscarproxy_handle(NetState *ns, fd_set *rfdp, fd_set *wfdp)
{
    struct oscarproxy_data *odata = ns->data;

    if (odata->cfd >= 0 && FD_ISSET(odata->cfd, wfdp)) {
	/* If we've buffered up data to write to the client, write it as
	 * soon as we can. */
	int res;
	Buffer *b = &(odata->cwbuf);
#ifdef DEBUG
	fprintf(stderr, "%d is writable\n", odata->cfd);
#endif
	res = buffer_writefd(odata->cfd, b);
	if (res <= 0) {
#ifdef DEBUG
	    fprintf(stderr, "Write to client failed.\n");
#endif
	    netstate_del(ns);
	    return;
	}
	if (b->bufsize == 0 && odata->serverdone) {
	    /* That's all the data in this direction */
	    shutdown(odata->cfd, SHUT_WR);
	    if (odata->clientdone) {
		/* Both sides are done.  Clean up. */
#ifdef DEBUG
		fprintf(stderr, "Cleaning up oscarproxy NetState (C).\n");
#endif
		netstate_del(ns);
		return;
	    }
	}
    }
    if (odata->sfd >= 0 && FD_ISSET(odata->sfd, wfdp)) {
	/* If we've buffered up data to write to the server, write it as
	 * soon as we can. */
	int res;
	Buffer *b = &(odata->swbuf);
#ifdef DEBUG
	fprintf(stderr, "%d is writable\n", odata->sfd);
#endif
	res = buffer_writefd(odata->sfd, b);
	if (res <= 0) {
#ifdef DEBUG
	    fprintf(stderr, "Write to server failed.\n");
#endif
	    netstate_del(ns);
	    return;
	}
	if (b->bufsize == 0 && odata->clientdone) {
	    /* That's all the data in this direction */
	    shutdown(odata->sfd, SHUT_WR);
	    if (odata->serverdone) {
		/* Both sides are done.  Clean up. */
#ifdef DEBUG
		fprintf(stderr, "Cleaning up oscarproxy NetState (S).\n");
#endif
		netstate_del(ns);
		return;
	    }
	}
    }
    if (odata->cfd >= 0 && FD_ISSET(odata->cfd, rfdp)) {
	/* Read data from the client */
	int res;
	Buffer *b = &(odata->crbuf);
#ifdef DEBUG
	fprintf(stderr, "%d is readable\n", odata->cfd);
#endif
	res = buffer_readfd(odata->cfd, b);
	if (res < 0) {
#ifdef DEBUG
	    fprintf(stderr, "Read from client failed: res = %d\n", res);
#ifdef WIN32
	    fprintf(stderr, "WSAGetLastError = %d\n", WSAGetLastError());
#else
	    fprintf(stderr, "errno = %d (%s)\n", errno, strerror(errno));
#endif
#endif
	    netstate_del(ns);
	    return;
	} else if (res == 0) {
	    /* That's all the data from the client */
	    odata->clientdone = 1;
	    shutdown(odata->cfd, SHUT_RD);
	    if (odata->swbuf.bufsize == 0) {
		shutdown(odata->sfd, SHUT_WR);
		if (odata->serverdone) {
		    /* Both sides are done.  Clean up. */
#ifdef DEBUG
		    fprintf(stderr, "Cleaning up oscarproxy NetState (C2).\n");
#endif
		    netstate_del(ns);
		    return;
		}
	    }
	}
    }
    if (odata->sfd >= 0 && FD_ISSET(odata->sfd, rfdp)) {
	/* Read data from the server */
	int res;
	Buffer *b = &(odata->srbuf);
#ifdef DEBUG
	fprintf(stderr, "%d is readable\n", odata->sfd);
#endif
	res = buffer_readfd(odata->sfd, b);
	if (res <= 0) {
#ifdef DEBUG
	    fprintf(stderr, "Read from server failed: res = %d\n", res);
#ifdef WIN32
	    fprintf(stderr, "WSAGetLastError = %d\n", WSAGetLastError());
#else
	    fprintf(stderr, "errno = %d (%s)\n", errno, strerror(errno));
#endif
#endif
	    netstate_del(ns);
	    return;
	} else if (res == 0) {
	    /* That's all the data from the server */
	    odata->serverdone = 1;
	    shutdown(odata->sfd, SHUT_RD);
	    if (odata->swbuf.bufsize == 0) {
		shutdown(odata->cfd, SHUT_WR);
		if (odata->clientdone) {
		    /* Both sides are done.  Clean up. */
#ifdef DEBUG
		    fprintf(stderr, "Cleaning up oscarproxy NetState (S2).\n");
#endif
		    netstate_del(ns);
		    return;
		}
	    }
	}
    }

    /* Here's where we do any interesting things for client->server
     * traffic. */

    while(1) {
	size_t flaplen;
	const char *flapdata;

	flapdata = flap_packet(&(odata->crbuf), &flaplen);
	if (!flapdata) {
#ifdef DEBUG
	    fprintf(stderr, "Not a FLAP packet!\n");
#endif
	    netstate_del(ns);
	    return;
	}
	if (flaplen) {
	    /* We've got a FLAP packet. */
	    flap_handle(ns, CLIENT_TO_SERVER, flapdata, flaplen);
	    buffer_discard(&(odata->crbuf), flaplen);
	} else {
	    break;
	}
    }

    /* Here's where we do any interesting things for server->client
     * traffic. */

    while(1) {
	size_t flaplen;
	const char *flapdata;

	flapdata = flap_packet(&(odata->srbuf), &flaplen);
	if (!flapdata) {
#ifdef DEBUG
	    fprintf(stderr, "Not a FLAP packet!\n");
#endif
	    netstate_del(ns);
	    return;
	}
	if (flaplen) {
	    /* We've got a FLAP packet. */
	    flap_handle(ns, SERVER_TO_CLIENT, flapdata, flaplen);
	    buffer_discard(&(odata->srbuf), flaplen);
	} else {
	    break;
	}
    }

    /* Make sure to clear the have_crdata flag */
    odata->have_crdata = 0;
}

/* Change a NetState to act as an OSCAR proxy between the given client
 * and server fds.  Copy any data in the given buffer to the client read
 * buffer, which we'll create. */
void oscarproxy_enter(NetState *ns, SOCKET cfd, SOCKET sfd, Buffer *crbuf)
{
    struct oscarproxy_data *odata;
    unsigned char seqnos[4];

    odata = calloc(1, sizeof(struct oscarproxy_data));
    assert(odata != NULL);
    odata->cfd = cfd;
    odata->sfd = sfd;
    odata->clientdone = 0;
    odata->serverdone = 0;
    buffer_new(&(odata->crbuf));
    buffer_append(&(odata->crbuf), crbuf->buf, crbuf->bufsize);
    buffer_new(&(odata->cwbuf));
    buffer_new(&(odata->srbuf));
    buffer_new(&(odata->swbuf));
    odata->have_crdata = (crbuf->bufsize > 0);
    gcry_create_nonce(seqnos, 4);
    odata->cseqno = (seqnos[0] << 8)  + seqnos[1];
    odata->sseqno = (seqnos[2] << 8)  + seqnos[3];
    odata->cwbuf_write = cwbuf_write;
    odata->swbuf_write = swbuf_write;
    odata->cwbuf_write_data = NULL;
    odata->swbuf_write_data = NULL;
    odata->username = NULL;
    odata->logged_in = 0;
    odata->atexit = NULL;
    odata->atexit_data = NULL;

#ifdef WIN32
    /* Trillian has a bug wherein it needs to receive and process the
     * last connection setup (e.g. SOCKS5) message, before it can
     * receive the first AIM protocol message (or else it forgets it's
     * received the AIM message).  So just to be safe, on Win32, we'll
     * pause here for a second. */
#ifdef DEBUG
    fprintf(stderr, "Sleeping for a second...\n");
    fflush(stderr);
#endif
    Sleep(1000);
#endif
#ifdef DEBUG
    fprintf(stderr, "Entering OSCAR proxying mode.\n\n");
#endif
    netstate_change(ns, NETSTATE_OSCARPROXY, oscarproxy_fdset,
	    oscarproxy_handle, oscarproxy_free_data, odata);
}

/* Find the NetState corresponding to the OSCAR connection with the
 * given accountname. */
NetState *oscarproxy_find_netstate(const char *accountname)
{
    NetState *ns = netstate_first();
    while (ns) {
	if (ns->type == NETSTATE_OSCARPROXY) {
	    struct oscarproxy_data *odata = ns->data;
	    if (!strcmp(accountname, odata->username) && odata->logged_in) {
		return ns;
	    }
	}
	ns = netstate_next(ns);
    }
    return NULL;
}

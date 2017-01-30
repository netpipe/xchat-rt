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
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include <gcrypt.h>

#include "sockdef.h"
#include "netstate.h"
#include "netutil.h"
#include "buffer.h"
#include "httpproxy.h"
#include "oscarproxy.h"

#define HTTPOSCAR_TIMEOUT 10

typedef struct {
    int channum;
    NetState *ns;
} Channel;

typedef struct s_Session {
    char sid_text[33];
    unsigned int seq;
    Buffer cwbuf;
    size_t num_channels;
    Channel *channels;
    struct s_Session *next;
    struct s_Session **tous;
} Session;

static Session *session_root = NULL;

typedef enum {
    HTTPOSCAR_WAITING_DNS,
    HTTPOSCAR_WAITING_CONNECT
} HttpOscarState;

struct httposcar_data {
    HttpOscarState state;             /* What state are we in */
    Session *session;                 /* The HTTP OSCAR session we are */
    int channum;                      /* The HTTP OSCAR channel we are */
    unsigned short port;              /* The port number to connect to */
#ifdef WIN32
    in_addr_t ipaddr;                 /* The resolved address */
#else
    int dnsfd;                        /* Listen for DNS replies on this fd */
#endif
    SOCKET sfd;                       /* The server fd */
};

static void httposcar_free_data(void *data)
{
    struct httposcar_data *hodata = data;
#ifndef WIN32
    if (hodata->dnsfd >= 0) close(hodata->dnsfd);
#endif
    if (hodata->sfd >= 0) closesocket(hodata->sfd);
    free(hodata);
}

static void session_result(Session *s, unsigned short channum,
	unsigned char result)
{
    Buffer *b;
    unsigned char resbuf[15] = { 0x00, 0x0d, 0x04, 0x43, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff };

    b = &(s->cwbuf);
    resbuf[12] = (channum >> 8) & 0xff;
    resbuf[13] = channum & 0xff;
    resbuf[14] = result;

    buffer_append(b, resbuf, 15);
}

struct s_cleandata {
    Session *s;
    int channum;
};

/* When a oscarproxy NetState exits, call this to clean the parent */
static void clean_channel(void *cleand)
{
    struct s_cleandata *cleandata = cleand;
    size_t i;

    for (i = 0; i < cleandata->s->num_channels; ++i) {
	if (cleandata->s->channels[i].channum == cleandata->channum) {
	    if (cleandata->s->channels[i].ns != NULL) {
		Buffer *b = &(cleandata->s->cwbuf);
		unsigned char resbuf[14] = { 0x00, 0x0c, 0x04, 0x43,
		    0x00, 0x06,
		    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		    0xff, 0xff };

		resbuf[12] = (cleandata->channum >> 8) & 0xff;
		resbuf[13] = cleandata->channum & 0xff;
		buffer_append(b, resbuf, 14);
		cleandata->s->channels[i].ns = NULL;
	    }
	}
    }

    free(cleand);
}

static void httposcar_cwbuf_write(NetState *ns, unsigned char *data,
	size_t len)
{
    struct oscarproxy_data *odata = ns->data;
    struct s_cleandata *cleandata = odata->cwbuf_write_data;
    Buffer *b = &(cleandata->s->cwbuf);
    unsigned char header[14] = {
	0xff, 0xff,  /* length */
	0x04, 0x43,  /* version */
	0x00, 0x05,  /* command: FLAP data */
	0x00, 0x00, 0x80, 0x00, 0x00, 0x00, /* server->client data */
	0xff, 0xff   /* channel */
    };
    unsigned short totlen = len + 12;

    if (totlen < len) return;

    header[0] = totlen >> 8;
    header[1] = totlen & 0xff;
    header[12] = cleandata->channum >> 8;
    header[13] = cleandata->channum & 0xff;
    buffer_append(b, header, 14);
    buffer_append(b, data, len);
}

static void httposcar_result(NetState *ns, unsigned char result)
{
    struct httposcar_data *hodata = ns->data;
    int channum = hodata->channum;

    if (channum < 0 || hodata->session == NULL) return;
    session_result(hodata->session, channum, result);

    if (result) {
	/* Clean up */
	size_t i;
	for (i = 0; i < hodata->session->num_channels; ++i) {
	    Channel *chan = hodata->session->channels + i;
	    if (chan->channum == channum) {
                chan->ns = NULL;
	    }
	}
	netstate_del(ns);
    } else {
	/* Successful connection; move to the oscarproxy state */
	Buffer emptybuf;
	SOCKET sfd = hodata->sfd;
	struct oscarproxy_data *odata;
	struct s_cleandata *cleandata = calloc(1,sizeof(struct s_cleandata));
	assert(cleandata != NULL);
	cleandata->channum = hodata->channum;
	cleandata->s = hodata->session;
	hodata->sfd = -1;
	buffer_new(&emptybuf);
	/* hodata will become invalid after this next call */
	oscarproxy_enter(ns, -1, sfd, &emptybuf);
	odata = ns->data;
	odata->atexit = clean_channel;
	odata->atexit_data = cleandata;
	odata->cwbuf_write = httposcar_cwbuf_write;
	odata->cwbuf_write_data = cleandata;
    }
}

static int httposcar_fdset(NetState *ns, fd_set *rfdp, fd_set *wfdp,
	int *maxfdp)
{
    struct httposcar_data *hodata = ns->data;
    int res = -1;

    switch(hodata->state) {
	case HTTPOSCAR_WAITING_DNS:
#ifdef WIN32
	    res = 0;
#else
	    FD_SET(hodata->dnsfd, rfdp);
	    if (*maxfdp < hodata->dnsfd) *maxfdp = hodata->dnsfd;
#endif
	    break;
	case HTTPOSCAR_WAITING_CONNECT:
	    FD_SET(hodata->sfd, wfdp);
	    if (*maxfdp < hodata->sfd) *maxfdp = hodata->sfd;
	    break;
    }

    return res;
}

static void httposcar_handle(NetState *ns, fd_set *rfdp, fd_set *wfdp)
{
    struct httposcar_data *hodata = ns->data;

    switch(hodata->state) {
	case HTTPOSCAR_WAITING_DNS:
#ifdef WIN32
	    if (1) {
#else
	    if (FD_ISSET(hodata->dnsfd, rfdp)) {
#endif
		/* Read the 4-byte address from the fd */
		in_addr_t ipaddr;
		struct sockaddr_in sin;
		int res;
#ifdef WIN32
		ipaddr = hodata->ipaddr;
#else

		res = read(hodata->dnsfd, &ipaddr, 4);

		/* Close the pipe, and check for error */
		close(hodata->dnsfd);
		hodata->dnsfd = -1;
		if (res < 4 || ipaddr == htonl(-1)) {
		    httposcar_result(ns, 0xfc);
		    return;
		}
#endif

		/* Construct the sockaddr_in */
		memset(&sin, 0, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(hodata->port);
		sin.sin_addr.s_addr = ipaddr;

		/* Make a new non-blocking socket and start the connect */
		hodata->sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (hodata->sfd < 0) {
		    httposcar_result(ns, 0xfc);
		    return;
		}
#ifndef WIN32
		fcntl(hodata->sfd, F_SETFL, O_NONBLOCK);
#endif

		res = connect(hodata->sfd, (struct sockaddr *)&sin,
			sizeof(sin));
		if (res == 0) {
		    /* It's already connected. */
		    httposcar_result(ns, 0x00);
		    return;
#ifndef WIN32
		} else if (errno == EINPROGRESS) {
		    /* We need to wait for the connect to finish */
		    hodata->state = HTTPOSCAR_WAITING_CONNECT;
#endif
		} else {
		    /* Some other failure */
		    httposcar_result(ns, 0xfc);
		    return;
		}
#ifdef WIN32  /* Match the braces */
	    }
#else
	    }
#endif
	    break;
	case HTTPOSCAR_WAITING_CONNECT:
	    if (FD_ISSET(hodata->sfd, wfdp)) {
		/* The connect completed.  But was it successful? */
		struct sockaddr_in sin;
		socklen_t sinlen = sizeof(sin);
		int res = getpeername(hodata->sfd, (struct sockaddr *)&sin,
			&sinlen);
		if (res == 0) {
		    /* Success! */
		    httposcar_result(ns, 0x00);
		    return;
		} else {
		    /* Failure */
		    httposcar_result(ns, 0xfc);
		    return;
		}
	    }
	    break;
    }
}

static void httposcar_add(const char *host, unsigned short port,
	Session *s, unsigned short channum)
{
    struct httposcar_data *hodata;
    NetState *ns;
    Channel *chan, *newchans;
    size_t i;
    
    /* See if we should even attempt the connection */
    if (port != 5190) {
	session_result(s, channum, 0xfc);
	return;
    }

    /* See if there's another channel with this number */
    for (i = 0; i < s->num_channels; ++i) {
	if (s->channels[i].channum == channum) {
	    session_result(s, channum, 0xfc);
	    return;
	}
    }

    /* Make a new channel */
    newchans = realloc(s->channels, (s->num_channels + 1) * sizeof(Channel));
    assert(newchans != NULL);
    s->channels = newchans;
    chan = s->channels + s->num_channels;
    ++(s->num_channels);

    hodata = calloc(1, sizeof(struct httposcar_data));
    assert(hodata != NULL);
    hodata->session = s;
    hodata->channum = channum;
#ifndef WIN32
    hodata->dnsfd = -1;
#endif
    hodata->sfd = -1;
    hodata->port = port;
    hodata->state = HTTPOSCAR_WAITING_DNS;

    ns = netstate_add(NETSTATE_HTTPOSCAR, httposcar_fdset,
	    httposcar_handle, httposcar_free_data, hodata);
    chan->channum = channum;
    chan->ns = ns;
#ifdef WIN32
    netutil_start_dns_win32(host, &(hodata->ipaddr));
#else
    if (netutil_start_dns(host, &(hodata->dnsfd)) < 0) {
	httposcar_result(ns, 0xfc);
    }
#endif
}

static void session_new(const unsigned char sid[16])
{
    int i;

    Session *s = malloc(sizeof(struct s_Session));
    if (s == NULL) return;

    for(i=0;i<16;++i) {
	sprintf(s->sid_text + 2*i, "%02x", sid[i]);
    }
    s->seq = 0;
    buffer_new(&(s->cwbuf));
    s->num_channels = 0;
    s->channels = NULL;

    s->next = session_root;
    if (session_root) {
	session_root->tous = &(s->next);
    }
    session_root = s;
    s->tous = &session_root;
}

static Session *session_find(const char *sid_text)
{
    Session *s = session_root;
    while(s) {
	if (!strncasecmp(s->sid_text, sid_text, 32)) {
	    return s;
	}
	s = s->next;
    }
    return NULL;
}

void httposcar_hello(NetState *ns)
{
    struct httpproxy_data *hdata = ns->data;
    Buffer *wbuf = &(hdata->cwbuf);
    unsigned char sid[16];

    /* This is the host and port we'll tell the AIM client the AOL HTTP
     * proxy is listening on.  This doesn't have to be anything like the
     * truth, since the client will just tell *us* this information, and
     * we can ignore it, or opt to recognize it, or whatever.  But just
     * in case the client does something broken like actually doing a
     * DNS resolution on what we put here, we give a sensible dotted
     * quad.  Similarly, the port we give here is irrelevant. */
    const char *fake_proxy_host = "127.0.0.1";
    unsigned short fake_proxy_port = 80;

    static const char *httpresphdrs =
	"HTTP/1.0 200 OK\r\n"
	"Connection: close\r\n"
	"Content-Type: AIM/HTTP\r\n"
	"Cache-Control: no-cache no-store\r\n"
	"Content-Length: ";

    static const char *httpresppart2 =
	"\x04\x43"                    /* Protocol version */
	"\x00\x02"                    /* Packet type */
	"\x00\x00\x00\x00\x00\x01"    /* Unknown */
	;

    size_t content_length = 2 + 2 + 2 + 6 + 16 + 2 + strlen(fake_proxy_host)
	+ 2;
    char content_length_str[30];
    unsigned char content_length_buf[2];
    unsigned char host_length_buf[2];
    unsigned char port_buf[2];
    size_t proxy_host_len = strlen(fake_proxy_host);
    sprintf(content_length_str, "%lu\r\n\r\n", (unsigned long)content_length);
    content_length_buf[0] = (content_length - 2) >> 8;
    content_length_buf[1] = (content_length - 2) & 0xff;
    host_length_buf[0] = (proxy_host_len) >> 8;
    host_length_buf[1] = (proxy_host_len) & 0xff;
    port_buf[0] = (fake_proxy_port) >> 8;
    port_buf[1] = (fake_proxy_port) & 0xff;

    /* Note that the above Proxy Host and Port aren't actually
     * important; the client will just send back to us whatever we put
     * here. */

    /* Pick a random sessionid.  This doesn't have to be
     * cryptographically secure, just unique. */
    gcry_create_nonce(sid, 16);

    /* Record the sessionid we're using in a new record. */
    session_new(sid);

    buffer_append(wbuf, httpresphdrs, strlen(httpresphdrs));
    buffer_append(wbuf, content_length_str, strlen(content_length_str));
    buffer_append(wbuf, content_length_buf, 2);
    buffer_append(wbuf, httpresppart2, 10);
    buffer_append(wbuf, sid, 16);
    buffer_append(wbuf, host_length_buf, 2);
    buffer_append(wbuf, fake_proxy_host, proxy_host_len);
    buffer_append(wbuf, port_buf, 2);
    hdata->state = HTTPPROXY_WRITECLOSE;
}

int httposcar_post(NetState *ns, const char *uripath,
	const unsigned char *postdata, size_t postlen)
{
    struct httpproxy_data *hdata = ns->data;
    const char *sidtext = strstr(uripath, "sid=");
    const char *seqtext = strstr(uripath, "seq=");
    unsigned int seq;
    unsigned short claimedlen;
    unsigned short command;
    Session *s;
    if (!sidtext || !seqtext || postlen < 12) return -1;
    s = session_find(sidtext + 4);
    if (!s) return -1;
    if (sscanf(seqtext + 4, "%u", &seq) != 1) return -1;

    /* Is this a dup? */
    if (seq <= s->seq) return -1;
    s->seq = seq;

    /* What length does the message claim it is? */
    claimedlen = (postdata[0] << 8) + postdata[1];
    if (claimedlen != postlen - 2) return -1;

    /* Check the protocol version */
    if (postdata[2] != '\x04' || postdata[3] != '\x43') return -1;

    /* What command is this? */
    command = (postdata[4] << 8) + postdata[5];

#ifdef DEBUG
    {int i;
    for(i=0;i<postlen;++i) fprintf(stderr, "%02x", postdata[i]);
    fprintf(stderr, "\n");}
    /* 0019 0443 0003 000000000000 0001 0009 6c6f63616c686f7374 1f90 */
#endif

    switch (command) {
	unsigned short channum, hostlen;
	char *host;
	unsigned short port;
	struct oscarproxy_data *odata;
	size_t i;
	case 3:
	    /* Make a connection */
	    postdata += 12;
	    postlen -= 12;
	    if (postlen < 4) return -1;
	    channum = (postdata[0] << 8) + postdata[1];
	    hostlen = (postdata[2] << 8) + postdata[3];
	    postdata += 4;
	    postlen -= 4;
	    if (postlen != hostlen + 2) return -1;
	    host = malloc(hostlen + 1);
	    if (host == NULL) return -1;
	    memmove(host, postdata, hostlen);
	    host[hostlen] = '\0';
	    postdata += hostlen;
	    postlen -= hostlen;
	    port = (postdata[0] << 8) + postdata[1];
	    buffer_append(&(hdata->cwbuf), "HTTP/1.1 200 OK\r\n"
		    "Connection: close\r\nContent-Length: 0\r\n\r\n", 57);
	    hdata->state = HTTPPROXY_WRITECLOSE;

	    /* Now create a new NetState to do the connection */
	    httposcar_add(host, port, s, channum);

	    return 0;
	case 5:
	    /* FLAP data */
	    postdata += 12;
	    postlen -= 12;
	    if (postlen < 2) return -1;
	    channum = (postdata[0] << 8) + postdata[1];
	    postdata += 2;
	    postlen -= 2;

	    /* Do we know this channel? */
	    odata = NULL;
	    for (i = 0; i < s->num_channels; ++i) {
		if (s->channels[i].channum == channum &&
			s->channels[i].ns != NULL) {
		    odata = s->channels[i].ns->data;
		}
	    }

	    if (odata) {
		Buffer *crbuf = &(odata->crbuf);
		buffer_append(crbuf, postdata, postlen);
		odata->have_crdata = 1;
	    }

	    buffer_append(&(hdata->cwbuf), "HTTP/1.1 200 OK\r\n"
		    "Connection: close\r\nContent-Length: 0\r\n\r\n", 57);
	    hdata->state = HTTPPROXY_WRITECLOSE;

	    return 0;
	case 6:
	    /* Close channel */
	    postdata += 12;
	    postlen -= 12;
	    if (postlen < 2) return -1;
	    channum = (postdata[0] << 8) + postdata[1];
	    postdata += 2;
	    postlen -= 2;

	    /* Do we know this channel? */
	    for (i = 0; i < s->num_channels; ++i) {
		if (s->channels[i].channum == channum &&
			s->channels[i].ns != NULL) {
		    NetState *ns = s->channels[i].ns;
		    s->channels[i].ns = NULL;
		    netstate_del(ns);
		}
	    }

	    buffer_append(&(hdata->cwbuf), "HTTP/1.1 200 OK\r\n"
		    "Connection: close\r\nContent-Length: 0\r\n\r\n", 57);
	    hdata->state = HTTPPROXY_WRITECLOSE;

	    return 0;
	default:
	    /* Unknown command */
	    return -1;
    }


    return -1;
}

/* Check to see if there is data waiting in a Session referenced by a
 * given sid */
int httposcar_datawaiting(NetState *ns, const char *uripath)
{
    const char *sidtext = strstr(uripath, "sid=");
    Session *s;

    if (!sidtext) return 0;
    s = session_find(sidtext + 4);
    if (!s) return 0;

    return (s->cwbuf.bufsize > 0);
}

/* Get any waiting HTTP OSCAR messages.  Return -1 on error, 0 on
 * success, and 1 if there's no data for us yet. */
int httposcar_get(NetState *ns, const char *uripath)
{
    struct httpproxy_data *hdata = ns->data;
    Buffer *wbuf = &(hdata->cwbuf);
    const char *sidtext = strstr(uripath, "sid=");
    time_t now = time(NULL);
    Session *s;

    if (!sidtext) return -1;
    s = session_find(sidtext + 4);
    if (!s) return -1;

    if (s->cwbuf.bufsize == 0) {
	/* We don't have the data yet.  Wait longer. */
	if (hdata->timeout == 0) {
	    hdata->timeout = now + HTTPOSCAR_TIMEOUT;
	}
	if (now < hdata->timeout) {
	    return 1;
	}
    }

    buffer_append(wbuf, "HTTP/1.1 200 OK\r\nConnection: close\r\n"
	    "Content-Type: AIM/HTTP\r\nContent-Length: ", 76);

    if (s->cwbuf.bufsize == 0) {
	/* Output the "no data available" message */
	buffer_append(wbuf, "15\r\n\r\n\x00\x0d\x04\x43\x00\x07"
		"\x00\x00\x00\x00\x00\x00\x00\x01\x00", 21);
    } else {
	char stext[15];
	sprintf(stext, "%u\r\n\r\n", (unsigned int)s->cwbuf.bufsize);
	buffer_append(wbuf, stext, strlen(stext));
	buffer_append(wbuf, s->cwbuf.buf, s->cwbuf.bufsize);
	buffer_discard(&(s->cwbuf), s->cwbuf.bufsize);
    }
    hdata->state = HTTPPROXY_WRITECLOSE;

    return 0;
}

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
#include "flappacket.h"

static size_t tlv_len(TLV *tlv)
{
    size_t totlen = tlv->datalen;
    TLV *sub = tlv->sub_tlv;

    while (sub) {
	totlen += sub->datalen + 4;
	sub = sub->next;
    }

    return totlen;
}

static void tlv_serialize(TLV *tlv, unsigned char **outp)
{
    unsigned short tlvlen = tlv_len(tlv);
    TLV *sub = tlv->sub_tlv;
    (*outp)[0] = tlv->type >> 8;
    (*outp)[1] = tlv->type & 0xff;
    (*outp)[2] = tlvlen >> 8;
    (*outp)[3] = tlvlen & 0xff;
    memmove((*outp) + 4, tlv->data, tlv->datalen);
    (*outp) += tlv->datalen + 4;

    while (sub) {
	tlv_serialize(sub, outp);
	sub = sub->next;
    }
}

static void create_tlvs(TLV **tlvp, unsigned int majmin,
	const unsigned char *tlvdata, const unsigned char *tlvend) {
    while (tlvdata + 4 <= tlvend) {
	unsigned short type = (tlvdata[0] << 8) + tlvdata[1];
	unsigned short len = (tlvdata[2] << 8) + tlvdata[3];
	unsigned short datalen = len;
	unsigned char *datap;
	TLV *sub = NULL;
	if (tlvdata + 4 + len > tlvend) break;
	if ((majmin == 0x040006 || majmin == 0x040007) && type == 0x02) {
	    /* We've got sub-TLVs */
	    create_tlvs(&sub, 0, tlvdata + 4, tlvdata + 4 + len);
	    datalen = 0;
	}
	datap = malloc(datalen + 1);
	assert(datap != NULL);
	memmove(datap, tlvdata+4, datalen);
	datap[datalen] = '\0';
	*tlvp = malloc(sizeof(TLV));
	assert(*tlvp != NULL);
	(*tlvp)->type = type;
	(*tlvp)->datalen = datalen;
	(*tlvp)->sub_tlv = sub;
	(*tlvp)->data = datap;
	(*tlvp)->next = NULL;
	tlvp = &((*tlvp)->next);
	tlvdata += 4 + len;
    }
}

/* Allocate a new FLAP packet from the given data and length */
FlapPacket flappacket_new(const unsigned char *flapdata, size_t flaplen)
{
    FlapPacket fp = malloc(sizeof(struct s_FlapPacket));
    if (fp == NULL) return NULL;
    if (flaplen < 6) return NULL;
    fp->channel = flapdata[1];
    fp->seqno = (flapdata[2] << 8) + flapdata[3];
    fp->datalen = flaplen - 6;
    fp->extralen = 0;
    fp->data = NULL;
    fp->tlv = NULL;

    /* Is the data portion a SNAC that contains TLVs we care about? */
    if (fp->channel == 0x02 && flaplen >= 16) {
	unsigned int majmin = (flapdata[6] << 24) + (flapdata[7] << 16) +
	                      (flapdata[8] << 8) + flapdata[9];
	unsigned short snacflags = (flapdata[10] << 8) + flapdata[11];
	size_t extralen = 0;

	if ((snacflags & 0x8000) && flaplen >= 18) {
	    /* There's extra data */
	    extralen = (flapdata[16] << 8) + flapdata[17] + 2;
	    if (flaplen < 16 + extralen) {
		free(fp);
		return NULL;
	    }
	    fp->extralen = extralen;
	}
	if (majmin == 0x170003 || majmin == 0x010005) {
	    create_tlvs(&(fp->tlv), majmin, flapdata + 16 + extralen,
		    flapdata + flaplen);
	    fp->datalen = 10 + extralen;
	} else if (majmin == 0x040006 && flaplen >= 27 + extralen) {
	    const unsigned char *start = flapdata + 16 + extralen;
	    /* unsigned short msgtype = (start[8] << 8) + start[9]; */
	    unsigned short fromlen = start[10];
	    if (flaplen >= 27 + extralen + fromlen) {
		create_tlvs(&(fp->tlv), majmin, start + 11 + fromlen,
			flapdata + flaplen);
		fp->datalen = 21 + extralen + fromlen;
	    }
	} else if (majmin == 0x040007 && flaplen >= 31 + extralen) {
	    const unsigned char *start = flapdata + 16 + extralen;
	    /* unsigned short msgtype = (start[8] << 8) + start[9]; */
	    unsigned short fromlen = start[10];
	    if (flaplen >= 31 + extralen + fromlen) {
		create_tlvs(&(fp->tlv), majmin, start + 15 + fromlen,
			flapdata + flaplen);
		fp->datalen = 25 + extralen + fromlen;
	    }
	}
    } else if (fp->channel == 0x01 && flaplen >= 10) {
	create_tlvs(&(fp->tlv), 0, flapdata + 10, flapdata + flaplen);
	fp->datalen = 4;
    } else if (fp->channel == 0x04 && flaplen >= 6) {
	create_tlvs(&(fp->tlv), 0, flapdata + 6, flapdata + flaplen);
	fp->datalen = 0;
    }
    fp->data = malloc(fp->datalen + 1);
    if (fp->data == NULL) {
	free(fp);
	return NULL;
    }
    memmove(fp->data, flapdata+6, fp->datalen);
    fp->data[fp->datalen] = '\0';
    return fp;
}

static void dumpdata(FILE *stream, const char *leader, unsigned int depth,
	const unsigned char *data, size_t len)
{
    const unsigned char *dataend = data + len;
    const unsigned char *p;
    const size_t rowsize = 16;

    for (p=data; p<dataend; p+=rowsize) {
	size_t amt = rowsize;
	size_t i;
	if (amt > (dataend-p)) amt = (dataend-p);
	fprintf(stream, "%s%*s  ", leader, 2*depth, "");
	for (i = 0; i < amt; ++i) {
	    fprintf(stream, "%02x", p[i]);
	}
	for (; i < rowsize; ++i) {
	    fprintf(stream, "  ");
	}
	fprintf(stream, "  ");
	for (i = 0; i < amt; ++i) {
	    if (p[i] >= ' ' && p[i] <= '~') {
		fprintf(stream, "%c", p[i]);
	    } else {
		fprintf(stream, ".");
	    }
	}
	fprintf(stream, "\n");
    }
    fflush(stream);
}

static void tlv_dump(FILE *stream, const char *leader, unsigned int depth,
	TLV *tlv)
{
    TLV *sub = tlv->sub_tlv;
    fprintf(stream, "%s%*sTLV_0x%04x:\n", leader, 2*depth, "", tlv->type);
    dumpdata(stream, leader, depth, tlv->data, tlv->datalen);
    while(sub) {
	tlv_dump(stream, leader, depth+1, sub);
	sub = sub->next;
    }
}

/* Dump the contents of a Flap Packet to the given stream.  Each line
 * should be prefixed by the given leader. */
void flappacket_dump(FILE *stream, const char *leader, FlapPacket flappacket)
{
    TLV *tlv;

    fprintf(stream, "%sChannel: %u\n", leader, flappacket->channel);
    fprintf(stream, "%sSeqno: %04x\n", leader, flappacket->seqno);
    fprintf(stream, "%sData:\n", leader);
    dumpdata(stream, leader, 0, flappacket->data, flappacket->datalen);
    for (tlv = flappacket->tlv; tlv; tlv = tlv->next) {
	tlv_dump(stream, leader, 0, tlv);
    }
    fprintf(stream, "\n");
}

/* Serialize the given FLAP packet.  *serialp should be a pointer to the
 * current outgoing serial number (which will be incremented by this
 * routine).  *datap and *lenp will be set to the resulting data and length
 * respectively.  The caller must free(*datap) when he's done with it.
 * Returns 0 on success, -1 on failure. */
int flappacket_serialize(FlapPacket flappacket, unsigned short *serialp,
	unsigned char **datap, size_t *lenp)
{
    unsigned char *data, *p;
    size_t totlen = 6 + flappacket->datalen;
    TLV *tlv;

    /* Find the total length of the TLV segment, if present */
    for (tlv = flappacket->tlv; tlv; tlv = tlv->next) {
	totlen += tlv_len(tlv) + 4;
    }

    if (totlen >= 0x10006) {
	/* The packet's too big! */
	return -1;
    }

    data = malloc(totlen);
    if (data == NULL) return -1;
    *datap = data;
    *lenp = totlen;
    ++(*serialp);
    data[0] = 0x2a;
    data[1] = flappacket->channel;
    data[2] = (*serialp) >> 8;
    data[3] = (*serialp) & 0xff;
    data[4] = (totlen - 6) >> 8;
    data[5] = (totlen - 6) & 0xff;
    memmove(data + 6, flappacket->data, flappacket->datalen);
    p = data + 6 + flappacket->datalen;
    for (tlv = flappacket->tlv; tlv; tlv = tlv->next) {
	tlv_serialize(tlv, &p);
    }
    return 0;
}

/* Deallocate a FLAP packet */
void flappacket_free(FlapPacket flappacket)
{
    TLV *tlv = flappacket->tlv;
    while (tlv) {
	TLV *next = tlv->next;
	free(tlv->data);
	free(tlv);
	tlv = next;
    }
    free(flappacket->data);
    free(flappacket);
}

/* Is the given packet a SNAC packet?  If so, set *majp and *minp
 * appropriately, if they're non-NULL. */
int flappacket_snac(FlapPacket flappacket, unsigned short *majp,
	unsigned short *minp)
{
    if (flappacket->channel != 0x02 || flappacket->datalen < 10) return 0;

    if (majp) *majp = (flappacket->data[0] << 8) + flappacket->data[1];
    if (minp) *minp = (flappacket->data[2] << 8) + flappacket->data[3];

    return 1;
}

/* Find the first TLV with the given type.  Return a pointer to it, or
 * NULL if not present. */
TLV *flappacket_find_tlv(TLV *tlv, unsigned short type)
{
    while(tlv) {
	if (tlv->type == type) return tlv;
	tlv = tlv->next;
    }
    return NULL;
}

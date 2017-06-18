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

#ifndef __FLAPPACKET_H__
#define __FLAPPACKET_H__

typedef struct s_TLV {
    unsigned short type;
    unsigned short datalen;
    unsigned char *data;
    struct s_TLV *sub_tlv;
    struct s_TLV *next;
} TLV;

typedef struct s_FlapPacket {
    unsigned char channel;
    unsigned short seqno;
    unsigned short datalen;
    unsigned short extralen;
    unsigned char *data;
    TLV *tlv;
} * FlapPacket;

/* Allocate a new FLAP packet from the given data and length */
FlapPacket flappacket_new(const unsigned char *flapdata, size_t flaplen);

/* Dump the contents of a Flap Packet to the given stream.  Each line
 * should be prefixed by the given leader. */
void flappacket_dump(FILE *stream, const char *leader, FlapPacket flappacket);

/* Serialize the given FLAP packet.  *serialp should be a pointer to the
 * current outgoing serial number (which will be incremented by this
 * routine).  *datap and *lenp will be set to the resulting data and length
 * respectively.  The caller must free(*datap) when he's done with it.
 * Returns 0 on success, -1 on failure. */
int flappacket_serialize(FlapPacket flappacket, unsigned short *serialp,
	unsigned char **datap, size_t *lenp);

/* Deallocate a FLAP packet */
void flappacket_free(FlapPacket flappacket);

/* Is the given packet a SNAC packet?  If so, set *majp and *minp
 * appropriately, if they're non-NULL. */
int flappacket_snac(FlapPacket flappacket, unsigned short *majp,
	unsigned short *minp);

/* Find the first TLV with the given type.  Return a pointer to it, or
 * NULL if not present. */
TLV *flappacket_find_tlv(TLV *tlv, unsigned short type);

#endif

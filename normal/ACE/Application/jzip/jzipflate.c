/******************************************************************************/
/*! @addtogroup Group2
    @file       jzipflate.c
    @brief      
    @date       2025/12/25
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
#include "jzip.h"

//#include <assert.h>
//#include <limits.h>

#if defined(UINT_MAX) && (UINT_MAX) < 0xFFFFFFFFUL
#  error "jzip requires unsigned int to be at least 32-bit"
#endif

/* -- Internal data structures -- */
struct jzip_tree {
	unsigned short counts[16]; /* Number of codes with a given length */
	unsigned short symbols[288]; /* Symbols sorted by code */
	int max_sym;
};

struct jzip_data {
	const unsigned char *source;
	const unsigned char *source_end;
	unsigned int tag;
	int bitcount;
	int overflow;

	unsigned char *dest_start;
	unsigned char *dest;
	unsigned char *dest_end;

	struct jzip_tree ltree; /* Literal/length tree */
	struct jzip_tree dtree; /* Distance tree */
};

/* -- Utility functions -- */

static unsigned int read_le16(const unsigned char *p)
{
	return ((unsigned int) p[0])
	     | ((unsigned int) p[1] << 8);
}

/* Build fixed Huffman trees */
static void jzip_build_fixed_trees(struct jzip_tree *lt, struct jzip_tree *dt)
{
	int i;

	/* Build fixed literal/length tree */
	for (i = 0; i < 16; ++i) {
		lt->counts[i] = 0;
	}

	lt->counts[7] = 24;
	lt->counts[8] = 152;
	lt->counts[9] = 112;

	for (i = 0; i < 24; ++i) {
		lt->symbols[i] = 256 + i;
	}
	for (i = 0; i < 144; ++i) {
		lt->symbols[24 + i] = i;
	}
	for (i = 0; i < 8; ++i) {
		lt->symbols[24 + 144 + i] = 280 + i;
	}
	for (i = 0; i < 112; ++i) {
		lt->symbols[24 + 144 + 8 + i] = 144 + i;
	}

	lt->max_sym = 285;

	/* Build fixed distance tree */
	for (i = 0; i < 16; ++i) {
		dt->counts[i] = 0;
	}

	dt->counts[5] = 32;

	for (i = 0; i < 32; ++i) {
		dt->symbols[i] = i;
	}

	dt->max_sym = 29;
}

/* Given an array of code lengths, build a tree */
static int jzip_build_tree(struct jzip_tree *t, const unsigned char *lengths,
                           unsigned int num)
{
	unsigned short offs[16];
	unsigned int i, num_codes, available;

	//assert(num <= 288);

	for (i = 0; i < 16; ++i) {
		t->counts[i] = 0;
	}

	t->max_sym = -1;

	/* Count number of codes for each non-zero length */
	for (i = 0; i < num; ++i) {
		//assert(lengths[i] <= 15);

		if (lengths[i]) {
			t->max_sym = i;
			t->counts[lengths[i]]++;
		}
	}

	/* Compute offset table for distribution sort */
	for (available = 1, num_codes = 0, i = 0; i < 16; ++i) {
		unsigned int used = t->counts[i];

		/* Check length contains no more codes than available */
		if (used > available) {
			return JZIP_DATA_ERROR;
		}
		available = 2 * (available - used);

		offs[i] = num_codes;
		num_codes += used;
	}

	/*
	 * Check all codes were used, or for the special case of only one
	 * code that it has length 1
	 */
	if ((num_codes > 1 && available > 0)
	 || (num_codes == 1 && t->counts[1] != 1)) {
		return JZIP_DATA_ERROR;
	}

	/* Fill in symbols sorted by code */
	for (i = 0; i < num; ++i) {
		if (lengths[i]) {
			t->symbols[offs[lengths[i]]++] = i;
		}
	}

	/*
	 * For the special case of only one code (which will be 0) add a
	 * code 1 which results in a symbol that is too large
	 */
	if (num_codes == 1) {
		t->counts[1] = 2;
		t->symbols[1] = t->max_sym + 1;
	}

	return JZIP_OK;
}

/* -- Decode functions -- */

static void jzip_refill(struct jzip_data *d, int num)
{
	//assert(num >= 0 && num <= 32);

	/* Read bytes until at least num bits available */
	while (d->bitcount < num) 
	{
		if (d->source != d->source_end) 
		{
			d->tag |= (unsigned int) *d->source++ << d->bitcount;
		}
		else 
		{
			d->overflow = 1;
		}
		d->bitcount += 8;
	}

	//assert(d->bitcount <= 32);
}

static unsigned int jzip_getbits_no_refill(struct jzip_data *d, int num)
{
	unsigned int bits;

	//assert(num >= 0 && num <= d->bitcount);

	/* Get bits from tag */
	bits = d->tag & ((1UL << num) - 1);

	/* Remove bits from tag */
	d->tag >>= num;
	d->bitcount -= num;

	return bits;
}

/* Get num bits from source stream */
static unsigned int jzip_getbits(struct jzip_data *d, int num)
{
	jzip_refill(d, num);
	return jzip_getbits_no_refill(d, num);
}

/* Read a num bit value from stream and add base */
static unsigned int jzip_getbits_base(struct jzip_data *d, int num, int base)
{
	return base + (num ? jzip_getbits(d, num) : 0);
}

/* Given a data stream and a tree, decode a symbol */
static int jzip_decode_symbol(struct jzip_data *d, const struct jzip_tree *t)
{
	int base = 0, offs = 0;
	int len;

	/*
	 * Get more bits while code index is above number of codes
	 *
	 * Rather than the actual code, we are computing the position of the
	 * code in the sorted order of codes, which is the index of the
	 * corresponding symbol.
	 *
	 * Conceptually, for each code length (level in the tree), there are
	 * counts[len] leaves on the left and internal nodes on the right.
	 * The index we have decoded so far is base + offs, and if that
	 * falls within the leaves we are done. Otherwise we adjust the range
	 * of offs and add one more bit to it.
	 */
	for (len = 1; ; ++len) 
	{
		offs = 2 * offs + jzip_getbits(d, 1);

		//assert(len <= 15);

		if (offs < t->counts[len]) {
			break;
		}

		base += t->counts[len];
		offs -= t->counts[len];
	}

	//assert(base + offs >= 0 && base + offs < 288);

	return t->symbols[base + offs];
}

/* Given a data stream, decode dynamic trees from it */
static int jzip_decode_trees(struct jzip_data *d, struct jzip_tree *lt,
                             struct jzip_tree *dt)
{
	unsigned char lengths[288 + 32];

	/* Special ordering of code length codes */
	static const unsigned char clcidx[19] = {
		16, 17, 18, 0,  8, 7,  9, 6, 10, 5,
		11,  4, 12, 3, 13, 2, 14, 1, 15
	};

	unsigned int hlit, hdist, hclen;
	unsigned int i, num, length;
	int res;

	/* Get 5 bits HLIT (257-286) */
	hlit = jzip_getbits_base(d, 5, 257);

	/* Get 5 bits HDIST (1-32) */
	hdist = jzip_getbits_base(d, 5, 1);

	/* Get 4 bits HCLEN (4-19) */
	hclen = jzip_getbits_base(d, 4, 4);

	/*
	 * The RFC limits the range of HLIT to 286, but lists HDIST as range
	 * 1-32, even though distance codes 30 and 31 have no meaning. While
	 * we could allow the full range of HLIT and HDIST to make it possible
	 * to decode the fixed trees with this function, we consider it an
	 * error here.
	 *
	 * 
	 */
	if (hlit > 286 || hdist > 30) {
		return JZIP_DATA_ERROR;
	}

	for (i = 0; i < 19; ++i) {
		lengths[i] = 0;
	}

	/* Read code lengths for code length alphabet */
	for (i = 0; i < hclen; ++i) {
		/* Get 3 bits code length (0-7) */
		unsigned int clen = jzip_getbits(d, 3);

		lengths[clcidx[i]] = clen;
	}

	/* Build code length tree (in literal/length tree to save space) */
	res = jzip_build_tree(lt, lengths, 19);

	if (res != JZIP_OK) {
		return res;
	}

	/* Check code length tree is not empty */
	if (lt->max_sym == -1) {
		return JZIP_DATA_ERROR;
	}

	/* Decode code lengths for the dynamic trees */
	for (num = 0; num < hlit + hdist; ) {
		int sym = jzip_decode_symbol(d, lt);

		if (sym > lt->max_sym) {
			return JZIP_DATA_ERROR;
		}

		switch (sym) {
		case 16:
			/* Copy previous code length 3-6 times (read 2 bits) */
			if (num == 0) {
				return JZIP_DATA_ERROR;
			}
			sym = lengths[num - 1];
			length = jzip_getbits_base(d, 2, 3);
			break;
		case 17:
			/* Repeat code length 0 for 3-10 times (read 3 bits) */
			sym = 0;
			length = jzip_getbits_base(d, 3, 3);
			break;
		case 18:
			/* Repeat code length 0 for 11-138 times (read 7 bits) */
			sym = 0;
			length = jzip_getbits_base(d, 7, 11);
			break;
		default:
			/* Values 0-15 represent the actual code lengths */
			length = 1;
			break;
		}

		if (length > hlit + hdist - num) {
			return JZIP_DATA_ERROR;
		}

		while (length--) {
			lengths[num++] = sym;
		}
	}

	/* Check EOB symbol is present */
	if (lengths[256] == 0) {
		return JZIP_DATA_ERROR;
	}

	/* Build dynamic trees */
	res = jzip_build_tree(lt, lengths, hlit);

	if (res != JZIP_OK) {
		return res;
	}

	res = jzip_build_tree(dt, lengths + hlit, hdist);

	if (res != JZIP_OK) {
		return res;
	}

	return JZIP_OK;
}

/* -- Block inflate functions -- */

/* Given a stream and two trees, inflate a block of data */
static int jzip_inflate_block_data(struct jzip_data *d, struct jzip_tree *lt,
                                   struct jzip_tree *dt)
{
	/* Extra bits and base tables for length codes */
	static const unsigned char length_bits[30] = {
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
		1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
		4, 4, 4, 4, 5, 5, 5, 5, 0, 127
	};

	static const unsigned short length_base[30] = {
		 3,  4,  5,   6,   7,   8,   9,  10,  11,  13,
		15, 17, 19,  23,  27,  31,  35,  43,  51,  59,
		67, 83, 99, 115, 131, 163, 195, 227, 258,   0
	};

	/* Extra bits and base tables for distance codes */
	static const unsigned char dist_bits[30] = {
		0, 0,  0,  0,  1,  1,  2,  2,  3,  3,
		4, 4,  5,  5,  6,  6,  7,  7,  8,  8,
		9, 9, 10, 10, 11, 11, 12, 12, 13, 13
	};

	static const unsigned short dist_base[30] = {
		   1,    2,    3,    4,    5,    7,    9,    13,    17,    25,
		  33,   49,   65,   97,  129,  193,  257,   385,   513,   769,
		1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
	};

	for (;;) {
		int sym = jzip_decode_symbol(d, lt);

		/* Check for overflow in bit reader */
		if (d->overflow) {
			return JZIP_DATA_ERROR;
		}

		if (sym < 256) {
			if (d->dest == d->dest_end) {
				return JZIP_BUF_ERROR;
			}
			*d->dest++ = sym;
		}
		else {
			int length, dist, offs;
			int i;

			/* Check for end of block */
			if (sym == 256) {
				return JZIP_OK;
			}

			/* Check sym is within range and distance tree is not empty */
			if (sym > lt->max_sym || sym - 257 > 28 || dt->max_sym == -1) {
				return JZIP_DATA_ERROR;
			}

			sym -= 257;

			/* Possibly get more bits from length code */
			length = jzip_getbits_base(d, length_bits[sym],
			                           length_base[sym]);

			dist = jzip_decode_symbol(d, dt);

			/* Check dist is within range */
			if (dist > dt->max_sym || dist > 29) {
				return JZIP_DATA_ERROR;
			}

			/* Possibly get more bits from distance code */
			offs = jzip_getbits_base(d, dist_bits[dist],
			                         dist_base[dist]);

			if (offs > d->dest - d->dest_start) {
				return JZIP_DATA_ERROR;
			}

			if (d->dest_end - d->dest < length) {
				return JZIP_BUF_ERROR;
			}

			/* Copy match */
			for (i = 0; i < length; ++i) {
				d->dest[i] = d->dest[i - offs];
			}

			d->dest += length;
		}
	}
}

/* Inflate an uncompressed block of data */
static int jzip_inflate_uncompressed_block(struct jzip_data *d)
{
	unsigned int length, invlength;

	if (d->source_end - d->source < 4) 
	{
		return JZIP_DATA_ERROR;
	}

	/* Get length */
	length = read_le16(d->source);

	/* Get one's complement of length */
	invlength = read_le16(d->source + 2);

	/* Check length */
	if (length != (~invlength & 0x0000FFFF)) 
	{
		return JZIP_DATA_ERROR;
	}

	d->source += 4;

	if (d->source_end - d->source < length) 
	{
		return JZIP_DATA_ERROR;
	}

	if (d->dest_end - d->dest < length) 
	{
		return JZIP_BUF_ERROR;
	}

	/* Copy block */
	while (length--) {
		*d->dest++ = *d->source++;
	}

	/* Make sure we start next block on a byte boundary */
	d->tag = 0;
	d->bitcount = 0;

	return JZIP_OK;
}

/* Inflate a block of data compressed with fixed Huffman trees */
static int jzip_inflate_fixed_block(struct jzip_data *d)
{
	/* Build fixed Huffman trees */
	jzip_build_fixed_trees(&d->ltree, &d->dtree);

	/* Decode block using fixed trees */
	return jzip_inflate_block_data(d, &d->ltree, &d->dtree);
}

/* Inflate a block of data compressed with dynamic Huffman trees */
static int jzip_inflate_dynamic_block(struct jzip_data *d)
{
	/* Decode trees from stream */
	int res = jzip_decode_trees(d, &d->ltree, &d->dtree);

	if (res != JZIP_OK) {
		return res;
	}

	/* Decode block using decoded trees */
	return jzip_inflate_block_data(d, &d->ltree, &d->dtree);
}

/* -- Public functions -- */

/* Initialize global (static) data */
void jzip_init(void)
{
	return;
}

/* Inflate stream from source to dest */
int jzip_uncompress(void *dest, unsigned int *destLen,
                    const void *source, unsigned int sourceLen)
{
	struct jzip_data d;
	int bfinal;

	/* Initialise data */
	d.source = (const unsigned char *) source;
	d.source_end = d.source + sourceLen;
	d.tag = 0;
	d.bitcount = 0;
	d.overflow = 0;

	d.dest = (unsigned char *) dest;
	d.dest_start = d.dest;
	d.dest_end = d.dest + *destLen;

	do {
		unsigned int btype;
		int res;

		/* Read final block flag */
		bfinal = jzip_getbits(&d, 1);

		/* Read block type (2 bits) */
		btype = jzip_getbits(&d, 2);

		/* Decompress block */
		switch (btype) {
		case 0:
			/* Decompress uncompressed block */
			res = jzip_inflate_uncompressed_block(&d);
			break;
		case 1:
			/* Decompress block with fixed Huffman trees */
			res = jzip_inflate_fixed_block(&d);
			break;
		case 2:
			/* Decompress block with dynamic Huffman trees */
			res = jzip_inflate_dynamic_block(&d);
			break;
		default:
			res = JZIP_DATA_ERROR;
			break;
		}

		if (res != JZIP_OK) {
			return res;
		}
	} while (!bfinal);

	/* Check for overflow in bit reader */
	if (d.overflow) {
		return JZIP_DATA_ERROR;
	}

	*destLen = d.dest - d.dest_start;

	return JZIP_OK;
}



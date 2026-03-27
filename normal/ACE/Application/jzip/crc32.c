/******************************************************************************/
/*! @addtogroup Group2
    @file       crc32.c
    @brief      
    @date       2025/12/25
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/

#include "jzip.h"

static const unsigned int jzip_crc32tab[16] = {
    0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190,
	0x6B6B51F4, 0x4DB26158, 0x5005713C, 0xEDB88320, 0xF00F9344,
	0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278,
	0xBDBDF21C
};

unsigned int jzip_crc32(const void* data, unsigned int length)
{
    const unsigned char *buf = (const unsigned char *) data;
	unsigned int crc = 0xFFFFFFFF;
	unsigned int i;

	if (length == 0) {
		return 0;
	}

	for (i = 0; i < length; ++i) {
		crc ^= buf[i];
		crc = jzip_crc32tab[crc & 0x0F] ^ (crc >> 4);
		crc = jzip_crc32tab[crc & 0x0F] ^ (crc >> 4);
	}

	return crc ^ 0xFFFFFFFF;
}

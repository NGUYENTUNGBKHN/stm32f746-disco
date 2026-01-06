/******************************************************************************/
/*! @addtogroup Group2
    @file       jzip.h
    @brief      
    @date       2025/12/25
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
#ifndef _JZIP_H_
#define _JZIP_H_
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Status codes returned.
 *
 * @see tinf_uncompress, tinf_gzip_uncompress, tinf_zlib_uncompress
 */
typedef enum {
	JZIP_OK         = 0,  /**< Success */
	JZIP_DATA_ERROR = -3, /**< Input error */
	JZIP_BUF_ERROR  = -5  /**< Not enough room for output */
} jzip_error_code;

/**
 * Initialize global data used by tinf.
 *
 * @deprecated No longer required, may be removed in a future version.
 */
void jzip_init(void);


int jzip_uncompress(void *dest, unsigned int *destLen,
                           const void *source, unsigned int sourceLen);


unsigned int jzip_crc32(const void *data, unsigned int length);


#ifdef __cplusplus
}
#endif
#endif



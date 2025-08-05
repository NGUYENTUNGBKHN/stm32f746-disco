/******************************************************************************/
/*! @addtogroup Group2
    @file       common.h
    @brief      
    @date       2025/08/05
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* CODE */
#include "stm32f746xx.h"

#define DEBUG
#if defined(DEBUG)
/* TRACE info : normal log */
#define TRACE_INFO(format, ... )                                                        \
    printf("%s (%d) : " format, __FUNCTION__, __LINE__, ##__VA_ARGS__)                  \
/* TRACE info : normal log */
#define ERROR(code)                                                                     \
    printf("%s %s (%d) : error code = %d \n", __FILE__, __FUNCTION__, __LINE__, code)   \

#else
#define TRACE_INFO(__format__, ...)
#define ERROR(__format__, ...)
#endif //  DEBUG

#ifdef __cplusplus
}
#endif
#endif
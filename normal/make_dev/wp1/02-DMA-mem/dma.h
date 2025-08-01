/******************************************************************************/
/*! @addtogroup Group2
    @file       dma.h
    @brief      
    @date       2025/07/31
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
#ifndef _DMA_H_
#define _DMA_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* CODE */
#include "stm32f746xx.h"

void dma_mem_to_mem_init();
void dma_mem_transfer(uint32_t src, uint32_t dest, uint16_t size);

#ifdef __cplusplus
}
#endif
#endif


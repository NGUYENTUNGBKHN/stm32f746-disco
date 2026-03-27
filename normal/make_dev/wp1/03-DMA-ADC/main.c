/******************************************************************************/
/*! @addtogroup Group2
    @file       main.c
    @brief      
    @date       2025/07/30
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include "stm32f746xx.h"
#include "dma.h"
#include "dma_adc.h"

extern uint16_t adc_raw_data[NUM_OF_CHNANELS];

int main()
{
    printf("DMA ADC test\n");
    while (1)
    {
        /* code */

    }
}


void DMA2_Stream0_IRQHandler()
{
    /* Check if transfer complete interrupt occured */
	if((DMA2->LISR) & DMA_LISR_TCIF0_Msk)
	{
		// g_transfer_cmplt = 1;
		/* Clear flag*/
		DMA2->LIFCR |= DMA_LIFCR_CTCIF0_Msk;
	}
	/* Check if transfer complete interrupt occured */
	if((DMA2->LISR) & DMA_LISR_TEIF0_Msk)
	{
//		g_transfer_cmplt = 0;
			/* Clear flag*/
		DMA2->LIFCR |= DMA_LIFCR_CTEIF0_Msk;
	}
}


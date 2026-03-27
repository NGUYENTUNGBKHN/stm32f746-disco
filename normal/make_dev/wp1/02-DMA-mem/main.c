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

#define BUFFER_SIZE 5
uint16_t sensor_data_arr[BUFFER_SIZE] = {892, 731, 1234, 90, 23};
uint16_t temp_data_arr[BUFFER_SIZE];

volatile uint8_t g_transfer_cmplt;

int main()
{
    printf("DMA memmory to memory\n");
    dma_mem_to_mem_init();
    dma_mem_transfer((uint32_t)sensor_data_arr, (uint32_t)temp_data_arr, BUFFER_SIZE);
    while(!g_transfer_cmplt){}
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		printf("temp buffer[%d] = %d\n", i, temp_data_arr[i]);
	}
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
		g_transfer_cmplt = 1;
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


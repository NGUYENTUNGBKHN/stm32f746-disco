/******************************************************************************/
/*! @addtogroup Group2
    @file       dma_adc.c
    @brief      
    @date       2025/08/04
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "dma_adc.h"
/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      COMMON VARIABLE DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
uint16_t adc_raw_data[NUM_OF_CHNANELS];

/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/


/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/


void dma_adc_init()
{
    /* 1. ADC initialize */
    /* 1.1 ADC clock */


    /* 2. DMA init */
    /* 2.1 DMA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    /* 2.2 If stream enable, disable it */
    do
    {
        /* Disable stream 0 */
        DMA2_Stream0->CR = 0;
    }
    while (DMA2_Stream0->CR & DMA_SxCR_EN);
    /* 2.3 Configure DMA*/
    /* 2.3.1 Transfer mode */
    /* Mode Peripheral to memory 00 */
    DMA2_Stream0->CR &= ~(0x1 << (DMA_SxCR_DIR_Pos + 1));
    DMA2_Stream0->CR &= ~(0x1 << DMA_SxCR_DIR_Pos);
    /* 2.3.2 Pointer incrementation */
    /* MINC - Memory increment mode */
    DMA2_Stream0->CR |= (1 << DMA_SxCR_MINC_Pos);
    /* MSIZE */
    DMA2_Stream0->CR &= ~(0x1 << (DMA_SxCR_MSIZE_Pos + 1));
    DMA2_Stream0->CR |= (0x1 << DMA_SxCR_MSIZE_Pos);
    /* 2.3.3 Circular mode */
    DMA2_Stream0->CR |= (0x1 << DMA_SxCR_CIRC_Pos);
    /* 2.3.4 Data width */
    DMA2_Stream0->PAR = (uint32_t)(&(ADC1->DR));
    DMA2_Stream0->M0AR = (uint32_t)(&adc_raw_data);
    DMA2_Stream0->NDTR = NUM_OF_CHNANELS;
    /* 2.3.5 Channel selection - Channel 0 */
    DMA2_Stream0->CR &= ~(0x01 << DMA_SxCR_CHSEL_Pos);
    DMA2_Stream0->CR &= ~(0x01 << DMA_SxCR_CHSEL_Pos + 1);
    DMA2_Stream0->CR &= ~(0x01 << DMA_SxCR_CHSEL_Pos + 2);
    /* 2.3.6 FIFO setting */


    /* 2.3.7 DMA enable */
    DMA2_Stream0->CR |= DMA_SxCR_EN;
}

/******************************** End of file *********************************/



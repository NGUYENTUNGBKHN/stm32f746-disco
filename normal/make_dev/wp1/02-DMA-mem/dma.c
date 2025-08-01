/******************************************************************************/
/*! @addtogroup Group2
    @file       dma.c
    @brief      
    @date       2025/07/31
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/

/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "dma.h"
/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      COMMON VARIABLE DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/


/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

void dma_mem_to_mem_init()
{
    /* 1. Enable DMA clock */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    /* 2. If stream enable, disable it*/
    do
    {
        /* Disable stream 0 */
        DMA2_Stream0->CR = 0;
    }
    while (DMA2_Stream0->CR & DMA_SxCR_EN);

    /* 3. Configure DMA */

    /* 3.1 Tranfer mode : Memory to Memory */
    DMA2_Stream0->CR |= (0x1 << (DMA_SxCR_DIR_Pos + 1));
    DMA2_Stream0->CR &= ~(0x1 << DMA_SxCR_DIR_Pos);
    /* 3.2 Pointer incrementation */
    /* MINC - Memory increment mode */
    DMA2_Stream0->CR |= (1 << DMA_SxCR_MINC_Pos);
    /* PINC - Peripheral increment mode */
    DMA2_Stream0->CR |= (1 << DMA_SxCR_PINC_Pos);
    /* Msize */
    DMA2_Stream0->CR &= ~(0x1 << (DMA_SxCR_MSIZE_Pos + 1));
    DMA2_Stream0->CR |= (0x1 << DMA_SxCR_MSIZE_Pos);
    /* Psize */
    DMA2_Stream0->CR &= ~(0x1 << (DMA_SxCR_PSIZE_Pos + 1));
    DMA2_Stream0->CR |= (0x1 << DMA_SxCR_PSIZE_Pos);
    /* 4. */
    /* 5. */
    /* 6. FIFO */
    /* 6.1 Direct mode disable */
    DMA2_Stream0->FCR |= (1 << DMA_SxFCR_DMDIS_Pos);
    /* 6.2 Setup Fifo threshold -> Full Fifo */
    DMA2_Stream0->FCR |= (1 << DMA_SxFCR_FTH_Pos);
    DMA2_Stream0->FCR |= (1 << (DMA_SxFCR_FTH_Pos + 1));

    /* 7. Interrupt */
    DMA2_Stream0->CR |= (1 << DMA_SxCR_TCIE_Pos);
    DMA2_Stream0->CR |= (1 << DMA_SxCR_TEIE_Pos);

    /* Setup event DMA function */
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

void dma_mem_transfer(uint32_t src, uint32_t dest, uint16_t size)
{
    DMA2_Stream0->M0AR = dest;

    DMA2_Stream0->PAR = src;

    DMA2_Stream0->NDTR = size;

    DMA2_Stream0->CR |= DMA_SxCR_EN;
}

/******************************** End of file *********************************/


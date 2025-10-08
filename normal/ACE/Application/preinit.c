/******************************************************************************/
/*! @addtogroup Group2
    @file       preinit.c
    @brief      
    @date       2025/10/08
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/

/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "stm32f7xx.h"
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


void SetSysClk()
{
    register uint32_t tmpreg, timeout = 0xFFFF;

    uint32_t PLL_M = 25,PLL_Q = 9, PLL_R = 7, PLL_N = 432, PLL_P = 2;

    /* Enable Power Control clock */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    /* Config Voltage Scale 1 */
    PWR->CR1 |= PWR_CR1_VOS;

    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        tmpreg = RCC->CR & RCC_CR_HSERDY;
    } while ((tmpreg != RCC_CR_HSERDY) && (timeout-- > 0));

    if (timeout != 0)
    {
        /* Select regulator voltage output Scale 1 mode */
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;

        PWR->CR1 |= PWR_CR1_VOS;

        /* Enable Over Drive to reach the 216MHz frequency */
        /* Enable ODEN */
        PWR->CR1 |= 0x00010000;
        timeout = 0xFFFF;
        /* Wait till ODR is ready and if Time out is reached exit */
        do
        {
            tmpreg = PWR->CSR1 & PWR_CSR1_ODRDY;
        } while ((tmpreg != PWR_CSR1_ODRDY) && (timeout-- > 0));

        /* Enable ODSW */
        PWR->CR1 |= 0x00020000;
        timeout = 0xFFFF;
        /* Wait till ODR is ready and if Time out is reached exit */
        do
        {
            tmpreg = PWR->CSR1 & PWR_CSR1_ODSWRDY;
        } while ((tmpreg != PWR_CSR1_ODSWRDY) && (timeout-- > 0));

        /* HCLK = SYSCLK / 1*/
        RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK / 2*/
        RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

        /* PCLK1 = HCLK / 4*/
        RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

        /* Configure the main PLL */
        RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) |
                       (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24) | (PLL_R << 28);

        /* Enable the main PLL */
        RCC->CR |= RCC_CR_PLLON;
    }
    /* Wait that PLL is ready */
    timeout = 0xFFFF;
    do
    {
        tmpreg = (RCC->CR & RCC_CR_PLLRDY);
    } while ((tmpreg != RCC_CR_PLLRDY) && (timeout-- > 0));

    if (timeout != 0)
    {
        /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
        FLASH->ACR = FLASH_ACR_LATENCY_7WS;

        /* Select the main PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= RCC_CFGR_SW_PLL;

        timeout = 0xFFFF;
        do
        {
            tmpreg = (RCC->CFGR & RCC_CFGR_SWS);
        } while ((tmpreg != RCC_CFGR_SWS) && (timeout-- > 0));
    }
}

void SystemInit_ExtMemCtl()
{
    
}


void preInit()
{
    SetSysClk();
}



/******************************** End of file *********************************/




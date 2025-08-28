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
#include "boot_main.h"

uint16_t test[30];

static void jump_to_application();
static void jump_to_bootloader();

int boot_main()
{
    TRACE_INFO("Bootloader \n");
    if (0)
    {
        TRACE_INFO("jumping to application \n");
        jump_to_application();
    }
    else
    {
        TRACE_INFO("jumping to bootloader \n");
        jump_to_bootloader();
    }
    while (1)
    {
        /* code */

    }
}

static void jump_to_application()
{
    void (*app_reset_handler)(void);

    /* app MSP address*/
    uint32_t app_msp_addr = *(volatile uint32_t*)(APP_START_ADDRESS);

    /* Reset handler address */
    uint32_t app_reset_handler_addr = *(volatile uint32_t*)(APP_START_ADDRESS + 4);

    /* Reset all register */
    __set_CONTROL(0x00000000);   // Set CONTROL to its reset value 0.
    __set_PRIMASK(0x00000000);   // Set PRIMASK to its reset value 0.
    __set_BASEPRI(0x00000000);   // Set BASEPRI to its reset value 0.
    __set_FAULTMASK(0x00000000); // Set FAULTMASK to its reset value 0.

    app_reset_handler = (void*)app_reset_handler_addr;    
     /* Set MSP */
    __set_MSP(app_msp_addr);
    /* Change vtor table */
    SCB->VTOR = APP_START_ADDRESS;
    /* Call reset handler */
    app_reset_handler();
}

static void jump_to_bootloader()
{
    bootloader();
}


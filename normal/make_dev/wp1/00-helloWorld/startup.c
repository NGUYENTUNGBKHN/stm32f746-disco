/******************************************************************************/
/*! @addtogroup Group2
    @file       startup.c
    @brief      
    @date       2025/07/30
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
#include <stdint.h>


extern uint32_t _estack;
extern int main();


extern uint32_t _sidata;

extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;


void Reset_Handler()
{
    volatile uint32_t *start = &_sdata;
    volatile uint32_t *end = &_edata;
    volatile uint32_t *data = &_sidata;
    volatile uint32_t *bss_start = &_sbss;
    volatile uint32_t *bss_end = &_ebss;

    while (start < end)
    {
        /* code */
        *start++ = *data++;
    }
   
    /* setup bss data = 0 */
    while (bss_start < bss_end)
    {
        /* code */
        *bss_start++ = 0;
    }


    /* Jump to __libc_init_array */
    // __libc_init_array();
    
    main();
}


void default_handler(void)
{
    while (1)
    {
        /* code */
    }
    
}

void NMI_Handler(void) __attribute((weak, alias("default_handler")));
void HardFault_Handler(void) __attribute((weak, alias("default_handler")));
void MemManage_Handler(void) __attribute((weak, alias("default_handler")));
void BusFault_Handler(void) __attribute((weak, alias("default_handler")));
void UsageFault_Handler(void) __attribute((weak, alias("default_handler")));
void SVC_Handler(void) __attribute((weak, alias("default_handler")));

__attribute((section(".isr_vector")))
uint32_t *_isr_vector[] = {
    (uint32_t*) &_estack,               /* top stack pointer */
    (uint32_t*) Reset_Handler,          /* Reset handler  */
    (uint32_t*) NMI_Handler,
    (uint32_t*) HardFault_Handler,
    (uint32_t*) MemManage_Handler,
    (uint32_t*) BusFault_Handler,
    (uint32_t*) UsageFault_Handler,
    0,
    0,
    0,
    0,
    (uint32_t*) SVC_Handler,
};



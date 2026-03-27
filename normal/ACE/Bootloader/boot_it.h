/******************************************************************************/
/*! @addtogroup Group2
    @file       boot_it.h
    @brief      
    @date       2025/10/02
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
#ifndef _BOOT_IT_H_
#define _BOOT_IT_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* CODE */
void NMI_IRQHandler(void);
void HardFault_IRQHandler(void);
void MemManage_IRQHandler(void);
void BusFault_IRQHandler(void);
void UsageFault_IRQHandler(void);
void SVC_IRQHandler(void);
void DebugMonitor_IRQHandler(void);
void PendSV_IRQHandler(void);
void SysTick_IRQHandler(void);

#ifdef __cplusplus
}
#endif
#endif

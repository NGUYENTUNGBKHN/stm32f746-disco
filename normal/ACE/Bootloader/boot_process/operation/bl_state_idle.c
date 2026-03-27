/******************************************************************************/
/*! @addtogroup Group2
    @file       bl_state_idle.c
    @brief      
    @date       2025/08/06
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/

/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "bootloader.h"
#include "common.h"
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

static void bl_state_idle_evt_start_cmd()
{
    TRACE_INFO("Ok test\n");
    bl_fsm_changeState(main_bl_fsm, bl_state_start_transition);
}


bl_fsm_p bl_state_idle_transition(bl_fsm_p state)
{
    static uint8_t first = 1;
    bl_fsm_func_t static_func;

    if (first)
    {
        first = 0;
        static_func.evt_func_start_cmd = bl_state_idle_evt_start_cmd;
    }
    state->func = static_func;

    return state;
}

/******************************** End of file *********************************/


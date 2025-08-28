/******************************************************************************/
/*! @addtogroup Group2
    @file       bl_state_data.c
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

static void bl_state_data_evt_data_cmd()
{
    TRACE_INFO("Ok test\n");
}

static void bl_state_data_evt_end_cmd()
{
    TRACE_INFO("Ok test\n");
}

static void bl_state_data_evt_error()
{
    TRACE_INFO("Ok test\n");
}

bl_fsm_p bl_state_data_transition(bl_fsm_p state)
{
    static uint8_t first = 1;
    bl_fsm_func_t static_func;

    if (first)
    {
        first = 0;
        static_func.evt_func_data_cmd = bl_state_data_evt_data_cmd;
        static_func.evt_func_end_cmd = bl_state_data_evt_end_cmd;
        static_func.evt_func_error = bl_state_data_evt_error;
    }
    state->func = static_func;

    return state;
}

/******************************** End of file *********************************/

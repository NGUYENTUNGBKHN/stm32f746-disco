/******************************************************************************/
/*! @addtogroup Group2
    @file       bootloader.c
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
#include <stdint.h>
#include <stddef.h>
#include "common.h"
#include "bootloader.h"
/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      COMMON VARIABLE DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
static uint32_t state = BL_STATE_IDLE;
bl_fsm_p main_bl_fsm;
/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/
static void bootloader_dwl();

/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

/**
 * 
 */
void bootloader()
{
    uint32_t cmd;
    /* Enter bootlodaer */
    TRACE_INFO("Enter bootloader \n");
    /* Init essential driver*/

    /* Communication device*/

    /* Check CRC */

    /* Mode download enabled */
    bl_fsm_init();
    cmd = BL_CMD_REQ_START; 
    while (1)
    {
        /* code */
        // bootloader_dwl(cmd);
    }
    
}

static void bootloader_dwl(uint32_t cmd)
{
    switch (cmd)
    {
    case BL_CMD_REQ_START:
        if (main_bl_fsm->func.evt_func_start_cmd != NULL)
        {   
            main_bl_fsm->func.evt_func_start_cmd(main_bl_fsm, 0);
        }
        break;
    case BL_CMD_REQ_DATA:
        if (main_bl_fsm->func.evt_func_data_cmd != NULL)
        {   
            main_bl_fsm->func.evt_func_data_cmd(main_bl_fsm, 0);
        }
        break;
    case BL_CMD_REQ_END:
        if (main_bl_fsm->func.evt_func_end_cmd != NULL)
        {   
            main_bl_fsm->func.evt_func_end_cmd(main_bl_fsm, 0);
        }
        break;
    case BL_CMD_REQ_RESET:
        if (main_bl_fsm->func.evt_func_reset_cmd != NULL)
        {   
            main_bl_fsm->func.evt_func_start_cmd(main_bl_fsm, 0);
        }
        break;
    default:
        break;
    }
}

void bl_fsm_init()
{
    bl_fsm_changeState(main_bl_fsm, bl_state_idle_transition);
}

void bl_fsm_changeState(bl_fsm_p state, transition_func_t func)
{
    state = func(state);
}

/******************************** End of file *********************************/



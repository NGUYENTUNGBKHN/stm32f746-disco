/******************************************************************************/
/*! @addtogroup Group2
    @file       bootloader.h
    @brief      
    @date       2025/08/06
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/

#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* CODE */
enum bl_state
{   
    BL_STATE_IDLE = 0,
    BL_STATE_DWL_START,
    BL_STATE_DWL_DATA,
    BL_STATE_DWL_END,
};

enum bl_cmd_req
{
    BL_CMD_REQ_START,
    BL_CMD_REQ_DATA,
    BL_CMD_REQ_END,
    BL_CMD_REQ_RESET,
};

typedef struct bl_fsm_s bl_fsm_t;

typedef void (*evt_func_t)(bl_fsm_t *state, void* data);

typedef struct 
{
    evt_func_t evt_func_start_cmd;
    evt_func_t evt_func_data_cmd;
    evt_func_t evt_func_end_cmd;
    evt_func_t evt_func_reset_cmd;
    evt_func_t evt_func_error;
}bl_fsm_func_t;

typedef struct bl_fsm_s
{
    uint8_t bl_state;
    bl_fsm_func_t func;
}bl_fsm_t, *bl_fsm_p;

typedef bl_fsm_p (*transition_func_t)(bl_fsm_p state);

void bl_fsm_init();

void bl_fsm_changeState(bl_fsm_p state, transition_func_t func);
extern bl_fsm_p bl_state_idle_transition(bl_fsm_p state);
extern bl_fsm_p bl_state_start_transition(bl_fsm_p state);
extern bl_fsm_p bl_state_data_transition(bl_fsm_p state);
extern bl_fsm_p bl_state_end_transition(bl_fsm_p state);

extern bl_fsm_p main_bl_fsm;

extern void bootloader();

#ifdef __cplusplus
}
#endif
#endif


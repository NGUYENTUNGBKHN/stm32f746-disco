/******************************************************************************/
/*! @addtogroup Group2
    @file       memory_map.h
    @brief      
    @date       2025/08/05
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
#ifndef _MEMORY_MAP_H_
#define _MEMORY_MAP_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* CODE */
#define FUNC(x)     (*((void (**)(void)) x ))

/* STM32 Memory Map */
#define ST_MEMORY_START_ADDRESS     0x00000000
#define ST_MEMORY_END_ADDRESS       0xFFFFFFFF

#define ST_QSPI_MAPPING_ADDRESS     0x90000000
#define ST_QSPI_MAPPING_SIZE        0x04000000      /* 16MB */

#define ST_QSPI_CTRL_RES_ADDRESS    0xA0000000
#define ST_QSPI_CTRL_RES_SIZE       0x00002000      /* 8KB */
/*   */


#define APP_START_ADDRESS   0x08040000
#define APP_SIZE            0x3FFFF      

#define DWL_START_ADDRESS   0x08008000

#define entry_dwl       FUNC(DWL_START_ADDRESS)
#define entry_dwl2      FUNC((DWL_START_ADDRESS + 4))

#ifdef __cplusplus
}
#endif
#endif

/**
 * @file       qspi_flash.c
 * @brief      
 * @date       2025/10/28
 * @author     [Gentantun] (nguyenthanhtung8196@gmail.com)
 * @details    
 * @ref        
 * @copyright  Copyright (c) 2025 RoboTun
*/
/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "qspi_flash.h"
#include "w25q128j.h"
/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/

static drv_qspi_flash_t *m_qspi_flash;
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


QSPI_RESULT qspi_flash_init()
{
    m_qspi_flash = w25q128j_Create();

    if (NULL == m_qspi_flash)
    {
        return QSPI_RESULT_FAIL;
    }

    if (m_qspi_flash->init(m_qspi_flash) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    if (m_qspi_flash->reset(m_qspi_flash) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    if (m_qspi_flash->status(m_qspi_flash) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }
    return QSPI_RESULT_OK;
}

QSPI_RESULT qspi_flash_read(uint32_t address, uint8_t *data, uint16_t size)
{
    if (m_qspi_flash->read(m_qspi_flash, address, data, size) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }
    return QSPI_RESULT_OK;
}

QSPI_RESULT qspi_flash_write(uint32_t address, uint8_t *data, uint16_t size)
{
    if (m_qspi_flash->erase_block(m_qspi_flash, address) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    if (m_qspi_flash->write(m_qspi_flash, address, data, size) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }
    return QSPI_RESULT_OK;
}

QSPI_RESULT qspi_flash_xip_en()
{
    if (m_qspi_flash->memoryMapped_active(m_qspi_flash) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }
    return QSPI_RESULT_OK;
}

/******************************** End of file *********************************/


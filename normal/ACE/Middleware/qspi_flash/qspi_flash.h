/**
 * @file       qspi_flash.h
 * @brief      
 * @date       2025/10/28
 * @author     [Gentantun] (nguyenthanhtung8196@gmail.com)
 * @details    
 * @ref        
 * @copyright  Copyright (c) 2025 RoboTun
*/
#ifndef _QSPI_FLASH_H_
#define _QSPI_FLASH_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* CODE */
#include "stm32f746xx.h"
#include "stm32f7xx_hal_conf.h"

typedef enum
{
    QSPI_RESULT_OK = 0x00,
    QSPI_RESULT_FAIL,
    QSPI_RESULT_TIMEOUT
}QSPI_RESULT;

typedef enum
{
    DRV_QSPI_STATE_NONE,
    DRV_QSPI_STATE_IDLE,
    DRV_QSPI_STATE_INIT,
    DRV_QSPI_STATE_READ,
    DRV_QSPI_STATE_WRITE,
    DRV_QSPI_STATE_MEMO,
    DRV_QSPI_STATE_ERASE
}drv_qspi_state;

typedef struct DRV_QSPI_FLASH_S drv_qspi_flash_t;
struct DRV_QSPI_FLASH_S
{
    uint8_t state;
    QSPI_RESULT (*init)(drv_qspi_flash_t *self);
    QSPI_RESULT (*get_status)(drv_qspi_flash_t *self);
    QSPI_RESULT (*reset)(drv_qspi_flash_t *self);
    QSPI_RESULT (*erase_chip)(drv_qspi_flash_t *self);
    QSPI_RESULT (*erase_block)(drv_qspi_flash_t *self, uint32_t address);
    QSPI_RESULT (*write)(drv_qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size);
    QSPI_RESULT (*read)(drv_qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size);
    QSPI_RESULT (*memoryMapped_active)(drv_qspi_flash_t *self);
    QSPI_RESULT (*callback)(drv_qspi_flash_t *self, uint32_t event, void* data);
};

extern QSPI_RESULT qspi_flash_init();
extern QSPI_RESULT qspi_flash_read(uint32_t address, uint8_t *data, uint16_t size);
extern QSPI_RESULT qspi_flash_write(uint32_t address, uint8_t *data, uint16_t size);
extern QSPI_RESULT qspi_flash_xip_en();

#ifdef __cplusplus
}
#endif
#endif

/**
 * @file       qspi_flash.h
 * @brief      
 * @date       2025/10/19
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
#include "stm32f7xx.h"
#include "stm32f7xx_hal_conf.h"
#include "reg_util.h"
#include "common.h"

#define QUADSPI_CLK_port        GPIOB
#define QUADSPI_CLK_pin         GPIO_PIN_2
#define QUADSPI_BK1_NCS_port    GPIOB
#define QUADSPI_BK1_NCS_pin     GPIO_PIN_6
#define QUADSPI_BK1_IO1_port    GPIOD
#define QUADSPI_BK1_IO1_pin     GPIO_PIN_12
#define QUADSPI_BK1_IO2_port    GPIOE
#define QUADSPI_BK1_IO2_pin     GPIO_PIN_2
#define QUADSPI_BK1_IO3_port    GPIOD
#define QUADSPI_BK1_IO3_pin     GPIO_PIN_13         


typedef struct QSPI_FLASH_S qspi_flash_t;

struct QSPI_FLASH_S
{
    /* data */
    QSPI_HandleTypeDef *handle;
    void (*init)(qspi_flash_t *self);
    void (*read)(qspi_flash_t *self, uint32_t address, uint8_t data, uint16_t size);
    void (*write)(qspi_flash_t *self, uint32_t address, uint8_t data, uint16_t size);
    void (*erase_chip)(qspi_flash_t *self);
    void (*memory_mapped)(qspi_flash_t *self);
};

qspi_flash_t *qspi_flash_create();

#ifdef __cplusplus
}
#endif
#endif

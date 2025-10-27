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

#define MCU_GPIO_PIN_0					0U
#define MCU_GPIO_PIN_1					1U
#define MCU_GPIO_PIN_2					2U
#define MCU_GPIO_PIN_3					3U
#define MCU_GPIO_PIN_4					4U
#define MCU_GPIO_PIN_5					5U
#define MCU_GPIO_PIN_6					6U
#define MCU_GPIO_PIN_7					7U
#define MCU_GPIO_PIN_8					8U
#define MCU_GPIO_PIN_9					9U
#define MCU_GPIO_PIN_10					10U
#define MCU_GPIO_PIN_11					11U
#define MCU_GPIO_PIN_12					12U
#define MCU_GPIO_PIN_13					13U
#define MCU_GPIO_PIN_14					14U
#define MCU_GPIO_PIN_15					15U

#define QUADSPI_CLK_port        GPIOB
#define QUADSPI_CLK_pin         MCU_GPIO_PIN_2
#define QUADSPI_BK1_NCS_port    GPIOB
#define QUADSPI_BK1_NCS_pin     MCU_GPIO_PIN_6
#define QUADSPI_BK1_IO0_port    GPIOD
#define QUADSPI_BK1_IO0_pin     MCU_GPIO_PIN_11
#define QUADSPI_BK1_IO1_port    GPIOD
#define QUADSPI_BK1_IO1_pin     MCU_GPIO_PIN_12
#define QUADSPI_BK1_IO2_port    GPIOE
#define QUADSPI_BK1_IO2_pin     MCU_GPIO_PIN_2
#define QUADSPI_BK1_IO3_port    GPIOD
#define QUADSPI_BK1_IO3_pin     MCU_GPIO_PIN_13        

#define QUADSPI_FMODE_INDIRECT_WRITE     0
#define QUADSPI_FMODE_INDIRECT_READ      1
#define QUADSPI_FMODE_AUTO_POLL          2
#define QUADSPI_FMODE_MEMO               3

/* QUADSPI Configure Instruction mode */
#define QUADSPI_INS_MODE_NONE           0
#define QUADSPI_INS_MODE_SING_LINE      1
#define QUADSPI_INS_MODE_TWO_LINE       2
#define QUADSPI_INS_MODE_FOUR_LINE      3

/* QUADSPI Configure Address mode */
#define QUADSPI_ADR_MODE_NONE           0
#define QUADSPI_ADR_MODE_SING_LINE      1
#define QUADSPI_ADR_MODE_TWO_LINE       2
#define QUADSPI_ADR_MODE_FOUR_LINE      3

/* QUADSPI Configure Address size */
#define QUADSPI_ADR_SIZE_8_BIT          0
#define QUADSPI_ADR_SIZE_16_BIT         1
#define QUADSPI_ADR_SIZE_24_BIT         2
#define QUADSPI_ADR_SIZE_32_BIT         3

/* QUADSPI Configure Alternate Byte mode */
#define QUADSPI_ALB_MODE_NONE           0
#define QUADSPI_ALB_MODE_SING_LINE      1
#define QUADSPI_ALB_MODE_TWO_LINE       2
#define QUADSPI_ALB_MODE_FOUR_LINE      3

/* QUADSPI Configure Alternate Byte size */
#define QUADSPI_ALB_SIZE_8_BIT          0
#define QUADSPI_ALB_SIZE_16_BIT         1
#define QUADSPI_ALB_SIZE_24_BIT         2
#define QUADSPI_ALB_SIZE_32_BIT         3

/* QUADSPI Configure Data mode */
#define QUADSPI_DATA_MODE_NONE           0
#define QUADSPI_DATA_MODE_SING_LINE      1
#define QUADSPI_DATA_MODE_TWO_LINE       2
#define QUADSPI_DATA_MODE_FOUR_LINE      3

/*  QUADSPI Configure Double data rate mode */
#define QUADSPI_DDRM_ENABLE             1
#define QUADSPI_DDRM_DISABLE            0

/* QUADSPI Configure Double data rate mode */
#define QUADSPI_DHHC_ANALOG_DELAY         0
#define QUADSPI_DHHC_1_4_DELAY            1

/* QUADSPI Configure Send instruction only once mode */
#define QUADSPI_SIOO_EVERY_TRANS         0
#define QUADSPI_SIOO_FIRST_TRANS         1

typedef struct QSPI_FLASH_MEMORY_MAPPED_S
{
    uint32_t TimeoutPeriod;
    uint32_t TimeOutActivations;
}qspi_flash_memory_mapped_t;

typedef struct QSPI_FLASH_CMD_S
{
    uint32_t Instruction;
    uint32_t Address;
    uint32_t AlternateBytes;
    uint32_t AddressSize;
    uint32_t AlternateBytesSize;
    uint32_t DummyCycles;
    uint32_t InstructionModes;
    uint32_t AddressMode;
    uint32_t AlternateByteMode;
    uint32_t DataMode;
    uint32_t NbData;
    uint32_t DdrMode;
    uint32_t DdrHoldHalfCycle;
    uint32_t SIOOMode;
}qspi_flash_cmd_t;

typedef struct QSPI_FLASH_AUTO_POLLING_S
{
    uint32_t Match;
    uint32_t Mask;
    uint32_t Interval;
    uint32_t StatusByteSize;
    uint32_t MatchMode;
    uint32_t AutomaticStop;
}qspi_flash_autoPolling_t;

typedef struct QSPI_FLASH_S qspi_flash_t;

struct QSPI_FLASH_S
{
    /* data */
    QUADSPI_TypeDef *pQspi;
    void (*init)(qspi_flash_t *self);
    void (*read)(qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size);
    void (*write)(qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size);
    void (*erase_chip)(qspi_flash_t *self, uint32_t BlockAddress);
    void (*memory_mapped)(qspi_flash_t *self);
};

qspi_flash_t *qspi_flash_create();

#ifdef __cplusplus
}
#endif
#endif

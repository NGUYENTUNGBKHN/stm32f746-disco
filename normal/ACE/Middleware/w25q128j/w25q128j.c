/******************************************************************************/
/*! @addtogroup Group2
    @file       w25q128j.c
    @brief      
    @date       2025/10/07
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "w25q128j.h"
#include <stdlib.h>
/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/
w25q128j_t *w25_qspi;

/*******************************************************************************
**                      COMMON VARIABLE DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
static uint8_t W25Q128J_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);
static uint8_t W25Q128J_WriteEnable(QSPI_HandleTypeDef *hqspi);
static void W25Q128J_Reset(QSPI_HandleTypeDef *hqspi);
/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/


/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

void W25Q128J_MspInit(QSPI_HandleTypeDef *hqspi, void *Params)
{
    GPIO_InitTypeDef gpio_init_structure;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable the QuadSPI memory interface clock */
    W25Q128J_CLK_ENABLE();
    /* Reset the QuadSPI memory interface */
    __HAL_RCC_QSPI_FORCE_RESET();
    __HAL_RCC_QSPI_RELEASE_RESET();
    /* Enable GPIO clocks */
    W25Q128J_GPIO_CLK_ENABLE;

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* QSPI CS GPIO pin configuration  */
    gpio_init_structure.Pin       = W25Q128J_GPIO_CS_PIN;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull      = GPIO_PULLUP;
    gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
    gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(W25Q128J_GPIO_CS_PORT, &gpio_init_structure);

    /* QSPI CLK GPIO pin configuration  */
    gpio_init_structure.Pin       = W25Q128J_GPIO_CLK_PIN;
    gpio_init_structure.Pull      = GPIO_NOPULL;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(W25Q128J_GPIO_CLK_PORT, &gpio_init_structure);

    /* QSPI D0 GPIO pin configuration  */
    gpio_init_structure.Pin       = W25Q128J_GPIO_D0_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(W25Q128J_GPIO_D0_PORT, &gpio_init_structure);

    /* QSPI D1 GPIO pin configuration  */
    gpio_init_structure.Pin       = W25Q128J_GPIO_D1_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(W25Q128J_GPIO_D1_PORT, &gpio_init_structure);

    /* QSPI D2 GPIO pin configuration  */
    gpio_init_structure.Pin       = W25Q128J_GPIO_D2_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(W25Q128J_GPIO_D2_PORT, &gpio_init_structure);

    /* QSPI D3 GPIO pin configuration  */
    gpio_init_structure.Pin       = W25Q128J_GPIO_D3_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(W25Q128J_GPIO_D3_PORT, &gpio_init_structure);

    /*##-3- Configure the NVIC for QSPI #########################################*/
    /* NVIC configuration for QSPI interrupt */
    HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
}

static void W25Q128J_Init(w25q128j_t *self)
{
    self->handle.Instance = QUADSPI;

    if (HAL_QSPI_DeInit(&self->handle) != HAL_OK)
    {
        ERROR("QSPI deinit fail.\n");
    }

    /* */
    W25Q128J_MspInit(&self->handle, NULL);
    self->handle.Init.ClockPrescaler = 1;   /* QSPI freq = 216 MHz/(1 + 1) = 108 MHz */
    self->handle.Init.FifoThreshold = 4;
    self->handle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    self->handle.Init.FlashSize = POSITION_VAL(W25Q128J_FLASH_SIZE) - 1;
    self->handle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
    self->handle.Init.ClockMode = QSPI_CLOCK_MODE_0;
    self->handle.Init.FlashID = QSPI_FLASH_ID_1;
    self->handle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

    if (HAL_QSPI_Init(&self->handle) != HAL_OK)
    {
        ERROR("QSPI init fail.\n");
        return ;
    }

    W25Q128J_Reset(&self->handle);
    TRACE_INFO("OK\n");
}

static void W25Q128J_Read(w25q128j_t *self, uint8_t *data, uint32_t addr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the read command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FAST_READ_QUAD_INOUT_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = addr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = W25Q128J_DUMMY_CYCLES_READ_QUAD;
    s_command.NbData            = size;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Configure the command */
    if (HAL_QSPI_Command(&self->handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return;
    }
    
    /* Set S# timing for Read command */
    MODIFY_REG(self->handle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE);
    
    /* Reception of the data */
    if (HAL_QSPI_Receive(&self->handle, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return;
    }
    
    /* Restore S# timing for nonRead commands */
    MODIFY_REG(self->handle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE);
    TRACE_INFO("OK\n");
}

static void W25Q128J_Write(w25q128j_t *self, uint8_t *data, uint32_t addr, uint32_t size)
{
    QSPI_CommandTypeDef s_command;
    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_size = W25Q128J_PAGE_SIZE - (addr % W25Q128J_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > size)
    {
        current_size = size;
    }

    /* Initialize the adress variables */
    current_addr = addr;
    end_addr = addr + size;

    /* Initialize the program command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_PAGE_PROG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Perform the write page by page */
    do
    {
        s_command.Address = current_addr;
        s_command.NbData  = current_size;

        /* Enable write operations */
        if (W25Q128J_WriteEnable(&self->handle) != W25Q128J_OK)
        {
            return ;
        }
        
        /* Configure the command */
        if (HAL_QSPI_Command(&self->handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return ;
        }
        
        /* Transmission of the data */
        if (HAL_QSPI_Transmit(&self->handle, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return ;
        }
        
        /* Configure automatic polling mode to wait for end of program */  
        if (W25Q128J_AutoPollingMemReady(&self->handle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != W25Q128J_OK)
        {
            return ;
        }
        
        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        data += current_size;
        current_size = ((current_addr + W25Q128J_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128J_PAGE_SIZE;

    } while (current_addr < end_addr);
    TRACE_INFO("OK\n");
    return ;
}

/**
  * @brief  Erases the specified block of the QSPI memory. 
  * @param  BlockAddress: Block address to erase  
  * @retval QSPI memory status
  */
uint8_t W25Q128J_Erase_Block(w25q128j_t *self, uint32_t BlockAddress)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = SECTOR_ERASE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.Address = BlockAddress;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (W25Q128J_WriteEnable(&self->handle) != W25Q128J_OK)
    {
        return W25Q128J_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(&self->handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return W25Q128J_ERROR;
    }

    /* Configure automatic polling mode to wait for end of erase */
    if (W25Q128J_AutoPollingMemReady(&self->handle, W25Q128J_SECTOR_ERASE_MAX_TIME) != W25Q128J_OK)
    {
        return W25Q128J_ERROR;
    }

    return W25Q128J_OK;
}

/**
  * @brief  Erases the entire QSPI memory.
  * @retval QSPI memory status
  */
uint8_t W25Q128J_Erase_Chip(w25q128j_t *self)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = CHIP_ERASE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (W25Q128J_WriteEnable(&self->handle) != W25Q128J_OK)
    {
        return W25Q128J_ERROR;
    }

    /* Send the command */
    if (HAL_QSPI_Command(&self->handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return W25Q128J_ERROR;
    }

    /* Configure automatic polling mode to wait for end of erase */
    if (W25Q128J_AutoPollingMemReady(&self->handle, W25Q128J_CHIP_ERASE_MAX_TIME) != W25Q128J_OK)
    {
        return W25Q128J_ERROR;
    }

    return W25Q128J_OK;
}

/**
  * @brief  Reads current status of the QSPI memory.
  * @retval QSPI memory status
  */
uint8_t W25Q128J_GetStatus(w25q128j_t *self)
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg;

    /* Initialize the read flag status register command */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = READ_STATUS_REG_2_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&self->handle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return W25Q128J_ERROR;
    }

    /* Reception of the data */
    if (HAL_QSPI_Receive(&self->handle, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return W25Q128J_ERROR;
    }

    /* Check the value of the register */
    if ((reg & W25Q128J_SR2_SUS) != 0)
    {
        return W25Q128J_SUSPENED;
    }
    else if ((reg & W25Q128J_SR_BUSY) != 0)
    {
        return W25Q128J_OK;
    }
    else
    {
        return W25Q128J_BUSY;
    }
}

/**
  * @brief  Return the configuration of the QSPI memory.
  * @param  pInfo: pointer on the configuration structure  
  * @retval QSPI memory status
  */
uint8_t W25Q128J_GetInfo(W25Q128J_Info* pInfo)
{
    /* Configure the structure with the memory configuration */
    pInfo->FlashSize          = W25Q128J_FLASH_SIZE;
    pInfo->EraseSectorSize    = W25Q128J_SECTOR_SIZE;
    pInfo->EraseSectorsNumber = (W25Q128J_FLASH_SIZE/W25Q128J_SECTOR_SIZE);
    pInfo->ProgPageSize       = W25Q128J_PAGE_SIZE;
    pInfo->ProgPagesNumber    = (W25Q128J_FLASH_SIZE/W25Q128J_PAGE_SIZE);

    return W25Q128J_OK;
}

/**
  * @brief  Configure the QSPI in memory-mapped mode
  * @retval QSPI memory status
  */
uint8_t W25Q128J_EnableMemoryMappedMode(w25q128j_t *self)
{
    QSPI_CommandTypeDef      s_command;
    QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

    /* Configure the command for the read instruction */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FAST_READ_QUAD_INOUT_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = W25Q128J_DUMMY_CYCLES_READ_QUAD;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    
    /* Configure the memory mapped mode */
    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    s_mem_mapped_cfg.TimeOutPeriod     = 0;
    
    if (HAL_QSPI_MemoryMapped(&self->handle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
    {
        return W25Q128J_ERROR;
    }

    return W25Q128J_OK;
}

static void W25Q128J_Reset(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the reset enable command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = RESET_ENABLE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        ERROR("CMD Enable fail.\n");
        return;
    }

    /* Send the reset memory command */
    s_command.Instruction = RESET_MEMORY_CMD;
    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        ERROR("Reset Memory fail.\n");
        return;
    }

    /* Configure automatic polling mode to wait the memory is ready */  
    if (W25Q128J_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != W25Q128J_OK)
    {
        ERROR("Auto polling mode fail.\n");
        return;
    }
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hqspi: QSPI handle
  * @param  Timeout
  * @retval None
  */
static uint8_t W25Q128J_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Configure automatic polling mode to wait for memory ready */  
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    s_config.Match           = 0;
    s_config.Mask            = W25Q128J_SR_BUSY;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, Timeout) != HAL_OK)
    {
        return W25Q128J_ERROR;
    }

    return W25Q128J_OK;
}

/**
 * @brief  This function send a Write Enable and wait it is effective.
 * @param  hqspi: QSPI handle
 * @retval None
 */
static uint8_t W25Q128J_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Enable write operations */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = WRITE_ENABLE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return W25Q128J_OK;
    }

    /* Configure automatic polling mode to wait for write enabling */
    s_config.Match = W25Q128J_SR_WEL;
    s_config.Mask = W25Q128J_SR_WEL;

    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = READ_STATUS_REG_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;

    if (HAL_QSPI_AutoPolling(hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return W25Q128J_ERROR;
    }

    return W25Q128J_OK;
}

w25q128j_t *W25Q128J_Create()
{
    w25_qspi = (w25q128j_t*)malloc(sizeof(w25q128j_t));

    w25_qspi->init = W25Q128J_Init;
    w25_qspi->write = W25Q128J_Write;
    w25_qspi->read = W25Q128J_Read;
    w25_qspi->erase_block = W25Q128J_Erase_Block;
    w25_qspi->mapped_memory = W25Q128J_EnableMemoryMappedMode;

    return w25_qspi;
}


/******************************** End of file *********************************/



/**
 * @file       w25q128j.c
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
#include "w25q128j.h"
#include "w25q128j_reg.h"
#include "stdlib.h"
#include "ace_bsp.h"
#include "reg_util.h"
#include "common.h"
/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/
#undef BARE_METAL

/*******************************************************************************
**                      COMMON VARIABLE DEFINITIONS
*******************************************************************************/
extern void DMA2_Stream7_IRQHandler(void);
extern void QuadSPI_IRQHandler(void);
/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
static drv_qspi_flash_t *m_w25q128j_qspi;
static QSPI_HandleTypeDef m_qspi_handle;
static DMA_HandleTypeDef     DmaHandle;
/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/



static QSPI_RESULT w25q128j_init(drv_qspi_flash_t *self, uint8_t mode_tranfer);
static uint32_t w25q128j_init_hw();

/* w25q128j  */
static HAL_StatusTypeDef w25q128j_command(QSPI_CommandTypeDef *cmd, uint32_t timeout);
static HAL_StatusTypeDef w25q128j_transmit(uint8_t *data, uint32_t timeout);
static HAL_StatusTypeDef w25q128j_received(uint8_t *data, uint32_t timeout);
static HAL_StatusTypeDef w25q128j_autoPolling(QSPI_CommandTypeDef *cmd, QSPI_AutoPollingTypeDef *cfg, uint32_t timeout);
/*  */

static QSPI_RESULT w25q128j_status(drv_qspi_flash_t *self);

static QSPI_RESULT w25q128j_reset(drv_qspi_flash_t *self);

static QSPI_RESULT w25q128j_read(drv_qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size);

static QSPI_RESULT w25q128j_write(drv_qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size);

static QSPI_RESULT w25q128j_erase_block(drv_qspi_flash_t *self, uint32_t address);

static QSPI_RESULT w25q128j_erase_chip(drv_qspi_flash_t *self);

static QSPI_RESULT w25q128j_memoryMapped_active(drv_qspi_flash_t *self);

#if defined(BARE_METAL)
#else
static QSPI_RESULT w25q128j_WriteEnable();

static QSPI_RESULT w25q128j_AutoPollingMemReady(uint32_t timeout);
#endif 
/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

drv_qspi_flash_t *w25q128j_Create()
{
    if (m_w25q128j_qspi == NULL)
    {
        m_w25q128j_qspi = (drv_qspi_flash_t*)malloc(sizeof(drv_qspi_flash_t));

        if(NULL == m_w25q128j_qspi)
        {
            return NULL;
        }
        m_w25q128j_qspi->state = DRV_QSPI_STATE_NONE;
        m_w25q128j_qspi->init = w25q128j_init;
        m_w25q128j_qspi->get_status = w25q128j_status;
        m_w25q128j_qspi->reset = w25q128j_reset;
        m_w25q128j_qspi->read = w25q128j_read;
        m_w25q128j_qspi->write = w25q128j_write;
        m_w25q128j_qspi->erase_block = w25q128j_erase_block;
        m_w25q128j_qspi->erase_chip = w25q128j_erase_chip;
        m_w25q128j_qspi->memoryMapped_active = w25q128j_memoryMapped_active;
    }
    return m_w25q128j_qspi;
}

static QSPI_RESULT w25q128j_init(drv_qspi_flash_t *self, uint8_t mode_tranfer)
{
    self->state = DRV_QSPI_STATE_INIT;
    w25q128j_init_hw();
    self->state = DRV_QSPI_STATE_IDLE;
    self->mode_tranfer = mode_tranfer;
    return QSPI_RESULT_OK;
}

static QSPI_RESULT w25q128j_status(drv_qspi_flash_t *self)
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg1, reg2, reg3;

    /* Initialize the read flag status register command */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = READ_STATUS_REG_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* Reception of the data */
    if (w25q128j_received(&reg1, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    s_command.Instruction = READ_STATUS_REG_2_CMD;
    /* Configure the command */
    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* Reception of the data */
    if (w25q128j_received(&reg2, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    s_command.Instruction = READ_STATUS_REG_3_CMD;
    /* Configure the command */
    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* Reception of the data */
    if (w25q128j_received(&reg3, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }
    // TRACE_INFO("reg1 = %x reg2 = %x reg3 = %x\n", reg1, reg2, reg3);
    return QSPI_RESULT_OK;
}

static QSPI_RESULT w25q128j_reset(drv_qspi_flash_t *self)
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
    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // ERROR("CMD Enable fail.\n");
        return QSPI_RESULT_FAIL;
    }

    /* Send the reset memory command */
    s_command.Instruction = RESET_MEMORY_CMD;
    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        // ERROR("Reset Memory fail.\n");
        return QSPI_RESULT_FAIL;
    }

    /* Configure automatic polling mode to wait the memory is ready */  
    if (w25q128j_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_RESULT_OK)
    {
        // ERROR("Auto polling mode fail.\n");
        return QSPI_RESULT_FAIL;
    }
    return QSPI_RESULT_OK;
}

static QSPI_RESULT w25q128j_read(drv_qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size)
{
    if (self->state == DRV_QSPI_STATE_IDLE)
    {
        QSPI_CommandTypeDef s_command;

        self->state = DRV_QSPI_STATE_READ;
        /* Initialize the read command */
        s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
        s_command.Instruction       = FAST_READ_QUAD_INOUT_CMD;
        s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
        s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
        s_command.Address           = address;
        s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        s_command.DataMode          = QSPI_DATA_4_LINES;
        s_command.DummyCycles       = W25Q128J_DUMMY_CYCLES_READ_QUAD;
        s_command.NbData            = size;
        s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
        s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
        s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
        
        /* Configure the command */
        if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return QSPI_RESULT_TIMEOUT;
        }
        
        /* Set S# timing for Read command */
        MODIFY_REG(m_qspi_handle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE);
        
        /* Reception of the data */
        if (w25q128j_received(data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            return QSPI_RESULT_TIMEOUT;
        }
        
        /* Restore S# timing for nonRead commands */
        MODIFY_REG(m_qspi_handle.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE);
        self->state = DRV_QSPI_STATE_IDLE;
        return QSPI_RESULT_OK;
    }
    return QSPI_RESULT_FAIL;
}

static QSPI_RESULT w25q128j_write(drv_qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size)
{
    if (self->state == DRV_QSPI_STATE_IDLE)
    {
        QSPI_CommandTypeDef s_command;
        uint32_t end_addr, current_size, current_addr;
        self->state = DRV_QSPI_STATE_WRITE;
        /* Calculation of the size between the write address and the end of the page */
        current_size = W25Q128J_PAGE_SIZE - (address % W25Q128J_PAGE_SIZE);

        /* Check if the size of the data is less than the remaining place in the page */
        if (current_size > size)
        {
            current_size = size;
        }

        /* Initialize the adress variables */
        current_addr = address;
        end_addr = address + size;

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
            if (w25q128j_WriteEnable() != QSPI_RESULT_OK)
            {
                return QSPI_RESULT_FAIL;
            }
            
            /* Configure the command */
            if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
            {
                return QSPI_RESULT_FAIL;
            }
            
            /* Transmission of the data */
            if (w25q128j_transmit(data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
            {
                return QSPI_RESULT_FAIL;
            }
            
            /* Configure automatic polling mode to wait for end of program */  
            if (w25q128j_AutoPollingMemReady(HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_RESULT_OK)
            {
                return QSPI_RESULT_FAIL;
            }
            
            /* Update the address and size variables for next page programming */
            current_addr += current_size;
            data += current_size;
            current_size = ((current_addr + W25Q128J_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128J_PAGE_SIZE;

        } while (current_addr < end_addr);
        self->state = DRV_QSPI_STATE_IDLE;
        return QSPI_RESULT_OK;
    }
    return QSPI_RESULT_FAIL;
}

static QSPI_RESULT w25q128j_erase_block(drv_qspi_flash_t *self, uint32_t address)
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = SECTOR_ERASE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.Address = address;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if (w25q128j_WriteEnable() != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* Send the command */
    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* Configure automatic polling mode to wait for end of erase */
    if (w25q128j_AutoPollingMemReady(W25Q128J_SECTOR_ERASE_MAX_TIME) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    return QSPI_RESULT_OK;
}

static QSPI_RESULT w25q128j_erase_chip(drv_qspi_flash_t *self)
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
    if (w25q128j_WriteEnable() != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* Send the command */
    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* Configure automatic polling mode to wait for end of erase */
    if (w25q128j_AutoPollingMemReady(W25Q128J_CHIP_ERASE_MAX_TIME) != QSPI_RESULT_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    return QSPI_RESULT_OK;
}

static QSPI_RESULT w25q128j_memoryMapped_active(drv_qspi_flash_t *self)
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
    
    if (HAL_QSPI_MemoryMapped(&m_qspi_handle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    return QSPI_RESULT_OK;
}

static void w25q128j_init_hw_pin()
{
#if defined(BARE_METAL)
    RCC_TypeDef *pRCC = RCC;

    /* Setup clk qspi */
    REG_SET_BIT(pRCC->AHB3ENR, RCC_AHB3ENR_QSPIEN_Pos);
    /* setup clk for GPIOB, D, E */
    REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN_Pos);
    REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIODEN_Pos);
    REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN_Pos);

    /* QUADSPI_CLK pin */
    REG_SET_VAL(W25Q128J_GPIO_CLK_PORT->MODER , 0x2U, 0x3, (W25Q128J_GPIO_CLK_PIN_pos * 2U)); 			/* Mode : Alternate func */
	REG_CLR_BIT(W25Q128J_GPIO_CLK_PORT->OTYPER, W25Q128J_GPIO_CLK_PIN_pos); 							/* Output type : push-pull*/
	REG_SET_VAL(W25Q128J_GPIO_CLK_PORT->OSPEEDR, 2U, 0x3U, (W25Q128J_GPIO_CLK_PIN_pos * 2U));           /* Speed : high speed */    
    REG_SET_VAL(W25Q128J_GPIO_CLK_PORT->AFR[0], 0x9U, 0xFU, (W25Q128J_GPIO_CLK_PIN_pos * 4U));          /* Alternate func : AF09 */
    /* QUADSPI_BK1_NCS pin */
    REG_SET_VAL(W25Q128J_GPIO_CS_PORT->MODER , 0x2U, 0x3, (W25Q128J_GPIO_CS_PIN_pos * 2U)); 			/* Mode : Alternate func */
	REG_CLR_BIT(W25Q128J_GPIO_CS_PORT->OTYPER, W25Q128J_GPIO_CS_PIN_pos); 							    /* Output type : push-pull*/
    REG_SET_VAL(W25Q128J_GPIO_CS_PORT->PUPDR, 0x1U, 0x3U, (W25Q128J_GPIO_CS_PIN_pos * 2U));             /* Push down */
	REG_SET_VAL(W25Q128J_GPIO_CS_PORT->OSPEEDR, 2U, 0x3U, (W25Q128J_GPIO_CS_PIN_pos * 2U));             /* Speed : high speed */   
    REG_SET_VAL(W25Q128J_GPIO_CS_PORT->AFR[0], 0xAU, 0xFU, (W25Q128J_GPIO_CS_PIN_pos * 4U));            /* Alternate func : AF10 */
    /* QUADSPI_BK1_IO0 pin */
    REG_SET_VAL(W25Q128J_GPIO_D0_PORT->MODER , 0x2U, 0x3, (W25Q128J_GPIO_D0_PIN_pos * 2U)); 			/* Mode : Alternate func */
	REG_CLR_BIT(W25Q128J_GPIO_D0_PORT->OTYPER, W25Q128J_GPIO_D0_PIN_pos); 							    /* Output type : push-pull*/
	REG_SET_VAL(W25Q128J_GPIO_D0_PORT->OSPEEDR, 2U, 0x3U, (W25Q128J_GPIO_D0_PIN_pos * 2U));             /* Speed : high speed */   
    REG_SET_VAL(W25Q128J_GPIO_D0_PORT->AFR[1], 0x9U, 0xFU, ((W25Q128J_GPIO_D0_PIN_pos%8) * 4U));        /* Alternate func : AF09 */
    /* QUADSPI_BK1_IO1 pin */
    REG_SET_VAL(W25Q128J_GPIO_D1_PORT->MODER , 0x2U, 0x3, (W25Q128J_GPIO_D1_PIN_pos * 2U)); 			/* Mode : Alternate func */
	REG_CLR_BIT(W25Q128J_GPIO_D1_PORT->OTYPER, W25Q128J_GPIO_D1_PIN_pos); 							    /* Output type : push-pull*/
	REG_SET_VAL(W25Q128J_GPIO_D1_PORT->OSPEEDR, 2U, 0x3U, (W25Q128J_GPIO_D1_PIN_pos * 2U));             /* Speed : high speed */   
    REG_SET_VAL(W25Q128J_GPIO_D1_PORT->AFR[1], 0x9U, 0xFU, ((W25Q128J_GPIO_D1_PIN_pos%8) * 4U));        /* Alternate func : AF09 */
    /* QUADSPI_BK1_IO2 pin */
    REG_SET_VAL(W25Q128J_GPIO_D2_PORT->MODER , 0x2U, 0x3, (W25Q128J_GPIO_D2_PIN_pos * 2U)); 			/* Mode : Alternate func */
	REG_CLR_BIT(W25Q128J_GPIO_D2_PORT->OTYPER, W25Q128J_GPIO_D2_PIN_pos); 							    /* Output type : push-pull*/
	REG_SET_VAL(W25Q128J_GPIO_D2_PORT->OSPEEDR, 2U, 0x3U, (W25Q128J_GPIO_D2_PIN_pos * 2U));             /* Speed : high speed */   
    REG_SET_VAL(W25Q128J_GPIO_D2_PORT->AFR[0], 0x9U, 0xFU, (W25Q128J_GPIO_D2_PIN_pos * 4U));            /* Alternate func : AF09 */
    /* QUADSPI_BK1_IO3 pin */
    REG_SET_VAL(W25Q128J_GPIO_D3_PORT->MODER , 0x2U, 0x3, (W25Q128J_GPIO_D3_PIN_pos * 2U)); 			/* Mode : Alternate func */
	REG_CLR_BIT(W25Q128J_GPIO_D3_PORT->OTYPER, W25Q128J_GPIO_D3_PIN_pos); 							    /* Output type : push-pull*/
	REG_SET_VAL(W25Q128J_GPIO_D3_PORT->OSPEEDR, 2U, 0x3U, (W25Q128J_GPIO_D3_PIN_pos * 2U));             /* Speed : high speed */   
    REG_SET_VAL(W25Q128J_GPIO_D3_PORT->AFR[1], 0x9U, 0xFU, ((W25Q128J_GPIO_D3_PIN_pos%8) * 4U));        /* Alternate func : AF09 */
#else
    GPIO_InitTypeDef gpio_init_structure;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable the QuadSPI memory interface clock */
    W25Q128J_CLK_ENABLE();
    /* Reset the QuadSPI memory interface */
    __HAL_RCC_QSPI_FORCE_RESET();
    __HAL_RCC_QSPI_RELEASE_RESET();
    /* Enable GPIO clocks */
    W25Q128J_GPIO_CLK_ENABLE;
    /* DMA2 clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

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

    // /*##-3- Configure the NVIC for QSPI #########################################*/
    // /* NVIC configuration for QSPI interrupt */
    HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
#endif 
}

static uint32_t w25q128j_init_hw()
{
#if defined(BARE_METAL)
    w25q128j_init_hw_pin();
    /*　Configure QSPI FIFO Threshold - 4byte */
    REG_SET_VAL(QUADSPI->CR, 0x03U, 0x1FU, QUADSPI_CR_FTHRES_Pos);

    while (REG_READ_BIT(QUADSPI->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
    }

    /* 1. Configure QSPI Clock Prescaler */
    REG_SET_VAL(QUADSPI->CR, 0x1U, 0xFFU, QUADSPI_CR_PRESCALER_Pos);
    /* 2. Configure QSPI SampleShift */
    REG_SET_BIT(QUADSPI->CR, QUADSPI_CR_SSHIFT_Pos);
    /* 3. Configure QSPI flash memory selection */
    REG_CLR_BIT(QUADSPI->CR, QUADSPI_CR_FSEL_Pos);
    /* 4. Configure QSPI Dual flash mode */
    REG_CLR_BIT(QUADSPI->CR, QUADSPI_CR_DFM_Pos);
    /* 5. Configure QSPI NCS low/high - MODE0 */
    REG_CLR_BIT(QUADSPI->DCR, QUADSPI_DCR_CKMODE_Pos);
    /* 6. Configure QSPI Chip select high time 6 cycle */
    REG_SET_VAL(QUADSPI->DCR, 0x5U, 0x3U, QUADSPI_DCR_CSHT_Pos);
    /* 7. Configure QSPI flash size */
    REG_SET_VAL(QUADSPI->DCR, 0x17U, 0x1FU, QUADSPI_DCR_FSIZE_Pos);

    /* Enable QSPI */
    REG_SET_BIT(QUADSPI->CR, QUADSPI_CR_EN_Pos);
#else
    m_qspi_handle.Instance = QUADSPI;

    if (HAL_QSPI_DeInit(&m_qspi_handle) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    /* */
    w25q128j_init_hw_pin();
    m_qspi_handle.Init.ClockPrescaler = 1;   /* QSPI freq = 216 MHz/(1 + 1) = 108 MHz */
    m_qspi_handle.Init.FifoThreshold = 4;
    m_qspi_handle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    m_qspi_handle.Init.FlashSize = POSITION_VAL(W25Q128J_FLASH_SIZE) - 1;
    m_qspi_handle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
    m_qspi_handle.Init.ClockMode = QSPI_CLOCK_MODE_0;
    m_qspi_handle.Init.FlashID = QSPI_FLASH_ID_1;
    m_qspi_handle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
    

    m_qspi_handle.hdma->Init.Channel = DMA_CHANNEL_3;                           /* DMA_CHANNEL_0                    */
    // m_qspi_handle.hdma->Init.Direction = DMA_MEMORY_TO_MEMORY;                /* M2M transfer mode                */
    // m_qspi_handle.hdma->Init.PeriphInc = DMA_PINC_ENABLE;                     /* Peripheral increment mode Enable */
    // m_qspi_handle.hdma->Init.MemInc = DMA_MINC_ENABLE;                        /* Memory increment mode Enable     */
    m_qspi_handle.hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;       /* Peripheral data alignment : Word */
    m_qspi_handle.hdma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;          /* memory data alignment : Word     */
    m_qspi_handle.hdma->Init.Mode = DMA_NORMAL;                               /* Normal DMA mode                  */
    m_qspi_handle.hdma->Init.Priority = DMA_PRIORITY_HIGH;                    /* priority level : high            */
    m_qspi_handle.hdma->Init.FIFOMode = DMA_FIFOMODE_ENABLE;                  /* FIFO mode enabled                */
    m_qspi_handle.hdma->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL; /* FIFO threshold: 1/4 full   */
    m_qspi_handle.hdma->Init.MemBurst = DMA_MBURST_SINGLE;                    /* Memory burst                     */
    m_qspi_handle.hdma->Init.PeriphBurst = DMA_PBURST_SINGLE;                 /* Peripheral burst                 */

    m_qspi_handle.hdma->Instance = DMA2_Stream0;

    if (HAL_DMA_Init(m_qspi_handle.hdma) != HAL_OK)
    {
        /* Initialization Error */

    }

    if (HAL_QSPI_Init(&m_qspi_handle) != HAL_OK)
    {
        // ERROR("QSPI init fail.\n");
        return QSPI_RESULT_FAIL;
    }

    if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_DMA)
    {
        // w25q128j_DMA_Init();
    }
    return QSPI_RESULT_OK;
#endif
}

#if defined(BARE_METAL)
static uint32_t w25q128j_cmd()
{

    while (REG_READ_BIT(QUADSPI->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
        // ERROR("error\n;");
    }

    w25q128j_cfg(QUADSPI, cmd, QUADSPI_FMODE_INDIRECT_WRITE);

    if (cmd->DataMode == QUADSPI_DATA_MODE_NONE)
    {   
        while (REG_READ_BIT(QUADSPI->SR, QUADSPI_SR_TCF_Pos) != 1)
        {
            /* code */
            // ERROR("error\n;");
        }
        REG_SET_BIT(QUADSPI->FCR, QUADSPI_FCR_CTCF_Pos);
    }
} 

static uint32_t w25q128j_receive()
{

}

static uint32_t w25q128j_transmit()
{

}

static uint32_t w25q128j_cfg(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, uint32_t mode)
{

}

static uint32_t w25q128j_AutoPolling()
{

}

static uint32_t w25q128j_MemoryMapped()
{
    
}
#else
static QSPI_RESULT w25q128j_WriteEnable()
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

    if (w25q128j_command(&s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
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

    if (w25q128j_autoPolling(&s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    return QSPI_RESULT_OK;
}

static QSPI_RESULT w25q128j_AutoPollingMemReady(uint32_t timeout)
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

    if (w25q128j_autoPolling(&s_command, &s_config, timeout) != HAL_OK)
    {
        return QSPI_RESULT_FAIL;
    }

    return QSPI_RESULT_OK;
}
#endif 

static HAL_StatusTypeDef w25q128j_command(QSPI_CommandTypeDef *cmd, uint32_t timeout)
{
    HAL_StatusTypeDef result = HAL_OK;
    if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_POLLING)
    {
        result = HAL_QSPI_Command(&m_qspi_handle, cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_IT)
    {
        result = HAL_QSPI_Command_IT(&m_qspi_handle, cmd);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_DMA)
    {
        result = HAL_QSPI_Command_IT(&m_qspi_handle, cmd);
    }
    return result;
    
}

static HAL_StatusTypeDef w25q128j_transmit(uint8_t *data, uint32_t timeout)
{
    HAL_StatusTypeDef result = HAL_OK;
    if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_POLLING)
    {
        result = HAL_QSPI_Transmit(&m_qspi_handle, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_IT)
    {
        result = HAL_QSPI_Transmit_IT(&m_qspi_handle, data);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_DMA)
    {
        result = HAL_QSPI_Transmit_DMA(&m_qspi_handle, data);
    }
    return result;
}

static HAL_StatusTypeDef w25q128j_received(uint8_t *data, uint32_t timeout)
{
    HAL_StatusTypeDef result = HAL_OK;
    if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_POLLING)
    {
        result = HAL_QSPI_Receive(&m_qspi_handle, data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_IT)
    {
        result = HAL_QSPI_Receive_IT(&m_qspi_handle, data);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_DMA)
    {
        result = HAL_QSPI_Receive_DMA(&m_qspi_handle, data);
    }
    return result;
}

static HAL_StatusTypeDef w25q128j_autoPolling(QSPI_CommandTypeDef *cmd, QSPI_AutoPollingTypeDef *cfg, uint32_t timeout)
{
    HAL_StatusTypeDef result = HAL_OK;
    if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_POLLING)
    {
        result = HAL_QSPI_AutoPolling(&m_qspi_handle, cmd, cfg, timeout);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_IT)
    {
        result = HAL_QSPI_AutoPolling_IT(&m_qspi_handle, cmd, cfg);
    }
    else if (m_w25q128j_qspi->mode_tranfer == DRV_QSPI_DMA)
    {
        result = HAL_QSPI_AutoPolling_IT(&m_qspi_handle, cmd, cfg);
    }
    return result;
}

void QuadSPI_IRQHandler(void)
{
    HAL_QSPI_IRQHandler(&m_qspi_handle);
}

void DMA2_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(m_qspi_handle.hdma);
}


/******************************** End of file *********************************/



/**
 * @file       qspi_flash.c
 * @brief      
 * @date       2025/10/19
 * @author     [Gentantun] (nguyenthanhtung8196@gmail.com)
 * @details    
 * @ref        
 * @copyright  Copyright (c) 2025 RoboTun
*/

/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "stdlib.h"
#include "qspi_flash.h"
#include "w25q128j_reg.h"
#include "w25q128j.h"

/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/
qspi_flash_t *m_qspi_flash;

/*******************************************************************************
**                      COMMON VARIABLE DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/


/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/
static void qspi_flash_setup_pin();
static void qspi_flash_setup_drive(QUADSPI_TypeDef *pQspi);
static void qspi_flash_reset(QUADSPI_TypeDef *pQspi);
static void qspi_flash_cmd_cfg(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, uint32_t mode);
static void qspi_flash_autoPolling(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, qspi_flash_autoPolling_t *cfg, uint32_t Timeout);
static void qspi_flash_autoPollingMemReady(QUADSPI_TypeDef *pQspi, uint32_t Timeout);
static void qspi_flash_receive(QUADSPI_TypeDef *pQspi, uint8_t *data, uint32_t timeout);
static void qspi_flash_transmit(QUADSPI_TypeDef *pQspi, uint8_t *data, uint32_t timeout);
static uint8_t qspi_flash_getStatus(QUADSPI_TypeDef *pQspi);
static void qspi_flash_writeEnable(QUADSPI_TypeDef *pQspi);
static void qspi_flash_cmd(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd);
static void qspi_flash_MemoryMapped(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, qspi_flash_memory_mapped_t *memo_cfg);
/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

static void qspi_flash_init(qspi_flash_t *self)
{
    uint8_t res = 0;
    qspi_flash_setup_pin();
    qspi_flash_setup_drive(self->pQspi);
    qspi_flash_reset(self->pQspi);
    res = qspi_flash_getStatus(self->pQspi);
    TRACE_INFO("res = %d\n", res);
}

static void qspi_flash_write(qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size)
{
    qspi_flash_cmd_t     s_command;
    uint32_t end_addr, current_size, current_addr;

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
    s_command.InstructionModes   = QSPI_INSTRUCTION_1_LINE;
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
        qspi_flash_writeEnable(self->pQspi);
        
        /* Configure the command */
        qspi_flash_cmd(self->pQspi, &s_command);

        /* Transmission of the data */
        qspi_flash_transmit(self->pQspi, data, 0);

        /* Configure automatic polling mode to wait for end of program */  
        qspi_flash_autoPollingMemReady(self->pQspi, 0);

        
        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        data += current_size;
        current_size = ((current_addr + W25Q128J_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128J_PAGE_SIZE;

    } while (current_addr < end_addr);
}

static void qspi_flash_read(qspi_flash_t *self, uint32_t address, uint8_t *data, uint16_t size)
{
    qspi_flash_cmd_t s_command;

    #if 0
    /* Initialize the read command */
    s_command.InstructionModes   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = FAST_READ_QUAD_INOUT_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
    s_command.Address           = address;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
    s_command.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    s_command.AlternateBytes       = 0xFF;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 4;
    s_command.NbData            = size;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
    #else
    /* Initialize the read command */
    s_command.InstructionModes   = QSPI_INSTRUCTION_1_LINE;
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
    #endif 
    /* Configure the command */
    qspi_flash_cmd(self->pQspi, &s_command);
    
    /* Set S# timing for Read command */
    REG_MODIFY(self->pQspi->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE);
    
    /* Reception of the data */
    qspi_flash_receive(self->pQspi, data, 0);
    
    /* Restore S# timing for nonRead commands */
    REG_MODIFY(self->pQspi->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE);
}

static void qspi_flash_erase_block(qspi_flash_t *self, uint32_t BlockAddress)
{
    qspi_flash_cmd_t s_command;

    /* Initialize the erase command */
    s_command.InstructionModes = QSPI_INSTRUCTION_1_LINE;
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
    qspi_flash_writeEnable(self->pQspi);

    /* Send the command */
    qspi_flash_cmd(self->pQspi, &s_command);

    /* Configure automatic polling mode to wait for end of erase */
    qspi_flash_autoPollingMemReady(self->pQspi, 0);
}

static void qspi_flash_enable_memo_mapped(qspi_flash_t *self)
{
    qspi_flash_cmd_t      s_command;
    qspi_flash_memory_mapped_t s_mem_mapped_cfg;

    /* Configure the command for the read instruction */
    s_command.InstructionModes   = QSPI_INSTRUCTION_1_LINE;
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
    s_mem_mapped_cfg.TimeOutActivations = QSPI_TIMEOUT_COUNTER_DISABLE;
    s_mem_mapped_cfg.TimeoutPeriod     = 0;
    
    qspi_flash_MemoryMapped(self->pQspi, &s_command, &s_mem_mapped_cfg);
}

qspi_flash_t *qspi_flash_create()
{
    m_qspi_flash = (qspi_flash_t*)malloc(sizeof(qspi_flash_t));

    if (NULL == m_qspi_flash)
    {
        return NULL;
    }

    m_qspi_flash->pQspi = QUADSPI;
    m_qspi_flash->init = qspi_flash_init;
    m_qspi_flash->read = qspi_flash_read;
    m_qspi_flash->write = qspi_flash_write;
    m_qspi_flash->erase_chip = qspi_flash_erase_block;
    m_qspi_flash->memory_mapped = qspi_flash_enable_memo_mapped;

    return m_qspi_flash;
}

/**
 * @brief      
 * 
 *       QUADSPI_CLK            PB2
 *       QUADSPI_BK1_NCS        PB6
 *       QUADSPI_BK1_IO1        PD12
 *       QUADSPI_BK1_IO2        PE2
 *       QUADSPI_BK1_IO3        PD13
 * 
*/
static void qspi_flash_setup_pin()
{
    RCC_TypeDef *pRCC = RCC;

    /* Setup clk qspi */
    REG_SET_BIT(pRCC->AHB3ENR, RCC_AHB3ENR_QSPIEN_Pos);
    /* setup clk for GPIOB, D, E */
    REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN_Pos);
    REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIODEN_Pos);
    REG_SET_BIT(pRCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN_Pos);


    /* QUADSPI_CLK pin */
    REG_SET_VAL(QUADSPI_CLK_port->MODER , 0x2U, 0x3, (QUADSPI_CLK_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_CLK_port->OTYPER, QUADSPI_CLK_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_CLK_port->OSPEEDR, 2U, 0x3U, (QUADSPI_CLK_pin * 2U));           /* Speed : high speed */    
    REG_SET_VAL(QUADSPI_CLK_port->AFR[0], 0x9U, 0xFU, (QUADSPI_CLK_pin * 4U));       
    /* QUADSPI_BK1_NCS pin */
    REG_SET_VAL(QUADSPI_BK1_NCS_port->MODER , 0x2U, 0x3, (QUADSPI_BK1_NCS_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_NCS_port->OTYPER, QUADSPI_BK1_NCS_pin); 							/* Output type : push-pull*/
    REG_SET_VAL(QUADSPI_BK1_NCS_port->PUPDR, 0x1U, 0x3U, (QUADSPI_BK1_NCS_pin * 2U));
	REG_SET_VAL(QUADSPI_BK1_NCS_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_NCS_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_NCS_port->AFR[0], 0xAU, 0xFU, (QUADSPI_BK1_NCS_pin * 4U));  
    /* QUADSPI_BK1_IO0 pin */
    REG_SET_VAL(QUADSPI_BK1_IO1_port->MODER , 0x2U, 0x3, (QUADSPI_BK1_IO0_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_IO1_port->OTYPER, QUADSPI_BK1_IO0_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_BK1_IO1_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_IO0_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_IO1_port->AFR[1], 0x9U, 0xFU, ((QUADSPI_BK1_IO0_pin%8) * 4U));     
    /* QUADSPI_BK1_IO1 pin */
    REG_SET_VAL(QUADSPI_BK1_IO1_port->MODER , 0x2U, 0x3, (QUADSPI_BK1_IO1_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_IO1_port->OTYPER, QUADSPI_BK1_IO1_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_BK1_IO1_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_IO1_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_IO1_port->AFR[1], 0x9U, 0xFU, ((QUADSPI_BK1_IO1_pin%8) * 4U));
    /* QUADSPI_BK1_IO2 pin */
    REG_SET_VAL(QUADSPI_BK1_IO2_port->MODER , 0x2U, 0x3, (QUADSPI_BK1_IO2_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_IO2_port->OTYPER, QUADSPI_BK1_IO2_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_BK1_IO2_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_IO2_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_IO2_port->AFR[0], 0x9U, 0xFU, (QUADSPI_BK1_IO2_pin * 4U)); 
    /* QUADSPI_BK1_IO3 pin */
    REG_SET_VAL(QUADSPI_BK1_IO3_port->MODER , 0x2U, 0x3, (QUADSPI_BK1_IO3_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_IO3_port->OTYPER, QUADSPI_BK1_IO3_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_BK1_IO3_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_IO3_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_IO3_port->AFR[1], 0x9U, 0xFU, ((QUADSPI_BK1_IO3_pin%8) * 4U)); 
}

static void qspi_flash_setup_drive(QUADSPI_TypeDef *pQspi)
{

    /*　Configure QSPI FIFO Threshold - 4byte */
    REG_SET_VAL(pQspi->CR, 0x03U, 0x1FU, QUADSPI_CR_FTHRES_Pos);

    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
    }

    /* 1. Configure QSPI Clock Prescaler */
    REG_SET_VAL(pQspi->CR, 0x1U, 0xFFU, QUADSPI_CR_PRESCALER_Pos);
    /* 2. Configure QSPI SampleShift */
    REG_SET_BIT(pQspi->CR, QUADSPI_CR_SSHIFT_Pos);
    /* 3. Configure QSPI flash memory selection */
    REG_CLR_BIT(pQspi->CR, QUADSPI_CR_FSEL_Pos);
    /* 4. Configure QSPI Dual flash mode */
    REG_CLR_BIT(pQspi->CR, QUADSPI_CR_DFM_Pos);
    /* 5. Configure QSPI NCS low/high - MODE0 */
    REG_CLR_BIT(pQspi->DCR, QUADSPI_DCR_CKMODE_Pos);
    /* 6. Configure QSPI Chip select high time 6 cycle */
    REG_SET_VAL(pQspi->DCR, 0x5U, 0x3U, QUADSPI_DCR_CSHT_Pos);
    /* 7. Configure QSPI flash size */
    REG_SET_VAL(pQspi->DCR, 0x17U, 0x1FU, QUADSPI_DCR_FSIZE_Pos);

    /* Enable QSPI */
    REG_SET_BIT(pQspi->CR, QUADSPI_CR_EN_Pos);
}

static void qspi_flash_cmd(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd)
{
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
        // ERROR("error\n;");
    }

    qspi_flash_cmd_cfg(pQspi, cmd, QUADSPI_FMODE_INDIRECT_WRITE);

    if (cmd->DataMode == QUADSPI_DATA_MODE_NONE)
    {   
        while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_TCF_Pos) != 1)
        {
            /* code */
            // ERROR("error\n;");
        }
        REG_SET_BIT(pQspi->FCR, QUADSPI_FCR_CTCF_Pos);
    }
}

static void qspi_flash_autoPolling(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, qspi_flash_autoPolling_t *cfg, uint32_t Timeout)
{
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
        // ERROR("error\n;");
    }
    /* Configure QSPI : PASMAR register with the status match value */
    REG_WRITE(pQspi->PSMAR, cfg->Match);
    /* Configure QSPI : PSMKR register with the status mask value */
    REG_WRITE(pQspi->PSMKR, cfg->Mask);
    /* Configure QSPI : PIR register with the interval value */
    REG_WRITE(pQspi->PIR, cfg->Interval);

    REG_MODIFY(pQspi->CR, (QUADSPI_CR_PMM | QUADSPI_CR_APMS),
               (cfg->MatchMode | QSPI_AUTOMATIC_STOP_ENABLE));
    
    cmd->NbData = cfg->StatusByteSize;
    qspi_flash_cmd_cfg(pQspi, cmd, QUADSPI_FMODE_AUTO_POLL);
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_SMF_Pos) != 1)
    {
        /* code */
        // ERROR("error\n");
    }
    REG_SET_BIT(pQspi->FCR, QUADSPI_FCR_CSMF_Pos);
}

static void qspi_flash_autoPollingMemReady(QUADSPI_TypeDef *pQspi, uint32_t Timeout)
{
    qspi_flash_cmd_t     s_command;
    qspi_flash_autoPolling_t s_config;

    /* Configure automatic polling mode to wait for memory ready */  
    s_command.InstructionModes   = QSPI_INSTRUCTION_1_LINE;
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
    s_config.StatusByteSize  = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_flash_autoPolling(pQspi, &s_command, &s_config, Timeout);
}

static void qspi_flash_transmit(QUADSPI_TypeDef *pQspi, uint8_t *data, uint32_t timeout)
{
    uint32_t data_len = 0;
    uint8_t *tmp_data = data;
    REG_SET_VAL(pQspi->CCR, QUADSPI_FMODE_INDIRECT_WRITE, 0x3U, QUADSPI_CCR_FMODE_Pos);
    data_len = REG_READ(pQspi->DLR) + 1U;
    while (data_len > 0)
    {
        while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_FTF_Pos) != 1)
        {
            /* code */
            // ERROR("error\n;");
        }
        REG_WRITE(pQspi->DR, *tmp_data);
        tmp_data++;
        data_len--;
    }
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_TCF_Pos) != 1)
    {
        /* code */
        // ERROR("error\n;");
    }

    REG_SET_BIT(pQspi->FCR, QUADSPI_FCR_CTCF_Pos);
}

static void qspi_flash_receive(QUADSPI_TypeDef *pQspi, uint8_t *data, uint32_t timeout)
{
    uint32_t data_len = 0;
    uint8_t *tmp_data = data;
    uint32_t addr_reg = REG_READ(pQspi->AR);
    REG_SET_VAL(pQspi->CCR, QUADSPI_FMODE_INDIRECT_READ, 0x3U, QUADSPI_CCR_FMODE_Pos);
    data_len = REG_READ(pQspi->DLR) + 1U;

    REG_WRITE(pQspi->AR, addr_reg);
    while (data_len > 0)
    {
        while (((REG_READ_BIT(pQspi->SR, QUADSPI_SR_FTF_Pos)) || 
                (REG_READ_BIT(pQspi->SR, QUADSPI_SR_TCF_Pos))) != 1)
        {
            /* code */
            // ERROR("error\n;");
        }
        *tmp_data = REG_READ(pQspi->DR);
        tmp_data++;
        data_len--;
    }
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_TCF_Pos) != 1)
    {
        /* code */
        ERROR("error\n;");
    }

    REG_SET_BIT(pQspi->FCR, QUADSPI_FCR_CTCF_Pos);
}

static void qspi_flash_cmd_cfg(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, uint32_t mode)
{
    if ((cmd->DataMode != QSPI_DATA_NONE) && (mode != QUADSPI_FMODE_MEMO))
    {
        /* Configure QSPI: DLR register with the number of data to read or write */
        REG_WRITE(pQspi->DLR, (cmd->NbData - 1U));
    }

    if (cmd->InstructionModes != QSPI_INSTRUCTION_NONE)
    {
        if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
        {
            /* Configure QSPI: ABR register with alternate bytes value */
            REG_WRITE(pQspi->ABR, cmd->AlternateBytes);

            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with instruction, address and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                       cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                       cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                       cmd->AddressSize | cmd->AddressMode | cmd->InstructionModes |
                                       cmd->Instruction | (mode << QUADSPI_CCR_FMODE_Pos)));

                if (mode != QUADSPI_FMODE_MEMO)
                {
                    /* Configure QSPI: AR register with address value */
                    REG_WRITE(pQspi->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with instruction and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                       cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                       cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                       cmd->AddressMode | cmd->InstructionModes |
                                       cmd->Instruction | (mode << QUADSPI_CCR_FMODE_Pos)));
            }
        }
        else
        {
            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with instruction and address ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                       cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                       cmd->AlternateByteMode | cmd->AddressSize | cmd->AddressMode |
                                       cmd->InstructionModes | cmd->Instruction | (mode << QUADSPI_CCR_FMODE_Pos)));

                if (mode != QUADSPI_FMODE_MEMO)
                {
                    /* Configure QSPI: AR register with address value */
                    REG_WRITE(pQspi->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only instruction ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                       cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                       cmd->AlternateByteMode | cmd->AddressMode |
                                       cmd->InstructionModes | cmd->Instruction | (mode << QUADSPI_CCR_FMODE_Pos)));
            }
        }
    }
    else
    {
        if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
        {
            /* Configure QSPI: ABR register with alternate bytes value */
            REG_WRITE(pQspi->ABR, cmd->AlternateBytes);

            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with address and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                       cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                       cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                       cmd->AddressSize | cmd->AddressMode |
                                       cmd->InstructionModes | (mode << QUADSPI_CCR_FMODE_Pos)));

                if (mode != QUADSPI_FMODE_MEMO)
                {
                    /* Configure QSPI: AR register with address value */
                    REG_WRITE(pQspi->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                       cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                       cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                       cmd->AddressMode | cmd->InstructionModes | (mode << QUADSPI_CCR_FMODE_Pos)));
            }
        }
        else
        {
            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with only address ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                       cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                       cmd->AlternateByteMode | cmd->AddressSize |
                                       cmd->AddressMode | cmd->InstructionModes | (mode << QUADSPI_CCR_FMODE_Pos)));

                if (mode != QUADSPI_FMODE_MEMO)
                {
                    /* Configure QSPI: AR register with address value */
                    REG_WRITE(pQspi->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only data phase ----*/
                if (cmd->DataMode != QSPI_DATA_NONE)
                {
                    /* Configure QSPI: CCR register with all communications parameters */
                    REG_WRITE(pQspi->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                           cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                           cmd->AlternateByteMode | cmd->AddressMode |
                                           cmd->InstructionModes | (mode << QUADSPI_CCR_FMODE_Pos)));
                }
            }
        }
    }
}

static void qspi_flash_reset(QUADSPI_TypeDef *pQspi)
{
    qspi_flash_cmd_t cmd;
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
    }

    /* Config command */
    // cmd.InstructionModes = QUADSPI_INS_MODE_SING_LINE;      /* instruction mode 1 line */
    // cmd.Instruction = RESET_ENABLE_CMD;                     /* instrcution command */
    // cmd.AddressMode = QUADSPI_ADR_MODE_NONE;                /* address mode none */
    // cmd.AlternateByteMode = QUADSPI_ALB_MODE_NONE;          /* alternate byte mode none */
    // cmd.DummyCycles = 0;                                    /* dummy cycles  = 0 */
    // cmd.DataMode = QUADSPI_DATA_MODE_NONE;                  /* data mode none */
    // cmd.DdrMode = QUADSPI_DDRM_DISABLE;                     /* double data mode disable  */
    // cmd.DdrHoldHalfCycle = QUADSPI_DHHC_ANALOG_DELAY;       /* using analog delay */
    // cmd.SIOOMode = QUADSPI_SIOO_EVERY_TRANS;                /* every transaction  */
    cmd.InstructionModes   = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction       = RESET_ENABLE_CMD;
    cmd.AddressMode       = QSPI_ADDRESS_NONE;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode          = QSPI_DATA_NONE;
    cmd.DummyCycles       = 0;
    cmd.DdrMode           = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    qspi_flash_cmd(pQspi, &cmd);

    /* Send the reset memory command */
    cmd.Instruction = RESET_MEMORY_CMD;
    qspi_flash_cmd(pQspi, &cmd);

    qspi_flash_autoPollingMemReady(pQspi, 0);
}

static uint8_t qspi_flash_getStatus(QUADSPI_TypeDef *pQspi)
{
    qspi_flash_cmd_t s_command;
    uint8_t reg = 0;

    /* Initialize the read flag status register command */
    s_command.InstructionModes = QSPI_INSTRUCTION_1_LINE;
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
    qspi_flash_cmd(pQspi, &s_command);

    /* Reception of the data */
    qspi_flash_receive(pQspi, &reg, 0);

    TRACE_INFO("reg = %x \n", reg);
    /* Check the value of the register */
    if ((reg & W25Q128J_SR2_SUS) != 0)
    {
        return 4;
    }
    else if ((reg & W25Q128J_SR_BUSY) != 0)
    {
        return 0;
    }
    else
    {
        return 2;
    }
}

static void qspi_flash_writeEnable(QUADSPI_TypeDef *pQspi)
{
    qspi_flash_cmd_t s_command;
    qspi_flash_autoPolling_t s_config;

    /* Enable write operations */
    s_command.InstructionModes = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = WRITE_ENABLE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    qspi_flash_cmd(pQspi, &s_command);

    /* Configure automatic polling mode to wait for write enabling */
    s_config.Match = W25Q128J_SR_WEL;
    s_config.Mask = W25Q128J_SR_WEL;

    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusByteSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = READ_STATUS_REG_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;

    qspi_flash_autoPolling(pQspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

static void qspi_flash_MemoryMapped(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, qspi_flash_memory_mapped_t *memo_cfg)
{
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
        // ERROR("error\n;");
    }
    
    REG_MODIFY(pQspi->CR, QUADSPI_CR_TCEN, memo_cfg->TimeOutActivations);

    if (memo_cfg->TimeOutActivations == QSPI_TIMEOUT_COUNTER_ENABLE)
    {
        REG_WRITE(pQspi->LPTR, memo_cfg->TimeoutPeriod);
        REG_WRITE(pQspi->FCR, QSPI_FLAG_TO);
        REG_SET_BIT(pQspi->CR, QUADSPI_CR_TOIE_Pos);
    }
    qspi_flash_cmd_cfg(pQspi, cmd, QUADSPI_FMODE_MEMO);
}

/******************************** End of file *********************************/


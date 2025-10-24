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
static void qspi_flash_cmd_send(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t cmd);
static void qspi_flash_cmd_cfg(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t *cmd, uint32_t mode);

/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

static void qspi_flash_init(qspi_flash_t *self)
{
    qspi_flash_setup_pin();
    qspi_flash_setup_drive(self->pQspi);
    qspi_flash_reset(self->pQspi);
}

static void qspi_flash_write(qspi_flash_t *self, uint32_t address, uint8_t data, uint16_t size)
{

}

static void qspi_flash_read(qspi_flash_t *self, uint32_t address, uint8_t data, uint16_t size)
{

}

static void qspi_flash_memo_mapped(qspi_flash_t *self)
{
    
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
    m_qspi_flash->memory_mapped = qspi_flash_memo_mapped;
    TRACE_INFO("completed \n");

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
    REG_SET_VAL(QUADSPI_CLK_port->MODER , 0x1U, 0x3, (QUADSPI_CLK_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_CLK_port->OTYPER, QUADSPI_CLK_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_CLK_port->OSPEEDR, 2U, 0x3U, (QUADSPI_CLK_pin * 2U));           /* Speed : high speed */    
    REG_SET_VAL(QUADSPI_CLK_port->AFR[0], 0xAU, 0xFU, (QUADSPI_CLK_pin * 4U));       
    /* QUADSPI_BK1_NCS pin */
    REG_SET_VAL(QUADSPI_BK1_NCS_port->MODER , 0x1U, 0x3, (QUADSPI_BK1_NCS_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_NCS_port->OTYPER, QUADSPI_BK1_NCS_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_BK1_NCS_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_NCS_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_NCS_port->AFR[0], 0x9U, 0xFU, (QUADSPI_BK1_NCS_pin * 4U));       
    /* QUADSPI_BK1_IO1 pin */
    REG_SET_VAL(QUADSPI_BK1_IO1_port->MODER , 0x1U, 0x3, (QUADSPI_BK1_IO1_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_IO1_port->OTYPER, QUADSPI_BK1_IO1_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_BK1_IO1_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_IO1_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_IO1_port->AFR[1], 0x9U, 0xFU, ((QUADSPI_BK1_IO1_pin%8) * 4U));
    /* QUADSPI_BK1_IO2 pin */
    REG_SET_VAL(QUADSPI_BK1_IO2_port->MODER , 0x1U, 0x3, (QUADSPI_BK1_IO2_pin * 2U)); 			/* Mode : Output */
	REG_CLR_BIT(QUADSPI_BK1_IO2_port->OTYPER, QUADSPI_BK1_IO2_pin); 							/* Output type : push-pull*/
	REG_SET_VAL(QUADSPI_BK1_IO2_port->OSPEEDR, 2U, 0x3U, (QUADSPI_BK1_IO2_pin * 2U));           /* Speed : high speed */   
    REG_SET_VAL(QUADSPI_BK1_IO2_port->AFR[0], 0x9U, 0xFU, (QUADSPI_BK1_IO2_pin * 4U)); 
    /* QUADSPI_BK1_IO3 pin */
    REG_SET_VAL(QUADSPI_BK1_IO3_port->MODER , 0x1U, 0x3, (QUADSPI_BK1_IO3_pin * 2U)); 			/* Mode : Output */
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
        while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_TCF_Pos) == 0)
        {
            /* code */
            // ERROR("error\n;");
        }
        REG_SET_BIT(pQspi->FCR, QUADSPI_FCR_CTCF_Pos);
        pQspi->FCR |= (1 << QUADSPI_FCR_CTCF_Pos);
    }
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
            ERROR("error\n;");
        }
        REG_WRITE(pQspi->DR, *tmp_data);
        tmp_data--;
        data_len--;
    }
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_TCF_Pos) != 1)
    {
        /* code */
        ERROR("error\n;");
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
            ERROR("error\n;");
        }
        REG_WRITE(pQspi->DR, *tmp_data);
        tmp_data--;
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
    // if ((cmd->DataMode != QUADSPI_DATA_MODE_NONE) && (mode != QUADSPI_FMODE_MEMO))
    // {
    //     /* Configure QSPI : DLR register with the number of data to read or write */
    //     REG_WRITE(pQspi->DLR, (cmd->NbData - 1U));
    // }
    
    // /* 2. Configure QSPI : CCR register with frame format, mode and instruction code */
    // /* 2.1 Instruction and instruction mode */
    // REG_SET_VAL(pQspi->CCR, cmd->Instruction, 0xFF, QUADSPI_CCR_INSTRUCTION_Pos);
    // REG_SET_VAL(pQspi->CCR, cmd->InstructionModes, 0x3U, QUADSPI_CCR_IMODE_Pos);
    // /* 2.2 Address mode and address size */
    // REG_SET_VAL(pQspi->CCR, cmd->AddressMode, 0x3U, QUADSPI_CCR_ADMODE_Pos);
    // if (cmd->AddressMode != QUADSPI_ADR_MODE_NONE)
    // {
    //     REG_SET_VAL(pQspi->CCR, cmd->AddressSize, 0x3U, QUADSPI_CCR_ADSIZE_Pos);
    //     if (mode != QUADSPI_FMODE_MEMO)
    //     {
    //         REG_WRITE(pQspi->AR, cmd->Address);
    //     }
    // }
    // /* 2.3 Alternate mode and Alternate size */
    // REG_SET_VAL(pQspi->CCR, cmd->AlternateByteMode, 0x3U, QUADSPI_CCR_ABMODE_Pos);
    // if (cmd->AlternateByteMode != QUADSPI_ALB_MODE_NONE)
    // {
    //     REG_SET_VAL(pQspi->CCR, cmd->AlternateBytesSize, 0x3U, QUADSPI_CCR_ABSIZE_Pos);
    //     REG_WRITE(pQspi->ABR, cmd->AlternateBytes);
    // }
    // /* 2.4 Data mode and data size */
    // REG_SET_VAL(pQspi->CCR, cmd->DataMode, 0x3U, QUADSPI_CCR_DMODE_Pos);
    // /* 2.5 Number of dummy cycles */
    // REG_SET_VAL(pQspi->CCR, cmd->DummyCycles, 0xFU, QUADSPI_CCR_DCYC_Pos);
    // /* 2.6  SIOO Sned instruction only once mode */
    // REG_SET_VAL(pQspi->CCR, cmd->SIOOMode, 0x1U, QUADSPI_CCR_SIOO_Pos);
    // /* 2.7  DDR hold */
    // REG_SET_VAL(pQspi->CCR, cmd->DdrHoldHalfCycle, 0x1U, QUADSPI_CCR_DHHC_Pos);
    // /* 2.8  Double data rate mode */
    // REG_SET_VAL(pQspi->CCR, cmd->DdrMode, 0x1U, QUADSPI_CCR_DDRM_Pos);
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
                                         cmd->Instruction | mode));

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
                                         cmd->Instruction | mode));
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
                                         cmd->InstructionModes | cmd->Instruction | mode));

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
                                         cmd->InstructionModes | cmd->Instruction | mode));
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
                                         cmd->InstructionModes | mode));

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
                                         cmd->AddressMode | cmd->InstructionModes | mode));
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
                                         cmd->AddressMode | cmd->InstructionModes | mode));

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
                                           cmd->InstructionModes | mode));
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
}

/******************************** End of file *********************************/


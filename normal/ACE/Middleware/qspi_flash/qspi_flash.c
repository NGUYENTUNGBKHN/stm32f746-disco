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
#include "qspi_flash.h"
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
static void qspi_flash_cmd_cfg(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t cmd);

/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

static void qspi_flash_init(qspi_flash_t *self)
{
    qspi_flash_setup_pin();
    qspi_flash_setup_drive(self->pQspi);
    // qspi_flash_reset(self->pQspi);
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
    TRACE_INFO("compeled \n");

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

static void qspi_flash_cmd_send(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t cmd)
{
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
    }


    qspi_flash_cmd_cfg(pQspi, cmd);

}

static void qspi_flash_cmd_cfg(QUADSPI_TypeDef *pQspi, qspi_flash_cmd_t cmd)
{
    /* 1. Configure QSPI : DLR register with the number of data to read or write */
    // REG_SET_VAL(pQspi->DLR, );
}

static void qspi_flash_reset(QUADSPI_TypeDef *pQspi)
{
    while (REG_READ_BIT(pQspi->SR, QUADSPI_SR_BUSY_Pos) != 0)
    {
        /* code */
    }
    
}

/******************************** End of file *********************************/


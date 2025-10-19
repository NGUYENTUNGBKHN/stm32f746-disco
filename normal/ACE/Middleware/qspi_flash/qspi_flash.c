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


/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/

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


    /* CLK pin */
    REG_SET_VAL(QUADSPI_CLK_port->MODER , 0x1U, 0x3, (QUADSPI_CLK_pin * 2U)); 			/*mode*/
	REG_CLR_BIT(QUADSPI_CLK_port->OTYPER, QUADSPI_CLK_pin); 							/*Output type*/
	REG_SET_VAL(QUADSPI_CLK_port->OSPEEDR, 2U, 0x3U, (QUADSPI_CLK_pin * 2U)); 
}

static void qspi_flash_setup_drive()
{

}

static void qspi_flash_init(qspi_flash_t *self)
{
   qspi_flash_setup_pin();
   qspi_flash_setup_drive();

}

static void qspi_flash_write(qspi_flash_t *self, uint32_t address, uint8_t data, uint16_t size)
{

}

static void qspi_flash_read(qspi_flash_t *self, uint32_t address, uint8_t data, uint16_t size)
{

}


qspi_flash_t *qspi_flash_create()
{
    m_qspi_flash = (qspi_flash_t*)malloc(sizeof(qspi_flash_t));

    if (NULL == m_qspi_flash)
    {
        return NULL;
    }

    m_qspi_flash->handle = QUADSPI;
    m_qspi_flash->init = qspi_flash_init;
    m_qspi_flash->read = qspi_flash_read;
    m_qspi_flash->write = qspi_flash_write;
    TRACE_INFO("compeled \n");

    return m_qspi_flash;
}



/******************************** End of file *********************************/


/******************************************************************************/
/*! @addtogroup Group2
    @file       main.c
    @brief      
    @date       2025/07/30
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
/*******************************************************************************
**                                INCLUDES
*******************************************************************************/
#include "main.h"
/*******************************************************************************
**                       INTERNAL MACRO DEFINITIONS
*******************************************************************************/
#define BUFFER_SIZE         ((uint32_t)0x0200)
#define WRITE_READ_ADDR     ((uint32_t)0x0050)
/*******************************************************************************
**                      COMMON VARIABLE DEFINITIONS
*******************************************************************************/

uint8_t qspi_aTxBuffer[BUFFER_SIZE];
uint8_t qspi_aRxBuffer[BUFFER_SIZE];
/*******************************************************************************
**                      INTERNAL VARIABLE DEFINITIONS
*******************************************************************************/
QSPI_HandleTypeDef QSPIHandle;
__IO uint8_t CmdCplt, RxCplt, TxCplt, StatusMatch;
QSPI_CommandTypeDef sCommand;
QSPI_MemoryMappedTypeDef sMemMappedCfg;
volatile static uint8_t count = 0;
/*******************************************************************************
**                      INTERNAL FUNCTION PROTOTYPES
*******************************************************************************/


/*******************************************************************************
**                          FUNCTION DEFINITIONS
*******************************************************************************/
static void SystemClock_Config();
static void Error_Handler();
static void MPU_Config();
static void CPU_CACHE_Enable();

/**
 * @brief  Fills buffer with user predefined data.
 * @param  pBuffer: pointer on the buffer to fill
 * @param  uwBufferLenght: size of the buffer to fill
 * @param  uwOffset: first value to fill on the buffer
 * @retval None
 */
static void Fill_Buffer(uint8_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
    uint32_t tmpIndex = 0;

    /* Put in global buffer different values */
    for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++)
    {
        pBuffer[tmpIndex] = tmpIndex + uwOffset;
    }
}

/**
 * @brief  Compares two buffers.
 * @param  pBuffer1, pBuffer2: buffers to be compared.
 * @param  BufferLength: buffer's length
 * @retval 1: pBuffer identical to pBuffer1
 *         0: pBuffer differs from pBuffer1
 */
static uint8_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint32_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return 1;
        }

        pBuffer1++;
        pBuffer2++;
    }

    return 0;
}

int main()
{
    /* MPU Configuration */
    MPU_Config();
    /* CPU cache Enable */
    CPU_CACHE_Enable();
    /* Hardware */
    HAL_Init();
    /* Clock configuration */
    SystemClock_Config();
    TRACE_INFO("Entered Application \n");
    #if 0
    w25q128j_t *test = W25Q128J_Create();
    test->init(test);

    test->erase_block(test, WRITE_READ_ADDR);

    Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, 0xD20F);
    test->write(test, qspi_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    test->read(test, qspi_aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    if (Buffercmp(qspi_aTxBuffer, qspi_aRxBuffer, BUFFER_SIZE) > 0)
    {
        TRACE_INFO("FAIL.\n");
    }
    else
    {
        TRACE_INFO("DONE.\n");
    }
    #else
    w25q128j_t *test = W25Q128J_Create();
    test->init(test);

    // test->erase_block(test, WRITE_READ_ADDR);

    // Fill_Buffer(qspi_aTxBuffer, BUFFER_SIZE, 0xD20F);
    // test->write(test, qspi_aTxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    // test->read(test, qspi_aRxBuffer, WRITE_READ_ADDR, BUFFER_SIZE);

    // if (Buffercmp(qspi_aTxBuffer, qspi_aRxBuffer, BUFFER_SIZE) > 0)
    // {
    //     TRACE_INFO("FAIL.\n");
    // }
    // else
    // {
    //     TRACE_INFO("DONE.\n");
    // }
    uint8_t res = test->mapped_memory(test);
    #endif 

    uint8_t byte0 = *(__IO uint8_t *)(ST_QSPI_MAPPING_ADDRESS);
    TRACE_INFO("byte0 = %d\n", byte0);


    while (1)
    {
        /* code */
        test_led();

    }
}

static void SystemClock_Config()
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;  
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* activate the OverDrive to reach the 216 Mhz Frequency */
    if(HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }
    
    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
        clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MPU_Config()
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = ST_MEMORY_START_ADDRESS;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.SubRegionDisable = 0x87;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /* Configure the MPU QSPI flash */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.BaseAddress = ST_QSPI_MAPPING_ADDRESS;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU QSPI control registers */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    MPU_InitStruct.BaseAddress = ST_QSPI_CTRL_RES_ADDRESS;
    MPU_InitStruct.Size = MPU_REGION_SIZE_8KB;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);


    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void CPU_CACHE_Enable()
{
    /* Enable I-Cache */
    SCB_EnableICache();

    /* Enable D-Cache */
    SCB_EnableDCache();
}


static void Error_Handler()
{
    while (1)
    {
        /* code */
    }
}

/******************************** End of file *********************************/

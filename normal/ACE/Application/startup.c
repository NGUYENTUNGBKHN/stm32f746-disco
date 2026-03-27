/******************************************************************************/
/*! @addtogroup Group2
    @file       startup.c
    @brief      
    @date       2025/07/30
    @author     Development Dept at Tokyo (nguyen-thanh-tung@jcm-hq.co.jp)
    @par        Revision
    $Id$
    @par        Copyright (C)
    Japan CashMachine Co, Limited. All rights reserved.
******************************************************************************/
#include <stdint.h>
#include "system_stm32f7xx.h"
#include "stm32f7xx.h"

extern uint32_t _estack;
extern int main();
void SystemInit_ExtMemCtl(void);

extern uint32_t _sidata;

extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;


void Reset_Handler()
{
    volatile uint32_t *start = &_sdata;
    volatile uint32_t *end = &_edata;
    volatile uint32_t *data = &_sidata;
    volatile uint32_t *bss_start = &_sbss;
    volatile uint32_t *bss_end = &_ebss;
    SystemInit();
    // SystemInit_ExtMemCtl();
    while (start < end)
    {
        /* code */
        *start++ = *data++;
    }
   
    /* setup bss data = 0 */
    while (bss_start < bss_end)
    {
        /* code */
        *bss_start++ = 0;
    }


    /* Jump to __libc_init_array */
    // __libc_init_array();
    
    main();
}

/**
 * @brief  Setup the external memory controller.
 *         Called in startup_stm32f7xx.s before jump to main.
 *         This function configures the external memories (SDRAM)
 *         This SDRAM will be used as program data memory (including heap and stack).
 * @param  None
 * @retval None
 */
void SystemInit_ExtMemCtl(void)
{
	register uint32_t tmpreg = 0, timeout = 0xFFFF;
	register __IO uint32_t index;

	/* Enable GPIOC, GPIOD, GPIOE, GPIOF, GPIOG and GPIOH interface
	clock */
	RCC->AHB1ENR |= 0x000000FC;
    tmpreg = RCC->AHB1ENR;
    (void)tmpreg;

	/* Connect PCx pins to FMC Alternate function */
	GPIOC->AFR[0] = 0x0000C000;
	GPIOC->AFR[1] = 0x00000000;
	/* Configure PCx pins in Alternate function mode */
	GPIOC->MODER = 0x00000080;
	/* Configure PCx pins speed to 50 MHz */
	GPIOC->OSPEEDR = 0x00000080;
	/* Configure PCx pins Output type to push-pull */
	GPIOC->OTYPER = 0x00000000;
	/* No pull-up, pull-down for PCx pins */
	GPIOC->PUPDR = 0x00000040;

	/* Connect PDx pins to FMC Alternate function */
	GPIOD->AFR[0] = 0x000000CC;
	GPIOD->AFR[1] = 0xCC000CCC;
	/* Configure PDx pins in Alternate function mode */
	GPIOD->MODER = 0xA02A000A;
	/* Configure PDx pins speed to 50 MHz */
	GPIOD->OSPEEDR = 0xA02A000A;
	/* Configure PDx pins Output type to push-pull */
	GPIOD->OTYPER = 0x00000000;
	/* No pull-up, pull-down for PDx pins */
	GPIOD->PUPDR = 0x50150005;

	/* Connect PEx pins to FMC Alternate function */
	GPIOE->AFR[0] = 0xC00000CC;
	GPIOE->AFR[1] = 0xCCCCCCCC;
	/* Configure PEx pins in Alternate function mode */
	GPIOE->MODER = 0xAAAA800A;
	/* Configure PEx pins speed to 50 MHz */
	GPIOE->OSPEEDR = 0xAAAA800A;
	/* Configure PEx pins Output type to push-pull */
	GPIOE->OTYPER = 0x00000000;
	/* No pull-up, pull-down for PEx pins */
	GPIOE->PUPDR = 0x55554005;

	/* Connect PFx pins to FMC Alternate function */
	GPIOF->AFR[0] = 0x00CCCCCC;
	GPIOF->AFR[1] = 0xCCCCC000;
	/* Configure PFx pins in Alternate function mode */
	GPIOF->MODER = 0xAA800AAA;
	/* Configure PFx pins speed to 50 MHz */
	GPIOF->OSPEEDR = 0xAA800AAA;
	/* Configure PFx pins Output type to push-pull */
	GPIOF->OTYPER = 0x00000000;
	/* No pull-up, pull-down for PFx pins */
	GPIOF->PUPDR = 0x55400555;

	/* Connect PGx pins to FMC Alternate function */
	GPIOG->AFR[0] = 0x00CC00CC;
	GPIOG->AFR[1] = 0xC000000C;
	/* Configure PGx pins in Alternate function mode */
	GPIOG->MODER = 0x80020A0A;
	/* Configure PGx pins speed to 50 MHz */
	GPIOG->OSPEEDR = 0x80020A0A;
	/* Configure PGx pins Output type to push-pull */
	GPIOG->OTYPER = 0x00000000;
	/* No pull-up, pull-down for PGx pins */
	GPIOG->PUPDR = 0x40010505;

	/* Connect PHx pins to FMC Alternate function */
	GPIOH->AFR[0] = 0x00C0C000;
	GPIOH->AFR[1] = 0x00000000;
	/* Configure PHx pins in Alternate function mode */
	GPIOH->MODER = 0x00000880;
	/* Configure PHx pins speed to 50 MHz */
	GPIOH->OSPEEDR = 0x00000880;
	/* Configure PHx pins Output type to push-pull */
	GPIOH->OTYPER = 0x00000000;
	/* No pull-up, pull-down for PHx pins */
	GPIOH->PUPDR = 0x00000440;

	/* Enable the FMC interface clock */
	RCC->AHB3ENR |= 0x00000001;
    tmpreg = RCC->AHB1ENR;
    (void)tmpreg;

	/* Configure and enable SDRAM bank1 */
    /* NC[1:0]   : 8bits  - Number of column address bits. 
       NR[1:0]   : 12bits - Number of row address bits.
       MWID[1:0] : 16bits - Memory data bus width.
       NB        : 1. Four internal bank.
       CAS[1:0]  : 2 cycles. 
       WP        : Write accesses allowed.
       SDCLK[1:0]: SDCLK period = 2 x HCLK periods.
       RBURST    : single read requests are always managed as bursts. */
	FMC_Bank5_6->SDCR[0] = 0x00001954;
    
	FMC_Bank5_6->SDTR[0] = 0x01115351;

	/* SDRAM initialization sequence */
	/* Clock enable command */
	FMC_Bank5_6->SDCMR = 0x00000011;
	tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
	while ((tmpreg != 0) && (timeout-- > 0))
	{
		tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
	}

	/* Delay */
	for (index = 0; index < 1000; index++)
		;

	/* PALL command */
	FMC_Bank5_6->SDCMR = 0x00000012;
	timeout = 0xFFFF;
	while ((tmpreg != 0) && (timeout-- > 0))
	{
		tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
	}

	/* Auto refresh command */
	FMC_Bank5_6->SDCMR = 0x000000F3;
	timeout = 0xFFFF;
	while ((tmpreg != 0) && (timeout-- > 0))
	{
		tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
	}

	/* MRD register program */
	FMC_Bank5_6->SDCMR = 0x00044014;
	timeout = 0xFFFF;
	while ((tmpreg != 0) && (timeout-- > 0))
	{
		tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
	}

	/* Set refresh count */
	tmpreg = FMC_Bank5_6->SDRTR;
	FMC_Bank5_6->SDRTR = (tmpreg | (0x0000050C << 1));

	/* Disable write protection */
	tmpreg = FMC_Bank5_6->SDCR[0];
	FMC_Bank5_6->SDCR[0] = (tmpreg & 0xFFFFFDFF);

	/*
	 * Disable the FMC bank1 (enabled after reset).
	 * This, prevents CPU speculation access on this bank which blocks the use of FMC during
	 * 24us. During this time the others FMC master (such as LTDC) cannot use it!
	 */
	FMC_Bank1->BTCR[0] = 0x000030d2;
}

void default_handler(void)
{
    while (1)
    {
        /* code */
    }
    
}

void NMI_IRQHandler(void) __attribute((weak, alias("default_handler")));
void HardFault_IRQHandler(void) __attribute((weak, alias("default_handler")));
void MemManage_IRQHandler(void) __attribute((weak, alias("default_handler")));
void BusFault_IRQHandler(void) __attribute((weak, alias("default_handler")));
void UsageFault_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SVC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DebugMonitor_IRQHandler(void) __attribute((weak, alias("default_handler")));
void PendSV_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SysTick_IRQHandler(void) __attribute((weak, alias("default_handler")));
void WWDG_IRQHandler(void) __attribute((weak, alias("default_handler")));
void PVD_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TAMP_STTAMP_IRQHandler(void) __attribute((weak, alias("default_handler")));
void RTC_WKUP_IRQHandler(void) __attribute((weak, alias("default_handler")));
void FLASH_IRQHandler(void) __attribute((weak, alias("default_handler")));
void RCC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void EXTI0_IRQHandler(void) __attribute((weak, alias("default_handler")));
void EXTI1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void EXTI2_IRQHandler(void) __attribute((weak, alias("default_handler")));
void EXTI3_IRQHandler(void) __attribute((weak, alias("default_handler")));
void EXTI4_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream0_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream2_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream3_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream4_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream5_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream6_IRQHandler(void) __attribute((weak, alias("default_handler")));
void ADC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN1_TX_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN1_RX0_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN1_RX1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN1_SCE_IRQHandler(void) __attribute((weak, alias("default_handler")));
void EXTI9_5_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM1_BRK_TIM9_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM1_UP_TIM10_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM1_CC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM2_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM3_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM4_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C1_EV_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C1_ER_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C2_EV_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C2_ER_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SPI1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SPI2_IRQHandler(void) __attribute((weak, alias("default_handler")));
void USART1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void USART2_IRQHandler(void) __attribute((weak, alias("default_handler")));
void USART3_IRQHandler(void) __attribute((weak, alias("default_handler")));
void EXTI15_10_IRQHandler(void) __attribute((weak, alias("default_handler")));
void RTC_ALARM_IRQHandler(void) __attribute((weak, alias("default_handler")));
void OTG_FS_WKUP_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM8_BRK_TIM12_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM8_UP_TIM13_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM8_TRG_COM_TIM14_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM8_CC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA1_Stream7_IRQHandler(void) __attribute((weak, alias("default_handler")));
void FMC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SDMMC1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM5_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SPI3_IRQHandler(void) __attribute((weak, alias("default_handler")));
void UART4_IRQHandler(void) __attribute((weak, alias("default_handler")));
void UART5_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM6_DAC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void TIM7_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream0_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream2_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream3_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream4_IRQHandler(void) __attribute((weak, alias("default_handler")));
void ETH_IRQHandler(void) __attribute((weak, alias("default_handler")));
void ETH_WKUP_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN2_TX_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN2_RX0_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN2_RX1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CAN2_SCE_IRQHandler(void) __attribute((weak, alias("default_handler")));
void OTG_FS_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream5_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream6_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2_Stream7_IRQHandler(void) __attribute((weak, alias("default_handler")));
void USART6_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C3_EV_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C3_ER_IRQHandler(void) __attribute((weak, alias("default_handler")));
void OTG_HS_EP1_OUT_IRQHandler(void) __attribute((weak, alias("default_handler")));
void OTG_HS_EP1_IN_IRQHandler(void) __attribute((weak, alias("default_handler")));
void OTG_HS_WKUP_IRQHandler(void) __attribute((weak, alias("default_handler")));
void OTG_HS_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DCMI_IRQHandler(void) __attribute((weak, alias("default_handler")));
void CRYP_IRQHandler(void) __attribute((weak, alias("default_handler")));
void HASH_RNG_IRQHandler(void) __attribute((weak, alias("default_handler")));
void FPU_IRQHandler(void) __attribute((weak, alias("default_handler")));
void UART7_IRQHandler(void) __attribute((weak, alias("default_handler")));
void UART8_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SPI4_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SPI5_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SPI6_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SAI1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void LCD_TFT_IRQHandler(void) __attribute((weak, alias("default_handler")));
void LTDC_ER_IRQHandler(void) __attribute((weak, alias("default_handler")));
void DMA2D_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SAI2_IRQHandler(void) __attribute((weak, alias("default_handler")));
void QuadSPI_IRQHandler(void) __attribute((weak, alias("default_handler")));
void LPTimer1_IRQHandler(void) __attribute((weak, alias("default_handler")));
void HDMI_CEC_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C4_EV_IRQHandler(void) __attribute((weak, alias("default_handler")));
void I2C4_ER_IRQHandler(void) __attribute((weak, alias("default_handler")));
void SPDIFRX_IRQHandler(void) __attribute((weak, alias("default_handler")));


__attribute((section(".isr_vector")))
uint32_t *_isr_vector[] = {
    (uint32_t*) &_estack,               /* top stack pointer */
    (uint32_t*) Reset_Handler,          /* Reset handler  */
    (uint32_t*) NMI_IRQHandler,
    (uint32_t*) HardFault_IRQHandler,
    (uint32_t*) MemManage_IRQHandler,
    (uint32_t*) BusFault_IRQHandler,
    (uint32_t*) UsageFault_IRQHandler,
    0,
    0,
    0,
    0,
    (uint32_t*) SVC_IRQHandler,
    (uint32_t*) DebugMonitor_IRQHandler,
    (uint32_t*) 0,
    (uint32_t*) PendSV_IRQHandler,
    (uint32_t*) SysTick_IRQHandler,
    (uint32_t*) WWDG_IRQHandler,
    (uint32_t*) PVD_IRQHandler,
    (uint32_t*) TAMP_STTAMP_IRQHandler,
    (uint32_t*) RTC_WKUP_IRQHandler,
    (uint32_t*) FLASH_IRQHandler,
    (uint32_t*) RCC_IRQHandler,
    (uint32_t*) EXTI0_IRQHandler,
    (uint32_t*) EXTI1_IRQHandler,
    (uint32_t*) EXTI2_IRQHandler,
    (uint32_t*) EXTI3_IRQHandler,
    (uint32_t*) EXTI4_IRQHandler,
    (uint32_t*) DMA1_Stream0_IRQHandler,
    (uint32_t*) DMA1_Stream1_IRQHandler,
    (uint32_t*) DMA1_Stream2_IRQHandler,
    (uint32_t*) DMA1_Stream3_IRQHandler,
    (uint32_t*) DMA1_Stream4_IRQHandler,
    (uint32_t*) DMA1_Stream5_IRQHandler,
    (uint32_t*) DMA1_Stream6_IRQHandler,
    (uint32_t*) ADC_IRQHandler,
    (uint32_t*) CAN1_TX_IRQHandler,
    (uint32_t*) CAN1_RX0_IRQHandler,
    (uint32_t*) CAN1_RX1_IRQHandler,
    (uint32_t*) CAN1_SCE_IRQHandler,
    (uint32_t*) EXTI9_5_IRQHandler,
    (uint32_t*) TIM1_BRK_TIM9_IRQHandler,
    (uint32_t*) TIM1_UP_TIM10_IRQHandler,
    (uint32_t*) TIM1_TRG_COM_TIM11_IRQHandler,
    (uint32_t*) TIM1_CC_IRQHandler,
    (uint32_t*) TIM2_IRQHandler,
    (uint32_t*) TIM3_IRQHandler,
    (uint32_t*) TIM4_IRQHandler,
    (uint32_t*) I2C1_EV_IRQHandler,
    (uint32_t*) I2C1_ER_IRQHandler,
    (uint32_t*) I2C2_EV_IRQHandler,
    (uint32_t*) I2C2_ER_IRQHandler,
    (uint32_t*) SPI1_IRQHandler,
    (uint32_t*) SPI2_IRQHandler,
    (uint32_t*) USART1_IRQHandler,
    (uint32_t*) USART2_IRQHandler,
    (uint32_t*) USART3_IRQHandler,
    (uint32_t*) EXTI15_10_IRQHandler,
    (uint32_t*) RTC_ALARM_IRQHandler,
    (uint32_t*) OTG_FS_WKUP_IRQHandler,
    (uint32_t*) TIM8_BRK_TIM12_IRQHandler,
    (uint32_t*) TIM8_UP_TIM13_IRQHandler,
    (uint32_t*) TIM8_TRG_COM_TIM14_IRQHandler,
    (uint32_t*) TIM8_CC_IRQHandler,
    (uint32_t*) DMA1_Stream7_IRQHandler,
    (uint32_t*) FMC_IRQHandler,
    (uint32_t*) SDMMC1_IRQHandler,
    (uint32_t*) TIM5_IRQHandler,
    (uint32_t*) SPI3_IRQHandler,
    (uint32_t*) UART4_IRQHandler,
    (uint32_t*) UART5_IRQHandler,
    (uint32_t*) TIM6_DAC_IRQHandler,
    (uint32_t*) TIM7_IRQHandler,
    (uint32_t*) DMA2_Stream0_IRQHandler,
    (uint32_t*) DMA2_Stream1_IRQHandler,
    (uint32_t*) DMA2_Stream2_IRQHandler,
    (uint32_t*) DMA2_Stream3_IRQHandler,
    (uint32_t*) DMA2_Stream4_IRQHandler,
    (uint32_t*) ETH_IRQHandler,
    (uint32_t*) ETH_WKUP_IRQHandler,
    (uint32_t*) CAN2_TX_IRQHandler,
    (uint32_t*) CAN2_RX0_IRQHandler,
    (uint32_t*) CAN2_RX1_IRQHandler,
    (uint32_t*) CAN2_SCE_IRQHandler,
    (uint32_t*) OTG_FS_IRQHandler,
    (uint32_t*) DMA2_Stream5_IRQHandler,
    (uint32_t*) DMA2_Stream6_IRQHandler,
    (uint32_t*) DMA2_Stream7_IRQHandler,
    (uint32_t*) USART6_IRQHandler,
    (uint32_t*) I2C3_EV_IRQHandler,
    (uint32_t*) I2C3_ER_IRQHandler,
    (uint32_t*) OTG_HS_EP1_OUT_IRQHandler,
    (uint32_t*) OTG_HS_EP1_IN_IRQHandler,
    (uint32_t*) OTG_HS_WKUP_IRQHandler,
    (uint32_t*) OTG_HS_IRQHandler,
    (uint32_t*) DCMI_IRQHandler,
    (uint32_t*) CRYP_IRQHandler,
    (uint32_t*) HASH_RNG_IRQHandler,
    (uint32_t*) FPU_IRQHandler,
    (uint32_t*) UART7_IRQHandler,
    (uint32_t*) UART8_IRQHandler,
    (uint32_t*) SPI4_IRQHandler,
    (uint32_t*) SPI5_IRQHandler,
    (uint32_t*) SPI6_IRQHandler,
    (uint32_t*) SAI1_IRQHandler,
    (uint32_t*) LCD_TFT_IRQHandler,
    (uint32_t*) LTDC_ER_IRQHandler,
    (uint32_t*) DMA2D_IRQHandler,
    (uint32_t*) SAI2_IRQHandler,
    (uint32_t*) QuadSPI_IRQHandler,
    (uint32_t*) LPTimer1_IRQHandler,
    (uint32_t*) HDMI_CEC_IRQHandler,
    (uint32_t*) I2C4_EV_IRQHandler,
    (uint32_t*) I2C4_ER_IRQHandler,
    (uint32_t*) SPDIFRX_IRQHandler,
};


/**
 * @file       debug_uart.c
 * @brief      Simple debug UART implementation (USART1, 115200 8N1)
 *
 * Hardware mapping (STM32F746-DISCO):
 *   USART1_TX  ->  PA9   (AF7)
 *
 * Clock assumptions (set by SystemClock_Config):
 *   SYSCLK = 216 MHz
 *   APB2   = SYSCLK / 2 = 108 MHz  -> USART1 clock = 108 MHz
 *
 * BRR for 115200 baud:
 *   BRR = 108 000 000 / 115200 = 937.5  -> 938 (0x3AA)
 */

#include "debug_uart.h"


#define UART_6_ENABLE
/* ------------------------------------------------------------------ */
/* Bit helpers                                                          */
/* ------------------------------------------------------------------ */
// #define USART_CR1_UE_Pos    0U
// #define USART_CR1_TE_Pos    3U

/* ------------------------------------------------------------------ */

void debug_uart_init(void)
{
    /* 1. Enable peripheral clocks ----------------------------------- */

    #ifdef UART_6_ENABLE
    /* GPIOC clock (AHB1) */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    #else
    /* GPIOA clock (AHB1) */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    #endif 

    #ifdef UART_6_ENABLE
    /* USART6 clock (APB2) */
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
    #else
    /* USART1 clock (APB2) */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    #endif 

    /* Small delay to let clocks stabilize */
    volatile uint32_t tmpreg = RCC->APB2ENR;
    (void)tmpreg;

    #ifdef UART_6_ENABLE
    /* 2. Configure PC6 as Alternate Function (AF8 = USART6_TX) ------ */
    /* Mode: Alternate Function (10b) for pin 6 */
    GPIOC->MODER &= ~(0x3U << (6U * 2U));
    GPIOC->MODER |=  (0x2U << (6U * 2U));

    /* Output speed: High */
    GPIOC->OSPEEDR |= (0x3U << (6U * 2U));  
    
    /* No pull-up / pull-down */
    GPIOC->PUPDR &= ~(0x3U << (6U * 2U));

    /* Alternate function 8 for PC6 (AFRL, pin 6 -> index 6 in AFR[0]) */
    GPIOC->AFR[0] &= ~(0xFU << (6U * 4U));
    GPIOC->AFR[0] |=  (0x8U << (6U * 4U));   /* AF8 */

    /* 3. Configure USART6 ------------------------------------------- */
    /* Disable USART before configuration */
    USART6->CR1 = 0U;
    /* BRR = fPCLK2 / baud = 108 000 000 / 115200 = 938 (0x3AA) */
    USART6->BRR = 938U;
    /* CR2: default (1 stop bit) */
    USART6->CR2 = 0U;
    /* CR3: default (no flow control) */
    USART6->CR3 = 0U;
    /* Enable TX + USART */
    USART6->CR1 = (1U << USART_CR1_TE_Pos)   /*
                | (1U << USART_CR1_RE_Pos)   // Enable RX if needed
                */
                | (1U << USART_CR1_UE_Pos);   /* UE */
    #else
    /* 2. Configure PA9 as Alternate Function (AF7 = USART1_TX) ------ */
    /* Mode: Alternate Function (10b) for pin 9 */
    GPIOA->MODER &= ~(0x3U << (9U * 2U));
    GPIOA->MODER |=  (0x2U << (9U * 2U));

    /* Output speed: High */
    GPIOA->OSPEEDR |= (0x3U << (9U * 2U));

    /* No pull-up / pull-down */
    GPIOA->PUPDR &= ~(0x3U << (9U * 2U));

    /* Alternate function 7 for PA9 (AFRH, pin 9 -> index 1 in AFR[1]) */
    GPIOA->AFR[1] &= ~(0xFU << ((9U - 8U) * 4U));
    GPIOA->AFR[1] |=  (0x7U << ((9U - 8U) * 4U));   /* AF7 */

    /* 3. Configure USART1 ------------------------------------------- */
    /* Disable USART before configuration */
    USART1->CR1 = 0U;

    /* BRR = fPCLK2 / baud = 108 000 000 / 115200 = 938 (0x3AA) */
    USART1->BRR = 938U;

    /* CR2: default (1 stop bit) */
    USART1->CR2 = 0U;

    /* CR3: default (no flow control) */
    USART1->CR3 = 0U;

    /* Enable TX + USART */
    USART1->CR1 = (1U << USART_CR1_TE_Pos)   /* TE */
                | (1U << USART_CR1_UE_Pos);   /* UE */
    #endif
}

/* ------------------------------------------------------------------ */

void debug_uart_putchar(char c)
{
    #ifdef UART_6_ENABLE
    /* Wait until TX data register is empty (TXE flag) */
    while (!(USART6->ISR & USART_ISR_TXE))
    {
        /* busy wait */
    }
    USART6->TDR = (uint8_t)c;
    #else
    /* Wait until TX data register is empty (TXE flag) */
    while (!(USART1->ISR & USART_ISR_TXE))
    {
        /* busy wait */
    }
    USART1->TDR = (uint8_t)c;
    #endif
}

/**
 * @file       debug_uart.h
 * @brief      Simple debug UART (USART1, PA9 TX, 115200 8N1)
 * @details    Redirect printf to USART1.
 *             Direct register access, no HAL dependency.
 */

#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "../SDK/Device/Include/stm32f746xx.h"

/**
 * @brief  Initialize USART1 for debug output.
 *         PA9 -> USART1_TX (AF7)
 *         Baud : 115200 @ 216 MHz APB2 (108 MHz)
 *         Frame: 8 data bits, no parity, 1 stop bit
 *         TX only, polling mode.
 */
void debug_uart_init(void);

/**
 * @brief  Transmit a single character (blocking).
 * @param  c  character to send
 */
void debug_uart_putchar(char c);

#endif /* DEBUG_UART_H_ */

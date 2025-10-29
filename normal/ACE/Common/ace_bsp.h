/**
 * @file       ace_bsp.h
 * @brief      
 * @date       2025/10/28
 * @author     [Gentantun] (nguyenthanhtung8196@gmail.com)
 * @details    
 * @ref        
 * @copyright  Copyright (c) 2025 RoboTun
*/
#ifndef _ACE_BSP_H_
#define _ACE_BSP_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* CODE */

/* General Define */
#define MCU_GPIO_PIN_0					0U
#define MCU_GPIO_PIN_1					1U
#define MCU_GPIO_PIN_2					2U
#define MCU_GPIO_PIN_3					3U
#define MCU_GPIO_PIN_4					4U
#define MCU_GPIO_PIN_5					5U
#define MCU_GPIO_PIN_6					6U
#define MCU_GPIO_PIN_7					7U
#define MCU_GPIO_PIN_8					8U
#define MCU_GPIO_PIN_9					9U
#define MCU_GPIO_PIN_10					10U
#define MCU_GPIO_PIN_11					11U
#define MCU_GPIO_PIN_12					12U
#define MCU_GPIO_PIN_13					13U
#define MCU_GPIO_PIN_14					14U
#define MCU_GPIO_PIN_15					15U

/* W25Q128J */
/* Configure Clock Resource */
#define W25Q128J_CLK_ENABLE()       __HAL_RCC_QSPI_CLK_ENABLE()
#define W25Q128J_CLK_DISABLE()      __HAL_RCC_QSPI_CLK_DISABLE()
#define W25Q128J_GPIO_CLK_ENABLE    __HAL_RCC_GPIOB_CLK_ENABLE();\
                                    __HAL_RCC_GPIOD_CLK_ENABLE();\
                                    __HAL_RCC_GPIOE_CLK_ENABLE();\

/* Configure GPIO Pin */
/* CLK GPIO-B-2 */
#define W25Q128J_GPIO_CLK_PORT      GPIOB
#define W25Q128J_GPIO_CLK_PIN       GPIO_PIN_2
#define W25Q128J_GPIO_CLK_PIN_pos   MCU_GPIO_PIN_2
/* CS GPIO-B-6 */
#define W25Q128J_GPIO_CS_PORT       GPIOB
#define W25Q128J_GPIO_CS_PIN        GPIO_PIN_6
#define W25Q128J_GPIO_CS_PIN_pos    MCU_GPIO_PIN_6
/* BK1-D0 GPIO-D-11 */
#define W25Q128J_GPIO_D0_PORT       GPIOD
#define W25Q128J_GPIO_D0_PIN        GPIO_PIN_11
#define W25Q128J_GPIO_D0_PIN_pos    MCU_GPIO_PIN_11
/* BK1-D1 GPIO-D-12 */
#define W25Q128J_GPIO_D1_PORT       GPIOD
#define W25Q128J_GPIO_D1_PIN        GPIO_PIN_12
#define W25Q128J_GPIO_D1_PIN_pos    MCU_GPIO_PIN_12
/* BK1-D2 GPIO-E-2 */
#define W25Q128J_GPIO_D2_PORT       GPIOE
#define W25Q128J_GPIO_D2_PIN        GPIO_PIN_2
#define W25Q128J_GPIO_D2_PIN_pos    MCU_GPIO_PIN_2
/* BK1-D3 GPIO-D-13 */
#define W25Q128J_GPIO_D3_PORT       GPIOD
#define W25Q128J_GPIO_D3_PIN        GPIO_PIN_13
#define W25Q128J_GPIO_D3_PIN_pos    MCU_GPIO_PIN_13


#ifdef __cplusplus
}
#endif
#endif
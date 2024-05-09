/**
  ******************************************************************************
  * @file       msp430_hal_gpio.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions prototypes for the HAL
  *             GPIO module driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 Universidad Panamericana.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DRIVERS_MSP430_HAL_GPIO_H_
#define DRIVERS_MSP430_HAL_GPIO_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430x2xx.h>

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    __IO uint8_t PIN;
    __IO uint8_t POUT;
    __IO uint8_t PDIR;
    __IO uint8_t PIFG;
    __IO uint8_t PIES;
    __IO uint8_t PIE;
    __IO uint8_t PSEL;
    __IO uint8_t PREN;
} GPIO_TypeDef_t;

typedef struct
{
    uint8_t Pin;
    uint8_t Mode;
} GPIO_InitTypeDef;

typedef __IO uint8_t PSEL2_TypeDef_t;

/* Exported constants --------------------------------------------------------*/
// GPIO PIN DEF
#define GPIO_PIN_0      BIT0
#define GPIO_PIN_1      BIT1
#define GPIO_PIN_2      BIT2
#define GPIO_PIN_3      BIT3
#define GPIO_PIN_4      BIT4
#define GPIO_PIN_5      BIT5
#define GPIO_PIN_6      BIT6
#define GPIO_PIN_7      BIT7
#define GPIO_PIN_ALL    (0xFF)

// GPIO Mode
#define GPIO_MODE_INPUT     (0x00)
#define GPIO_MODE_OUTPUT    (0x01)
#define GPIO_MODE_ALTFUN_0  (0x02)
#define GPIO_MODE_ALTFUN_1  (0x03)
#define GPIO_MODE_ALTFUN_2  (0x04)
#define GPIO_MODE_ALTFUN_3  (0x05)
#define GPIO_MODE_ANALOG    (0x06)
#define GPIO_PULL_NONE      (0x00)
#define GPIO_PULL_DOWN      (BIT3)
#define GPIO_PULL_UP        (BIT4)

// IO State
#define GPIO_PIN_LOW     (0x00)
#define GPIO_PIN_HIGH    (0x01)

/* Exported macro ------------------------------------------------------------*/
#define HAL_GPIO_WritePin(GPIOx, PinMask, PinState)   GPIOx->POUT = ((PinState) ? GPIOx->POUT | (PinMask) : GPIOx->POUT & ~(PinMask))
#define HAL_GPIO_ReadPin(GPIOx, PinMask)              (GPIOx->PIN & (PinMask))
#define HAL_GPIO_TogglePin(GPIOx, PinMask)            GPIOx->POUT ^= (PinMask)

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/*!
 * @name BSP_IO_Init
 * @brief Configure MSP430-EXP430G2 I/Os
 * @return None
 */
void HAL_GPIO_Reset(void);
void HAL_GPIO_Init(GPIO_TypeDef_t *GPIOx, const GPIO_InitTypeDef *GPIO_Init);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_MSP430_HAL_GPIO_H_ */

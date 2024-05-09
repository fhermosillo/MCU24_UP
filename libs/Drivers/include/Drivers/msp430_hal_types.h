/**
  ******************************************************************************
  * @file       msp430_hal_types.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the basic datatypes for the MSP430 HAL.
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
#ifndef DRIVERS_MSP430_HAL_TYPES_H_
#define DRIVERS_MSP430_HAL_TYPES_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

/* Exported types ------------------------------------------------------------*/
typedef uint16_t HAL_BaseTypeDef;


/* Exported constants --------------------------------------------------------*/
#define HAL_UNLOCKED    0x00
#define HAL_LOCKED      0x01
#define HAL_OK          0x02
#define HAL_BUSY        0x03
#define HAL_ERROR       0x04

#define __I     volatile const
#define __O     volatile
#define __IO    volatile

#define __weak  __attribute__((weak))

/* Exported macro ------------------------------------------------------------*/
#define assert_param(param)                 if(!(param)) return;
#define assert_param_ret(param,xReturn)     if(!(param)) return (xReturn);

#define __HAL_LOCK(__HANDLE__)      uint16_t __InterruptStatus = __get_SR_register() & 0x0008; \
                                    __disable_interrupt();                  \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)    \
                                    {                                       \
                                        __bis_SR_register(__InterruptStatus);\
                                       return HAL_BUSY;                     \
                                    }                                       \
                                    else                                    \
                                    {                                       \
                                       (__HANDLE__)->Lock = HAL_LOCKED;     \
                                    }

#define __HAL_UNLOCK(__HANDLE__)    (__HANDLE__)->Lock = HAL_UNLOCKED;    \
                                    __bis_SR_register(__InterruptStatus)

#define UNUSED(VAR) (void)(VAR)

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_MSP430_HAL_TYPES_H_ */


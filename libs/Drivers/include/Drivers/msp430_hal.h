/**
  ******************************************************************************
  * @file       msp430_hal.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions prototypes for the HAL
  *             module driver.
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
#ifndef DRIVERS_MSP430_HAL_H_
#define DRIVERS_MSP430_HAL_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430x2xx.h>
#include <Drivers/msp430_hal_gpio.h>
#include <Drivers/msp430_hal_bcm.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define configCPU_CLOCK_HZ  (1000000UL)

/* Exported macro ------------------------------------------------------------*/
#define HAL_Delay_Micros(xTimeInUs) __delay_cycles((configCPU_CLOCK_HZ/1000000UL)*(xTimeInUs));
#define HAL_Delay_Millis(xTimeInMs) __delay_cycles((configCPU_CLOCK_HZ/1000UL)*(xTimeInMs));

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

#endif /* DRIVERS_MSP430_HAL_H_ */

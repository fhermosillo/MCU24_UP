/**
  ******************************************************************************
  * @file       msp430x2xx_hal_bcm.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the peripherals structures for the HAL
  *             Basic Clock Module Plus (BCM+) module driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 Universidad Panamericana.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DRIVERS_MSP430_HAL_BCM_H_
#define DRIVERS_MSP430_HAL_BCM_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430x2xx.h>

/** @addtogroup MSP430X2XX_HAL_Driver
 * @{
 */

/** @addtogroup BCM
 * @{
 */

/* Exported types ------------------------------------------------------------*/

/** @defgroup BCM_Exported_Types BCM Exported Types
 * @brief    BCM Exported Types
 * @{
 */


/**
 * @}
 */

/* Exported constants --------------------------------------------------------*/
#define BCM_DCOCLK_HZ_DEFAULT   (1100000UL)
#define BCM_MCLK_HZ_DEFAULT     BCM_DCOCLK_HZ_DEFAULT
#define BCM_SMCLK_HZ_DEFAULT    BCM_DCOCLK_HZ_DEFAULT
#define BCM_ACLK_HZ_DEFAULT     (32736UL)
#define BCM_VLOCLK_HZ           (12000UL)
#define BCM_CLK_HZ_ERR          (0UL)
#define BCM_CLK_HZ_USER         (1UL)

#define BCM_MCLK            (0)
#define BCM_SMCLK           (1)
#define BCM_ACLK            (2)
#define BCM_CLK_MAX         (3)
#define BCM_CLKSRC_DCO      (0)
#define BCM_CLKSRC_LFXT1    (1)
#define BCM_CLKSRC_VLO      (2)
#define BCM_CLKSRC_MAX      (3)
#define BCM_CLKDIV_1        (0)
#define BCM_CLKDIV_2        (1)
#define BCM_CLKDIV_4        (2)
#define BCM_CLKDIV_8        (3)
#define BCM_CLKDIV_MAX      (4)
#define BCM_DCOCLK_1MHZ     (0)
#define BCM_DCOCLK_8MHZ     (1)
#define BCM_DCOCLK_12MHZ    (2)
#define BCM_DCOCLK_16MHZ    (3)
#define BCM_DCOCLK_MAX      (4)

/* Exported macro ------------------------------------------------------------*/


 /* Exported functions --------------------------------------------------------*/

 /** @defgroup BCM_Exported_Functions BCM Exported Functions
   * @brief    BCM Exported functions
   * @{
   */
void HAL_BCM_Setup(uint8_t clk, uint8_t clksrc, uint8_t clkdiv);
void HAL_BCM_Set_DCO_Freq(uint8_t dcocfg, uint16_t regconfig);
uint32_t HAL_BCM_GetCLKFreq(uint8_t clk);

 /**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_MSP430_HAL_BCM_H_ */

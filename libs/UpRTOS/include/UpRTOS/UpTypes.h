/**
  ******************************************************************************
  * @file       UpTypes.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the definitions for the UpRTOS data types.
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
#ifndef UPRTOS_UPTYPES_H_
#define UPRTOS_UPTYPES_H_

#ifdef __cplusplus
 extern "C" {
#endif


 /* Includes ------------------------------------------------------------------*/
#include <msp430.h>
#include <stdint.h>
#include <UpRTOSConfig.h>

/** @addtogroup UPTYPES
 * @{
 */

/* Exported types ------------------------------------------------------------*/

/** @defgroup UPTYPES_Exported_Types UPTYPES Exported Types
 * @brief    UPTYPES Exported Types
 * @{
 */

/**
 * @brief  XXXXXXX
 */


 typedef void (* TaskFunction_t)( void * arg );
/**
 * @}
 */

/* Exported constants --------------------------------------------------------*/

/** @defgroup UPTYPES_Exported_Constants UPTYPES Exported Constants
  * @brief    UPTYPES Exported constants
  * @{
  */
#if configUSE_16_BIT_TICKS == (1)
#define portMAX_DELAY               (UINT16_MAX)
#else
#define portMAX_DELAY               (UINT32_MAX)
#endif

/**
 * @}
 */


/* Exported macro ------------------------------------------------------------*/


 /* Exported functions --------------------------------------------------------*/

 /** @defgroup UPTYPES_Exported_Functions UPTYPES Exported Functions
   * @brief    UPTYPES Exported functions
   * @{
   */

 /**
  * @}
  */

/**
 * @}
 */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

 #ifdef __cplusplus
 }
 #endif

#endif /* UPRTOS_UPTYPES_H_ */

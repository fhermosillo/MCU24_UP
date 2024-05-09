/**
  ******************************************************************************
  * @file       UpMutex.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the prototype functions for the UpRTOS mutex
  *             module
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
#ifndef UPRTOS_UPMUTEX_H_
#define UPRTOS_UPMUTEX_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <UpRTOSConfig.h>
#include <UpRTOS/UpTypes.h>
#include <UpRTOS/UpPortable.h>

/* Exported types ------------------------------------------------------------*/
typedef void * MutexHandle_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
MutexHandle_t xMutexCreate(void);
UBaseType_t xMutexTake(MutexHandle_t xMutex, TickType_t xTicksToWait);
UBaseType_t xMutexGive(MutexHandle_t xMutex);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UPRTOS_UPMUTEX_H_ */

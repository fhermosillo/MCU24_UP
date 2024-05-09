/**
  ******************************************************************************
  * @file       UpRTOS.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the inclusion of all UpRTOS services.
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
#ifndef DRIVER_UPRTOS_H_
#define DRIVER_UPRTOS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <UpRTOSConfig.h>
#include <UpRTOS/UpTask.h>
#include <UpRTOS/UpQueue.h>
#include <UpRTOS/UpMutex.h>

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
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

#endif /* DRIVER_UPRTOS_H_ */

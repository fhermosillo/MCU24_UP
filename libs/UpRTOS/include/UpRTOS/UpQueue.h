/**
  ******************************************************************************
  * @file       UpQueue.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the prototype functions for the UpRTOS queue
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
#ifndef UPRTOS_UPQUEUE_H_
#define UPRTOS_UPQUEUE_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include <UpRTOSConfig.h>
#include <UpRTOS/UpTypes.h>
#include <UpRTOS/UpPortable.h>

/* Exported types ------------------------------------------------------------*/
typedef void * QueueHandle_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
QueueHandle_t xQueueCreate(uint8_t ucNumItems, uint8_t ucSizePerItem);
UBaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait);
UBaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UPRTOS_UPQUEUE_H_ */

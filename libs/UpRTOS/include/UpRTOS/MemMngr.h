/**
  ******************************************************************************
  * @file       MemMngr.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the prototype functions of the UpRTOS memory
  *             manager module
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
#ifndef UPRTOS_MEMMNGR_H_
#define UPRTOS_MEMMNGR_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include <UpRTOSConfig.h>
#include <UpRTOS/UpTypes.h>
#include <UpRTOS/UpPortable.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void *pvPortMalloc(uint16_t xWantedSize);
void vPortFree(void *pvPtr);
uint16_t xPortGetFreeHeapSize(void);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UPRTOS_MEMMNGR_H_ */

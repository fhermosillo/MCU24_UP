/**
  ******************************************************************************
  * @file       UpTask.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the prototype functions for the UpRTOS task
  *             and schedule module
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
#ifndef UPRTOS_UPTASK_H_
#define UPRTOS_UPTASK_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <UpRTOSConfig.h>
#include <UpRTOS/UpTypes.h>
#include <UpRTOS/MemMngr.h>
#include <UpRTOS/UpList.h>

/* Exported types ------------------------------------------------------------*/
typedef void * TaskHandle_t;

typedef enum {
    eNoAction,
    eSetBits,
    eIncrement,
    eSetValueWithOverwrite
} eNotifyAction;

/* Exported constants --------------------------------------------------------*/
#define tskMAX_DELAY_FLAG   (0x01)
#define tskTIMEOUT_FLAG     (0x02)
#define tskTICK_LESS        (0x04)

#define yldSTATE_UNCHANGE   (0x00)
#define yldSTATE_CHANGE     (0x01)
#define yldNORMAL_YIELD     (yldSTATE_CHANGE)

#define tskPRIORITY_0          (configIDLE_PRIORITY)
#define tskPRIORITY_1          (configIDLE_PRIORITY + 1)
#define tskPRIORITY_2          (configIDLE_PRIORITY + 2)
#define tskPRIORITY_3          (configIDLE_PRIORITY + 3)

/* Exported macro ------------------------------------------------------------*/
#define vTaskYield()    vPortTaskYield(yldNORMAL_YIELD)

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void vTaskStartScheduller(void);
UBaseType_t xTaskCreate(TaskFunction_t xTaskFunc,
                        StackType_t uxStackDepth,
                        void *pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t *pxHandle);
TaskHandle_t xTaskGetCurrentTaskHandle(void);
TaskHandle_t xTaskGetHandle(UBaseType_t uxTaskID );
void vTaskDelay(const TickType_t xTicksToDelay);
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
void vTaskResume(TaskHandle_t xTaskToResume);

// Must run with privilege
BaseType_t xTaskRemoveFromEventList(List_t * const pxEventList );
BaseType_t xTaskPlaceOnEventList(List_t * const pxEventList, const TickType_t xTicksToWait);
void vTaskYieldFromEventList(List_t * const xList);

TickType_t xTaskGetTickCount(void);
UBaseType_t xTaskCheckTimeout(void);

// Notifications
UBaseType_t xTaskNotifyWait(    uint16_t usBitsToClear,
                                uint16_t *pusNotificationValue,
                                TickType_t xTicksToWait);
UBaseType_t xTaskNotify( TaskHandle_t xTaskToNotify,
                       uint16_t usValue,
                       eNotifyAction eAction);
void vTaskNotifyFromISR(    TaskHandle_t xTaskToNotify,
                            uint16_t usValue,
                            eNotifyAction eAction,
                            UBaseType_t *pxHigherPriorityTaskWoken );

// Port
// TODO: Move to UpPortable.asm
void vPortTaskYield(UBaseType_t xYieldFlags);
inline void vPortSaveContextFromISR(void);
inline void vPortRestoreContextFromISR(void);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UPRTOS_UPTASK_H_ */

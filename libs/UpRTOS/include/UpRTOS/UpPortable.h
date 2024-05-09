/**
  ******************************************************************************
  * @file       UpPortable.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the prototype functions for the UpRTOS
  *             specific MCU module.
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
#ifndef UPRTOS_UPPORTABLE_H_
#define UPRTOS_UPPORTABLE_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include <msp430.h>
#include <UpRTOSConfig.h>
#include <UpRTOS/UpTypes.h>

/* Exported types ------------------------------------------------------------*/
typedef uint16_t StackType_t;

#if configUSE_16_BIT_TICKS == (1)
typedef uint16_t tick_t;
#else
typedef uint32_t TickType_t;
#endif

typedef uint16_t UBaseType_t;
typedef int16_t BaseType_t;

/* Exported constants --------------------------------------------------------*/
#define portINT_ENABLED_MASK    (0x0008)
#define portMCLK_FREQUENCY_HZ   (16000000UL)

/* Exported macro ------------------------------------------------------------*/
#define portENABLE_INTERRUPTS()     __enable_interrupt();\
                                    asm(" nop \n ")
#define portDISABLE_INTERRUPTS()    __disable_interrupt();\
                                    asm(" nop \n ")

#define portENTER_CRITICAL()    {\
                                    UBaseType_t xInterruptStatus__ = __get_SR_register() & 0x0008;\
                                    __disable_interrupt();
#define portEXIT_CRITICAL()         __bis_SR_register(xInterruptStatus__);\
                                }

#define portSAVE_CPU_STATUS()       asm(" push  SR\n")
#define portRESTORE_CPU_STATUS()    asm(" pop  SR\n")

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
UBaseType_t xPortSetuptTimerInterrupt(void);    //// SysTick (TA1-CCR0)
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters);
//void vPortPreemptiveTickISR(void);
//void vPortCooperativeTickISR(void);
//__bic_SR_register_on_exit(SCG1 | SCG0 | OSCOFF | CPUOFF);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UPRTOS_UPPORTABLE_H_ */

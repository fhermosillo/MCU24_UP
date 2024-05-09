/*
 * UpRTOSConfig.h
 *
 *  Created on: 10 mar 2024
 *      Author: User123
 */

#ifndef UPRTOS_UPRTOSCONFIG_H_
#define UPRTOS_UPRTOSCONFIG_H_

/* Public includes ------------------------------------*/

/* Defines --------------------------------------------*/
// UpRTOS
#define configUSE_PREEMPTION        (1)
#define configMAX_TASKS             (5)
#define configMAX_PRIORITIES        (3)
#define configCPU_CLOCK_HZ          (16000000UL)
#define configTICK_RATE_HZ          (1000UL)
#define configUSE_16_BIT_TICKS      (0)
// Mutex
#define configUSE_MUTEXS            (1)
// Notifications
#define configUSE_NOTIFICATIONS     (1)

// Stack
#define configTOTAL_HEAP_SIZE       (400)   // (in bytes)(max bytes = 512 - C_STACK(Project Properties)
#define configMINIMAL_STACK_SIZE    (40)    // (in bytes)(32 bytes for cpu registers)
#define configSTACK_ENHANCED        (0)




// Do not modify the following definitions
#define configIDLE_PRIORITY         (0)

#define pdTRUE  (1)
#define pdFALSE (0)

#ifndef NULL
#define NULL    ((void *)(0))
#endif

/* Macros ----------------------------------------------------*/
#define configASSERT_RETURN(xConditionToAssert,xReturn) if(!(xConditionToAssert)) return (xReturn);
#define configASSERT(xConditionToAssert) if(!(xConditionToAssert)) return;

#define pdMS_TO_TICKS( xTimeInMs )    ( ( TickType_t )( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )

/* Typedefs -------------------------------------------*/

/* Public prototype function --------------------------*/


#endif /* UPRTOS_UPRTOSCONFIG_H_ */


/*
 * UpMutex.c
 *
 *  Created on: 26 mar 2024
 *      Author: User123
 */


/* Private includes -----------------------------------*/
#include <UpRTOS/UpMutex.h>
#include <UpRTOS/MemMngr.h>
#include <UpRTOS/UpTask.h>

/* Private defines ---------------------------------------------------*/

/* Private macros ----------------------------------------------------*/

/* Private typedefs --------------------------------------------------*/
typedef struct
{
    volatile UBaseType_t uxLock;
    UBaseType_t uxRecursiveCallCount;
    TaskHandle_t xTaskHolder;
    List_t xTasksWaitingToHold;
} Mutex_t;

/* Private prototype function ----------------------------------------*/


/* Private variables -------------------------------------------------*/


/* Reference function ------------------------------------------------*/
MutexHandle_t xMutexCreate(void)
{
    Mutex_t *pxMutex = NULL;
    portENTER_CRITICAL();
    pxMutex = (Mutex_t *)pvPortMalloc(sizeof(Mutex_t));
    if(pxMutex != NULL)
    {
        pxMutex->uxLock = 0;
        pxMutex->uxRecursiveCallCount = 0;
        vListCreateStatic(&pxMutex->xTasksWaitingToHold);
        pxMutex->xTaskHolder = NULL;
    }
    portEXIT_CRITICAL();

    return (MutexHandle_t)pxMutex;
}

UBaseType_t xMutexTake(MutexHandle_t hMutex, TickType_t xTicksToWait)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    UBaseType_t xReturn = pdTRUE;
    Mutex_t *pxMutex = (Mutex_t *)hMutex;

    // Enter a critical section
    portENTER_CRITICAL();

    if( pxMutex->uxLock )
    {
        // Increase recursive call counter
        pxMutex->uxRecursiveCallCount++;

        // Add current task to pending list
        xTaskPlaceOnEventList(&pxMutex->xTasksWaitingToHold, xTicksToWait);

        // Task yield
        vPortTaskYield(yldSTATE_UNCHANGE);

        // Decrement recursive call count
        pxMutex->uxRecursiveCallCount--;

        // Remove from the pending list
        xTaskRemoveFromEventList(&pxMutex->xTasksWaitingToHold);

        // Check timeout event
        if( xTaskCheckTimeout() ) xReturn = pdFALSE;
    }

    if(xReturn)
    {
        pxMutex->uxLock = 1;
        pxMutex->xTaskHolder = xTaskGetCurrentTaskHandle();
    }


    portEXIT_CRITICAL();

    // The sr needs restoring after returning to this task
    portRESTORE_CPU_STATUS();

    return xReturn;
}

UBaseType_t xMutexGive(MutexHandle_t hMutex)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    UBaseType_t xReturn = pdFALSE;
    Mutex_t *pxMutex = (Mutex_t *)hMutex;

    // Critical section
    portENTER_CRITICAL();

    // Check if the current task is the task that took the mutex
    if(xTaskGetCurrentTaskHandle() == pxMutex->xTaskHolder )
    {
        // Release mutex
        pxMutex->uxLock = 0;
        pxMutex->xTaskHolder = NULL;

        // Check if there is any pending task trying to take the mutex
        if(pxMutex->xTasksWaitingToHold.uxNumberOfItems)
        {
            // Yield
            vTaskYieldFromEventList(&pxMutex->xTasksWaitingToHold);
        }

        xReturn = pdTRUE;
    }
    portEXIT_CRITICAL();

    //
    portRESTORE_CPU_STATUS();
    return xReturn;
}

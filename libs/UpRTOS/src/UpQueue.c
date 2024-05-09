/*
 * UpQueue.c
 *
 *  Created on: 26 mar 2024
 *      Author: User123
 */

/* Private includes -----------------------------------*/
#include <UpRTOS/UpQueue.h>
#include <UpRTOS/MemMngr.h>
#include <UpRTOS/UpTask.h>
#include <string.h>

/* Private defines ---------------------------------------------------*/

/* Private macros ----------------------------------------------------*/

/* Private typedefs --------------------------------------------------*/
typedef struct
{
    uint8_t *pucTail;
    uint8_t *pucHead;
    uint8_t *pucReadFrom;
    uint8_t *pucWriteTo;

    List_t xTasksWaitingToSend;             /**< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
    List_t xTasksWaitingToReceive;          /**< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

    volatile UBaseType_t uxMessagesWaiting; /**< The number of items currently in the queue. */
    UBaseType_t uxLength;                   /**< The length of the queue defined as the number of items it will hold, not the number of bytes. */
    UBaseType_t uxItemSize;                 /**< The size of each items that the queue will hold. */

    //volatile UBaseType_t cRxLock;         /**< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
    //volatile UBaseType_t cTxLock;         /**< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
} Queue_t;

/* Private prototype function ----------------------------------------*/


/* Private variables -------------------------------------------------*/


/* Reference function ------------------------------------------------*/
QueueHandle_t xQueueCreate(uint8_t ucNumItems, uint8_t ucSizePerItem)
{
    Queue_t *pxQueue = NULL;
    portENTER_CRITICAL();
    pxQueue = (Queue_t *)pvPortMalloc(sizeof(Queue_t));
    if(pxQueue != NULL)
    {
        uint16_t uiQueueTotalSize = ucNumItems * ucSizePerItem;
        //pxQueue->cRxLock = 0;
        //pxQueue->cTxLock = 0;
        pxQueue->pucHead = (uint8_t *)pvPortMalloc(uiQueueTotalSize);
        if(pxQueue->pucHead == NULL) return NULL;
        pxQueue->uxItemSize = ucSizePerItem;
        pxQueue->uxLength = ucNumItems;
        pxQueue->pucTail = pxQueue->pucHead + uiQueueTotalSize;
        pxQueue->pucWriteTo  = pxQueue->pucHead;
        pxQueue->pucReadFrom = pxQueue->pucTail - pxQueue->uxItemSize;
        vListCreateStatic(&pxQueue->xTasksWaitingToSend);
        vListCreateStatic(&pxQueue->xTasksWaitingToReceive);
        pxQueue->uxMessagesWaiting = 0;
    }
    portEXIT_CRITICAL();

    return pxQueue;
}

UBaseType_t xQueueSend(QueueHandle_t hQueue, const void *pvItemToQueue, TickType_t xTicksToWait)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    UBaseType_t xReturn = pdTRUE;

    // Enter a critical section
    portENTER_CRITICAL();

    Queue_t *pxQueue = (Queue_t *)hQueue;
    if( pxQueue->uxMessagesWaiting == pxQueue->uxLength)
    {
        // Add current task to pending list
        xTaskPlaceOnEventList(&pxQueue->xTasksWaitingToSend, xTicksToWait);

        // Task yield
        vPortTaskYield(yldSTATE_UNCHANGE);

        // Remove from the pending list
        xTaskRemoveFromEventList(&pxQueue->xTasksWaitingToSend);

        // Check if timeout (TODO should I clear status flag?)
        if( xTaskCheckTimeout() ) xReturn = pdFALSE;
    }

    if(xReturn)
    {
        // Send to back
        memcpy(( void * )pxQueue->pucWriteTo, ( void * )pvItemToQueue, pxQueue->uxItemSize);
        pxQueue->pucWriteTo += pxQueue->uxItemSize;
        if( pxQueue->pucWriteTo >= pxQueue->pucTail )
        {
            pxQueue->pucWriteTo = pxQueue->pucHead;
        }
        pxQueue->uxMessagesWaiting++;

        // Wake-up tasks waiting to receive
        vTaskYieldFromEventList(&pxQueue->xTasksWaitingToReceive);
    }


    portEXIT_CRITICAL();

    // The sr needs restoring after returning to this task
    portRESTORE_CPU_STATUS();

    return xReturn;
}

UBaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    UBaseType_t xReturn = pdTRUE;
    // Enter a critical section
    portENTER_CRITICAL();

    Queue_t *pxQueue = (Queue_t *)xQueue;
    if( pxQueue->uxMessagesWaiting == 0)
    {
        // Add current task to pending list
        xTaskPlaceOnEventList(&pxQueue->xTasksWaitingToReceive, xTicksToWait);

        // Task yield
        vPortTaskYield(yldSTATE_UNCHANGE);

        // Remove from the pending list
        xTaskRemoveFromEventList(&pxQueue->xTasksWaitingToReceive);

        // Check if timeout (TODO should I clear status flag?)
        if( xTaskCheckTimeout() ) xReturn = pdFALSE;
    }

    if(xReturn)
    {
        // Pop front
        pxQueue->pucReadFrom += pxQueue->uxItemSize;
        if( pxQueue->pucReadFrom >= pxQueue->pucTail )
        {
            pxQueue->pucReadFrom = pxQueue->pucHead;
        }
        memcpy(( void * )pvBuffer, ( void * )pxQueue->pucReadFrom, pxQueue->uxItemSize );
        pxQueue->uxMessagesWaiting--;

        // Wake-up tasks waiting to send
        vTaskYieldFromEventList(&pxQueue->xTasksWaitingToSend);
    }


    portEXIT_CRITICAL();

    // The sr needs restoring after returning to this task
    portRESTORE_CPU_STATUS();

    return xReturn;
}

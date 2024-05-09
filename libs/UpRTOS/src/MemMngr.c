/*
 * MemMngr.c
 *
 *  Created on: 10 mar 2024
 *      Author: User123
 */


/* Public includes ------------------------------------*/
#include <UpRTOS/MemMngr.h>

/* Defines --------------------------------------------*/

/* Macros ---------------------------------------------*/

/* Typedefs -------------------------------------------*/

/* Private Variables ----------------------------------*/
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
uint16_t xNextFreeByte = ( uint16_t ) 0U;

/* Reference function ---------------------------------*/
void *pvPortMalloc(uint16_t xWantedSize)
{
    void * pvReturn = NULL;

    /* Enter a critical section */
    portENTER_CRITICAL();

    /* TODO: Ensure that always allocates multiples of CPU word size in bytes */
    if( xWantedSize & (sizeof(UBaseType_t)-1) ) xWantedSize++;

    // Check if there is enough space for the allocation and
    if( ( xWantedSize > 0 ) &&
        ( ( xNextFreeByte + xWantedSize ) < configTOTAL_HEAP_SIZE ) &&  // valid size
        ( ( xNextFreeByte + xWantedSize ) > xNextFreeByte ) )           // Also check for overflow
    {
        /* Return the next free byte then increment the index past this
         * block. */
        pvReturn = (uint8_t *)ucHeap + xNextFreeByte;
        xNextFreeByte += xWantedSize;
    }

    /* Exit a critical section */
    portEXIT_CRITICAL();

    return pvReturn;
}
void vPortFree(void *pvPtr)
{
    (void)pvPtr;
}

uint16_t xPortGetFreeHeapSize(void)
{
    return configTOTAL_HEAP_SIZE - xNextFreeByte;
}

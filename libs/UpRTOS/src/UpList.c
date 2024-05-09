/*
 * UpList.c
 *
 *  Created on: 12 mar 2024
 *      Author: User123
 */

/* Private includes --------------------------------------------------*/
#include <UpRTOS/UpList.h>

/* Private defines ---------------------------------------------------*/


/* Private macros ----------------------------------------------------*/

/* Private typedefs --------------------------------------------------*/

/* Private prototype function ----------------------------------------*/

/* Private variables -------------------------------------------------*/

/* Reference function ------------------------------------------------*/
/*!
 * @name pxListCreate
 * @brief Allocate an empty double-linked list object
 * @return Pointer to the created list
 */
List_t *pxListCreate(void)
{
    List_t *pxList = (List_t *)pvPortMalloc(sizeof(List_t));
    configASSERT_RETURN(pxList != NULL, NULL);

    pxList->pxHead = NULL;
    pxList->pxTail = NULL;
    pxList->uxNumberOfItems = 0;

    return pxList;
}

void vListCreateStatic(List_t *pxList)
{
    pxList->pxHead = NULL;
    pxList->pxHead = NULL;
    pxList->uxNumberOfItems = 0;
}

UBaseType_t xListInsert(List_t *pxList, ListNode_t *pxNodeToInsert)
{
    configASSERT_RETURN(pxList != NULL, pdFALSE);
    configASSERT_RETURN(pxNodeToInsert != NULL, pdFALSE);
    configASSERT_RETURN(pxNodeToInsert->pvContainer == NULL, pdFALSE);

    pxNodeToInsert->pvContainer = (void *)pxList;

    if (pxList->uxNumberOfItems) {
        pxNodeToInsert->pxPrev = pxList->pxTail;
        pxNodeToInsert->pxNext = NULL;
        pxList->pxTail->pxNext = pxNodeToInsert;
        pxList->pxTail = pxNodeToInsert;
    } else {
        pxList->pxHead = pxList->pxTail = pxNodeToInsert;
        pxNodeToInsert->pxPrev = pxNodeToInsert->pxNext = NULL;
    }

    pxList->uxNumberOfItems++;

    return pdTRUE;
}

UBaseType_t xListRemove(List_t *pxList,  ListNode_t *pxNodeToRemove)
{
    configASSERT_RETURN(pxList->uxNumberOfItems,pdFALSE);
    configASSERT_RETURN((List_t *)pxNodeToRemove->pvContainer == pxList, pdFALSE);

    // Check if the previous node of pxNoteToRemove is the head
    if(pxNodeToRemove->pxPrev) pxNodeToRemove->pxPrev->pxNext = pxNodeToRemove->pxNext;
    else pxList->pxHead = pxNodeToRemove->pxNext;

    // Check if the next node of pxNoteToRemove is the tail
    if(pxNodeToRemove->pxNext) pxNodeToRemove->pxNext->pxPrev = pxNodeToRemove->pxPrev;
    else pxList->pxTail = pxNodeToRemove->pxPrev;

    // Remove its container
    pxNodeToRemove->pvContainer = NULL;

    // Decrement list length
    pxList->uxNumberOfItems--;

    return pdTRUE;
}

/* Private reference functions -----------------------------------*/

/* Interrupt handler --------------------------------------------*/

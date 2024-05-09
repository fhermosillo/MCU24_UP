/**
  ******************************************************************************
  * @file       UpList.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the prototype functions of the UpRTOS list
  *             structure
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
#ifndef UPRTOS_UPLIST_H_
#define UPRTOS_UPLIST_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include <UpRTOSConfig.h>
#include <UpRTOS/UpTypes.h>
#include <UpRTOS/MemMngr.h>

/* Exported types ------------------------------------------------------------*/
struct ListNode
{
    void *pvItem;               /**< The value being listed.  In most cases this is used to sort the list in ascending order. */
    struct ListNode *pxNext;    /**< Pointer to the next ListItem_t in the list. */
    struct ListNode *pxPrev;    /**< Pointer to the previous ListItem_t in the list. */
    void *pvContainer;          /**< Pointer to the owner. */
};

typedef struct ListNode ListNode_t;

typedef struct
{
    ListNode_t *pxHead;
    ListNode_t *pxTail;
    UBaseType_t uxNumberOfItems;
} List_t;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// Insert back
#define vListInsert(pxList,pxNodeToInsert)   {\
                                                if((pxList) != NULL && pxNodeToInsert != NULL && (pxNodeToInsert)->pvContainer == NULL)\
                                                {\
                                                    (pxNodeToInsert)->pvContainer = (void *)(pxList);\
                                                    if ((pxList)->uxNumberOfItems) {\
                                                        (pxNodeToInsert)->pxPrev = (pxList)->pxTail;\
                                                        (pxNodeToInsert)->pxNext = NULL;\
                                                        (pxList)->pxTail->pxNext = (pxNodeToInsert);\
                                                        (pxList)->pxTail = (pxNodeToInsert);\
                                                    } else {\
                                                        (pxList)->pxHead = (pxList)->pxTail = (pxNodeToInsert);\
                                                        (pxNodeToInsert)->pxPrev = (pxNodeToInsert)->pxNext = NULL;\
                                                    }\
                                                    (pxList)->uxNumberOfItems++;\
                                                }\
                                            }
#define vListInsertBack(pxList,pxNodeToInsert) vListInsert((pxList),(pxNodeToInsert))

#define vListRemove(pxList,pxNodeToRemove)  {\
                                                if((pxList) != NULL && (pxList)->uxNumberOfItems && (List_t *)(pxNodeToRemove)->pvContainer == (pxList))\
                                                {\
                                                    if((pxNodeToRemove)->pxPrev) (pxNodeToRemove)->pxPrev->pxNext = (pxNodeToRemove)->pxNext;\
                                                    else (pxList)->pxHead = (pxNodeToRemove)->pxNext;\
                                                    if((pxNodeToRemove)->pxNext) (pxNodeToRemove)->pxNext->pxPrev = (pxNodeToRemove)->pxPrev;\
                                                    else (pxList)->pxTail = (pxNodeToRemove)->pxPrev;\
                                                    (pxNodeToRemove)->pvContainer = NULL;\
                                                    (pxNodeToRemove)->pvItem = NULL;\
                                                    (pxList)->uxNumberOfItems--;\
                                                }\
                                            }

#define pxListBack(pxList)  ((pxList) != NULL && (pxList)->uxNumberOfItems != 0 ? (pxList)->pxTail : NULL)
#define pxListFront(pxList) ((pxList) != NULL && (pxList)->uxNumberOfItems != 0 ? (pxList)->pxHead : NULL)

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
List_t *pxListCreate(void);
void vListCreateStatic(List_t *pxList);
UBaseType_t xListInsert(List_t *pxList, ListNode_t *pxNodeToInsert);
UBaseType_t xListRemove(List_t *pxList,  ListNode_t *pxNodeToRemove);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UPRTOS_UPLIST_H_ */

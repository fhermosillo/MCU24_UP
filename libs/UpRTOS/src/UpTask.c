/*
 * UpTask.c
 *
 *  Created on: 12 mar 2024
 *      Author: User123
 */


/* Private includes -----------------------------------*/
#include <UpRTOS/UpTask.h>
#include <UpRTOS/UpList.h>

/* Private defines ---------------------------------------------------*/
#define osIDLE_TASK_SET     0x01
#define osSCHEDULER_STARTED 0x80
#define osSCHEDULER_RUNNING 0x40
#define osTICK_OVERFLOW     0x20

/* Private macros ----------------------------------------------------*/
#define osCHECK_FLAG(REG,FLAG)  ((REG) & FLAG)

// Save current task stack pointer
#define portSAVE_CONTEXT()      asm(" push R15\n");\
                                asm(" push R14\n");\
                                asm(" push R13\n");\
                                asm(" push R12\n");\
                                asm(" push R11\n");\
                                asm(" push R10\n");\
                                asm(" push R9\n");\
                                asm(" push R8\n");\
                                asm(" push R7\n");\
                                asm(" push R6\n");\
                                asm(" push R5\n");\
                                asm(" push R4\n");\
                                asm(" mov.w &pxCurrentTCB,R15\n");\
                                asm(" mov.w SP,0(R15)\n");


// Restore current task stack pointer
#define portRESTORE_CONTEXT()   asm(" mov.w &pxCurrentTCB,R15 \n");\
                                asm(" mov.w 0(R15),SP \n");\
                                asm(" nop \n ");\
                                asm(" pop R4\n");\
                                asm(" pop R5\n");\
                                asm(" pop R6\n");\
                                asm(" pop R7\n");\
                                asm(" pop R8\n");\
                                asm(" pop R9\n");\
                                asm(" pop R10\n");\
                                asm(" pop R11\n");\
                                asm(" pop R12\n");\
                                asm(" pop R13\n");\
                                asm(" pop R14\n");\
                                asm(" pop R15\n");\
                                asm(" reti\n");

#define portSAVE_CONTEXT_FROM_ISR() asm(" pop &pxAuxTCB");\
                                    asm(" push R9\n");\
                                    asm(" push R8\n");\
                                    asm(" push R7\n");\
                                    asm(" push R6\n");\
                                    asm(" push R5\n");\
                                    asm(" push R4\n");\
                                    asm(" mov.w &pxCurrentTCB,R15\n");\
                                    asm(" mov.w SP,0(R15)\n");\
                                    asm(" push &pxAuxTCB");

#define portCALL_SCHEDULER_FROM_ISR()   asm(" mov.w &pxTopOfSchStack, R15\n");\
                                        asm(" mov.w R15, SP \n");\
                                        asm(" reti\n");

/* Private typedefs --------------------------------------------------*/
typedef enum {
    TASK_RUNNING = 0,
    TASK_READY,
    TASK_SUSPENDED,
    TASK_BLOCKED
} eTaskState;

struct tcb
{
    StackType_t *pxTopOfStack; /*!< For scheduling mechanism */
#if configSTACK_ENHANCED == (1)
    StackType_t *pxEndOfStack;
    StackType_t *pxBeginOfStack;
#endif
    UBaseType_t uxId;                   /*!< For scheduling mechanism */
    UBaseType_t uxPriority;             /*!< For scheduling mechanism */
    eTaskState xState;                  /*!< For scheduling mechanism */

    UBaseType_t uxStatus;               /*!< For timing constraints TODO: Can be omitted by checking overflow or set task to suspended on case of portMAX_DELAY */
    TickType_t xTimeToWake;             /*!< For timing constraints */

#if ( configUSE_NOTIFICATIONS == 1 )
    uint16_t xNotificationValue;        /*!< For notifications */
#endif

    ListNode_t xEventListItem;          /*!< For mutex and queues */

    struct tcb *pxNextTCB;              /*!< For context switch TODO: Can be omitted using a  pxReadyTasksLists */
};
typedef struct tcb tcb_t;


/* Private prototype function ----------------------------------------*/
static void vTaskSwitchContext(void);
static void vTaskIdleHook(void *pvParams);
static void vTaskRun(tcb_t *pxTaskToRun);


/* Private variables -------------------------------------------------*/
tcb_t * volatile pxCurrentTCB = NULL;     // Current TCB
tcb_t *pxTCBList = NULL;        // Task TCB List [TODO: CHANGE TO XLIST FOR IMPROVE]
tcb_t *pxAuxTCB = NULL;         // Auxiliary TCB
static TaskHandle_t xIdleTaskHandle = NULL; // Idle task TCB

UBaseType_t uxCurrentNumberOfTasks = 0;
volatile TickType_t xTickCount = 0;
UBaseType_t uxSchedulerFlags = 0x00;
//static volatile BaseType_t xNumOfOverflows = ( BaseType_t ) 0;
static StackType_t xScheduleStack[2] = {0x0000, 0x0000};
static StackType_t *pxTopOfSchStack = NULL;

// UPRTOS_OVERHEAD = sizeof(tcb_t)*4 + sizeof(uint8_t)*2 + sizeof(uint32_t) + sizeof(stack_t)*3 + sizeof(uint16_t)
//                 = 24 bytes






/* Reference function ------------------------------------------------*/
/*!
 * @name xGetTicks
 * @brief Get current time in ticks
 * @return Current ticks
 */
TickType_t xTaskGetTickCount(void)
{
    TickType_t xTicks;

    portENTER_CRITICAL();

    xTicks = xTickCount;

    portEXIT_CRITICAL();

    return xTicks;
}


UBaseType_t xTaskCheckTimeout(void)
{
    UBaseType_t xReturn = pdFALSE;

    // Check if task is blocked
    if(pxCurrentTCB->uxStatus & tskTIMEOUT_FLAG) xReturn = pdTRUE;
    pxCurrentTCB->uxStatus &= ~tskTIMEOUT_FLAG;

    return xReturn;
}




BaseType_t xTaskRemoveFromEventList(List_t * const pxEventList )
{
    // Remove from the pending list
    configASSERT_RETURN(pxEventList != NULL, pdFALSE);
    configASSERT_RETURN(pxCurrentTCB->xEventListItem.pvContainer == pxEventList, pdFALSE);

    vListRemove(pxEventList, &pxCurrentTCB->xEventListItem);

    return pdTRUE;
}

BaseType_t xTaskPlaceOnEventList(List_t * const pxEventList, const TickType_t xTicksToWait)
{
    // Assert
    configASSERT_RETURN(pxEventList != NULL, pdFALSE);
    configASSERT_RETURN(pxCurrentTCB->xEventListItem.pvContainer == NULL, pdFALSE);

    // Add current task to pending list
    pxCurrentTCB->xEventListItem.pvItem = (void *)pxCurrentTCB;
    vListInsert(pxEventList, &pxCurrentTCB->xEventListItem);

    // Set delayed task property
    if( xTicksToWait == portMAX_DELAY )
    {
        //
        pxCurrentTCB->xState = TASK_SUSPENDED;
    }
    else
    {
        // Calculate the time at which the task should be woken if the event does not occur
        // TODO: Take care of overflow
        pxCurrentTCB->xTimeToWake = xTickCount + xTicksToWait;
        pxCurrentTCB->xState = TASK_BLOCKED;
    }
    pxCurrentTCB->uxStatus &= ~(tskTIMEOUT_FLAG | tskMAX_DELAY_FLAG);

    return pdTRUE;
}

void vTaskYieldFromEventList(List_t * const xList)
{
    portENTER_CRITICAL();

    // Select the highest priority task
    UBaseType_t xHigherPriority = configIDLE_PRIORITY;
    ListNode_t *pxNode = xList->pxHead;
    pxAuxTCB = NULL;
    while(pxNode != NULL)
    {
        // Check if the current pending task is timeout
        tcb_t *pxTCB = ((tcb_t *)pxNode->pvItem);
        if( !osCHECK_FLAG(pxTCB->uxStatus, tskTIMEOUT_FLAG) )
        {
            if(pxTCB->uxPriority >= xHigherPriority)
            {
                xHigherPriority = pxTCB->uxPriority;
                pxAuxTCB = pxTCB;
            }
        }
        pxNode = pxNode->pxNext;
    }

    // Switch to the highest priority task
    if(pxAuxTCB != NULL)
    {
        // Put it in ready state and force to run (TODO is it necessary to force to run?)
        // Or just to put in ready state
        pxAuxTCB->xState = TASK_READY;
        vTaskRun(pxAuxTCB);
    }
    portEXIT_CRITICAL();
}





void vTaskStartScheduller(void)
{
    configASSERT( !osCHECK_FLAG(uxSchedulerFlags, osSCHEDULER_STARTED) );

    // Check if Idle task was added
    if( !osCHECK_FLAG(uxSchedulerFlags, osIDLE_TASK_SET) )
    {
        xTaskCreate(vTaskIdleHook, configMINIMAL_STACK_SIZE, NULL, configIDLE_PRIORITY, &xIdleTaskHandle);
    }

    // Set scheduller task
    xScheduleStack[1] = (StackType_t)vTaskSwitchContext;
    xScheduleStack[0] = 0x0000;
    pxTopOfSchStack = &xScheduleStack[0];

    // Systick init
    if ( xPortSetuptTimerInterrupt() != pdTRUE)
    {
        while(1); // cpu trap
    }

    // Set flag
    uxSchedulerFlags |= osSCHEDULER_STARTED;
    uxSchedulerFlags |= osSCHEDULER_RUNNING;

    // Call task scheduler
    portRESTORE_CONTEXT();
}



UBaseType_t xTaskCreate(TaskFunction_t xTaskFunc, StackType_t uxStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxHandle)
{
    UBaseType_t xReturn = pdFALSE;
    tcb_t *pxNewTCB = NULL;
    StackType_t *pxEndOfStack = NULL;

    // To avoid compiler warnings
    (void)pxTopOfSchStack;

    // Assert input arguments
#if configUSE_PREEMPTION == (1)
    if(uxPriority > configIDLE_PRIORITY && uxCurrentNumberOfTasks >= configMAX_TASKS) return pdFALSE;
#endif
    if(uxPriority == configIDLE_PRIORITY && osCHECK_FLAG(uxSchedulerFlags,osIDLE_TASK_SET) )  return pdFALSE;  // Prevent to redefine IDLE task
    if(uxStackDepth < configMINIMAL_STACK_SIZE || (uxStackDepth + sizeof(tcb_t)) > xPortGetFreeHeapSize()) return pdFALSE;

    // Disable interrupts
    portENTER_CRITICAL();

    // Check priority
    if(uxPriority > configMAX_PRIORITIES) uxPriority = configMAX_PRIORITIES;

    // Check for idle task
    UBaseType_t uxTaskIDTmp = uxCurrentNumberOfTasks;   // save current number of tasks
    if(uxPriority == configIDLE_PRIORITY) {
        uxCurrentNumberOfTasks = configMAX_TASKS;
        uxSchedulerFlags |= osIDLE_TASK_SET;
    }

    // Add task to scheduller
    pxNewTCB = (tcb_t *)pvPortMalloc(sizeof(tcb_t));
    if(pxNewTCB != NULL)
    {
        pxNewTCB->uxId = uxCurrentNumberOfTasks;
        pxNewTCB->uxStatus = 0x00;
        pxNewTCB->xState = TASK_READY;
        pxNewTCB->uxPriority = uxPriority;

        // Initialize task stack
#if configSTACK_ENHANCED == (1)
        pxNewTCB->pxEndOfStack = (stack_t *)pvPortMalloc(uxStackDepth);
        uxStackDepth >>= 1; // uxStackDepth = uxStackDepth/2
        pxNewTCB->pxBeginOfStack = &pxNewTCB->pxEndOfStack[uxStackDepth-1];
        *pxNewTCB->pxBeginOfStack = (stack_t)( xTaskFunc );
        vStackInit(&pxNewTCB->pxEndOfStack[uxStackDepth-2]);
        pxNewTCB->pxTopOfStack = &pxNewTCB->pxEndOfStack[uxStackDepth-14];
#else
        pxEndOfStack  = (StackType_t *)pvPortMalloc(uxStackDepth);
        if(pxEndOfStack != NULL)
        {
            xReturn = pdTRUE;

            pxNewTCB->pxTopOfStack = pxPortInitialiseStack(pxEndOfStack + (uxStackDepth>>1) - 1, xTaskFunc, pvParameters);
#endif
            pxNewTCB->pxNextTCB = NULL;
            // Set task handle
            if(pxHandle != NULL) *pxHandle = (TaskHandle_t)pxNewTCB;

            // Check idle priority
            if(uxPriority == configIDLE_PRIORITY) uxCurrentNumberOfTasks = uxTaskIDTmp;
            else uxCurrentNumberOfTasks++;

            // Add to list
            if(uxPriority != configIDLE_PRIORITY)
            {
                if(pxTCBList == NULL)
                {
                    pxTCBList = pxNewTCB;
                    pxTCBList->pxNextTCB = NULL;
                    pxCurrentTCB = pxNewTCB;
                }
                else
                {
                    // Set current TCB as the highest priority task
                    if(pxNewTCB->uxPriority > pxCurrentTCB->uxPriority) pxCurrentTCB = pxNewTCB;

                    // Insert into the ready task list in descending order
                    pxAuxTCB = pxTCBList;
                    if(pxNewTCB->uxPriority >= pxTCBList->uxPriority)
                    {
                        pxTCBList = pxNewTCB;
                        pxTCBList->pxNextTCB = pxAuxTCB;
                        pxTCBList->pxNextTCB->pxNextTCB = NULL;
                    }
                    else
                    {
                        while( pxAuxTCB != NULL )
                        {
                            if(pxAuxTCB->pxNextTCB == NULL)
                            {
                                pxAuxTCB->pxNextTCB = pxNewTCB;
                                pxAuxTCB->pxNextTCB->pxNextTCB = NULL;
                                break;
                            }
                            else if(pxNewTCB->uxPriority >= pxAuxTCB->pxNextTCB->uxPriority)
                            {
                                //
                                tcb_t *pxTmp = pxAuxTCB->pxNextTCB;
                                pxAuxTCB->pxNextTCB = pxNewTCB;
                                pxAuxTCB->pxNextTCB->pxNextTCB = pxTmp;
                                break;
                            }
                            pxAuxTCB = pxAuxTCB->pxNextTCB;
                        }
                    }
                }
            }
        }
    }
    // Enable global interrupts if were activated
    portEXIT_CRITICAL();

    return xReturn;
}







TaskHandle_t xTaskGetHandle(UBaseType_t uxTaskID )
{
    TaskHandle_t xTaskHandle = NULL;
    configASSERT_RETURN(uxSchedulerFlags & osSCHEDULER_STARTED, NULL);
    configASSERT_RETURN(uxTaskID < uxCurrentNumberOfTasks, NULL);

    portENTER_CRITICAL();
    pxAuxTCB = pxTCBList;
    while(pxAuxTCB != NULL)
    {
        if(pxAuxTCB->uxId == uxTaskID)
        {
            xTaskHandle = (TaskHandle_t)pxCurrentTCB;
            break;
        }
    }
    portEXIT_CRITICAL();

    return xTaskHandle;
}

TaskHandle_t xTaskGetCurrentTaskHandle(void)
{
    TaskHandle_t xCurrentTaskHandle = NULL;
    configASSERT_RETURN(osCHECK_FLAG(uxSchedulerFlags,osSCHEDULER_STARTED), NULL);

    portENTER_CRITICAL();
    xCurrentTaskHandle = (TaskHandle_t)pxCurrentTCB;
    portEXIT_CRITICAL();

    return xCurrentTaskHandle;
}

void vTaskDelay(const TickType_t xTicksToDelay)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    // Disable interrupts
    portDISABLE_INTERRUPTS();

    // Save context
    portSAVE_CONTEXT();

    //
    if( xTicksToDelay == portMAX_DELAY )
    {
        //
        pxCurrentTCB->xState = TASK_SUSPENDED;
    }
    else
    {
        // Calculate the time at which the task should be woken if the event does not occur
        // TODO: Take care of overflow
        pxCurrentTCB->xTimeToWake = xTickCount + xTicksToDelay;
        pxCurrentTCB->xState = TASK_BLOCKED;
    }

    // Call scheduller
    vTaskSwitchContext();
}


void vPortTaskYield(UBaseType_t xYieldFlags)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    // Disable interrupts
    portDISABLE_INTERRUPTS();

    // Save context
    portSAVE_CONTEXT();

    //
    if( osCHECK_FLAG(xYieldFlags,yldSTATE_CHANGE) ) pxCurrentTCB->xState = TASK_READY;

    // Call scheduller
    vTaskSwitchContext();
}

void vTaskSuspend(TaskHandle_t xTaskToSuspend)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    // Critical section begins
    portENTER_CRITICAL();

    if(xTaskToSuspend != NULL)
    {
        if( ((tcb_t *)xTaskToSuspend)->xState != TASK_SUSPENDED )
        {
            if( ((tcb_t *)xTaskToSuspend)->uxId < uxCurrentNumberOfTasks )
            {
                ((tcb_t *)xTaskToSuspend)->xState = TASK_SUSPENDED;
            }
        }
    } else {
        pxCurrentTCB->xState = TASK_SUSPENDED;
        vPortTaskYield(yldSTATE_UNCHANGE);
    }

    // Critical section ends
    portEXIT_CRITICAL();

    // Restore status register
    portRESTORE_CPU_STATUS();
}

void vTaskResume(TaskHandle_t xTaskToResume)
{
    configASSERT(xTaskToResume != NULL);
    configASSERT(((tcb_t *)xTaskToResume)->uxId != pxCurrentTCB->uxId);

    portENTER_CRITICAL();

    if( ((tcb_t *)xTaskToResume)->uxId < uxCurrentNumberOfTasks)
    {
        if(((tcb_t *)xTaskToResume)->xState == TASK_SUSPENDED)
        {
            if(((tcb_t *)xTaskToResume)->uxPriority > pxCurrentTCB->uxPriority)
            {
                // Yield
                vTaskRun((tcb_t *)xTaskToResume);
            }
            else
            {
                ((tcb_t *)xTaskToResume)->xState = TASK_READY;
            }
        }
    }

    portEXIT_CRITICAL();
}


inline void vPortSaveContextFromISR(void)
{
    portSAVE_CONTEXT_FROM_ISR();
}

inline void vPortRestoreContextFromISR(void)
{
    portRESTORE_CONTEXT();
}





void vTaskIdleHook(void *pvParams)
{
    asm(" mov #1023,R15\n"); // For debug
    while(1)
    {
        __no_operation();
    }
}







UBaseType_t xTaskNotifyWait( uint16_t uxBitsToClear,
                             uint16_t *pxNotificationValue,
                           TickType_t xTicksToWait )
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    UBaseType_t xReturn = pdTRUE;

    // Disable interrupts
    portDISABLE_INTERRUPTS();

    // Clear notification values
    pxCurrentTCB->xNotificationValue &= ~uxBitsToClear;

    // Suspend task
    if( xTicksToWait == portMAX_DELAY )
    {
        //
        pxCurrentTCB->xState = TASK_SUSPENDED;
    }
    else
    {
        // Calculate the time at which the task should be woken if the event does not occur
        // TODO: Take care of overflow
        pxCurrentTCB->xTimeToWake = xTickCount + xTicksToWait;
        pxCurrentTCB->xState = TASK_BLOCKED;
    }

    // Task yield
    vPortTaskYield(0);

    // Check for timeout
    //if(!(pxCurrentTCB->uxStatus & tskMAX_DELAY_FLAG) && xTickCount >= pxCurrentTCB->xTimeToWake) xReturn = pdFALSE;
    if( xTaskCheckTimeout() ) xReturn = pdFALSE;
    else *pxNotificationValue = pxCurrentTCB->xNotificationValue;

    // Restore previous SR
    portRESTORE_CPU_STATUS();

    return xReturn;

}


UBaseType_t xTaskNotify( TaskHandle_t xTaskToNotify,
                         uint16_t xValue,
                         eNotifyAction eAction)
{
    // Assertions
    configASSERT_RETURN(xTaskToNotify != NULL, pdFALSE);                                    // Task to notify must not be NULL
    configASSERT_RETURN(((tcb_t *)xTaskToNotify)->uxId < uxCurrentNumberOfTasks, pdFALSE);  // Task to notify must have a valid ID
    configASSERT_RETURN(((tcb_t *)xTaskToNotify)->uxId != pxCurrentTCB->uxId, pdFALSE);     // Task to notify must not be the running task
    configASSERT_RETURN(((tcb_t *)xTaskToNotify)->xState == TASK_BLOCKED, pdFALSE);         // Task to notify should be in the blocked state

    // Disable interrupts
    portENTER_CRITICAL();

    // Notify
    switch(eAction)
    {
        case eSetBits:
            ((tcb_t *)xTaskToNotify)->xNotificationValue |= xValue;
        break;

        case eIncrement:
            ((tcb_t *)xTaskToNotify)->xNotificationValue++;
        break;

        case eSetValueWithOverwrite:
            ((tcb_t *)xTaskToNotify)->xNotificationValue = xValue;
        break;

        default:
        break;
    }

    // Put in ready state
    ((tcb_t *)xTaskToNotify)->xState = TASK_READY;

    // Enable interrupts
    portEXIT_CRITICAL();

    return pdTRUE;
}


void vTaskNotifyFromISR( TaskHandle_t xTaskToNotify,
                                uint16_t xValue,
                                eNotifyAction eAction,
                                UBaseType_t *pxHigherPriorityTaskWoken )
{
    // Cast to tcb
    pxAuxTCB = (tcb_t *)xTaskToNotify;

    // Assertions
    configASSERT(xTaskToNotify != NULL);
    configASSERT(pxAuxTCB->xState == TASK_BLOCKED);
    configASSERT(pxAuxTCB->uxId < uxCurrentNumberOfTasks);
    configASSERT(pxAuxTCB->uxId != pxCurrentTCB->uxId);

    // Notify
    switch(eAction)
    {
        case eSetBits:
            pxAuxTCB->xNotificationValue |= xValue;
        break;

        case eIncrement:
            pxAuxTCB->xNotificationValue++;
        break;

        case eSetValueWithOverwrite:
            pxAuxTCB->xNotificationValue = xValue;
        break;

        default:
        break;
    }

    pxAuxTCB->xState = TASK_READY;
    if( pxAuxTCB->uxPriority >= pxCurrentTCB->uxPriority )
    {
        // Suspend current task
        pxCurrentTCB->xState = TASK_READY;

        // Change current task to the task to notify
        pxCurrentTCB = pxAuxTCB;
        pxCurrentTCB->xState = TASK_RUNNING;

        // Set true
        *pxHigherPriorityTaskWoken = pdTRUE;
    }
}












/* Private reference functions -----------------------------------*/
void vTaskSwitchContext(void)
{
    // Suspend current task
#if configUSE_PREEMPTION == (1)
    pxAuxTCB = pxTCBList;
    UBaseType_t xHighestPriority = pxCurrentTCB->uxPriority;
    if(pxCurrentTCB->xState != TASK_READY)
    {
        xHighestPriority = configIDLE_PRIORITY;
    }

    while(pxAuxTCB != NULL)
    {
        // Check timeout
        if(pxAuxTCB->xState == TASK_BLOCKED)
        {
            if(xTickCount >= pxAuxTCB->xTimeToWake)
            {
                pxAuxTCB->uxStatus |= tskTIMEOUT_FLAG;
                pxAuxTCB->xState = TASK_READY;
            }
        }

        // Check priority for ready tasks
        if(pxAuxTCB->xState == TASK_READY)
        {
            if(pxAuxTCB->uxPriority > xHighestPriority)
            {
                pxCurrentTCB = pxAuxTCB;
                xHighestPriority = pxCurrentTCB->uxPriority;
            }
        }
        pxAuxTCB = pxAuxTCB->pxNextTCB;
    }
#else
    uint8_t xPass = 0;
    do
    {
        pxCurrentTCB = pxCurrentTCB->pxNextTCB;
        // Check for delayed
        if(pxCurrentTCB->xState == TASK_DELAYED) {
            if(xTickCount >= pxCurrentTCB->xTimeToWake) {
                pxCurrentTCB->xState = TASK_READY;
                break;
            }
        }
        xPass++;
    } while(pxCurrentTCB->xState != TASK_READY && xPass < uCurrentNumberOfTasks);
#endif

    if(pxCurrentTCB->xState != TASK_READY)
    {
        pxCurrentTCB = (tcb_t *)xIdleTaskHandle;
    }
    pxCurrentTCB->xState = TASK_RUNNING;

    //!< New implementation
    portRESTORE_CONTEXT();
}

static void vTaskRun(tcb_t *pxTaskToRun)
{
    // The sr needs saving before it is modified.
    portSAVE_CPU_STATUS();

    portSAVE_CONTEXT();
    pxCurrentTCB->xState = TASK_READY;

    // Switch task
    pxCurrentTCB = pxTaskToRun;
    pxCurrentTCB->xState = TASK_RUNNING;
    portRESTORE_CONTEXT();
}






/* Interrupt handler ------------------------------*/
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Systick_IRQHandler(void)
{
    // Save current context
    asm(" pop R15\n");  // Seems like R15 is always pushed onto the stack on every interrupt
                        // This is because of xTickCount or pxCurrentTCB or call to scheduler
    portSAVE_CONTEXT();

    // Increment systick
    ++xTickCount;
    pxCurrentTCB->xState = TASK_READY;

    // Call scheduler from ISR
    portCALL_SCHEDULER_FROM_ISR();
}

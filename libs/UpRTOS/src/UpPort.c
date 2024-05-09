/*
 * UpTask.c
 *
 *  Created on: 12 mar 2024
 *      Author: User123
 */


/* Private includes -----------------------------------*/
#include <UpRTOS/UpPortable.h>

/* Private defines ---------------------------------------------------*/

/* Private macros ----------------------------------------------------*/

/* Private typedefs --------------------------------------------------*/



/* Private prototype function ----------------------------------------*/


/* Private variables -------------------------------------------------*/



/* Reference function ------------------------------------------------*/
/*!
 * @name xSystickConfig
 * @brief Configure Timer A1
 * @return Error
 */
//void vPortSetuptTimerInterrupt(void);
UBaseType_t xPortSetuptTimerInterrupt(void)
{
    configASSERT_RETURN(configCPU_CLOCK_HZ/configTICK_RATE_HZ <= UINT16_MAX, pdFALSE);

    // Reset Timer A1
    TA1CTL |= BIT2;
    // Clock source: TA1CLKSRC = SMCLK (16 MHz in this example)
    TA1CTL |= BIT9;
    // Input Divider: DIV=1 -> Timer Clock [TA1CLK] = TA1CLKSRC/DIV = 16MHz/1 = 16MHz
    TA1CTL &= ~(BIT7 | BIT6);
    //TA1CTL = TASSEL_2 | ID_3 | MC_1 | TACLR;
    // Period = [1/TA1CLK]*[TA1CCR0 + 1]
    TA1CCR0 = (uint16_t)(configCPU_CLOCK_HZ/configTICK_RATE_HZ - 1);

    // Interrupt
    TA1CCTL0 |= BIT4;   // Enable TA0CCR0 interrupt
    TA1CCTL0 &= ~BIT0;  // Clear CC0IFG flag

    // Count mode: Up (This start timer)
    TA1CTL |= BIT4;

    return pdTRUE;
}





/* Private reference functions -----------------------------------*/
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
    *pxTopOfStack-- = (StackType_t)( pxCode );      // PC
    *pxTopOfStack-- = portINT_ENABLED_MASK;        // Status Register
    *pxTopOfStack-- = 0xFFFF;                       // R15
    *pxTopOfStack-- = 0xEEEE;                       // R14
    *pxTopOfStack-- = 0xDDDD;                       // R13
    *pxTopOfStack-- = (StackType_t)pvParameters;    // R12 (Input Arguments)
    *pxTopOfStack-- = 0xBBBB;                       // R11
    *pxTopOfStack-- = 0xAAAA;                       // R10
    *pxTopOfStack-- = 0x9999;                       // R9
    *pxTopOfStack-- = 0x8888;                       // R8
    *pxTopOfStack-- = 0x7777;                       // R7
    *pxTopOfStack-- = 0x6666;                       // R6
    *pxTopOfStack-- = 0x5555;                       // R5
    *pxTopOfStack   = 0x4444;                       // R4

    return pxTopOfStack;
}



/* Interrupt handler ------------------------------*/

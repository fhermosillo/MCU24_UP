/* Includes ----------------------------------------------------*/
#include <msp430.h> 

/* Defines -----------------------------------------------------*/
// I/O's
#define TA0_OUT0   (BIT1)
#define TA0_OUT1   (BIT2)
// Constants
#define configCPU_CLOCK_HZ  (1000000UL)
#define configPWM_CLOCK_HZ  (1000UL)

/**------------------------------------------------------------------*
 * main.c
 *-------------------------------------------------------------------*
 */
void main(void)
{
    /* WDT Config ----------------------------------------------------*/
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/* BCM Config ----------------------------------------------------*/
	// MCLK at 1MHz
	BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
	// SMCLK is MCLK
	// ACLK is external clock

	/* GPIO Config ---------------------------------------------------*/
	// P1.2 = TA0.1
	P1DIR  |= TA0_OUT1;
	P1SEL  |= TA0_OUT1;
	P1SEL2 &= ~TA0_OUT1;

	/* TIMER Config --------------------------------------------------*/
	// Reset Timer A0
    TA0CTL |= BIT2;
    // Clock source: TA0CLKSRC = SMCLK (1 MHz in this example)
    TA0CTL |= BIT9;
    // Input Divider: DIV=1
    TA0CTL &= ~(BIT7 | BIT6);
    // Timer Clock [TA0CLK] = TA0CLKSRC/DIV = 1MHz/1 = 1MHz

    /* PWM Config ----------------------------------------------------*/
    // PWM Period = 1ms
    TA0CCR0 = (configCPU_CLOCK_HZ / configPWM_CLOCK_HZ) - 1;
    // PWM Duty Cycle (DC) [0,TA0CCR0]
    TA0CCR1 = 200;
    // PWM Output: TA0.1 (available in P1.2 or P1.6)
    TA0CCTL1 |= OUTMOD_7;
    // Counter mode: UP (This start timer)
    TA0CTL |= BIT4;

    /* SuperLoop -----------------------------------------------------*/
    while(1)
    {
    }
}

/* END OF FILE -------------------------------------------------------*/

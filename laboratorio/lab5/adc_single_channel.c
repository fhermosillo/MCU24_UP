/* Includes ---------------------*/
#include <msp430.h> 
#include <stdint.h>

/* Defines -----------------------*/
// BOARD defines
#define LED_RED     BIT0
#define LED_GREEN   BIT6
#define BUTTON      BIT3
#define CPU_F       ((unsigned long)(8000000))
// I/O defines
// ADC defines
#define ADC_MAX_VAL     (1023)
#define ADC_VREF_MAX    (3.3F)
#define ADC_VREF_MIN    (0.0F)
// USER define
#define BIT15   BITF
#define BIT14   BITE
#define BIT13   BITD
#define BIT12   BITC
#define BIT11   BITB
#define BIT10   BITA

/* Macros ------------------------*/
// SYSTEM defines
#define MS_TO_CYCLES(MS)    (unsigned long)((CPU_F/1000) * (MS))
#define US_TO_CYCLES(US)    (unsigned long)((CPU_F/1000000) * (US))

/* Global variables --------------*/


/* Private prototype function ----*/

/**! ------------------------------
 * main.c
 *
 * @brief   Read analog input A1
 *          in single-channel single
 *          conversion mode.
 * --------------------------------
 */
void main(void)
{
    /* Local variables */
    unsigned int adc_value = 0x0000;

    /* Watchdog Timer config */
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    /* Clock config */
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL  = CALDCO_8MHZ;

    /* I/O config */
    P1SEL  = 0x00;                  // All P1 as GPIO
    P1SEL2 = 0x00;                  // All P1 as GPIO
    P1OUT  = 0x00;                  // Clear P1
    // GPIO
    P1DIR |= LED_GREEN | LED_RED;   // LED_RED, LED_GREEN as output
    P1DIR &= ~BUTTON;               // BUTTON as input
    // Pull-up/down
    P1REN |= BUTTON;                // Rpull enable on BUTTON
    P1OUT |= BUTTON;                // Pull-up

    /* ADC10 config */
    // Enable analog input A5
    ADC10AE0 |= BIT5;
    // Control Register 1
    ADC10CTL1 |= BIT7 | BIT6 | BIT5;    // Clock div = 8
    ADC10CTL1 |= BIT4 | BIT3;           // Clock source: SMCLK
    // ADC10CLK = SMCLK/8
    // Control Register 0
    ADC10CTL0 |= BIT12; // Sample&hold time = 16 * ADC10CLK
    ADC10CTL0 |= BIT4;    // Power-on ADC10

    /* Interrupts */

    // Bucle principal
    while(1)
    {
        // Start a new ADC conversion
        ADC10CTL1 &= ~(0xF000);         // Clear BIT15 - BIT12
        ADC10CTL1 |= INCH_5;            // Set analog channel 5
        ADC10CTL0 |= BIT1 | BIT0;       // Enable conversion and start conversion
        while(ADC10CTL0 & ADC10BUSY);   // Wait until conversion is done
        adc_value = ADC10MEM;           // Read converted value
        ADC10CTL0 &= ~ENC;              // Disable conversion

        // Compare to Vin=Vr(+)/2 voltage --> Nadc = 1023*[Vin - Vr(-)]/[Vr(+) - Vr(-)]
        const unsigned int uiCompareValue = ADC_MAX_VAL * (ADC_VREF_MAX/2 - ADC_VREF_MIN)/(ADC_VREF_MAX - ADC_VREF_MIN);
        if(adc_value > uiCompareValue)
        {
            P1OUT |= LED_RED;
            P1OUT &= ~LED_GREEN;
        }
        else
        {
            P1OUT |= LED_GREEN;
            P1OUT &= ~LED_RED;
        }
    } // End of while
}



/* Interrupt service routine ---------------------*/

/* Private reference function --------------------*/

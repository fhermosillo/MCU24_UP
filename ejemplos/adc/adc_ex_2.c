#include <msp430.h> 

#define LED_RED     (BIT0)
#define LED_GREEN   (BIT6)

unsigned int uiAcdData[3] = {0, 0, 0};
/**
 * main.c
 *
 * @brief: Multi-channel single conversion
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    ADC10AE0 |= BIT3 | BIT2 | BIT1;                                   // Enable A3, A2, and A1
    //ADC10CTL1 |= BIT1;     // Single sequence of channels (One-shot): From A3 to A1
    ADC10CTL1 = INCH_3 | CONSEQ_1;                      // Single sequence of channels (One-shot): From A3 to A1
    ADC10CTL0 = ADC10SHT_2 | MSC | ADC10ON;   // 16 x ADC10CLK cycles
                                                        // Multi-sample conversion
                                                        // Enable ADC10 module
                                                        // Enable interrupts
    ADC10DTC1 = 3;                                      // 3 conversions
    P1DIR |= LED_RED;                                   // Set P1.0 output

    while(1)
    {
        ADC10CTL0 &= ~ENC;                      // Disable conversion
        while (ADC10CTL1 & ADC10BUSY);          // Wait until ADC10 core is done

        ADC10SA = (unsigned int)uiAcdData;      // Data buffer start address
        P1OUT |= LED_RED;                       // P1.0 = 1
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        while (ADC10CTL1 & ADC10BUSY);          // Wait until ADC10 core i
        // At this point, the measurements of channels
        // A3 to A1 were acquired and stored in the
        // uiAcdData buffer in the following order:
        //uiAcdData[0] -> A3
        //uiAcdData[1] -> A2
        //uiAcdData[2] -> A1
        // Hence, we can perform some computations over
        // these data
        uiAcdData[0]; // A3
        uiAcdData[1]; // A2
        uiAcdData[2]; // A1
    }
}

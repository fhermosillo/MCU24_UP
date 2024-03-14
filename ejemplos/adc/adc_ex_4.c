#include <msp430.h> 


unsigned int uiAcdData[5]; // ADC samples for channels A4,A3,A2,A1,A0 are stored here

#define LED_RED     (BIT0)
#define LED_GREEN   (BIT6)

/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//reading voltage
	ADC10AE0 = BIT4 | BIT3 | BIT2 | BIT1;   // Channels A4, A3, A2, A1, and A0 are configured as analog inputs
	ADC10CTL1 = INCH_4 | CONSEQ_1;          // Channel A4 is multiplexed
	                                        // Sampling channels A4/A3/A2/A1/A0 (one-shot)
	 //2.5 reference voltage, 16 cycles, Multi start conversion
	ADC10CTL0 = REF2_5V | ADC10SHT_2 | MSC | REFON | ADC10ON; //  | ADC10IE
	ADC10DTC1 = 0x0005; // 5 conversions
	P1DIR |= 0x01; // Set P1.0 output

	while( 1 )
	{
	    // Wait if ADC10 core is active
        ADC10CTL0 &= ~ENC;
        while (ADC10CTL1 & BUSY);

        // ADC conversion
        ADC10SA = (int)uiAcdData;       // ADC Data buffer start
        ADC10CTL0 |= ENC | ADC10SC;     // Starting sampling and conversion
        while(ADC10CTL0 & ADC10BUSY);   // Waits until the conversion is done
        //__bis_SR_register(CPUOFF + GIE); // LPM0, ADC10_ISR will force exit
        P1OUT &= ~LED_RED; // Clear P1.0 LED off
	}
}


/*#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    //code from reading from res and sending it to AP
    __bic_SR_register_on_exit(CPUOFF); // Clear CPUOFF bit from 0(SR)
}
*/

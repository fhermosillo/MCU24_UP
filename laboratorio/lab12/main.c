#include <msp430.h> 
#include <stdint.h>


#define LED_RED     BIT0
#define LED_GREEN   BIT6
#define BUTTON      BIT3

// Since ulAdcSample can be modified externally (e.g., in an interrupt)
// it should be defined as a volatile variable
volatile uint16_t ulAdcSample = 0;

/**
 * main.c
 */
void main(void)
{
    /* WDT */
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/* BCM Clock Module */
	BCSCTL1 = CALBC1_16MHZ; // 16MHz
    DCOCTL = CALDCO_16MHZ;  // 16MHz
    BCSCTL3 |= LFXT1S_2;    // ACLK = VLOCLK @12KHz

    /* GPIO */
    P1OUT = 0x00;
    P1SEL = 0x00;
    P1SEL2 = 0x00;
    P1DIR |= LED_RED | LED_GREEN;

    /* ADC */
    ADC10CTL1 = SHS_1 | CONSEQ_2 | INCH_1;    // TA1 trigger Sample&Hold Start (SHI)
                                              // Repeat single channel
                                              // A1 is multiplexed

    // VREF = ON, REF = 1.5V, Enable ADC, enable ADC interrupts
    ADC10CTL0 = SREF_1 | ADC10SHT_2 | REFON | ADC10ON | ADC10IE;

    // Delay to allow VREF to settle (30 us)
    __delay_cycles(30);


    // Channel A1 is configured as analog input
    ADC10AE0  |= BIT1;

    // ADC10 Enable Conversion
    ADC10CTL0 |= ENC;

    /* TIMER TA0 */
    // Timer Period: This is the sampling frequency Fs = 1000 Hz (sample rate: samples/second)
    // Or the sampling period Ts = 1/Fs = 1ms
    TA0CCR0 = 12-1;
    // TAxCCR1 PWM Duty Cycle 99 percent
    TA0CCR1 = 12-1;
    // TAxCCR1 set/reset: Every time TAxR>=TAxCCR1, TA0 = 1
    // otherwise, TA0 = 0. In this case, just TAxCCR0=TAxCCR1
    // It controls SHI signal
    TA0CCTL1 = OUTMOD_3;

    // TA0CLK = ACLK, DIV=1, UP mode
    TA0CTL = TASSEL_1 | MC_1;

    /* Global Interrupts */
    __enable_interrupt();


    /* Super-loop */
    uint16_t DACOUT = 0x0000;
    while(1)
    {
        // Sleep CPU until an interrupt wake-up
        __bis_SR_register(LPM3_bits);

        /*
         * Digital Signal Processing: @BEGIN
         */
        // Whatever mathematical transformation goes here
        DACOUT = ulAdcSample;
        /*
         * Digital Signal Processing: @END
         */

        // Digital to Analog Conversion
        P2OUT = (DACOUT >> 2); // Our DAC is 8-bits, hence the value to be
                                    // converted must also be 8-bits.
    }
}


// ADC10 interrupt service routine: Called evvery 1 millisecond
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    // Debug: To check sampling period in this signal
    P1OUT ^= LED_RED;

    // Read converted value
    ulAdcSample = ADC10MEM;

    // Wake-up CPU
    __bic_SR_register_on_exit(LPM3_bits);
}

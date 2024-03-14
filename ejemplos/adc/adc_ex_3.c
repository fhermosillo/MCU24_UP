#include <msp430.h> 

#define LED_RED     (BIT0)
#define LED_GREEN   (BIT6)

/**
 * main.c
 *
 * @brief: ADC single-channel repeated conversions
 * performed every period of a timer. It also employs
 * interrupts and low-power mode to suspend the CPU
 * execution until the ADC interrupt wake-ups the CPU.
 */
#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    ADC10CTL1 = SHS_1 | CONSEQ_2 | INCH_1;    // TA1 trigger Sample&Hold Start (SHI)
                                              // Repeat single channel
                                              // A1 is multiplexed

    //
    ADC10CTL0 = SREF_1 | ADC10SHT_2 | REFON | ADC10ON | ADC10IE;

    /* Delay to allow VREF to settle (30 us) */
    // This can be software controlled by __delay_cyles( x ),
    // where x is the number of cycles for 30us
    TACCR0 = 30;                              // Delay to allow Ref to settle (30 us)
    TACCTL0 |= CCIE;                          // Capture&Compare 0 interrupt.
    TACTL = TASSEL_2 + MC_1;                  // TACLK = SMCLK, Up mode.
    __bis_SR_register(LPM0_bits | GIE);       // Disable CPU until an interrupt wakes-up the CPU
                                              //  Also enable global interrupts.
    TACCTL0 &= ~CCIE;                         // Disable timer Interrupt: At this point the settle
                                              // time has been reached, that is, the timer interrupt
                                              // has been executed
    __disable_interrupt();

    /* Configure GPIOs */
    P1DIR |= LED_RED;                         // Set P1.0 output

    /* Config ADC */
    ADC10CTL0 |= ENC;                         // ADC10 Enable Conversion
    ADC10AE0 |= 0x02;                         // Channel A1 is configured as analog input

    /* Configure TIMER */
    // It will serve as a timing signal,
    // every time timer counter TA0xR reaches
    // the TAxCCR0 value, the ADC will convert
    // an analog value into a digital one
    TA0CCR0 = 2048-1;                          // Timer Period: This is the sampling time
    TA0CCR1 = 2047;                            // TAxCCR1 PWM Duty Cycle
    TA0CCTL1 = OUTMOD_3;                       // TAxCCR1 set/reset: Every time TAxR>=TAxCCR1, TA0 = 1
                                              // otherwise, TA0 = 0. In this case, just TAxCCR0=TAxCCR1
                                              // This controls SHI signal
    TA0CTL = TASSEL_1 | MC_1;                  // ACLK, up mode

    /* Enter low-power mode */
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3 w/ interrupts
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    if (ADC10MEM < 0x155)                     // ADC10MEM = A1 > 0.5V?
        P1OUT &= ~LED_RED;                    // Clear P1.0 LED off
    else
        P1OUT |= LED_RED;                     // Set P1.0 LED on
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
    TA0CCR0 = 0x0000;
    TA0CTL |= TACLR; // Disable timer
    TA0CTL = 0x0000;
    __bic_SR_register_on_exit(LPM0_bits);  // Exit LPM0 on return (for VREF settling time)
}

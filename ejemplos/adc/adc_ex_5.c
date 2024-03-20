/* Includes ----------------------------------------------------*/
#include <msp430.h> 
#include <stdint.h>

/* Defines -----------------------------------------------------*/
// I/O's
#define LED_RED         (BIT0)
#define LED_GREEN       (BIT6)
// ADC
#define TSENS_CHANNEL   (INCH_10)
// CPU
#define configCPU_CLOCK_HZ  (1000000UL)

/* Macros ------------------------------------------------------*/
#define pdUS_TO_CYCLES(xTimeInUs)   ( ( uint32_t  )( ( ( uint32_t ) ( xTimeInUs ) * ( uint32_t ) configCPU_CLOCK_HZ ) / ( uint32_t ) 1000000U ) )
#define pdMS_TO_CYCLES(xTimeInMs)   ( ( uint32_t  )( ( ( uint32_t ) ( xTimeInMs ) * ( uint32_t ) configCPU_CLOCK_HZ ) / ( uint32_t ) 1000U ) )

/* Variables ---------------------------------------------------*/
uint16_t u16Sample = 0x0000;
float fTemp = 0.0;


/**
 * main.c
 *
 * @brief: ADC single-channel repeated conversions
 * performed every period of a timer. It also employs
 * interrupts and low-power mode to suspend the CPU
 * execution until the ADC interrupt wake-ups the CPU.
 *
 * The Analog channel is A10, which is the internal
 * temperature sensor. To sample this channel, it
 * must be ensured that t_sh >= 30us
 */
int main(void)
{
    /* WDT Config ----------------------------------------------------*/
    WDTCTL = WDTPW | WDTHOLD;   // Stop WDT

    /* BCM Config ----------------------------------------------------*/
    // MCLK at 1MHz
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    // SMCLK is MCLK (1MHz)
    // ACLK is VLOCLK @12KHz
    BCSCTL3 |= LFXT1S_2;

    /* GPIO Config ---------------------------------------------------*/
    P1OUT = 0x00;
    P1SEL = 0x00;
    P1SEL2 = 0x00;
    P1DIR |= LED_RED | LED_GREEN;

    /* ADC Config ----------------------------------------------------*/
    // SHS_1: TA0.1 triggers S&H source
    // CONSEQ_2: Single-channel repeated
    // INCH_10: Internal temperature sensor channel
    // ADC10DIV_3: ADC10CLK = ADC10CLKSRC / 4
    // By default: ADC10CLKSRC=ADC10OSC ~ F_ADC10OSC = 6.3MHz [Max], 3.7MHz [Min]
    //             ADC10OSC = 1/6.3MHz
    // ADC10CLK = 1/(F_ADC10OSC/DIV) = 1/(6.3MHz/4) = 4/6.3MHz
    // Then, the sample&hold time t_sh = 64*ADC10CLK = 64 * (4/6.3MHz)
    //                                 =  40.63us > 30us
    ADC10CTL1 = SHS_1 | CONSEQ_2 | TSENS_CHANNEL | ADC10DIV_3;

    // SREF_1: VR(+)=VREF(+), VR(-)=Vss
    // ADC10SHT_3: S&H=64xADC10CLK
    // REFON: VREF=1.5, VREF ON
    // ADC10ON: ADC10 ON
    // ADC10IE: ADC10 interrupts ON
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON | ADC10IE;

    // Delay to allow internal reference voltage (VREF) settling (30 us)
    __delay_cycles(pdUS_TO_CYCLES(30));

    // ADC10 Enable Conversion
    ADC10CTL0 |= ENC;

    /* TIMER Config --------------------------------------------------*/
    TA0CCR0 = 1200-1;                        // Timer Period: This is the sampling time @1sample/second
    TA0CCR1 = 1200-1;                        // TAxCCR1 PWM Duty Cycle
    TA0CCTL1 = OUTMOD_3;                     // TAxCCR1 set/reset: Every time TAxR>=TAxCCR1, TA0.1 = 1
                                             // otherwise1, TA0.1 = 0. Here, TAxCCR0=TAxCCR1
                                             // This serves as the Sample&Hold Init (SHI) signal
    TA0CTL = TASSEL_1 | MC_1;                // ACLK, DIV=1, up mode
    __enable_interrupt();

    /* SuperLoop -----------------------------------------------------*/
    while(1)
    {
        // Enter to low-power mode
        __bis_SR_register(LPM3_bits);

        // Compute the temperature using the Vtemp
        //Vtemp = ADC_VALUE * (VR(+) - VR(-))/1023 - VR(-)
        float Vtemp=((float)u16Sample/1023.0F)*(1.5F);
        // T(°C) = (Vtemp - 0.986)/ 0.00355
        fTemp=(Vtemp - 0.986F)/0.00355F;
    }


}

/* IRQs --------------------------------------------------------*/
// ADC10
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_IRQHandler(void)
{
    // For debugging
    P1OUT ^= LED_RED;

    // Copy data to buffer
    u16Sample = ADC10MEM;

    // Wake-up from Low-power mode
    __bic_SR_register_on_exit(LPM3_bits);
}


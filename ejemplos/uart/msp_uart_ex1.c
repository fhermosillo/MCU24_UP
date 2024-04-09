/**
  ******************************************************************************
  * @file       main.c
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the main function for the Universal Serial
  *             Communication Interface (USCI) example configured as UART.
  *
  * @notes      It uses the serial to usb converted module embedded into the
  *             launchpad board, therefore, TXD & RXD jumpers must be rotated 90°
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

/* Includes ------------------------------------------------------------------*/
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>


/* Private constants ---------------------------------------------------------*/
#define configMCLK_CLOCK_HZ     (1000000UL)
#define configSMCLK_CLOCK_HZ    configMCLK_CLOCK_HZ
#define configBRCLK_CLOCK_HZ    configSMCLK_CLOCK_HZ

/* Private macro -------------------------------------------------------------*/
#define pdMS_TO_CYCLES(ulTimeInMs)  ((configMCLK_CLOCK_HZ/1000UL)*ulTimeInMs)
#define pdUS_TO_CYCLES(ulTimeInUs)  ((configMCLK_CLOCK_HZ/1000000UL)*ulTimeInUs)

#define HAL_DelayMs(ulTimeInMs)    __delay_cycles(pdMS_TO_CYCLES(ulTimeInMs))
#define HAL_DelayUs(ulTimeInUs)    __delay_cycles(pdUS_TO_CYCLES(ulTimeInUs))


/* Private functions ---------------------------------------------------------*/
void HAL_UART_Write(const unsigned char ucByteToWrite);
void HAL_UART_Read(unsigned char *ucByteToRead);
void HAL_UART_Puts(const unsigned char *pucStreamIn, unsigned char ucStreamLength);
void HAL_UART_Gets(unsigned char *pucStreamOut, unsigned char ucStreamLength);

/* Private variables ---------------------------------------------------------*/
static volatile char data;

/* Main program --------------------------------------------------------------*/
void main(void)
{
    /* WDT config */
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT

    /* BCM config */
    BCSCTL1 = CALBC1_1MHZ;  // Set DCO to 1MHz
    DCOCTL  = CALDCO_1MHZ;  // Set DCO to 1MHz

    /* GPIO config */
    P1DIR |= BIT0 + BIT6;   // Set the LEDs on P1.0, P1.6 as outputs
    P1OUT = 0x00;           // Set P1.0
    // UART
    P1SEL  = BIT1 | BIT2 ; // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 | BIT2 ; // P1.1 = RXD, P1.2=TXD

    /* UART config */
    // USCI logic held in reset state
    UCA0CTL1 = BIT0;

    // USCI Config
    UCA0CTL1 |= BIT7 | BIT6;   // SMCLK
    UCA0CTL0 = 0x00;
    /*
     * BaudRate = 9600, No oversampling
     * #1 BRCLK prescaler
     *      N = BRCLK/ BaudRate = 1,000,000/9600 = 104.17
     * #2 Prescaler setup
     * a) No Oversampling
     *      1. UCA0BR = int(N) = 104
     *      2. UCA0MCTL.BRS = round((N - int(N))*8)
     * b) Oversampling
     *      1. UCA0BR = int(N/16)
     *      2. UCA0MCTL.BRF = round((N/16 - int(N/16))*16)
     */
    // 104 = 0x0068
    // UCA0BR(16-BITS)
    UCA0BR0 = 0x68;          // 1MHz 9600
    UCA0BR1 = 0x00;          // 1MHz 9600
    UCA0MCTL = BIT1;      // Modulation UCBRSx<3:1>
    // USCI reset released for operation
    UCA0CTL1 &= ~BIT0;

    // Send data
    while(1)
    {
      HAL_UART_Puts("#MSP430>>Hola Mundo\n\r", 13);
      HAL_DelayMs(1000);
    }
}

/* Interrupt Service Routines ------------------------------------------------*/

/* Reference functions -------------------------------------------------------*/
void HAL_UART_Write(const unsigned char ucByteToWrite)
{
    // Wait for TX buffer to be ready for new data
    while( !(IFG2 & UCA0TXIFG) ) { }
    UCA0TXBUF = ucByteToWrite;
}

void HAL_UART_Read(unsigned char *ucByteToRead)
{
    // Wait for RX buffer to be ready for new data
    while( !(IFG2 & UCA0RXIFG) ) { }
    *ucByteToRead = UCA0RXBUF;
}

// Send number of bytes Specified in ucStreamLength in the array at using the hardware UART0
void HAL_UART_Puts(const unsigned char *pucStreamIn, unsigned char ucStreamLength)
{
    while(ucStreamLength--)
    {
        // Wait for TX buffer to be ready for new data
        while( !(IFG2 & UCA0TXIFG) ) { }
        UCA0TXBUF = *pucStreamIn++;
    }
}

void HAL_UART_Gets(unsigned char *pucStreamOut, unsigned char ucStreamLength)
{
    while(ucStreamLength--)
    {
        // Wait for RX buffer to be ready for new data
        while( !(IFG2 & UCA0TXIFG) ) { }
        *pucStreamOut++ = UCA0RXBUF;
    }
}

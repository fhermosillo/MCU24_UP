/**
  ******************************************************************************
  * @file       main.c
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file provides an example for the Universal Serial
  *             Communication Interface (USCIA0) configured as SPI in Mode 0.
  *
  *             There are four primary SPI modes according to two clock behaviors:
  *             - Clock phase (CKPH): The clock state when read/write
  *             - Clock polarity (CKPL): The clock state when communication is IDLE
  *             - In TI's MSP430, the CKPH is inverted
  *
  *             The four modes are:
  *             1. SPI_MODE_0 (CKPH = 1, CKPL = 0): Clock idles on 0, and reads first
  *                                                 on rising edge, then writes on
  *                                                 falling edge
  *             2. SPI_MODE_1 (CKPH = 0, CKPL = 0): Clock idles on 0, and writes first
  *                                                 on rising edge, then  reads on
  *                                                 falling edge
  *             3. SPI_MODE_2 (CKPH = 1, CKPL = 1): Clock idles on 0, and reads first
  *                                                 on rising edge, then writes on
  *                                                 falling edge
  *             4. SPI_MODE_3 (CKPH = 0, CKPL = 1): Clock idles on 0, and reads first
  *                                                 on rising edge, then writes on
  *                                                 falling edge
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
#include <stdlib.h>

/* Private constants ---------------------------------------------------------*/
// Clocks
#define configMCLK_CLOCK_HZ     (1000000UL)
#define configSMCLK_CLOCK_HZ    configMCLK_CLOCK_HZ
#define configBRCLK_CLOCK_HZ    configSMCLK_CLOCK_HZ
// SPI I/Os
#define SPI1_MISO_Pin BIT1
#define SPI1_MOSI_Pin BIT2
#define SPI1_SCK_Pin  BIT4
//#define SPI1_STE_Pin  BIT5    /* Not used in this example */
#define SPI_CS_Pin      BIT5    /* Needed to enable a specific SPI slave, not driven by hardware */
// External peripheral I/Os
#define LED_RED_Pin     BIT0
#define LED_GREEN_Pin   BIT6
#define BUTTON_Pin      BIT3

// SPI constants
#define SPI_MODE_0      (0x80)
#define SPI_MODE_1      (0x00)
#define SPI_MODE_2      (0xC0)
#define SPI_MODE_3      (0x40)
#define SPI_MODE_MASK   (0xC0)

/* Private macro -------------------------------------------------------------*/
#define pdMS_TO_CYCLES(ulTimeInMs)  ((configMCLK_CLOCK_HZ/1000UL)*ulTimeInMs)
#define pdUS_TO_CYCLES(ulTimeInUs)  ((configMCLK_CLOCK_HZ/1000000UL)*ulTimeInUs)

#define HAL_Delay_MS(ulTimeInMs)    __delay_cycles(pdMS_TO_CYCLES(ulTimeInMs))
#define HAL_Delay_US(ulTimeInUs)    __delay_cycles(pdUS_TO_CYCLES(ulTimeInUs))
// SPI
#define HAL_SPI_CS_On()     P1OUT |= SPI_CS_Pin
#define HAL_SPI_CS_Off()    P1OUT &= ~SPI_CS_Pin;
#define HAL_SPI_nCS_On()    HAL_SPI_CS_Off()
#define HAL_SPI_nCS_Off()   HAL_SPI_CS_On()

/* Private functions ---------------------------------------------------------*/
void HAL_SPI_Init(void);
void HAL_SPI_Transmit(const uint8_t *pData, uint16_t Size);
void HAL_SPI_Receive(uint8_t *pData, uint16_t Size);
void HAL_SPI_Set_Mode(uint8_t mode);
void HAL_SPI_TransmitReceive(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

/* Private variables ---------------------------------------------------------*/

/* Main program --------------------------------------------------------------*/
void main(void)
{
    /* WDT config */
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT

    /* BCM config */
    BCSCTL1 = CALBC1_1MHZ;  // Set DCO to 1MHz
    DCOCTL  = CALDCO_1MHZ;  // Set DCO to 1MHz

    /* GPIO config */
    // P1 reset
    P1SEL = 0x00;
    P1SEL2 = 0x00;
    P1OUT = 0x00;
    P1DIR = 0x00;
    P1REN = 0x00;
    // P2 reset
    P2OUT = 0x00;
    P2DIR = 0xFF;
    // P3 reset
    P3OUT = 0x00;
    P3DIR = 0xFF;
    // led output
    P1DIR |= LED_RED_Pin + LED_GREEN_Pin;
    // button input
    P1DIR  &= ~BUTTON_Pin;
    P1REN |= BUTTON_Pin;
    P1OUT &= ~BUTTON_Pin;
    // spi input/output
    P1DIR  |= SPI_CS_Pin;   // The Slave Select operation on the MSP430
                            // must be performed using GPIOs
    P1SEL  |= SPI1_MISO_Pin | SPI1_MOSI_Pin | SPI1_SCK_Pin;
    P1SEL2 |= SPI1_MISO_Pin | SPI1_MOSI_Pin | SPI1_SCK_Pin;

    /* SPI config */
    HAL_SPI_Init();

    /* Super loop */
    uint8_t ucSpiBuffer[4] = {0x01,0x02,0x03,0x04};
    while(true)
    {
        // Neg Chip Select: On
        HAL_SPI_nCS_On();

        // Transmit and receive data over the SPI bus
        HAL_SPI_TransmitReceive(ucSpiBuffer, ucSpiBuffer, 4);

        // Neg Chip Select: Off
        HAL_SPI_nCS_Off();

        HAL_Delay_MS(1000);
    }
}


/* Interrupt Service Routines ------------------------------------------------*/


/* Reference functions -------------------------------------------------------*/
void HAL_SPI_Init(void)
{
    // USCI logic held in reset state
    UCA0CTL1 = UCSWRST;
    // SPI_CLOCK_MODE_0
    // - UCCKPH = 1
    // - UCCKPL = 0 (default)
    // UCMSB: MSB first
    // 8-bit data (default)
    // UCMST: Master mode
    // UCMODE_0: 3-pin SPI
    // UCSYNC: Synchronous mode
    UCA0CTL0 = UCCKPH | UCMSB | UCMST | SPI_MODE_0 | UCSYNC;

    // BRCLK = SMCLK = 1MHz
    UCA0CTL1 |= UCSSEL_2;

    // Clock frequency
    // BRCLK / UCA0BR = 1MHz/2 = 500KHz
    UCA0BR0  = 0x02;
    UCA0BR1  = 0x00;
    UCA0MCTL = 0x00; // No modulation

    // USCI reset released for operation
    UCA0CTL1 &= ~UCSWRST;
}

void HAL_SPI_Set_Mode(uint8_t mode)
{
    // Mode
    UCA0CTL0 &= ~SPI_MODE_MASK;
    UCA0CTL0 |= mode;
}

void HAL_SPI_Transmit(const uint8_t *pData, uint16_t Size)
{
    if(pData==NULL) return;

    // Transmit data
    while(Size--)
    {
        while (!(IFG2 & UCA0TXIFG)); // USCI_A0 TX buffer ready?
        UCA0TXBUF = *pData++; // Send data over SPI to Slave
    }
}
void HAL_SPI_Receive(uint8_t *pData, uint16_t Size)
{
    if(pData==NULL) return;

    // Receive data
    while(Size--)
    {
        while (!(IFG2 & UCA0RXIFG)); // USCI_A0 RX Received?
        *pData++ = UCA0RXBUF; // Store received data
    }
}

void HAL_SPI_TransmitReceive(const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
    // Transmit&Receive data
    while(Size--)
    {
        // TX is always first received
        while (!(IFG2 & UCA0TXIFG)); // USCI_A0 TX ready
        if(pTxData) UCA0TXBUF = *pTxData++;

        while (!(IFG2 & UCA0RXIFG)); // New data RX Received on USCI_A0
        if(pRxData) *pRxData++ = UCA0RXBUF;
    }
}

/**
  ******************************************************************************
  * @file       i2c.c
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions definitions for the I2C
  *             module driver.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 Universidad Panamericana.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"
#include <stdlib.h>

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/


/* Reference functions -------------------------------------------------------*/
void HAL_I2C_Init(uint16_t prescaler)
{
    /* I2C I/O */
    P1SEL  |= I2C_SDA_Pin | I2C_SCL_Pin;    // SDA & SCL pin configuration
    P1SEL2 |= I2C_SDA_Pin | I2C_SCL_Pin;    // SDA & SCL pin configuration

    /* USCIB Config */
    // USCI logic held in reset state
    UCB0CTL1 = UCSWRST;
    // Control register 0
    // Master
    // USCIB0 I2C mode
    // Synchronous mode
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

    // Control register 1
    // BRCLK = SMCLK = 1MHz
    UCB0CTL1 |= UCSSEL_2;

    // Clock frequency
    UCB0BR0 = (uint8_t)(prescaler & 0x00FF);
    UCB0BR1 = (uint8_t)((prescaler>>8) & 0x00FF);

    // USCI reset released for operation
    UCB0CTL1 &= ~UCSWRST;
}

uint16_t HAL_I2C_Write(uint8_t Address, const uint8_t Data)
{
    uint16_t err = 0;

    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    UCB0CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    // Not acknowledge was received from slave?
    if (UCB0STAT & UCNACKIFG)
    {

        // Clear the interrupt flag
        UCB0STAT &= ~UCNACKIFG;

        err = UCNACKIFG;
    }
    else
    {
        // Write a byte to the TX buffer
        UCB0TXBUF = Data;

        // Wait until the byte is transmitted
        while(!(IFG2 & UCB0TXIFG));
    }

    // Send stop bit
    UCB0CTL1 |= UCTXSTP;

    // Wait until the stop bit is transmitted
    while (UCB0CTL1 & UCTXSTP);

    return err;
}

uint16_t HAL_I2C_Write_Multi(uint8_t Address, const uint8_t *pData, uint8_t Size)
{
    if(pData == NULL || !Size) return 1;

    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    UCB0CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // If no error and bytes left to send, transmit the data
    do
    {
        UCB0TXBUF = *pData++;
        while ( !(IFG2 & UCB0TXIFG) );
    }
    while(--Size);

    // Send the STOP bit
    UCB0CTL1 |= UCTXSTP;

    // Wait until the STOP bit is transmitted
    while((UCB0CTL1 & UCTXSTP));

    return 0;
}


uint16_t HAL_I2C_Request(uint8_t Address, uint8_t *pData, uint8_t Size)
{
    if(pData == NULL || !Size) return 1;

    // Clear UCR
    UCB0CTL1 &= ~UCTR;

    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set slave ADDRESS
    UCB0I2CSA = Address;

    // Generate START condition
    UCB0CTL1 |= UCTXSTT;

    // Wait for the start condition to be sent and ready to read flags
    while (UCB0CTL1 & UCTXSTT);

    // Receive the data until no more bytes left
    do
    {
        // Check if there is one byte left to receive
        if (Size == 1)
        {
            UCB0CTL1 |= UCTXNACK;   // Send NACK bit. Is it necessary?
            UCB0CTL1 |= UCTXSTP;    // Send stop bit
        }

        // Wait until a new data will be received
        while( !(IFG2 & UCB0RXIFG) );

        // Read from the RX buffer
        *pData++ = UCB0RXBUF;
    } while (--Size);

    // Wait for the STOP bit is transmitted
    while (UCB0CTL1 & UCTXSTP);

    return 0;
}

int HAL_I2C_Write_Reg(uint8_t Address, uint8_t RegAddress, uint8_t RegData)
{
    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    UCB0CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    if( HAL_I2C_Ack() ) return UCNACKIFG;

    UCB0TXBUF = RegAddress;

    while(!(IFG2 & UCB0TXIFG));

    UCB0TXBUF = RegData;

    while(!(IFG2 & UCB0TXIFG));

    UCB0CTL1 |= UCTXSTP;

    while((UCB0CTL1 & UCTXSTP));

    return 0;
}


int HAL_I2C_Read_Reg(uint8_t Address, uint8_t RegAddress, uint8_t *RegData)
{
    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    UCB0CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    if( HAL_I2C_Ack() ) return UCNACKIFG;

    UCB0TXBUF = RegAddress;

    while(!(IFG2 & UCB0TXIFG));

    // RX
    UCB0CTL1 &= ~UCTR;

    // Send restart condition
    UCB0CTL1 |= UCTXSTT;

    while((UCB0CTL1 & UCTXSTT));

    UCB0CTL1 |= UCTXSTP;

    while(!(IFG2 & UCB0RXIFG));

    *RegData = UCB0RXBUF;

    while((UCB0CTL1 & UCTXSTP));

    return 0;
}


int HAL_I2C_Ack(void)
{
    int err = 0;

    // Not acknowledge was received from slave?
    if (UCB0STAT & UCNACKIFG)
    {
        // Stop the I2C transmission
        UCB0CTL1 |= UCTXSTP;

        // Clear the interrupt flag
        UCB0STAT &= ~UCNACKIFG;

        err = UCNACKIFG;
    }

    return err;
}

int HAL_I2C_Connected(uint8_t Address)
{
    int Connection = 0;

    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set Slave address
    UCB0I2CSA = Address;

    // Send start, address, stop bits
    UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;

    // Wiat until
    while (UCB0CTL1 & UCTXSTP);

    if( !(UCB0STAT & UCNACKIFG) ) Connection = 1;
    UCB0STAT &= ~UCNACKIFG;

    return Connection;
}


int HAL_I2C_Read_Regs(uint8_t Address, uint8_t RegAddress, uint8_t *RegData, uint8_t Size)
{
    if(RegData == NULL || !Size) return -1;

    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    UCB0CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    if( HAL_I2C_Ack() ) return UCNACKIFG;

    UCB0TXBUF = RegAddress;

    while(!(IFG2 & UCB0TXIFG));

    // RX
    UCB0CTL1 &= ~UCTR;

    // Send restart condition
    UCB0CTL1 |= UCTXSTT;

    while((UCB0CTL1 & UCTXSTT));

    // Read data
    do
    {
        // Check if this is the last byte to read
        if(Size == 1)
        {
            UCB0CTL1 |= UCTXNACK;   // Send the NACK bit
            UCB0CTL1 |= UCTXSTP;    // Send the STOP bit
        }

        // Wait until there is a new byte to read
        while(!(IFG2 & UCB0RXIFG));

        // Read the newest RX byte
        *RegData++ = UCB0RXBUF;
    } while(--Size);

    // Wait until the STOP bit is transmitted
    while((UCB0CTL1 & UCTXSTP));

    return 0;
}

int HAL_I2C_Write_Regs(uint8_t Address, uint8_t RegAddress, const uint8_t *TxBuffer, uint8_t Size)
{
    // Wait until I2C STOP is sent
    while (UCB0CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    UCB0CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    if( HAL_I2C_Ack() ) return UCNACKIFG;

    // Send register address
    UCB0TXBUF = RegAddress;
    while(!(IFG2 & UCB0TXIFG));

    // Write data
    do
    {
        // Write the byte to transmit
        UCB0TXBUF = *TxBuffer++;

        // Wait until the byte is transmitted
        while(!(IFG2 & UCB0TXIFG));
    }
    while(--Size);

    // Send stop condition
    UCB0CTL1 |= UCTXSTP;
    while((UCB0CTL1 & UCTXSTP));

    return 0;
}

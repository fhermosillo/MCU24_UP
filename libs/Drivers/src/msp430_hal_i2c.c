/**
  ******************************************************************************
  * @file       msp430_hal_i2c.c
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions definitions for the HAL
  *             I2C module driver.
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
#include <Drivers/msp430_hal_i2c.h>
#include <Drivers/usci_reg.h>
#include <Drivers/msp430_hal_bcm.h>

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
#define I2C_STATE_IDLE      (0)
#define I2C_STATE_NACK      (1)
#define I2C_STATE_TX        (2)
#define I2C_STATE_RX        (3)
#define I2C_STATE_TIMEOUT   (4)

/* Private macros ------------------------------------------------------------*/
#define HAL_I2C_IT_START(__HANDLE__)    __HANDLE__->Instance->CTL1 |= UCTXSTT
#define HAL_I2C_IT_STOP(__HANDLE__)     __HANDLE__->Instance->CTL1 |= UCTXSTP
#define HAL_I2C_IT_SendAddress(__HANDLE__)
#define HAL_I2C_IT_Write(__HANDLE__,__DATA__)   __HANDLE__->Instance->TXBUF = __DATA__;
#define HAL_I2C_IT_Read(__HANDLE__)             __HANDLE__->Instance->RXBUF

/* Private functions ---------------------------------------------------------*/
/*
 * @name    HAL_I2C_Ack
 * @brief   Assert the acknowledge bit
 * @param   None
 * @return  I2C error {I2C_ERROR_NONE, I2C_ERROR_NACK}
 * @require HAL_I2C_Start
 */
static uint16_t HAL_I2C_Ack(I2C_TypeDef *I2Cx);

static void HAL_I2C_EV_ISR(void *argin);
static void HAL_I2C_ER_ISR(void *argin);

/* Reference functions -------------------------------------------------------*/
void HAL_I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct)
{
    assert_param(IS_I2C_ALL_INSTANCE(I2Cx));

    // USCI logic held in reset state
    I2Cx->CTL1 = UCSWRST;
    // Control register 0
    // Master/Slave mode
    // USCIB0 I2C mode
    // Synchronous mode
    I2Cx->CTL0 = UCMODE_3 | UCSYNC;
    if(I2C_InitStruct->Mode == I2C_MODE_MASTER) I2Cx->CTL0 |= UCMST;

    // Control register 1
    I2Cx->CTL1 |= I2C_InitStruct->ClkSrc << USCI_CTL1_SSEL_S;

    // Clock frequency
#if configHAL_CPU_HAVE_FPU == (1)
    uint32_t brclk_clock_hz = 0x00;
    switch(I2C_InitStruct->ClkSrc)
    {
        case I2C_CLKSRC_UCLK:
            brclk_clock_hz = configUSCI_CLOCK_HZ;
            break;

        case I2C_CLKSRC_ACLK:
            brclk_clock_hz = HAL_BCM_GetCLKFreq(BCM_ACLK);
            break;

        case I2C_CLKSRC_SMCLK:
            brclk_clock_hz = HAL_BCM_GetCLKFreq(BCM_SMCLK);
            break;

        default:
            brclk_clock_hz = 0;
            break;
    }

    // Assert baud rate
    assert_param(I2C_InitStruct->BaudRate < brclk_clock_hz);

    // Set prescaler
    uint16_t Prescaler = brclk_clock_hz/I2C_InitStruct->BaudRate;
    I2Cx->BR0 = Prescaler & 0x00FF;
    I2Cx->BR1 = (Prescaler >> 8) & 0x00FF;
#else
    I2Cx->BR0 = I2C_InitStruct->Prescaler & 0x00FF;
    I2Cx->BR1 = (I2C_InitStruct->Prescaler >> 8) & 0x00FF;
#endif
    // Setup own address
    if(I2C_InitStruct->OwnAddress)
    {
        if(I2C_InitStruct->OwnAddress > 255) I2Cx->CTL0 |= UCA10;
        UCB0I2COA = I2C_InitStruct->OwnAddress;
    }

    // USCI reset released for operation
    I2Cx->CTL1 &= ~UCSWRST;
}

uint16_t HAL_I2C_Begin(I2C_TypeDef *I2Cx, uint16_t Address)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(I2Cx), I2C_ERROR_INSTANCE);
    assert_param_ret(Address <= 1023, I2C_ERROR_OVADR);
    //assert_param_ret(!(I2Cx->STAT & UCBBUSY), I2C_ERROR_BUSY);

    // Wait until I2C STOP is sent
    while (I2Cx->CTL1 & UCTXSTP);

    // Check for 10-bits addressing mode
    if(Address > 255)
    {
        I2Cx->CTL0 |= UCSLA10;
    }

    // Set slave address
    UCB0I2CSA = Address;

    // Set transmitter
    // Generate start condition
    I2Cx->CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while(!(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    if( HAL_I2C_Ack(I2C1) ) return I2C_ERROR_NACK;

    return I2C_ERROR_NONE;
}

uint16_t HAL_I2C_Write(I2C_TypeDef *I2Cx, const uint8_t *pData, uint8_t Size)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(I2Cx), I2C_ERROR_INSTANCE);
    //assert_param_ret((I2Cx->STAT & UCBBUSY), I2C_ERROR_IDLE);

    int16_t err = I2C_ERROR_NONE;
    if(Size && pData)
    {
        // If no error and bytes left to send, transmit the data
        while(!err && Size--)
        {

            I2Cx->TXBUF = *pData++;
            while ( !(IFG2 & UCB0TXIFG) )
            {
                err = HAL_I2C_Ack(I2Cx);
                if (err)
                {
                    break;
                }
            }
        }
    }

    return err;
}


uint16_t HAL_I2C_Read(I2C_TypeDef *I2Cx, uint8_t *pData, uint8_t Size)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(I2Cx), I2C_ERROR_INSTANCE);
    //assert_param_ret((I2Cx->STAT & UCBBUSY), I2C_ERROR_IDLE);
    assert_param_ret(Size, I2C_ERROR_NONE);

    /* Send Restart condition */
    HAL_I2C_End(I2C1, I2C_RESTART_RX);
    /*I2Cx->CTL1 &= ~UCTR;
    I2Cx->CTL1 |= UCTXSTT;
    while((I2Cx->CTL1 & UCTXSTT));*/

    /* Receive the data */
    if(Size <= 1)
    {
        HAL_I2C_End(I2C1, I2C_STOP);
        //I2Cx->CTL1 |= UCTXSTP;
    }

    while(Size)
    {
        while(!(IFG2 & UCB0RXIFG));
        *pData++ = I2Cx->RXBUF;
        Size--;

        if (Size == 1)
        {
            HAL_I2C_End(I2Cx, I2C_STOP);
            //I2Cx->CTL1 |= UCTXSTP;
        }
    }

    return I2C_ERROR_NONE;
}

uint16_t HAL_I2C_Ack(I2C_TypeDef *I2Cx)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(I2Cx), I2C_ERROR_INSTANCE);

    int16_t err = I2C_ERROR_NONE;
    if (I2Cx->STAT & UCNACKIFG)
    {
        // Stop the I2C transmission
        I2Cx->CTL1 |= UCTXSTP;

        // Clear the interrupt flag
        I2Cx->STAT &= ~UCNACKIFG;

        err = I2C_ERROR_NACK;
    }

    return err;
}

uint16_t HAL_I2C_End(I2C_TypeDef *I2Cx, uint8_t BusRelease)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(I2Cx), I2C_ERROR_INSTANCE);
    assert_param_ret((I2Cx->STAT & UCBBUSY), I2C_ERROR_IDLE);

    if(BusRelease & BIT0)
    {
        // Toggle transmitter/receiver
        if(BusRelease & BIT1) I2Cx->CTL1 &= ~UCTR;
        else I2Cx->CTL1 |= UCTR;

        // Send start condition
        I2Cx->CTL1 |= UCTXSTT;

        // Wait for the start condition to be sent
        while(I2Cx->CTL1 & UCTXSTT);
    }
    else
    {
        // Send stop condition
        I2Cx->CTL1 |= UCTXSTP;

        // Wait for the stop condition to be sent
        while (I2Cx->CTL1 & UCTXSTP);
    }

    return I2C_ERROR_NONE;
}


uint16_t HAL_I2C_Transfer(I2C_TypeDef *I2Cx, I2C_MessageTypeDef *I2C_MessageStruct)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(I2Cx), I2C_ERROR_INSTANCE);
    assert_param_ret(!(I2Cx->STAT & UCBBUSY), I2C_ERROR_BUSY);

    // Start
    int16_t err = HAL_I2C_Start(I2Cx, I2C_MessageStruct->DevAddress);
    if(err != I2C_ERROR_NONE) return err;

    // Write
    if(I2C_MessageStruct->TxCount)
    {
        err = HAL_I2C_Write(I2Cx, (const uint8_t *)I2C_MessageStruct->TxBuffer, I2C_MessageStruct->TxCount);
        if(err != I2C_ERROR_NONE) return err;
    }

    // Read
    if(I2C_MessageStruct->RxCount)
    {
        HAL_I2C_Stop(I2Cx, I2C_RESTART_RX);
        err = HAL_I2C_Read(I2Cx, (uint8_t *)I2C_MessageStruct->RxBuffer, I2C_MessageStruct->RxCount);
        if(err != I2C_ERROR_NONE) return err;
    }
    else
    {
        HAL_I2C_Stop(I2Cx, I2C_STOP);
    }

    return I2C_ERROR_NONE;
}




uint16_t HAL_I2C_Connected(I2C_TypeDef *I2Cx, uint8_t Address)
{
    UCB0I2CSA = Address;
    IE2 &= ~(UCB0TXIE + UCB0RXIE);
    UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;
    while (UCB0CTL1 & UCTXSTP);

    return !(UCB0STAT & UCNACKIFG);
}


uint16_t HAL_I2C_Read_Regs(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t RegAddress, uint8_t *RegData, uint8_t len)
{
    // Wait until I2C STOP is sent
    while (I2Cx->CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    I2Cx->CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    if( HAL_I2C_Ack(I2C1) ) return I2C_ERROR_NACK;

    UCB0TXBUF = RegAddress;

    while(!(IFG2 & UCB0TXIFG));

    // RX
    I2Cx->CTL1 &= ~UCTR;

    // Send restart condition
    I2Cx->CTL1 |= UCTXSTT;

    while((I2Cx->CTL1 & UCTXSTT));

    // Read data
    if(len<=1)
    {
        I2Cx->CTL1 |= UCTXSTP;
    }

    while(len--)
    {
        while(!(IFG2 & UCB0RXIFG));
        *RegData++ = I2Cx->RXBUF;
    }

    while((I2Cx->CTL1 & UCTXSTP));

    return 0;
}

uint16_t HAL_I2C_Write_Regs(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t RegAddress, const uint8_t *TxBuffer, uint8_t len)
{
    // Wait until I2C STOP is sent
    while (I2Cx->CTL1 & UCTXSTP);

    // Set slave address
    UCB0I2CSA = Address;

    // Generate start condition
    I2Cx->CTL1 |= UCTR | UCTXSTT;

    // Wait for the start condition to be sent and ready to transmit flags
    while( !(IFG2 & UCB0TXIFG) );

    // Check acknowledge
    if( HAL_I2C_Ack(I2C1) ) return I2C_ERROR_NACK;

    // Send register address
    I2Cx->TXBUF = RegAddress;
    while(!(IFG2 & UCB0TXIFG));

    // Write data
    while(len--)
    {
        I2Cx->TXBUF = *TxBuffer++;
        while(!(IFG2 & UCB0TXIFG));
    }

    // Send stop condition
    I2Cx->CTL1 |= UCTXSTP;
    while((I2Cx->CTL1 & UCTXSTP));

    return 0;
}





uint16_t HAL_I2C_Alloc_ISR(I2C_HandleTypeDef *hi2c)
{
    // Assert input arguments
    assert_param_ret(IS_I2C_ALL_INSTANCE(hi2c->Instance), I2C_ERROR_INSTANCE);
    assert_param_ret(!(hi2c->Instance->STAT & UCBBUSY), I2C_ERROR_BUSY);
    assert_param_ret(hi2c->State==I2C_STATE_IDLE, I2C_ERROR_BUSY); // Redundat?

    // Lock handler
    __HAL_LOCK(hi2c);

    // Allocate I2C Event ISR
    usci_callback_config_t cfg ={
       .Argin = hi2c,
       .Callback = HAL_I2C_EV_ISR,
       .Mode = USCIB_MODE_I2C_TX,
       .Module = USCI_MODULE_B
    };
    uint16_t err = HAL_USCI_Intr_Alloc(&cfg) == USCI_ERROR_NONE ? HAL_OK : HAL_ERROR;

    // Allocate I2C Error ISR
    cfg.Callback = HAL_I2C_ER_ISR;
    cfg.Mode = USCIB_MODE_I2C_RX,
    err = HAL_USCI_Intr_Alloc(&cfg) == USCI_ERROR_NONE ? HAL_OK : HAL_ERROR;

    // Unlock handler
    __HAL_UNLOCK(hi2c);

    return err;
}


uint16_t HAL_I2C_Free_ISR(I2C_HandleTypeDef *hi2c)
{
    // Assert input arguments
    assert_param_ret(IS_I2C_ALL_INSTANCE(hi2c->Instance), I2C_ERROR_INSTANCE);
    assert_param_ret(!(hi2c->Instance->STAT & UCBBUSY), I2C_ERROR_BUSY);
    assert_param_ret(hi2c->State==I2C_STATE_IDLE, I2C_ERROR_BUSY); // Redundat?

    // Lock handler
    __HAL_LOCK(hi2c);

    // Allocate I2C ISR
    usci_callback_config_t cfg ={
       .Argin = hi2c,
       .Callback = HAL_I2C_EV_ISR,
       .Mode = USCIB_MODE_I2C_TX,
       .Module = USCI_MODULE_B
    };
    uint16_t err = HAL_USCI_Intr_Free(&cfg) == USCI_ERROR_NONE ? HAL_OK : HAL_ERROR;
    cfg.Mode = USCIB_MODE_I2C_RX,
    err = HAL_USCI_Intr_Free(&cfg) == USCI_ERROR_NONE ? HAL_OK : HAL_ERROR;

    // Unlock handler
    __HAL_UNLOCK(hi2c);

    return err;
}

uint16_t HAL_I2C_Transmit_IT(I2C_HandleTypeDef *hi2c, I2C_MessageTypeDef *I2C_Message)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(hi2c->Instance), I2C_ERROR_INSTANCE);
    assert_param_ret(!(hi2c->Instance->STAT & UCBBUSY), I2C_ERROR_BUSY);
    assert_param_ret(hi2c->State==I2C_STATE_IDLE, I2C_ERROR_BUSY); // Redundat?
    //assert_param_ret(I2C_Message->TxBuffer && I2C_Message->TxCount, I2C_ERROR_ARGIN);
    assert_param_ret(I2C_Message->DevAddress < 1024, I2C_ERROR_OVADR);

    __HAL_LOCK(hi2c);

    // Set parameters
    hi2c->TxBuffer = I2C_Message->TxBuffer;
    hi2c->TxCount = I2C_Message->TxCount;
    hi2c->State = I2C_STATE_TX;

    // Check for 10-bits addressing mode
    if(I2C_Message->DevAddress > 255)
    {
        hi2c->Instance->CTL0 |= UCSLA10;
    }

    // Set slave address
    __HAL_I2C_SET_SADDR(hi2c,I2C_Message->DevAddress);

    // Generate start condition
    hi2c->Instance->CTL1 |= UCTR | UCTXSTT;

    // Clear flags
    HAL_I2C_ClearPendingFlags(hi2c, I2C_TXIFG | I2C_RXIFG);
    __HAL_I2C_ENABLE_IT(hi2c, I2C_TXIE);

    __HAL_UNLOCK(hi2c);

    return I2C_ERROR_NONE;
}


uint16_t HAL_I2C_TransmitReceive_IT(I2C_HandleTypeDef *hi2c, I2C_MessageTypeDef *I2C_Message)
{
    assert_param_ret(IS_I2C_ALL_INSTANCE(hi2c->Instance), I2C_ERROR_INSTANCE);
    assert_param_ret(!(hi2c->Instance->STAT & UCBBUSY), I2C_ERROR_BUSY);
    assert_param_ret(hi2c->State==I2C_STATE_IDLE, I2C_ERROR_BUSY); // Redundat?
    //assert_param_ret(I2C_Message->TxBuffer && I2C_Message->TxCount, I2C_ERROR_ARGIN);
    //assert_param_ret(I2C_Message->RxBuffer && I2C_Message->RxCount, I2C_ERROR_ARGIN);
    assert_param_ret(I2C_Message->DevAddress < 1024, I2C_ERROR_OVADR);

    __HAL_LOCK(hi2c);

    // Set parameters
    hi2c->TxBuffer = I2C_Message->TxBuffer;
    hi2c->TxCount = I2C_Message->TxCount;
    hi2c->RxBuffer = I2C_Message->RxBuffer;
    hi2c->RxCount = I2C_Message->RxCount;
    hi2c->State = I2C_STATE_TX;

    // Check for 10-bits addressing mode
    if(I2C_Message->DevAddress > 255)
    {
        hi2c->Instance->CTL0 |= UCSLA10;
    }

    // Set slave address
    __HAL_I2C_SET_SADDR(hi2c,I2C_Message->DevAddress);

    // Generate start condition
    hi2c->Instance->CTL1 |= UCTR | UCTXSTT;

    // Clear flags
    HAL_I2C_ClearPendingFlags(hi2c, I2C_TXIFG | I2C_RXIFG);
    __HAL_I2C_ENABLE_IT(hi2c, I2C_TXIE);

    __HAL_UNLOCK(hi2c);

    return I2C_ERROR_NONE;
}





void HAL_I2C_EV_ISR(void *argin)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)argin;

    if(HAL_I2C_CheckPendingFlag(hi2c,I2C_RXIFG))
    {
        //Must read from UCB0RXBUF
        uint8_t rx_val = hi2c->Instance->RXBUF;

        // RX Handler
        if(hi2c->State == I2C_STATE_RX)
        {
            if(hi2c->RxCount)
            {
                *hi2c->RxBuffer++ = rx_val;
                hi2c->RxCount--;

                if (hi2c->RxCount == 1)
                {
                    // Send stop bit
                    hi2c->Instance->CTL1 |= UCTXSTP;
                }
                else if (!hi2c->RxCount)
                {
                    // Disable rx interrupt
                    __HAL_I2C_DISABLE_IT(hi2c,I2C_RXIE);
                    hi2c->State = I2C_STATE_IDLE;

                    // Callback
                    if(hi2c->RxCpltCallback) hi2c->RxCpltCallback(hi2c);
                }
            }
        }
    }
    else if(HAL_I2C_CheckPendingFlag(hi2c,I2C_TXIFG))
    {
        // TX
        if(hi2c->State == I2C_STATE_TX)
        {
            if(hi2c->TxCount)
            {
                hi2c->Instance->TXBUF = *hi2c->TxBuffer++;
                hi2c->TxCount--;
            }

            // End of tx
            if (!hi2c->TxCount)
            {
                // Disable tx interrupt
                __HAL_I2C_DISABLE_IT(hi2c,I2C_TXIE);

                // Check if it is expected rx data
                if(hi2c->RxCount)
                {
                    hi2c->State = I2C_STATE_RX;
                    __HAL_I2C_ENABLE_IT(hi2c, I2C_RXIE);
                    hi2c->Instance->CTL1 &= ~UCTR;      // Switch to receiver
                    hi2c->Instance->CTL1 |= UCTXSTT;    // Send repeated start
                    if (hi2c->RxCount == 1)
                    {
                        // Must send stop since this is the N-1 byte
                        while(hi2c->Instance->CTL1 & UCTXSTT);
                        hi2c->Instance->CTL1 |= UCTXSTP;      // Send stop condition
                    }
                }
                else
                {
                    hi2c->State = I2C_STATE_IDLE;
                }

                // Callback
                if(hi2c->TxCpltCallback) hi2c->TxCpltCallback(hi2c);
            }
        }
    }
}


void HAL_I2C_ER_ISR(void *argin)
{
    if (UCB0STAT & UCNACKIFG)
    {
        UCB0STAT &= ~UCNACKIFG;
    }
    else if (UCB0STAT & UCSTPIFG)
    {
        UCB0STAT &= ~(UCSTTIFG + UCSTPIFG + UCNACKIFG);
    }
    else if (UCB0STAT & UCSTTIFG)
    {
        UCB0STAT &= ~(UCSTTIFG);
    }
}

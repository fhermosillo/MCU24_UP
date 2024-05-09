/**
  ******************************************************************************
  * @file       msp430x2xx_hal_bcm.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      Source file of UART HAL module.
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
#include <Drivers/msp430_hal_uart.h>
#include <Drivers/msp430_hal_gpio.h>
#include <Drivers/msp430_hal_bcm.h>
#include <Drivers/usci_reg.h>
#include <math.h>


/* Private types -------------------------------------------------------------*/


/* Private constants ---------------------------------------------------------*/
#ifndef NULL
#define NULL    ((void *)(0))
#endif

/* Private variables ---------------------------------------------------------*/

static void HAL_UART_TXISR(void *argin);
static void HAL_UART_RXISR(void *argin);

/* Reference functions -------------------------------------------------------*/
void HAL_UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct)
{
    // Assert
    assert_param(IS_UART_ALL_INSTANCE(UARTx));

    // GPIO
    // TODO: This should be in the MX_GPIO_Init()
    //GPIO_InitTypeDef GPIO_Init = {.Mode=TXD_GPIO_Mode, .Pin=TXD_Pin | RXD_Pin};
    //HAL_GPIO_Init(RXD_Port, &GPIO_Init);

    // UART
    UARTx->CTL1 |= UCSWRST;

    // CTL0
    UARTx->CTL0 = 0x00;
    UARTx->CTL0 |= UART_InitStruct->Parity << USCI_CTL0_PEN_S;
    UARTx->CTL0 |= UART_InitStruct->MSBFirst << USCI_CTL0_MSB_S;
    UARTx->CTL0 |= UART_InitStruct->WordLength << USCI_CTL0_7BIT_S;
    UARTx->CTL0 |= UART_InitStruct->StopBits << USCI_CTL0_SPB_S;

    // CTL1
    UARTx->CTL1 &= UCSWRST; // Clear all other fields
    UARTx->CTL1 |= UART_InitStruct->ClkSrc << USCI_CTL1_SSEL_S;

    // BaudRate
#if configHAL_CPU_HAVE_FPU == (1)
    uint32_t brclk_clock_hz = 0x00;
    switch(UART_InitStruct->ClkSrc)
    {
        case UART_CLKSRC_ACLK:
            brclk_clock_hz = HAL_BCM_GetCLKFreq(BCM_ACLK);
            break;

        case UART_CLKSRC_SMCLK:
            brclk_clock_hz = HAL_BCM_GetCLKFreq(BCM_SMCLK);
            break;

        case UART_CLKSRC_UCLK:
            brclk_clock_hz = configUSCI_CLOCK_HZ;
            break;

        default:
            brclk_clock_hz = 0;
            break;
    }

    float Nf = (float)brclk_clock_hz/(float)UART_InitStruct->BaudRate;
    uint16_t Ni = (uint16_t)Nf;
    if(Ni >= 16)
    {
        Nf = Nf/16.0F;
        Ni = (uint16_t)floorf(Nf);
        UARTx->MCTL = ((uint8_t)roundf((Nf - Ni)*16.0F) << USCI_CTL1_BRF_S) | USCI_CTL1_OS16_M;
    }
    else
    {
        UARTx->MCTL = (uint8_t)roundf((Nf - Ni)*8.0F) << USCI_CTL1_BRS_S;
    }
    UARTx->BR0 = Ni & 0x00FF;
    UARTx->BR1 = (Ni >> 8) & 0x00FF;
#else
    UARTx->MCTL = UART_InitStruct->Modulation;
    UARTx->BR0 = UART_InitStruct->Prescaler & 0x00FF;
    UARTx->BR1 = (UART_InitStruct->Prescaler >> 8) & 0x00FF;
#endif
    //
    UCA0CTL1 &= ~UCSWRST;
}

void HAL_UART_Write(UART_TypeDef *UARTx, const unsigned char ByteToWrite)
{
    // Wait for TX buffer to be ready for new data
    while( HAL_UART_TXBusy() ) { }
    UARTx->TXBUF = ByteToWrite;
}

void HAL_UART_Read(UART_TypeDef *UARTx, unsigned char *ByteToRead)
{
    // Wait for RX buffer to be ready for new data
    while( HAL_UART_RXBusy() ) { }
    *ByteToRead = UARTx->RXBUF;
}

// Send number of bytes Specified in ucStreamLength in the array at using the hardware UART0
void HAL_UART_Puts(UART_TypeDef *UARTx, const unsigned char *StreamIn, unsigned char StreamLength)
{
    while(StreamLength--)
    {
        // Wait for TX buffer to be ready for new data
        while( HAL_UART_TXBusy() ) { }
        UARTx->TXBUF = *StreamIn++;
    }
}

void HAL_UART_Gets(UART_TypeDef *UARTx, unsigned char *StreamOut, unsigned char StreamLength)
{
    // Loop until StringLength == 0 and post decrement
    while(StreamLength--)
    {
        // Wait for RX buffer to be ready for new data
        while( HAL_UART_RXBusy() ) { }
        *StreamOut++ = UARTx->RXBUF;
    }
}


uint16_t HAL_UART_Write_IT(UART_HandleTypeDef *huart, UARTCallback_t TxCpltCallback, uint8_t *pData, uint8_t len)
{
    assert_param_ret(IS_UART_ALL_INSTANCE(huart->UARTx), HAL_ERROR);
    assert_param_ret(pData != NULL || len, HAL_ERROR);

    __HAL_LOCK(huart);

    uint16_t uart_err = HAL_BUSY;
    if(!huart->TxSize)
    {
        huart->TxCpltCallback = TxCpltCallback;
        huart->TxBuffer = pData;
        huart->TxSize = len;
        uart_err = HAL_OK;
    }

    if(!(IE2 & UCA0TXIE) && uart_err == HAL_OK)
    {
        usci_callback_config_t intr_cfg;
        intr_cfg.Module = USCI_MODULE_A;
        intr_cfg.Mode = USCIA_MODE_UART_TX;
        intr_cfg.Argin = huart;
        intr_cfg.Callback = HAL_UART_TXISR;
        uart_err = HAL_USCI_Intr_Alloc(&intr_cfg);
        if( uart_err == USCI_ERROR_NONE )
        {
            IFG2 &= ~UCA0TXIFG;
            IE2 |= UCA0TXIE;
        }
        else
        {
            huart->TxBuffer = NULL;
            huart->TxSize = 0;
            uart_err = HAL_ERROR;
        }
    }


    __HAL_UNLOCK(huart);

    return uart_err;
}

uint16_t HAL_UART_Read_IT(UART_HandleTypeDef *huart, UARTCallback_t RxCpltCallback, uint8_t *pData, uint8_t len)
{
    assert_param_ret(IS_UART_ALL_INSTANCE(huart->UARTx), HAL_ERROR);
    assert_param_ret(pData != NULL || len, HAL_ERROR);

    __HAL_LOCK(huart);

    uint16_t uart_err = HAL_BUSY;
    if(!huart->TxSize)
    {
        huart->RxCpltCallback = RxCpltCallback;
        huart->RxBuffer = pData;
        huart->RxSize = len;
        uart_err = HAL_OK;
    }

    if(!(IE2 & UCA0RXIE) && uart_err == HAL_OK)
    {
        usci_callback_config_t intr_cfg;
        intr_cfg.Module = USCI_MODULE_A;
        intr_cfg.Mode = USCIA_MODE_UART_RX;
        intr_cfg.Argin = huart;
        intr_cfg.Callback = HAL_UART_RXISR;
        uart_err = HAL_USCI_Intr_Alloc(&intr_cfg);
        if( uart_err == USCI_ERROR_NONE )
        {
            IFG2 &= ~UCA0RXIFG;
            IE2 |= UCA0RXIE;
        }
        else
        {
            uart_err = HAL_ERROR;
            huart->RxBuffer = NULL;
            huart->RxSize = 0;
        }
    }

    __HAL_UNLOCK(huart);

    return uart_err;
}

static void HAL_UART_TXISR(void *argin)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)argin;

    // Clear pending flag
    HAL_UART_ClearPendingFlag(UCA0TXIFG);

    // Transmit
    if(huart->TxSize)
    {
        huart->UARTx->TXBUF = *huart->TxBuffer++;
        huart->TxSize--;
        if(!huart->TxSize)
        {
            if(huart->TxCpltCallback) huart->TxCpltCallback(huart);
        }
    }
}

static void HAL_UART_RXISR(void *argin)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)argin;

    // Clear pending flag
    HAL_UART_ClearPendingFlag(UCA0RXIFG);

    // Receive
    if(huart->RxSize)
    {
         *huart->TxBuffer++ = huart->UARTx->RXBUF;
        huart->RxSize--;
        if(!huart->RxSize)
        {
            if(huart->RxCpltCallback) huart->RxCpltCallback(huart);
        }
    }
}

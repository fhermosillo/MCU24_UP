/**
  ******************************************************************************
  * @file       msp430x2xx_hal_bcm.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      Source file of USCI SPI HAL module.
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
#include <Drivers/msp430_hal_spi.h>
#include <Drivers/msp430_hal_gpio.h>
#include <Drivers/msp430_hal_bcm.h>
#include <Drivers/usci_reg.h>

/* Private types -------------------------------------------------------------*/


/* Private constants ---------------------------------------------------------*/
#ifndef NULL
#define NULL    ((void *)(0))
#endif

/* Private variables ---------------------------------------------------------*/

static void HAL_SPI_TXISR(void *argin);
static void HAL_SPI_RXISR(void *argin);

/* Reference functions -------------------------------------------------------*/
void HAL_SPI_Init(SPI_HandleTypeDef *hspi, SPI_InitTypeDef *SPI_InitStruct)
{
    // Assert
    assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
    assert_param(IS_SPI_MASTER(SPI_InitStruct->Master));
    assert_param(IS_SPI_FIRST_BIT(SPI_InitStruct->FirstBit));
    assert_param(IS_SPI_CLOCK_SOURCE(SPI_InitStruct->ClkSrc));
    assert_param(IS_SPI_DATASIZE(SPI_InitStruct->DataSize));
    assert_param(IS_SPI_INTERFACE(SPI_InitStruct->Interface));
    assert_param(IS_SPI_MODE(SPI_InitStruct->Mode));

    // SPI config
    // USCI logic held in reset state
    hspi->Instance->CTL1 = UCSWRST;

    // Control 0
    hspi->Instance->CTL0 = 0x00;
    hspi->Instance->CTL0 |= USCI_CTL0_SYNC_M;
    hspi->Instance->CTL0 |= SPI_InitStruct->Mode     << USCI_CTL0_CPOL_S;
    hspi->Instance->CTL0 |= SPI_InitStruct->FirstBit << USCI_CTL0_MSB_S;
    hspi->Instance->CTL0 |= SPI_InitStruct->DataSize << USCI_CTL0_7BIT_S;
    hspi->Instance->CTL0 |= SPI_InitStruct->Master   << USCI_CTL0_MODE_S;

    // Control 1
    hspi->Instance->CTL1 |= SPI_InitStruct->ClkSrc   << USCI_CTL1_SSEL_S;

    // BaudRate
#if configHAL_CPU_HAVE_FPU == (1)
    uint32_t brclk_clock_hz = 0x00;
    switch(SPI_InitStruct->ClkSrc)
    {
        case SPI_CLKSRC_ACLK:
            brclk_clock_hz = HAL_BCM_GetCLKFreq(BCM_ACLK);
            break;

        case SPI_CLKSRC_SMCLK:
            brclk_clock_hz = HAL_BCM_GetCLKFreq(BCM_SMCLK);
            break;

        default:
            brclk_clock_hz = 0;
            break;
    }
    uint16_t Prescaler = brclk_clock_hz/SPI_InitStruct->BaudRate;
    hspi->Instance->BR0 = Prescaler & 0x00FF;
    hspi->Instance->BR1 = (Prescaler >> 8) & 0x00FF;
#else
    hspi->Instance->BR0 = SPI_InitStruct->Prescaler & 0x00FF;
    hspi->Instance->BR1 = (SPI_InitStruct->Prescaler >> 8) & 0x00FF;
#endif

    // USCI reset released for operation
    hspi->Instance->CTL1 &= ~UCSWRST;
}

void HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size)
{
    // Assert
    assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
    assert_param(pData != NULL);
    // Chip select (active high)
    //P1OUT |= SPI_CS_Pin;

    // Transmit data
    while(Size--)
    {
        while (__HAL_SPI_GET_TX_FLAG(hspi)); // USCI_A0 TX buffer ready?
        hspi->Instance->TXBUF = *pData--; // Send data over SPI to Slave
    }

    // Chip unselect (active low)
    //P1OUT &= ~SPI_CS_Pin;
}
void HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
    // Assert
    assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
    assert_param(pData != NULL);

    // Chip select (active high)
    //P1OUT |= SPI_CS_Pin;

    // Receive data
    while(Size--)
    {
        while (__HAL_SPI_GET_RX_FLAG(hspi)); // USCI_A0 RX Received?
        *pData++ = hspi->Instance->RXBUF; // Store received data
    }

    // Chip unselect (active low)
    //P1OUT &= ~SPI_CS_Pin;
}

void HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
    // Assert
    assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));

    // TODO: Must be performed externally?
    // Chip select (active high)
    //P1OUT |= SPI_CS_Pin;

    // Transmit&Receive data
    while(Size--)
    {
        while (__HAL_SPI_GET_TX_FLAG(hspi)); // TX buffer ready?
        hspi->Instance->TXBUF = *pTxData--; // Send data over SPI to Slave

        while (__HAL_SPI_GET_RX_FLAG(hspi)); // RX buffer ready?
        *pRxData++ = hspi->Instance->RXBUF; // Store received data
    }

    // Chip unselect (active low)
    // TODO: Must be performed externally
    //P1OUT &= ~SPI_CS_Pin;
}


uint16_t HAL_SPI_RegisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID, SPI_CallbackTypeDef Callback)
{
    // Assert
    assert_param_ret(IS_SPI_ALL_INSTANCE(hspi->Instance), HAL_ERROR);

    // Enter critical section
    __HAL_LOCK(hspi);

    // Critical section
    usci_callback_config_t intr_cfg;
    intr_cfg.Module = (hspi->Instance == SPI1 ? USCI_MODULE_A : USCI_MODULE_B);
    intr_cfg.Argin = hspi;
    switch(CallbackID)
    {
    case HAL_SPI_CB_TX_COMPLETE_ID:
        hspi->TxCpltCallback = Callback;
        intr_cfg.Mode = USCIA_MODE_SPI_TX;
        intr_cfg.Callback = HAL_SPI_TXISR;
        HAL_USCI_Intr_Alloc(&intr_cfg);
        break;

    case HAL_SPI_CB_RX_COMPLETE_ID:
        hspi->RxCpltCallback = Callback;
        intr_cfg.Mode = USCIA_MODE_SPI_RX;
        intr_cfg.Callback = HAL_SPI_RXISR;
        HAL_USCI_Intr_Alloc(&intr_cfg);
        break;

    case HAL_SPI_CB_TXRX_COMPLETE_ID:
        hspi->TxRxCpltCallback = Callback;
        intr_cfg.Mode = USCIA_MODE_SPI_TX;
        intr_cfg.Callback = HAL_SPI_TXISR;
        HAL_USCI_Intr_Alloc(&intr_cfg);
        intr_cfg.Mode = USCIA_MODE_SPI_RX;
        intr_cfg.Callback = HAL_SPI_RXISR;
        HAL_USCI_Intr_Alloc(&intr_cfg);
        break;

    default:
        break;
    }

    // Exit critical
    __HAL_UNLOCK(hspi);

    return HAL_OK;
}

void HAL_SPI_UnregisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID)
{
    // Assert
    assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));

    // TODO: Implement
}

uint16_t HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
    // Assert
    assert_param_ret(IS_SPI_ALL_INSTANCE(hspi->Instance), HAL_ERROR);

    __HAL_LOCK(hspi);

    // Set parameters
    hspi->pTxBuffPtr = pData;
    hspi->TxXferSize = Size;

    // Enable interrupts
    __HAL_SPI_CLEAR_TX_FLAG(hspi);
    __HAL_SPI_ENABLE_TX_IT(hspi);

    __HAL_UNLOCK(hspi);

    return HAL_OK;
}

uint16_t HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size)
{
    // Assert
    assert_param_ret(IS_SPI_ALL_INSTANCE(hspi->Instance), HAL_ERROR);

    __HAL_LOCK(hspi);

    // Set parameters
    hspi->pRxBuffPtr = pData;
    hspi->RxXferSize = Size;

    // Enable interrupts
    __HAL_SPI_CLEAR_RX_FLAG(hspi);
    __HAL_SPI_ENABLE_RX_IT(hspi);

    __HAL_UNLOCK(hspi);

    return HAL_OK;
}

uint16_t HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
    // Assert
    assert_param_ret(IS_SPI_ALL_INSTANCE(hspi->Instance), HAL_ERROR);

    __HAL_LOCK(hspi);

    // Set parameters
    hspi->pTxBuffPtr = pTxData;
    hspi->TxXferSize = Size;
    hspi->pRxBuffPtr = pRxData;
    hspi->RxXferSize = Size;

    // Enable interrupts
    __HAL_SPI_CLEAR_TX_FLAG(hspi);
    __HAL_SPI_CLEAR_RX_FLAG(hspi);
    __HAL_SPI_ENABLE_TX_IT(hspi);
    __HAL_SPI_ENABLE_RX_IT(hspi);

    __HAL_UNLOCK(hspi);

    return HAL_OK;
}



static void HAL_SPI_TXISR(void *argin)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)argin;

    // Transmit
    if(hspi->TxXferSize)
    {
        hspi->Instance->TXBUF = *hspi->pTxBuffPtr++; // Send data over SPI to Slave
        hspi->TxXferSize--;
        if(!hspi->TxXferSize)
        {
            __HAL_SPI_DISABLE_TX_IT(hspi);
            if(hspi->TxCpltCallback) hspi->TxCpltCallback(hspi);
        }
    }
}

static void HAL_SPI_RXISR(void *argin)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)argin;

    // Receive
    if(hspi->RxXferSize)
    {
        *hspi->pRxBuffPtr++ = hspi->Instance->RXBUF;
        hspi->RxXferSize--;
        if(!hspi->RxXferSize)
        {
            __HAL_SPI_DISABLE_RX_IT(hspi);
            if(hspi->RxCpltCallback) hspi->RxCpltCallback(hspi);
        }
    }
}

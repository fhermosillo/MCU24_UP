/**
  ******************************************************************************
  * @file       msp430x2xx_hal_bcm.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      Header file of UART HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DRIVERS_MSP430_HAL_UART_H_
#define DRIVERS_MSP430_HAL_UART_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430x2xx.h>
#include <Drivers/msp430_hal_usci.h>

/** @addtogroup MSP430X2XX_HAL_Driver
 * @{
 */

/** @addtogroup BCM
 * @{
 */

/* Exported types ------------------------------------------------------------*/

typedef struct
{
    __IO uint8_t CTL0;
    __IO uint8_t CTL1;
    __IO uint8_t BR0;
    __IO uint8_t BR1;
    __IO uint8_t MCTL;
    __IO uint8_t STAT;
    __I  uint8_t RXBUF;
    __IO uint8_t TXBUF;

} UART_TypeDef;


typedef struct
{
#if configHAL_CPU_HAVE_FPU == (1)
    uint32_t BaudRate;
#else
    uint16_t Prescaler;
    uint8_t Modulation;
#endif
    uint16_t WordLength;
    uint16_t StopBits;
    uint16_t Parity;
    uint16_t MSBFirst;
    uint16_t ClkSrc;
} UART_InitTypeDef;

typedef void(* UARTCallback_t)(void *);

typedef struct
{
    UART_TypeDef *UARTx;

    uint8_t *TxBuffer;
    uint8_t *RxBuffer;
    volatile uint8_t TxSize;
    volatile uint8_t RxSize;

    UARTCallback_t RxCpltCallback;
    UARTCallback_t TxCpltCallback;

    HAL_BaseTypeDef Lock;
} UART_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
#define UART_WORDLENGTH_7B  (BIT0)
#define UART_WORDLENGTH_8B  (0)

#define UART_MSB_FIRST      (BIT0)
#define UART_LSB_FIRST      (0)

#define UART_PARITY_NONE    (0x00)
#define UART_PARITY_ODD     BIT1
#define UART_PARITY_EVEN    (BIT1 | BIT0)

#define UART_STOPBITS_1     (0x00)
#define UART_STOPBITS_2     BIT0

#define UART_OVERSAMPLING_ENABLE    BIT0
#define UART_OVERSAMPLING_DISABLE   (0x00)

#define UART_CLKSRC_UCLK    0x00
#define UART_CLKSRC_ACLK    BIT0
#define UART_CLKSRC_SMCLK   (BIT1 | BIT0)

/* Exported macro ------------------------------------------------------------*/
#define HAL_UART_TXBusy()   !(IFG2 & UCA0TXIFG)
#define HAL_UART_RXBusy()   !(IFG2 & UCA0RXIFG)
#define HAL_UART_ClearPendingFlag(FLAG) IFG2 &= ~(FLAG)

/* Exported functions --------------------------------------------------------*/
void HAL_UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct);
void HAL_UART_Write(UART_TypeDef *UARTx, const unsigned char ByteToWrite);
void HAL_UART_Read(UART_TypeDef *UARTx, unsigned char *ByteToRead);
void HAL_UART_Puts(UART_TypeDef *UARTx, const unsigned char *StreamIn, unsigned char StreamLength);
void HAL_UART_Gets(UART_TypeDef *UARTx, unsigned char *StreamOut, unsigned char StreamLength);

uint16_t HAL_UART_Write_IT(UART_HandleTypeDef *huart, UARTCallback_t TxCpltCallback, uint8_t *pData, uint8_t len);
uint16_t HAL_UART_Read_IT(UART_HandleTypeDef *huart, UARTCallback_t RxCpltCallback, uint8_t *pData, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_MSP430_HAL_UART_H_ */

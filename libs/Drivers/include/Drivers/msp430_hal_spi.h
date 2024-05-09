/**
  ******************************************************************************
  * @file       msp430x2xx_hal_bcm.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      Header file of USCI SPI HAL module.
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
#ifndef DRIVERS_MSP430_HAL_SPI_H_
#define DRIVERS_MSP430_HAL_SPI_H_

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
   uint8_t Master;
   uint8_t Interface;
   uint8_t DataSize;
   uint8_t Mode;
   uint8_t FirstBit;
   uint8_t ClkSrc;
#if configHAL_CPU_HAVE_FPU == (1)
   uint32_t BaudRate;
#else
   uint16_t Prescaler;
#endif
 } SPI_InitTypeDef;

 typedef struct
 {
     __IO uint8_t CTL0;
     __IO uint8_t CTL1;
     __IO uint8_t BR0;
     __IO uint8_t BR1;
     __IO uint8_t : 8;   // Unused
     __IO uint8_t STAT;
     __I  uint8_t RXBUF;
     __IO uint8_t TXBUF;
 } SPI_TypeDef;

/*typedef struct
{
    uint8_t Pin;
    GPIO_TypeDef_t *GPIOx;
} SPI_SlaveTypeDef;*/

 typedef struct __SPI_HandleTypeDef
 {
     // Hw registers
     SPI_TypeDef *Instance;

     // Interrupts
     volatile uint8_t *pTxBuffPtr;
     volatile uint16_t TxXferSize;
     volatile uint8_t *pRxBuffPtr;
     volatile uint16_t RxXferSize;

     // Callbacks
     void(*TxCpltCallback)(struct __SPI_HandleTypeDef *hspi);
     void(*RxCpltCallback)(struct __SPI_HandleTypeDef *hspi);
     void(*TxRxCpltCallback)(struct __SPI_HandleTypeDef *hspi);
     //void(*HAL_SPI_ErrorCallback)(struct __SPI_HandleTypeDef *hspi);

     // CS
     uint8_t CS_Pin;
     __IO uint8_t *CS_Port;


     // Flags
     HAL_BaseTypeDef Lock;
 } SPI_HandleTypeDef;

 typedef void(*SPI_CallbackTypeDef)(SPI_HandleTypeDef *hspi);

 typedef enum
 {
     HAL_SPI_CB_TX_COMPLETE_ID    = 0x00U,    /*!< SPI Tx Completed callback ID         */
     HAL_SPI_CB_RX_COMPLETE_ID    = 0x01U,    /*!< SPI Rx Completed callback ID         */
     HAL_SPI_CB_TXRX_COMPLETE_ID  = 0x02U,    /*!< SPI TxRx Completed callback ID       */
     HAL_SPI_CB_MAX_ID
 } HAL_SPI_CallbackIDTypeDef;

/* Exported constants --------------------------------------------------------*/
// Master/Slave selection
#define SPI_MODE_SLAVE          (0x00)
#define SPI_MODE_MASTER         (0x01)
#define IS_SPI_MASTER(MSSEL)    (((MSSEL) == SPI_MODE_SLAVE) || \
                                ((MSSEL) == SPI_MODE_MASTER))
// Word length
#define SPI_DATASIZE_8BITS          (0x00)
#define SPI_DATASIZE_7BITS          (0x01)
#define IS_SPI_DATASIZE(DATASIZE)   (((DATASIZE) == SPI_DATASIZE_7BITS) || \
                                    ((DATASIZE) == SPI_DATASIZE_8BITS))
// First bit
#define SPI_MSB_FIRSTBIT            (0x01)
#define SPI_LSB_FIRSTBIT            (0x00)
#define IS_SPI_FIRST_BIT(FIRSTBIT)  (((FIRSTBIT) == SPI_MSB_FIRSTBIT) || \
                                    ((FIRSTBIT) == SPI_LSB_FIRSTBIT))
// Clock source
#define SPI_CLKSRC_ACLK    (0x01)
#define SPI_CLKSRC_SMCLK   (0x02)
#define IS_SPI_CLOCK_SOURCE(CLKSRC) (((CLKSRC) == SPI_CLKSRC_ACLK) || \
                                    ((CLKSRC) == SPI_CLKSRC_SMCLK))
// Interface
#define SPI_INTERFACE_3WIRE         (0x00)
#define SPI_INTERFACE_4WIRE_STE     (0x01)
#define SPI_INTERFACE_4WIRE_NSTE    (0x02)
#define IS_SPI_INTERFACE(INTERFACE) (((INTERFACE) == SPI_INTERFACE_3WIRE) || \
                                    ((INTERFACE) == SPI_INTERFACE_4WIRE_STE) || \
                                    ((INTERFACE) == SPI_INTERFACE_4WIRE_NSTE))
// Mode
#define SPI_MODE_0    (0x02)
#define SPI_MODE_1    (0x00)
#define SPI_MODE_2    (0x03)
#define SPI_MODE_3    (0x01)
#define IS_SPI_MODE(MODE)   (((MODE) == SPI_MODE_0) || \
                            ((MODE) == SPI_MODE_1) || \
                            ((MODE) == SPI_MODE_2) || \
                            ((MODE) == SPI_MODE_3))
// Chip Select (CS)
#define SPI_CS_LOW  (0x00)
#define SPI_CS_HIGH (0x01)
#define IS_SPI_CS   (((CSCON) == SPI_CS_LOW) || \
                    ((CSCON) == SPI_CS_HIGH))

// Interrupts (TODO: should it be in the msp430g2x5x?)
#define SPI0_TXIE   UCA0TXIE
#define SPI0_RXIE   UCA0RXIE
#define SPI0_TXIFG  UCA0TXIFG
#define SPI0_RXIFG  UCA0RXIFG
#define SPI1_TXIE   UCB0TXIE
#define SPI1_RXIE   UCB0RXIE
#define SPI1_TXIFG  UCB0TXIFG
#define SPI1_RXIFG  UCB0RXIFG

/* Exported macro ------------------------------------------------------------*/
#define __HAL_SPI_TXBUSY(__HANDLE__)            !(IFG2 & (__HANDLE__->Instance == SPI1 ? UCA0TXIFG : UCB0TXIFG))
#define __HAL_SPI_RXBUSY(__HANDLE__)            !(IFG2 & (__HANDLE__->Instance == SPI1 ? UCA0RXIFG : UCB0RXIFG))
#define __HAL_SPI_ENABLE_TX_IT(__HANDLE__)     IE2 |= (__HANDLE__->Instance == SPI1 ? UCA0TXIE : UCB0TXIE)
#define __HAL_SPI_DISABLE_TX_IT(__HANDLE__)    IE2 &= ~(__HANDLE__->Instance == SPI1 ? UCA0TXIE : UCB0TXIE)
#define __HAL_SPI_ENABLE_RX_IT(__HANDLE__)     IE2 |= (__HANDLE__->Instance == SPI1 ? UCA0RXIE : UCB0RXIE)
#define __HAL_SPI_DISABLE_RX_IT(__HANDLE__)    IE2 &= ~(__HANDLE__->Instance == SPI1 ? UCA0RXIE : UCB0RXIE)
#define __HAL_SPI_GET_TX_FLAG(__HANDLE__)       !(IFG2 & (__HANDLE__->Instance == SPI1 ? UCA0TXIFG : UCB0TXIFG))
#define __HAL_SPI_GET_RX_FLAG(__HANDLE__)       !(IFG2 & (__HANDLE__->Instance == SPI1 ? UCA0RXIFG : UCB0RXIFG))
#define __HAL_SPI_CLEAR_TX_FLAG(__HANDLE__)     IFG2 &= ~(__HANDLE__->Instance == SPI1 ? UCA0TXIFG : UCB0TXIFG)
#define __HAL_SPI_CLEAR_RX_FLAG(__HANDLE__)     IFG2 &= ~(__HANDLE__->Instance == SPI1 ? UCA0TXIFG : UCB0TXIFG)

 #define HAL_SPI_TXBusy(SPIx)   !(IFG2 & (SPIx == SPI1 ? UCA0TXIFG : UCB0TXIFG)
#define HAL_SPI_RXBusy(SPIx)   !(IFG2 & (SPIx == SPI1 ? UCA0TXIFG : UCB0TXIFG)
#define HAL_SPI_ClearPendingFlag(FLAG) IFG2 &= ~(FLAG)

/* Exported functions --------------------------------------------------------*/
void HAL_SPI_Init(SPI_HandleTypeDef *hspi, SPI_InitTypeDef *SPI_InitStruct);
void HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, const uint8_t *pData, uint16_t Size);
void HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
void HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, const uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
uint16_t HAL_SPI_RegisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID, SPI_CallbackTypeDef Callback);
void HAL_SPI_UnregisterCallback(SPI_HandleTypeDef *hspi, HAL_SPI_CallbackIDTypeDef CallbackID);
uint16_t HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
uint16_t HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
uint16_t HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_MSP430_HAL_SPI_H_ */

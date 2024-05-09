/**
  ******************************************************************************
  * @file       msp430_hal_i2c.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions prototypes for the HAL
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DRIVERS_MSP430_HAL_I2C_H_
#define DRIVERS_MSP430_HAL_I2C_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430x2xx.h>
#include <Drivers/msp430_hal_usci.h>

/* Exported types ------------------------------------------------------------*/
 typedef struct
 {
     volatile const uint8_t *TxBuffer;
     volatile uint8_t TxCount;
     volatile uint8_t *RxBuffer;
     volatile uint8_t RxCount;
     uint16_t DevAddress;
 } I2C_MessageTypeDef;

typedef struct
{
    uint8_t Mode;
    uint16_t OwnAddress;
#if configHAL_CPU_HAVE_FPU == (1)
    uint32_t BaudRate;
#else
    uint16_t Prescaler;
#endif
    uint8_t ClkSrc;
} I2C_InitTypeDef;

 typedef struct
 {
     __IO uint8_t CTL0;
     __IO uint8_t CTL1;
     __IO uint8_t BR0;
     __IO uint8_t BR1;
     __IO uint8_t I2CIE;
     __IO uint8_t STAT;
     __I  uint8_t RXBUF;
     __IO uint8_t TXBUF;

 } I2C_TypeDef;

typedef struct
{
    I2C_TypeDef *Instance;

    volatile const uint8_t *TxBuffer;
    volatile uint8_t TxCount;
    volatile uint8_t *RxBuffer;
    volatile uint8_t RxCount;

    void(*TxCpltCallback)(void *hi2c);
    void(*RxCpltCallback)(void *hi2c);
    void(*ErrorCallback)(void *hi2c);

    uint8_t State;
    HAL_BaseTypeDef Lock;
} I2C_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
// Flags
#define I2C_MODE_SLAVE      (0)
#define I2C_MODE_MASTER     (1)
#define I2C_OWNADDR_7BITS   (0)
#define I2C_OWNADDR_10BITS  (1)
#define I2C_MODE_RX        (0x00)
#define I2C_MODE_TX        (BIT4)
#define I2C_STOP    (0)
#define I2C_RESTART_TX (0x01)
#define I2C_RESTART_RX (0x02)
// Error
#define I2C_ERROR_NONE  (0)
#define I2C_ERROR_NACK  (1)
#define I2C_ERROR_OVADR (2)
#define I2C_ERROR_INSTANCE  (3)
#define I2C_ERROR_BUSY      (4)
#define I2C_ERROR_IDLE      (5)
#define I2C_ERROR_ARGIN     (6)
// Clocks
#define I2C_CLKSRC_UCLK    (0)
#define I2C_CLKSRC_ACLK    (1)
#define I2C_CLKSRC_SMCLK   (3)
// Interrupts
#define I2C_TXIFG  UCB0TXIFG
#define I2C_RXIFG  UCB0RXIFG
#define I2C_TXIE   UCB0TXIE
#define I2C_RXIE   UCB0RXIE

/* Exported macro ------------------------------------------------------------*/
#define __HAL_I2C_ENABLE_IT(__HANDLE__, __FLAGS__)  IE2 |= (__FLAGS__)
#define __HAL_I2C_DISABLE_IT(__HANDLE__, __FLAGS__) IE2 &= ~(__FLAGS__)
#define __HAL_I2C_SET_SADDR(__HANDLE__,__SADDR__) UCB0I2CSA = __SADDR__
#define HAL_I2C_ClearPendingFlags(__HANDLE__, __FLAGS__) IFG2 &= ~(__FLAGS__)
#define HAL_I2C_CheckPendingFlag(__HANDLE__, __FLAG__)  IFG2 & __FLAG__
/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
 /*
  * @name    HAL_I2C_Init
  * @brief   Initialize the I2C module
  * @param   I2C_InitStruct
  * @return  None
  * @require None
  */
void HAL_I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct);

 /*
  * @name    HAL_I2C_Start
  * @brief   Sends the start bit and the slave address of an I2C message
  * @param   DevAddress, address of the slave device
  * @return  I2C error
  * @require Calling HAL_I2C_Init
  */
uint16_t HAL_I2C_Begin(I2C_TypeDef *I2Cx, uint16_t DevAddress);

 /*
  * @name    HAL_I2C_Write
  * @brief   Write a set of bytes via I2C protocol
  *
  * @param   pData, pointer of data to send
  * @param   Size, number of bytes to send
  *
  * @return  I2C error
  * @require HAL_I2C_Start
  */
uint16_t HAL_I2C_Write(I2C_TypeDef *I2Cx, const uint8_t *pData, uint8_t Size);

 /*
  * @name    HAL_I2C_Read
  * @brief   Read a set of bytes via I2C protocol
  *
  * @param   pData, pointer of data to receive
  *
  * @return  I2C error
  * @require HAL_I2C_Write, HAL_I2C_Stop
  */
uint16_t HAL_I2C_Read(I2C_TypeDef *I2Cx, uint8_t *pData, uint8_t Size);

 /*
  * @name    HAL_I2C_Stop
  * @brief   Send the stop bit of a I2C message
  * @param   BusRelease, a flag to determine if a stop bit will be sent or a restart bit
  * @return  None
  * @require HAL_I2C_Start
  */
uint16_t HAL_I2C_End(I2C_TypeDef *I2Cx, uint8_t BusRelease);



uint16_t HAL_I2C_Connected(I2C_TypeDef *I2Cx, uint8_t Address);

uint16_t HAL_I2C_Read_Regs(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t RegAddress, uint8_t *RegData, uint8_t len);

uint16_t HAL_I2C_Write_Regs(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t RegAddress, const uint8_t *RegData, uint8_t len);

 /*
  * @name    HAL_I2C_Transfer
  * @brief   Perform a complete I2C transferring, including the write and read operations
  *
  * @param   DevAddress
  * @param   I2C_MessageStruct
  *
  * @return  I2C error {I2C_ERROR_NONE, I2C_ERROR_NACK, I2C_ERROR_OVADR}
  * @require None
  */
uint16_t HAL_I2C_Transfer(I2C_TypeDef *I2Cx, I2C_MessageTypeDef *I2C_MessageStruct);

uint16_t HAL_I2C_Alloc_ISR(I2C_HandleTypeDef *hi2c);
uint16_t HAL_I2C_Free_ISR(I2C_HandleTypeDef *hi2c);
uint16_t HAL_I2C_Transmit_IT(I2C_HandleTypeDef *hi2c, I2C_MessageTypeDef *I2C_Message);
uint16_t HAL_I2C_TransmitReceive_IT(I2C_HandleTypeDef *hi2c, I2C_MessageTypeDef *I2C_Message);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_MSP430_HAL_I2C_H_ */

/**
  ******************************************************************************
  * @file       i2c.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions prototypes for the I2C
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_H_
#define I2C_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <msp430.h>
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define I2C_SCL_Pin     BIT6
#define I2C_SDA_Pin     BIT7

#define I2C_ERROR_NONE  (0)
#define I2C_ERROR_NACK  (1)
#define I2C_ERROR_OVADR (2)

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
 /*
  * @name    HAL_I2C_Init
  * @brief   Initialize the I2C module
  * @param   I2C_InitStruct
  * @return  None
  * @require None
  */
 void HAL_I2C_Init(uint16_t prescaler);

 /*
  * @name    HAL_I2C_Ack
  * @brief   Check the acknowledge bit
  * @param   None
  * @return  The NACK flag if acknowledge was not detected, otherwise zero (0)
  * @require Start condition and i2c slave address
  */
 int HAL_I2C_Ack(void);


 /*
   * @name    HAL_I2C_Write
   * @brief   Write a byte to the i2c slave device with address "Address"
   *
   * @param   Address, i2c slave address to write
   * @param   Data, byte to write
   *
   * @return  Whether the byte was transmitted (0) or not (Any other code)
   * @require HAL_I2C_Init
   */
uint16_t HAL_I2C_Write(uint8_t Address, const uint8_t Data);

/*
 * @name    HAL_I2C_Write_Multi
 * @brief   Write a byte to the i2c slave device with address "Address"
 *
 * @param   Address, i2c slave address to write
 * @param   pData, pointer to uint8_t that stores the data to write
 * @param   Size, the number of bytes to write
 *
 * @return  Whether the bytes were transmitted (0) or not (Any other code)
 * @require HAL_I2C_Init
 */
uint16_t HAL_I2C_Write_Multi(uint8_t Address, const uint8_t *pData, uint8_t Size);

/*
 * @name    HAL_I2C_Request
 * @brief   Read a set of bytes to the i2c slave device with address "Address"
 *
 * @param   Address, i2c slave address to read from
 * @param   pData, pointer to uint8_t that will store the read data
 * @param   Size, the number of bytes to read
 *
 * @return  Whether the bytes were read (0) or not (Any other code)
 * @require HAL_I2C_Init
 */
uint16_t HAL_I2C_Request(uint8_t Address, uint8_t *pData, uint8_t Size);



 /*
  * @name    HAL_I2C_Connected
  * @brief   Check if the slave with address "Address" is connected in the i2c bus
  * @param   Address, i2c slave address to check
  * @return  Whether the slave was detected (1) or not (0)
  * @require HAL_I2C_Init
  */
 int HAL_I2C_Connected(uint8_t Address);


 /*
  * @name    HAL_I2C_Read_Reg
  * @brief   Read a register from an i2c slave
  * @param   Address, The i2c slave address
  * @param   RegAddress, The address of the register to read
  * @param   RegData, Pointer to an uint8_t where data will be stored
  *
  * @return  The NACK flag if acknowledge was not detected, otherwise zero (0)
  * @require HAL_I2C_Init
  */
 int HAL_I2C_Read_Reg(uint8_t Address, uint8_t RegAddress, uint8_t *RegData);

 /*
  * @name    HAL_I2C_Write_Reg
  * @brief   Write to a register of an i2c slave
  * @param   Address, The i2c slave address
  * @param   RegAddress, The address of the register to read
  * @param   RegData, Data to write in the register
  *
  * @return  The NACK flag if acknowledge was not detected, otherwise zero (0)
  * @require HAL_I2C_Init
  */
 int HAL_I2C_Write_Reg(uint8_t Address, uint8_t RegAddress, uint8_t RegData);

 /*
  * @name    HAL_I2C_Read_Regs
  * @brief   Read to a set of registers of an i2c slave
  * @param   Address, The i2c slave address
  * @param   RegAddress, The address of the first register to read
  * @param   RegData, Pointer to an array of uint8_t where data will be stored
  * @param   Size, Number of registers to read
  *
  * @return  The NACK flag if acknowledge was not detected, otherwise zero (0)
  * @require HAL_I2C_Init
  */
 int HAL_I2C_Read_Regs(uint8_t Address, uint8_t RegAddress, uint8_t *RegData, uint8_t Size);

 /*
  * @name    HAL_I2C_Write_Regs
  * @brief   Write to a set of registers of an i2c slave
  * @param   Address, The i2c slave address
  * @param   RegAddress, The address of the first register to write
  * @param   RegData, Pointer to an array of uint8_t that stores the data to write
  * @param   Size, Number of registers to write
  *
  * @return  The NACK flag if acknowledge was not detected, otherwise zero (0)
  * @require HAL_I2C_Init
  */
 int HAL_I2C_Write_Regs(uint8_t Address, uint8_t RegAddress, const uint8_t *RegData, uint8_t Size);


/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */

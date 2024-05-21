/**
  ******************************************************************************
  * @file       main.c
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains the main function for the Universal Serial
  *             Communication Interface (USCI) example configured as I2C.
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
#include "i2c.h"

/* Types ---------------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
#define configMCLK_CLOCK_HZ     (1000000UL)
#define configSMCLK_CLOCK_HZ    configMCLK_CLOCK_HZ
#define configBRCLK_CLOCK_HZ    configSMCLK_CLOCK_HZ

// I/Os
#define LED_RED_Pin     BIT0
#define LED_GREEN_Pin   BIT6
#define BUTTON_Pin      BIT3



#define MPU6050_ADDRESS 0x68
#define MPU60X0_INT_Pin BIT1
// MPU60X0 Register Map
#define MPU60X0_SMPLRT_DIV      0x19
#define MPU60X0_CONFIG          0x1A
#define MPU60X0_GYRO_CONFIG     0x1B
#define MPU60X0_ACCEL_CONFIG    0x1C
#define MPU60X0_ACCEL_CONFIG_2  0x1D
#define MPU60X0_INT_PIN_CFG     0x37
#define MPU60X0_INT_ENABLE      0x38
#define MPU60X0_INT_STATUS      0x3A
#define MPU60X0_ACCEL_XOUT_H    0x3B
#define MPU60X0_ACCEL_XOUT_L    0x3C
#define MPU60X0_ACCEL_YOUT_H    0x3D
#define MPU60X0_ACCEL_YOUT_L    0x3E
#define MPU60X0_ACCEL_ZOUT_H    0x3F
#define MPU60X0_ACCEL_ZOUT_L    0x40
#define MPU60X0_TEMP_OUT_H      0x41
#define MPU60X0_TEMP_OUT_L      0x42
#define MPU60X0_GYRO_XOUT_H     0x43
#define MPU60X0_GYRO_XOUT_L     0x44
#define MPU60X0_GYRO_YOUT_H     0x45
#define MPU60X0_GYRO_YOUT_L     0x46
#define MPU60X0_GYRO_ZOUT_H     0x47
#define MPU60X0_GYRO_ZOUT_L     0x48
#define MPU60X0_USER_CTRL       0x6A
#define MPU60X0_PWR_MGMT_1      0x6B
#define MPU60X0_PWR_MGMT_2      0x6C
#define MPU60X0_FIFO_COUNT_H    0x72
#define MPU60X0_FIFO_COUT_L     0x73
#define MPU60X0_FIFO_R_W        0x74
#define MPU60X0_WHO_AM_I        0x75
#define MPU60X0_WHO_AM_I_ID     0x70

/* Private macro -------------------------------------------------------------*/
#define pdMS_TO_CYCLES(ulTimeInMs)  ((configMCLK_CLOCK_HZ/1000UL)*ulTimeInMs)
#define pdUS_TO_CYCLES(ulTimeInUs)  ((configMCLK_CLOCK_HZ/1000000UL)*ulTimeInUs)

#define HAL_Delay_Millis(ulTimeInMs)        __delay_cycles(pdMS_TO_CYCLES(ulTimeInMs))
#define HAL_Delay_Micros(ulTimeInUs)        __delay_cycles(pdUS_TO_CYCLES(ulTimeInUs))

/* Private functions ---------------------------------------------------------*/
int MPU6050_Init(void);

/* Private variables ---------------------------------------------------------*/
uint16_t ulAcceleration[3] = {0x0000, 0x0000, 0x0000};

/* Main program --------------------------------------------------------------*/
void main(void)
{
    /* WDT config */
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT

    /* BCM config */
    BCSCTL1 = CALBC1_1MHZ;  // Set DCO to 1MHz
    DCOCTL  = CALDCO_1MHZ;  // Set DCO to 1MHz

    /* GPIO config */
    P1OUT = 0x00;
    P1DIR |= LED_RED_Pin;
    P1DIR &= ~BUTTON_Pin;
    P1REN |= BUTTON_Pin;
    P1OUT &= ~BUTTON_Pin;


    /* I2C config */
    HAL_I2C_Init(configBRCLK_CLOCK_HZ/100000UL);

    /* MPU Init */
    // MPU6560 I2C communications at up to 400kHz
    //               { - 1101000=0x68, AD0=0
    // I2C Address = {
    //               { - 1101001=0x69, AD0=1
    // Gyroscope
    //  - Full-scale range: ±250, ±500, ±1000, and ±2000°/sec (dp)
    //  - Max Sample Rate: 8kHz
    // Accelerometer
    //  - Full-scale range: ±2g, ±4g, ±8g, and ±16g
    //  - Max Sample Rate: 1kHz
    //  - Axis: 3 (X,Y,Z)
    //  - Data Width: 16-bits
    if( !MPU6050_Init() )
    {
        // Failed to configure MPU6500
        while(1)
        {
            P1OUT ^= LED_RED_Pin;
            HAL_Delay_Millis(100);
        }
    }

    /* Super loop */
    while(true)
    {
        // Wait until a new MPU60X0 sample is converted
        // Polling for the MPU60X0 INT pin
        if(P1IN & MPU60X0_INT_Pin)
        {
            P1OUT ^= LED_RED_Pin;

            /* Method #1: Register by Register */
            // Read the acceleration at the x-axis:
            // The reading is suppose to clear the interrupt status
            uint8_t regdata = 0x00;
            HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_XOUT_H, &regdata);  // Read the most significant 8-bits
            ulAcceleration[0] = (uint16_t)regdata << 8;
            HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_XOUT_L, &regdata);  // Read the less significant 8-bits
            ulAcceleration[0] |= (uint16_t)regdata;
            // Read the acceleration at the y-axis
            HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_YOUT_H, &regdata);  // Read the most significant 8-bits
            ulAcceleration[1] = (uint16_t)regdata << 8;
            HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_YOUT_L, &regdata);  // Read the less significant 8-bits
            ulAcceleration[1] |= (uint16_t)regdata;
            // Read the acceleration at the z-axis
            HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_ZOUT_H, &regdata);  // Read the most significant 8-bits
            ulAcceleration[2] = (uint16_t)regdata << 8;
            HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_ZOUT_L, &regdata);  // Read the less significant 8-bits
            ulAcceleration[2] |= (uint16_t)regdata;

            /* Method #2: Read the set of 6 registers at once */
            // This function reads the set of 6 registers into the array ulAcceleration
            // This is the fastest way to read multiple registers
            HAL_I2C_Read_Regs(MPU6050_ADDRESS, MPU60X0_ACCEL_XOUT_H, (uint8_t *)ulAcceleration, 6);
        }
    }
}


/* Interrupt service routines ------------------------------------------------*/

/* Reference functions -------------------------------------------------------*/
int MPU6050_Init(void)
{
    // Small delay to ensure power on
    HAL_Delay_Millis(40);

    // Check connection
    if( !HAL_I2C_Connected(MPU6050_ADDRESS) ) return 0;
    uint8_t ID = 0x00;
    HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_WHO_AM_I, &ID);
    if (ID != MPU60X0_WHO_AM_I_ID) return 0;

    // GPIO Config
    P1DIR &= ~MPU60X0_INT_Pin;

    // Reset
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_PWR_MGMT_1, 0x80);
    uint8_t regdata = 0x80;
    while(regdata & 0x80)
    {
        HAL_I2C_Read_Reg(MPU6050_ADDRESS, MPU60X0_PWR_MGMT_1, &regdata);
    }

    // Clock selection & wakeup device
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_PWR_MGMT_1, 0x03);

    //
    //HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_PWR_MGMT_2, 0x00);


    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_SMPLRT_DIV, 0x00);

    // Disable FSYNC and set 41Hz GYRO filtering, 1KHz sampling
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_CONFIG, 0x03);

    // Full-scale range (+-2000 degs/s)
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_GYRO_CONFIG, 0x18);

    // Set accelerometer full scale range (+-16g)
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_CONFIG, 0x18);

    // 41 Hz filtering
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_ACCEL_CONFIG_2, 0x03);

    // LATCH_INT_EN: the INT pin is held high until the interrupt is cleared
    // INT_ANYRD_2CLEAR:  interrupt status is cleared if any read operation is performed
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_INT_PIN_CFG, 0x32);

    // RAW_RDY_EN: Enable Raw Sensor Data Ready interrupt to propagate to interrupt pin
    HAL_I2C_Write_Reg(MPU6050_ADDRESS, MPU60X0_INT_ENABLE, 0x01);

    // Wait for sensor to stabilize
    HAL_Delay_Millis(100);

    // Maybe calibration routine?

    return 1;
}

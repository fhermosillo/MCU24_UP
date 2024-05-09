/**
  ******************************************************************************
  * @file       msp430x2xx.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the peripherals structures for the HAL
  *             module driver.
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
#ifndef DRIVERS_MSP430X2XX_H_
#define DRIVERS_MSP430X2XX_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430_hal_types.h>

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
#define configHAL_CPU_HAVE_FPU  (1)

// GPIO ===========================================
#define configHAL_GPIO_NUM  (2)
#define GPIO1_BASE          (0x0020)
#define GPIO2_BASE          (0x0028)
#define PSEL2_BASE          (0x0041)
#define GPIO1               ((GPIO_TypeDef_t *)GPIO1_BASE)
#define GPIO2               ((GPIO_TypeDef_t *)GPIO2_BASE)
#define PSEL2               ((PSEL2_TypeDef_t *)PSEL2_BASE)

// USCI ===========================================
#define configHAL_USCI_NUM  (2)
#define configUSCI_INTR     (1)
#ifndef configUSCI_CLOCK_HZ
#define configUSCI_CLOCK_HZ (0)
#endif
#define USCIA_BASE          (0x60)
#define USCIB_BASE          (0x68)

// UART ===========================================
#define configHAL_UART_NUM  (1)
#define UART1_BASE          (USCIA_BASE)
#define UART1               ((UART_TypeDef *)(UART1_BASE))
#define UART1_RXD_Pin       GPIO_PIN_1
#define UART1_TXD_Pin       GPIO_PIN_2
#define UART1_RXD_Port      GPIO1
#define UART1_TXD_Port      GPIO1
#define UART1_GPIO_TXD_Mode GPIO_MODE_ALTFUN_3
#define UART1_GPIO_RXD_Mode GPIO_MODE_ALTFUN_3

 // SPI ===========================================
#define configHAL_SPI_NUM   (2)
#define SPI1_BASE           (USCIA_BASE)
#define SPI2_BASE           (USCIB_BASE)
#define SPI1                ((SPI_TypeDef *)(SPI1_BASE))
#define SPI2                ((SPI_TypeDef *)(SPI2_BASE))
#define SPI1_MISO_Pin       GPIO_PIN_1
#define SPI1_MOSI_Pin       GPIO_PIN_2
#define SPI1_SCK_Pin        GPIO_PIN_4
#define SPI1_STE_Pin        GPIO_PIN_5
#define SPI1_MISO_Port      GPIO1
#define SPI1_MOSI_Port      GPIO1
#define SPI1_SCK_Port       GPIO1
#define SPI1_STE_Port       GPIO1
#define SPI2_MISO_Pin       GPIO_PIN_6
#define SPI2_MOSI_Pin       GPIO_PIN_7
#define SPI2_SCK_Pin        GPIO_PIN_5
#define SPI2_STE_Pin        GPIO_PIN_4
#define SPI2_MISO_Port      GPIO1
#define SPI2_MOSI_Port      GPIO1
#define SPI2_SCK_Port       GPIO1
#define SPI2_STE_Port       GPIO1
#define SPI_GPIO_MISO_Mode  GPIO_MODE_ALTFUN_3
#define SPI_GPIO_MOSI_Mode  GPIO_MODE_ALTFUN_3
#define SPI_GPIO_SCK_Mode   GPIO_MODE_ALTFUN_3
#define SPI_GPIO_STE_Mode   GPIO_MODE_ALTFUN_3

// I2C ===========================================
#define configHAL_I2C_NUM   (1)
#define I2C1_BASE           (USCIB_BASE)
#define I2C1                ((I2C_TypeDef *)(I2C1_BASE))
#define I2C1_SCL_Pin        GPIO_PIN_6
#define I2C1_SDA_Pin        GPIO_PIN_7
#define I2C1_SCL_Port       GPIO1
#define I2C1_SDA_Port       GPIO1
#define I2C_GPIO_SCL_Mode   GPIO_MODE_ALTFUN_3
#define I2C_GPIO_SDA_Mode   GPIO_MODE_ALTFUN_3

// ADC ===========================================
#define ADC_IN0_Pin     GPIO_PIN_0
#define ADC_IN0_Port    GPIO_PIN_0
#define ADC_IN1_Pin     GPIO_PIN_1
#define ADC_IN1_Port    GPIO_PIN_1
#define ADC_IN2_Pin     GPIO_PIN_2
#define ADC_IN2_Port    GPIO_PIN_2
#define ADC_IN3_Pin     GPIO_PIN_3
#define ADC_IN3_Port    GPIO_PIN_3
#define ADC_IN4_Pin     GPIO_PIN_4
#define ADC_IN4_Port    GPIO_PIN_4
#define ADC_IN5_Pin     GPIO_PIN_5
#define ADC_IN5_Port    GPIO_PIN_5
#define ADC_IN6_Pin     GPIO_PIN_6
#define ADC_IN6_Port    GPIO_PIN_6
#define ADC_IN7_Pin     GPIO_PIN_7
#define ADC_IN7_Port    GPIO_PIN_7
#define ADC_VREF_P_Pin  GPIO_PIN_4
#define ADC_VREF_N_Pin  GPIO_PIN_3
#define ADC_VeREF_P_Pin GPIO_PIN_4
#define ADC_VeREF_N_Pin GPIO_PIN_3
#define ADC_TSENSE_IN   INCH_10

// CA ===========================================
#define CA_IN0_Pin      GPIO_PIN_0
#define CA_IN1_Pin      GPIO_PIN_1
#define CA_IN2_Pin      GPIO_PIN_2
#define CA_IN3_Pin      GPIO_PIN_3
#define CA_IN4_Pin      GPIO_PIN_4
#define CA_IN5_Pin      GPIO_PIN_5
#define CA_IN6_Pin      GPIO_PIN_6
#define CA_IN7_Pin      GPIO_PIN_7
#define CAOUT0_Pin      GPIO_PIN_3
#define CAOUT1_Pin      GPIO_PIN_7


// PWM ===========================================
#define PWM0_OUT0_Pin   GPIO_PIN_1
#define PWM0_OUT1_Pin   GPIO_PIN_2
#define PWM1_OUT0_Pin   GPIO_PIN_0
#define PWM1_OUT1_Pin   GPIO_PIN_1
#define PWM1_OUT2_Pin   GPIO_PIN_4

// TIMER ===========================================
#define TA0_INCLK_Pin   GPIO_PIN_0
#define TA0_OUT0_Pin    GPIO_PIN_1
#define TA0_OUT1_Pin    GPIO_PIN_2
#define TA0_CCI0A_Pin   GPIO_PIN_1
#define TA0_CCI1A_Pin   GPIO_PIN_2
#define TA1_INCLK_Pin   GPIO_PIN_0
#define TA1_OUT0_Pin    GPIO_PIN_0
#define TA1_OUT1_Pin    GPIO_PIN_1
#define TA1_OUT2_Pin    GPIO_PIN_4
#define TA1_CCI0A_Pin   GPIO_PIN_0
#define TA1_CCI1A_Pin   GPIO_PIN_1
#define TA1_CCI0B_Pin   GPIO_PIN_3
#define TA1_CCI1B_Pin   GPIO_PIN_2
#define TA1_CCI2B_Pin   GPIO_PIN_4

// Clock ===========================================
#define ACLK_Pin        GPIO_PIN_0
#define ADC10CLK_Pin    GPIO_PIN_3
#define SMCLK_Pin       GPIO_PIN_4
#define XIN_Pin         GPIO_PIN_6
#define XOUT_Pin        GPIO_PIN_7

// CAP ===========================================
#define CAP1_IN0_Pin    GPIO_PIN_0
#define CAP1_IN1_Pin    GPIO_PIN_1
#define CAP1_IN2_Pin    GPIO_PIN_2
#define CAP1_IN3_Pin    GPIO_PIN_3
#define CAP1_IN4_Pin    GPIO_PIN_4
#define CAP1_IN5_Pin    GPIO_PIN_5
#define CAP1_IN6_Pin    GPIO_PIN_6
#define CAP1_IN7_Pin    GPIO_PIN_7
#define CAP2_IN0_Pin    GPIO_PIN_0
#define CAP2_IN1_Pin    GPIO_PIN_1
#define CAP2_IN2_Pin    GPIO_PIN_2
#define CAP2_IN3_Pin    GPIO_PIN_3
#define CAP2_IN4_Pin    GPIO_PIN_4
#define CAP2_IN5_Pin    GPIO_PIN_5
#define CAP2_IN6_Pin    GPIO_PIN_6
#define CAP2_IN7_Pin    GPIO_PIN_7
#define CAP1_IN0_Port   GPIO1
#define CAP1_IN1_Port   GPIO1
#define CAP1_IN2_Port   GPIO1
#define CAP1_IN3_Port   GPIO1
#define CAP1_IN4_Port   GPIO1
#define CAP1_IN5_Port   GPIO1
#define CAP1_IN6_Port   GPIO1
#define CAP1_IN7_Port   GPIO1
#define CAP2_IN0_Port   GPIO2
#define CAP2_IN1_Port   GPIO2
#define CAP2_IN2_Port   GPIO2
#define CAP2_IN3_Port   GPIO2
#define CAP2_IN4_Port   GPIO2
#define CAP2_IN5_Port   GPIO2
#define CAP2_IN6_Port   GPIO2
#define CAP2_IN7_Port   GPIO2

/* Exported macro ------------------------------------------------------------*/
#define IS_GPIO_ALL_INSTANCE(INSTANCE)  ( ((INSTANCE) == GPIO1) || \
                                        ((INSTANCE) == GPIO2))

#define IS_UART_ALL_INSTANCE(INSTANCE) ((INSTANCE) == UART1)

#define IS_SPI_ALL_INSTANCE(INSTANCE) ( ((INSTANCE) == SPI1) || ((INSTANCE) == SPI2))

#define IS_I2C_ALL_INSTANCE(INSTANCE) ((INSTANCE) == I2C1)

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

 #endif /* DRIVERS_MSP430X2XX_H_ */

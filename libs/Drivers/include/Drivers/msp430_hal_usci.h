/**
  ******************************************************************************
  * @file       msp430_hal_gpio.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions prototypes for the HAL
  *             USCI module driver.
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
#ifndef DRIVERS_MSP430_HAL_USCI_H_
#define DRIVERS_MSP430_HAL_USCI_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430x2xx.h>

/* Exported types ------------------------------------------------------------*/
typedef void(* USCICallback_t)(void *);

typedef struct
{
    uint8_t Module;             // USCIA, USCIB
    uint8_t Instance;           // USCIAx,  USCIBx
    uint8_t Mode;               // SPI, I2C, UART, | TX, RX
    USCICallback_t Callback;    // USCI callback
    void *Argin;
} usci_callback_config_t;


/* Exported constants --------------------------------------------------------*/
#define USCI_MODULE_A     (0)
#define USCI_MODULE_B     (1)
#define USCI_MODULE_MAX   (2)

#define USCI_INSTANCES_MAX (1)

#define USCIA_MODE_MAX (2)
#define USCIB_MODE_MAX (2)

#define USCIA_MODE_SPI_TX   (0x00)
#define USCIA_MODE_SPI_RX   (0x02)
#define USCIA_MODE_UART_TX  (0x01)
#define USCIA_MODE_UART_RX  (0x03)
#define USCIB_MODE_SPI_TX   (0x00)
#define USCIB_MODE_SPI_RX   (0x02)
#define USCIB_MODE_I2C_TX   (0x01)
#define USCIB_MODE_I2C_RX   (0x03)
#define USCI_MODE_MAX       (0x04)

#define USCI_ERROR_NONE         (0)
#define USCI_ERROR_INV_TYPE     (1)
#define USCI_ERROR_INV_INSTANCE (2)
#define USCI_ERROR_INV_ID       (3)
#define USCI_ERROR_INV_TXRX     (4)
#define USCI_ERROR_BUSY         (5)
#define USCI_ERROR_DIFF         (6)

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
uint16_t HAL_USCI_Intr_Alloc(usci_callback_config_t *cfg);
uint16_t HAL_USCI_Intr_Free(usci_callback_config_t *cfg);

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_MSP430_HAL_USCI_H_ */

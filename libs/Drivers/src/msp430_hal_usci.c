/**
  ******************************************************************************
  * @file       msp430_hal_gpio.h
  * @author     Fernando Hermosillo Reynoso
  * @brief      This file contains all the functions references for the HAL
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

/* Includes ------------------------------------------------------------------*/
#include <Drivers/msp430_hal_usci.h>

/* Exported types ------------------------------------------------------------*/

/* Private constants --------------------------------------------------------*/
#ifndef NULL
#define NULL    ((void *)0)
#endif

#define USCI_INTR_ALLOC_M    (0x80)
#define USCI_INTR_MODE_M     (0x03)
#define USCI_INTR_TX_M       (0x02)

/* Exported macro ------------------------------------------------------------*/
typedef struct
{
    USCICallback_t cb;
    void *argin;
    uint8_t status;
} usci_intr_inst_t;
typedef struct
{
    usci_intr_inst_t intr_tx;
    usci_intr_inst_t intr_rx;
} usci_intr_t;

/* Exported variables --------------------------------------------------------*/
static usci_intr_t usci_intr_vector[USCI_MODULE_MAX] = {0};

/* Exported functions --------------------------------------------------------*/
uint16_t HAL_USCI_Intr_Alloc(usci_callback_config_t *cfg)
{
    assert_param_ret(cfg->Module < USCI_MODULE_MAX, USCI_ERROR_INV_TYPE);
    assert_param_ret(cfg->Mode < USCI_MODE_MAX, USCI_ERROR_INV_ID);

    // TX or RX
    usci_intr_inst_t * usci_intr_inst_1 = &usci_intr_vector[cfg->Module].intr_rx;
    usci_intr_inst_t * usci_intr_inst_2 = &usci_intr_vector[cfg->Module].intr_tx;
    if(cfg->Mode & USCI_INTR_TX_M)
    {
        usci_intr_inst_t *usci_intr_aux =  usci_intr_inst_1;
        usci_intr_inst_1 = usci_intr_inst_2;
        usci_intr_inst_2 = usci_intr_aux;
    }

    // Check if it is allocated
    assert_param_ret(!(usci_intr_inst_1->status & USCI_INTR_ALLOC_M), USCI_ERROR_BUSY);

    // Check if it is other type
    if((usci_intr_inst_2->status & USCI_INTR_ALLOC_M))
    {
        assert_param_ret(usci_intr_inst_2->status & USCI_INTR_MODE_M == cfg->Mode, USCI_ERROR_DIFF);
    }

    // I2C interrupt always runs on TX
    usci_intr_inst_1->status = cfg->Mode | USCI_INTR_ALLOC_M;
    usci_intr_inst_1->cb = cfg->Callback;
    usci_intr_inst_1->argin = cfg->Argin;

    return USCI_ERROR_NONE;
}




uint16_t HAL_USCI_Intr_Free(usci_callback_config_t *cfg)
{
    assert_param_ret(cfg->Module < USCI_MODULE_MAX, USCI_ERROR_INV_TYPE);
    assert_param_ret(cfg->Mode < USCI_MODE_MAX, USCI_ERROR_INV_ID);

    // TX or RX
    usci_intr_inst_t * usci_intr_inst_1 = &usci_intr_vector[cfg->Module].intr_rx;
    usci_intr_inst_t * usci_intr_inst_2 = &usci_intr_vector[cfg->Module].intr_tx;
    if(cfg->Mode & USCI_INTR_TX_M)
    {
        usci_intr_inst_t *usci_intr_aux =  usci_intr_inst_1;
        usci_intr_inst_1 = usci_intr_inst_2;
        usci_intr_inst_2 = usci_intr_aux;
    }

    // Check if it is allocated
    assert_param_ret(!(usci_intr_inst_1->status & USCI_INTR_ALLOC_M), USCI_ERROR_BUSY);

    // Check if it is same type
    assert_param_ret(usci_intr_inst_1->status & USCI_INTR_MODE_M == cfg->Mode, USCI_ERROR_DIFF);

    // Free interrupt allocation
    usci_intr_inst_1->cb = NULL;
    usci_intr_inst_1->status = 0x00;
    usci_intr_inst_1->argin = NULL;

    return USCI_ERROR_NONE;
}


/* Interrupt service routines ------------------------------------------------*/
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_IRQHandle(void)
{
    /* USCIA0 */
    if(usci_intr_vector[USCI_MODULE_A].intr_rx.status & USCI_INTR_ALLOC_M)
    {
        if(usci_intr_vector[USCI_MODULE_A].intr_rx.cb)
        {
            usci_intr_vector[USCI_MODULE_A].intr_rx.cb(usci_intr_vector[USCI_MODULE_A].intr_rx.argin);
        }
    }

    /* USCIB0 */
    if(usci_intr_vector[USCI_MODULE_B].intr_rx.status & USCI_INTR_ALLOC_M)
    {
        if(usci_intr_vector[USCI_MODULE_B].intr_rx.cb)
        {
            usci_intr_vector[USCI_MODULE_B].intr_rx.cb(usci_intr_vector[USCI_MODULE_B].intr_rx.argin);
        }
    }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_IRQHandle(void)
{
    /* USCIA0 */
    if(usci_intr_vector[USCI_MODULE_A].intr_tx.status & USCI_INTR_ALLOC_M)
    {
        if(usci_intr_vector[USCI_MODULE_A].intr_tx.cb)
        {
            usci_intr_vector[USCI_MODULE_A].intr_tx.cb(usci_intr_vector[USCI_MODULE_A].intr_tx.argin);
        }
    }

    /* USCIB0 */
    if(usci_intr_vector[USCI_MODULE_B].intr_tx.status & USCI_INTR_ALLOC_M)
    {
        if(usci_intr_vector[USCI_MODULE_B].intr_tx.cb)
        {
            usci_intr_vector[USCI_MODULE_B].intr_tx.cb(usci_intr_vector[USCI_MODULE_B].intr_tx.argin);
        }
    }
}
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*
 * msp430_lauchpad_io.c
 *
 *  Created on: 15 mar 2024
 *      Author: User123
 */

#include <Drivers/msp430_hal_gpio.h>

#define GPIO_MODE_M (0x07)
#define GPIO_PULL_M (0x18)

void HAL_GPIO_Reset(void)
{
    // P1
    P1DIR = 0x00;
    P1SEL  = 0x00;
    P1SEL2 = 0x00;
    P1REN = 0x00;
    P1IE = 0x00;
    P1IES = 0x00;
    P1OUT = 0x00;
    // P2
    P2DIR = 0x00;
    P2SEL  = 0x00;
    P2SEL2 = 0x00;
    P2REN = 0x00;
    P2IE = 0x00;
    P2IES = 0x00;
    P2OUT = 0x00;
    // P3
    P3DIR = 0x00;
    P3SEL  = 0x00;
    P3SEL2 = 0x00;
    P3REN = 0x00;
    P3OUT = 0x00;

}


void HAL_GPIO_Init(GPIO_TypeDef_t *GPIOx, const GPIO_InitTypeDef *GPIO_Init)
{
    // Assert
    assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));

    // Set PSEL2
    PSEL2_TypeDef_t *PxSEL2 = PSEL2;
    if(GPIOx == GPIO2) PxSEL2++;

    // Mode
    switch(GPIO_Init->Mode & GPIO_MODE_M)
    {
        case GPIO_MODE_INPUT:
            GPIOx->PDIR &= ~GPIO_Init->Pin;
            if(GPIO_Init->Mode & GPIO_PULL_M)
            {
                GPIOx->PREN |= GPIO_Init->Pin;

                if(GPIO_Init->Mode & GPIO_PULL_DOWN)
                {
                    GPIOx->POUT &= ~GPIO_Init->Pin;
                }
                else if(GPIO_Init->Mode & GPIO_PULL_UP)
                {
                    GPIOx->POUT |= GPIO_Init->Pin;
                }
            }
        break;

        case GPIO_MODE_OUTPUT:
            GPIOx->PDIR |= GPIO_Init->Pin;
        break;

        case GPIO_MODE_ALTFUN_0:
            GPIOx->PSEL  &= ~GPIO_Init->Pin;
            *PxSEL2 &= ~GPIO_Init->Pin;
        break;

        case GPIO_MODE_ALTFUN_1:
            GPIOx->PSEL  |= GPIO_Init->Pin;
            *PxSEL2 &= ~GPIO_Init->Pin;
        break;

        case GPIO_MODE_ALTFUN_2:
            GPIOx->PSEL  &= ~GPIO_Init->Pin;
            *PxSEL2 |= GPIO_Init->Pin;
        break;

        case GPIO_MODE_ALTFUN_3:
            GPIOx->PSEL  |= GPIO_Init->Pin;
            *PxSEL2 |= GPIO_Init->Pin;
        break;

        case GPIO_MODE_ANALOG:
            if(GPIOx == GPIO1) ADC10AE0 |= GPIO_Init->Pin;
        break;

        default:
        break;
    }
}

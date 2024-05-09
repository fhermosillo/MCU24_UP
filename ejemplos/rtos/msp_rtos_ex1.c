#include <msp430.h> 
#include <UpRTOS/UpRTOS.h>


#define BUTTON_Pin  BIT3
#define LED_RED_Pin BIT0

QueueHandle_t hQueue;

void vTask1(void *pvArgs)
{
    int ulButtonCounter = 0;

    // GPIO Button
    P1DIR &= ~BUTTON_Pin;
    P1DIR |= BIT6;
    P1REN |= BUTTON_Pin;
    P1OUT |= BUTTON_Pin;

    while(1)
    {
        if( !(P1IN & BUTTON_Pin) )
        {
            P1OUT ^= BIT6;
            ulButtonCounter++;
            xQueueSend(hQueue, &ulButtonCounter, 100);
        }
        vTaskDelay(250);
    }
}

void vTask2(void *pvArgs)
{
    int ulButtonCounter = 0;

    P1DIR |= LED_RED_Pin;
    P1OUT &= ~LED_RED_Pin;

    while(1)
    {
        if( xQueueReceive(hQueue, &ulButtonCounter, portMAX_DELAY) )
        {
            //
            if(ulButtonCounter >= 10)
            {
                P1OUT |= LED_RED_Pin;
            }
        }
        //vTaskDelay(100);
    }
}

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	/* Clock Config */
	DCOCTL = CALDCO_16MHZ;
	BCSCTL1= CALBC1_16MHZ;

	/* GPIO Config */

	/* Queue */
	hQueue = xQueueCreate(5, sizeof(int));

	/* Add tasks */
	xTaskCreate(vTask1, 70, NULL, 1, NULL);
	xTaskCreate(vTask2, 70, NULL, 1, NULL);

	// Arranque del RTOS
	vTaskStartScheduller();

	return 0;
}

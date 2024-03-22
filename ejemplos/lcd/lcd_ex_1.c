/* Includes --------------------------------------------*/

#include <msp430.h>

/* Defines ---------------------------------------------*/
// Clock
#define configCPU_CLOCK_HZ	(1000000UL)
// LCD Control signals
#define LCD_RS_Pin  BIT0
#define LCD_RW_Pin  BIT1
#define LCD_E_Pin	BIT2
// LCD Data signals
#define LCD_DB0_Pin BIT0
#define LCD_DB1_Pin BIT1
#define LCD_DB2_Pin BIT2
#define LCD_DB3_Pin BIT3
#define LCD_DB4_Pin BIT4
#define LCD_DB5_Pin BIT5
#define LCD_DB6_Pin BIT6
#define LCD_DB7_Pin BIT7
#define LCD_DB_Port	P2OUT
// LCD Timings (Adjust according to datasheet)
#define LCD_TPW_DELAY  1
#define LCD_CLR_DELAY 1
#define LCD_ADD_DELAY 1
#define LCD_WDD_DELAY 1
int main(void)
{
	// Clock config =======================
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;

  // I/O config =========================
  P1OUT = 0x00;
  P2OUT = 0x00;
  P1SEL = 0x00;
  P1SEL2 = 0x00;
  P2SEL = 0x00;
  P2SEL2 = 0x00;
  P1REN = 0x00;
  P2REN = 0x00;
  P1DIR |= LCD_RS_Pin | LCD_RW_Pin | LCD_E_Pin;
  P2DIR = 0xFF; // All as Output
  
	// LCD initialization
	// .
	// .
	// .
	
	/* LCD command example */
    // Clear display command
    P1OUT &= ~LCD_RW_Pin;
    P1OUT &= ~LCD_RS_Pin;
    LCD_DB_Port = 0b00000001;
    P1OUT |= LCD_E_Pin;
    __delay_cycles(LCD_TPW_DELAY);
    P1OUT &= ~LCD_E_Pin;
    __delay_cycles(LCD_CLR_DELAY);
	
    // Set DDRAM Address = 0x48
    P1OUT = P1OUT & ~LCD_RW_Pin;
    P1OUT = P1OUT & ~LCD_RS_Pin;
    P2OUT = 0b11001000;
    P1OUT |= LCD_E_Pin;
    __delay_cycles(LCD_TPW_DELAY);
    P1OUT &= ~LCD_E_Pin;
	  __delay_cycles(LCD_ADD_DELAY);
  
    // Write DDRAM = 'g' = 0x67
    P1OUT = P1OUT & ~LCD_RW_Pin;
    P1OUT = P1OUT | LCD_RS_Pin;
    P2OUT = 0b01100111;
    P1OUT |= LCD_E_Pin;
    __delay_cycles(LCD_TPW_DELAY);
    P1OUT &= ~LCD_E_Pin;
  __delay_cycles(LCD_WDD_DELAY);
	
	while (1)
	{
	}
}

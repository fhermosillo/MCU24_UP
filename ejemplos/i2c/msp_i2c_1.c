#include <msp430.h>

#define I2C_SCL_Pin     BIT6
#define I2C_SDA_Pin     BIT7

void HAL_I2C_Init(void);

void main(void)
{
  /* WDT config */
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT

  /* BCM config */
  BCSCTL1 = CALBC1_1MHZ;  // Set DCO to 1MHz
  DCOCTL  = CALDCO_1MHZ;  // Set DCO to 1MHz

  /* GPIO config */
  // SDA & SCL pin configuration
  P1SEL  |= I2C_SDA_Pin | I2C_SCL_Pin;
  P1SEL2 |= I2C_SDA_Pin | I2C_SCL_Pin;

  /* I2C config */
  HAL_I2C_Init();

  
  while(1)
  {
    HAL_I2C_Write(0x81,0x3F); // Write to slave with address 0x81 the value 0x3F
    __delay_cycles(1000000); // Delay 1second
  }
}


void HAL_I2C_Init(void)
{
  // USCI logic held in reset state
  UCB0CTL1 = UCSWRST;
  // Control register 0
  // Master
  // USCIB0 I2C mode
  // Synchronous mode
  UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

  // Control register 1
  // BRCLK = SMCLK = 1MHz
  UCB0CTL1 |= UCSSEL_2;

  // Bits per second (100Kbps)
  // BRCLK / UCA0BR = 1MHz/10 = 100Kbps
  UCB0BR0 = 0x0A;
  UCB0BR1 = 0x00;

  // USCI reset released for operation
  UCB0CTL1 &= ~UCSWRST;

  // Small delay to ensure power on
  __delay_cyles(40000);
}

int HAL_I2C_Write(uint8_t Address, uint8_t Data)
{
  // Wait until I2C STOP is sent
  while (UCB0CTL1 & UCTXSTP);

  // Set slave address
  UCB0I2CSA = Address;

  // Generate start condition
  UCB0CTL1 |= UCTR | UCTXSTT;

  // Wait for the start condition to be sent and ready to transmit flags
  while( !(IFG2 & UCB0TXIFG) );

  // Check acknowledge
  if (UCB0STAT & UCNACKIFG)
  {
      // Stop the I2C transmission
      UCB0CTL1 |= UCTXSTP;

      // Clear the interrupt flag
      UCB0STAT &= ~UCNACKIFG;

      return 0;
  }

  // Write data
  UCB0TXBUF = Data;
  while(!(IFG2 & UCB0TXIFG));

  // Send stop condition
  UCB0CTL1 |= UCTXSTP;
  while((UCB0CTL1 & UCTXSTP));

  return 1;
}

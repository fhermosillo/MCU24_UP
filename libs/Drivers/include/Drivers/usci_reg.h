/*
 * usci_reg.h
 *
 *  Created on: 2 abr 2024
 *      Author: User123
 */

#ifndef DRIVERS_USCI_REG_H_
#define DRIVERS_USCI_REG_H_

#define USCI_CTL0_CPH_S     (7)
#define USCI_CTL0_CPOL_S    (6)
#define USCI_CTL0_PEN_M     BIT7
#define USCI_CTL0_PEN_S     (7)
#define USCI_CTL0_PAR_M     BIT6
#define USCI_CTL0_PAR_S     (6)
#define USCI_CTL0_MSB_M     BIT5
#define USCI_CTL0_MSB_S     (5)
#define USCI_CTL0_7BIT_M    BIT4
#define USCI_CTL0_7BIT_S    (4)
#define USCI_CTL0_SPB_M     BIT3
#define USCI_CTL0_SPB_S     (3)
#define USCI_CTL0_MODE_M    (BIT2 | BIT1)
#define USCI_CTL0_MODE_S    (1)
#define USCI_CTL0_SYNC_M    BIT0
#define USCI_CTL0_SYNC_S    (0)

#define USCI_CTL1_SSEL_M    (BIT7 | BIT6)
#define USCI_CTL1_SSEL_S    (6)
#define USCI_CTL1_RXEIE_M   BIT5
#define USCI_CTL1_RXEIE_S   (5)
#define USCI_CTL1_BRKIE_M   BIT4
#define USCI_CTL1_BRKIE_S   (4)
#define USCI_CTL1_DORM_M    BIT3
#define USCI_CTL1_DORM_S    (3)
#define USCI_CTL1_TXADR_M   BIT2
#define USCI_CTL1_TXADR_S   (2)
#define USCI_CTL1_TXBRK_M   BIT1
#define USCI_CTL1_TXBRK_S   (1)
#define USCI_CTL1_SWRST_M   BIT0
#define USCI_CTL1_SWRST_S   (0)

#define USCI_CTL1_BRF_M     (0xF0)
#define USCI_CTL1_BRF_S     (4)
#define USCI_CTL1_BRS_M     (BIT3 | BIT2 | BIT1)
#define USCI_CTL1_BRS_S     (1)
#define USCI_CTL1_OS16_M    BIT0
#define USCI_CTL1_OS16_S    (0)


#endif /* DRIVERS_USCI_REG_H_ */

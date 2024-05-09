/*
 * lcd.h
 *
 *  Created on: 21 mar 2024
 *      Author: User123
 */

#ifndef DRIVERS_HD44780_H_
#define DRIVERS_HD44780_H_

/* Exported Includes -------------------------------------------*/
#include <Drivers/msp430_hal.h>

/* Exported Constants ------------------------------------------*/
// LCD Constants
#define HD44780_NUM_ROWS    (2)
#define HD44780_NUM_COLS    (16)
#define HD44780_ROW_0       (0)
#define HD44780_ROW_1       (1)
#define HD44780_ROW_2       (2)
#define HD44780_ROW_3       (3)
// LCD Control signals (Port = P1)
#define HD44780_RS_PIN      BIT0
#define HD44780_RW_PIN      BIT1
#define HD44780_E_PIN       BIT2
#define HD44780_CTRL_PORT   P1OUT
#define HD44780_CTRL_DIR    P1DIR
// LCD Data signals (Port = P2)
#define HD44780_DB_PORT     P2OUT
#define HD44780_DB_DIR      P2DIR
// LCD Commands
#define HD44780_CLR_CMD     BIT0
#define HD44780_HOME_CMD    BIT1
#define HD44780_ENTRY_CMD   BIT2
#define HD44780_DISPLAY_CMD BIT3
#define HD44780_CURSOR_CMD  BIT4
#define HD44780_FUNCSET_CMD BIT5
#define HD44780_SET_ACG_CMD BIT6
#define HD44780_SET_ADD_CMD BIT7
// LCD Command BIT
#define HD44780_ID_BIT      BIT1
#define HD44780_S_BIT       BIT0
#define HD44780_D_BIT       BIT2
#define HD44780_C_BIT       BIT1
#define HD44780_B_BIT       BIT0
#define HD44780_SC_BIT      BIT3
#define HD44780_RL_BIT      BIT2
#define HD44780_DL_BIT      BIT4
#define HD44780_N_BIT       BIT3
#define HD44780_F_BIT       BIT2
#define HD44780_BUSY        BIT7

// Flags for display entry mode
#define HD44780_SHIFT       BIT0
#define HD44780_NO_SHIFT    (0x00)
#define HD44780_INCREMENT   BIT1
#define HD44780_DECREMENT   (0x00)
// Flags for display on/off control
#define HD44780_DISPLAY_ON  BIT2
#define HD44780_DISPLAY_OFF (0x00)
#define HD44780_CURSOR_ON   BIT1
#define HD44780_CURSOR_OFF  (0x00)
#define HD44780_BLINK_ON    BIT0
#define HD44780_BLINK_OFF   (0x00)
// flags for display/cursor shift
#define HD44780_DISPLAY_SHIFT    BIT3
#define HD44780_CURSOR_MOVE     (0x00)
#define HD44780_SHIFT_RIGHT     BIT2
#define HD44780_SHIFT_LEFT      (0x00)
// Flags for function set instruction
#define HD44780_8BIT_MODE   BIT4
#define HD44780_4BIT_MODE   (0x00)
#define HD44780_2_LINES     BIT3
#define HD44780_1_LINES     (0x00)
#define HD44780_FONT_5X10   BIT2
#define HD44780_FONT_5X8    (0x00)
// Init mode
#define HD44780_INIT_8BITS  0x01
#define HD44780_INIT_4BITS  0x02
#define HD44780_INIT_SERIAL 0x0C

/* Exported Macros -----------------------------------*/
#define HD44780_Write_Data(DT) HD44780_Write(DT, 1)

#define HD44780_Write_Command(CMD) HD44780_Write(CMD, 0)

#define HD44780_Home()  HD44780_Write_Command(HD44780_HOME_CMD);\
                        HAL_Delay_Millis(2);

#define HD44780_Clear_Screen()  HD44780_Send_Command(HD44780_CLR_CMD); \
                                HAL_Delay_Millis(2);

/* Exported Functions ---------------------------------*/
unsigned int HD44780_Init(unsigned char init_mode); // TODO: Add 4-bits init sequence
void HD44780_Write(unsigned char data, unsigned char data_ncmd);
void HD44780_Write_String(const char *str);
void HD44780_Set_Cursor(unsigned char ucRow, unsigned char ucCol);      // TODO: Must return
void HD44780_Set_Bitmap(unsigned char location, unsigned char *bitmap); // TODO: Must return

#endif /* DRIVERS_HD44780_H_ */

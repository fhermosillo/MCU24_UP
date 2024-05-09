/*
 * glcd.h
 *
 *  Created on: 21 mar 2024
 *      Author: User123
 */

#ifndef DRIVERS_ST7920_H_
#define DRIVERS_ST7920_H_

/* Exported Includes -------------------------------------------*/
#include <Drivers/msp430_hal.h>

/* Exported typedefs -------------------------------------------*/
typedef enum
{
    ST7920_MODE_TEXT = 0,
    ST7920_MODE_GRAPHICS
} st7920_mode_t;

typedef enum
{
    ST7920_PIXEL_OFF=0,
    ST7920_PIXEL_ON
} st7920_pixel_t;

/* Exported Constants ------------------------------------------*/
// Config
#define ST7920_INTERFACE_4BITS  (0) /* DB<7:4> <- ST7920_DB_OUT<3:0> */
#define ST7920_INTERFACE_8BITS  (1) /* DB<7:0> <- ST7920_DB_OUT<7:0> */
#define ST7920_INTERFACE_SERIAL (2)
// Constants (Text mode)
#define ST7920_MAX_ROWS   (4)
#define ST7920_MAX_COLS   (16)
#define ST7920_ROW_0      (0)
#define ST7920_ROW_1      (1)
#define ST7920_ROW_2      (2)
#define ST7920_ROW_3      (3)
// Constants (Graphics mode)
#define ST7920_WIDTH      (128)
#define ST7920_HEIGHT     (64)
#define ST7920_MAX_GROWS  (64)
#define ST7920_MAX_GCOLS  (16)
// Control signals (Port = P1)
#define ST7920_RS_PIN      BIT0
#define ST7920_RW_PIN      BIT1
#define ST7920_E_PIN       BIT2
#define ST7920_CTRL_OUT    P1OUT
#define ST7920_CTRL_DIR    P1DIR
// Data signals (Port = P2)
#define ST7920_DB_OUT      P2OUT
#define ST7920_DB_IN       P2IN
#define ST7920_DB_DIR      P2DIR

// Commands [Basic/Text Mode]
#define ST7920_CLR_CMD     BIT0
#define ST7920_HOME_CMD    BIT1
#define ST7920_ENTRY_CMD   BIT2
#define ST7920_DISPLAY_CMD BIT3
#define ST7920_CURSOR_CMD  BIT4
#define ST7920_FUNCSET_CMD BIT5
#define ST7920_SET_ACG_CMD BIT6
#define ST7920_SET_ADD_CMD BIT7
// Commands [Extended/Graphics Mode]
#define ST7920_STANDBY_EXTCMD
#define ST7920_SCROLL_ADDRRAM_EXTCMD
#define ST7920_REVERSE_EXTCMD
#define ST7920_FUNCSET_EXTCMD
#define ST7920_SCROLL_ADDR_EXTCMD
#define ST7920_GRAPHIC_RAM_EXTCMD
// Flags for display entry mode
#define ST7920_INCREMENT  BIT1
#define ST7920_DECREMENT  (0x00)
#define ST7920_NO_SHIFT   (0x00)
#define ST7920_SHIFT      BIT0
// Flags for display on/off control
#define ST7920_DISPLAY_ON  BIT2
#define ST7920_DISPLAY_OFF (0x00)
#define ST7920_CURSOR_ON   BIT1
#define ST7920_CURSOR_OFF  (0x00)
#define ST7920_BLINK_ON    BIT0
#define ST7920_BLINK_OFF   (0x00)
// flags for display/cursor shift
#define ST7920_DISPLAY_MOVE    BIT3
#define ST7920_CURSOR_MOVE     (0x00)
#define ST7920_MOVE_RIGHT      BIT2
#define ST7920_MOVE_LEFT       (0x00)
// Flags for function set instruction
#define ST7920_8BIT_MODE      BIT4
#define ST7920_4BIT_MODE      (0x00)
#define ST7920_EXTENDED_MODE  BIT2
#define ST7920_BASIC_MODE     (0x00)
#define ST7920_GRAPHICS_ON    BIT1
#define ST7920_GRAPHICS_OFF   (0x00)
// Extended Commands
#define ST7920_SET_AGD_CMD    (0x80)

#define ST7920_COMMAND (0)
#define ST7920_DATA    (1)
//
#define ST7920_GRAPHICS_MODE   ST7920_FUNCSET_CMD | ST7920_8BIT_MODE | ST7920_EXTENDED_MODE
/* Exported Macros -----------------------------------*/
#define ST7920_Write_Data(DT) ST7920_Write(DT, ST7920_DATA)

#define ST7920_Write_Command(CMD) ST7920_Write(CMD, ST7920_COMMAND)

#define ST7920_Home()   ST7920_Write_Command(ST7920_HOME_CMD);\
                        delay_ms(2);

#define ST7920_Clear_Screen()   ST7920_Write_Command(ST7920_FUNCSET_CMD | ST7920_8BIT_MODE | ST7920_BASIC_MODE);\
                                ST7920_Write_Command(ST7920_CLR_CMD); \
                                delay_ms(2);


#define ST7920_INVERT_ROW_0   (0x00)
#define ST7920_INVERT_ROW_1   (0x01)
#define ST7920_INVERT_ROW_2   (0x02)
#define ST7920_INVERT_ROW_3   (0x03)
#define ST7920_INVERT_ALL     (0xFF)




// Config
#define st7920ST7920_WAIT_BUSY  (0)
#define st7920INTERFACE_MODE    ST7920_INTERFACE_8BITS

/* Exported Functions ---------------------------------*/
/*
 * @name    ST7920_Init
 *
 * @param   None
 *
 * @return  None
 */
void ST7920_Init(void);

/*
 * @name    ST7920_Write
 * @param[in]   ucDataToWrite
 * @param[in]   ucData_nCmd
 *
 * @return  None
 */
void ST7920_Write(unsigned char ucDataToWrite, unsigned char ucData_nCmd);

/*
 * @name    ST7920_Write_Multi
 * @param[in]   pDataToWrite
 *
 * @return  None
 */
void ST7920_Write_Multi(const unsigned char *pucDataToWrite, unsigned char ucData_nCmd);

/*
 * @name    ST7920_Read
 * @param   None
 * @return  Byte at current address
 * @notes   After address set to read (CGRAM,DDRAM,IRAM…..)a DUMMY READ is required.
 *          There is no need to DUMMY READ for the following bytes unless a new
 *          address set instruction is issued.
 */

uint8_t ST7920_Read(void);
/*
 * @name    ST7920_Read_Multi
 * @param[out]  pBufer
 * @param[in]   len
 *
 * @return  None
 * @notes   After address set to read (CGRAM,DDRAM,IRAM…..)a DUMMY READ is required.
 *          There is no need to DUMMY READ for the following bytes unless a new
 *          address set instruction is issued.
 */
void ST7920_Read_Multi(unsigned char *pBufer, unsigned int uiLen);

// ============================================================
// TEXT MODE
// ============================================================
/*
 * @name    ST7920_Write_String
 * @param[in]   str
 *
 * @return  None
 */
void ST7920_Write_String(const char *pcString);

/*
 * @name    ST7920_Set_Cursor
 * @param[in]   ucRow
 * @param[in]   ucCol
 *
 * @return  None
 */
void ST7920_Set_Cursor(unsigned char ucRow, unsigned char ucCol);

/*
 * @name    ST7920_Print
 * @param[in]   ucRow
 * @param[in]   ucCol
 * @param[in]   str
 *
 * @return  None
 */
void ST7920_Print(unsigned char ucRow, unsigned char ucCol, const char *pcString);

// ============================================================
// GRAPHICS MODE
// ============================================================
/*
 * @name    ST7920_Set_Mode
 * @param[in]   xMode
 *
 * @return  None
 */
void ST7920_Set_Mode(st7920_mode_t xMode);

/*
 * @name    ST7920_SetPos
 * @param[in]   ucRow
 * @param[in]   ucCol
 *
 * @return  None
 */
void ST7920_SetPos(unsigned char ucRow, unsigned char ucCol);

/*
 * @name    ST7920_Set_Pixel
 * @param[in]   x, 0 <= x < ST7920_WIDTH
 * @param[in]   y, 0 <= y < ST7920_HEIGHT
 * @param[in]   xPixelValue
 *
 * @return  None
 */
void ST7920_Set_Pixel(unsigned char x, unsigned char y, st7920_pixel_t xPixelValue);

/*
 * @name    ST7920_Invert
 * @param[in]   ucInvMode
 *
 * @return  None
 */
void ST7920_Invert(unsigned char ucInvMode);

/*
 * @name    ST7920_Set_Bitmap
 * @param[in]   location
 * @param[in]   bitmap
 *
 * @return  None
 */
void ST7920_Set_Bitmap(unsigned char ucLoc, unsigned char *pucBitmap);

/*
 * @name    ST7920_ClearGDRAM
 * @param   None
 *
 * @return  None
 */
void ST7920_ClearGDRAM(void);



#endif /* DRIVERS_ST7920_H_ */

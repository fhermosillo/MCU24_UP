/*
 * lcd.c
 *
 *  Created on: 21 mar 2024
 *      Author: User123
 */

#include <Drivers/hd44780.h>

const uint8_t uLcdRowOffsets[] = {0x00, 0x40, 0x14, 0x54};

void(* HD44780_Write_Callback)(uint8_t,uint8_t);

static void HD44789_Write_8bits(unsigned char data, unsigned char data_ncmd);
static void HD44789_Write_4bits(unsigned char data, unsigned char data_ncmd);
static void HD44789_Write_Serial(unsigned char data, unsigned char data_ncmd);

unsigned int HD44780_Init(unsigned char mode)
{
    if(mode == HD44780_INIT_8BITS) HD44780_Write_Callback = HD44789_Write_8bits;
    else if(mode == HD44780_INIT_4BITS) HD44780_Write_Callback = HD44789_Write_4bits;
    else if(mode == HD44780_INIT_SERIAL) HD44780_Write_Callback = HD44789_Write_Serial;
    else return 1;

    // Wait for more than 40ms
    delay_ms(50);

    // Function set command: 8bits-mode
    HD44780_Write_Command(HD44780_FUNCSET_CMD | HD44780_8BIT_MODE);
    delay_ms(5); // > 4.1ms

    // Repeat command
    HD44780_Write_Command(HD44780_FUNCSET_CMD | HD44780_8BIT_MODE);
    delay_us(200); // > 100us

    // Repeat command
    HD44780_Write_Command(HD44780_FUNCSET_CMD | HD44780_8BIT_MODE);

    // Function set command: 8bits-mode, 2 lines, 5x8 font
    HD44780_Write_Command(HD44780_FUNCSET_CMD | HD44780_8BIT_MODE | HD44780_2_LINES | HD44780_FONT_5X8);

    // Display on, cursor on, blink off
    HD44780_Write_Command(HD44780_DISPLAY_CMD | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_BLINK_OFF);

    // Clear the screen command 0x01
    HD44780_Write_Command(HD44780_CLR_CMD);

    // Entry mode command: increment cursor
    HD44780_Write_Command(HD44780_ENTRY_CMD | HD44780_INCREMENT | HD44780_SHIFT);

    // Set DDRAM address (0,0)
    HD44780_Write_Command(HD44780_SET_ADD_CMD);

    // Another delay why?
    delay_ms(10);

    return 0;
}


void HD44780_Write(unsigned char data, unsigned char data_ncmd)
{
    /*HD44780_CTRL_PORT &= ~HD44780_E_PIN;
    delay_us(1);

    // Data write
    HD44780_CTRL_PORT &= ~HD44780_RW_PIN;   // R/W = 0
    if(isdata) HD44780_CTRL_PORT |= HD44780_RS_PIN;    // RS = 1
    else HD44780_CTRL_PORT &= ~HD44780_RS_PIN;

    // Data to Write to LCD
    HD44780_DB_PORT = data;

    // Execution cycle
    HD44780_CTRL_PORT |= HD44780_E_PIN;
    delay_us(1); // enable pulse must be > 450 ns
    HD44780_CTRL_PORT &= ~HD44780_E_PIN;
    HD44780_DB_PORT = 0x00;

    // Clear outputs
    HD44780_CTRL_PORT &= ~(HD44780_RW_PIN | HD44780_RS_PIN);*/

    // Call write callback
    HD44780_Write_Callback(data, data_ncmd);

    // Instructions need a delay > 37 us to settle
    delay_us(50);
}


static void HD44789_Write_8bits(unsigned char data, unsigned char data_ncmd)
{
    // Clear enable pin
    HD44780_CTRL_PORT &= ~HD44780_E_PIN;
    delay_us(1);

    // LCD write mode (R/W = 0, RS = data_ncmd)
    HD44780_CTRL_PORT &= ~HD44780_RW_PIN;
    if(data_ncmd) HD44780_CTRL_PORT |= HD44780_RS_PIN;
    else HD44780_CTRL_PORT &= ~HD44780_RS_PIN;

    // Data to Write to LCD
    HD44780_DB_PORT = data;

    // Execution cycle
    HD44780_CTRL_PORT |= HD44780_E_PIN;
    delay_us(1); // enable pulse must be > 450 ns
    HD44780_CTRL_PORT &= ~HD44780_E_PIN;
    HD44780_DB_PORT = 0x00;

    // Clear outputs
    HD44780_DB_PORT = 0x00;
    HD44780_CTRL_PORT &= ~(HD44780_RW_PIN | HD44780_RS_PIN);
}

static void HD44789_Write_4bits(unsigned char data, unsigned char data_ncmd)
{
    HD44780_CTRL_PORT &= ~HD44780_E_PIN;
    delay_us(1);

    // LCD write mode (R/W = 0, RS = data_ncmd)
    HD44780_CTRL_PORT &= ~HD44780_RW_PIN;
    if(data_ncmd) HD44780_CTRL_PORT |= HD44780_RS_PIN;
    else HD44780_CTRL_PORT &= ~HD44780_RS_PIN;

    // Execution cycle I: Send four-MSBs
    HD44780_DB_PORT &= ~0x0F;               // Clear DB
    HD44780_DB_PORT = (data >> 4) & 0x0F;
    HD44780_CTRL_PORT |= HD44780_E_PIN;
    delay_us(1);                            // Enable pulse must be > 450 ns
    HD44780_CTRL_PORT &= ~HD44780_E_PIN;

    // Small delay between execution cycles
    delay_us(1);

    // Execution cycle II: Send four-LSBs
    HD44780_DB_PORT &= ~0x0F;               // Clear DB
    HD44780_DB_PORT = data & 0x0F;
    HD44780_CTRL_PORT |= HD44780_E_PIN;     // Enable pulse
    delay_us(1);                            // Tepw > 450 ns
    HD44780_CTRL_PORT &= ~HD44780_E_PIN;

    // Clear outputs
    HD44780_DB_PORT = 0x00;
    HD44780_CTRL_PORT &= ~(HD44780_RW_PIN | HD44780_RS_PIN);
}


static void HD44789_Write_Serial(unsigned char data, unsigned char data_ncmd)
{

}

void HD44780_Write_String(const char *str)
{
    while(*str)
    {
        HD44780_Write_Data(*str++);
    }
}

void HD44780_Set_Cursor(unsigned char ucRow, unsigned char ucCol)
{
    // Position clipping
    if(ucRow >= HD44780_NUM_ROWS) return;
    if(ucCol >= HD44780_NUM_COLS) return;

    HD44780_Write_Command(HD44780_SET_ADD_CMD | ucCol | uLcdRowOffsets[ucRow]);

}

void HD44780_Set_Bitmap(unsigned char location, unsigned char *bitmap)
{
    if(location > 7) return;

    // Set CGRAM Address
    HD44780_Write_Command(HD44780_SET_ACG_CMD | location << 3);

    // Write charmap
    unsigned int i = 0;
    for(i = 0; i < 8; i++)
    {
        HD44780_Write_Data(bitmap[i]);
    }
}

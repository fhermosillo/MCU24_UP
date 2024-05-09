/*
 * glcd.c
 *
 *  Created on: 21 mar 2024
 *      Author: User123
 */

#include <Drivers/st7920.h>

/* Private Constants ------------------------------------------------*/
#if st7920INTERFACE_MODE == ST7920_INTERFACE_8BITS
#define st7920INTERFACE_MODE_BIT    ST7920_8BIT_MODE
#elif st7920INTERFACE_MODE == ST7920_INTERFACE_4BITS
#define st7920INTERFACE_MODE_BIT    ST7920_4BIT_MODE
#elif st7920INTERFACE_MODE == ST7920_INTERFACE_SERIAL
#define st7920INTERFACE_MODE_BIT    ST7920_8BIT_MODE
#else
#error "#Driver/ST7920: Unknown interface mode"
#endif

/* Private Prototype Functions --------------------------------------*/
#if st7920ST7920_WAIT_BUSY==(1)
static void pvrST7920_Wait_Busy(void);
#endif

/* Private variables ------------------------------------------------*/
static const uint8_t ucGlcdRowOffset[] = {0x80,0x90,0x88,0x98};
static st7920_mode_t xMode = ST7920_MODE_TEXT;

#if st7920ST7920_WAIT_BUSY==(1)
static uint8_t ucBusyFlagMask = BIT7;
#endif

void ST7920_Init(void)
{
    // Wait time > 40ms
    HAL_Delay_Millis(50);

    // Function set
    ST7920_Write_Command(ST7920_FUNCSET_CMD | st7920INTERFACE_MODE_BIT | ST7920_BASIC_MODE);
    HAL_Delay_Micros(100); // Wait time > 100us

    // Function set
    ST7920_Write_Command(ST7920_FUNCSET_CMD | st7920INTERFACE_MODE_BIT | ST7920_BASIC_MODE);
    HAL_Delay_Micros(50); // Wait time > 37us

    // Display on, cursor of, blink off
    ST7920_Write_Command(ST7920_DISPLAY_CMD | ST7920_DISPLAY_ON | ST7920_CURSOR_OFF | ST7920_BLINK_OFF);
    HAL_Delay_Micros(100); // Wait time > 100us

    // Clear the screen command 0x01
    ST7920_Write_Command(ST7920_CLR_CMD);
    HAL_Delay_Millis(10); // > 10ms

    // Entry mode command: increment cursor
    ST7920_Write_Command(ST7920_ENTRY_CMD | ST7920_INCREMENT);

    // Set DDRAM address (0,0)
    ST7920_Write_Command(ST7920_SET_ADD_CMD);

    HAL_Delay_Millis(10);

    xMode = ST7920_MODE_TEXT;
}


#if st7920INTERFACE_MODE == ST7920_INTERFACE_8BITS
void ST7920_Write(unsigned char ucDataToWrite, unsigned char ucData_nCmd)
{
    // Data write
    ST7920_CTRL_OUT &= ~ST7920_RW_PIN;             // R/W = 0
    if(ucData_nCmd) ST7920_CTRL_OUT |= ST7920_RS_PIN;   // RS = 1
    else ST7920_CTRL_OUT &= ~ST7920_RS_PIN;

    // DB is output
    ST7920_DB_DIR = 0xFF;

    // Data to send to LCD
    ST7920_DB_OUT = ucDataToWrite;

    // Execution cycle
    ST7920_CTRL_OUT |= ST7920_E_PIN;
    HAL_Delay_Micros(1); // enable pulse must be > 450 ns
    ST7920_CTRL_OUT &= ~ST7920_E_PIN;

    // Instructions need a delay > 72 us to settle
    HAL_Delay_Micros(100);
}

void ST7920_Write_Multi(const unsigned char *pDataToWrite, unsigned char ucData_nCmd)
{
    // Data write
    ST7920_CTRL_OUT &= ~ST7920_RW_PIN;             // R/W = 0
    if(ucData_nCmd) ST7920_CTRL_OUT |= ST7920_RS_PIN;   // RS = 1
    else ST7920_CTRL_OUT &= ~ST7920_RS_PIN;

    // DB is output
    ST7920_DB_DIR = 0xFF;

    while(*pDataToWrite)
    {
        // Data to send to LCD
        ST7920_DB_OUT = *pDataToWrite++;

        // Execution cycle
        ST7920_CTRL_OUT |= ST7920_E_PIN;
        HAL_Delay_Micros(1); // enable pulse must be > 450 ns
        ST7920_CTRL_OUT &= ~ST7920_E_PIN;

        // Instructions need a delay > 72 us to settle
        HAL_Delay_Micros(100);
    }
}

uint8_t ST7920_Read(void)
{
    uint8_t ucRecvData = 0x00;

    // Data write
    ST7920_CTRL_OUT |= ST7920_RW_PIN;  // R/W=1
    ST7920_CTRL_OUT |= ST7920_RS_PIN;  // RS=1

    // DB as input
    ST7920_DB_DIR = 0x00;

    // Execution cycle
    ST7920_CTRL_OUT |= ST7920_E_PIN;
    HAL_Delay_Micros(1);

    // Move lower nibble
    ucRecvData = ST7920_DB_IN;

    // End of execution cycle
    HAL_Delay_Micros(1);
    ST7920_CTRL_OUT &= ~ST7920_E_PIN;

    return ucRecvData;
}

void ST7920_Read_Multi(unsigned char *pBufer, unsigned int len)
{
    // DB as input
    ST7920_DB_DIR = 0x00;

    // Data write
    ST7920_CTRL_OUT |= ST7920_RW_PIN;  // R/W=1
    ST7920_CTRL_OUT |= ST7920_RS_PIN;  // RS=1

    // Execution cycle begin
    while(len--)
    {
        // Move data
        *pBufer = ST7920_DB_IN;

        // Execution cycle
        ST7920_CTRL_OUT |= ST7920_E_PIN;
        HAL_Delay_Micros(1);
        ST7920_CTRL_OUT &= ~ST7920_E_PIN;

        pBufer++;
    }
}

#elif st7920INTERFACE_MODE == ST7920_INTERFACE_4BITS
void ST7920_Write(unsigned char ucDataToWrite, unsigned char ucData_nCmd)
{
    // Data write
    ST7920_CTRL_OUT &= ~ST7920_RW_PIN;             // R/W = 0
    if(ucData_nCmd) ST7920_CTRL_OUT |= ST7920_RS_PIN;   // RS = 1
    else ST7920_CTRL_OUT &= ~ST7920_RS_PIN;

    // DB is output
    ST7920_DB_DIR |= 0x0F;

    // Move higher nibble first
    ST7920_DB_OUT = ST7920_DB_OUT & 0xF0 | ((ucDataToWrite & 0xF0) >> 4);
    ST7920_CTRL_OUT |= ST7920_E_PIN;
    HAL_Delay_Micros(1);
    ST7920_CTRL_OUT &= ~ST7920_E_PIN;

    // Then move the lower nibble
    ST7920_DB_OUT = ST7920_DB_OUT & 0xF0 | ((ucDataToWrite & 0x0F));

    // Execution cycle
    ST7920_CTRL_OUT |= ST7920_E_PIN;
    HAL_Delay_Micros(1); // enable pulse must be > 450 ns
    ST7920_CTRL_OUT &= ~ST7920_E_PIN;

    // Instructions need a delay > 72 us to settle
    HAL_Delay_Micros(100);

}

void ST7920_Write_Multi(const unsigned char *pDataToWrite, unsigned char ucData_nCmd)
{
    // Data write
    ST7920_CTRL_OUT &= ~ST7920_RW_PIN;             // R/W = 0
    if(ucData_nCmd) ST7920_CTRL_OUT |= ST7920_RS_PIN;   // RS = 1
    else ST7920_CTRL_OUT &= ~ST7920_RS_PIN;

    // DB is output
    ST7920_DB_DIR |= 0x0F;

    while(*pDataToWrite++)
    {
        // Data to send to LCD
        // Move higher nibble first
        ST7920_DB_OUT = ST7920_DB_OUT & 0xF0 | ((*pDataToWrite & 0xF0) >> 4);
        ST7920_CTRL_OUT |= ST7920_E_PIN;
        HAL_Delay_Micros(1);
        ST7920_CTRL_OUT &= ~ST7920_E_PIN;

        // Then move the lower nibble
        ST7920_DB_OUT = ST7920_DB_OUT & 0xF0 | ((*pDataToWrite & 0x0F));
        ST7920_CTRL_OUT |= ST7920_E_PIN;
        HAL_Delay_Micros(1); // enable pulse must be > 450 ns
        ST7920_CTRL_OUT &= ~ST7920_E_PIN;

        // Instructions need a delay > 72 us to settle
        HAL_Delay_Micros(100);
    }
}

uint8_t ST7920_Read(void)
{
    uint8_t ucRecvData = 0x00;

    // DB is input
    if(xInteface == ST7920_INTERFACE_8BITS) ST7920_DB_DIR = 0x00;
    else if(xInteface == ST7920_INTERFACE_4BITS) ST7920_DB_DIR &= 0xF0;

    // Data write
    ST7920_CTRL_OUT |= ST7920_RW_PIN;  // R/W=1
    ST7920_CTRL_OUT |= ST7920_RS_PIN;  // RS=1

    // Read Data
    ST7920_DB_DIR &= 0xF0;

    // Execution cycle I
    ST7920_CTRL_OUT |= ST7920_E_PIN;
    HAL_Delay_Micros(1);

    // Move higher nibble first
    ucRecvData |= (ST7920_DB_IN & 0x0F) << 4;

    // End of execution cycle I
    HAL_Delay_Micros(1);
    ST7920_CTRL_OUT &= ~ST7920_E_PIN;

    // Execution cycle II
    ST7920_CTRL_OUT |= ST7920_E_PIN;
    HAL_Delay_Micros(1);

    // Move lower nibble
    ucRecvData |= (ST7920_DB_IN & 0x0F);

    // End of execution cycle II
    HAL_Delay_Micros(1);
    ST7920_CTRL_OUT &= ~ST7920_E_PIN;

    return ucRecvData;
}

void ST7920_Read_Multi(unsigned char *pBufer, unsigned int len)
{
    // DB is input
    ST7920_DB_DIR &= 0xF0;

    // Data write
    ST7920_CTRL_OUT |= ST7920_RW_PIN;  // R/W=1
    ST7920_CTRL_OUT |= ST7920_RS_PIN;  // RS=1

    // Read data
    while(len--)
    {
        // Execution cycle I
        ST7920_CTRL_OUT |= ST7920_E_PIN;
        HAL_Delay_Micros(1);

        // Move higher nibble first
        *pBufer |= (ST7920_DB_IN & 0x0F) << 4;

        // End of execution cycle I
        HAL_Delay_Micros(1);
        ST7920_CTRL_OUT &= ~ST7920_E_PIN;

        // Execution cycle II
        ST7920_CTRL_OUT |= ST7920_E_PIN;
        HAL_Delay_Micros(1);

        // Move lower nibble
        *pBufer |= (ST7920_DB_IN & 0x0F);

        // End of execution cycle II
        HAL_Delay_Micros(1);
        ST7920_CTRL_OUT &= ~ST7920_E_PIN;
        pBufer++;
    }
}
#elif st7920INTERFACE_MODE == ST7920_INTERFACE_SERIAL
#error "#Driver/ST7920: Not implemented yet"
#else
#error "#Driver/ST7920: Unknown interface mode"
#endif




// ============================================================
// TEXT MODE
// ============================================================
void ST7920_Write_String(const char *str)
{
    if(xMode != ST7920_MODE_TEXT) return;
    while(*str)
    {
        ST7920_Write_Data(*str++);
    }
}

void ST7920_Set_Cursor(unsigned char ucRow, unsigned char ucCol)
{
    if(xMode != ST7920_MODE_TEXT) return;

    uint8_t ucAddr;
    uint8_t ucIsOddXPos = ucCol & 0x01;

    // Position clipping
    if(ucRow >= ST7920_MAX_ROWS) return;
    if(ucCol >= ST7920_MAX_COLS) return;

    ucAddr = ucGlcdRowOffset[ucRow];
    ucAddr |= ucCol >> 1;
    ST7920_Write_Command(ST7920_FUNCSET_CMD | ST7920_8BIT_MODE | ST7920_BASIC_MODE);
    ST7920_Write_Command(ST7920_SET_ADD_CMD | ucAddr);

    // If column position is odd, read a dummy byte to increase internal DDRAM address pointer
    if(ucIsOddXPos)
    {
        ST7920_Read();
    }

}

void ST7920_Print(unsigned char ucRow, unsigned char ucCol, const char *str)
{
    if(xMode != ST7920_MODE_TEXT) return;

    // Position clipping
    if(ucRow >= ST7920_MAX_ROWS) return;
    if(ucCol >= ST7920_MAX_COLS) return;
    ST7920_Set_Cursor(ucRow, ucCol);

    // Print
    while(*str)
    {
        char cChar = *str++;
        switch(cChar)
        {
            case '\n':
                ucRow++;
                if (ucRow == ST7920_MAX_ROWS )
                {
                    ucRow = 0;
                }
                ST7920_Set_Cursor(ucRow, ucCol);
            break;

            case '\r':
                ucCol=0;
                ST7920_Set_Cursor(ucRow, ucCol);
            break;

            case '\t':
                ucCol+=4;
                if (ucCol>=ST7920_MAX_COLS)
                {
                    ucCol &= ST7920_MAX_COLS-1;
                    ucRow++;
                    if (ucRow == ST7920_MAX_ROWS )
                    {
                        ucRow = 0;
                    }
                }
                ST7920_Set_Cursor(ucRow, ucCol);
            break;

            default:
                ST7920_Write_Data(cChar);
                ucCol++;
                // Check for overflow along the column-axis
                if ( !( ucCol & 0x0F ))
                {
                    ucCol &= ST7920_MAX_COLS-1;
                    ucRow++;
                    if (ucRow == ST7920_MAX_ROWS )
                    {
                        ucRow = 0;
                    }
                    ST7920_Set_Cursor(ucRow, ucCol);
                }
            break;
        }
    }
}

// ============================================================
// GRAPHICS MODE
// ============================================================
void ST7920_Set_Mode(st7920_mode_t mode)
{
    if(mode==ST7920_MODE_GRAPHICS)
    {
        ST7920_Write_Command(ST7920_FUNCSET_CMD | st7920INTERFACE_MODE_BIT | ST7920_EXTENDED_MODE);
        ST7920_Write_Command(ST7920_FUNCSET_CMD | st7920INTERFACE_MODE_BIT | ST7920_EXTENDED_MODE | ST7920_GRAPHICS_ON);
    }
    else
    {
        ST7920_Write_Command(ST7920_FUNCSET_CMD | st7920INTERFACE_MODE_BIT | ST7920_BASIC_MODE);
    }

    xMode = mode;
}

void ST7920_ClearGDRAM(void)
{
    if(xMode != ST7920_MODE_GRAPHICS) return;

    uint8_t x,y;
    for (x = 0; x < ST7920_MAX_GCOLS; x++)
    {
        for (y = 0; y < ST7920_MAX_GROWS/2; y++) {
            ST7920_Write_Command(ST7920_SET_AGD_CMD | y);
            ST7920_Write_Command(ST7920_SET_AGD_CMD | x);
            // 16-bits
            ST7920_Write_Data(0x00);
            ST7920_Write_Data(0x00);
        }
    }
}

void ST7920_SetPos(unsigned char ucRow, unsigned char ucCol)
{
    // Assert
    if(xMode != ST7920_MODE_GRAPHICS) return;
    if(ucRow >= ST7920_MAX_GROWS) return;
    if(ucCol >= ST7920_MAX_GCOLS) return;

    // Check flags
    uint8_t ucScreenPosFlags = 0x00;
    if(ucRow >= ST7920_MAX_GROWS/2)
    {
        ucScreenPosFlags |= BIT0;
        ucRow &= ST7920_MAX_GROWS/2-1; // ROW/2
    }
    if(ucCol & 0x01)  ucScreenPosFlags |= BIT1;

    // Saturate
    ucCol >>= 1; // COL/2

    // Set row & column
    ST7920_Write_Command(ST7920_SET_AGD_CMD | ucRow);
    if(ucScreenPosFlags & BIT0) ST7920_Write_Command(ST7920_SET_AGD_CMD | 0x08 | ucCol);
    else ST7920_Write_Command(ST7920_SET_AGD_CMD | ucCol);

    if(ucScreenPosFlags & BIT1)
    {
        ST7920_Read(); // Dummy read
    }
}

void ST7920_Set_Pixel(unsigned char x, unsigned char y, st7920_pixel_t val)
{
    // Assert
    if(xMode != ST7920_MODE_GRAPHICS) return;
    if(x >= ST7920_WIDTH) return;
    if(y >= ST7920_HEIGHT) return;

    // Set position in rows and columns
    uint8_t ucCol = x>>3;
    ST7920_SetPos(y, ucCol);

    // Get current column data
    ST7920_Read();
    uint8_t ucByte = ST7920_Read();

    // Set again position
    ST7920_SetPos((y), ucCol);

    // Write new byte value
    uint8_t mbit = 7 - (x & 0x07);
    if(val) ucByte |= 1 << mbit;
    else ucByte &= ~(1 << mbit);
    ST7920_Write_Data(ucByte);
}

void ST7920_Invert(unsigned char mode)
{
    if(xMode != ST7920_MODE_GRAPHICS) return;

    if(mode <= ST7920_INVERT_ROW_3)
    {
        ST7920_Write_Command(0x40 | mode);
    }
    else
    {
        ST7920_Write_Command(0x04 | ST7920_INVERT_ROW_0);
        ST7920_Write_Command(0x04 | ST7920_INVERT_ROW_1);
        ST7920_Write_Command(0x04 | ST7920_INVERT_ROW_2);
        ST7920_Write_Command(0x04 | ST7920_INVERT_ROW_3);
    }
}

/* Private Reference Function --------------------------------------*/
#if st7920ST7920_WAIT_BUSY==(1)
static void pvrST7920_Wait_Busy(void)
{
    // DB is input
    ST7920_DB_DIR = 0x00;

    // Data write
    ST7920_CTRL_OUT |= ST7920_RW_PIN;             // R/W = 0
    ST7920_CTRL_OUT &= ~ST7920_RS_PIN;

    // Execution cycle begin
    ST7920_CTRL_OUT |= ST7920_E_PIN;
    delay_us(1); // enable pulse must be > 450 ns

    // Read data from DB port
    while(ST7920_DB_IN & ST7920_BUSY);

    // Execution cycle end
    ST7920_CTRL_OUT &= ~ST7920_E_PIN;
}
#endif

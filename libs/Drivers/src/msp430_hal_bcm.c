/*
 * msp430_hal_bcm.c
 *
 *  Created on: 20 mar 2024
 *      Author: User123
 */

#include <Drivers/msp430_hal_bcm.h>

#define BCCFGR_DCOFR_M      (0x0007)
#define BCCFGR_DCOFR_S      (0)
#define BCCFGR_DCOFR_DEF    (0x0004)
#define BCCFGR_DCOFR_USER   BCCFGR_DCOFR_M

#define BCCFGR_MCLKSRC_M    (BIT3)
#define BCCFGR_MCLKSRC_S    (3)

#define BCCFGR_SMCLKSRC_M   (BIT4)
#define BCCFGR_SMCLKSRC_S   (4)

#define BCCFGR_ACLKSRC_M    (BIT5)
#define BCCFGR_ACLKSRC_S    (5)

#define BCCFGR_MCLKDIV_M   (BIT7 | BIT6)
#define BCCFGR_MCLKDIV_S   (6)

#define BCCFGR_SMCLKDIV_M  (BIT9 | BIT8)
#define BCCFGR_SMCLKDIV_S  (8)

#define BCCFGR_ACLKDIV_M   (BITB | BITA)
#define BCCFGR_ACLKDIV_S   (10)

/* BCCFGR: BCM Configuration Register
 * Bitfiels:
 *      - DCOFR @BCCFGR<2:0>
 *           000 - 1MHz
 *           001 - 8MHz
 *           010 - 12MHz
 *           011 - 16MHz
 *           100 - 1.1MHz
 *           111 - Other
 *
 *      - MCLKSRC @BCCFGR<3>
 *          0 - DCO
 *          1 - ACLK
 *      - SMCLKSCR @BCCFGR<4>
 *          0 - DCO
 *          1 - ACLK
 *      - ACLKSRC @BCCFGR<5>
 *          0 - LFXT1CLK
 *          1 - VLOCLK
 *
 *      - MCLKDIV @BCCFGR<7:6>
 *          00 - DIV_1
 *          01 - DIV_2
 *          10 - DIV_4
 *          11 - DIV_8
 *      - SMCLKDIV @BCCFGR<9:8>
 *          00 - DIV_1
 *          01 - DIV_2
 *          10 - DIV_4
 *          11 - DIV_8
 *      - ACLKDIV @BCCFGR<11:10>
 *          00 - DIV_1
 *          01 - DIV_2
 *          10 - DIV_4
 *          11 - DIV_8
 */
static uint16_t BCCFGR = BCCFGR_DCOFR_DEF;

void HAL_BCM_Set_DCO_Freq(uint8_t dcocfg, uint16_t regconfig)
{
    BCCFGR &= ~BCCFGR_DCOFR_M;
    if(dcocfg < BCM_DCOCLK_MAX)
    {
        switch (dcocfg) {
            case BCM_DCOCLK_1MHZ:
                BCSCTL1 = CALBC1_1MHZ;
                DCOCTL = CALDCO_1MHZ;
            break;

            case BCM_DCOCLK_8MHZ:
                BCSCTL1 = CALBC1_8MHZ;
                DCOCTL = CALDCO_8MHZ;
            break;

            case BCM_DCOCLK_12MHZ:
                BCSCTL1 = CALBC1_12MHZ;
                DCOCTL = CALDCO_12MHZ;
            break;

            default:
                BCSCTL1 = CALBC1_16MHZ;
                DCOCTL = CALDCO_16MHZ;
            break;
        }

        BCCFGR |= (uint16_t)dcocfg << BCCFGR_DCOFR_S;
    }
    else
    {
        //
        BCSCTL1 = (uint8_t)((regconfig & 0xFF00) >> 8);
        DCOCTL = (uint8_t)((regconfig & 0x00FF));
        BCCFGR |= BCCFGR_DCOFR_USER;
    }
}

void HAL_BCM_Setup(uint8_t clk, uint8_t clksrc, uint8_t clkdiv)
{
    assert_param(clk < BCM_CLK_MAX);
    assert_param(clksrc < BCM_CLKSRC_MAX);
    assert_param(clkdiv < BCM_CLKDIV_MAX);

    // If CLKSRC
    switch(clk)
    {
        case BCM_MCLK:
            BCSCTL2 &= ~(0xF0);
            BCCFGR &= ~(BCCFGR_MCLKSRC_M | BCCFGR_MCLKDIV_M);
            if(clksrc != BCM_CLKSRC_DCO)
            {
                BCSCTL2 |= BIT7 | BIT6;
                BCCFGR |= BCCFGR_MCLKSRC_M;
            }
            BCSCTL2 |= (uint8_t)clkdiv << 4;
            BCCFGR |= clkdiv << BCCFGR_MCLKDIV_S;
        break;

        case BCM_SMCLK:
            BCSCTL2 &= ~(0x0E);
            BCCFGR &= ~(BCCFGR_SMCLKSRC_M | BCCFGR_SMCLKDIV_M);
            if(clksrc != BCM_CLKSRC_DCO)
            {
                BCSCTL2 |= BIT3;
                BCCFGR |= BCCFGR_SMCLKSRC_M;
            }
            BCSCTL2 |= (uint8_t)clkdiv << 1;
            BCCFGR |= clkdiv << BCCFGR_SMCLKDIV_S;
        break;

        default:
            if(clksrc != BCM_CLKSRC_DCO)
            {
                BCSCTL1 &= ~(0x30);
                BCSCTL3 &= ~(0x30);
                BCCFGR &= ~(BCCFGR_ACLKSRC_M | BCCFGR_ACLKDIV_M);
                if(clksrc == BCM_CLKSRC_VLO)
                {
                    BCSCTL3 |= BIT5;
                    BCCFGR |= BCCFGR_ACLKSRC_M;
                }
                BCSCTL1 |= (uint8_t)clkdiv << 4;
                BCCFGR |= clkdiv << BCCFGR_ACLKDIV_S;
            }
        break;
    }
}

uint32_t HAL_BCM_GetCLKFreq(uint8_t clk)
{
    // Assert parameter
    assert_param_ret(clk < BCM_CLK_MAX, BCM_CLK_HZ_ERR);

    uint8_t bcmConfig = 0x80; // DCO
    uint32_t XCLK_clock_hz = 0;
    switch(clk)
    {
        case BCM_MCLK:
            // DIV
            bcmConfig |= (uint8_t)((BCCFGR&BCCFGR_MCLKDIV_M) >> BCCFGR_MCLKDIV_S);
            // ACLK
            if(BCCFGR & BCCFGR_MCLKSRC_M) bcmConfig &= ~0x80;
        break;

        case BCM_SMCLK:
            // DIV
            bcmConfig |= (uint8_t)((BCCFGR&BCCFGR_SMCLKDIV_M) >> BCCFGR_SMCLKDIV_S);
            // ACLK
            if(BCCFGR & BCCFGR_SMCLKSRC_M) bcmConfig &= ~0x80;
        break;

        case BCM_ACLK:
            // DIV
            bcmConfig |= (uint8_t)((BCCFGR&BCCFGR_ACLKDIV_M) >> BCCFGR_ACLKDIV_S);
            bcmConfig &= ~0x80; // No DCO
            // Clock
            if(BCCFGR & BCCFGR_ACLKSRC_M) XCLK_clock_hz = BCM_VLOCLK_HZ;
            else XCLK_clock_hz = BCM_ACLK_HZ_DEFAULT;
        break;

        default:
            return BCM_CLK_HZ_ERR;
    }

    // Check for DCO flag
    if(bcmConfig & 0x80)
    {
        switch(BCCFGR & BCCFGR_DCOFR_M)
        {
            case BCM_DCOCLK_1MHZ:
                XCLK_clock_hz = 1000000UL;
            break;

            case BCM_DCOCLK_8MHZ:
                XCLK_clock_hz = 8000000UL;
            break;

            case BCM_DCOCLK_12MHZ:
                XCLK_clock_hz = 12000000UL;
            break;

            case BCM_DCOCLK_16MHZ:
                XCLK_clock_hz = 16000000UL;
            break;

            case BCCFGR_DCOFR_DEF:
                XCLK_clock_hz = BCM_DCOCLK_HZ_DEFAULT;
            break;

            case BCCFGR_DCOFR_USER:
                XCLK_clock_hz = BCM_CLK_HZ_USER;
            break;

            default:
                XCLK_clock_hz = BCM_CLK_HZ_ERR;
            break;
        }
    }

    // Clkdiv
    XCLK_clock_hz >>= (bcmConfig & 0x03);

    // Return
    return XCLK_clock_hz;
}

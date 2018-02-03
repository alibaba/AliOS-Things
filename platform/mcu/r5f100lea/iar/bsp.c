#include <k_api.h>
#include "bsp.h"

uint32_t sys_freq(void);

void int_dummy(void)
{
   while (1);
}

void tick_init(uint32_t ticks_per_sec)
{
    REG_PER0  |=  PER0_ENABLE_CLOCK;
    REG_TPS0   =  TPS0_SET_CLOCK_FRQ;
    REG_TMR00  =  TMR00_SET_CK00_SRC;

    REG_TDR00  = (sys_freq() / ticks_per_sec) - 1;

    REG_TOE0  &= 0x00FE;
    REG_TO0   &= 0x00FE;
    REG_TOM0  &= 0x00FE;
    REG_TOL0  &= 0x00FE;

    REG_PR01L  |= 0x10;
    REG_PR11L  |= 0x10;

    REG_IF1L  &= ~BIT_04;
    REG_MK1L  &= ~BIT_04;
    REG_TS0   |=  TS0_EN_TMR_OUTPUT;
}

uint32_t sys_freq(void)
{
    uint8_t  freq_sel;
    uint8_t  clk_src;
    uint32_t cpu_clk;

    freq_sel = OPTBYTE2_VAL & 0x0F;
    clk_src = REG_CLK_GEN_CKC;
    if (BIT_IS_SET(clk_src, BIT_CKC_CSS)) {
        cpu_clk = FSUB_CLKRATE;
        if (BIT_IS_SET(clk_src, BIT_CKC_MCM0)) {
            clk_src = REG_CLK_GEN_CMC;
            cpu_clk = FMX_CLKRATE;
        } else {
            switch(freq_sel) {
                case FRQSEL_32MHz:
                     cpu_clk = CLKRATE32;
                     break;

                case FRQSEL_24MHz:
                     cpu_clk = CLKRATE24;
                     break;

                case FRQSEL_16MHz:
                     cpu_clk = CLKRATE16;
                     break;

                case FRQSEL_12MHz:
                     cpu_clk = CLKRATE12;
                     break;

                case FRQSEL_8MHz:
                     cpu_clk = CLKRATE08;
                     break;

                case FRQSEL_4MHz:
                     cpu_clk = CLKRATE04;
                     break;

                case FRQSEL_1MHz:
                     cpu_clk = CLKRATE01;
                     break;

                default:
                     break;
            }
        }
    }

    return (cpu_clk);
}


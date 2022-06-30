/*************************************************************************
 *
 * 时钟相关函数
 *
 *************************************************************************/


#include <stdio.h>
#include "ls1c_regs.h"
#include "ls1c_public.h"
#include "start.h"
#include "ls1c_delay.h"


/*
 * 获取PLL频率
 * @ret PLL频率
 */
unsigned long clk_get_pll_rate(void)
{
    unsigned int ctrl;
    unsigned long pll_rate = 0;

    ctrl = reg_read_32((volatile unsigned int *)LS1C_START_FREQ);
    pll_rate = (((ctrl & M_PLL) >> M_PLL_SHIFT) + ((ctrl & FRAC_N) >> FRAC_N_SHIFT)) * APB_CLK / 4;
    
    return pll_rate;
}


/*
 * 获取CPU频率
 * @ret CPU频率
 */
unsigned long clk_get_cpu_rate(void)
{
    unsigned long pll_rate, cpu_rate;
    unsigned int ctrl;

    pll_rate = clk_get_pll_rate();
    ctrl = reg_read_32((volatile unsigned int *)LS1C_CLK_DIV_PARAM);

    // 选择时钟来源
    if (DIV_CPU_SEL & ctrl)     // pll分频作为时钟信号
    {
        if (DIV_CPU_EN & ctrl)
        {
            cpu_rate = pll_rate / ((ctrl & DIV_CPU) >> DIV_CPU_SHIFT);
        }
        else
        {
            cpu_rate = pll_rate / 2;
        }
    }
    else                        // bypass模式，晶振作为时钟输入
    {
        cpu_rate = APB_CLK;
    }

    return cpu_rate;
}


/*
 * 获取DDR频率
 * @ret DDR频率
 */
unsigned long clk_get_ddr_rate(void)
{
    unsigned long cpu_rate = 0;
    unsigned long ddr_rate = 0;
    unsigned int ctrl;

    cpu_rate = clk_get_cpu_rate();
    ctrl = reg_read_32((volatile unsigned int *)LS1C_START_FREQ);
    ctrl = (ctrl & DIV_SDRAM) >> DIV_SDRAM_SHIFT;

    switch (ctrl)
    {
        case 0:
            ddr_rate = cpu_rate / 2;
            break;

        case 1:
            ddr_rate = cpu_rate / 4;
            break;

        case 2:
        case 3:
            ddr_rate = cpu_rate / 3;
            break;
    }

    return ddr_rate;
}


/*
 * 获取APB频率
 * @ret APB频率
 */
unsigned long clk_get_apb_rate(void)
{
    return clk_get_ddr_rate();
}


/*
 * 获取DC频率
 * @ret DC频率
 */
unsigned long clk_get_dc_rate(void)
{
    unsigned long pll_rate, dc_rate;
    unsigned int ctrl;

    pll_rate = clk_get_pll_rate();
    ctrl = reg_read_32((volatile unsigned int *)LS1C_CLK_DIV_PARAM);

    dc_rate = pll_rate / ((ctrl & DIV_PIX) >> DIV_PIX_SHIFT);

    return dc_rate;
}


// 打印所有时钟的信息
void clk_print_all(void)
{
    unsigned long pll_rate, cpu_rate, ddr_rate, apb_rate, dc_rate;

    printf("\r\n[%s] clk info\r\n", __FUNCTION__);
    
    pll_rate = clk_get_pll_rate();
    printf("pll_rate = %luHz\r\n", pll_rate);
    
    cpu_rate = clk_get_cpu_rate();
    printf("cpu_rate = %luHz\r\n", cpu_rate);
    
    ddr_rate = clk_get_ddr_rate();
    printf("ddr_rate = %luHz\r\n", ddr_rate);
    
    apb_rate = clk_get_apb_rate();
    printf("apb_rate = %luHz\r\n", apb_rate);
    
//    dc_rate  = clk_get_dc_rate();
//    printf("dc_rate  = %luHz\r\n", dc_rate);

    printf("\r\n");

    return ;
}



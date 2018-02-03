/*
*********************************************************************************************************
*                                     MICRIUM BOARD SUPPORT SUPPORT
*
*                          (c) Copyright 2003-2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      BOARD SUPPORT PACKAGE (BSP)
*
*                                            RENESAS RX64M
*                                               on the
*                                     RSKRX64M DEVELOPMENT BOARD
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/*#include <lib_def.h>*/
#include <bsp_cfg.h>

#include <iodefine.h>

#if BSP_CFG_LED_EN         > 0u
#include  <bsp_led.h>
#endif


/*
*********************************************************************************************************
*                                          LOCAL VARIABLES
*********************************************************************************************************
*/

#define  BSP_SYS_MOSCWTCR_WAIT_TIME         0x52u               /* Main Clock Oscillator Wait Time                      */

#define  BSP_SYS_TIMEOUT_VAL                0xFFFFu


/*
*********************************************************************************************************
*                                          REGISTER DEFINES
*********************************************************************************************************
*/

                                                                /* --------------- PLL CONTROL REGISTER --------------- */
#define  BSP_SYS_PLLCR_PLLDIV_1             0x00u
#define  BSP_SYS_PLLCR_PLLDIV_1_2           0x01u
#define  BSP_SYS_PLLCR_PLLDIV_1_3           0x10u

#define  BSP_SYS_PLLCR_STC_x10              0x13u
#define  BSP_SYS_PLLCR_STC_x10_5            0x14u
#define  BSP_SYS_PLLCR_STC_x11              0x15u
#define  BSP_SYS_PLLCR_STC_x11_5            0x16u
#define  BSP_SYS_PLLCR_STC_x12              0x17u
#define  BSP_SYS_PLLCR_STC_x12_5            0x18u
#define  BSP_SYS_PLLCR_STC_x13              0x19u
#define  BSP_SYS_PLLCR_STC_x13_5            0x1Au
#define  BSP_SYS_PLLCR_STC_x14              0x1Bu
#define  BSP_SYS_PLLCR_STC_x14_5            0x1Cu
#define  BSP_SYS_PLLCR_STC_x15              0x1Du
#define  BSP_SYS_PLLCR_STC_x15_5            0x1Eu
#define  BSP_SYS_PLLCR_STC_x16              0x1Fu
#define  BSP_SYS_PLLCR_STC_x16_5            0x20u
#define  BSP_SYS_PLLCR_STC_x17              0x21u
#define  BSP_SYS_PLLCR_STC_x17_5            0x22u
#define  BSP_SYS_PLLCR_STC_x18              0x23u
#define  BSP_SYS_PLLCR_STC_x18_5            0x24u
#define  BSP_SYS_PLLCR_STC_x19              0x25u
#define  BSP_SYS_PLLCR_STC_x19_5            0x26u
#define  BSP_SYS_PLLCR_STC_x20              0x27u
#define  BSP_SYS_PLLCR_STC_x20_5            0x28u
#define  BSP_SYS_PLLCR_STC_x21              0x29u
#define  BSP_SYS_PLLCR_STC_x21_5            0x2Au
#define  BSP_SYS_PLLCR_STC_x22              0x2Bu
#define  BSP_SYS_PLLCR_STC_x22_5            0x2Cu
#define  BSP_SYS_PLLCR_STC_x23              0x2Du
#define  BSP_SYS_PLLCR_STC_x23_5            0x2Eu
#define  BSP_SYS_PLLCR_STC_x24              0x2Fu
#define  BSP_SYS_PLLCR_STC_x24_5            0x30u
#define  BSP_SYS_PLLCR_STC_x25              0x31u
#define  BSP_SYS_PLLCR_STC_x25_5            0x32u
#define  BSP_SYS_PLLCR_STC_x26              0x33u
#define  BSP_SYS_PLLCR_STC_x26_5            0x34u
#define  BSP_SYS_PLLCR_STC_x27              0x35u
#define  BSP_SYS_PLLCR_STC_x27_5            0x36u
#define  BSP_SYS_PLLCR_STC_x28              0x37u
#define  BSP_SYS_PLLCR_STC_x28_5            0x38u
#define  BSP_SYS_PLLCR_STC_x29              0x39u
#define  BSP_SYS_PLLCR_STC_x29_5            0x3Au
#define  BSP_SYS_PLLCR_STC_x30              0x3Bu

                                                                /* -------------- SYS CLK CTRL REGISTER 2 ------------- */
#define  BSP_SYS_SCKCR2_UCK_1_2             0x01u
#define  BSP_SYS_SCKCR2_UCK_1_3             0x02u
#define  BSP_SYS_SCKCR2_UCK_1_4             0x03u
#define  BSP_SYS_SCKCR2_UCK_1_5             0x04u

                                                                /* -------------- SYS CLK CTRL REGISTER 3 ------------- */
#define  BSP_SYS_SCKCR3_CKSEL_LOCO          0x00u
#define  BSP_SYS_SCKCR3_CKSEL_HOCO          0x01u
#define  BSP_SYS_SCKCR3_CKSEL_MAIN          0x02u
#define  BSP_SYS_SCKCR3_CKSEL_SUB_CLK       0x03u
#define  BSP_SYS_SCKCR3_CKSEL_PLL           0x04u


/*
*********************************************************************************************************
*                                           BSP_SysInit()
*
* Description : Initializes & Configures system clock and peripheral clocks.
*
* Argument(s) : none.
*
* Caller(s)   : none.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_SysInit (void)
{
    uint16_t  timeout;
    
    
    timeout                 = BSP_SYS_TIMEOUT_VAL;              /* Init Var(s).                                         */

    SYSTEM.PRCR.WORD        = 0xA50Bu;                          /* Remove Protection from Registers.                    */    

    MPC.PWPR.BIT.B0WI       = 0u;                               /* Enable Writing to MPC Pin Function Control reg.      */
    MPC.PWPR.BIT.PFSWE      = 1u;
    
    SYSTEM.MOFCR.BIT.MOSEL  = 0u;                               /* Set Clk Osc Switch to Resonator.                     */
    SYSTEM.MOFCR.BIT.MODRV2 = 0u;                               /* Set Clk Osc Drive to  20-24 MHz.                     */
    
    SYSTEM.MOSCWTCR.BYTE    = BSP_SYS_MOSCWTCR_WAIT_TIME;       /* Main Clk Osc Wait Time.                              */
    SYSTEM.MOSCCR.BIT.MOSTP = 0u;                               /* Set Main Clk Osc to Operation Mode.                  */

                                                                /* Wait for Main Clk Osc to be stable for Sys Clk use.  */
    while ((SYSTEM.OSCOVFSR.BIT.MOOVF == 0u) && (timeout > 0u)) {
        timeout--;
    }
                                                                /* ---------------- PLL CONFIGURATION ----------------- */
    SYSTEM.PLLCR.BIT.PLIDIV = BSP_SYS_PLLCR_PLLDIV_1;           /*      - PLIDIV = 1                                    */
    SYSTEM.PLLCR.BIT.STC    = BSP_SYS_PLLCR_STC_x10;            /*      - STC    = 10                                   */
                                                                /*      - PLL    = (XTAL_FREQ * PLIDIV) * STC           */
                                                                /*               = (12000000 * 1) * 10                  */
                                                                /*               = 120 MHz                              */
    SYSTEM.PLLCR.BIT.PLLSRCSEL = 0u;                            /* Set main Clk Osc as PPL Clk Source.                  */
    SYSTEM.PLLCR2.BIT.PLLEN    = 0u;                            /* Start PLL Operation.                                 */

    timeout = BSP_SYS_TIMEOUT_VAL;                              /* Reset Timeout Val & Check PLL Clk Stability.         */
    while ((SYSTEM.OSCOVFSR.BIT.PLOVF == 0u) && (timeout > 0u)) {
        timeout--;
    }
    
                                                                /* --------------- SYSTEM CONFIGURATION --------------- */
    SYSTEM.SCKCR.LONG = 0x21021222u;                            /*      - PCKD = PLL * 1/4 = 30 MHz.                    */
                                                                /*      - PCKC = PLL * 1/4 = 30 MHz.                    */
                                                                /*      - PCKB = PLL * 1/4 = 30 MHz.                    */
                                                                /*      - PCKA = PLL * 1/2 = 60 MHz.                    */
                                                                /*      - BCK  = PLL * 1/4 = 30 MHz.                    */
                                                                /*      - ICK  = PLL * 1/2 = 60 MHz.                    */
                                                                /*      - FCK  = PLL * 1/4 = 30 MHz.                    */
    
    SYSTEM.SOSCCR.BIT.SOSTP = 1u;                               /* Disable Sub-Clock Osc.                               */

    timeout = BSP_SYS_TIMEOUT_VAL;                              /* Reset Timeout Val & Check Sub-Clock is Disabled.     */
    while ((SYSTEM.SOSCCR.BIT.SOSTP == 0u) && (timeout > 0u)) {
        timeout--;
    }
    
    SYSTEM.LOCOCR.BIT.LCSTP = 0u;                               /* Set Low Speed On-Chip Osc.                           */
    SYSTEM.SCKCR2.BIT.UCK   = BSP_SYS_SCKCR2_UCK_1_5;           /* Set USB Clk Select = PLL * 1/5 = 24 MHz.             */
    SYSTEM.SCKCR.BIT.PSTOP0 = 1u;                               /* SDCLK Pin Output Control is Disabled.                */
    SYSTEM.SCKCR3.BIT.CKSEL = BSP_SYS_SCKCR3_CKSEL_PLL;         /* Set Clock Source as PLL Circuit.                     */
}


/*
*********************************************************************************************************
*                                       BSP_SysPerClkFreqGet()
*
* Description : This function is used to retrieve peripheral clock frequency.
*
* Argument(s) : none.
*
* Caller(s)   : none.
*
* Return(s)   : Peripheral clock frequency in cycles.
*
* Note(s)     : none.
*********************************************************************************************************
*/

uint32_t  BSP_SysPerClkFreqGet (void)
{
	uint32_t  pll_stc;
	uint32_t  pll_div;
	uint32_t  pll_freq;
	uint32_t  per_div;
	uint32_t  per_clk;


    pll_stc = SYSTEM.PLLCR.BIT.STC;
    pll_stc++;

    pll_div = SYSTEM.PLLCR.BIT.PLIDIV;
    
    switch (pll_div) {
        case 0u:
             pll_div = 1u;
             break;

        case 1u:
             pll_div = 2u;
             break;

        case 2u:
             pll_div = 4u;
             break;

        case 3u:
        default:
             return (0u);

    }

    pll_freq = BSP_CFG_SYS_EXT_CLK_FREQ * pll_stc / pll_div;
    
    per_div  = SYSTEM.SCKCR.BIT.PCKB;
    
    switch (per_div) {
        case 0u:
            per_div = 1u;
            break;

        case 1u:
            per_div = 2u;
            break;

        case 2u:
            per_div = 4u;
            break;

        case 3u:
            per_div = 8u;
            break;

        case 4u:
            per_div = 16u;
            break;

        case 5u:
            per_div = 32u;
            break;

        case 6u:
            per_div = 64u;
            break;

        default:
            return (0u);
    }

    per_clk = (pll_freq / per_div);

    return (per_clk);
}

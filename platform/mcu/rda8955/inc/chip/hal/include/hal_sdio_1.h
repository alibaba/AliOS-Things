/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#if (CHIP_HAS_SDIO == 1)

#include "cs_types.h"
#include "chip_id.h"
#include "global_macros.h"
#include "sdmmc.h"
#include "sdmmc2.h"
#include "sys_ctrl.h"
#include "cfg_regs.h"

#include "hal_sdio.h"
#include "hal_clk.h"
#include "halp_sys_ifc.h"
#include "halp_sys.h"
#include "hal_host.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "cfg_regs.h"
#include "gpio.h"
#include "hal_gpio.h"
#include "pmd_m.h"
#include "rfd_xcv.h"
#include "halp_debug.h"
#include "pmd_m.h"
#include "iomux.h"

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
// Resolve SDMMC2 compatible issue on 8808 and 8808S
extern HWP_SDMMC2_T *g_halSdmmc2Hwp;
#undef hwp_sdmmc2
#define hwp_sdmmc2 g_halSdmmc2Hwp
#endif

#define NEW_WIFI_BOARD 1

// =============================================================================
//  Global variables
// =============================================================================
#if NEW_WIFI_BOARD
HAL_GPIO_GPIO_ID_T wifi_intr_gpio = HAL_GPIO_26;
HAL_GPIO_GPIO_ID_T wifi_pwn_gpio = HAL_GPIO_30;
#else
HAL_GPIO_GPIO_ID_T wifi_intr_gpio = HAL_GPIO_3;
HAL_GPIO_GPIO_ID_T wifi_pwn_gpio = HAL_GPIO_2;
#endif

PRIVATE HAL_GPIO_CFG_T g_gpioCfg;

UINT8 g_halSdioWriteCh = HAL_UNKNOWN_CHANNEL;
UINT8 g_halSdioReadCh = HAL_UNKNOWN_CHANNEL;

/// SDMMC2 clock frequency.
PRIVATE UINT32 g_halSdioFreq = 200000;
static HAL_CLK_T *gSdioClk = NULL;

PRIVATE HAL_SDIO_IRQ_HANDLER_T g_halSdioRegistry = NULL;


//=============================================================================
// hal_SdioUpdateDivider
//-----------------------------------------------------------------------------
/// Update the SDMMC2 module divider to keep the desired frequency, whatever
/// the system frequency is.
/// @param sysFreq System Frequency currently applied or that will be applied
/// if faster.
//=============================================================================
UINT32 hal_SdioUpdateDivider(HAL_SYS_FREQ_T sysFreq);


//=============================================================================
// hal_SdioOpen
//-----------------------------------------------------------------------------
/// Open the SDMMC2 module. Take a resource.
//=============================================================================
PUBLIC VOID hal_SdioGpioInit(VOID)
{
#ifdef CHIP_DIE_8955
#else
    hwp_gpio->gpio_oen_set_out |=0x1D000;
    hwp_gpio->gpio_set |=0x1D000;
    hwp_gpio->gpo_set |=0x300;
    hwp_configRegs->Alt_mux_select |=0x600000;
#endif
}
PUBLIC VOID hal_SdioGpioSet(VOID)
{
#ifdef CHIP_DIE_8955
#else
    hwp_configRegs->GPIO_Mode |=  0x15000;;//modify 2012-02-24 ,wuys
    hwp_configRegs->Alt_mux_select &=  ~0x800;//modify 2012-02-24 ,wuys
#endif

}
PUBLIC VOID hal_SdioGpioClr(VOID)
{
#ifdef CHIP_DIE_8955
#else
    hwp_configRegs->GPIO_Mode &=  ~0x15000;;//modify 2012-02-24 ,wuys
    hwp_configRegs->Alt_mux_select |=  0x800;//modify 2012-02-24 ,wuys
#endif

}
VOID hal_sdioDataInIrqMaskClear(VOID);

PUBLIC VOID hal_SdioPwnReset( )
{
    HAL_GPIO_CFG_T cfg  =
    {
        .direction      = HAL_GPIO_DIRECTION_OUTPUT,
        .value          = 0,
    };
    hal_GpioOpen(wifi_pwn_gpio, &cfg);
    sxr_Sleep(1638);
    cfg.value = 1;
    hal_GpioOpen(wifi_pwn_gpio, &cfg);
}

PUBLIC VOID hal_SdioOpenGpio( HAL_SDIO_IRQ_HANDLER_T handler)
{

    HAL_GPIO_CFG_T cfg  =
    {
        .direction      = HAL_GPIO_DIRECTION_INPUT,
        .irqMask        =
        {
            .rising     = FALSE,
            .falling    = FALSE,
            .debounce   = FALSE,
            .level      = FALSE
        },
        .irqHandler     = handler
    };

    hal_GpioOpen(wifi_intr_gpio, &cfg);
}

PUBLIC VOID hal_SdioOpen(UINT8 clk_adj, HAL_SDIO_IRQ_HANDLER_T handler)
{
    pmd_EnablePower(PMD_POWER_SDMMC, FALSE);
    //g_halSdioRegistry = handler;
    // hal_GpioSetIn(wifi_intr_gpio);
    //hal_GpioIrqSetHandler(wifi_intr_gpio,handler,0);
#if NEW_WIFI_BOARD
    hwp_iomux->pad_KEYIN_0_cfg = IOMUX_PAD_KEYIN_0_SEL(1);
    hwp_iomux->pad_KEYIN_1_cfg = IOMUX_PAD_KEYIN_1_SEL(1);
    hwp_iomux->pad_KEYOUT_0_cfg = IOMUX_PAD_KEYOUT_0_SEL(1);
#endif

    hal_SdioPwnReset();
    hal_SdioOpenGpio(handler);
    // Take the module out of reset
    hwp_sysCtrl->REG_DBG     = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_CLR_RST_SDMMC;
    hwp_sysCtrl->Clk_Per_Enable |= SYS_CTRL_ENABLE_PER_SDMMC;
    hwp_sysCtrl->Clk_Sys_Mode |= SYS_CTRL_MODE_SYS_PCLK_DATA_MANUAL;
    // hwp_sysCtrl->Sys_Rst_Clr = SYS_CTRL_CLR_RST_SDMMC2;

    hwp_sysCtrl->REG_DBG     = SYS_CTRL_PROTECT_LOCK;
    pmd_EnablePower(PMD_POWER_SDMMC, TRUE);

    // We don't use interrupts.
    // hwp_sdmmc2->SDMMC2_INT_MASK = 0x80;
    hwp_sdmmc->SDMMC_INT_MASK = 0x00;

    hwp_sdmmc->SDMMC_MCLK_ADJUST = clk_adj|(1<<4);// fprd electrical specification  inv clk to low(1<<4)
    hal_sdioDataInIrqMaskClear();
    // Take a resource (The idea is to be able to get a 25Mhz clock)
    if (!hal_ClkIsEnabled(gSdioClk))
        hal_ClkEnable(gSdioClk);
    hal_ClkSetRate(gSdioClk, HAL_CLK_RATE_250M);

    // pmd_EnablePower(PMD_POWER_SDMMC2, TRUE);
}

UINT32 set_irq_mask = 0;
VOID Mac_WifiDataInd(UINT8 intflag);
VOID hal_sdioDataInIrqMaskClear(VOID)
{
    HAL_GPIO_IRQ_MASK_T maskCfg;
    maskCfg.rising = FALSE;
    maskCfg.falling = FALSE;
    maskCfg.debounce = FALSE;
    maskCfg.level = FALSE;
    hal_GpioIrqSetMask(wifi_intr_gpio, &maskCfg);
    //  hal_GpioResetIrq(wifi_intr_gpio);
    //hal_GpioClose(wifi_intr_gpio);
    set_irq_mask = 0;
    //hwp_sdmmc->SDMMC_INT_MASK = 0x00;

}

VOID hal_sdioDataInIrqMaskSet(VOID)
{
    int ret=0;
    if(hal_GpioGet(wifi_intr_gpio)==0 || (ret = check_pending_int()) & 0x1)
    {
        Mac_WifiDataInd(0);
        //hal_HstSendEvent(0x20130412);
    }
    //hwp_sdmmc2->SDMMC2_INT_MASK = SDMMC2_DATA_IND_MK|SDMMC2_DAT_OVER_MK;
    //hwp_sdmmc->SDMMC2_INT_MASK = SDMMC2_DATA_IND_MK;
    HAL_GPIO_IRQ_MASK_T maskCfg;
    maskCfg.rising = FALSE;
    maskCfg.falling = TRUE;
    maskCfg.debounce = FALSE;
    maskCfg.level = FALSE;
    hal_GpioIrqSetMask(wifi_intr_gpio, &maskCfg);
    set_irq_mask = 1;
}
UINT32 hal_sdioDataInIrqMaskGet(VOID)
{
    //return hwp_sdmmc2->SDMMC2_INT_MASK ;
    return set_irq_mask;
}
VOID hal_sdioDataInIrqMaskSetEx(UINT32 mask_t)
{
    if(set_irq_mask)
        hal_sdioDataInIrqMaskClear();
    else
        hal_sdioDataInIrqMaskSet();
    //hwp_sdmmc2->SDMMC2_INT_MASK = mask_t;

}

void setclockmode(void)
{
    hwp_sysCtrl->REG_DBG     = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Sys_Mode |= SYS_CTRL_MODE_SYS_PCLK_DATA_MANUAL;
    hwp_sysCtrl->REG_DBG     = SYS_CTRL_PROTECT_LOCK;
}

//=============================================================================
// hal_SdioClose
//-----------------------------------------------------------------------------
/// Close the SDMMC2 module. Take a resource.
//=============================================================================
PUBLIC VOID hal_SdioClose(VOID)
{
    g_halSdioRegistry = NULL;

    // pmd_EnablePower(PMD_POWER_SDMMC2, FALSE);
    // Free a resource
    hal_ClkDisable(gSdioClk);

    // Put the module in reset, as its clock is free running.
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Clk_Per_Disable = SYS_CTRL_DISABLE_PER_SDMMC;
    hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SET_RST_SDMMC;
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
    hal_SdioPwnReset();
}


//=============================================================================
// hal_SdioWakeUp
//-----------------------------------------------------------------------------
/// This function requests a resource of #HAL_SYS_FREQ_52M.
/// hal_SdioSleep() must be called before any other
//=============================================================================
PUBLIC VOID hal_SdioWakeUp(VOID)
{
    // Take a resource (The idea is to be able to get a 25Mhz clock)
//    hwp_sysCtrl->Clk_Per_Enable |=  SYS_CTRL_ENABLE_PER_SDMMC2;//modify 2012-02-24 ,wuys

//    wifi_i2c_rf_write_data(0x3f, 0x0000,1);


    if (!hal_ClkIsEnabled(gSdioClk))
        hal_ClkEnable(gSdioClk);
    hal_ClkSetRate(gSdioClk, HAL_CLK_RATE_250M);

//   hwp_configRegs->GPIO_Mode &=  ~CFG_REGS_MODE_PIN_SPI1_CLK;//modify 2012-02-24 ,wuys
#if 0
    hwp_configRegs->GPIO_Mode &=  ~0x1D000;;//modify 2012-02-24 ,wuys
    //   hwp_configRegs->Alt_mux_select &=  ~0x800;//modify 2012-02-24 ,wuys
    hwp_configRegs->Alt_mux_select |=  0x800;//modify 2012-02-24 ,wuys

#endif

//hal_SdioGpioClr();
    //hwp_gpio->gpio_oen_set_out
    //hwp_sysCtrl->Clk_Per_Enable |=  SYS_CTRL_ENABLE_PER_SDMMC2;//modify 2012-02-24 ,wuys

//hal_SdioSetClk(2000000);

}


//=============================================================================
// hal_SdioSleep
//-----------------------------------------------------------------------------
/// This function release the resource to #HAL_SYS_FREQ_32K.
//=============================================================================
PUBLIC VOID hal_SdioSleep(VOID)
{
    // We just release the resource, because the clock gating in sdmmc controller
    // will disable the clock. We should wait for the clock to be actually disabled
    // but the module does not seam to have a status for that...

    // Free a resource
    //  hwp_sysCtrl->Clk_Per_Disable |=  SYS_CTRL_DISABLE_PER_SDMMC2;//modify 2012-02-24 ,wuys
    //wifi_i2c_rf_write_data(0x3f, 0x0000,1);


//hwp_gpio->gpio_oen_set_out =

//hal_SdioGpioSet();
#if 0
    hwp_configRegs->GPIO_Mode |=  0x1D000;;//modify 2012-02-24 ,wuys
    //hwp_configRegs->Alt_mux_select |=  0x800;//modify 2012-02-24 ,wuys
    hwp_configRegs->Alt_mux_select &=  ~0x800;//modify 2012-02-24 ,wuys

#endif

    hal_ClkDisable(gSdioClk);

    // hwp_sysCtrl->Clk_Per_Disable |=  SYS_CTRL_DISABLE_PER_SDMMC2;//modify 2012-02-24 ,wuys


//hal_SdioSetClk(100000);

}


// =============================================================================
// hal_SdioSendCmd HAL_SDMMC_CMD_T HAL_SDMMC_CMD_GO_IDLE_STATE
// -----------------------------------------------------------------------------
/// Send a command to a SD card plugged to the sdmmc port.
/// @param cmd Command
/// @param arg Argument of the command
/// @param suspend Feature not implemented yet.
// =============================================================================
PUBLIC VOID hal_SdioSendCmds(HAL_SDIO_CMD_T cmd, UINT32 arg, BOOL suspend)
{
    UINT32 configReg = 0;
#if !defined(GALLITE_IS_8805)
    hwp_sdmmc->SDMMC_CONFIG = SDMMC_AUTO_FLAG_EN;
#else
    hwp_sdmmc->SDMMC_CONFIG = 0x00000000;
#endif


    switch (cmd)
    {
        case HAL_SDIO_CMD_GO_IDLE_STATE:
            configReg = SDMMC2_SDMMC2_SENDCMD;
            break;


        case HAL_SDIO_CMD_ALL_SEND_CID:
            configReg = SDMMC2_RSP_SEL_R2    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x51
            break;

        case HAL_SDIO_CMD_SEND_RELATIVE_ADDR:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SEND_IF_COND:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SET_DSR:
            configReg = SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_SELECT_CARD:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_SEND_CSD                :
            configReg = SDMMC2_RSP_SEL_R2    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_STOP_TRANSMISSION        :
            configReg = 0; // TODO
            break;

        case HAL_SDIO_CMD_SEND_STATUS            :
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_SET_BLOCKLEN            :
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_READ_SINGLE_BLOCK        :
            configReg =  SDMMC2_RD_WT_SEL_READ
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x111
            break;

        case HAL_SDIO_CMD_READ_MULT_BLOCK        :
            configReg =  SDMMC2_S_M_SEL_MULTIPLE
                         | SDMMC2_RD_WT_SEL_READ
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x511;
            break;

        case HAL_SDIO_CMD_WRITE_SINGLE_BLOCK    :
            configReg = SDMMC2_RD_WT_SEL_WRITE
                        | SDMMC2_RD_WT_EN
                        | SDMMC2_RSP_SEL_OTHER
                        | SDMMC2_RSP_EN
                        | SDMMC2_SDMMC2_SENDCMD; // 0x311
            break;

        case HAL_SDIO_CMD_WRITE_MULT_BLOCK        :
            configReg =  SDMMC2_S_M_SEL_MULTIPLE
                         | SDMMC2_RD_WT_SEL_WRITE
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x711
            break;

        case HAL_SDIO_CMD_APP_CMD                :
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SET_BUS_WIDTH            :
        case HAL_SDIO_CMD_SWITCH:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SEND_NUM_WR_BLOCKS    :
            configReg =  SDMMC2_RD_WT_SEL_READ
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x111
            break;

        case HAL_SDIO_CMD_SET_WR_BLK_COUNT        :
            configReg =  SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_IO_SEND_OP_COND:
        case HAL_SDIO_CMD_SEND_OP_COND:
            configReg =  SDMMC2_RSP_SEL_R3    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x31
            break;

        default:
            break;
    }

    // TODO Add suspend management
    hwp_sdmmc->SDMMC_CMD_INDEX = SDMMC2_COMMAND(cmd);
    hwp_sdmmc->SDMMC_CMD_ARG   = SDMMC2_ARGUMENT(arg);

#if !defined(GALLITE_IS_8805)
    hwp_sdmmc->SDMMC_CONFIG    = configReg |SDMMC2_AUTO_FLAG_EN;
#else
    hwp_sdmmc->SDMMC_CONFIG    = configReg ;
#endif

}

PUBLIC BOOL hal_SdioFifoEmpty(HAL_IFC_REQUEST_ID_T requestId, UINT8 channel)
{

    // Return whether the TC from the current IFC channel is empty.
    return hal_IfcChannelIsFifoEmpty(requestId,channel);
}
PUBLIC VOID hal_SdioReadFlushFifo(void)
{

    hal_IfcChannelFlush(HAL_IFC_SDMMC2_RX,  g_halSdioReadCh);
}





PUBLIC VOID hal_SdioSendCmd(HAL_SDIO_CMD_T cmd, UINT32 arg, BOOL suspend, UINT8 direct,BOOL multiblock)
{
    UINT32 configReg = 0;
#if ( (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE && defined(GALLITE_IS_8806)) || \
      (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) )
    if(multiblock)
        hwp_sdmmc->SDMMC_CONFIG = 0x00000000;
    else
        hwp_sdmmc->SDMMC_CONFIG = SDMMC2_AUTO_FLAG_EN;
#else
    hwp_sdmmc->SDMMC_CONFIG = 0x00000000;
#endif


    switch (cmd)
    {
        case HAL_SDIO_CMD_GO_IDLE_STATE:
            configReg = SDMMC2_SDMMC2_SENDCMD;
            break;


        case HAL_SDIO_CMD_ALL_SEND_CID:
            configReg = SDMMC2_RSP_SEL_R2    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x51
            break;

        case HAL_SDIO_CMD_SEND_RELATIVE_ADDR:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SEND_IF_COND:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SET_DSR:
            configReg = SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_SELECT_CARD:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_SEND_CSD                :
            configReg = SDMMC2_RSP_SEL_R2    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_STOP_TRANSMISSION        :
            configReg = 0; // TODO
            break;

        case HAL_SDIO_CMD_SEND_STATUS            :
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_SET_BLOCKLEN            :
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD;
            break;

        case HAL_SDIO_CMD_READ_SINGLE_BLOCK        :
            configReg =  SDMMC2_RD_WT_SEL_READ
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x111
            break;

        case HAL_SDIO_CMD_READ_MULT_BLOCK        :
            configReg =  SDMMC2_S_M_SEL_MULTIPLE
                         | SDMMC2_RD_WT_SEL_READ
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x511;
            break;

        case HAL_SDIO_CMD_WRITE_SINGLE_BLOCK    :
            configReg = SDMMC2_RD_WT_SEL_WRITE
                        | SDMMC2_RD_WT_EN
                        | SDMMC2_RSP_SEL_OTHER
                        | SDMMC2_RSP_EN
                        | SDMMC2_SDMMC2_SENDCMD; // 0x311
            break;

        case HAL_SDIO_CMD_WRITE_MULT_BLOCK        :
            configReg =  SDMMC2_S_M_SEL_MULTIPLE
                         | SDMMC2_RD_WT_SEL_WRITE
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x711
            break;

        case HAL_SDIO_CMD_APP_CMD                :
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SET_BUS_WIDTH            :
        case HAL_SDIO_CMD_SWITCH:
            configReg = SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_SEND_NUM_WR_BLOCKS    :
            configReg =  SDMMC2_RD_WT_SEL_READ
                         | SDMMC2_RD_WT_EN
                         | SDMMC2_RSP_SEL_OTHER
                         | SDMMC2_RSP_EN
                         | SDMMC2_SDMMC2_SENDCMD; // 0x111
            break;

        case HAL_SDIO_CMD_SET_WR_BLK_COUNT        :
            configReg =  SDMMC2_RSP_SEL_OTHER | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x11
            break;

        case HAL_SDIO_CMD_IO_SEND_OP_COND:
        case HAL_SDIO_CMD_SEND_OP_COND:
            configReg =  SDMMC2_RSP_SEL_R3    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x31
            break;
        case 5:
            configReg =  SDMMC2_RSP_SEL_OTHER    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x31
            break;
        case 52:
            configReg =  SDMMC2_RSP_SEL_OTHER    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x31
            break;
        case 53:
            if(direct == 0)
            {
                configReg = SDMMC2_RD_WT_SEL_READ

                            | SDMMC2_RD_WT_EN
                            | SDMMC2_RSP_SEL_OTHER
                            | SDMMC2_RSP_EN
                            | SDMMC2_SDMMC2_SENDCMD;
            }
            else
            {
                configReg = SDMMC2_RD_WT_SEL_WRITE
                            | SDMMC2_RD_WT_EN
                            | SDMMC2_RSP_SEL_OTHER
                            | SDMMC2_RSP_EN
                            | SDMMC2_SDMMC2_SENDCMD;
            } // 0x311
            break;

        default:
            configReg =  SDMMC2_RSP_SEL_OTHER    | SDMMC2_RSP_EN | SDMMC2_SDMMC2_SENDCMD; // 0x31

            break;
    }

    // TODO Add suspend management
    hwp_sdmmc->SDMMC_CMD_INDEX = SDMMC_COMMAND(cmd);
    hwp_sdmmc->SDMMC_CMD_ARG   = SDMMC_ARGUMENT(arg);

#if ( (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE && defined(GALLITE_IS_8806)) || \
      (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) )
    if(multiblock)
        hwp_sdmmc->SDMMC_CONFIG    = configReg|SDMMC2_S_M_SEL;
    else
        hwp_sdmmc->SDMMC_CONFIG    = configReg |SDMMC2_AUTO_FLAG_EN;
#else
    if(multiblock)
        hwp_sdmmc->SDMMC_CONFIG    = configReg|SDMMC2_S_M_SEL;
  else
    hwp_sdmmc->SDMMC_CONFIG    = configReg ;
#endif

}

// =============================================================================
// hal_SdioNeedResponse
// -----------------------------------------------------------------------------
/// Tells if the given command waits for a reponse.
/// @return \c TRUE if the command needs an answer, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SdioNeedResponse(HAL_SDIO_CMD_T cmd)
{
    switch (cmd)
    {
        case HAL_SDIO_CMD_GO_IDLE_STATE:
        case HAL_SDIO_CMD_SET_DSR:
        case HAL_SDIO_CMD_STOP_TRANSMISSION:
            return FALSE;
            break;


        case HAL_SDIO_CMD_ALL_SEND_CID:
        case HAL_SDIO_CMD_SEND_RELATIVE_ADDR:
        case HAL_SDIO_CMD_SEND_IF_COND:

        case HAL_SDIO_CMD_SELECT_CARD:
        case HAL_SDIO_CMD_SEND_CSD:
        case HAL_SDIO_CMD_SEND_STATUS:
        case HAL_SDIO_CMD_SET_BLOCKLEN:
        case HAL_SDIO_CMD_READ_SINGLE_BLOCK:
        case HAL_SDIO_CMD_READ_MULT_BLOCK:
        case HAL_SDIO_CMD_WRITE_SINGLE_BLOCK:
        case HAL_SDIO_CMD_WRITE_MULT_BLOCK:
        case HAL_SDIO_CMD_APP_CMD:
        case HAL_SDIO_CMD_SET_BUS_WIDTH:
        case HAL_SDIO_CMD_SEND_NUM_WR_BLOCKS:
        case HAL_SDIO_CMD_SET_WR_BLK_COUNT:
        case HAL_SDIO_CMD_IO_SEND_OP_COND:
        case HAL_SDIO_CMD_SEND_OP_COND:
        case HAL_SDIO_CMD_SWITCH:
            return TRUE;
            break;

        default:
            // HAL_ASSERT(FALSE, "Unsupported SDMMC2 command:%d", cmd);
            // Dummy return, for the compiler to be pleased.
            return TRUE;
            break;
    }
}


// =============================================================================
// hal_SdioCmdDone
// -----------------------------------------------------------------------------
/// @return \c TRUE of there is not command pending or being processed.
// =============================================================================
PUBLIC BOOL hal_SdioCmdDone(VOID)
{
    return (!(hwp_sdmmc->SDMMC_STATUS & SDMMC_NOT_SDMMC_OVER));
}


// =============================================================================
// hal_SdioGetCardDetectPinLevel
// -----------------------------------------------------------------------------
/// @return \c TRUE if card detect (DAT3) line is high,
///         \c FALSE if the line is low.
/// User must check with SD spec and board pull-up/down resistor to
/// interpret this value.
// =============================================================================
PUBLIC BOOL hal_SdioGetCardDetectPinLevel(VOID)
{
    HAL_ASSERT(FALSE, "TODO: hal_SdioGetCardDetectPinLevel not implemented for your chip!");
    return FALSE;
}


//=============================================================================
// hal_SdioUpdateDivider
//-----------------------------------------------------------------------------
/// Update the SDMMC2 module divider to keep the desired frequency, whatever
/// the system frequency is.
/// @param sysFreq System Frequency currently applied or that will be applied
/// if faster.
//=============================================================================
UINT32 hal_SdioUpdateDivider(HAL_SYS_FREQ_T sysFreq)
{
    if (g_halSdioFreq != 0)
    {
        UINT32 divider = (sysFreq-1)/(2*g_halSdioFreq) + 1;
        if (divider>1)
        {
            divider = divider-1;
        }

        if (divider > 0xFF)
        {
            divider = 0xFF;
        }

        hwp_sdmmc->SDMMC_TRANS_SPEED = SDMMC2_SDMMC2_TRANS_SPEED(divider);
    }
    return g_halSdioFreq;
}


// =============================================================================
// hal_SdioSetClk
// -----------------------------------------------------------------------------
/// Set the SDMMC2 clock frequency to something inferior but close to that,
/// taking into account system frequency.
// =============================================================================
PUBLIC VOID hal_SdioSetClk(UINT32 clock)
{
    // TODO Add assert to stay on supported values ?
    g_halSdioFreq = clock;

    UINT32 scStatus = hal_SysEnterCriticalSection();
    // Update the divider takes care of the registers configuration
    hal_SdioUpdateDivider(hal_SysGetFreq());
    hal_SysExitCriticalSection(scStatus);
}

// =============================================================================
// hal_SdmmcGetOpStatus HAL_SDMMC_OP_STATUS_T
// -----------------------------------------------------------------------------
/// @return The operational status of the SDMMC2 module.
// =============================================================================
PUBLIC HAL_SDIO_OP_STATUS_T hal_SdioGetOpStatus(VOID)
{
    return ((HAL_SDIO_OP_STATUS_T)(UINT32)hwp_sdmmc->SDMMC_STATUS);
}


// =============================================================================
// hal_SdmmcGetResp
// -----------------------------------------------------------------------------
/// This function is to be used to get the argument of the response of a
/// command. It is needed to provide the command index and its application
/// specific status, in order to determine the type of answer expected.
///
/// @param cmd Command to send
/// @param arg Pointer to a four 32 bit word array, where the argument will be
/// stored. Only the first case is set in case of a response of type R1, R3 or
/// R6, 4 if it is a R2 response.
/// @param suspend Unsupported
// =============================================================================
PUBLIC VOID hal_SdioGetResp(HAL_SDIO_CMD_T cmd, UINT32* arg, BOOL suspend)
{
    // TODO Check in the spec for all the commands response types
    switch (cmd)
    {
        // If they require a response, it is cargoed
        // with a 32 bit argument.
        case HAL_SDIO_CMD_GO_IDLE_STATE       :
        case HAL_SDIO_CMD_SEND_RELATIVE_ADDR  :
        case HAL_SDIO_CMD_SEND_IF_COND        :
        case HAL_SDIO_CMD_SET_DSR               :
        case HAL_SDIO_CMD_SELECT_CARD           :
        case HAL_SDIO_CMD_STOP_TRANSMISSION   :
        case HAL_SDIO_CMD_SEND_STATUS           :
        case HAL_SDIO_CMD_SET_BLOCKLEN           :
        case HAL_SDIO_CMD_READ_SINGLE_BLOCK   :
        case HAL_SDIO_CMD_READ_MULT_BLOCK       :
        case HAL_SDIO_CMD_WRITE_SINGLE_BLOCK  :
        case HAL_SDIO_CMD_WRITE_MULT_BLOCK       :
        case HAL_SDIO_CMD_APP_CMD               :
        case HAL_SDIO_CMD_SET_BUS_WIDTH       :
        case HAL_SDIO_CMD_SEND_NUM_WR_BLOCKS  :
        case HAL_SDIO_CMD_SET_WR_BLK_COUNT       :
        case HAL_SDIO_CMD_IO_SEND_OP_COND    :
        case HAL_SDIO_CMD_SEND_OP_COND           :
        case HAL_SDIO_CMD_SWITCH:
            arg[0] = hwp_sdmmc->SDMMC_RESP_ARG3;
            arg[1] = 0;
            arg[2] = 0;
            arg[3] = 0;
            break;

        // Those response arguments are 128 bits
        case HAL_SDIO_CMD_ALL_SEND_CID           :
        case HAL_SDIO_CMD_SEND_CSD               :
            arg[0] = hwp_sdmmc->SDMMC_RESP_ARG0;
            arg[1] = hwp_sdmmc->SDMMC_RESP_ARG1;
            arg[2] = hwp_sdmmc->SDMMC_RESP_ARG2;
            arg[3] = hwp_sdmmc->SDMMC_RESP_ARG3;
            break;

        default:
            //     HAL_ASSERT(FALSE, "hal_SdmmcGetResp called with "
            //                        "an unsupported command: %d", cmd);
            arg[0] = hwp_sdmmc->SDMMC_RESP_ARG0;
            arg[1] = hwp_sdmmc->SDMMC_RESP_ARG1;
            arg[2] = hwp_sdmmc->SDMMC_RESP_ARG2;
            arg[3] = hwp_sdmmc->SDMMC_RESP_ARG3;

            break;
    }
}


// =============================================================================
// hal_SdmmcGetResp32BitsArgument
// -----------------------------------------------------------------------------
/// This function is to be used to get the argument of the response of a
/// command triggerring a response with a 32 bits argument (typically,
/// R1, R3 or R6).
/// @return 32 bits of arguments of a 48 bits response token
// =============================================================================
PUBLIC UINT32 hal_SdioGetResp32BitsArgument(VOID)
{
    return hwp_sdmmc->SDMMC_RESP_ARG3;
}


// =============================================================================
// hal_SdmmcGetResp128BitsArgument
// -----------------------------------------------------------------------------
/// This function is to be used to get the argument of the response of a
/// command triggerring a response with a 128 bits argument (typically,
/// R2).
/// @param Pointer to a 4 case arrays of 32 bits where the argument of the
/// function will be stored.
// =============================================================================
PUBLIC VOID hal_SdioGetResp128BitsArgument(UINT32* buf)
{
    buf[0] = hwp_sdmmc->SDMMC_RESP_ARG0;
    buf[1] = hwp_sdmmc->SDMMC_RESP_ARG1;
    buf[2] = hwp_sdmmc->SDMMC_RESP_ARG2;
    buf[3] = hwp_sdmmc->SDMMC_RESP_ARG3;
}


// =============================================================================
// hal_SdmmcEnterDataTransferMode
// -----------------------------------------------------------------------------
/// Configure the SDMMC2 module to support data transfers
/// FIXME Find out why we need that out of the transfer functions...
// =============================================================================
PUBLIC VOID hal_SdioEnterDataTransferMode(VOID)
{
    hwp_sdmmc->SDMMC_CONFIG |= SDMMC_RD_WT_EN;
}


// =============================================================================
// hal_SdmmcSetDataWidth  HAL_SDMMC_DATA_BUS_WIDTH_T
// -----------------------------------------------------------------------------
/// Set the data bus width
/// @param width Number of line of the SD data bus used.
// =============================================================================
PUBLIC VOID hal_SdioSetDataWidth(HAL_SDIO_DIRECTION_T width)
{
    switch (width)
    {
        case HAL_SDIO_DATA_BUS_WIDTH_1:
            hwp_sdmmc->SDMMC_DATA_WIDTH = 1;
            break;

        case HAL_SDIO_DATA_BUS_WIDTH_4:
            hwp_sdmmc->SDMMC_DATA_WIDTH = 4;
            break;

        default:
            HAL_ASSERT(FALSE, "hal_SdmmcSetDataWidth(%d) is an invalid width",
                       width);
    }
}


// =============================================================================
// hal_SdmmcTransfer
// -----------------------------------------------------------------------------
/// Start the ifc transfer to feed the SDMMC2 controller fifo.
/// @param transfer Transfer to program.
/// @return HAL_ERR_NO or HAL_ERR_RESOURCE_BUSY.
// =============================================================================
PUBLIC HAL_ERR_T hal_SdioTransfer(HAL_SDIO_TRANSFER_T* transfer)
{
    UINT8 channel = 0;
    HAL_IFC_REQUEST_ID_T ifcReq = HAL_IFC_NO_REQWEST;
    UINT32 length = 0;
    UINT32 lengthExp = 0;


    HAL_ASSERT((transfer->blockSize>=4) && (transfer->blockSize<=2048),
               "Block Length(%d) is invalid!\n", transfer->blockSize);

    length =  transfer->blockSize;

    // The block size register
    while (length != 1)
    {
        length >>= 1;
        lengthExp++;
    }

    // Configure amount of data
    hwp_sdmmc->SDMMC_BLOCK_CNT  = SDMMC2_SDMMC2_BLOCK_CNT(transfer->blockNum);
    hwp_sdmmc->SDMMC_BLOCK_SIZE = SDMMC2_SDMMC2_BLOCK_SIZE(lengthExp);

    // Configure Bytes reordering
    hwp_sdmmc->apbi_ctrl_sdmmc = SDMMC_SOFT_RST_L | SDMMC2_L_ENDIAN(1);

    switch (transfer->direction)
    {
        case HAL_SDIO_DIRECTION_READ:
            ifcReq = HAL_IFC_SDMMC_RX;
            break;

        case HAL_SDIO_DIRECTION_WRITE:
            ifcReq = HAL_IFC_SDMMC_TX;
            break;

        default:
            HAL_ASSERT(FALSE, "hal_SdmmcTransfer with erroneous %d direction",
                       transfer->direction);
            break;
    }

    channel = hal_IfcTransferStart(ifcReq, transfer->sysMemAddr,
                                   ( transfer->blockNum*transfer->blockSize),
                                   HAL_IFC_SIZE_32_MODE_AUTO);
    if (channel == HAL_UNKNOWN_CHANNEL)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        // Record Channel used.
        if (transfer->direction == HAL_SDIO_DIRECTION_READ)
        {
            g_halSdioReadCh = channel;
        }
        else
        {
            g_halSdioWriteCh = channel;
        }
        return HAL_ERR_NO;
    }
}

PUBLIC HAL_ERR_T hal_SdioTransferinit(HAL_SDIO_TRANSFER_T* transfer)
{
    // UINT8 channel = 0;
//   HAL_IFC_REQUEST_ID_T ifcReq = HAL_IFC_NO_REQWEST;
    UINT32 length = 0;
    UINT32 lengthExp = 0;


    HAL_ASSERT((transfer->blockSize>=4) && (transfer->blockSize<=2048),
               "Block Length(%d) is invalid!\n", transfer->blockSize);

    length =  transfer->blockSize;

    // The block size register
    while (length != 1)
    {
        length >>= 1;
        lengthExp++;
    }

    // Configure amount of data
    hwp_sdmmc->SDMMC_BLOCK_CNT  = SDMMC2_SDMMC2_BLOCK_CNT(transfer->blockNum);
    hwp_sdmmc->SDMMC_BLOCK_SIZE = SDMMC2_SDMMC2_BLOCK_SIZE(lengthExp);
    hwp_sdmmc->apbi_ctrl_sdmmc = SDMMC2_SOFT_RST_L | SDMMC2_L_ENDIAN(1);

//   hwp_sdmmc2->apbi_ctrl_sdmmc =  SDMMC2_L_ENDIAN(1);

    return HAL_ERR_NO;

    // Configure Bytes reordering
    //hwp_sdmmc2->apbi_ctrl_sdmmc = SDMMC2_SOFT_RST_L | SDMMC2_L_ENDIAN(1);
}
// =============================================================================
// hal_SdmmcTransferDone
// -----------------------------------------------------------------------------
/// Check the end of transfer status.
/// Attention: This means the SDMMC2 module has finished to transfer data.
/// In case of a read operation, the Ifc will finish its transfer shortly
/// after. Considering the way this function is used (after reading at least
/// 512 bytes, and flushing cache before releasing the data to the user), and
/// the fifo sizes, this is closely equivalent to the end of the transfer.
/// @return \c TRUE if a transfer is over, \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SdioTransferDone_bak(VOID)
{
    // The link is not full duplex. We check both the
    // direction, but only one can be in progress at a time.

    if (g_halSdioReadCh != HAL_UNKNOWN_CHANNEL)
    {
        // Operation in progress is a read.
        // The SDMMC2 module itself can know it has finished
        if ((hwp_sdmmc->SDMMC_INT_STATUS & SDMMC2_DAT_OVER_INT)
                && (hwp_sysIfc->std_ch[g_halSdioReadCh].tc == 0))
        {
            // Transfer is over
            hwp_sdmmc->SDMMC_INT_CLEAR = SDMMC2_DAT_OVER_CL;
            hal_IfcChannelRelease(HAL_IFC_SDMMC_RX, g_halSdioReadCh);

            // We finished a read
            g_halSdioReadCh = HAL_UNKNOWN_CHANNEL;

            //  Put the FIFO in reset state.
            hwp_sdmmc->apbi_ctrl_sdmmc = 0 | SDMMC2_L_ENDIAN(1);

            return TRUE;
        }
    }

    if (g_halSdioWriteCh != HAL_UNKNOWN_CHANNEL)
    {
        // Operation in progress is a write.
        // The SDMMC2 module itself can know it has finished
        if ((hwp_sdmmc->SDMMC_INT_STATUS & SDMMC2_DAT_OVER_INT)
                && (hwp_sysIfc->std_ch[g_halSdioWriteCh].tc == 0))
        {
            // Transfer is over
            hwp_sdmmc->SDMMC_INT_CLEAR = SDMMC2_DAT_OVER_CL;
            hal_IfcChannelRelease(HAL_IFC_SDMMC_TX, g_halSdioWriteCh);

            // We finished a write
            g_halSdioWriteCh = HAL_UNKNOWN_CHANNEL;

            //  Put the FIFO in reset state.
            hwp_sdmmc->apbi_ctrl_sdmmc = 0 | SDMMC2_L_ENDIAN(1);

            return TRUE;
        }
    }

    // there's still data running through a pipe (or no transfer in progress ...)
    return FALSE;

}
PUBLIC BOOL hal_SdioTransferFiFoDone(VOID)
{
    if ((hwp_sdmmc->SDMMC_INT_STATUS & SDMMC2_DAT_OVER_INT))
    {
        // Transfer is over
        //    hwp_sdmmc2->SDMMC2_INT_CLEAR = SDMMC2_DAT_OVER_CL;
        //    hal_IfcChannelRelease(HAL_IFC_SDMMC2_RX, g_halSdioReadCh);

        // We finished a read
        //   g_halSdioReadCh = HAL_UNKNOWN_CHANNEL;

        //  Put the FIFO in reset state.
        //  hwp_sdmmc2->apbi_ctrl_sdmmc = 0 | SDMMC2_L_ENDIAN(1);

        return TRUE;
    }




    // there's still data running through a pipe (or no transfer in progress ...)
    return FALSE;

}
PUBLIC BOOL hal_SdioDataInd(VOID)
{
    if ((hwp_sdmmc->SDMMC_INT_STATUS & SDMMC2_DATA_IND_INT))
    {
        // Transfer is over
        //    hwp_sdmmc2->SDMMC2_INT_CLEAR = SDMMC2_DAT_OVER_CL;
        //    hal_IfcChannelRelease(HAL_IFC_SDMMC2_RX, g_halSdioReadCh);

        // We finished a read
        //   g_halSdioReadCh = HAL_UNKNOWN_CHANNEL;

        //  Put the FIFO in reset state.
        //  hwp_sdmmc2->apbi_ctrl_sdmmc = 0 | SDMMC2_L_ENDIAN(1);

        return TRUE;
    }




    // there's still data running through a pipe (or no transfer in progress ...)
    return FALSE;

}

PUBLIC VOID hal_SdioTransferFiFoDoneClr(VOID)
{
    hwp_sdmmc->SDMMC_INT_CLEAR = SDMMC2_DAT_OVER_CL;

}
PUBLIC VOID hal_SdioDataIndClr(VOID)
{
    hwp_sdmmc->SDMMC_INT_CLEAR = SDMMC2_DATA_IND_CL;

}
PUBLIC INT32 hal_SdioReadSdioConfig(VOID)
{
    return hwp_sdmmc->SDMMC_CONFIG ;

}

UINT32 hal_read_databyFIFO(void)
{
    UINT32 val;
    val = hwp_sdmmc->APBI_FIFO_TxRx;
    //    MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO: 0x%x", val);

    return val;


}
VOID hal_Write_databyFIFO(UINT32 vData)
{

    hwp_sdmmc->APBI_FIFO_TxRx = vData;
    //    MCD_TRACE(MCD_INFO_TRC, 0, "read FIFO: 0x%x", val);



}

UINT32 returnTC(void)
{
    UINT32 ret =0;
    ret = hwp_sysIfc->std_ch[g_halSdioWriteCh].tc;
    return ret;
}
UINT32 returnsdmmcSta(void)
{
//UINT32 ret =0;
    return ((UINT32)hwp_sdmmc->SDMMC_STATUS);
//return ret;
}
BOOL IsSdioBusy(void)
{
//UINT32 ret =0;
    return( ((UINT32)hwp_sdmmc->SDMMC_STATUS & (SDMMC2_DL_BUSY|SDMMC2_BUSY))!=0x00);
//return ret;
}

PUBLIC BOOL hal_SdioTransferDone(VOID)
{
    // The link is not full duplex. We check both the
    // direction, but only one can be in progress at a time.
    if (g_halSdioReadCh != HAL_UNKNOWN_CHANNEL)
    {
        // Operation in progress is a read.
        // The SDMMC2 module itself can know it has finished
        if ((hwp_sdmmc->SDMMC_INT_STATUS & SDMMC2_DAT_OVER_INT)
                && (hwp_sysIfc->std_ch[g_halSdioReadCh].tc == 0))
        {
            // Transfer is over
            hwp_sdmmc->SDMMC_INT_CLEAR = SDMMC2_DAT_OVER_CL;
            hal_IfcChannelRelease(HAL_IFC_SDMMC_RX, g_halSdioReadCh);
            // We finished a read
            g_halSdioReadCh = HAL_UNKNOWN_CHANNEL;

            //  Put the FIFO in reset state.
            //    modify 2011_06_20, by wuys
            //    hwp_sdmmc->apbi_ctrl_sdmmc = 0 | SDMMC_L_ENDIAN(1);

            return TRUE;
        }
    }

    if (g_halSdioWriteCh != HAL_UNKNOWN_CHANNEL)
    {
        // Operation in progress is a write.
        // The SDMMC2 module itself can know it has finished
        if ((hwp_sdmmc->SDMMC_INT_STATUS & SDMMC2_DAT_OVER_INT)
                && (hwp_sysIfc->std_ch[g_halSdioWriteCh].tc == 0))
        {
            // Transfer is over
            hwp_sdmmc->SDMMC_INT_CLEAR = SDMMC2_DAT_OVER_CL;
            hal_IfcChannelRelease(HAL_IFC_SDMMC_TX, g_halSdioWriteCh);

            // We finished a write
            g_halSdioWriteCh = HAL_UNKNOWN_CHANNEL;

            //  Put the FIFO in reset state.
            //    modify 2011_06_20, by wuys
            //     hwp_sdmmc->apbi_ctrl_sdmmc = 0 | SDMMC_L_ENDIAN(1);

            return TRUE;
        }
    }

    // there's still data running through a pipe (or no transfer in progress ...)
    return FALSE;

}

// =============================================================================
// hal_SdioStopTransfer
// -----------------------------------------------------------------------------
/// Stop the ifc transfer feeding the SDMMC2 controller fifo.
/// @param transfer Transfer to program.
/// @return #HAL_ERR_NO
// =============================================================================
PUBLIC HAL_ERR_T hal_SdioStopTransfer(HAL_SDIO_TRANSFER_T* transfer)
{
    // Configure amount of data
    hwp_sdmmc->SDMMC_BLOCK_CNT  = SDMMC2_SDMMC2_BLOCK_CNT(0);
    hwp_sdmmc->SDMMC_BLOCK_SIZE = SDMMC2_SDMMC2_BLOCK_SIZE(0);

    //  Put the FIFO in reset state.
    hwp_sdmmc->apbi_ctrl_sdmmc = 0 | SDMMC2_L_ENDIAN(1);


    if (transfer->direction == HAL_SDIO_DIRECTION_READ)
    {
        hal_IfcChannelFlush(HAL_IFC_SDMMC_RX, g_halSdioReadCh);
        while(!hal_IfcChannelIsFifoEmpty(HAL_IFC_SDMMC_RX, g_halSdioReadCh));
        hal_IfcChannelRelease(HAL_IFC_SDMMC_RX, g_halSdioReadCh);
        g_halSdioReadCh = HAL_UNKNOWN_CHANNEL;
    }
    else
    {
        hal_IfcChannelFlush(HAL_IFC_SDMMC_TX, g_halSdioWriteCh);
        while(!hal_IfcChannelIsFifoEmpty(HAL_IFC_SDMMC_TX, g_halSdioReadCh));
        hal_IfcChannelRelease(HAL_IFC_SDMMC_TX, g_halSdioWriteCh);
        g_halSdioWriteCh = HAL_UNKNOWN_CHANNEL;
    }
    return HAL_ERR_NO;
}


// =============================================================================
// hal_SdioIsBusy
// -----------------------------------------------------------------------------
/// Check if the SD/MMC is busy.
///
/// @return \c TRUE if the SD/MMC controller is busy.
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SdioIsBusy(VOID)
{
    if (g_halSdioReadCh    != HAL_UNKNOWN_CHANNEL
            || g_halSdioWriteCh   != HAL_UNKNOWN_CHANNEL
            || ((hwp_sdmmc->SDMMC_STATUS & (SDMMC2_NOT_SDMMC2_OVER | SDMMC2_BUSY | SDMMC2_DL_BUSY)) != 0)
       )
    {
        // SD/MMc is busy doing something.
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


// =============================================================================
// hal_SdioDmaIsEnable
// -----------------------------------------------------------------------------
/// Check if the BB-chip DMA is enable
///
/// @return \c TRUE if the BB chip DMA is enable.
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL hal_SdioDmaIsEnable(VOID)
{

    if((hal_SysGetChipId(HAL_SYS_CHIP_PROD_ID) == HAL_SYS_CHIP_PROD_ID_8808)
            &&(rfd_XcvGetChipId() < XCV_CHIP_ID_8808_V4))
    {
        return FALSE;

    }
    return TRUE;


}

// ============================================================================
// hal_SdioIrqHandler
// ----------------------------------------------------------------------------
/// SDIO IRQ handler, clearing the IRQ cause regiter and calling the user handler
/// defined by hal_SdioIrqSetHandler.
/// @param interruptId Id with which the handler is entered
// ============================================================================
PROTECTED VOID hal_SdioIrqHandler(UINT8 interruptId)
{
    if (g_halSdioRegistry != NULL)
    {
        g_halSdioRegistry();
    }
}


/***************************************************************/
/* use gpio for wifi data indicatior interrupt, because there is no this IRQ in SDMMC1*/
/***************************************************************/

HAL_MODULE_CLK_INIT(SDIO)
{
    gSdioClk = hal_ClkGet(FOURCC_SDIO);
    HAL_ASSERT((gSdioClk != NULL), "Clk SDIO not found!");
}

#endif


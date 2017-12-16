/**
 *****************************************************************************
 * @file     clk.h
 * @author   lujiangang
 * @version  V1.0.0
 * @date     29-May-2013
 * @brief    clk module driver interface
 *****************************************************************************
 * @attention
 *
 * This module provide clk driver, all the other module need to consider how to
 * use these function
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define NORMAL_MODE                             0     /**<normal mode provide 12.288m and 11.2896M */
#define USB_MODE                                1     /**<usb mode always 12M */
#define EXTCLK_NORMAL_MODE                      2     /**<extern clk input: The external clock freq must be a multiple of sample rate*/
#define EXTCLK_USB_MODE                         3     /**<extern clk input; The external clock freq must be 12MHz>*/    

#define MCLK_SRC_DPLL_12MHZ                                 0     /**<generally is usb mode */
#define MCLK_SRC_EXTCLK                                     1     /**<extern clk input */
#define MCLK_SRC_DPLL_12M288                                2     /**<generate 12.288M */
#define MCLK_SRC_DPLL_11M2896                               3     /**<generate 11.2896M */

/**
 * CLK module switch macro define
 */
typedef enum _CLK_MODULE_SWITCH
{
    BUART_CLK_EN = (1 << 0),            /**<buart module clk switch */
    FUART_CLK_EN = (1 << 1),            /**<fuart module clk switch */
    DLUT_CLK_EN = (1 << 2),             /**<DLUT module clk switch */
    DMAC_CLK_EN = (1 << 3),             /**<DMAC module clk switch */
    IIS_CLK_EN = (1 << 4),              /**<IIS module clk switch */
    LCDC_CLK_EN = (1 << 5),             /**<LCDC module clk switch */
    PHUB_CLK_EN = (1 << 6),             /**<PHUB module clk switch */
    SD_CLK_EN = (1 << 7),               /**<SD module clk switch */
    SPIM_CLK_EN = (1 << 8),             /**<SPIM module clk switch */
    SPIS_CLK_EN = (1 << 9),             /**<SPIS module clk switch */
    TIMER0_CLK_EN = (1 << 10),          /**<TIMER0 module clk switch */
    TIMER1_CLK_EN = (1 << 11),          /**<TIMER1 module clk switch */
    DECODER_CLK_EN = (1 << 12),         /**<DECODER module clk switch */
    SDRAM_CLK_EN = (1 << 13),           /**<SDRAM module clk switch */
    FSHC_CLK_EN = (1 << 14),            /**<FSHC module clk switch */
    USB_CLK_EN = (1 << 15),             /**<USB module clk switch */
    CODEC_CLK_EN = (1 << 16),           /**<CODEC module clk switch */
    ALL_MODULE_CLK_SWITCH = (0x1FFFF),  /**<all module clk SWITCH*/
} CLK_MODULE_SWITCH;

/**
 * CLK module GATE switch macro define
 */
typedef enum _CLK_MODULE_GATE_SWITCH
{
    CACHE_CLK_GATE_EN = (1 << 0),            /**<CACHE module clk gating switch */
    CM3_CLK_GATE_EN = (1 << 1),              /**<CM3 module clk gating switch */
    CODEC_CLK_GATE_EN = (1 << 2),            /**<CODEC module clk gating switch */
    DMAC_CLK_GATE_EN = (1 << 3),             /**<DMAC module clk gating switch */
    SDRAM_CLK_GATE_EN = (1 << 4),            /**<SDRAM module clk gating switch */
    FSHC_CLK_GATE_EN = (1 << 5),             /**<FSHC module clk gating switch */
    LCDC_CLK_GATE_EN = (1 << 6),             /**<LCDC module clk gating switch */
    PMEM_CLK_GATE_EN = (1 << 7),             /**<PMEM module clk gating switch */
    ROM_CLK_GATE_EN = (1 << 8),              /**<ROM module clk gating switch */
    XMEM_CLK_GATE_EN = (1 << 9),             /**<XMEM module clk gating switch */
    VMEM_CLK_GATE_EN = (1 << 10),            /**<VMEM module clk gating switch */
    GPIO_CLK_GATE_EN = (1 << 11),            /**<GPIO module clk gating switch */
    DLUT_CLK_GATE_EN = (1 << 12),            /**<DLUT module clk gating switch */
    DEC_TX_CLK_GATE_EN = (1 << 13),          /**<DEC TX module clk gating switch */
    DEC_REG_CLK_GATE_EN = (1 << 14),         /**<DEC REG module clk gating switch */
    DEC_MP3_CLK_GATE_EN = (1 << 15),         /**<DEC MP3 module clk gating switch */
    DEC_WMA_CLK_GATE_EN = (1 << 16),         /**<DEC WMA module clk gating switch */
    DEC_SBC_CLK_GATE_EN = (1 << 17),         /**<DEC SBC module clk gating switch */
    ALL_MODULE_CLK_GATE_SWITCH = (0x3FFFF),  /**<all module clk gating SWITCH*/	
} CLK_MODULE_GATE_SWITCH;

/**
 * @brief  select flash module clk
 * @param  ClkSel 0:pll_clk_60m
 *                1:sys_clk_96m   sel this clk will affect by sys clk div
 *                2:pll_clk_80m
 *                3:pll_clk_48m
 * @return NONE
 */
void ClkFshcClkSel(uint32_t ClkSel);

/**
 * @brief  open module clk
 * @param  ClkSel each module usb one bit control.
 *         totally 16bits, can use enum type 'CLK_MODULE_SWITCH' to sel
 *         which module clk need to open
 * @return NONE
 */
void ClkModuleEn(CLK_MODULE_SWITCH ClkSel);

/**
 * @brief  close module clk
 * @param  ClkSel:each module usb one bit control.
 *         totally 16bits, can use enum type 'CLK_MODULE_SWITCH' to sel which module clk need to close
 * @return NONE
 */
void ClkModuleDis(CLK_MODULE_SWITCH ClkSel);

/**
 * @brief  clk gate enable
 * @param  ClkGateSel 0~17 bits are valid, can use enum type 'CLK_MODULE_GATE_SWITCH'
 *         to sel which module clk need to open
 * @return NONE
 */
void ClkModuleGateEn(CLK_MODULE_GATE_SWITCH ClkGateSel);

/**
 * @brief  clk gate disable
 * @param  ClkGateSel 0~17 bits are valid, can use enum type 'CLK_MODULE_GATE_SWITCH'
 *         to sel which module clk need to close
 * @return NONE
 */
void ClkModuleGateDis(CLK_MODULE_GATE_SWITCH ClkGateSel);

/**
 * @brief  mclk sel
 * @param  Mode:
 *         0  sys clk 12M
 *         1  extern mclk input
 *         2  clk 12.288M
 *         3  clk 11.2896M
 * @return NONE
 */
void ClkMclkSel(uint8_t Mode);

/**
 * @brief  clk switch from rc48m to dpll
 *         sysclk is 96m, but fshc clk is 80m, doesn't affect by sys clk div
 * @param  ClkSrc 0:32.768K, 1~31:1M~31M
 * @return TRUE OR FALSE
 */
bool ClkPorRcToDpll(uint8_t ClkSrc);


/**
	The flowing two functions use none-blocking method to switch RC to DPLL.
	first use ClkPorRcToDpllSetting() to set the DPLL clk source 
	then use ClkCheckAndSwitchRCToDpll() to check wether DPLL is locked, if it is locked, then switch from RC to DPLL, return TRUE.
	if not, return FALSE
*/
/**
 * @brief  clk setting for rc48m to dpll
 *         sysclk is 96m, but fshc clk is 80m, doesn't affect by sys clk div
 * @param  ClkSrc 0:32.768K, 1~31:1M~31M
 * @return TRUE OR FALSE
 * 
 */
bool ClkPorRcToDpllSetting(uint8_t ClkSrc);

/**
 * @brief  clk switch from rc48m to dpll
 * @param  None
 * @return TRUE OR FALSE
 *		if DPLL is Locked, switch RC to DPLL, and return TRUE,
 *		if DPLL is not Locked, returen FALSE.
 */
bool ClkCheckAndSwitchRCToDpll(void);

/**
 * @brief  enable DPLL CLK GATING
 * @param  ClkGatEn 1:enable 0:disable
 * @return NONE
 */
void ClkDpllClkGatingEn(bool ClkGatEn);

/**
 * @brief  disable dpll auto calibration
 * @param  DefaultNdac must a average val
 * @return NONE
 */
void ClkDisDpllAutoCalibration(uint16_t DefaultNdac);


/**
 * @brief  enable DPLL auto calibration function
 * @param  NONE
 * @return DefaultNdac a average val for NDAC
 */
uint16_t ClkEnableAutoCalibrationDpll(void);

/**
 * @brief  generate 12M or 16M CLK
 * @param  Mode:
 *         0  clk 12M
 *         1  clk 16m
 * @return NONE
 */
void Clk12MOr16MGenerate(uint8_t Mode);

/**
 * @brief  clk switch from dpll to rc48m
 *         clk switch during working not por
 * @param  NONE
 * @return NONE
 */
void ClkSwitchDpllToRc(void);

/**
 * @brief  clk switch from rc48m to dpll
 *         clk switch during working not por
 * @param  NONE
 * @return NONE
 */
void ClkSwitchRcToDpll(void);

/**
 * @brief  DPLL close, for low power
 * @param  NONE
 * @return NONE
 */
void ClkDpllClose(void);

/**
 * @brief  get system clock selection during clock switch
 * @param  NONE
 * @return Mode:
 *         0  use sys 96M clk
 *         1  use sys 80M clk
 */
char ClkSysClkSelGet(void);

/**
 * @brief  during working switch sys clk
 * @param  Mode:
 *         0  use PLL 60MHz
 *         1  use sys clk
 *	     2  use PLL 80MHz
 *	     3  use PLL 48MHz
 * @return NONE
 */
char ClkFshcClkSelGet(void);

/**
 * @brief  get current clock source
 * @param  NONE
 * @return  0: RC clk; 1:DPLL clk
 */
uint32_t ClkModuleClkSelGet(void);

/**
 * @brief  during working switch sys clk
 * @param  Mode:
 *         0  use sys 96M clk
 *         1  use sys 80M clk
 * @return NONE
 */
void ClkSysClkSel(uint8_t Mode);


/**
 * @brief  sys clk div set
 * @param  DivVal(1~255):
 *         1  no div
 *         2  divided 2
 *          ...
 *         256 divided 256
 *         only support divide the frequency by even
 * @return NONE
 */
void ClkModuleDivSet(uint32_t DivVal);

/**
 * @brief  get sys div clk coefficient
 * @param  NONE
 * @return  sys div clk coefficient
 */
uint32_t ClkModuleDivGet(void);

/**
 * @brief  sys clk div disable
 * @param  NONE
 * @return NONE
 */
void ClkModuleDivDis(void);

/**
 * @brief  Set the Frequency of MCLK at each sampling rate.
 * @Param  SampleRate support nine
 * @Param  Mode	Clocking mode(1: USB mode, 0: Normal mode)
 * @return NONE
 */
void ClkMclkSelBySampleRate(uint32_t SampleRate, uint8_t Mode);

/**
 * @brief  after osc32k is working,use osc32k check rc48m,make sure sys can get correct freq at any time
 * @param  NONE
 * @return !0 current RC freq, 0 means is time out
 */
uint32_t ClkUseOsc32kCheckRc48m(void);

/**
 * @brief  get clk current status
 * @param  NONE
 * @return current clk status
 */
uint32_t ClkGetCurrentSysClkFreq(void);

/**
 * @brief  get clk current status
 * @param  NONE
 * @return 1:DPLL MODE  0:RC48M mode
 */
uint32_t ClkCurrentClkModeGet(void);

/**
 * @brief  osc32k no need ext capacitance set
 * @param  CapacityXiVal range:[0,15],step=2.587pf.base=12.8pf
 * @param  CapacityXoVal range:[0,15],step=2.587pf.base=12.8pf
 * @return NONE
 */
void ClkOsc32kNoNeedExtCapacitanceSet(uint8_t CapacityXiVal, uint8_t CapacityXoVal);

/**
 * @brief  set osc power mode
 * @param  Current range:[0,7],adjust current, default 7 is min,0 is the max
 * @return None
 */
void ClkOscPowerModeSel(uint8_t Current);

/**
 * @brief  RC48M trim frequency calibrated by 1MHz output
 * @param  NONE
 * @return 0:no 1MHz calibration output,otherwise return RC48M actual frequency after 1MHz calibration
 */
int RC48MHzTrimBy1MHz(void);

/**
 * @Brief	Notify clock divide or multiply frequency
 * @Param	PreFq System Frequency before changed
 * @Param	CurFq System Frequency after changed
 */
void ClkModuleDivSetNotify(unsigned int PreFq, unsigned int CurFq);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif

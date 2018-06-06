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


#ifndef _TCU_H_
#define _TCU_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'tcu'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define NB_TCO                                  (12)
#define NB_TCU_PROG_EVENTS                      (60)

// ============================================================================
// INTERNAL_TCO_MAPPING_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// Internal TCO mapping
    TCO_GMSK_ON                                 = 0x00000006,
    TCO_TX_OEN                                  = 0x00000007,
    TCO_TX_ON                                   = 0x00000008,
    TCO_RX_ON                                   = 0x00000009,
    TCO_RX_DEC_ON                               = 0x0000000A,
    TCO_PDN                                     = 0x0000000B
} INTERNAL_TCO_MAPPING_T;


// ============================================================================
// TCU_EVENT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
/// Clear TCO 0 : set the TCO 0 to the inactive state
/// To clear TCO n, use event 2*n
    CLR_TCO_0                                   = 0x00000000,
/// Set TCO 0 : set the TCO 0 to the active state
/// To set TCO n, use event 2*n+1
    SET_TCO_0                                   = 0x00000001,
/// ...
    CLR_TCO_1                                   = 0x00000002,
/// stop modulation
    STOP_GMSK                                   = 0x0000000C,
/// starts modulation and output on IQ DAC
    START_GMSK                                  = 0x0000000D,
    HIGHZ_IQ_DAC                                = 0x0000000E,
    DRIVE_IQ_DAC                                = 0x0000000F,
    DISABLE_IQ_DAC                              = 0x00000010,
    ENABLE_IQ_DAC                               = 0x00000011,
/// disable IQ ADC
    DISABLE_IQ_ADC                              = 0x00000012,
/// enable IQ ADC
    ENABLE_IQ_ADC                               = 0x00000013,
/// stop recording IQ samples
    STOP_RFIN_RECORD                            = 0x00000014,
/// start recording IQ samples
    START_RFIN_RECORD                           = 0x00000015,
/// Clear RF_PDN
    CLR_PDN                                     = 0x00000016,
/// Set RF_PDN
    SET_PDN                                     = 0x00000017,
/// Send RF spi command
    SEND_SPI_CMD                                = 0x00000018,
    NEXT_GAIN                                   = 0x00000019,
    FIRST_GAIN                                  = 0x0000001A,
    NEXT_FC                                     = 0x0000001B,
/// Start Ramp 0
    PA_RAMP0                                    = 0x0000001C,
/// Start Ramp 1
    PA_RAMP1                                    = 0x0000001D,
/// Start Ramp 2
    PA_RAMP2                                    = 0x0000001E,
/// Start Ramp 3
    PA_RAMP3                                    = 0x0000001F,
/// Start Ramp 4
    PA_RAMP4                                    = 0x00000020,
    RX_SOC                                      = 0x00000021,
    DIGRF_STB                                   = 0x00000022,
/// Trigger BCPU TCU irq 0
    BCPU_TCU_IRQ0                               = 0x00000023,
/// Trigger BCPU TCU irq 1
    BCPU_TCU_IRQ1                               = 0x00000024,
/// Trigger XCPU TCU irq 0
    XCPU_TCU_IRQ0                               = 0x00000025,
/// Trigger XCPU TCU irq 1
    XCPU_TCU_IRQ1                               = 0x00000026,
/// End of the WakeUp Mode
    WAKEUP_DONE                                 = 0x00000027,
    NO_EVENT                                    = 0x0000003F
} TCU_EVENT_T;

#define LP_FRAC_NB_BITS                         (16)
#define LP_COUNT_INT_NB_BITS                    (19)
#define LP_COUNT_NB_BITS                        (LP_COUNT_INT_NB_BITS + LP_FRAC_NB_BITS)
#define LP_RATE_INT_NB_BITS                     (12)
#define LP_RATE_NB_BITS                         (LP_RATE_INT_NB_BITS + LP_FRAC_NB_BITS)
#define SYS_COUNT_NB_BITS                       (29)
#define FRAME_COUNT_NB_BITS                     (10)
#define PU_COUNT_NB_BITS                        (12)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// TCU_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_TCU_BASE               0x01A0F000

typedef volatile struct
{
    REG32                          Ctrl;                         //0x00000000
    REG32                          Wrap_Val;                     //0x00000004
    REG32                          Cur_Val;                      //0x00000008
    REG32                          Latch;                        //0x0000000C
    REG32                          Setup;                        //0x00000010
    REG32                          Disable_Event;                //0x00000014
    REG32                          Enable_Event;                 //0x00000018
    REG32                          Set_TCO;                      //0x0000001C
    REG32                          Clr_TCO;                      //0x00000020
    REG32                          Cfg_Clk_Div;                  //0x00000024
    REG32                          LPS_SF_Ctrl;                  //0x00000028
    REG32                          LPS_SF_Status;                //0x0000002C
    REG32                          LPS_SF_Frames;                //0x00000030
    REG32                          LPS_SF_Restart_Time;          //0x00000034
    REG32                          LPS_SF_Frame_Period;          //0x00000038
    /// The rate is the number of System Clocks per 32kHz Clocks.
    REG32                          LPS_SF_Rate;                  //0x0000003C
    REG32                          LPS_SF_Elapsed_Frames;        //0x00000040
    REG32                          LPS_SF_Sys_Count;             //0x00000044
    REG32                          LPS_PU_Ctrl;                  //0x00000048
    REG32                          LPS_PU_Status;                //0x0000004C
    REG32                          LPS_PU_Times;                 //0x00000050
    REG32                          LPS_PU_IRQ;                   //0x00000054
    REG32                          LPS_PU_IRQ_Mask;              //0x00000058
    REG32                          RX_GAIN_INTERVAL;             //0x0000005C
    REG32 Reserved_00000060[40];                //0x00000060
    REG32                          Event[NB_TCU_PROG_EVENTS];    //0x00000100
} HWP_TCU_T;

#define hwp_tcu                    ((HWP_TCU_T*) KSEG1(REG_TCU_BASE))


//Ctrl
#define TCU_LOAD_VAL(n)            (((n)&0x3FFF)<<0)
#define TCU_ENABLE_DISABLED        (0<<16)
#define TCU_ENABLE_ENABLED         (1<<16)
#define TCU_LOAD                   (1<<20)
#define TCU_NOLATCH_NORMAL         (0<<28)
#define TCU_NOLATCH_FORCE_ONLY     (1<<28)

//Wrap_Val
#define TCU_WRAP_VAL(n)            (((n)&0x3FFF)<<0)

//Cur_Val
#define TCU_CUR_VAL(n)             (((n)&0x3FFF)<<0)

//Latch
#define TCU_FORCELATCH_AREA_0      (1<<0)
#define TCU_FORCELATCH_AREA_1      (1<<1)
#define TCU_FORCELATCH_AREA_2      (1<<2)
#define TCU_FORCELATCH_AREA_3      (1<<3)
#define TCU_FORCELATCH_AREA_4      (1<<4)
#define TCU_FORCELATCH_AREA_5      (1<<5)
#define TCU_FORCELATCH_AREA_6      (1<<6)
#define TCU_FORCELATCH_AREA_7      (1<<7)
#define TCU_FORCE_NOEVENT          (1<<16)
#define TCU_CLEARPROGAREA          (1<<31)
#define TCU_FORCELATCH(n)          (((n)&0xFF)<<0)
#define TCU_FORCELATCH_MASK        (0xFF<<0)
#define TCU_FORCELATCH_SHIFT       (0)

//Setup
#define TCU_POL_TCO_0_ACTIVE_HIGH  (0<<0)
#define TCU_POL_TCO_0_ACTIVE_LOW   (1<<0)
#define TCU_POL_TCO_1_ACTIVE_HIGH  (0<<1)
#define TCU_POL_TCO_1_ACTIVE_LOW   (1<<1)
#define TCU_POL_TCO_2_ACTIVE_HIGH  (0<<2)
#define TCU_POL_TCO_2_ACTIVE_LOW   (1<<2)
#define TCU_POL_TCO_3_ACTIVE_HIGH  (0<<3)
#define TCU_POL_TCO_3_ACTIVE_LOW   (1<<3)
#define TCU_POL_TCO_4_ACTIVE_HIGH  (0<<4)
#define TCU_POL_TCO_4_ACTIVE_LOW   (1<<4)
#define TCU_POL_TCO_5_ACTIVE_HIGH  (0<<5)
#define TCU_POL_TCO_5_ACTIVE_LOW   (1<<5)
#define TCU_POL_TCO_6_ACTIVE_HIGH  (0<<6)
#define TCU_POL_TCO_6_ACTIVE_LOW   (1<<6)
#define TCU_POL_TCO_7_ACTIVE_HIGH  (0<<7)
#define TCU_POL_TCO_7_ACTIVE_LOW   (1<<7)
#define TCU_POL_TCO_8_ACTIVE_HIGH  (0<<8)
#define TCU_POL_TCO_8_ACTIVE_LOW   (1<<8)
#define TCU_POL_TCO_9_ACTIVE_HIGH  (0<<9)
#define TCU_POL_TCO_9_ACTIVE_LOW   (1<<9)
#define TCU_POL_TCO_10_ACTIVE_HIGH (0<<10)
#define TCU_POL_TCO_10_ACTIVE_LOW  (1<<10)
#define TCU_POL_TCO_11_ACTIVE_HIGH (0<<11)
#define TCU_POL_TCO_11_ACTIVE_LOW  (1<<11)
#define TCU_WRITE_ERROR            (1<<28)
#define TCU_DEBUG_ACTIVE_NORMAL    (0<<31)
#define TCU_DEBUG_ACTIVE_DEBUG     (1<<31)
#define TCU_TCO_POLARITY(n)        (((n)&0xFFF)<<0)
#define TCU_TCO_POLARITY_MASK      (0xFFF<<0)
#define TCU_TCO_POLARITY_SHIFT     (0)

//Disable_Event
#define TCU_DISABLE_TCO_0          (1<<0)
#define TCU_DISABLE_TCO_1          (1<<1)
#define TCU_DISABLE_TCO_2          (1<<2)
#define TCU_DISABLE_TCO_3          (1<<3)
#define TCU_DISABLE_TCO_4          (1<<4)
#define TCU_DISABLE_TCO_5          (1<<5)
#define TCU_DISABLE_TCO_6          (1<<6)
#define TCU_DISABLE_TCO_7          (1<<7)
#define TCU_DISABLE_TCO_8          (1<<8)
#define TCU_DISABLE_TCO_9          (1<<9)
#define TCU_DISABLE_TCO_GMSK_ON    (1<<10)
#define TCU_DISABLE_TCO_TX_OEN     (1<<11)
#define TCU_DISABLE_TCO_TX_ON      (1<<12)
#define TCU_DISABLE_TCO_RX_ON      (1<<13)
#define TCU_DISABLE_TCO_RX_DEC_ON  (1<<14)
#define TCU_DISABLE_SEND_SPI_CMD   (1<<15)
#define TCU_DISABLE_NEXT_GAIN      (1<<16)
#define TCU_DISABLE_FIRST_GAIN     (1<<17)
#define TCU_DISABLE_NEXT_FC        (1<<18)
#define TCU_DISABLE_RAMP_0         (1<<19)
#define TCU_DISABLE_RAMP_1         (1<<20)
#define TCU_DISABLE_RAMP_2         (1<<21)
#define TCU_DISABLE_RAMP_3         (1<<22)
#define TCU_DISABLE_RAMP_4         (1<<23)
#define TCU_DISABLE_RX_SOC         (1<<24)
#define TCU_DISABLE_DIGRF_STROBE   (1<<25)
#define TCU_DISABLE_BCPU_IRQ_0     (1<<26)
#define TCU_DISABLE_BCPU_IRQ_1     (1<<27)
#define TCU_DISABLE_XCPU_IRQ_0     (1<<28)
#define TCU_DISABLE_XCPU_IRQ_1     (1<<29)
#define TCU_DISABLE_RF_SPI_MARKED_CMD (1<<31)
#define TCU_DISABLE_TCO(n)         (((n)&0x3FF)<<0)
#define TCU_DISABLE_TCO_MASK       (0x3FF<<0)
#define TCU_DISABLE_TCO_SHIFT      (0)
#define TCU_DISABLE_INTERNAL_TCO(n) (((n)&31)<<10)
#define TCU_DISABLE_INTERNAL_TCO_MASK (31<<10)
#define TCU_DISABLE_INTERNAL_TCO_SHIFT (10)
#define TCU_DISABLE_RAMP(n)        (((n)&31)<<19)
#define TCU_DISABLE_RAMP_MASK      (31<<19)
#define TCU_DISABLE_RAMP_SHIFT     (19)
#define TCU_DISABLE_BCPU_IRQ(n)    (((n)&3)<<26)
#define TCU_DISABLE_BCPU_IRQ_MASK  (3<<26)
#define TCU_DISABLE_BCPU_IRQ_SHIFT (26)
#define TCU_DISABLE_XCPU_IRQ(n)    (((n)&3)<<28)
#define TCU_DISABLE_XCPU_IRQ_MASK  (3<<28)
#define TCU_DISABLE_XCPU_IRQ_SHIFT (28)

//Enable_Event
#define TCU_ENABLE_TCO_0           (1<<0)
#define TCU_ENABLE_TCO_1           (1<<1)
#define TCU_ENABLE_TCO_2           (1<<2)
#define TCU_ENABLE_TCO_3           (1<<3)
#define TCU_ENABLE_TCO_4           (1<<4)
#define TCU_ENABLE_TCO_5           (1<<5)
#define TCU_ENABLE_TCO_6           (1<<6)
#define TCU_ENABLE_TCO_7           (1<<7)
#define TCU_ENABLE_TCO_8           (1<<8)
#define TCU_ENABLE_TCO_9           (1<<9)
#define TCU_ENABLE_TCO_GMSK_ON     (1<<10)
#define TCU_ENABLE_TCO_TX_OEN      (1<<11)
#define TCU_ENABLE_TCO_TX_ON       (1<<12)
#define TCU_ENABLE_TCO_RX_ON       (1<<13)
#define TCU_ENABLE_TCO_RX_DEC_ON   (1<<14)
#define TCU_ENABLE_SEND_SPI_CMD    (1<<15)
#define TCU_ENABLE_NEXT_GAIN       (1<<16)
#define TCU_ENABLE_FIRST_GAIN      (1<<17)
#define TCU_ENABLE_NEXT_FC         (1<<18)
#define TCU_ENABLE_RAMP_0          (1<<19)
#define TCU_ENABLE_RAMP_1          (1<<20)
#define TCU_ENABLE_RAMP_2          (1<<21)
#define TCU_ENABLE_RAMP_3          (1<<22)
#define TCU_ENABLE_RAMP_4          (1<<23)
#define TCU_ENABLE_RX_SOC          (1<<24)
#define TCU_ENABLE_DIGRF_STROBE    (1<<25)
#define TCU_ENABLE_BCPU_IRQ_0      (1<<26)
#define TCU_ENABLE_BCPU_IRQ_1      (1<<27)
#define TCU_ENABLE_XCPU_IRQ_0      (1<<28)
#define TCU_ENABLE_XCPU_IRQ_1      (1<<29)
#define TCU_ENABLE_RF_SPI_MARKED_CMD (1<<31)
#define TCU_ENABLE_TCO(n)          (((n)&0x3FF)<<0)
#define TCU_ENABLE_TCO_MASK        (0x3FF<<0)
#define TCU_ENABLE_TCO_SHIFT       (0)
#define TCU_ENABLE_INTERNAL_TCO(n) (((n)&31)<<10)
#define TCU_ENABLE_INTERNAL_TCO_MASK (31<<10)
#define TCU_ENABLE_INTERNAL_TCO_SHIFT (10)
#define TCU_ENABLE_RAMP(n)         (((n)&31)<<19)
#define TCU_ENABLE_RAMP_MASK       (31<<19)
#define TCU_ENABLE_RAMP_SHIFT      (19)
#define TCU_ENABLE_BCPU_IRQ(n)     (((n)&3)<<26)
#define TCU_ENABLE_BCPU_IRQ_MASK   (3<<26)
#define TCU_ENABLE_BCPU_IRQ_SHIFT  (26)
#define TCU_ENABLE_XCPU_IRQ(n)     (((n)&3)<<28)
#define TCU_ENABLE_XCPU_IRQ_MASK   (3<<28)
#define TCU_ENABLE_XCPU_IRQ_SHIFT  (28)

//Set_TCO
#define TCU_SET_TCO_0              (1<<0)
#define TCU_SET_TCO_1              (1<<1)
#define TCU_SET_TCO_2              (1<<2)
#define TCU_SET_TCO_3              (1<<3)
#define TCU_SET_TCO_4              (1<<4)
#define TCU_SET_TCO_5              (1<<5)
#define TCU_SET_TCO_6              (1<<6)
#define TCU_SET_TCO_7              (1<<7)
#define TCU_SET_TCO_8              (1<<8)
#define TCU_SET_TCO_9              (1<<9)
#define TCU_SET_TCO_10             (1<<10)
#define TCU_SET_TCO_11             (1<<11)
#define TCU_SET_TCO(n)             (((n)&0xFFF)<<0)
#define TCU_SET_TCO_MASK           (0xFFF<<0)
#define TCU_SET_TCO_SHIFT          (0)

//Clr_TCO
#define TCU_CLR_TCO_0              (1<<0)
#define TCU_CLR_TCO_1              (1<<1)
#define TCU_CLR_TCO_2              (1<<2)
#define TCU_CLR_TCO_3              (1<<3)
#define TCU_CLR_TCO_4              (1<<4)
#define TCU_CLR_TCO_5              (1<<5)
#define TCU_CLR_TCO_6              (1<<6)
#define TCU_CLR_TCO_7              (1<<7)
#define TCU_CLR_TCO_8              (1<<8)
#define TCU_CLR_TCO_9              (1<<9)
#define TCU_CLR_TCO_10             (1<<10)
#define TCU_CLR_TCO_11             (1<<11)
#define TCU_CLR_TCO(n)             (((n)&0xFFF)<<0)
#define TCU_CLR_TCO_MASK           (0xFFF<<0)
#define TCU_CLR_TCO_SHIFT          (0)

//Cfg_Clk_Div
#define TCU_TCU_CLK_SAME_SYS_DISABLED (0<<29)
#define TCU_TCU_CLK_SAME_SYS_ENABLED (1<<29)
#define TCU_ENABLE_DAI_SIMPLE_208K_DISABLED (0<<30)
#define TCU_ENABLE_DAI_SIMPLE_208K_ENABLED (1<<30)
#define TCU_ENABLE_QBIT_DISABLED   (0<<31)
#define TCU_ENABLE_QBIT_ENABLED    (1<<31)

//LPS_SF_Ctrl
#define TCU_LPS_SF_ENABLE          (1<<0)
#define TCU_LPS_SF_LOWPOWER(n)     (((n)&3)<<4)
#define TCU_LPS_SF_LOWPOWER_MASK   (3<<4)
#define TCU_LPS_SF_LOWPOWER_SHIFT  (4)
#define TCU_LPS_SF_LOWPOWER_STOP   (0<<4)
#define TCU_LPS_SF_LOWPOWER_SKIPFRAME (1<<4)
#define TCU_LPS_SF_LOWPOWER_CALIB  (3<<4)
#define TCU_LPS_SF_WAKEUP0_DISABLED (0<<8)
#define TCU_LPS_SF_WAKEUP0_ENABLED (1<<8)
#define TCU_LPS_SF_WAKEUP0_CFG_0   (0<<9)
#define TCU_LPS_SF_WAKEUP0_CFG_1   (1<<9)

//LPS_SF_Status
#define TCU_LPS_SF_READY           (1<<0)
#define TCU_LPS_SF_SLOWRUNNING     (1<<4)
#define TCU_LPS_SF_CALIBRATIONDONE (1<<8)
#define TCU_LPS_SF_PU_REACHED      (1<<31)

//LPS_SF_Frames
#define TCU_LPS_SF_FRAME(n)        (((n)&0x3FF)<<0)
#define TCU_LPS_SF_PU_FRAME(n)     (((n)&0x3FF)<<16)

//LPS_SF_Restart_Time
#define TCU_LPS_SF_RESTART_TIME(n) (((n)&0x7FFFF)<<0)

//LPS_SF_Frame_Period
#define TCU_LPS_SF_FRAME_PERIOD(n) (((n)&0x7FFFF)<<0)

//LPS_SF_Rate
#define TCU_LPS_SF_RATE_INT(n)     (((n)&0xFFF)<<16)
#define TCU_LPS_SF_RATE_FRAC(n)    (((n)&0xFFFF)<<0)

//LPS_SF_Elapsed_Frames
#define TCU_LPS_SF_ELAPSED_FRAMES(n) (((n)&0x3FF)<<0)

//LPS_SF_Sys_Count
#define TCU_LPS_SF_SYS_COUNT(n)    (((n)&0x1FFFFFFF)<<0)

//LPS_PU_Ctrl
#define TCU_LPS_PU_ENABLE          (1<<0)
#define TCU_LPS_PU_ENABLE_ENABLE   (1<<0)
#define TCU_LPS_PU_ENABLE_DISABLE  (0<<0)
#define TCU_LPS_PU_FORCESTART      (1<<4)
#define TCU_LPS_PU_FORCESTART_ENABLE (1<<4)
#define TCU_LPS_PU_FORCESTART_DISABLE (0<<4)
#define TCU_LPS_PU_CO0_ON          (1<<8)
#define TCU_LPS_PU_CO1_ON          (1<<9)
#define TCU_LPS_PU_CLK_RF_ON       (1<<10)
#define TCU_LPS_PU_SPI_ON          (1<<11)
#define TCU_LPS_PU_CO0_POL         (1<<12)
#define TCU_LPS_PU_CO1_POL         (1<<13)
#define TCU_LPS_PU_CLK_RF_POL      (1<<14)
#define TCU_LPS_PU_SPI_STEP_CO_S1  (0<<16)
#define TCU_LPS_PU_SPI_STEP_RFCLKREADY (1<<16)
#define TCU_LPS_PU_SPI_STEP_READY  (2<<16)
#define TCU_LPS_PU_SPI_STEP_DONE   (3<<16)
#define TCU_LPS_PU_MASK_CO0_S1_ENABLE (1<<20)
#define TCU_LPS_PU_MASK_CO0_S1_IGNORE (0<<20)
#define TCU_LPS_PU_MASK_CO1_S1_ENABLE (1<<21)
#define TCU_LPS_PU_MASK_CO1_S1_IGNORE (0<<21)
#define TCU_LPS_PU_MASK_CO0_S2_ENABLE (1<<22)
#define TCU_LPS_PU_MASK_CO0_S2_IGNORE (0<<22)
#define TCU_LPS_PU_MASK_CO1_S2_ENABLE (1<<23)
#define TCU_LPS_PU_MASK_CO1_S2_IGNORE (0<<23)
#define TCU_LPS_PU_MASK_CLK_RF_ENABLE (1<<24)
#define TCU_LPS_PU_MASK_CLK_RF_IGNORE (0<<24)
#define TCU_LPS_PU_MASK_ST_PLL_ENABLE (1<<25)
#define TCU_LPS_PU_MASK_ST_PLL_IGNORE (0<<25)
#define TCU_LPS_PU_MASK_SW_RF_SWITCH (1<<26)
#define TCU_LPS_PU_MASK_SW_RF_IGNORE (0<<26)
#define TCU_LPS_PU_MASK_SW_FAST_SWITCH (1<<27)
#define TCU_LPS_PU_MASK_SW_FAST_IGNORE (0<<27)
#define TCU_LPS_PU_CPU_DONE        (1<<28)
#define TCU_LPS_PU_RUN_TCU_WAKEUP  (1<<29)
#define TCU_LPS_PU_CO_ON(n)        (((n)&3)<<8)
#define TCU_LPS_PU_CO_ON_MASK      (3<<8)
#define TCU_LPS_PU_CO_ON_SHIFT     (8)
#define TCU_LPS_PU_CO_POL(n)       (((n)&3)<<12)
#define TCU_LPS_PU_CO_POL_MASK     (3<<12)
#define TCU_LPS_PU_CO_POL_SHIFT    (12)
#define TCU_LPS_PU_MASK_CO_S1(n)   (((n)&3)<<20)
#define TCU_LPS_PU_MASK_CO_S1_MASK (3<<20)
#define TCU_LPS_PU_MASK_CO_S1_SHIFT (20)
#define TCU_LPS_PU_MASK_CO_S2(n)   (((n)&3)<<22)
#define TCU_LPS_PU_MASK_CO_S2_MASK (3<<22)
#define TCU_LPS_PU_MASK_CO_S2_SHIFT (22)

//LPS_PU_Status
#define TCU_LPS_PU_DONE            (1<<0)
#define TCU_LPS_PU_READY           (1<<4)

//LPS_PU_Times
#define TCU_LPS_PU_TCO_S2(n)       (((n)&31)<<0)
#define TCU_LPS_PU_TCLK_RF_EN(n)   (((n)&31)<<6)
#define TCU_LPS_PU_TCLK_RF(n)      (((n)&0xFFF)<<12)
#define TCU_LPS_PU_TCLK_PLL(n)     (((n)&31)<<24)
#define TCU_LPS_PU_PLL_LOCK_BYPASS (0<<31)
#define TCU_LPS_PU_PLL_LOCK_CHECK  (1<<31)

//LPS_PU_IRQ
#define TCU_LPS_IRQ_CALIBRATION_DONE_CAUSE (1<<0)
#define TCU_LPS_IRQ_SLOW_RUNNING_CAUSE (1<<1)
#define TCU_LPS_IRQ_PU_DONE_CAUSE  (1<<2)
#define TCU_LPS_IRQ_PU_READY_CAUSE (1<<3)
#define TCU_LPS_IRQ_PU_REACHED_CAUSE (1<<4)
#define TCU_LPS_IRQ_CALIBRATION_DONE_STATUS (1<<16)
#define TCU_LPS_IRQ_SLOW_RUNNING_STATUS (1<<17)
#define TCU_LPS_IRQ_PU_DONE_STATUS (1<<18)
#define TCU_LPS_IRQ_PU_READY_STATUS (1<<19)
#define TCU_LPS_IRQ_PU_REACHED_STATUS (1<<20)
#define TCU_LPS_PU_IRQ_CAUSE(n)    (((n)&31)<<0)
#define TCU_LPS_PU_IRQ_CAUSE_MASK  (31<<0)
#define TCU_LPS_PU_IRQ_CAUSE_SHIFT (0)
#define TCU_LPS_PU_IRQ_STATUS(n)   (((n)&31)<<16)
#define TCU_LPS_PU_IRQ_STATUS_MASK (31<<16)
#define TCU_LPS_PU_IRQ_STATUS_SHIFT (16)

//LPS_PU_IRQ_Mask
#define TCU_LPS_IRQ_CALIBRATION_DONE_MASK (1<<0)
#define TCU_LPS_IRQ_SLOW_RUNNING_MASK (1<<1)
#define TCU_LPS_IRQ_PU_DONE_MASK   (1<<2)
#define TCU_LPS_IRQ_PU_READY_MASK  (1<<3)
#define TCU_LPS_IRQ_PU_REACHED_MASK (1<<4)
#define TCU_LPS_PU_IRQ_MASK(n)     (((n)&31)<<0)
#define TCU_LPS_PU_IRQ_MASK_MASK   (31<<0)
#define TCU_LPS_PU_IRQ_MASK_SHIFT  (0)

//RX_GAIN_INTERVAL
#define TCU_RX_GAIN_INTERVAL(n)    (((n)&0x3FF)<<0)
#define TCU_RX_GAIN_OFFSET(n)      (((n)&0x7F)<<10)
#define TCU_SW_OFF_TIME(n)         (((n)&7)<<17)
#define TCU_RX_GAIN_DELAY(n)       (((n)&0x7F)<<20)

//Event
#define TCU_EVENT_TIME(n)          (((n)&0x3FFF)<<0)
#define TCU_EVENT_ID(n)            (((n)&0x3F)<<16)
#define TCU_EVENT_ID_CLR_TCO_0     (0x0<<16)
#define TCU_EVENT_ID_SET_TCO_0     (0x1<<16)
#define TCU_EVENT_ID_CLR_TCO_1     (0x2<<16)
#define TCU_EVENT_ID_STOP_GMSK     (0xC<<16)
#define TCU_EVENT_ID_START_GMSK    (0xD<<16)
#define TCU_EVENT_ID_HIGHZ_IQ_DAC  (0xE<<16)
#define TCU_EVENT_ID_DRIVE_IQ_DAC  (0xF<<16)
#define TCU_EVENT_ID_DISABLE_IQ_DAC (0x10<<16)
#define TCU_EVENT_ID_ENABLE_IQ_DAC (0x11<<16)
#define TCU_EVENT_ID_DISABLE_IQ_ADC (0x12<<16)
#define TCU_EVENT_ID_ENABLE_IQ_ADC (0x13<<16)
#define TCU_EVENT_ID_STOP_RFIN_RECORD (0x14<<16)
#define TCU_EVENT_ID_START_RFIN_RECORD (0x15<<16)
#define TCU_EVENT_ID_CLR_PDN       (0x16<<16)
#define TCU_EVENT_ID_SET_PDN       (0x17<<16)
#define TCU_EVENT_ID_SEND_SPI_CMD  (0x18<<16)
#define TCU_EVENT_ID_NEXT_GAIN     (0x19<<16)
#define TCU_EVENT_ID_FIRST_GAIN    (0x1A<<16)
#define TCU_EVENT_ID_NEXT_FC       (0x1B<<16)
#define TCU_EVENT_ID_PA_RAMP0      (0x1C<<16)
#define TCU_EVENT_ID_PA_RAMP1      (0x1D<<16)
#define TCU_EVENT_ID_PA_RAMP2      (0x1E<<16)
#define TCU_EVENT_ID_PA_RAMP3      (0x1F<<16)
#define TCU_EVENT_ID_PA_RAMP4      (0x20<<16)
#define TCU_EVENT_ID_RX_SOC        (0x21<<16)
#define TCU_EVENT_ID_DIGRF_STB     (0x22<<16)
#define TCU_EVENT_ID_BCPU_TCU_IRQ0 (0x23<<16)
#define TCU_EVENT_ID_BCPU_TCU_IRQ1 (0x24<<16)
#define TCU_EVENT_ID_XCPU_TCU_IRQ0 (0x25<<16)
#define TCU_EVENT_ID_XCPU_TCU_IRQ1 (0x26<<16)
#define TCU_EVENT_ID_WAKEUP_DONE   (0x27<<16)
#define TCU_EVENT_ID_NO_EVENT      (0x3F<<16)





#endif


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


#ifndef _FMD_H_
#define _FMD_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'fmd'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// FMD_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_FMD_BASE               0xA2000640

typedef volatile struct
{
    REG32                          REG_00;                       //0x00000000
    REG32                          REG_01;                       //0x00000004
    REG32                          REG_02;                       //0x00000008
    REG32                          REG_03;                       //0x0000000C
    REG32                          REG_04;                       //0x00000010
    REG32                          REG_05;                       //0x00000014
    REG32                          REG_06;                       //0x00000018
    REG32                          REG_07;                       //0x0000001C
    REG32                          REG_08;                       //0x00000020
    REG32                          REG_09;                       //0x00000024
    REG32                          REG_0a__Read_Only_;           //0x00000028
    REG32                          REG_0b__Read_Only_;           //0x0000002C
    REG32                          REG_0c__Read_Only_;           //0x00000030
    REG32                          REG_0d__Read_Only_;           //0x00000034
    REG32                          REG_0e__Read_Only_;           //0x00000038
    REG32                          REG_0f__Read_Only_;           //0x0000003C
    REG32                          REG_10__Read_Only_;           //0x00000040
    REG32                          REG_11;                       //0x00000044
    REG32                          REG_12;                       //0x00000048
    REG32                          REG_13;                       //0x0000004C
    REG32                          REG_14;                       //0x00000050
    REG32                          REG_15;                       //0x00000054
    REG32                          REG_16;                       //0x00000058
    REG32                          REG_17;                       //0x0000005C
    REG32                          REG_18;                       //0x00000060
    REG32                          REG_19;                       //0x00000064
    REG32                          REG_1a;                       //0x00000068
    REG32                          REG_1b;                       //0x0000006C
    REG32                          REG_1c;                       //0x00000070
    REG32                          REG_1d;                       //0x00000074
    REG32                          REG_1e;                       //0x00000078
    REG32                          REG_1f;                       //0x0000007C
    REG32                          REG_20;                       //0x00000080
    REG32                          REG_21;                       //0x00000084
    REG32                          REG_22;                       //0x00000088
    REG32                          REG_23;                       //0x0000008C
    REG32                          REG_24;                       //0x00000090
    REG32                          REG_25;                       //0x00000094
    REG32                          REG_26;                       //0x00000098
    REG32                          REG_27;                       //0x0000009C
    REG32                          REG_28;                       //0x000000A0
    REG32                          REG_29;                       //0x000000A4
    REG32                          REG_2a;                       //0x000000A8
    REG32                          REG_2b;                       //0x000000AC
    REG32                          REG_2c;                       //0x000000B0
    REG32                          REG_2d;                       //0x000000B4
    REG32                          REG_2e;                       //0x000000B8
    REG32                          REG_2f;                       //0x000000BC
    REG32                          REG_30;                       //0x000000C0
    REG32                          REG_31;                       //0x000000C4
    REG32                          REG_32;                       //0x000000C8
    REG32                          REG_33;                       //0x000000CC
    REG32                          REG_34;                       //0x000000D0
    REG32                          REG_35;                       //0x000000D4
    REG32                          REG_36;                       //0x000000D8
    REG32                          REG_37;                       //0x000000DC
    REG32                          REG_38;                       //0x000000E0
    REG32                          REG_39;                       //0x000000E4
    REG32                          REG_3a;                       //0x000000E8
    REG32                          REG_3b;                       //0x000000EC
    REG32                          REG_3c;                       //0x000000F0
    REG32                          REG_3d;                       //0x000000F4
    REG32                          REG_3e;                       //0x000000F8
    REG32                          REG_3f;                       //0x000000FC
    REG32                          REG_40;                       //0x00000100
    REG32                          REG_41;                       //0x00000104
    REG32                          REG_42;                       //0x00000108
    REG32                          REG_43;                       //0x0000010C
    REG32                          REG_44;                       //0x00000110
    REG32                          REG_45;                       //0x00000114
    REG32                          REG_46;                       //0x00000118
    REG32                          REG_47;                       //0x0000011C
    REG32                          REG_48;                       //0x00000120
    REG32                          REG_49;                       //0x00000124
    REG32                          REG_4a__Read_Only_;           //0x00000128
    REG32                          REG_4b__Read_Only_;           //0x0000012C
    REG32                          REG_4c__Read_Only_;           //0x00000130
    REG32                          REG_4d__Read_Only_;           //0x00000134
    REG32                          REG_4e__Read_Only_;           //0x00000138
    REG32                          REG_4f;                       //0x0000013C
    REG32                          REG_50;                       //0x00000140
    REG32                          REG_51;                       //0x00000144
    REG32                          REG_52;                       //0x00000148
    REG32                          REG_53;                       //0x0000014C
    REG32                          REG_54;                       //0x00000150
    REG32                          REG_55;                       //0x00000154
    REG32                          REG_56;                       //0x00000158
    REG32                          REG_57;                       //0x0000015C
    REG32                          REG_58;                       //0x00000160
    REG32                          REG_59;                       //0x00000164
    REG32                          REG_5a;                       //0x00000168
    REG32                          REG_5b;                       //0x0000016C
    REG32                          REG_5c;                       //0x00000170
    REG32                          REG_5d;                       //0x00000174
    REG32                          REG_5e;                       //0x00000178
    REG32                          REG_5f;                       //0x0000017C
    REG32                          REG_60;                       //0x00000180
    REG32                          REG_55;                       //0x00000184
    REG32                          REG_62;                       //0x00000188
} HWP_FMD_T;

#define hwp_fmd                    ((HWP_FMD_T*) KSEG1(REG_FMD_BASE))


//REG_00
#define FMD_WRITE_REQ              (1<<31)
#define FMD_READ_REQ               (1<<30)
#define FMD_CHIPID_15_0_(n)        (((n)&0xFFFF)<<0)

//REG_01
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_REVID_5_0_(n)          (((n)&0x3F)<<10)
//#define FMD_RESERVED(n)          (((n)&3)<<8)
#define FMD_FIRMWARE_7_0_(n)       (((n)&0xFF)<<0)

//REG_02
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_DHIZ                   (1<<15)
#define FMD_DMUTE                  (1<<14)
#define FMD_MONO                   (1<<13)
#define FMD_BASS                   (1<<12)
#define FMD_RFPLL_CNT_EN_B         (1<<11)
#define FMD_CLK32K_INPUT_EN_B      (1<<10)
#define FMD_SEEKUP                 (1<<9)
#define FMD_SEEK                   (1<<8)
#define FMD_SKMODE                 (1<<7)
#define FMD_CLK_MODE_2_0_(n)       (((n)&7)<<4)
#define FMD_RDS_EN                 (1<<3)
#define FMD_RSVD                   (1<<2)
#define FMD_SOFT_RESET             (1<<1)
#define FMD_ENABLE                 (1<<0)

//REG_03
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_CHAN_REG_9_0_(n)       (((n)&0x3FF)<<6)
#define FMD_DIRECT_REG             (1<<5)
#define FMD_TUNE                   (1<<4)
#define FMD_BAND_1_0_(n)           (((n)&3)<<2)
#define FMD_SPACE_1_0_(n)          (((n)&3)<<0)

//REG_04
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDSIEN                 (1<<15)
#define FMD_STCIEN                 (1<<14)
#define FMD_RBDS                   (1<<13)
#define FMD_RDS_FIFO_EN            (1<<12)
#define FMD_DE                     (1<<11)
#define FMD_RDS_FIFO_CLR           (1<<10)
#define FMD_SOFTMUTE_EN            (1<<9)
#define FMD_AFCD                   (1<<8)
#define FMD_RSVD_X1                (1<<7)
#define FMD_I2S_ENABLE             (1<<6)
#define FMD_GPIO3_1_0_(n)          (((n)&3)<<4)
#define FMD_GPIO2_1_0_(n)          (((n)&3)<<2)
#define FMD_GPIO1_1_0_(n)          (((n)&3)<<0)

//REG_05
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_INT_MODE               (1<<15)
#define FMD_SEEK_MODE_1_0(n)       (((n)&3)<<13)
//#define FMD_RESERVED             (1<<12)
#define FMD_SEEK_TH_53_0_(n)       (((n)&15)<<8)
#define FMD_LNA_PORT_SEL_1_0_(n)   (((n)&3)<<6)
#define FMD_LNA_ICSEL_BIT_1_0_(n)  (((n)&3)<<4)
#define FMD_DAC_GAIN_BIT_3_0_(n)   (((n)&15)<<0)

//REG_06
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSVD_X2                (1<<15)
#define FMD_PAGE                   (1<<14)
#define FMD_RSVD_X3                (1<<13)
#define FMD_SLAVE_MASTER           (1<<12)
#define FMD_WS_LR                  (1<<11)
#define FMD_SCLK_I_EDGE            (1<<10)
#define FMD_DATA_SIGNED            (1<<9)
#define FMD_WS_I_EDGE              (1<<8)
#define FMD_WS_MODE_3_0_(n)        (((n)&15)<<4)
#define FMD_WS_O_EDGE              (1<<3)
#define FMD_SCLK_O_EDGE            (1<<2)
#define FMD_L_DELAY                (1<<1)
#define FMD_R_DELAY                (1<<0)

//REG_07
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RESERVED             (1<<15)
#define FMD_TH_SOFTBLEND_5_0_(n)   (((n)&31)<<10)
#define FMD_BAND_2_                (1<<9)
//#define FMD_RESERVED             (1<<8)
#define FMD_RSVDSEEKTH(n)          (((n)&0x3F)<<2)
#define FMD_SOFTBLEND_EN           (1<<1)
#define FMD_FREQ_MODE              (1<<0)

//REG_08
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_FREQ_DIRECT_15_0_(n)   (((n)&0xFFFF)<<0)

//REG_09
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSVD_X4(n)             (((n)&0x3F)<<10)
#define FMD_PD_CRYOSC              (1<<9)
#define FMD_PD_ANA                 (1<<8)
//#define FMD_RSVD_X1              (1<<7)
#define FMD_PD_RFPLL               (1<<6)
#define FMD_PD_LNA                 (1<<5)
#define FMD_PD_MIXER               (1<<4)
#define FMD_PD_PGA                 (1<<3)
#define FMD_PD_ADC                 (1<<2)
#define FMD_PD_DIG                 (1<<1)
#define FMD_PD_DAC                 (1<<0)

//REG_0a__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDSR                   (1<<15)
#define FMD_STC                    (1<<14)
#define FMD_SF                     (1<<13)
#define FMD_RDSS                   (1<<12)
#define FMD_BLK_E                  (1<<11)
#define FMD_ST                     (1<<10)
#define FMD_READCHAN_9_0_(n)       (((n)&0x3FF)<<0)

//REG_0b__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSSI_6_0_(n)           (((n)&0x7F)<<9)
#define FMD_FM_TRUE                (1<<8)
#define FMD_FM_READY               (1<<7)
#define FMD_RSVD_X5(n)             (((n)&3)<<5)
#define FMD_ABCD_E                 (1<<4)
#define FMD_BLERA_1_0_(n)          (((n)&3)<<2)
#define FMD_BLERB_1_0_(n)          (((n)&3)<<0)

//REG_0c__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDSA_15_0_(n)          (((n)&0xFFFF)<<0)

//REG_0d__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDSB_15_0_(n)          (((n)&0xFFFF)<<0)

//REG_0e__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDSC_15_0_(n)          (((n)&0xFFFF)<<0)

//REG_0f__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDSD_15_0_(n)          (((n)&0xFFFF)<<0)

//REG_10__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_BLERC_1_0_(n)          (((n)&3)<<14)
#define FMD_BLERD_1_0_(n)          (((n)&3)<<12)
#define FMD_SNR_RDS_5_0_(n)        (((n)&0x3F)<<6)
#define FMD_RDS_FIFO_FULL_FLAG     (1<<5)
#define FMD_RDS_FIFO_EMPTY_FLAG    (1<<4)
#define FMD_RDS_FLAG               (1<<3)
#define FMD_GPIO3_I                (1<<2)
#define FMD_GPIO2_I                (1<<1)
#define FMD_GPIO1_I                (1<<0)

//REG_11
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_SNR_OUT_5_0_(n)        (((n)&0x3F)<<10)
#define FMD_SK_CMP_GRP_FLAG_4_0_(n) (((n)&31)<<5)
#define FMD_SK_CMP_GRP_EN_4_0_(n)  (((n)&31)<<0)

//REG_12
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_SIGNAL_DB2_6_0_(n)     (((n)&0x7F)<<8)
#define FMD_OFFSET2_7_0_(n)        (((n)&0xFF)<<0)

//REG_13
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ADC_RESET_EN_DSP       (1<<15)
#define FMD_ADC_DATA_TEST          (1<<14)
#define FMD_ADC_ICTRL_BIT_1_0_(n)  (((n)&3)<<12)
#define FMD_ADC_RESET_DR           (1<<11)
#define FMD_ADC_RESET_REG          (1<<10)
#define FMD_ADC_IQ_SWAP            (1<<9)
#define FMD_ADC_TEST_ENABLE        (1<<8)
#define FMD_ADC_DIG_PWR_DR         (1<<7)
#define FMD_ADC_DIG_PWR_REG_2_0_(n) (((n)&7)<<4)
#define FMD_ADC_CAL_REFI_BIT_DR    (1<<3)
#define FMD_ADC_CAL_REFI_BIT_REG_2_0_(n) (((n)&7)<<0)

//REG_14
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_PLL_FBC_SEL_2_0_(n)    (((n)&7)<<13)
#define FMD_PGA_ICTRL_3_0_(n)      (((n)&15)<<9)
#define FMD_PGA_TEST_EN            (1<<8)
#define FMD_PGA_TEST_BIT_1_0_(n)   (((n)&3)<<6)
#define FMD_DAC_COMMON_BIT_1_0_(n) (((n)&3)<<4)
#define FMD_DAC_ISEL_BIT_3_0_(n)   (((n)&15)<<0)

//REG_15
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_PLL_MINI_PD            (1<<15)
#define FMD_RFPLL_FTS_CAP_BIT_2_0_(n) (((n)&7)<<12)
#define FMD_RFPLL_PRESC_VREG_BIT_3_0_(n) (((n)&15)<<8)
#define FMD_RFPLL_DIV_IBIT_3_0_(n) (((n)&15)<<4)
#define FMD_PGA_BW_1_0_(n)         (((n)&3)<<2)
#define FMD_ADC_ISEL_BIT_1_0_(n)   (((n)&3)<<0)

//REG_16
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RFPLL_VCO_IBIT_3_0_(n) (((n)&15)<<12)
#define FMD_RFPLL_VCO_BAND_DR      (1<<11)
#define FMD_RFPLL_VCO_BAND_REG_10_0_(n) (((n)&0x7FF)<<0)

//REG_17
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_PLL_FBC_STATE          (1<<15)
#define FMD_RFPLL_VARDAC_DR        (1<<14)
#define FMD_RFPLL_VAR_SEL_REG_4_0_(n) (((n)&31)<<9)
#define FMD_RFPLL_RDAC_DIN_REG_8_0_(n) (((n)&0x1FF)<<0)

//REG_18
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RFPLL_CAL_FAST_MODE    (1<<15)
#define FMD_RFPLL_CAL_TIME_SEL_5_0_(n) (((n)&0x3F)<<9)
#define FMD_RFPLL_DIV_DIRECT       (1<<8)
#define FMD_RFPLL_DIV6_MODE        (1<<7)
#define FMD_RFPLL_DIV5_MODE        (1<<6)
#define FMD_RFPLL_DIV_CLK_ADC_DR   (1<<5)
#define FMD_RFPLL_DIV_CLK_ADC_REG_4_0_(n) (((n)&31)<<0)

//REG_19
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RFPLL_CAL_ERR_CNT_TH_7_0_(n) (((n)&0xFF)<<8)
#define FMD_RFPLL_CAL_OPT          (1<<7)
#define FMD_RSVD_X6                (1<<6)
#define FMD_RFPLL_CAL_GAIN_SEL_2_0_(n) (((n)&7)<<3)
#define FMD_RFPLL_CAL_RDAC_NUM_2_0_(n) (((n)&7)<<0)

//REG_1a
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_PLL_RESET_REG          (1<<15)
#define FMD_PLL_START_EN           (1<<14)
#define FMD_ADC_CAL_SELECT_1_0_(n) (((n)&3)<<12)
#define FMD_DC_CAL_SELECT_1_0_(n)  (((n)&3)<<10)
#define FMD_CRYSTAL_OFFSET_9_0_(n) (((n)&0x3FF)<<0)

//REG_1b
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_OFFSET_CAL_ONETIME     (1<<15)
#define FMD_OFFSET_CAL_INIT_DELAY_1_0_(n) (((n)&3)<<13)
#define FMD_OFFSET_CAL_BIT_DR      (1<<12)
#define FMD_OFFSET_CAL_BIT_I_REG_5_0_(n) (((n)&0x3F)<<6)
#define FMD_OFFSET_CAL_BIT_Q_REG_5_0_(n) (((n)&0x3F)<<0)

//REG_1c
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSVD_X7(n)             (((n)&3)<<14)
#define FMD_DIG_PWR_BIT_2_0_(n)    (((n)&7)<<11)
#define FMD_DVDD_IO_BIT_1_0_(n)    (((n)&3)<<9)
#define FMD_LDO_VCTRL1_BIT_2_0_(n) (((n)&7)<<6)
#define FMD_LDO_VCTRL2_BIT_2_0_(n) (((n)&7)<<3)
#define FMD_LDO_VCTRL3_BIT_2_0_(n) (((n)&7)<<0)

//REG_1d
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RESET_SDM_DELAY_1_0_(n) (((n)&3)<<14)
#define FMD_SDM_RESETN_DR          (1<<13)
#define FMD_SDM_RESETN_REG         (1<<12)
#define FMD_RESET_DSP_DELAY_1_0_(n) (((n)&3)<<10)
#define FMD_DSP_RESETN_DR          (1<<9)
#define FMD_DSP_RESETN_REG         (1<<8)
#define FMD_DAC_POLARITY           (1<<7)
#define FMD_DAC_PUP_DELAY_1_0_(n)  (((n)&3)<<5)
#define FMD_DAC_PUP_BYPASS         (1<<4)
#define FMD_I2C_DRV_BIT_1_0_(n)    (((n)&3)<<2)
#define FMD_GPIO_DRV_IBIT_1_0_(n)  (((n)&3)<<0)

//REG_1e
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_SK_TIMER1_3_0_(n)      (((n)&15)<<12)
#define FMD_SK_TIMER2_5_0_(n)      (((n)&0x3F)<<6)
#define FMD_SK_TIMER3_5_0_(n)      (((n)&0x3F)<<0)

//REG_1f
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_TH_SOFTBLEND2_5_0_(n)  (((n)&0x3F)<<9)
#define FMD_GAINCT_8_0_(n)         (((n)&0x1FF)<<0)

//REG_20
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_SEEK_MODE_2_           (1<<14)
#define FMD_RESERVED_SEEK_MODE_2_0_(n) (((n)&3)<<12)
#define FMD_SEL_FIRCUT_2_0_(n)     (((n)&7)<<9)
#define FMD_SEL_FIRCUT2_2_0_(n)    (((n)&7)<<6)
#define FMD_FIRCUT_SEL0_2_0(n)     (((n)&7)<<3)
#define FMD_FIRCUT_OFFSET_SEL_BYPASS (1<<2)

//REG_21
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_IMGREJ                 (1<<14)
#define FMD_IMGREJ_DSP             (1<<13)
#define FMD_IQ_SWAP                (1<<12)
#define FMD_FIRCUT_BYPASS          (1<<11)
#define FMD_BYPASS_LPF             (1<<10)
#define FMD_LR_SWAP                (1<<9)
#define FMD_PILOT_COSINE           (1<<8)
#define FMD_AUDIO_DCC_BYPASS       (1<<7)
#define FMD_DEEMPH_BYPASS          (1<<6)
#define FMD_SDM_BYPASS             (1<<5)
#define FMD_TS_MOD_4_0_(n)         (((n)&31)<<0)

//REG_22
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_RSSI_H_TH_6_0_(n)      (((n)&7)<<11)
#define FMD_RSSI_L_TH_6_0_(n)      (((n)&0x7FF)<<0)

//REG_23
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_NOISE_H_TH_6_04_(n)    (((n)&0x7FFFF)<<13)
#define FMD_RESERVEDNOISE_L_TH_6_0_(n) (((n)&0xFFFFFFFF)<<0)

//REG_24
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_PILOT_H_TH_6_0_(n)     (((n)&0x7F)<<7)
#define FMD_PILOT_L_TH_6_0_(n)     (((n)&0x7F)<<0)

//REG_25
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_OFFSET_H_TH_6_0_(n)    (((n)&0x7F)<<7)
#define FMD_OFFSET_L_TH_6_0_(n)    (((n)&0x7F)<<0)

//REG_26
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_SK_TIMER4_5_0_(n)      (((n)&0x3F)<<10)
#define FMD_SNR_TH_5_0_(n)         (((n)&0x3F)<<4)
#define FMD_SNR_CNT_TH_3_0_(n)     (((n)&15)<<0)

//REG_27
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSSI_SELECT_1_0_(n)    (((n)&3)<<14)
#define FMD_ADC_ICTRL_BIT_CAL_1_0_(n) (((n)&3)<<12)
#define FMD_CT_U_RSSI_5_0_(n)      (((n)&0x3F)<<6)
#define FMD_CT_U_NOISE_5_0_(n)     (((n)&0x3F)<<0)

//REG_28
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_AUDIO_DCC_CT_2_0_(n)   (((n)&7)<<11)
#define FMD_BASS_GAIN_1_0_(n)      (((n)&3)<<9)
#define FMD_VOLUME_3_0_(n)         (((n)&15)<<5)
#define FMD_RSSI_FLT_CT_U_4_0_(n)  (((n)&31)<<0)

//REG_29
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSVD_X8(n)             (((n)&15)<<12)
#define FMD_SOFTMUTE_TH2_5_0_(n)   (((n)&0x3F)<<6)
#define FMD_SOFTMUTE_TH3_5_0_(n)   (((n)&0x3F)<<0)

//REG_2a
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_PILOT_CNT_TH_7_0_(n)   (((n)&0xFF)<<8)
#define FMD_RSSI_CNT_TH_7_0_(n)    (((n)&0xFF)<<0)

//REG_2b
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_NOISE_CNT_TH_7_0_(n)   (((n)&0xFF)<<8)
#define FMD_OFFSET_CNT_TH_7_0_(n)  (((n)&0xFF)<<0)

//REG_2c
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X8(n)           (((n)&15)<<12)
#define FMD_CT_U_PILOT_5_0_(n)     (((n)&0x3F)<<6)
#define FMD_RSVD_X9(n)             (((n)&0x3F)<<0)

//REG_2d
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_AFC_SELECT0            (1<<13)
#define FMD_AFC_INV0               (1<<12)
#define FMD_SEEK_AFC_ON            (1<<11)
#define FMD_AFC_CT_5_0_(n)         (((n)&0x3F)<<5)
#define FMD_SEEK_RANGE_4_0_(n)     (((n)&31)<<0)

//REG_2e
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X8(n)           (((n)&15)<<12)
#define FMD_AFC_RANGE_5_0_(n)      (((n)&0x3F)<<6)
#define FMD_AFC_CT_SK_5_0_(n)      (((n)&0x3F)<<0)

//REG_2f
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LIMIT_SEL              (1<<13)
#define FMD_DIG_DELAYCT_4_0_(n)    (((n)&31)<<8)
#define FMD_DC_CT_7_0_(n)          (((n)&0xFF)<<0)

//REG_30
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_SOFTMUTE_TH_5_0_(n)    (((n)&0x3F)<<8)
#define FMD_CT_INTERVAL_SM_1_0_(n) (((n)&3)<<6)
#define FMD_SM_ATTENU_3_0_(n)      (((n)&15)<<2)
#define FMD_SM_RATE_1_0_(n)        (((n)&3)<<0)

//REG_31
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSVD_X10(n)            (((n)&7)<<13)
#define FMD_NCO_CT_U_2_0_(n)       (((n)&7)<<10)
#define FMD_NCO_CT_U_DT_2_0_(n)    (((n)&7)<<7)
#define FMD_NCO_CT_U_DT_SK_2_0_(n) (((n)&7)<<4)
#define FMD_NCO_CT_BW_3_0_(n)      (((n)&15)<<0)

//REG_32
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_OVER_TH_SEL_2_0_(n)    (((n)&7)<<12)
#define FMD_CT_ENDELAY_2_0_(n)     (((n)&7)<<9)
#define FMD_AGC_INDEX_IN_3_0_(n)   (((n)&15)<<5)
#define FMD_AGC_THD_4_0_(n)        (((n)&31)<<0)

//REG_33
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_AGC_CT_U_2_0_(n)       (((n)&7)<<13)
#define FMD_AGC_TEST               (1<<12)
#define FMD_AGC_UPDATE0_2_0_(n)    (((n)&7)<<9)
#define FMD_AGC_UPDATE1_2_0_(n)    (((n)&7)<<6)
#define FMD_AGC_LOOP_GAIN0_2_0_(n) (((n)&7)<<3)
#define FMD_AGC_LOOP_GAIN1_2_0_(n) (((n)&7)<<0)

//REG_34
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X10(n)          (((n)&7)<<13)
#define FMD_DIG_GAIN_IN_4_         (1<<12)
#define FMD_AGC_TARGET_PWR_5_0_(n) (((n)&0x3F)<<6)
#define FMD_AGC_CHG_TH_1_0_(n)     (((n)&3)<<4)
#define FMD_DIG_GAIN_IN_3_0_(n)    (((n)&15)<<0)

//REG_35
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X10(n)          (((n)&7)<<13)
#define FMD_LOG_OVER_SEL           (1<<12)
#define FMD_LOG_AGC_TH_5_0_(n)     (((n)&0x3F)<<6)
#define FMD_OVER_STEP_5_0_(n)      (((n)&0x3F)<<0)

//REG_36
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_AFC_SELECT1            (1<<15)
#define FMD_AFC_INV1               (1<<14)
#define FMD_FREQ_CHANGE_DR         (1<<13)
#define FMD_FREQ_CHANGE_REG        (1<<12)
#define FMD_RFPLL_CLOSE_CAL_GAIN_1_0_(n) (((n)&3)<<10)
#define FMD_DAC_GAIN_BIT_PWRUP(n)  (((n)&15)<<6)
#define FMD_RFPLL_CAL_TIME_SEL_SK_2_0_(n) (((n)&7)<<3)
#define FMD_RFPLL_CAL_GAIN_SEL_SK_2_0_(n) (((n)&7)<<0)

//REG_37
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_TH_MAX_BW_6_0_(n)      (((n)&0x7F)<<7)
#define FMD_TH_MIN_BW_6_0_(n)      (((n)&0x7F)<<0)

//REG_38
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_NCO_CT_BW_3_0__X1(n)   (((n)&15)<<11)
//#define FMD_RESERVED             (1<<10)
#define FMD_LPF15K_BW_DR           (1<<9)
#define FMD_LPF15K_BW_REG_2_0_(n)  (((n)&7)<<6)
#define FMD_TH_MIN_HCC_5_0_(n)     (((n)&0x3F)<<0)

//REG_39
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LNA_GAIN_BIT_F_1_0_(n) (((n)&3)<<12)
#define FMD_PGA_GAIN_BIT_F_2_0_(n) (((n)&7)<<9)
#define FMD_ADC_GAIN_BIT_F_1_0_(n) (((n)&3)<<7)
#define FMD_LNA_GAIN_BIT_E_1_0_(n) (((n)&3)<<5)
#define FMD_PGA_GAIN_BIT_E_2_0_(n) (((n)&7)<<2)
#define FMD_ADC_GAIN_BIT_E_1_0_(n) (((n)&3)<<0)

//REG_3a
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LNA_GAIN_BIT_D_1_0_(n) (((n)&3)<<12)
#define FMD_PGA_GAIN_BIT_D_2_0_(n) (((n)&7)<<9)
#define FMD_ADC_GAIN_BIT_D_1_0_(n) (((n)&3)<<7)
#define FMD_LNA_GAIN_BIT_C_1_0_(n) (((n)&3)<<5)
#define FMD_PGA_GAIN_BIT_C_2_0_(n) (((n)&7)<<2)
#define FMD_ADC_GAIN_BIT_C_1_0_(n) (((n)&3)<<0)

//REG_3b
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LNA_GAIN_BIT_B_1_0_(n) (((n)&3)<<12)
#define FMD_PGA_GAIN_BIT_B_2_0_(n) (((n)&7)<<9)
#define FMD_ADC_GAIN_BIT_B_1_0_(n) (((n)&3)<<7)
#define FMD_LNA_GAIN_BIT_A_1_0_(n) (((n)&3)<<5)
#define FMD_PGA_GAIN_BIT_A_2_0_(n) (((n)&7)<<2)
#define FMD_ADC_GAIN_BIT_A_1_0_(n) (((n)&3)<<0)

//REG_3c
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LNA_GAIN_BIT_9_1_0_(n) (((n)&3)<<12)
#define FMD_PGA_GAIN_BIT_9_2_0_(n) (((n)&7)<<9)
#define FMD_ADC_GAIN_BIT_9_1_0_(n) (((n)&3)<<7)
#define FMD_LNA_GAIN_BIT_8_1_0_(n) (((n)&3)<<5)
#define FMD_PGA_GAIN_BIT_8_2_0_(n) (((n)&7)<<2)
#define FMD_ADC_GAIN_BIT_8_1_0_(n) (((n)&3)<<0)

//REG_3d
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LNA_GAIN_BIT_7_1_0_(n) (((n)&3)<<12)
#define FMD_PGA_GAIN_BIT_7_2_0_(n) (((n)&7)<<9)
#define FMD_ADC_GAIN_BIT_7_1_0_(n) (((n)&3)<<7)
#define FMD_LNA_GAIN_BIT_6_1_0_(n) (((n)&3)<<5)
#define FMD_PGA_GAIN_BIT_6_2_0_(n) (((n)&7)<<2)
#define FMD_ADC_GAIN_BIT_6_1_0_(n) (((n)&3)<<0)

//REG_3e
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LNA_GAIN_BIT_5_1_0_(n) (((n)&3)<<12)
#define FMD_PGA_GAIN_BIT_5_2_0_(n) (((n)&7)<<9)
#define FMD_ADC_GAIN_BIT_5_1_0_(n) (((n)&3)<<7)
#define FMD_LNA_GAIN_BIT_4_1_0_(n) (((n)&3)<<5)
#define FMD_PGA_GAIN_BIT_4_2_0_(n) (((n)&7)<<2)
#define FMD_ADC_GAIN_BIT_4_1_0_(n) (((n)&3)<<0)

//REG_3f
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_LNA_GAIN_BIT_3_1_0_(n) (((n)&3)<<12)
#define FMD_PGA_GAIN_BIT_3_2_0_(n) (((n)&7)<<9)
#define FMD_ADC_GAIN_BIT_3_1_0_(n) (((n)&3)<<7)
#define FMD_LNA_GAIN_BIT_2_1_0_(n) (((n)&3)<<5)
#define FMD_PGA_GAIN_BIT_2_2_0_(n) (((n)&7)<<2)
#define FMD_ADC_GAIN_BIT_2_1_0_(n) (((n)&3)<<0)

//REG_40
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSVD_X11(n)            (((n)&0xFFFF)<<0)

//REG_41
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X11(n)          (((n)&0xFFFF)<<0)

//REG_42
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ANA_GAIN_RSSI_F_7_0_(n) (((n)&0xFF)<<8)
#define FMD_ANA_GAIN_RSSI_E_7_0_(n) (((n)&0xFF)<<0)

//REG_43
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ANA_GAIN_RSSI_D_7_0_(n) (((n)&0xFF)<<8)
#define FMD_ANA_GAIN_RSSI_C_7_0_(n) (((n)&0xFF)<<0)

//REG_44
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ANA_GAIN_RSSI_B_7_0_(n) (((n)&0xFF)<<8)
#define FMD_ANA_GAIN_RSSI_A_7_0_(n) (((n)&0xFF)<<0)

//REG_45
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ANA_GAIN_RSSI_9_7_0_(n) (((n)&0xFF)<<8)
#define FMD_ANA_GAIN_RSSI_8_7_0_(n) (((n)&0xFF)<<0)

//REG_46
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ANA_GAIN_RSSI_7_7_0_(n) (((n)&0xFF)<<8)
#define FMD_ANA_GAIN_RSSI_6_7_0_(n) (((n)&0xFF)<<0)

//REG_47
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ANA_GAIN_RSSI_5_7_0_(n) (((n)&0xFF)<<8)
#define FMD_ANA_GAIN_RSSI_4_7_0_(n) (((n)&0xFF)<<0)

//REG_48
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_ANA_GAIN_RSSI_3_7_0_(n) (((n)&0xFF)<<8)
#define FMD_ANA_GAIN_RSSI_2_7_0_(n) (((n)&0xFF)<<0)

//REG_49
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_INTERVAL_DR            (1<<14)
#define FMD_INTERVAL_REG_13_0_(n)  (((n)&0x3FFF)<<0)

//REG_4a__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSSI_CNT_7_0_(n)       (((n)&0xFF)<<8)
#define FMD_NOISE_CNT_7_0_(n)      (((n)&0xFF)<<0)

//REG_4b__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_PILOT_CNT_7_0_(n)      (((n)&0xFF)<<8)
#define FMD_OFFSET_CNT_7_0_(n)     (((n)&0xFF)<<0)

//REG_4c__Read_Only_
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X10(n)          (((n)&7)<<13)
#define FMD_NOISE_DB2_6_0_(n)      (((n)&0x7F)<<6)
#define FMD_SNR19K_OUT_5_0_(n)     (((n)&0x3F)<<0)

//REG_4d__Read_Only_
//#define FMD_READ_REQ             (1<<30)
#define FMD_CT_HCC_1_0_(n)         (((n)&3)<<14)
#define FMD_FIRCUT_BW_2_0_(n)      (((n)&7)<<11)
#define FMD_SNR_CNT2_3_0_(n)       (((n)&15)<<7)
#define FMD_PILOT_DB2_6_0_(n)      (((n)&0x7F)<<0)

//REG_4e__Read_Only_
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X10(n)          (((n)&7)<<13)
#define FMD_FMDEM_SEL              (1<<12)
#define FMD_DIG_GAINCT_7_4_(n)     (((n)&15)<<8)
#define FMD_INDEX_OUT_3_0_(n)      (((n)&15)<<4)
#define FMD_DIG_GAICT_3_0_(n)      (((n)&15)<<0)

//REG_4f
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RESERVED_DSP_15_0_(n)  (((n)&0xFFFF)<<0)

//REG_50
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_OFFSET_ENABLE          (1<<14)
#define FMD_OFFSET_VALUE(n)        (((n)&0x3FFF)<<0)

//REG_51
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X7(n)           (((n)&3)<<14)
#define FMD_DITHER_BYPASS          (1<<13)
#define FMD_TRI_WAVE_BYPASS        (1<<12)
#define FMD_NPHASE(n)              (((n)&15)<<8)
#define FMD_INT_DEC_SEL_(n)        (((n)&3)<<6)
#define FMD_DEC_NUMERATOR(n)       (((n)&7)<<3)
#define FMD_OFFSET_MODE(n)         (((n)&7)<<0)

//REG_52
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RSVD_X12(n)            (((n)&0xFF)<<8)
#define FMD_RESERVED_SDM(n)        (((n)&0xFF)<<0)

//REG_53
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_GAIN_38K1(n)           (((n)&0x7FFF)<<0)

//REG_54
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_GAIN_38K2(n)           (((n)&0x7FFF)<<0)

//REG_55
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDS_ROLLOFF(n)         (((n)&7)<<13)
#define FMD_RDS_PFD_GAIN(n)        (((n)&7)<<10)
#define FMD_RDS_NCO_CT_U(n)        (((n)&7)<<7)
#define FMD_RDS_NCO_CT_BW(n)       (((n)&0x3F)<<1)
#define FMD_RDS_PLL_1ORD_          (1<<0)

//REG_56
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_RDS_GAIN_CT_U(n)       (((n)&7)<<13)
#define FMD_RDS_HPF_CT_U(n)        (((n)&7)<<10)
#define FMD_RDS_MATCH_CT_U(n)      (((n)&7)<<7)
#define FMD_RDS_GAIN_DR            (1<<6)
#define FMD_RDS_GAIN1_(n)          (((n)&0x3F)<<0)

//REG_57
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_RDS_TEST               (1<<14)
#define FMD_RDS_NOERROR_CM         (1<<13)
#define FMD_RDS_NOSHIFT_CM         (1<<12)
#define FMD_RDS_SYNC_TH1_(n)       (((n)&0x3F)<<6)
#define FMD_RDS_SYNC_TH2_(n)       (((n)&0x3F)<<0)

//REG_58
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_RDS_LPFIL_BYPASS       (1<<14)
#define FMD_RDS_NOTCHFIL_BYPASS    (1<<13)
#define FMD_RDS_HPFIL_BYPASS       (1<<12)
#define FMD_RDS_H_TH_(n)           (((n)&0x3F)<<6)
#define FMD_RDS_L_TH_(n)           (((n)&0x3F)<<0)

//REG_59
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_RDS_CNT_TH(n)          (((n)&0x3F)<<9)
#define FMD_GAIN_TARGET_(n)        (((n)&0x3F)<<3)
#define FMD_GAIN_CHG_TH_(n)        (((n)&7)<<0)

//REG_5a
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_RDS_CNT(n)             (((n)&0xFF)<<7)
#define FMD_RDS_DB2_(n)            (((n)&0x7F)<<0)

//REG_5b
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_PLLDEM_GAIN(n)         (((n)&0x3F)<<10)
#define FMD_PFD_SEL                (1<<8)
#define FMD_RDS_GAIN_RSSI_SEL      (1<<7)
#define FMD_RDS_NCO_DISABLE        (1<<6)
#define FMD__RDS_EN_STRETCH        (1<<5)
#define FMD_RDS_ERR_TH_(n)         (((n)&15)<<1)
#define FMD__BPF_57K_BYPASS        (1<<0)

//REG_5c
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_PLLDEM_SWAP            (1<<15)
#define FMD_FMDEM_SEL_GRP(n)       (((n)&7)<<12)
#define FMD_PLLDEM_TH_MAX(n)       (((n)&0x3F)<<6)
#define FMD_PLLDEM_TH_MIN(n)       (((n)&0x3F)<<0)

//REG_5d
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RESERVED(n)          (((n)&0x3FF)<<6)
#define FMD_RDS_FIRCUT(n)          (((n)&7)<<3)
#define FMD_RDS_FIRCUT2(n)         (((n)&7)<<0)

//REG_5e
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RESERVED(n)          (((n)&0xFFFF)<<0)

//REG_5f
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RESERVED(n)          (((n)&0xFFFF)<<0)

//REG_60
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
#define FMD_MT                     (1<<15)
#define FMD_RSVD_60_14             (1<<14)
#define FMD_PLL_W(n)               (((n)&0x3FFF)<<0)

//REG_55
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X10(n)          (((n)&7)<<13)
#define FMD_HLSI                   (1<<12)
#define FMD_MS                     (1<<11)
#define FMD_RSVD_61_10             (1<<10)
#define FMD_RSVD_61_9              (1<<9)
#define FMD_SWP1                   (1<<8)
#define FMD_SWP2                   (1<<7)
#define FMD_STBY                   (1<<6)
#define FMD_RSVD_61_5              (1<<5)
#define FMD_RSVD_61_4              (1<<4)
#define FMD_SMUTE                  (1<<3)
#define FMD_RSVD_61_2              (1<<2)
#define FMD_RSVD_61_1              (1<<1)
#define FMD_SI                     (1<<0)

//REG_62
//#define FMD_WRITE_REQ            (1<<31)
//#define FMD_READ_REQ             (1<<30)
//#define FMD_RSVD_X2              (1<<15)
#define FMD_DTC                    (1<<14)
#define FMD_RSVD_X13(n)            (((n)&0x3FFF)<<0)





#endif


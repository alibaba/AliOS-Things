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


#ifndef _CFG_REGS_ASM_H_
#define _CFG_REGS_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'cfg_regs'."
#endif


#define PROD_ID                                 (0X0809)
#define SUB_ID                                  (0X19)

//==============================================================================
// cfg_regs
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CONFIG_REGS_BASE       0x01A24000

#define REG_CONFIG_REGS_BASE_HI    BASE_HI(REG_CONFIG_REGS_BASE)
#define REG_CONFIG_REGS_BASE_LO    BASE_LO(REG_CONFIG_REGS_BASE)

#define REG_CONFIG_REGS_CHIP_ID    REG_CONFIG_REGS_BASE_LO + 0x00000000
#define REG_CONFIG_REGS_BUILD_VERSION REG_CONFIG_REGS_BASE_LO + 0x00000004
#define REG_CONFIG_REGS_IO_DRIVE1_SELECT REG_CONFIG_REGS_BASE_LO + 0x00000010
#define REG_CONFIG_REGS_IO_DRIVE2_SELECT REG_CONFIG_REGS_BASE_LO + 0x00000014
#define REG_CONFIG_REGS_AUDIO_PD_SET REG_CONFIG_REGS_BASE_LO + 0x00000018
#define REG_CONFIG_REGS_AUDIO_PD_CLR REG_CONFIG_REGS_BASE_LO + 0x0000001C
#define REG_CONFIG_REGS_AUDIO_SEL_CFG REG_CONFIG_REGS_BASE_LO + 0x00000020
#define REG_CONFIG_REGS_AUDIO_MIC_CFG REG_CONFIG_REGS_BASE_LO + 0x00000024
#define REG_CONFIG_REGS_AUDIO_SPK_CFG REG_CONFIG_REGS_BASE_LO + 0x00000028
#define REG_CONFIG_REGS_AUDIO_RCV_GAIN REG_CONFIG_REGS_BASE_LO + 0x0000002C
#define REG_CONFIG_REGS_AUDIO_HEAD_GAIN REG_CONFIG_REGS_BASE_LO + 0x00000030
#define REG_CONFIG_REGS_THERMAL_CAL REG_CONFIG_REGS_BASE_LO + 0x00000034
#define REG_CONFIG_REGS_DBGOUT_CFG REG_CONFIG_REGS_BASE_LO + 0x00000038

//CHIP_ID
#define CFG_REGS_METAL_ID(n)       (((n)&0xFFF)<<0)
#define CFG_REGS_METAL_ID_MASK     (0xFFF<<0)
#define CFG_REGS_METAL_ID_SHIFT    (0)
#define CFG_REGS_BOND_ID(n)        (((n)&7)<<12)
#define CFG_REGS_BOND_ID_MASK      (7<<12)
#define CFG_REGS_BOND_ID_SHIFT     (12)
#define CFG_REGS_SUB_ID(n)         (((n)&31)<<15)
#define CFG_REGS_SUB_ID_MASK       (31<<15)
#define CFG_REGS_SUB_ID_SHIFT      (15)
#define CFG_REGS_PROD_ID(n)        (((n)&0xFFF)<<20)
#define CFG_REGS_PROD_ID_MASK      (0xFFF<<20)
#define CFG_REGS_PROD_ID_SHIFT     (20)

//Build_Version
#define CFG_REGS_MAJOR(n)          (((n)&15)<<28)
#define CFG_REGS_YEAR(n)           (((n)&15)<<24)
#define CFG_REGS_MONTH(n)          (((n)&0xFF)<<16)
#define CFG_REGS_DAY(n)            (((n)&0xFF)<<8)
#define CFG_REGS_BUILD_STYLE_FPGA  (0<<4)
#define CFG_REGS_BUILD_STYLE_CHIP  (1<<4)
#define CFG_REGS_BUILD_STYLE_FPGA_USB (2<<4)
#define CFG_REGS_BUILD_STYLE_FPGA_GSM (3<<4)
#define CFG_REGS_BUILD_REVISION(n) (((n)&15)<<0)

//IO_Drive1_Select
#define CFG_REGS_M_D_0_DRIVE_MASK  (7<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_0 (0<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_1 (1<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_2 (2<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_3 (3<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_4 (4<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_5 (5<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_6 (6<<0)
#define CFG_REGS_M_D_0_DRIVE_DRV_LEVEL_7 (7<<0)
#define CFG_REGS_M_D_1_DRIVE(n)    (((n)&7)<<3)
#define CFG_REGS_M_SPI_DRIVE_MASK  (7<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_0 (0<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_1 (1<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_2 (2<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_3 (3<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_4 (4<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_5 (5<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_6 (6<<6)
#define CFG_REGS_M_SPI_DRIVE_DRV_LEVEL_7 (7<<6)
#define CFG_REGS_LCD_DATA_DRIVE_MASK (3<<9)
#define CFG_REGS_LCD_DATA_DRIVE_FAST_FAST (0<<9)
#define CFG_REGS_LCD_DATA_DRIVE_FAST (1<<9)
#define CFG_REGS_LCD_DATA_DRIVE_SLOW (2<<9)
#define CFG_REGS_LCD_DATA_DRIVE_SLOW_SLOW (3<<9)
#define CFG_REGS_CAM_DRIVE_MASK    (3<<11)
#define CFG_REGS_CAM_DRIVE_FAST_FAST (0<<11)
#define CFG_REGS_CAM_DRIVE_FAST    (1<<11)
#define CFG_REGS_CAM_DRIVE_SLOW    (2<<11)
#define CFG_REGS_CAM_DRIVE_SLOW_SLOW (3<<11)
#define CFG_REGS_GPO_DRIVE_MASK    (3<<13)
#define CFG_REGS_GPO_DRIVE_FAST_FAST (0<<13)
#define CFG_REGS_GPO_DRIVE_FAST    (1<<13)
#define CFG_REGS_GPO_DRIVE_SLOW    (2<<13)
#define CFG_REGS_GPO_DRIVE_SLOW_SLOW (3<<13)
#define CFG_REGS_KEYPAD_DRIVE_MASK (3<<15)
#define CFG_REGS_KEYPAD_DRIVE_FAST_FAST (0<<15)
#define CFG_REGS_KEYPAD_DRIVE_FAST (1<<15)
#define CFG_REGS_KEYPAD_DRIVE_SLOW (2<<15)
#define CFG_REGS_KEYPAD_DRIVE_SLOW_SLOW (3<<15)
#define CFG_REGS_SDMMC_DRIVE_MASK  (3<<17)
#define CFG_REGS_SDMMC_DRIVE_FAST_FAST (0<<17)
#define CFG_REGS_SDMMC_DRIVE_FAST  (1<<17)
#define CFG_REGS_SDMMC_DRIVE_SLOW  (2<<17)
#define CFG_REGS_SDMMC_DRIVE_SLOW_SLOW (3<<17)

//IO_Drive2_Select
#define CFG_REGS_M_SPI_D_MASK      (3<<0)
#define CFG_REGS_M_SPI_D_NORMAL    (0<<0)
#define CFG_REGS_M_SPI_D_INPUT     (1<<0)
#define CFG_REGS_M_SPI_D_OUTPUT_0  (2<<0)
#define CFG_REGS_M_SPI_D_OUTPUT_1  (3<<0)
#define CFG_REGS_SPI_CAM_LCD_MASK  (3<<2)
#define CFG_REGS_SPI_CAM_LCD_NO_DIRECT (0<<2)
#define CFG_REGS_SPI_CAM_LCD_DIRECT (1<<2)
#define CFG_REGS_SPI_CAM_LCD_HIGH_Z (2<<2)
#define CFG_REGS_SPI_CAM_LCD_DIRECT_GPIO (3<<2)
#define CFG_REGS_FM_PON_FM_OFF     (0<<6)
#define CFG_REGS_FM_PON_FM_ON      (1<<6)
#define CFG_REGS_USB_ID_HOST_MODE  (0<<16)
#define CFG_REGS_USB_ID_DEVICE     (1<<16)

//audio_pd_set
#define CFG_REGS_AU_DEEP_PD_N      (1<<0)
#define CFG_REGS_AU_REF_PD_N       (1<<1)
#define CFG_REGS_AU_MIC_PD_N       (1<<2)
#define CFG_REGS_AU_AUXMIC_PD_N    (1<<3)
#define CFG_REGS_AU_AD_PD_N        (1<<4)
#define CFG_REGS_AU_DAC_PD_N       (1<<5)
#define CFG_REGS_AU_DAC_RESET_N    (1<<8)

//audio_pd_clr
//#define CFG_REGS_AU_DEEP_PD_N    (1<<0)
//#define CFG_REGS_AU_REF_PD_N     (1<<1)
//#define CFG_REGS_AU_MIC_PD_N     (1<<2)
//#define CFG_REGS_AU_AUXMIC_PD_N  (1<<3)
//#define CFG_REGS_AU_AD_PD_N      (1<<4)
//#define CFG_REGS_AU_DAC_PD_N     (1<<5)
//#define CFG_REGS_AU_DAC_RESET_N  (1<<8)

//audio_sel_cfg
#define CFG_REGS_AU_AUXMIC_SEL     (1<<0)
#define CFG_REGS_AU_SPK_SEL        (1<<1)
#define CFG_REGS_AU_SPK_MONO_SEL   (1<<2)
#define CFG_REGS_AU_RCV_SEL        (1<<3)
#define CFG_REGS_AU_HEAD_SEL       (1<<4)
#define CFG_REGS_AU_FM_SEL         (1<<5)

//audio_mic_cfg
#define CFG_REGS_AU_MIC_GAIN(n)    (((n)&15)<<0)
#define CFG_REGS_AU_MIC_MUTE_N     (1<<4)

//audio_spk_cfg
#define CFG_REGS_AU_SPK_GAIN(n)    (((n)&15)<<0)
#define CFG_REGS_AU_SPK_MUTE_N     (1<<4)

//audio_rcv_gain
#define CFG_REGS_AU_RCV_GAIN(n)    (((n)&15)<<0)
#define CFG_REGS_AU_RCV_GAIN_MASK  (15<<0)

//audio_head_gain
#define CFG_REGS_AU_HEAD_GAIN(n)   (((n)&15)<<0)
#define CFG_REGS_AU_HEAD_GAIN_MASK (15<<0)

//thermal_cal
#define CFG_REGS_THERMAL_CAL_VALUE(n) (((n)&0x3F)<<0)
#define CFG_REGS_THERMAL_CAL_VALID (1<<6)

//dbgout_cfg
#define CFG_REGS_DBGOUT_SEL(n)     (((n)&15)<<0)
#define CFG_REGS_DBGOUT_EN(n)      (((n)&0xFFFF)<<16)




#endif

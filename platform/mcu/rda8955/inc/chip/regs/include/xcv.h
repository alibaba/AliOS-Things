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


#ifndef _XCV_H_
#define _XCV_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'xcv'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// XCV_8809_PAGE0_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCV_8809_PAGE0_BASE    0xA2000000

typedef volatile struct
{
    REG32                          revision_id_00H;              //0x00000000
    REG32                          rx_gain_settings_01H;         //0x00000004
    REG32                          tx_gain_settings_02H;         //0x00000008
    REG32                          tx_ramp_setting_for_curve_0_03H; //0x0000000C
    REG32                          tx_gain_setting_for_slot_0_04H; //0x00000010
    REG32                          rxpll_frequency_setting_05H;  //0x00000014
    REG32                          rxpll_frequency_setting_06H;  //0x00000018
    REG32                          freq_compensation_register_and_rampdac_settings_07H; //0x0000001C
    REG32                          rfrequency_channel_number_08H; //0x00000020
    REG32                          ldo_setting_09H;              //0x00000024
    REG32                          calibration_result_of_tuning_and_filter_0AH; //0x00000028
    REG32                          calibration_result_of_mixer_and_filter_0BH; //0x0000002C
    REG32                          rxpll_vco_calibration_0CH;    //0x00000030
    REG32                          calibration_bits_0DH;         //0x00000034
    REG32                          rxpll_settings_0EH;           //0x00000038
    REG32                          rxpll_setting_0FH;            //0x0000003C
    REG32                          rxpll_settings_10H;           //0x00000040
    REG32                          rxpll_settings_11H;           //0x00000044
    REG32                          rxpll_settings_12H;           //0x00000048
    REG32                          rxpll_settings_13H;           //0x0000004C
    REG32                          rxpll_settings_14H;           //0x00000050
    REG32                          rxpll_settings_15H;           //0x00000054
    REG32                          analog_mode_settings_16H;     //0x00000058
    REG32                          rx_path_settings_17H;         //0x0000005C
    REG32                          adc_settings_18H;             //0x00000060
    REG32                          dac_crystal_setting_19H;      //0x00000064
    REG32                          crystal_setting_1AH;          //0x00000068
    REG32                          crystal_xdrv_setting_1BH;     //0x0000006C
    REG32                          thermometer_calibration_result_1CH; //0x00000070
    REG32                          vga_settings_1DH;             //0x00000074
    REG32                          tx_path_settings_1EH;         //0x00000078
    REG32                          bgap_afc_setting_1FH;         //0x0000007C
    REG32                          digital_power_20H;            //0x00000080
    REG32                          ldo_registers_21H;            //0x00000084
    REG32                          ldo_pd_setting_22H;           //0x00000088
    REG32                          pd_settings_23H;              //0x0000008C
    REG32                          timer_gpio_setting_24H;       //0x00000090
    REG32                          sdm_settings_25H;             //0x00000094
    REG32                          sdm_settings_26H;             //0x00000098
    REG32                          sdm_settings_27H;             //0x0000009C
    REG32                          sdm_settings_28H;             //0x000000A0
    REG32                          dsp_setting_29H;              //0x000000A4
    REG32                          ramp_setting_for_slot_1_2AH;  //0x000000A8
    REG32                          tx_gain_setting_for_slot_1_2BH; //0x000000AC
    REG32                          ramp_setting_for_slot_2_2CH;  //0x000000B0
    REG32                          tx_gain_setting_for_slot_2_2DH; //0x000000B4
    REG32                          dsp_setting_2EH;              //0x000000B8
    REG32                          misc_2FH;                     //0x000000BC
    REG32                          system_control_setting_30H;   //0x000000C0
    REG32                          equfil_setting_31H;           //0x000000C4
    REG32                          equfil_setting_32H;           //0x000000C8
    REG32                          equfil_setting_33H;           //0x000000CC
    REG32                          ramp_setting_for_slot_3_34H;  //0x000000D0
    REG32                          tx_gain_setting_for_slot_3_35H; //0x000000D4
    REG32                          ramp_curve_setting_for_gsm_in_special_use_36H; //0x000000D8
    REG32                          tx_block_setting_37H;         //0x000000DC
    REG32                          rx_block_setting_38H;         //0x000000E0
    REG32                          equfil_parameter_1_39H;       //0x000000E4
    REG32                          tx_block_setting_3AH;         //0x000000E8
    REG32                          equfil_parameter_2_3BH;       //0x000000EC
    REG32                          dc_cancel_3CH;                //0x000000F0
    REG32                          pa_ramp_array_program_register_3DH; //0x000000F4
    REG32                          tx_timer_control_3EH;         //0x000000F8
    REG32                          page_setting_3FH;             //0x000000FC
} HWP_XCV_8809_PAGE0_T;

#define hwp_xcv8809Page0           ((HWP_XCV_8809_PAGE0_T*) KSEG1(REG_XCV_8809_PAGE0_BASE))


//revision_id_00H
#define XCV_8809_PAGE0_WRITE_REQ   (1<<31)
#define XCV_8809_PAGE0_READ_REQ    (1<<30)
#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x3FFFF)<<0)

//rx_gain_settings_01H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RX_GAIN_TABLE_ENABLE (1<<17)
#define XCV_8809_PAGE0_RX_GAIN_TABLE_POINTER(n) (((n)&7)<<14)
#define XCV_8809_PAGE0_RX_GAIN_BLOCKER_CMP(n) (((n)&15)<<10)
#define XCV_8809_PAGE0_GAIN1_CT(n) (((n)&3)<<8)
#define XCV_8809_PAGE0_GAIN2_CT_REG(n) (((n)&0xFF)<<0)

//tx_gain_settings_02H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_R2_RESERVED(n) (((n)&0x7F)<<11)
#define XCV_8809_PAGE0_TX_BLOCK_BB_SEND_DATA_FLAG (1<<10)
#define XCV_8809_PAGE0_DIGRF_STREAM_MODE_ENABLE (1<<9)
#define XCV_8809_PAGE0_TXLPF_GAIN_BIT(n) (((n)&0x1FF)<<0)

//tx_ramp_setting_for_curve_0_03H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&7)<<15)
#define XCV_8809_PAGE0_MULTI_SLOT_SEL(n) (((n)&3)<<13)
#define XCV_8809_PAGE0_RAMP_CURVE_SEL_0(n) (((n)&7)<<10)
#define XCV_8809_PAGE0_RAMP_MULT_FACTOR_0(n) (((n)&0x3FF)<<0)

//tx_gain_setting_for_slot_0_04H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x1FFF)<<5)
#define XCV_8809_PAGE0_GAIN3_CT_0(n) (((n)&31)<<0)

//rxpll_frequency_setting_05H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_DCDC_1P2_ENABLE (1<<17)
#define XCV_8809_PAGE0_DCDC_1P2_LDO_ENABLE (1<<16)
#define XCV_8809_PAGE0_DCDC_2P4_ENABLE (1<<15)
#define XCV_8809_PAGE0_DCDC_2P4_LDO_ENABLE (1<<14)
#define XCV_8809_PAGE0_FREQ_SYNTHESIZE(n) (((n)&0x3FFF)<<0)

//rxpll_frequency_setting_06H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_FREQ_SYNTHESIZE(n) (((n)&0x3FFFF)<<0)

//freq_compensation_register_and_rampdac_settings_07H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RAMP_GAIN_BIT(n) (((n)&31)<<13)
#define XCV_8809_PAGE0_PD_DAC_RAMP_DR (1<<12)
#define XCV_8809_PAGE0_RAMP_DIN_DR (1<<11)
#define XCV_8809_PAGE0_PD_DAC_RAMP_REG (1<<10)
#define XCV_8809_PAGE0_RAMP_DIN_REG(n) (((n)&0x3FF)<<0)

//rfrequency_channel_number_08H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_MDLL_DIV_REG(n) (((n)&3)<<14)
#define XCV_8809_PAGE0_BAND_SEL(n) (((n)&3)<<12)
#define XCV_8809_PAGE0_RXON_ARFCN  (1<<11)
#define XCV_8809_PAGE0_TXON_ARFCN  (1<<10)
#define XCV_8809_PAGE0_ARFCN(n)    (((n)&0x3FF)<<0)

//ldo_setting_09H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_XEN_DR      (1<<17)
#define XCV_8809_PAGE0_XEN_REG     (1<<16)
#define XCV_8809_PAGE0_XEN_BT_DR   (1<<15)
#define XCV_8809_PAGE0_XEN_BT_REG  (1<<14)
#define XCV_8809_PAGE0_RESRVED(n)  (((n)&15)<<10)
#define XCV_8809_PAGE0_TEST_BIT(n) (((n)&3)<<8)
#define XCV_8809_PAGE0_LDO_ADC_VBIT(n) (((n)&7)<<5)
#define XCV_8809_PAGE0_LDO_ADC_BYPASS (1<<4)
#define XCV_8809_PAGE0_LDO_XTAL_VBIT(n) (((n)&7)<<1)
#define XCV_8809_PAGE0_LDO_XTAL_BYPASS (1<<0)

//calibration_result_of_tuning_and_filter_0AH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x1FFFF)<<1)
#define XCV_8809_PAGE0_PDN_ARFCN   (1<<0)

//calibration_result_of_mixer_and_filter_0BH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED  (1<<17)
#define XCV_8809_PAGE0_FILTER_DAC2_BIT_DR (1<<16)
#define XCV_8809_PAGE0_FILTER_DAC2_BIT_I_REG(n) (((n)&0xFF)<<8)
#define XCV_8809_PAGE0_FILTER_DAC2_BIT_Q_REG(n) (((n)&0xFF)<<0)

//rxpll_vco_calibration_0CH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED  (1<<17)
#define XCV_8809_PAGE0_RFVCO_VCOM_VAR_BIT(n) (((n)&7)<<14)
#define XCV_8809_PAGE0_RFVCO_BIT_DR (1<<13)
#define XCV_8809_PAGE0_RFVCO_BIT_REG(n) (((n)&0x1FFF)<<0)

//calibration_bits_0DH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x3FF)<<8)
#define XCV_8809_PAGE0_ADC_DIGI_PWR_BIT_DR (1<<7)
#define XCV_8809_PAGE0_ADC_CAL_REFI_BIT_DR (1<<6)
#define XCV_8809_PAGE0_ADC_DIGI_PWR_BIT_REG(n) (((n)&7)<<3)
#define XCV_8809_PAGE0_ADC_CAL_REFI_BIT_REG(n) (((n)&7)<<0)

//rxpll_settings_0EH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_THERMO_4BIT_3_DR (1<<17)
#define XCV_8809_PAGE0_THERMO_4BIT_3_REG(n) (((n)&15)<<13)
#define XCV_8809_PAGE0_THERMO_4BIT_1_DR (1<<12)
#define XCV_8809_PAGE0_THERMO_7BIT_1_DR (1<<11)
#define XCV_8809_PAGE0_THERMO_4BIT_1_REG(n) (((n)&15)<<7)
#define XCV_8809_PAGE0_THERMO_7BIT_1_REG(n) (((n)&0x7F)<<0)

//rxpll_setting_0FH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RFVCO_VARCOM_BIT(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_RXPLL_LPF_GAIN(n) (((n)&15)<<12)
#define XCV_8809_PAGE0_RXPLL_SINK_MODE(n) (((n)&7)<<9)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&31)<<4)
#define XCV_8809_PAGE0_VCO_TXBUF_EN_DR (1<<3)
#define XCV_8809_PAGE0_VCO_TXBUF_EN_REG (1<<2)
#define XCV_8809_PAGE0_BAND_SEL_VCO_DR (1<<1)
#define XCV_8809_PAGE0_BAND_SEL_VCO_REG (1<<0)

//rxpll_settings_10H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RXPLL_LOCK  (1<<17)
//#define XCV_8809_PAGE0_RESERVED  (1<<16)
#define XCV_8809_PAGE0_RXPLL_REG_DIGI_BIT(n) (((n)&15)<<12)
#define XCV_8809_PAGE0_RXPLL_REG_FLT_BIT(n) (((n)&15)<<8)
#define XCV_8809_PAGE0_RXPLL_REG_DIV2_BIT(n) (((n)&15)<<4)
#define XCV_8809_PAGE0_RXPLL_REG_PRESC_BIT(n) (((n)&15)<<0)

//rxpll_settings_11H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RXPLL_REFMULTI2_EN (1<<17)
#define XCV_8809_PAGE0_RXPLL_CAL_CLK_SEL (1<<16)
#define XCV_8809_PAGE0_RXPLL_OPEN_EN (1<<15)
#define XCV_8809_PAGE0_RXPLL_XFER_AUX_EN (1<<14)
#define XCV_8809_PAGE0_RXPLL_BYPASS_NOTCH (1<<13)
#define XCV_8809_PAGE0_RXPLL_PFD_RES_BIT(n) (((n)&0x3F)<<7)
#define XCV_8809_PAGE0_RXPLL_PHASE_CTRL_DLY(n) (((n)&3)<<5)
#define XCV_8809_PAGE0_RXPLL_FBC_SEL(n) (((n)&7)<<2)
#define XCV_8809_PAGE0_RXPLL_CAL_BIT(n) (((n)&3)<<0)

//rxpll_settings_12H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_FILTER_CAL1_ENABLE (1<<17)
#define XCV_8809_PAGE0_FILTER_TUNING_BIT(n) (((n)&15)<<13)
#define XCV_8809_PAGE0_FILTER_CTRL_BIT(n) (((n)&0x1FFF)<<0)

//rxpll_settings_13H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_DIAG_DAC_SINE (1<<17)
#define XCV_8809_PAGE0_IQ_SWAP     (1<<16)
#define XCV_8809_PAGE0_CTRL_EXT(n) (((n)&0xFFFF)<<0)

//rxpll_settings_14H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_DAC_MUX_EN(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_DAC_CORE_BIT(n) (((n)&7)<<13)
#define XCV_8809_PAGE0_DAC_IOUT_ENABLE (1<<12)
#define XCV_8809_PAGE0_DAC_VTR_SEL (1<<11)
#define XCV_8809_PAGE0_DAC_CM_BIT(n) (((n)&3)<<9)
#define XCV_8809_PAGE0_DAC_RANGE_BIT(n) (((n)&3)<<7)
#define XCV_8809_PAGE0_DAC_BIAS_SEL(n) (((n)&7)<<4)
#define XCV_8809_PAGE0_AUX_DAC_OUT_ENABLE (1<<3)
#define XCV_8809_PAGE0_DAC_LOW_BIT(n) (((n)&7)<<0)

//rxpll_settings_15H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_FORMER_FIFO_EN (1<<17)
#define XCV_8809_PAGE0_FORMER_DEM_INV (1<<16)
#define XCV_8809_PAGE0_FORMER_CTSAMPLE(n) (((n)&15)<<12)
#define XCV_8809_PAGE0_FORMER_START_TH(n) (((n)&0x3F)<<6)
#define XCV_8809_PAGE0_FORMER_CLK_EDGE (1<<5)
#define XCV_8809_PAGE0_FORMER_WT_CT(n) (((n)&15)<<1)
#define XCV_8809_PAGE0_DC_OUT_EN   (1<<0)

//analog_mode_settings_16H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_PLL_RESGISTER_POINTER_DR (1<<17)
#define XCV_8809_PAGE0_PLL_REGISTER_POINTER_REG(n) (((n)&3)<<15)
#define XCV_8809_PAGE0_SDM_CLK_SEL_PLL_CAL_EN (1<<14)
#define XCV_8809_PAGE0_SDM_CLK_SEL_PLL_CAL_NEN (1<<13)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<11)
#define XCV_8809_PAGE0_FILTER_CAL_MODE (1<<10)
#define XCV_8809_PAGE0_FILTER_RSTN_DR (1<<9)
#define XCV_8809_PAGE0_DIV_GRP_HIGHBAND_DR (1<<8)
#define XCV_8809_PAGE0_IMGREJ_DSP_DR (1<<7)
#define XCV_8809_PAGE0_FILTER_LP_MODE_DR (1<<6)
#define XCV_8809_PAGE0_FILTER_IF200K_MODE_DR (1<<5)
#define XCV_8809_PAGE0_FILTER_RSTN_REG (1<<4)
#define XCV_8809_PAGE0_DIV_GRP_HIGHBAND_REG (1<<3)
#define XCV_8809_PAGE0_IMGREJ_DSP_REG (1<<2)
#define XCV_8809_PAGE0_FILTER_LP_MODE_REG (1<<1)
#define XCV_8809_PAGE0_FILTER_IF200K_MODE_REG (1<<0)

//rx_path_settings_17H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_ADC_RESET_DR (1<<17)
#define XCV_8809_PAGE0_ADC_RESET_REG (1<<16)
#define XCV_8809_PAGE0_ADC_RESET_MODE(n) (((n)&3)<<14)
#define XCV_8809_PAGE0_ADC_IQ_SWAP (1<<13)
#define XCV_8809_PAGE0_ADC_ISEL(n) (((n)&3)<<11)
//#define XCV_8809_PAGE0_RESERVED  (1<<10)
#define XCV_8809_PAGE0_DAC_RSTN_TIME_SEL(n) (((n)&3)<<8)
#define XCV_8809_PAGE0_PD_DAC      (1<<7)
#define XCV_8809_PAGE0_DAC_RSTN_DR (1<<6)
#define XCV_8809_PAGE0_DAC_RSTN_REG (1<<5)
#define XCV_8809_PAGE0_THERMO_4BIT_2_DR (1<<4)
#define XCV_8809_PAGE0_THERMO_4BIT_2_REG(n) (((n)&15)<<0)

//adc_settings_18H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_ADC_RESET_DSP (1<<15)
#define XCV_8809_PAGE0_TIMER_SEL_DSP_RESET_MS(n) (((n)&3)<<13)
#define XCV_8809_PAGE0_TIMER_FILTER_RSTN_SEL(n) (((n)&3)<<11)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<9)
#define XCV_8809_PAGE0_RESET_ADC_DELAY(n) (((n)&0x1FF)<<0)

//dac_crystal_setting_19H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_CAP_BANK_BIT_NOR(n) (((n)&0xFF)<<10)
#define XCV_8809_PAGE0_OSC_CFIX_NOR (1<<9)
#define XCV_8809_PAGE0_IBIT_XOSC_NOR(n) (((n)&15)<<5)
#define XCV_8809_PAGE0_FIX_IBIT_XOSC_NOR(n) (((n)&31)<<0)

//crystal_setting_1AH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_XDRV_AUX_SW_BIT(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_XDRV_AUX_SW_BYPASS (1<<15)
#define XCV_8809_PAGE0_XDRV_AUX_REG_BIT(n) (((n)&7)<<12)
#define XCV_8809_PAGE0_XDRV_AUX_STEP_EN(n) (((n)&15)<<8)
//#define XCV_8809_PAGE0_RESERVED  (1<<7)
#define XCV_8809_PAGE0_XTAL_RDAC_HLSEL(n) (((n)&3)<<5)
#define XCV_8809_PAGE0_XTAL_RESERVED(n) (((n)&3)<<3)
#define XCV_8809_PAGE0_XTAL_REG_BIT(n) (((n)&7)<<0)

//crystal_xdrv_setting_1BH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_THMT_DAC_REF(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_THMT_GAIN(n) (((n)&3)<<14)
#define XCV_8809_PAGE0_PKDTC_EN_DR (1<<13)
#define XCV_8809_PAGE0_PKDTC_EN_REG (1<<12)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<10)
#define XCV_8809_PAGE0_CLK26M_ADC_ENABLE_DR (1<<9)
#define XCV_8809_PAGE0_CLK26M_ADC_ENABLE_REG (1<<8)
#define XCV_8809_PAGE0_CLK26M_DSP_ENABLE_DR (1<<7)
#define XCV_8809_PAGE0_CLK26M_DSP_ENABLE_REG (1<<6)
#define XCV_8809_PAGE0_CLK26M_INTF_ENABLE_DR (1<<5)
#define XCV_8809_PAGE0_CLK26M_INTF_ENABLE_REG (1<<4)
//#define XCV_8809_PAGE0_RESERVED  (1<<3)
#define XCV_8809_PAGE0_CLK26M_PKD_ENABLE (1<<2)
#define XCV_8809_PAGE0_CLK26M_PLL_ENABLE_DR (1<<1)
#define XCV_8809_PAGE0_CLK26M_PLL_ENABLE_REG (1<<0)

//thermometer_calibration_result_1CH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_THMT_CURR_SW (1<<17)
#define XCV_8809_PAGE0_THMT_PTAT_VAR(n) (((n)&15)<<13)
#define XCV_8809_PAGE0_THERMO_PULSE_RESTRAIN_CT(n) (((n)&3)<<11)
#define XCV_8809_PAGE0_THERMO_CAL_CNT(n) (((n)&15)<<7)
#define XCV_8809_PAGE0_THERMO_CAL_BIT_DR (1<<6)
#define XCV_8809_PAGE0_THERMO_CAL_BIT_REG(n) (((n)&0x3F)<<0)

//vga_settings_1DH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_AGC_TABLE_REG(n) (((n)&0x3FFFF)<<0)

//tx_path_settings_1EH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RESERVED_DSP(n) (((n)&0x3FFFF)<<0)

//bgap_afc_setting_1FH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE0_DIGI_AFC_BIT(n) (((n)&0x3FFF)<<0)

//digital_power_20H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_PU_XTAL_DR  (1<<17)
#define XCV_8809_PAGE0_PU_XTAL_REG (1<<16)
#define XCV_8809_PAGE0_PD_MDLL_DR  (1<<15)
#define XCV_8809_PAGE0_PD_MDLL_REG (1<<14)
#define XCV_8809_PAGE0_PD_PADRV_DR (1<<13)
#define XCV_8809_PAGE0_PD_PADRV_REG (1<<12)
#define XCV_8809_PAGE0_SEL_BG      (1<<11)
#define XCV_8809_PAGE0_DIGITAL_POWER_LP_EN (1<<10)
#define XCV_8809_PAGE0_DIGITAL_POWER_LP_MODE (1<<9)
#define XCV_8809_PAGE0_DIGITAL_POWER_TEST_EN (1<<8)
#define XCV_8809_PAGE0_DIGITAL_POWER_IREF_BIT(n) (((n)&7)<<5)
#define XCV_8809_PAGE0_DVDD_REF_DSP(n) (((n)&7)<<2)
#define XCV_8809_PAGE0_PD_DIGITAL_POWER_DR (1<<1)
#define XCV_8809_PAGE0_PD_DIGITAL_POWER_REG (1<<0)

//ldo_registers_21H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_PD_LDO_DR   (1<<17)
#define XCV_8809_PAGE0_PD_LDO_REG  (1<<16)
#define XCV_8809_PAGE0_LDO_ANA_VBIT(n) (((n)&7)<<13)
#define XCV_8809_PAGE0_LDO_ANA_BYPASS (1<<12)
#define XCV_8809_PAGE0_LDO_VCO_VBIT(n) (((n)&7)<<9)
#define XCV_8809_PAGE0_LDO_VCO_BYPASS (1<<8)
#define XCV_8809_PAGE0_LDO_PADRV_VBIT(n) (((n)&7)<<5)
#define XCV_8809_PAGE0_LDO_PADRV_BYPASS (1<<4)
#define XCV_8809_PAGE0_LDO_PDT_VBIT(n) (((n)&7)<<1)
#define XCV_8809_PAGE0_LDO_PDT_BYPASS (1<<0)

//ldo_pd_setting_22H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x7FF)<<7)
#define XCV_8809_PAGE0_PD_RXVCO_TXBUF (1<<6)
#define XCV_8809_PAGE0_PD_RXVCO_RXBUF (1<<5)
#define XCV_8809_PAGE0_PD_RXVCO    (1<<4)
#define XCV_8809_PAGE0_PD_RXPLL_PRESC (1<<3)
#define XCV_8809_PAGE0_PD_RXPLL_PERI (1<<2)
#define XCV_8809_PAGE0_PD_RXPLL_GRO (1<<1)
#define XCV_8809_PAGE0_PD_RXPLL_RDAC (1<<0)

//pd_settings_23H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_PD_LNA_GRP  (1<<17)
#define XCV_8809_PAGE0_SEL_LNA850  (1<<16)
#define XCV_8809_PAGE0_SEL_LNA900  (1<<15)
#define XCV_8809_PAGE0_SEL_LNA1800 (1<<14)
#define XCV_8809_PAGE0_SEL_LNA1900 (1<<13)
#define XCV_8809_PAGE0_SEL_LNA_SINGLE_LB (1<<12)
#define XCV_8809_PAGE0_SEL_LNA_SINGLE_HB (1<<11)
#define XCV_8809_PAGE0_PD_MIXER_GRP (1<<10)
#define XCV_8809_PAGE0_SEL_MIXER_L (1<<9)
#define XCV_8809_PAGE0_SEL_MIXER_H (1<<8)
#define XCV_8809_PAGE0_PD_FILTER   (1<<7)
#define XCV_8809_PAGE0_PD_ADC      (1<<6)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&15)<<2)
#define XCV_8809_PAGE0_PD_BGIVERF  (1<<1)
#define XCV_8809_PAGE0_PD_THERMO   (1<<0)

//timer_gpio_setting_24H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_TIMER_TXPOWER_START_DELAY(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_RX_WAIT_PLL_STABLE_TIMER_ON (1<<15)
#define XCV_8809_PAGE0_TX_WAIT_PLL_STABLE_TIMER_ON (1<<14)
#define XCV_8809_PAGE0_RX_WAIT_PLL_STABLE_TIMER(n) (((n)&3)<<12)
#define XCV_8809_PAGE0_TX_WAIT_PLL_STABLE_TIMER(n) (((n)&3)<<10)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&7)<<7)
#define XCV_8809_PAGE0_AUX_BUF_DELAY (1<<6)
#define XCV_8809_PAGE0_GPIO_AUXCLK_EN_DR (1<<5)
#define XCV_8809_PAGE0_GPIO_AUXCLK_EN_REG (1<<4)
#define XCV_8809_PAGE0_GPIO_PAON_DR (1<<3)
#define XCV_8809_PAGE0_TIMER_VGA_ON_OVER_DR (1<<2)
#define XCV_8809_PAGE0_GPIO_PAON_REG (1<<1)
#define XCV_8809_PAGE0_TIMER_VGA_ON_OWER_REG (1<<0)

//sdm_settings_25H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_FORMER_CLK_EDGE_SDM (1<<17)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<15)
#define XCV_8809_PAGE0_TRI_WAVE_BYPASS (1<<14)
#define XCV_8809_PAGE0_TRI_WAVE_STEP(n) (((n)&3)<<12)
#define XCV_8809_PAGE0_INT_DEC_SEL(n) (((n)&3)<<10)
#define XCV_8809_PAGE0_OFFSET_ENABLE (1<<9)
#define XCV_8809_PAGE0_SQUARE_WAVE_IN (1<<8)
#define XCV_8809_PAGE0_NPHASE(n)   (((n)&15)<<4)
#define XCV_8809_PAGE0_OFFSET_SELECT_WAVE(n) (((n)&7)<<1)
#define XCV_8809_PAGE0_SHIFT_MORE  (1<<0)

//sdm_settings_26H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_MDLL_DITHER_EN (1<<17)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&31)<<12)
#define XCV_8809_PAGE0_DITHER_BYPASS (1<<11)
#define XCV_8809_PAGE0_FORMER_BYPASS_DR (1<<10)
#define XCV_8809_PAGE0_SDM_RESETN_DR (1<<9)
#define XCV_8809_PAGE0_RESETN_DSP_DR (1<<8)
#define XCV_8809_PAGE0_TX_RX_DR    (1<<7)
#define XCV_8809_PAGE0_IF_MODE_DR  (1<<6)
#define XCV_8809_PAGE0_FORMER_BYPASS_REG (1<<5)
#define XCV_8809_PAGE0_SDM_RESETN_REG (1<<4)
#define XCV_8809_PAGE0_RESETN_DSP_REG (1<<3)
#define XCV_8809_PAGE0_TX_RX_REG   (1<<2)
#define XCV_8809_PAGE0_IF_MODE_REG(n) (((n)&3)<<0)

//sdm_settings_27H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_MDLL_DITHER_MODE (1<<17)
#define XCV_8809_PAGE0_MDLL_DIHTER_BIT(n) (((n)&7)<<14)
#define XCV_8809_PAGE0_MDLL_CP_IBIT(n) (((n)&7)<<11)
#define XCV_8809_PAGE0_MDLL_STARTUP_V(n) (((n)&7)<<8)
#define XCV_8809_PAGE0_MDLL_VCTRL_TEST_EN (1<<7)
#define XCV_8809_PAGE0_MDLL_BAND_SEL_DR (1<<6)
#define XCV_8809_PAGE0_MDLL_BAND_SEL_REG (1<<5)
#define XCV_8809_PAGE0_MDLL_BAND_DR (1<<4)
//#define XCV_8809_PAGE0_RESERVED  (1<<3)
#define XCV_8809_PAGE0_MDLL_BAND_REG(n) (((n)&7)<<0)

//sdm_settings_28H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_DEC_NUMERATOR(n) (((n)&7)<<15)
#define XCV_8809_PAGE0_EDGE_POLAR_MODE (1<<14)
#define XCV_8809_PAGE0_PN9_EN      (1<<13)
#define XCV_8809_PAGE0_DAC_FIX_EN  (1<<12)
#define XCV_8809_PAGE0_DAC_FIX(n)  (((n)&0xFFF)<<0)

//dsp_setting_29H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_LPFIL_BYPASS (1<<17)
#define XCV_8809_PAGE0_DCCANCEL1_BYPASS (1<<16)
#define XCV_8809_PAGE0_DCCANCEL2_BYPASS (1<<15)
#define XCV_8809_PAGE0_COMPFIL_MODE (1<<14)
#define XCV_8809_PAGE0_BW_SEL(n)   (((n)&7)<<11)
#define XCV_8809_PAGE0_GAIN2_DELAY_BYPASS (1<<10)
#define XCV_8809_PAGE0_GAIN2_DELAY_CT(n) (((n)&0x3F)<<4)
#define XCV_8809_PAGE0_TS_MODE(n)  (((n)&15)<<0)

//ramp_setting_for_slot_1_2AH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_GAIN3_CT_1(n) (((n)&31)<<13)
#define XCV_8809_PAGE0_RAMP_CURVE_SEL_1(n) (((n)&7)<<10)
#define XCV_8809_PAGE0_RAMP_MULT_FACTOR_1(n) (((n)&0x3FF)<<0)

//tx_gain_setting_for_slot_1_2BH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RMX_OFFSET_RANGE_BIT(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_PGA_OP_IBIT (1<<15)
#define XCV_8809_PAGE0_PGA_BW_CBIT(n) (((n)&3)<<13)
#define XCV_8809_PAGE0_PGA_BW_RBIT(n) (((n)&3)<<11)
#define XCV_8809_PAGE0_PGA_CTRL_BIT(n) (((n)&0x7FF)<<0)

//ramp_setting_for_slot_2_2CH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_GAIN3_CT_2(n) (((n)&31)<<13)
#define XCV_8809_PAGE0_RAMP_CURVE_SEL_2(n) (((n)&7)<<10)
#define XCV_8809_PAGE0_RAMP_MULT_FACTOR_2(n) (((n)&0x3FF)<<0)

//tx_gain_setting_for_slot_2_2DH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_LNA_I_BIT(n) (((n)&7)<<15)
#define XCV_8809_PAGE0_LNA_FREQ_LB(n) (((n)&7)<<12)
#define XCV_8809_PAGE0_LNA_FREQ_HB(n) (((n)&7)<<9)
#define XCV_8809_PAGE0_LNA_CTRL_BIT (1<<8)
#define XCV_8809_PAGE0_LNA_NOTCH_LBAND_DR (1<<7)
#define XCV_8809_PAGE0_LNA_NOTCH_LBAND_REG (1<<6)
#define XCV_8809_PAGE0_LNA_NOTCH_HBAND_DR (1<<5)
#define XCV_8809_PAGE0_LNA_NOTCH_HBAND_REG (1<<4)
#define XCV_8809_PAGE0_LNA_POWDET_LBAND_EN_DR (1<<3)
#define XCV_8809_PAGE0_LNA_POWDET_LBAND_EN_REG (1<<2)
#define XCV_8809_PAGE0_LNA_POWDET_HBAND_EN_DR (1<<1)
#define XCV_8809_PAGE0_LNA_POWDET_HBAND_EN_REG (1<<0)

//dsp_setting_2EH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_DLL_MODE_DR (1<<17)
#define XCV_8809_PAGE0_DLL_MODE_REG(n) (((n)&3)<<15)
#define XCV_8809_PAGE0_MDLL_STARTUP_DR (1<<14)
#define XCV_8809_PAGE0_MDLL_STARTUP_REG (1<<13)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<11)
#define XCV_8809_PAGE0_MDLL_ON_XEN_BT (1<<10)
#define XCV_8809_PAGE0_MDLL_STARTUP_DELAY(n) (((n)&3)<<8)
#define XCV_8809_PAGE0_RESERVED_SDM(n) (((n)&0xFF)<<0)

//misc_2FH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_MIXER_POWER_PSR_EN (1<<17)
#define XCV_8809_PAGE0_MIXER_POWER_ANA_EN (1<<16)
#define XCV_8809_PAGE0_RESERVED_ANALOG(n) (((n)&0xFFFF)<<0)

//system_control_setting_30H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_SINGLE_LNA_MODE (1<<17)
#define XCV_8809_PAGE0_RX_BLOCKER_MODE_DR (1<<16)
#define XCV_8809_PAGE0_RX_BLOCKER_MODE_REG (1<<15)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&7)<<12)
#define XCV_8809_PAGE0_DIGRF_TEST_EN (1<<11)
#define XCV_8809_PAGE0_GSM_MODE    (1<<10)
#define XCV_8809_PAGE0_DIGRF_ENABLE (1<<9)
#define XCV_8809_PAGE0_ZF_MODE     (1<<8)
#define XCV_8809_PAGE0_IF_200K_MODE (1<<7)
#define XCV_8809_PAGE0_FREQ_MODE   (1<<6)
#define XCV_8809_PAGE0_DIRECT_REG  (1<<5)
#define XCV_8809_PAGE0_MDLL_KEEP_ON (1<<4)
#define XCV_8809_PAGE0_IMGREJ      (1<<3)
#define XCV_8809_PAGE0_MDLL_DIV_DR (1<<2)
#define XCV_8809_PAGE0_CHIP_SELF_CAL_ENABLE (1<<1)
#define XCV_8809_PAGE0_SOFT_RESETN (1<<0)

//equfil_setting_31H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_EQUFIL_TX_G_GSM_D8(n) (((n)&0xFFFF)<<0)

//equfil_setting_32H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&7)<<15)
#define XCV_8809_PAGE0_RESET_DSP_DELAY(n) (((n)&0xFF)<<7)
#define XCV_8809_PAGE0_RESET_SDM_DELAY(n) (((n)&0x7F)<<0)

//equfil_setting_33H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_FORMER_CT_SHIFT_GSM_LB(n) (((n)&7)<<15)
#define XCV_8809_PAGE0_FORMER_CT_SHIFT_GSM_HB(n) (((n)&7)<<12)
#define XCV_8809_PAGE0_R33_RESERVED(n) (((n)&0xFFF)<<0)

//ramp_setting_for_slot_3_34H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_GAIN3_CT_3(n) (((n)&31)<<13)
#define XCV_8809_PAGE0_RAMP_CURVE_SEL_3(n) (((n)&7)<<10)
#define XCV_8809_PAGE0_RAMP_MULT_FACTOR_3(n) (((n)&0x3FF)<<0)

//tx_gain_setting_for_slot_3_35H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_FREQ_COMP(n) (((n)&0xFFFF)<<0)

//ramp_curve_setting_for_gsm_in_special_use_36H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_DC_CAL_ONE_TIME (1<<17)
#define XCV_8809_PAGE0_DC_TABLE_FULL_MODE (1<<16)
#define XCV_8809_PAGE0_DC_TABLE_POINTER_DR (1<<15)
#define XCV_8809_PAGE0_DC_TABLE_POINTER_REG(n) (((n)&7)<<12)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0xFF)<<4)
#define XCV_8809_PAGE0_TIME_SEL_AFTER_RAMPD(n) (((n)&3)<<2)
#define XCV_8809_PAGE0_TIME_SEL_DIGRF_TXEN(n) (((n)&3)<<0)

//tx_block_setting_37H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_SLOT_NUMBER_DR (1<<17)
#define XCV_8809_PAGE0_SLOT_NUMBER_REG(n) (((n)&3)<<15)
#define XCV_8809_PAGE0_TXCLKPOLARITY (1<<14)
#define XCV_8809_PAGE0_TX_BLOCK_FLR (1<<13)
#define XCV_8809_PAGE0_TXDATA_OUT_TIMER(n) (((n)&0xFF)<<5)
#define XCV_8809_PAGE0_SLOT_LENGTH_DELTA(n) (((n)&31)<<0)

//rx_block_setting_38H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_RXCLKPOLARITY (1<<17)
#define XCV_8809_PAGE0_RX_DELAY_COUNTER(n) (((n)&15)<<13)
#define XCV_8809_PAGE0_RX_IQ_ORDER (1<<12)
#define XCV_8809_PAGE0_SAMPLE_MODE(n) (((n)&3)<<10)
#define XCV_8809_PAGE0_R38_RESERVED(n) (((n)&0x7F)<<3)
#define XCV_8809_PAGE0_RESETN_RX_DATA_MODE(n) (((n)&3)<<1)
#define XCV_8809_PAGE0_RESETN_RX_DATA_REG (1<<0)

//equfil_parameter_1_39H
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_GPIO_SLEW_BIT(n) (((n)&3)<<16)
#define XCV_8809_PAGE0_DIGRF_SLEW_BIT_TEST(n) (((n)&3)<<14)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&3)<<12)
#define XCV_8809_PAGE0_GPIO_BS_GSM850_RX(n) (((n)&7)<<9)
#define XCV_8809_PAGE0_GPIO_BS_GSM900_RX(n) (((n)&7)<<6)
#define XCV_8809_PAGE0_GPIO_BS_DCS1800_RX(n) (((n)&7)<<3)
#define XCV_8809_PAGE0_GPIO_BS_PCS1900_RX(n) (((n)&7)<<0)

//tx_block_setting_3AH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE0_GPIO_BS_GSM850_TX(n) (((n)&7)<<9)
#define XCV_8809_PAGE0_GPIO_BS_GSM900_TX(n) (((n)&7)<<6)
#define XCV_8809_PAGE0_GPIO_BS_DCS1800_TX(n) (((n)&7)<<3)
#define XCV_8809_PAGE0_GPIO_BS_PCS1900_TX(n) (((n)&7)<<0)

//equfil_parameter_2_3BH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x7F)<<11)
#define XCV_8809_PAGE0_DC_CT1(n)   (((n)&0x7FF)<<0)

//dc_cancel_3CH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_LDO_CUR_LMT(n) (((n)&0x7F)<<11)
#define XCV_8809_PAGE0_DC_CT2(n)   (((n)&0x7FF)<<0)

//pa_ramp_array_program_register_3DH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_ARRAY_DATA(n) (((n)&0x3FF)<<8)
#define XCV_8809_PAGE0_ARRAY_ADDR(n) (((n)&0xFF)<<0)

//tx_timer_control_3EH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
#define XCV_8809_PAGE0_SWITCH_ON_DELAY(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE0_PA_ON_DELAY(n) (((n)&0x3F)<<6)
#define XCV_8809_PAGE0_VGA_ON_DELAY(n) (((n)&0x3F)<<0)

//page_setting_3FH
//#define XCV_8809_PAGE0_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE0_READ_REQ  (1<<30)
//#define XCV_8809_PAGE0_RESERVED(n) (((n)&0x7FFF)<<3)
#define XCV_8809_PAGE0_PAGE(n)     (((n)&7)<<0)



// ============================================================================
// XCV_8809_PAGE1_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCV_8809_PAGE1_BASE    0xA2000100

typedef volatile struct
{
    REG32                          page_setting_00H;             //0x00000000
    REG32                          rxpll_settings_01H;           //0x00000004
    REG32                          rxpll_settings_02H;           //0x00000008
    REG32                          rxpll_settings_03H;           //0x0000000C
    REG32                          rxpll_settings_04H;           //0x00000010
    REG32                          rx_gain_settings_05H;         //0x00000014
    REG32                          rx_gain_settings_06H;         //0x00000018
    REG32                          rx_gain_settings_07H;         //0x0000001C
    REG32                          rx_gain_settings_08H;         //0x00000020
    REG32                          rx_gain_settings_09H;         //0x00000024
    REG32                          rx_gain_settings_0AH;         //0x00000028
    REG32                          thermo_map_to_rfvco_current_registers_0BH; //0x0000002C
    REG32                          thermo_map_to_rfvco_current_registers_0CH; //0x00000030
    REG32                          thermo_map_to_rfvco_div_ibit_registers_0DH; //0x00000034
    REG32                          thermo_map_to_rfvco_div_ibit_registers_0EH; //0x00000038
    REG32                          thermo_map_to_div_grp_tx_ibit_registers_0FH; //0x0000003C
    REG32                          thermo_map_to_div_grp_tx_ibit_registers_10H; //0x00000040
    REG32                          thermo_map_to_rxpll_reg_gro_bit_registers_11H; //0x00000044
    REG32                          thermo_map_to_rxpll_reg_gro_bit_registers_12H; //0x00000048
    REG32                          thermo_map_to_rxpll_reg_pres_bit_registers_13H; //0x0000004C
    REG32                          thermo_map_to_rxpll_reg_pres_bit_registers_14H; //0x00000050
    REG32                          thermo_map_to_rxpll_reg_pres_bit_registers_15H; //0x00000054
    REG32                          thermo_map_to_rxpll_reg_pres_bit_registers_16H; //0x00000058
    REG32                          thermo_map_to_rxpll_reg_rdac_bit_registers_17H; //0x0000005C
    REG32                          thermo_map_to_rxpll_reg_rdac_bit_registers_18H; //0x00000060
    REG32                          vco_cp_bit_compensation_1_19H; //0x00000064
    REG32                          vco_cp_bit_compensation_2_1AH; //0x00000068
    REG32                          vco_cp_bit_compensation_3_1BH; //0x0000006C
    REG32                          thermo_map_to_rmx_lo_div_ibit_1CH; //0x00000070
    REG32                          thermo_map_to_rmx_lo_div_ibit_1DH; //0x00000074
    REG32                          sub_module_control_signals_1EH; //0x00000078
    REG32                          dsp_signal_1FH;               //0x0000007C
    REG32                          dsp_signal_20H;               //0x00000080
    REG32                          dsp_signal_21H;               //0x00000084
    REG32                          dsp_signal_for_read_only_22H; //0x00000088
    REG32                          dsp_signal_23H;               //0x0000008C
    REG32                          dsp_signal_24H;               //0x00000090
    REG32                          dsp_signal_25H;               //0x00000094
    REG32                          dsp_signal_26H;               //0x00000098
    REG32                          dsp_signal_27H;               //0x0000009C
    REG32                          dsp_signal_28H;               //0x000000A0
    REG32                          dsp_signal_29H;               //0x000000A4
    REG32                          polarity_setting_2AH;         //0x000000A8
    REG32                          dsp_signal_2BH;               //0x000000AC
    REG32                          compensation_setting_2CH;     //0x000000B0
    REG32                          compensation_setting_2DH;     //0x000000B4
    REG32                          compensation_setting_2EH;     //0x000000B8
    REG32                          compensation_setting_2FH;     //0x000000BC
    REG32                          compensation_setting_30H;     //0x000000C0
    REG32                          compensation_setting_31H;     //0x000000C4
    REG32                          strobe_timer_32H;             //0x000000C8
    REG32                          strobe_timer_33H;             //0x000000CC
    REG32                          strobe_timer_34H;             //0x000000D0
    REG32                          digital_interface_setting_35H; //0x000000D4
    REG32                          reserved_register_36H;        //0x000000D8
    REG32                          compensate_setting_37H;       //0x000000DC
    REG32                          compensate_setting_38H;       //0x000000E0
    REG32                          compensate_setting_39H;       //0x000000E4
    REG32                          dc_cancel_1_3AH;              //0x000000E8
    REG32                          dc_cancel_2_3BH;              //0x000000EC
    REG32                          dc_cancel_3_3CH;              //0x000000F0
    REG32                          former_settings_3DH;          //0x000000F4
    REG32                          xcv_8809_page1_3EH;           //0x000000F8
    REG32                          xcv_8809_page1_3FH;           //0x000000FC
} HWP_XCV_8809_PAGE1_T;

#define hwp_xcv8809Page1           ((HWP_XCV_8809_PAGE1_T*) KSEG1(REG_XCV_8809_PAGE1_BASE))


//page_setting_00H
#define XCV_8809_PAGE1_WRITE_REQ   (1<<31)
#define XCV_8809_PAGE1_READ_REQ    (1<<30)
#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//rxpll_settings_01H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_RFPLL_BIT_850(n) (((n)&0xFF)<<10)
#define XCV_8809_PAGE1_RFPLL_R_BIT_850(n) (((n)&3)<<8)
#define XCV_8809_PAGE1_RFPLL_CP_BIT_850(n) (((n)&15)<<4)
#define XCV_8809_PAGE1_RFPLL_VCO_GAIN_BIT_850(n) (((n)&15)<<0)

//rxpll_settings_02H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_RFPLL_BIT_900(n) (((n)&0x3FFFF)<<0)

//rxpll_settings_03H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_RFPLL_BIT_1800(n) (((n)&0x3FFFF)<<0)

//rxpll_settings_04H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_RFPLL_BIT_1900(n) (((n)&0x3FFFF)<<0)

//rx_gain_settings_05H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_0(n) (((n)&0xFF)<<10)
#define XCV_8809_PAGE1_LNA_GAIN2_BIT_0(n) (((n)&3)<<8)
#define XCV_8809_PAGE1_TIA_GAIN_BIT_0(n) (((n)&3)<<6)
#define XCV_8809_PAGE1_PGA_GAIN_BIT_0(n) (((n)&3)<<4)
#define XCV_8809_PAGE1_FILTER_GAIN_BIT_0(n) (((n)&3)<<2)
#define XCV_8809_PAGE1_ADC_GAIN_BIT_0(n) (((n)&3)<<0)

//rx_gain_settings_06H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_1(n) (((n)&0x3FFFF)<<0)

//rx_gain_settings_07H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_2(n) (((n)&0x3FFFF)<<0)

//rx_gain_settings_08H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_3(n) (((n)&0x3FFFF)<<0)

//rx_gain_settings_09H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_4(n) (((n)&0x3FFFF)<<0)

//rx_gain_settings_0AH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_5(n) (((n)&0x3FFFF)<<0)

//thermo_map_to_rfvco_current_registers_0BH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_6(n) (((n)&0x3FFFF)<<0)

//thermo_map_to_rfvco_current_registers_0CH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_AGC_TABLE_7(n) (((n)&0x3FFFF)<<0)

//thermo_map_to_rfvco_div_ibit_registers_0DH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_THERMO_4BIT_8(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_THERMO_4BIT_7(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_THERMO_4BIT_6(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_THERMO_4BIT_5(n) (((n)&15)<<2)
#define XCV_8809_PAGE1_THERMO_4BIT_4(n) (((n)&3)<<0)

//thermo_map_to_rfvco_div_ibit_registers_0EH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_THERMO_4BIT_4(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_THERMO_4BIT_3(n) (((n)&15)<<12)
#define XCV_8809_PAGE1_THERMO_4BIT_2(n) (((n)&15)<<8)
#define XCV_8809_PAGE1_THERMO_4BIT_1(n) (((n)&15)<<4)
#define XCV_8809_PAGE1_THERMO_4BIT_0(n) (((n)&15)<<0)

//thermo_map_to_div_grp_tx_ibit_registers_0FH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_THERMO_7BIT_1_8(n) (((n)&0x7F)<<11)
#define XCV_8809_PAGE1_THERMO_7BIT_1_7(n) (((n)&0x7F)<<4)
#define XCV_8809_PAGE1_THERMO_7BIT_1_6(n) (((n)&15)<<0)

//thermo_map_to_div_grp_tx_ibit_registers_10H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_THERMO_7BIT_1_6(n) (((n)&7)<<15)
#define XCV_8809_PAGE1_THERMO_7BIT_1_5(n) (((n)&0x7F)<<8)
#define XCV_8809_PAGE1_THERMO_7BIT_1_4(n) (((n)&0x7F)<<1)
#define XCV_8809_PAGE1_THERMO_OFF_IDLE (1<<0)

//thermo_map_to_rxpll_reg_gro_bit_registers_11H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_MDLL_DIV_NUM_ABN_LB(n) (((n)&15)<<12)
#define XCV_8809_PAGE1_MDLL_DIV_NUM_ABN_DCS(n) (((n)&15)<<8)
#define XCV_8809_PAGE1_MDLL_DIV_NUM_ABN_PCS(n) (((n)&15)<<4)
#define XCV_8809_PAGE1_MDLL_DIV_NUM_NORMAL(n) (((n)&15)<<0)

//thermo_map_to_rxpll_reg_gro_bit_registers_12H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_PK_DET_TIME_SEL(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_PK_DET_OUT_POLARITY (1<<9)
#define XCV_8809_PAGE1_PK_SAR_CLK_FREQ_SEL(n) (((n)&3)<<7)
#define XCV_8809_PAGE1_PK_SAR_CLK_INV (1<<6)
#define XCV_8809_PAGE1_PK_DET_LPF_BW(n) (((n)&15)<<2)
#define XCV_8809_PAGE1_PEAK_DETECT_RESETN_DR (1<<1)
#define XCV_8809_PAGE1_PEAK_DETECT_RESETN_REG (1<<0)

//thermo_map_to_rxpll_reg_pres_bit_registers_13H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_PK_OUT(n)   (((n)&0x3F)<<12)
#define XCV_8809_PAGE1_PK_HIGH_TH(n) (((n)&0x3F)<<6)
#define XCV_8809_PAGE1_PK_LOW_TH(n) (((n)&0x3F)<<0)

//thermo_map_to_rxpll_reg_pres_bit_registers_14H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_PK_LAST_TIME_SEL(n) (((n)&0xFF)<<10)
#define XCV_8809_PAGE1_PK_LOW_COUNTER_TH(n) (((n)&0x3FF)<<0)

//thermo_map_to_rxpll_reg_pres_bit_registers_15H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_CURRENT_HT_LB_BLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_CURRENT_LT_LB_BLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_CURRENT_TH_LB_BLK(n) (((n)&0x3F)<<0)

//thermo_map_to_rxpll_reg_pres_bit_registers_16H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_CURRENT_HT_HB_BLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_CURRENT_LT_HB_BLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_CURRENT_TH_HB_BLK(n) (((n)&0x3F)<<0)

//thermo_map_to_rxpll_reg_rdac_bit_registers_17H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_GSM850_RX(n) (((n)&31)<<13)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_GSM900_RX(n) (((n)&31)<<8)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_DCS1800_RX(n) (((n)&31)<<3)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_PCS1900_RX(n) (((n)&7)<<0)

//thermo_map_to_rxpll_reg_rdac_bit_registers_18H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0xFFFF)<<2)
//#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_PCS1900_RX(n) (((n)&3)<<0)

//vco_cp_bit_compensation_1_19H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_VCO_CP_BIT_FACTOR_GSM850(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE1_VCO_CP_BIT_FACTOR_GSM900(n) (((n)&0x3F)<<6)
#define XCV_8809_PAGE1_VCO_CP_BIT_FACTOR_DCS1800(n) (((n)&0x3F)<<0)

//vco_cp_bit_compensation_2_1AH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_R5A_RESERVED (1<<17)
#define XCV_8809_PAGE1_CPBIT_LPFGAIN_SEL (1<<16)
#define XCV_8809_PAGE1_CPBIT_LPFGAIN_UNNUSE(n) (((n)&31)<<11)
#define XCV_8809_PAGE1_VCO_CP_BIT_FACTOR_PCS1900(n) (((n)&0x3F)<<5)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_PCS1900_TX(n) (((n)&31)<<0)

//vco_cp_bit_compensation_3_1BH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_R5B_RESERVED(n) (((n)&7)<<15)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_GSM850_TX(n) (((n)&31)<<10)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_GSM900_TX(n) (((n)&31)<<5)
#define XCV_8809_PAGE1_VCO_CP_BIT_FINE_DCS1800_TX(n) (((n)&31)<<0)

//thermo_map_to_rmx_lo_div_ibit_1CH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_THERMO_4BIT_2_8(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_THERMO_4BIT_2_7(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_THERMO_4BIT_2_6(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_THERMO_4BIT_2_5(n) (((n)&15)<<2)
#define XCV_8809_PAGE1_THERMO_4BIT_2_4(n) (((n)&3)<<0)

//thermo_map_to_rmx_lo_div_ibit_1DH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_THERMO_4BIT_2_4(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_THERMO_4BIT_2_3(n) (((n)&15)<<12)
#define XCV_8809_PAGE1_THERMO_4BIT_2_2(n) (((n)&15)<<8)
#define XCV_8809_PAGE1_THERMO_4BIT_2_1(n) (((n)&15)<<4)
#define XCV_8809_PAGE1_THERMO_4BIT_2_0(n) (((n)&15)<<0)

//sub_module_control_signals_1EH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_THERMO_7BIT_1_3(n) (((n)&0x7F)<<7)
#define XCV_8809_PAGE1_THERMO_7BIT_1_2(n) (((n)&0x7F)<<0)

//dsp_signal_1FH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_THERMO_7BIT_1_1(n) (((n)&0x7F)<<7)
#define XCV_8809_PAGE1_THERMO_7BIT_1_0(n) (((n)&0x7F)<<0)

//dsp_signal_20H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//dsp_signal_21H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_HT_LB_BLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_LT_LB_BLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_TH_LB_BLK(n) (((n)&0x3F)<<0)

//dsp_signal_for_read_only_22H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_HT_HB_BLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_LT_HB_BLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_TH_HB_BLK(n) (((n)&0x3F)<<0)

//dsp_signal_23H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_DC_CAL_DIG_TIME_SEL(n) (((n)&3)<<14)
#define XCV_8809_PAGE1_OFFSET_INIT_DELAY(n) (((n)&3)<<12)
#define XCV_8809_PAGE1_DC_CAL__MODE(n) (((n)&3)<<10)
#define XCV_8809_PAGE1_TX_DC_CAL_EN_GSM (1<<9)
#define XCV_8809_PAGE1_RXPLL_CAL_MODE (1<<8)
#define XCV_8809_PAGE1_RXPLL_INIT_DELAY(n) (((n)&7)<<5)
#define XCV_8809_PAGE1_RXPLL_OPEN_CLOSE_SEL (1<<4)
#define XCV_8809_PAGE1_RXPLL_VCOBIT_MSB_FIX (1<<3)
#define XCV_8809_PAGE1_RXPLL_CAL_OPT (1<<2)
#define XCV_8809_PAGE1_RXPLL_CNT_DELAY_SEL(n) (((n)&3)<<0)

//dsp_signal_24H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_THERMO_4BIT_3_8(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_THERMO_4BIT_3_7(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_THERMO_4BIT_3_6(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_THERMO_4BIT_3_5(n) (((n)&15)<<2)
#define XCV_8809_PAGE1_THERMO_4BIT_3_4(n) (((n)&3)<<0)

//dsp_signal_25H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_THERMO_4BIT_3_4(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_THERMO_4BIT_3_3(n) (((n)&15)<<12)
#define XCV_8809_PAGE1_THERMO_4BIT_3_2(n) (((n)&15)<<8)
#define XCV_8809_PAGE1_THERMO_4BIT_3_1(n) (((n)&15)<<4)
#define XCV_8809_PAGE1_THERMO_4BIT_3_0(n) (((n)&15)<<0)

//dsp_signal_26H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//dsp_signal_27H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_EQUFIL_TX_G_GSM_D7(n) (((n)&0xFFFF)<<0)

//dsp_signal_28H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_EQUFIL_TX_G_GSM_D9(n) (((n)&0xFFFF)<<0)

//dsp_signal_29H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_COEF_B_F(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_EQUFIL_TX_G_GSM_D10(n) (((n)&0xFFFF)<<0)

//polarity_setting_2AH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&7)<<15)
#define XCV_8809_PAGE1_FILTER_DAC1_CAL_IQ_SWAP (1<<14)
#define XCV_8809_PAGE1_FILTER_DAC2_CAL_IQ_SWAP (1<<13)
#define XCV_8809_PAGE1_CLK_RXPLL_CAL_INV_REG (1<<12)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&3)<<10)
#define XCV_8809_PAGE1_CLK_DSP_INVERT_REG (1<<9)
#define XCV_8809_PAGE1_CLK_GATING_ALWAYS_ON (1<<8)
#define XCV_8809_PAGE1_TX_PATH_DC_CAL_POLARITY (1<<7)
#define XCV_8809_PAGE1_THERMO_CAL_POLARITY (1<<6)
#define XCV_8809_PAGE1_ADC_CAL_REFI_POLARITY (1<<5)
#define XCV_8809_PAGE1_ADC_CAL_REG_POLARITY (1<<4)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&3)<<2)
#define XCV_8809_PAGE1_FILTER_DAC1_CAL_POLARITY (1<<1)
#define XCV_8809_PAGE1_FILTER_DAC2_CAL_POLARITY (1<<0)

//dsp_signal_2BH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0xFF)<<10)
#define XCV_8809_PAGE1_SELF_CAL_READY_DR (1<<9)
#define XCV_8809_PAGE1_ADC_CAL_REG_RESETN_DR (1<<8)
#define XCV_8809_PAGE1_RXPLL_CAL_RESETN_DR (1<<7)
#define XCV_8809_PAGE1_THERMO_CAL_RESETN_DR (1<<6)
#define XCV_8809_PAGE1_DC_OFFSET_CAL_FSM_RESETN_DR (1<<5)
#define XCV_8809_PAGE1_SELF_CAL_READY_REG (1<<4)
#define XCV_8809_PAGE1_ADC_CAL_REG_RESETN_REG (1<<3)
#define XCV_8809_PAGE1_RXPLL_CAL_RESETN_REG (1<<2)
#define XCV_8809_PAGE1_THERMO_CAL_RESETN_REG (1<<1)
#define XCV_8809_PAGE1_DC_OFFSET_CAL_FSM_RESETN (1<<0)

//compensation_setting_2CH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//compensation_setting_2DH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//compensation_setting_2EH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//compensation_setting_2FH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//compensation_setting_30H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_THERMO_DIFF_8(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_THERMO_DIFF_7(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_THERMO_DIFF_6(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_THERMO_DIFF_5(n) (((n)&15)<<2)
#define XCV_8809_PAGE1_THERMO_DIFF_4(n) (((n)&3)<<0)

//compensation_setting_31H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_THERMO_DIFF_4(n) (((n)&3)<<16)
#define XCV_8809_PAGE1_THERMO_DIFF_3(n) (((n)&15)<<12)
#define XCV_8809_PAGE1_THERMO_DIFF_2(n) (((n)&15)<<8)
#define XCV_8809_PAGE1_THERMO_DIFF_1(n) (((n)&15)<<4)
#define XCV_8809_PAGE1_THERMO_DIFF_0(n) (((n)&15)<<0)

//strobe_timer_32H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_CURRENT_HT_LB_NBLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_CURRENT_LT_LB_NBLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_CURRENT_TH_LB_NBLK(n) (((n)&0x3F)<<0)

//strobe_timer_33H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_CURRENT_HT_HB_NBLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_CURRENT_LT_HB_NBLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_CURRENT_TH_HB_NBLK(n) (((n)&0x3F)<<0)

//strobe_timer_34H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_HT_LB_NBLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_LT_LB_NBLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_TH_LB_NBLK(n) (((n)&0x3F)<<0)

//digital_interface_setting_35H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_HT_HB_NBLK(n) (((n)&15)<<10)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_LT_HB_NBLK(n) (((n)&15)<<6)
#define XCV_8809_PAGE1_RFVCO_VCOBUF_IBIT_TH_HB_NBLK(n) (((n)&0x3F)<<0)

//reserved_register_36H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0xFFF)<<6)
#define XCV_8809_PAGE1_THERMO_DIFF_ENABLE (1<<5)
#define XCV_8809_PAGE1_THERMO_DIFF_DR (1<<4)
#define XCV_8809_PAGE1_THERMO_DIFF_REG(n) (((n)&15)<<0)

//compensate_setting_37H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE1_MISMATCH_DIFF_ENABLE (1<<11)
//#define XCV_8809_PAGE1_THERMO_DIFF_ENABLE (1<<10)
//#define XCV_8809_PAGE1_THERMO_DIFF_DR (1<<9)
#define XCV_8809_PAGE1_MISMATCH_DIFF_DR (1<<8)
//#define XCV_8809_PAGE1_THERMO_DIFF_REG(n) (((n)&15)<<4)
#define XCV_8809_PAGE1_MISMATCH_DIFF_REG(n) (((n)&15)<<0)

//compensate_setting_38H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&31)<<13)
#define XCV_8809_PAGE1_TX_DAC_CAL_EN (1<<12)
#define XCV_8809_PAGE1_TX_DAC_BIT_I(n) (((n)&0xFFF)<<0)

//compensate_setting_39H
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE1_TX_DAC_BIT_Q(n) (((n)&0xFFF)<<0)

//dc_cancel_1_3AH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_OFFSET_VALUE(n) (((n)&0xFFF)<<6)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&7)<<3)
#define XCV_8809_PAGE1_DC_IN_I_RX_REG(n) (((n)&7)<<0)

//dc_cancel_2_3BH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_DC_IN_RX_DR (1<<17)
//#define XCV_8809_PAGE1_DC_IN_I_RX_REG(n) (((n)&0x7F)<<10)
#define XCV_8809_PAGE1_DC_IN_Q_RX_REG(n) (((n)&0x3FF)<<0)

//dc_cancel_3_3CH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
#define XCV_8809_PAGE1_DC_TX_CAL_EN (1<<17)
#define XCV_8809_PAGE1_DC_IN_I_TX_REG(n) (((n)&0x7F)<<10)
#define XCV_8809_PAGE1_DC_IN_Q_TX_REG(n) (((n)&0x3FF)<<0)

//former_settings_3DH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_OFFSET_VALUE(n) (((n)&0xFFF)<<6)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&7)<<3)
#define XCV_8809_PAGE1_DC_IN_I_TX(n) (((n)&7)<<0)

//xcv_8809_page1_3EH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page1_3FH
//#define XCV_8809_PAGE1_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE1_READ_REQ  (1<<30)
//#define XCV_8809_PAGE1_RESERVED(n) (((n)&0x3FFFF)<<0)



// ============================================================================
// XCV_8809_PAGE2_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCV_8809_PAGE2_BASE    0xA2000200

typedef volatile struct
{
    REG32                          xcv_8809_page2_00H;           //0x00000000
    REG32                          edge_dsp_setting_01H;         //0x00000004
    REG32                          edge_dsp_setting_02H;         //0x00000008
    REG32                          edge_dsp_setting_03H;         //0x0000000C
    REG32                          edge_dsp_setting_04H;         //0x00000010
    REG32                          edge_dsp_setting_05H;         //0x00000014
    REG32                          edge_dsp_setting_06H;         //0x00000018
    REG32                          edge_dsp_setting_07H;         //0x0000001C
    REG32                          edge_dsp_setting_08H;         //0x00000020
    REG32                          edge_dsp_setting_09H;         //0x00000024
    REG32                          edge_dsp_setting_0AH;         //0x00000028
    REG32                          edge_dsp_setting_0BH;         //0x0000002C
    REG32                          edge_dsp_setting_0CH;         //0x00000030
    REG32                          edge_dsp_setting_0DH;         //0x00000034
    REG32                          edge_dsp_setting_0EH;         //0x00000038
    REG32                          edge_dsp_setting_0FH;         //0x0000003C
    REG32                          edge_dsp_setting_10H;         //0x00000040
    REG32                          edge_dsp_setting_11H;         //0x00000044
    REG32                          edge_dsp_setting_12H;         //0x00000048
    REG32                          edge_dsp_setting_13H;         //0x0000004C
    REG32                          edge_dsp_setting_14H;         //0x00000050
    REG32                          edge_dsp_setting_15H;         //0x00000054
    REG32                          edge_dsp_setting_16H;         //0x00000058
    REG32                          edge_dsp_setting_17H;         //0x0000005C
    REG32                          edge_dsp_setting_18H;         //0x00000060
    REG32                          edge_dsp_setting_19H;         //0x00000064
    REG32                          edge_dsp_setting_1AH;         //0x00000068
    REG32                          edge_dsp_setting_1BH;         //0x0000006C
    REG32                          edge_dsp_setting_1CH;         //0x00000070
    REG32                          edge_dsp_setting_1DH;         //0x00000074
    REG32                          edge_dsp_setting_1EH;         //0x00000078
    REG32                          edge_dsp_setting_1FH;         //0x0000007C
    REG32                          digital_pll_setting_20H;      //0x00000080
    REG32                          digital_pll_setting_21H;      //0x00000084
    REG32                          digital_pll_setting_22H;      //0x00000088
    REG32                          digital_pll_setting_23H;      //0x0000008C
    REG32                          digital_pll_setting_24H;      //0x00000090
    REG32                          digital_pll_setting_25H;      //0x00000094
    REG32                          digital_pll_setting_26H;      //0x00000098
    REG32                          digital_pll_setting_27H;      //0x0000009C
    REG32                          digital_pll_setting_28H;      //0x000000A0
    REG32                          digital_pll_setting_29H;      //0x000000A4
    REG32                          digital_pll_setting_2AH;      //0x000000A8
    REG32                          digital_pll_setting_2BH;      //0x000000AC
    REG32                          xcv_8809_page2_2CH;           //0x000000B0
    REG32                          xcv_8809_page2_2DH;           //0x000000B4
    REG32                          xcv_8809_page2_2EH;           //0x000000B8
    REG32                          xcv_8809_page2_2FH;           //0x000000BC
    REG32                          xcv_8809_page2_30H;           //0x000000C0
    REG32                          xcv_8809_page2_31H;           //0x000000C4
    REG32                          xcv_8809_page2_32H;           //0x000000C8
    REG32                          xcv_8809_page2_33H;           //0x000000CC
    REG32                          xcv_8809_page2_34H;           //0x000000D0
    REG32                          xcv_8809_page2_35H;           //0x000000D4
    REG32                          xcv_8809_page2_36H;           //0x000000D8
    REG32                          xcv_8809_page2_37H;           //0x000000DC
    REG32                          xcv_8809_page2_38H;           //0x000000E0
    REG32                          xcv_8809_page2_39H;           //0x000000E4
    REG32                          xcv_8809_page2_3AH;           //0x000000E8
    REG32                          xcv_8809_page2_3BH;           //0x000000EC
    REG32                          xcv_8809_page2_3CH;           //0x000000F0
    REG32                          xcv_8809_page2_3DH;           //0x000000F4
    REG32                          xcv_8809_page2_3EH;           //0x000000F8
    REG32                          xcv_8809_page2_3FH;           //0x000000FC
} HWP_XCV_8809_PAGE2_T;

#define hwp_xcv8809Page2           ((HWP_XCV_8809_PAGE2_T*) KSEG1(REG_XCV_8809_PAGE2_BASE))


//xcv_8809_page2_00H
#define XCV_8809_PAGE2_WRITE_REQ   (1<<31)
#define XCV_8809_PAGE2_READ_REQ    (1<<30)
#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_01H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE2_RESERVED0_AGC_LNA(n) (((n)&0xFFFF)<<0)

//edge_dsp_setting_02H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE2_RESERVED1_AGC_LNA(n) (((n)&0xFFFF)<<0)

//edge_dsp_setting_03H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE2_RESERVED2_AGC_LNA(n) (((n)&0xFFFF)<<0)

//edge_dsp_setting_04H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE2_RESERVED3_AGC_LNA(n) (((n)&0xFFFF)<<0)

//edge_dsp_setting_05H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_06H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
#define XCV_8809_PAGE2_FREQ_TX_OFFSET(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_07H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE2_EQUFIL_TX_B1(n) (((n)&0xFFF)<<0)

//edge_dsp_setting_08H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE2_EQUFIL_TX_B2(n) (((n)&0xFFF)<<0)

//edge_dsp_setting_09H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE2_EQUFIL_TX_A1(n) (((n)&0xFFF)<<0)

//edge_dsp_setting_0AH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3F)<<12)
#define XCV_8809_PAGE2_EQUFIL_TX_A2(n) (((n)&0xFFF)<<0)

//edge_dsp_setting_0BH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&7)<<15)
#define XCV_8809_PAGE2_EQUFIL_TX_K(n) (((n)&0x7FFF)<<0)

//edge_dsp_setting_0CH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE2_FREQ_LOW_GSM850(n) (((n)&0x7F)<<7)
#define XCV_8809_PAGE2_FREQ_HIGH_GSM850(n) (((n)&0x7F)<<0)

//edge_dsp_setting_0DH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE2_FREQ_LOW_GSM900(n) (((n)&0x7F)<<7)
#define XCV_8809_PAGE2_FREQ_HIGH_GSM900(n) (((n)&0x7F)<<0)

//edge_dsp_setting_0EH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE2_FREQ_LOW_DCS1800(n) (((n)&0x7F)<<7)
#define XCV_8809_PAGE2_FREQ_HIGH_DCS1800(n) (((n)&0x7F)<<0)

//edge_dsp_setting_0FH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE2_FREQ_LOW_PCS1900(n) (((n)&0x7F)<<7)
#define XCV_8809_PAGE2_FREQ_HIGH_PCS1900(n) (((n)&0x7F)<<0)

//edge_dsp_setting_10H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_11H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_12H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_13H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_14H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_15H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_16H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_17H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_18H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_19H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_1AH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_1BH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_1CH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_1DH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_1EH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//edge_dsp_setting_1FH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_20H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_21H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_22H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_23H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_24H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_25H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_26H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_27H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_28H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_29H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_2AH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//digital_pll_setting_2BH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_2CH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_2DH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_2EH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_2FH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_30H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_31H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_32H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_33H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_34H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_35H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_36H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_37H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_38H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_39H
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_3AH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_3BH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_3CH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_3DH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_3EH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page2_3FH
//#define XCV_8809_PAGE2_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE2_READ_REQ  (1<<30)
//#define XCV_8809_PAGE2_RESERVED(n) (((n)&0x3FFFF)<<0)



// ============================================================================
// XCV_8809_PAGE3_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCV_8809_PAGE3_BASE    0xA2000300

typedef volatile struct
{
    REG32                          rtc_setting_00H;              //0x00000000
    REG32                          xcv_8809_page3_01H;           //0x00000004
    REG32                          rtc_setting_02H;              //0x00000008
    REG32                          rtc_setting_03H;              //0x0000000C
    REG32                          rtc_setting_04H;              //0x00000010
    REG32                          xcv_8809_page3_05H;           //0x00000014
    REG32                          xcv_8809_page3_06H;           //0x00000018
    REG32                          xcv_8809_page3_07H;           //0x0000001C
    REG32                          xcv_8809_page3_08H;           //0x00000020
    REG32                          xcv_8809_page3_09H;           //0x00000024
    REG32                          xcv_8809_page3_0AH;           //0x00000028
    REG32                          xcv_8809_page3_0BH;           //0x0000002C
    REG32                          xcv_8809_page3_0CH;           //0x00000030
    REG32                          xcv_8809_page3_0DH;           //0x00000034
    REG32                          xcv_8809_page3_0EH;           //0x00000038
    REG32                          rtc_setting_0FH;              //0x0000003C
    REG32                          xcv_8809_page3_10H;           //0x00000040
    REG32                          xcv_8809_page3_11H;           //0x00000044
    REG32                          xcv_8809_page3_12H;           //0x00000048
    REG32                          xcv_8809_page3_13H;           //0x0000004C
    REG32                          xcv_8809_page3_14H;           //0x00000050
    REG32                          xcv_8809_page3_15H;           //0x00000054
    REG32                          xcv_8809_page3_16H;           //0x00000058
    REG32                          xcv_8809_page3_17H;           //0x0000005C
    REG32                          xcv_8809_page3_18H;           //0x00000060
    REG32                          xcv_8809_page3_19H;           //0x00000064
    REG32                          xcv_8809_page3_1AH;           //0x00000068
    REG32                          xcv_8809_page3_1BH;           //0x0000006C
    REG32                          xcv_8809_page3_1CH;           //0x00000070
    REG32                          xcv_8809_page3_1DH;           //0x00000074
    REG32                          xcv_8809_page3_1EH;           //0x00000078
    REG32                          xcv_8809_page3_1FH;           //0x0000007C
    REG32                          xcv_8809_page3_20H;           //0x00000080
    REG32                          xcv_8809_page3_21H;           //0x00000084
    REG32                          xcv_8809_page3_22H;           //0x00000088
    REG32                          xcv_8809_page3_23H;           //0x0000008C
    REG32                          xcv_8809_page3_24H;           //0x00000090
    REG32                          xcv_8809_page3_25H;           //0x00000094
    REG32                          xcv_8809_page3_26H;           //0x00000098
    REG32                          xcv_8809_page3_27H;           //0x0000009C
    REG32                          xcv_8809_page3_28H;           //0x000000A0
    REG32                          xcv_8809_page3_29H;           //0x000000A4
    REG32                          xcv_8809_page3_2AH;           //0x000000A8
    REG32                          xcv_8809_page3_2BH;           //0x000000AC
    REG32                          xcv_8809_page3_2CH;           //0x000000B0
    REG32                          xcv_8809_page3_2DH;           //0x000000B4
    REG32                          xcv_8809_page3_2EH;           //0x000000B8
    REG32                          xcv_8809_page3_2FH;           //0x000000BC
    REG32                          xcv_8809_page3_30H;           //0x000000C0
    REG32                          xcv_8809_page3_31H;           //0x000000C4
    REG32                          xcv_8809_page3_32H;           //0x000000C8
    REG32                          xcv_8809_page3_33H;           //0x000000CC
    REG32                          xcv_8809_page3_34H;           //0x000000D0
    REG32                          xcv_8809_page3_35H;           //0x000000D4
    REG32                          xcv_8809_page3_36H;           //0x000000D8
    REG32                          xcv_8809_page3_37H;           //0x000000DC
    REG32                          xcv_8809_page3_38H;           //0x000000E0
    REG32                          xcv_8809_page3_39H;           //0x000000E4
    REG32                          xcv_8809_page3_3AH;           //0x000000E8
    REG32                          xcv_8809_page3_3BH;           //0x000000EC
    REG32                          xcv_8809_page3_3CH;           //0x000000F0
    REG32                          xcv_8809_page3_3DH;           //0x000000F4
    REG32                          xcv_8809_page3_3EH;           //0x000000F8
    REG32                          xcv_8809_page3_3FH;           //0x000000FC
} HWP_XCV_8809_PAGE3_T;

#define hwp_xcv8809Page3           ((HWP_XCV_8809_PAGE3_T*) KSEG1(REG_XCV_8809_PAGE3_BASE))


//rtc_setting_00H
#define XCV_8809_PAGE3_WRITE_REQ   (1<<31)
#define XCV_8809_PAGE3_READ_REQ    (1<<30)
#define XCV_8809_PAGE3_CAP_BANK_BIT_LP(n) (((n)&0xFF)<<10)
#define XCV_8809_PAGE3_OSC_CFIX_LP (1<<9)
#define XCV_8809_PAGE3_IBIT_XOSC_LP(n) (((n)&15)<<5)
#define XCV_8809_PAGE3_FIX_IBIT_XOSC_LP(n) (((n)&31)<<0)

//xcv_8809_page3_01H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
#define XCV_8809_PAGE3_RESERVED(n) (((n)&15)<<14)
#define XCV_8809_PAGE3_LDO_PWR_TIME_SEL(n) (((n)&7)<<11)
#define XCV_8809_PAGE3_XTAL_REG_TIME_SEL(n) (((n)&7)<<8)
#define XCV_8809_PAGE3_PD_LDO_DR   (1<<7)
#define XCV_8809_PAGE3_PD_LDO_REG(n) (((n)&3)<<5)
#define XCV_8809_PAGE3_ENABLE_CLK_6P5M_REG (1<<4)
#define XCV_8809_PAGE3_LP_MODE_EN_DR (1<<3)
#define XCV_8809_PAGE3_LP_MODE_EN_REG (1<<2)
#define XCV_8809_PAGE3_LP_MODE_H_DR (1<<1)
#define XCV_8809_PAGE3_LP_MODE_H_REG (1<<0)

//rtc_setting_02H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE3_STEP_OFFSET_NORMAL(n) (((n)&0xFF)<<8)
#define XCV_8809_PAGE3_STEP_OFFSET_LP(n) (((n)&0xFF)<<0)

//rtc_setting_03H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x1FFFF)<<1)
#define XCV_8809_PAGE3_STEP_OFFSET_UPDATE (1<<0)

//rtc_setting_04H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
#define XCV_8809_PAGE3_CLK32K_26MXTAL_EN_PMU (1<<17)
#define XCV_8809_PAGE3_CLK32K_LPO_EN_PMU (1<<16)
#define XCV_8809_PAGE3_CLK32K_XTAL_EN_PMU (1<<15)
#define XCV_8809_PAGE3_LPO_32K_PU_PMU (1<<14)
#define XCV_8809_PAGE3_PU_XTAL32K_PMU (1<<13)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0xFFF)<<1)
#define XCV_8809_PAGE3_XEN_BT_ENABLE (1<<0)

//xcv_8809_page3_05H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_06H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_07H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_08H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_09H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_0AH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_0BH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_0CH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_0DH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_0EH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//rtc_setting_0FH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x1FFFF)<<1)
#define XCV_8809_PAGE3_SOFT_RESETN_RTC (1<<0)

//xcv_8809_page3_10H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_11H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_12H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_13H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_14H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_15H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_16H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_17H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_18H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_19H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_1AH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_1BH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_1CH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_1DH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_1EH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_1FH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_20H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_21H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_22H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_23H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_24H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_25H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_26H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_27H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_28H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_29H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_2AH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_2BH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_2CH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_2DH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_2EH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_2FH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_30H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_31H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_32H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_33H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_34H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_35H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_36H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_37H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_38H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_39H
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_3AH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_3BH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_3CH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_3DH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_3EH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page3_3FH
//#define XCV_8809_PAGE3_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE3_READ_REQ  (1<<30)
//#define XCV_8809_PAGE3_RESERVED(n) (((n)&0x3FFFF)<<0)



// ============================================================================
// XCV_8809_PAGE4_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_XCV_8809_PAGE4_BASE    0xA2000400

typedef volatile struct
{
    REG32                          xcv_8809_page4_00H;           //0x00000000
    REG32                          xcv_8809_page4_01H;           //0x00000004
    REG32                          xcv_8809_page4_02H;           //0x00000008
    REG32                          xcv_8809_page4_03H;           //0x0000000C
    REG32                          xcv_8809_page4_04H;           //0x00000010
    REG32                          xcv_8809_page4_05H;           //0x00000014
    REG32                          xcv_8809_page4_06H;           //0x00000018
    REG32                          xcv_8809_page4_07H;           //0x0000001C
    REG32                          xcv_8809_page4_08H;           //0x00000020
    REG32                          xcv_8809_page4_09H;           //0x00000024
    REG32                          xcv_8809_page4_0AH;           //0x00000028
    REG32                          xcv_8809_page4_0BH;           //0x0000002C
    REG32                          xcv_8809_page4_0CH;           //0x00000030
    REG32                          xcv_8809_page4_0DH;           //0x00000034
    REG32                          xcv_8809_page4_0EH;           //0x00000038
    REG32                          xcv_8809_page4_0FH;           //0x0000003C
    REG32                          xcv_8809_page4_10H;           //0x00000040
    REG32                          xcv_8809_page4_11H;           //0x00000044
    REG32                          xcv_8809_page4_12H;           //0x00000048
    REG32                          xcv_8809_page4_13H;           //0x0000004C
    REG32                          xcv_8809_page4_14H;           //0x00000050
    REG32                          xcv_8809_page4_15H;           //0x00000054
    REG32                          xcv_8809_page4_16H;           //0x00000058
    REG32                          xcv_8809_page4_17H;           //0x0000005C
    REG32                          xcv_8809_page4_18H;           //0x00000060
    REG32                          xcv_8809_page4_19H;           //0x00000064
    REG32                          xcv_8809_page4_1AH;           //0x00000068
    REG32                          xcv_8809_page4_1BH;           //0x0000006C
    REG32                          xcv_8809_page4_1CH;           //0x00000070
    REG32                          xcv_8809_page4_1DH;           //0x00000074
    REG32                          xcv_8809_page4_1EH;           //0x00000078
    REG32                          xcv_8809_page4_1FH;           //0x0000007C
    REG32                          xcv_8809_page4_20H;           //0x00000080
    REG32                          xcv_8809_page4_21H;           //0x00000084
    REG32                          xcv_8809_page4_22H;           //0x00000088
    REG32                          xcv_8809_page4_23H;           //0x0000008C
    REG32                          xcv_8809_page4_24H;           //0x00000090
    REG32                          xcv_8809_page4_25H;           //0x00000094
    REG32                          xcv_8809_page4_26H;           //0x00000098
    REG32                          xcv_8809_page4_27H;           //0x0000009C
    REG32                          xcv_8809_page4_28H;           //0x000000A0
    REG32                          xcv_8809_page4_29H;           //0x000000A4
    REG32                          xcv_8809_page4_2AH;           //0x000000A8
    REG32                          xcv_8809_page4_2BH;           //0x000000AC
    REG32                          xcv_8809_page4_2CH;           //0x000000B0
    REG32                          xcv_8809_page4_2DH;           //0x000000B4
    REG32                          xcv_8809_page4_2EH;           //0x000000B8
    REG32                          xcv_8809_page4_2FH;           //0x000000BC
    REG32                          xcv_8809_page4_30H;           //0x000000C0
} HWP_XCV_8809_PAGE4_T;

#define hwp_xcv8809Page4           ((HWP_XCV_8809_PAGE4_T*) KSEG1(REG_XCV_8809_PAGE4_BASE))


//xcv_8809_page4_00H
#define XCV_8809_PAGE4_WRITE_REQ   (1<<31)
#define XCV_8809_PAGE4_READ_REQ    (1<<30)
#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_01H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE4_PLL_CLKOUT_EN(n) (((n)&15)<<12)
#define XCV_8809_PAGE4_PLL_CPAUX_BIT(n) (((n)&7)<<9)
#define XCV_8809_PAGE4_PLL_FILTER_IBIT(n) (((n)&7)<<6)
#define XCV_8809_PAGE4_PLL_CP_BIT(n) (((n)&7)<<3)
#define XCV_8809_PAGE4_PLL_INT_MODE (1<<2)
#define XCV_8809_PAGE4_PLL_SDM_CLK_SEL_RST (1<<1)
#define XCV_8809_PAGE4_PLL_SDM_CLK_SEL_NOR (1<<0)

//xcv_8809_page4_02H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE4_PLL_LOCK    (1<<15)
#define XCV_8809_PAGE4_PLL_SDM_CLK_TEST_EN (1<<14)
#define XCV_8809_PAGE4_PLL_REFMULTI2 (1<<13)
#define XCV_8809_PAGE4_PLL_VCO_HIGH_TEST (1<<12)
#define XCV_8809_PAGE4_PLL_VCO_LOW_TEST (1<<11)
#define XCV_8809_PAGE4_PLL_TEST_EN (1<<10)
#define XCV_8809_PAGE4_PLL_VREG_BIT(n) (((n)&15)<<6)
#define XCV_8809_PAGE4_PD_PLL_DR   (1<<5)
#define XCV_8809_PAGE4_PD_PLL_REG  (1<<4)
#define XCV_8809_PAGE4_PLL_RESET_DR (1<<3)
#define XCV_8809_PAGE4_PLL_RESET_REG (1<<2)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<0)

//xcv_8809_page4_03H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_04H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_05H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE4_BBPLL_SDM1_FREQ(n) (((n)&0xFFFF)<<0)

//xcv_8809_page4_06H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
//#define XCV_8809_PAGE4_BBPLL_SDM1_FREQ(n) (((n)&0xFFF)<<4)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&15)<<0)

//xcv_8809_page4_07H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE4_BBPLL_SDM1_INT_DEC_SEL(n) (((n)&7)<<13)
#define XCV_8809_PAGE4_BBPLL_SDM1_DITHER_BYPASS (1<<12)
#define XCV_8809_PAGE4_BBPLL_SDM1_SS_EN (1<<11)
#define XCV_8809_PAGE4_BBPLL_SDM1_SS_SQURE_TRI_SEL (1<<10)
#define XCV_8809_PAGE4_BBPLL_SDM1_DCDC2_OUTPUT_INV (1<<9)
#define XCV_8809_PAGE4_BBPLL_SDM1_DCDC3_OUTPUT_INV (1<<8)
#define XCV_8809_PAGE4_BBPLL_SDM1_RESERVED (1<<7)
#define XCV_8809_PAGE4_BBPLL_SDM1_DCDC4_ONE_MORE_DIV2 (1<<6)
#define XCV_8809_PAGE4_BBPLL_SDM1_DCDC3_ONE_MORE_DIV2 (1<<5)
#define XCV_8809_PAGE4_BBPLL_SDM1_DCDC2_ONE_MORE_DIV2 (1<<4)
#define XCV_8809_PAGE4_BBPLL_SDM1_DCDC1_ONE_MORE_DIV2 (1<<3)
#define XCV_8809_PAGE4_BBPLL_SDM1_CLK_FBC_INV (1<<2)
#define XCV_8809_PAGE4_BBPLL_SDM1_RESETN_DR (1<<1)
#define XCV_8809_PAGE4_BBPLL_SDM1_RESETN_REG (1<<0)

//xcv_8809_page4_08H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE4_BBPLL_SDM1_SS_DEVI_CT(n) (((n)&0xFF)<<8)
#define XCV_8809_PAGE4_BBPLL_SDM1_SS_PERI_CT(n) (((n)&0xFF)<<0)

//xcv_8809_page4_09H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
#define XCV_8809_PAGE4_BBPLL_SDM2_FREQ(n) (((n)&0xFFFF)<<0)

//xcv_8809_page4_0AH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_0BH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_0CH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_0DH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<14)
#define XCV_8809_PAGE4_DCDC_CLKGEN1_CLK_SEL (1<<13)
#define XCV_8809_PAGE4_DCDC_CLKGEN1_DITHER_CT(n) (((n)&7)<<10)
#define XCV_8809_PAGE4_DCDC_CLKGEN1_DIV_BASEBUCK1(n) (((n)&0xFF)<<2)
#define XCV_8809_PAGE4_DCDC_CLKGEN1_RESETN_DR (1<<1)
#define XCV_8809_PAGE4_DCDC_CLKGEN1_RESETN_REG (1<<0)

//xcv_8809_page4_0EH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<14)
#define XCV_8809_PAGE4_DCDC_CLKGEN2_CLK_SEL (1<<13)
#define XCV_8809_PAGE4_DCDC_CLKGEN2_DITHER_CT(n) (((n)&7)<<10)
#define XCV_8809_PAGE4_DCDC_CLKGEN2_DIV_BASEBUCK2_AND_BOOST(n) (((n)&0xFF)<<2)
#define XCV_8809_PAGE4_DCDC_CLKGEN2_RESETN_DR (1<<1)
#define XCV_8809_PAGE4_DCDC_CLKGEN2_RESETN_REG (1<<0)

//xcv_8809_page4_0FH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_10H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<14)
#define XCV_8809_PAGE4_SDM_RESET_TIME_SEL(n) (((n)&3)<<12)
#define XCV_8809_PAGE4_SDM_AUX_RESET_TIME_SEL(n) (((n)&3)<<10)
#define XCV_8809_PAGE4_SDMCLK_SEL_TIME_SEL(n) (((n)&3)<<8)
#define XCV_8809_PAGE4_PLL_RESET_TIME_SEL(n) (((n)&3)<<6)
#define XCV_8809_PAGE4_DCDC1_RESETN_TIME_SEL(n) (((n)&3)<<4)
#define XCV_8809_PAGE4_DCDC2_RESETN_TIME_SEL(n) (((n)&3)<<2)
#define XCV_8809_PAGE4_DCDC3_RESETN_TIME_SEL(n) (((n)&3)<<0)

//xcv_8809_page4_11H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&15)<<12)
#define XCV_8809_PAGE4_LDO_BYPASS  (1<<11)
#define XCV_8809_PAGE4_LDO_TEST_BIT(n) (((n)&3)<<9)
#define XCV_8809_PAGE4_LDO_TEST_ENABLE (1<<8)
#define XCV_8809_PAGE4_LDO_VBIT(n) (((n)&7)<<5)
#define XCV_8809_PAGE4_SEL_BG      (1<<4)
#define XCV_8809_PAGE4_PD_BG_DR    (1<<3)
#define XCV_8809_PAGE4_PD_BG_REG   (1<<2)
#define XCV_8809_PAGE4_PD_LDO_DR   (1<<1)
#define XCV_8809_PAGE4_PD_LDO_REG  (1<<0)

//xcv_8809_page4_12H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_13H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_14H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_15H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_16H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_17H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_18H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_19H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_1AH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_1BH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_1CH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_1DH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_1EH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_1FH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_20H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_21H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_22H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_23H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_24H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_25H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_26H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_27H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_28H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_29H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_2AH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_2BH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_2CH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_2DH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_2EH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_2FH
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFFF)<<0)

//xcv_8809_page4_30H
//#define XCV_8809_PAGE4_WRITE_REQ (1<<31)
//#define XCV_8809_PAGE4_READ_REQ  (1<<30)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&3)<<16)
//#define XCV_8809_PAGE4_RESERVED(n) (((n)&0x3FFF)<<2)
#define XCV_8809_PAGE4_REGISTER_RESETN_PLL (1<<1)
#define XCV_8809_PAGE4_SOFT_RESETN_PLL (1<<0)





#endif


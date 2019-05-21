/**
 ****************************************************************************************
 *
 * @file phy_trident.h
 *
 * @brief File containing the nX Trident Radio register access functions and defines.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */
#ifndef PHY_TRIDENT_H_
#define PHY_TRIDENT_H_

#define PHY_DEBUG

#ifdef PHY_DEBUG
#define PHY_PRT       os_printf
#define PHY_WPRT      os_null_printf
#else
#define PHY_PRT       os_null_printf
#define PHY_WPRT      os_null_printf
#endif

/**
 ****************************************************************************************
 * @addtogroup PHY
 * @brief This driver is specific to the TRIDENT radio board based PHY. A very light
 * control on MODEM, AGC, RF (through RC) and TRIDENT board components is grouped into
 * specific block control functions in this driver. Its content remains transparent to SW
 * in order for other drivers to be usable with the same SW. The PHY driver represents the
 * link between SW and PHY.
 *
 * !!! Trident related settings use A,B, C radio paths.
 * !!! MDM and RC related settings use paths 0,1,2
 * One register will have the mapping between the two.
 *
 * The MAXIM2829 Radio(s) are controlled through the Radio Controller.
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
//for __INLINE
#include "compiler.h"

#include "mac.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#if CFG_SUPPORT_TPC_PA_MAP
// Limit as defined per radio specifications
#define PHY_TRIDENT_MAX_PWR_24G 20 // dBm
#define PHY_TRIDENT_MIN_PWR_24G 5 // dBm

#define PHY_TRIDENT_MAX_PWR_5G 22 // dBm
#define PHY_TRIDENT_MIN_PWR_5G -9 // dBm

#define PHY_TRIDENT_PA_GAIN_24G 0 // dBm
#define PHY_TRIDENT_PA_GAIN_5G  26 // dBm

#define PHY_TRIDENT_TX_GAIN_MIN -35 // dBm

// To prevent distortion Limit pwr to 10 dBm
#define PHY_TRIDENT_LIMIT_PWR    25 // dBm
#else
// Limit as defined per radio specifications
#define PHY_TRIDENT_MAX_PWR_24G 25 // dBm
#define PHY_TRIDENT_MIN_PWR_24G -6 // dBm

#define PHY_TRIDENT_MAX_PWR_5G 22 // dBm
#define PHY_TRIDENT_MIN_PWR_5G -9 // dBm

#define PHY_TRIDENT_PA_GAIN_24G 29 // dBm
#define PHY_TRIDENT_PA_GAIN_5G  26 // dBm

#define PHY_TRIDENT_TX_GAIN_MIN -35 // dBm

// To prevent distortion Limit pwr to 10 dBm
#define PHY_TRIDENT_LIMIT_PWR    10 // dBm
#endif


///MAXIM radio SPI prescaler
#define RF_SPI_PRESCALER  2//for until debug over

///Bit offset for generic enables for Radio path A
#define RF_PATH_A     0
///Bit offset for generic enables for Radio path B
#define RF_PATH_B     1
///Bit offset for generic enables for Radio path C
#define RF_PATH_C     2

///Bit offset for generic enables for MDM+RC path 0
#define RC_PATH_0     0
///Bit offset for generic enables for MDM+RC path 1
#define RC_PATH_1     1
///Bit offset for generic enables for MDM+RC path 2
#define RC_PATH_2     2

//Upper nibble of PHY_CFG parameter of phy_init
#define MDM_TYPE_1x1  0x00
#define MDM_TYPE_1x2  0x10
#define MDM_TYPE_1x3  0x20
#define MDM_TYPE_2x3  0x30
#define MDM_TYPE_3x3  0x40
//Lower nibble of PHY_CFG parameter of phy_init
#define MDM2RF_ABC    0x00
#define MDM2RF_BCA    0x01
#define MDM2RF_CAB    0x02
#define MDM2RF_ACB    0x03
#define MDM2RF_BAC    0x04
#define MDM2RF_CBA    0x05

/*
 * CHANNEL SYNTHESIZER SETTINGS FOR RF REGISTERS
 ****************************************************************************************
 */
/// PHY driver context.
struct phy_env_tag
{
    ///Exact mapping of (MDM+RC) to Radio paths
    uint8_t phy2rf;
    ///Rf selection: b0=A, b1=B, b2=C
    uint8_t rf_path_sel;
    ///MDM+RC Path selection: b0=0, b1=1, b2=2
    uint8_t rc_path_sel;
    ///MIMO enable
    uint8_t mimo_en;
    ///Number of Rx Paths (for modem setting)
    uint8_t nb_rx_paths;
    ///Current selected band
    uint8_t band;
    //20/40/80/160/80+80 channel type
    uint8_t chnl_type;
    ///Currently configured 20MHz primary frequency (in MHz)
    uint16_t chnl_prim20_freq;
    ///Currently configured nMHz contiguous channel (of primary 80 if 80+80)
    uint16_t chnl_center1_freq;
    ///Currently configured secondary 80MHz channel if 80+80, unused otherwise
    uint16_t chnl_center2_freq;
};

/// Global PHY driver environment.
extern struct phy_env_tag phy_env[];

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
void phy_rc_rf_reg_write(uint8_t addr, uint16_t value);

/*
 * MAXIM 2829 REGISTER ACCESS
 ****************************************************************************************
 */
#define RF_DONT_CARE           0
#define RF_DEFAULT0            0
#define RF_DEFAULT1            1
#define RF_FIXED0              0
#define RF_FIXED1              1
#define RF_VCO_SPI_BANDSW_DIS  0
#define RF_VCO_BANDSW_DIS      0
#define RF_VCO_BANDSW_EN       1
#define RF_5G_LOW              0
#define RF_5G_HIGH             1

//parameters are in lowest to highest bit positions order
// STANDBY-------------------------------------------------------------------------------
#define RF_STANDBY_ADDR   0x02
#define RF_STANDBY_RST    0x1007
#define RF_STANDBY_FIXED  0x1007

__INLINE void rf_standby_set(uint8_t pa_bias, uint8_t v_ref, uint8_t mimo_sel)
{
    phy_rc_rf_reg_write(RF_STANDBY_ADDR,
                        RF_STANDBY_FIXED | (pa_bias << 10) | (v_ref << 11) | (mimo_sel << 13));
}

// INTEGER DIVIDER RATIO------------------------------------------------------------------
//no fields to define - values in table contain mandatory 0 set fields
#define RF_INTDIV_ADDR    0x03
#define RF_INTDIV_RST     0x30A2

__INLINE void rf_intdiv_set(uint16_t value)
{
    phy_rc_rf_reg_write(RF_INTDIV_ADDR, value);
}

// FRACTIONAL DIVIDER RATIO---------------------------------------------------------------
//no fields to define - LSBs of value in RF are intdiv register MSBs !
#define RF_FRACDIV_ADDR    0x04
#define RF_FRACDIV_RST     0x1DDD

__INLINE void rf_fracdiv_set(uint16_t value)
{
    phy_rc_rf_reg_write(RF_FRACDIV_ADDR, value);
}

// BAND SELECT----------------------------------------------------------------------------
//register used only in 2.4GHz band and never enable band switching, in 5GHz RC_SHADOW programs it
#define RF_BANDSEL_ADDR    0x05
#define RF_BANDSEL_RST     0x1824
#define RF_BANDSEL_FIXED   0x1800

__INLINE void rf_bandsel_set(uint8_t band, uint8_t ref_div, uint8_t mimo)
{
    phy_rc_rf_reg_write(RF_BANDSEL_ADDR,
                        RF_BANDSEL_FIXED | (band) | (ref_div << 1) | (RF_DEFAULT1 << 5) |
                        (RF_DONT_CARE << 6) | (RF_VCO_BANDSW_DIS << 7) | (RF_VCO_SPI_BANDSW_DIS << 8) |
                        (RF_DONT_CARE << 9) | (mimo << 13));
}

// CALIBRATION---------------------------------------------------------------------------
#define RF_CALIB_ADDR      0x06
#define RF_CALIB_RST       0x1C00
#define RF_CALIB_FIXED     0x0400

__INLINE void rf_calib_set(uint8_t rxcal_en, uint8_t txcal_en, uint8_t txcal_cntl)
{
    phy_rc_rf_reg_write(RF_CALIB_ADDR,
                        RF_CALIB_FIXED | (rxcal_en) | (txcal_en << 1) | (txcal_cntl << 11));
}

// LOW PASS FILTER------------------------------------------------------------------------
#define RF_LPF_ADDR        0x07
#define RF_LPF_RST         0x002A
#define RF_LPF_FIXED       0x0000

__INLINE void rf_lpf_set(uint8_t rx_cf_fine, uint8_t rx_cf_crs, uint8_t tx_cf_crs, uint8_t rssi_hbw_en)
{
    phy_rc_rf_reg_write(RF_LPF_ADDR,
                        RF_LPF_FIXED | (rx_cf_fine) | (rx_cf_crs << 3) |
                        (tx_cf_crs << 5) | (rssi_hbw_en << 11));
}

// RX CONTROL/RSSI------------------------------------------------------------------------
#define RF_RXRSSI_ADDR     0x08
#define RF_RXRSSIF_RST     0x0025
#define RF_RXRSSIF_FIXED   0x0021

__INLINE void rf_rxrssi_set(uint8_t rx_cf, uint8_t temp_out_en, uint8_t rssi_mode, uint8_t rssi_range, uint8_t rx_vga_gain_prog_en)
{
    phy_rc_rf_reg_write(RF_RXRSSI_ADDR,
                        RF_RXRSSIF_FIXED | (rx_cf << 2) | (temp_out_en << 8) |
                        (rssi_mode << 10) | (rssi_range << 11) | (rx_vga_gain_prog_en << 12));
}

// TXLINEARITY/BB GAIN--------------------------------------------------------------------
#define RF_TXLIN_ADDR     0x09
#define RF_TXLIN_RST      0x0200
#define RF_TXLIN_FIXED    0x0000

__INLINE void rf_txlin_set(uint8_t bb_gain, uint8_t upconv_lin, uint8_t vga_lin, uint8_t pa_lin, uint8_t tx_vga_gain_prog_en)
{
    phy_rc_rf_reg_write(RF_TXLIN_ADDR,
                        RF_TXLIN_FIXED | (bb_gain) | (upconv_lin << 2) |
                        (vga_lin << 6) | (pa_lin << 8) | (tx_vga_gain_prog_en << 10));
}

// PA BIAS DAC----------------------------------------------------------------------------
#define RF_PABIAS_ADDR     0x0A
#define RF_PABIAS_RST      0x03C0
#define RF_PABIAS_FIXED    0x0000

__INLINE void rf_pabias_set(uint8_t dac_o_cur, uint8_t dac_on_del)
{
    phy_rc_rf_reg_write(RF_PABIAS_ADDR, RF_PABIAS_FIXED | (dac_o_cur) | (dac_on_del << 6));
}

// RX GAIN--------------------------------------------------------------------------------
#define RF_RXGAIN_ADDR     0x0B
#define RF_RXGAIN_RST      0x007F
#define RF_RXGAIN_FIXED    0x0000
//todo check if two params or just one
__INLINE void rf_rxgain_set(uint8_t value)
{
    phy_rc_rf_reg_write(RF_RXGAIN_ADDR, RF_RXGAIN_FIXED | value);
}

// TX VGA GAIN----------------------------------------------------------------------------
#define RF_TXGAIN_ADDR     0x0C
#define RF_TXGAIN_RST      0x0000
#define RF_TXGAIN_FIXED    0x0000

__INLINE void rf_txgain_set(uint8_t value)
{
    phy_rc_rf_reg_write(RF_TXGAIN_ADDR, RF_TXGAIN_FIXED | value);
}


/**
 ****************************************************************************************
 * @brief Simple check for set bits in a value
 * @param[in] value     A one byte value in which a certain bit will be checked for its state.
 * @param[in] bit_pos   Index of the bit to check, possible values from 0 to 7
 * @return True is the bit at the indicated position is set, False otherwise.
 ****************************************************************************************
 */
__INLINE bool phy_is_bit_set(uint8_t value, uint8_t bit_pos)
{
    if ((value & (1 << bit_pos)) == (1 << bit_pos))
        return true;
    return false;
}

/**
 ****************************************************************************************
 * @brief Extraction of rf_sel and MIMO information out of MODEM type and MDM2RF path map
 * @param[in] cfg    Combined info with MDM2RF path mapping in lower nibble and  Modem nxm type
 *               in upper nibble.
 ****************************************************************************************
 */
__INLINE void phy_get_config(uint8_t cfg)
{
    uint8_t mdm_type = cfg & 0xF0;

    switch(mdm_type)
    {
    case MDM_TYPE_1x1:
    {
        phy_env->mimo_en     = 0;
        phy_env->nb_rx_paths = 1;
        phy_env->rc_path_sel = (1 << RC_PATH_0);
    }
    break;
    case MDM_TYPE_1x2:
    {
        phy_env->mimo_en     = 1;
        phy_env->nb_rx_paths = 2;
        phy_env->rc_path_sel = (1 << RC_PATH_0) | (1 << RC_PATH_1);
    }
    break;
    default:
    {
        phy_env->mimo_en     = 1;
        phy_env->nb_rx_paths = 3;
        phy_env->rc_path_sel = (1 << RC_PATH_0) | (1 << RC_PATH_1) | (1 << RC_PATH_2);
    }
    break;
    }

    switch(cfg & 0x0F)
    {
        //PHY 0,1,2 to RF A, B, C
    case MDM2RF_ABC:
        //get how many channels will be used rx+tx
        // 1x1 - A alone
        if (mdm_type == MDM_TYPE_1x1)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A);
        }
        // 1x2 - A&B
        else if (mdm_type == MDM_TYPE_1x2)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_B);
        }
        // 1x3, 2x3, 3x3 - all 3...
        else
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_B) | (1 << RF_PATH_C);
        }
        break;

        //PHY 0,1,2 to RF B, C, A
    case MDM2RF_BCA:
        //get how many channels will be used rx+tx
        // 1x1 - B alone
        if (mdm_type == MDM_TYPE_1x1)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_B);
        }
        // 1x2 - B&C
        else if (mdm_type == MDM_TYPE_1x2)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_B) | (1 << RF_PATH_C);
        }
        // 1x3, 2x3, 3x3 - all 3...
        else
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_B) | (1 << RF_PATH_C);
        }
        break;

        //PHY 0,1,2 to RF C, A, B
    case MDM2RF_CAB:
        //get how many channels will be used rx+tx
        // 1x1 - C alone
        if (mdm_type == MDM_TYPE_1x1)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_C);
        }
        // 1x2 - C&A
        else if (mdm_type == MDM_TYPE_1x2)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_C) | (1 << RF_PATH_A);
        }
        // 1x3, 2x3, 3x3 - all 3...
        else
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_B) | (1 << RF_PATH_C);
        }
        break;

        //PHY 0,1,2 to RF A, C, B
    case MDM2RF_ACB:
        //get how many channels will be used rx+tx
        // 1x1 - A alone
        if (mdm_type == MDM_TYPE_1x1)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A);
        }
        // 1x2 - A&C
        else if (mdm_type == MDM_TYPE_1x2)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_C);
        }
        // 1x3, 2x3, 3x3 - all 3...
        else
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_B) | (1 << RF_PATH_C);
        }
        break;

        //PHY 0,1,2 to RF B, A, C
    case MDM2RF_BAC:
        //get how many channels will be used rx+tx
        // 1x1 - B alone
        if (mdm_type == MDM_TYPE_1x1)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_B);
        }
        // 1x2 - B&A
        else if (mdm_type == MDM_TYPE_1x2)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_B) | (1 << RF_PATH_A);
        }
        // 1x3, 2x3, 3x3 - all 3...
        else
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_B) | (1 << RF_PATH_C);
        }
        break;

        //PHY 0,1,2 to RF C, B, A
    case MDM2RF_CBA:
        //get how many channels will be used rx+tx
        // 1x1 - C alone
        if (mdm_type == MDM_TYPE_1x1)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_B);
        }
        // 1x2 - C&B
        else if (mdm_type == MDM_TYPE_1x2)
        {
            phy_env->rf_path_sel = (1 << RF_PATH_C) | (1 << RF_PATH_B);
        }
        // 1x3, 2x3, 3x3 - all 3...
        else
        {
            phy_env->rf_path_sel = (1 << RF_PATH_A) | (1 << RF_PATH_B) | (1 << RF_PATH_C);
        }
        break;
    default:
        break;
    }
}

extern void phy_init_after_wakeup(void);
extern void phy_disable_lsig_intr(void);
extern void phy_enable_lsig_intr(void);

/// @}
#endif /* PHY_TRIDENT_H_ */



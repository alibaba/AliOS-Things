/**
 ****************************************************************************************
 *
 * @file phy.h
 *
 * @brief Header file for nX Radio physical layer driver.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */
#ifndef _PHY_H_
#define _PHY_H_

/*
 ****************************************************************************************
 * @defgroup PHY PHY
 * @ingroup PLATFORM_DRIVERS
 * @brief Declaration of the PHY Drivers
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// default inclusion directives
#include "co_int.h"
#include "rwnx_config.h"

//for __INLINE
#include "compiler.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Invalid value for some octet parameters reserved for the future
#define PHY_UNUSED           0xFF

/// Maximum number of words in the configuration buffer
#define PHY_CFG_BUF_SIZE     16

enum
{
    /// 2.4GHz Band
    PHY_BAND_2G4,
    /// 5GHz band
    PHY_BAND_5G,
    /// Number of bands
    PHY_BAND_MAX,
};

enum
{
    PHY_CHNL_BW_20,
    PHY_CHNL_BW_40,
    PHY_CHNL_BW_80,
    PHY_CHNL_BW_160,
    PHY_CHNL_BW_80P80,
    PHY_CHNL_BW_OTHER,
};

enum
{
    /// Primary radar detection chain (i.e for the operating channel)
    PHY_PRIM,
    /// Secondary radar detection chain
    PHY_SEC,
};

/// Structure containing the information about the PHY channel that was used for this RX
struct phy_channel_info
{
    /// PHY channel information 1
    uint32_t info1;
    /// PHY channel information 2
    uint32_t info2;
};

struct phy_radar_pulse
{
    /// In our PHY a radar pulse is only one 32-bit word
    uint32_t pulse;
};

/// Structure containing the parameters of the PHY configuration
struct phy_cfg_tag
{
    /// Buffer containing the parameters specific for the PHY used
    uint32_t parameters[PHY_CFG_BUF_SIZE];
};


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Modem and Radio initialization function.
 * This function is called at platform reset and initializes modem, then radio.
 * @param[in] cfg     Pointer to the PHY parameters (used radio paths, other cfg)
 ****************************************************************************************
 */
void phy_init(const struct phy_cfg_tag *config);

/**
 ****************************************************************************************
 * @brief Resets some parts of the PHY.
 * This function is called upon a system reset during the recovery procedure.
 ****************************************************************************************
 */
void phy_reset(void);


/**
 ****************************************************************************************
 * @brief PHY version reading function.
 * This function returns the PHY version on two 32-bit words. The format of these two
 * words is PHY dependent.
 *
 * @param[out] version_1  Pointer to the 1st version word
 * @param[out] version_2  Pointer to the 2nd version word
 ****************************************************************************************
 */
void phy_get_version(uint32_t *version_1, uint32_t *version_2);

/**
 ****************************************************************************************
 * @brief Set channel function.
 * This function is called when the MAC stack needs to change the channel on which the
 * radio is locked.
 * The function is blocking until the PLL has locked on the new channel frequency.
 *
 * @param[in] band          Band for which the channel index is specified
 * @param[in] type          Channel type for 20/40/80/160/80+80 or other
 * @param[in] prim20_freq   Primary 20MHz channel frequency (in MHz)
 * @param[in] center1_freq  Center frequency of whole channel (when contiguous),
 *                          =prim20_freq for a 20MHz channel. Center or primary 80MHz channel
 *                          if 80+80 type
 * @param[in] center2_freq  Center frequency of secondary 80MHz channel if 80+80
 * @param[in] index         Index of the RF for which the channel has to be set (@PHY_PRIM:
 *                          operating (primary RF)), 1: secondary RF (@PHY_SEC: used for
 *                          additional radar detection). This parameter is reserved if no
 *                          secondary RF is available in the system
 ****************************************************************************************
 */
void phy_set_channel(uint8_t band, uint8_t type, uint16_t prim20_freq,
                     uint16_t center1_freq, uint16_t center2_freq, uint8_t index);

/**
 ****************************************************************************************
 * @brief Get channel function.
 * This function is called when the MAC stack needs to get the information on the channel
 * on which the radio is currently locked.
 *
 * @param[out] info        Pointer on the phy_channel_info structure used to return the
 *                         information
 * @param[in] index        Index of the RF for which the channel has to be retrieved
 *                         (@PHY_PRIM or @PHY_SEC)
 *
 ****************************************************************************************
 */
void phy_get_channel(struct phy_channel_info *info, uint8_t index);

/**
 ****************************************************************************************
 * @brief PHY stop function.
 * This function is called when the MAC stack needs to stop the PHY.
 * It puts back the MAC HW to the IDLE state
 ****************************************************************************************
 */
void phy_stop(void);

/**
 ****************************************************************************************
 * @brief Compute the channel number from its center frequency and its band
 * @param[in] band  RF band (must be @ref PHY_BAND_2G4 or @ref PHY_BAND_5G)
 * @param[in] freq  Center frequency of the channel
 ****************************************************************************************
 */
__INLINE int phy_freq_to_channel(uint8_t band, uint16_t freq)
{
    int channel = 0;

    do
    {
        //2.4.GHz
        if (band == PHY_BAND_2G4)
        {
            // Check if frequency is in the expected range
            if ((freq < 2412) || (freq > 2484))
                break;

            // Compute the channel number
            if (freq == 2484)
                channel = 14;
            else
                channel = (freq - 2407) / 5;
        }
        //5 GHz
        else if (band == PHY_BAND_5G)
        {
            // Check if frequency is in the expected range
            if ((freq < 5005) || (freq > 5825))
                break;

            // Compute the channel number
            channel = (freq - 5000) / 5;
        }
    }
    while(0);

    return (channel);
}

/**
 ****************************************************************************************
 * @brief Compute the channel frequency from its number and band
 * @param[in] band    RF band (must be @ref PHY_BAND_2G4 or @ref PHY_BAND_5G)
 * @param[in] channel Channel number
 ****************************************************************************************
 */
__INLINE uint16_t phy_channel_to_freq(uint8_t band, int channel)
{
    uint16_t freq = (uint16_t) - 1;

    do
    {
        //2.4.GHz
        if (band == PHY_BAND_2G4)
        {
            // Check if the channel number is in the expected range
            if ((channel < 1) || (channel > 14))
                break;

            // Compute the channel number
            if (channel == 14)
                freq = 2484;
            else
                freq = 2407 + channel * 5;
        }
        //5 GHz
        else if (band == PHY_BAND_5G)
        {
            // Check if frequency is in the expected range
            if ((channel < 1) || (channel > 165))
                break;

            // Compute the channel number
            freq = 5000 + channel * 5;
        }
    }
    while(0);

    return (freq);
}

/**
 ****************************************************************************************
 * @brief Modem interrupt service routine.
 * This function is called when a Modem interrupt is asserted to the interrupt controller
 ****************************************************************************************
 */
void phy_mdm_isr(void);

/**
 ****************************************************************************************
 * @brief RC interrupt service routine.
 * This function is called when a RC interrupt is asserted to the interrupt controller
 ****************************************************************************************
 */
void phy_rc_isr(void);

/**
 ****************************************************************************************
 * @brief Indicates if the PHY has radar pulses available or not.
 *
 * @param[in] rd_idx Index of the radar detection chain that needs to be checked
 * (@PHY_PRIM or @PHY_SEC)
 *
 * @return true if pulse(s) is(are) available, false otherwise
 *
 ****************************************************************************************
 */
bool phy_has_radar_pulse(int rd_idx);

/**
 ****************************************************************************************
 * @brief Read a radar pulse from the FIFO, and writes it to the buffer passed as parameter
 *
 * @param[in] rd_idx Index of the radar detection chain that needs to be read
 * (@PHY_PRIM or @PHY_SEC)
 * @param[out] pulse Pointer to the pulse structure where to write to
 *
 * @return true if a pulse has been written, false otherwise
 *
 ****************************************************************************************
 */
bool phy_get_radar_pulse(int rd_idx, struct phy_radar_pulse *pulse);

/**
 ****************************************************************************************
 * @brief Return the Beamformee capability of the PHY
 *
 * @return true if Beamformee is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_bfmee_supported(void);

/**
 ****************************************************************************************
 * @brief Return the Beamformer capability of the PHY
 *
 * @return true if Beamformer is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_bfmer_supported(void);

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO RX capability of the PHY
 *
 * @return true if MU-MIMO RX is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_mu_mimo_rx_supported(void);

/**
 ****************************************************************************************
 * @brief Return the MU-MIMO TX capability of the PHY
 *
 * @return true if MU-MIMO TX is supported by the PHY, false otherwise
 ****************************************************************************************
 */
bool phy_mu_mimo_tx_supported(void);

#if RW_MUMIMO_RX_EN
/**
 ****************************************************************************************
 * @brief Set the MU-MIMO RX group information as received in the VHT Group ID action frame
 *
 * @param[in] membership_addr  Address of the membership status in the received frame
 * @param[in] userpos_addr     Address of the user position array in the received frame
 ****************************************************************************************
 */
void phy_set_group_id_info(uint32_t membership_addr, uint32_t userpos_addr);
#endif

/**
 ****************************************************************************************
 * @brief Return the NSS of the PHY
 *
 * @return Number of Spatial Streams supported by the PHY (start from 0, 0 is for 1 SS)
 ****************************************************************************************
 */
uint8_t phy_get_nss(void);

/**
 ****************************************************************************************
 * @brief Return the NTX of the PHY
 *
 * @return Number of antenna supported for TX minus 1 (i.e. 0 means 1 antenna)
 ****************************************************************************************
 */
uint8_t phy_get_ntx(void);

/**
 ****************************************************************************************
 * @brief Get radio code to set a specific TX power
 * Return the idx to use (set in policy table), to conigure a specific TX power. If the TX
 * power requested is not possible, it should return idx to get the maximum TX power that
 * do not  exceed the request. In this case, power parameter should be updated tieh the
 * actual TX power set.
 *
 * @param[in/out] power TX power (in dBm) to configure for the radio, (updated with
 *                      the actual power set if request is not possible)
 * @param[out]    idx   Idx to use in policy table to configure a tx power of <gain>
 ****************************************************************************************
 */
void phy_get_rf_gain_idx(int8_t *power, uint8_t *idx);


/**
 * @brief Get radio's transmission power capabilities
 *
 * @param[out] max Maximum TX power for the radio (in dBm)
 * @param[out] min Minimum TX power for the radio (in dBm)
 */
void phy_get_rf_gain_capab(int8_t *max, int8_t *min);

#if NX_DEBUG_DUMP
/**
 ****************************************************************************************
 * @brief Read the state of all PHY diagnostic ports and put it in the debug dump info
 * structure
 *
 * @param[out] dbg_info Pointer to the debug dump information structure
 *
 ****************************************************************************************
 */
void phy_get_diag_state(struct dbg_debug_info_tag *dbg_info);
#endif

/// @}

#endif //_PHY_H_

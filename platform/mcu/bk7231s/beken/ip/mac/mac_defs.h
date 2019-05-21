/**
 ****************************************************************************************
 *
 * @file mac_defs.h
 *
 * @brief Protocol related definitions used in MAC SW implementation.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _MAC_DEFS_H_
#define _MAC_DEFS_H_

/**
 ****************************************************************************************
 * @defgroup CO_MAC_DEF CO_MAC_DEF
 * @ingroup CO_MAC
 * @brief  Common defines,structures
 *
 * This module contains defines commonaly used for MAC
 * @{
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */
/// POWER SAVE MODES.
enum
{
    /// power save mode active.
    POWER_MODE_ACTIVE,
    /// power save mode passive
    POWER_MODE_PASSIVE,
    /// power save mode fast
    POWER_MODE_FAST
};

/// PHY Types
enum
{
    /// phy type none
    MAC_PHYTYPE_NONE = 0,
    MAC_PHYTYPE_FHSS,
    /// 802.11
    MAC_PHYTYPE_DSSS,
    MAC_PHYTYPE_IR,
    /// 802.11a
    MAC_PHYTYPE_OFDM,
    /// 802.11b
    MAC_PHYTYPE_HRDSSS,
    /// 802.11g
    MAC_PHYTYPE_ERP,
    /// 802.11n
    MAC_PHYTYPE_HT
};

/// operating frequency
enum
{
    /// 5 GHz
    MAC_OP_FREQUENCY_5,
    /// 2.4 GHz
    MAC_OP_FREQUENCY_2_4
};

/// preamble policy in b
enum
{
    MAC_PREAMBLE_SHORT_STRICT       = 0,
    MAC_PREAMBLE_SHORT_ONLY         = 1,
    MAC_PREAMBLE_PERFORMANCE        = 3,
    MAC_PREAMBLE_COMPATIBLE         = 4,
    MAC_PREAMBLE_LONG_ONLY          = 5
};

/// slot policy in b/g
enum
{
    MAC_SLOT_SHORT_STRICT       = 0,
    MAC_SLOT_SHORT_ONLY         = 1,
    MAC_SLOT_PERFORMANCE_IBSS   = 2,
    MAC_SLOT_PERFORMANCE        = 3,
    MAC_SLOT_COMPATIBLE         = 4,
    MAC_SLOT_LONG_ONLY          = 5
};

/// Block Ack Policy type
enum
{
    BA_DELAYED     = 0,    /// Delayed BA Policy
    BA_IMMEDIATE ,         /// Immediate BA Policy
    BA_ACK_NONE
};

/// Block Ack device type
enum
{
    BA_RESPONDER = 0,    /// BA Responder
    BA_ORIGINATOR ,      /// BA Originator
    BA_DEV_NONE
};

///Traffic ID direction enumeration
enum
{
    TID_DIR_UP_LINK,
    TID_DIR_DOWN_LINK,
    TID_DIR_DIRECT_LINK,
    TID_DIR_BIDIRECTION_LINK
};


/// Security Mode
enum
{
    ENC_WEP,
    ENC_WPA,
    ENC_WPA2,
    ENC_NONE
};

/// BSS type
enum
{
    INFRASTRUCTURE_MODE = 1,
    INDEPENDENT_BSS_MODE,
    ANY_BSS_MODE
};

/// Station mode
enum
{
    STA_NONE = 0, // STA is not yet started
    AP , // access point
    STA_ESS, // station joined BSS network
    STA_IBSS_OWNER, // station created IBSS
    STA_IBSS,  // station joined IBSS
};

/// protection policy
enum
{
    MAC_PROTECT_ERP_STRICT  = 0,
    MAC_PROTECT_ERP_ONLY    = 1,
    MAC_PROTECT_PERFORMANCE = 3,
    MAC_PROTECT_COMPATIBLE  = 4,
    MAC_PROTECT_FORCED      = 5
};

/// HT protection policy
enum
{
    MAC_HT_NO_PROTECTION            = 0,
    MAC_HT_NONMEMB_PROTECTION       = 1,
    MAC_HT_20MHZ_PROTECTION         = 2,
    MAC_HT_NONHTMIXED_PROTECTION    = 3,
};

/// @}

#endif // _MAC_DEFS_H_

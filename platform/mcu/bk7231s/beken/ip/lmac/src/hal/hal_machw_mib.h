/**
 ****************************************************************************************
 *
 * @file hal_machw_mib.h
 *
 * @brief MACHW MIB structure
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _HAL_MIB_H_
#define _HAL_MIB_H_

/**
 *****************************************************************************************
 * @defgroup MACHW_MIB MACHW_MIB
 * @ingroup HAL
 * @brief MACHW MIB structure
 * @{
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "co_int.h"

/*
 * DEFINES
 ****************************************************************************************
 */
/// Simulated MIB structure
struct machw_mib_tag
{
    /// MIB element to count the number of unencrypted frames that have been discarded
    uint32_t dot11_wep_excluded_count;
    /// MIB element to count the receive FCS errors
    uint32_t dot11_fcs_error_count;
    /**
      * MIB element to count the number of PHY Errors reported during a receive
      * transaction.
      */
    uint32_t nx_rx_phy_error_count;
    /// MIB element to count the number of times the receive FIFO has overflowed
    uint32_t nx_rd_fifo_overflow_count;
    /**
      * MIB element to count the number of times underrun has occured on the
      * transmit side
      */
    uint32_t nx_tx_underun_count;
    /// Reserved
    uint32_t reserved_1[7];
    /// MIB element to count unicast transmitted MPDU
    uint32_t nx_qos_utransmitted_mpdu_count[8];
    /// MIB element to count group addressed transmitted MPDU
    uint32_t nx_qos_gtransmitted_mpdu_count[8];
    /**
      * MIB element to count the number of MSDUs or MMPDUs discarded because of
      * retry-limit reached
      */
    uint32_t dot11_qos_failed_count[8];
    /**
      * MIB element to count number of unfragmented MSDU's or MMPDU's transmitted
      * successfully after 1 or more transmission
      */
    uint32_t dot11_qos_retry_count[8];
    /// MIB element to count number of successful RTS Frame transmission
    uint32_t dot11_qos_rts_success_count[8];
    /// MIB element to count number of unsuccessful RTS Frame transmission
    uint32_t dot11_qos_rts_failure_count[8];
    /// MIB element to count number of MPDU's not received ACK
    uint32_t nx_qos_ack_failure_count[8];
    /// MIB element to count number of unicast MPDU's received successfully
    uint32_t nx_qos_ureceived_mpdu_count[8];
    /// MIB element to count number of group addressed MPDU's received successfully
    uint32_t nx_qos_greceived_mpdu_count[8];
    /**
      * MIB element to count the number of unicast MPDUs not destined to this device
      * received successfully.
      */
    uint32_t nx_qos_ureceived_other_mpdu[8];
    /// MIB element to count the number of MPDUs received with retry bit set
    uint32_t dot11_qos_retries_received_count[8];
    /**
      * MIB element to count the number of unicast A-MSDUs that were transmitted
      * successfully
      */
    uint32_t nx_utransmitted_amsdu_count[8];
    /**
      * MIB element to count the number of group-addressed A-MSDUs that were
      * transmitted successfully
      */
    uint32_t nx_gtransmitted_amsdu_count[8];
    /// MIB element to count number of AMSDU's discarded because of retry limit reached
    uint32_t dot11_failed_amsdu_count[8];
    /// MIB element to count number of A-MSDU's transmitted successfully with retry
    uint32_t dot11_retry_amsdu_count[8];
    /**
      * MIB element to count number of bytes of an A-MSDU that was
      * transmitted successfully
      */
    uint32_t dot11_transmitted_octets_in_amsdu[8];
    /**
      * MIB element to counts the number of A-MSDUs that did not receive an ACK frame
      * successfully in response
      */
    uint32_t dot11_amsdu_ack_failure_count[8];
    /// MIB element to count number of unicast A-MSDUs received successfully
    uint32_t nx_ureceived_amsdu_count[8];
    /// MIB element to count number of group addressed A-MSDUs received successfully
    uint32_t nx_greceived_amsdu_count[8];
    /**
      * MIB element to count number of unicast A-MSDUs not destined to this device
      * received successfully
      */
    uint32_t nx_ureceived_other_amsdu[8];
    /// MIB element to count number of bytes in an A-MSDU is received
    uint32_t dot11_received_octets_in_amsdu_count[8];
    /// Reserved
    uint32_t reserved_2[24];
    /// MIB element to count number of A-MPDUs transmitted successfully
    uint32_t dot11_transmitted_ampdu_count;
    /// MIB element to count number of MPDUs transmitted in an A-MPDU
    uint32_t dot11_transmitted_mpdus_in_ampdu_count ;
    /// MIB element to count the number of bytes in a transmitted A-MPDU
    uint32_t dot11_transmitted_octets_in_ampdu_count ;
    /// MIB element to count number of unicast A-MPDU's received
    uint32_t wnlu_ampdu_received_count;
    /// MIB element to count number of group addressed A-MPDU's received
    uint32_t nx_gampdu_received_count;
    /**
      * MIB element to count number of unicast A-MPDUs received not destined
      * to this device
      */
    uint32_t nx_other_ampdu_received_count ;
    /// MIB element to count number of MPDUs received in an A-MPDU
    uint32_t dot11_mpdu_in_received_ampdu_count;
    /// MIB element to count number of bytes received in an A-MPDU
    uint32_t dot11_received_octets_in_ampdu_count;
    /// MIB element to count number of CRC errors in MPDU delimeter of and A-MPDU
    uint32_t dot11_ampdu_delimiter_crc_error_count;
    /**
      * MIB element to count number of implicit BAR frames that did not received
      * BA frame successfully in response
      */
    uint32_t dot11_implicit_bar_failure_count;
    /**
      * MIB element to count number of explicit BAR frames that did not received
      * BA frame successfully in response
      */
    uint32_t dot11_explicit_bar_failure_count;
    /// Reserved
    uint32_t reserved_3[5];
    /// MIB element to count the number of frames transmitted at 20 MHz BW
    uint32_t dot11_20mhz_frame_transmitted_count;
    /// MIB element to count the number of frames transmitted at 40 MHz BW
    uint32_t dot11_40mhz_frame_transmitted_count;
    /// MIB element to count the number of frames received at 20 MHz BW
    uint32_t dot11_20mhz_frame_received_count;
    /// MIB element to count the number of frames received at 40 MHz BW
    uint32_t dot11_40mhz_frame_received_count;
    /// MIB element to count the number of attempts made to acquire a 40 MHz TXOP
    uint32_t nx_failed_40mhz_txop;
    /// MIB element to count the number of successful TXOPs
    uint32_t nx_successful_txops;
    /// Reserved
    uint32_t reserved_4[4];
    /// MIB element to count the number of times the dual CTS fails
    uint32_t dot11_dualcts_success_count;
    /**
      * MIB element to count the number of times the AP does not detect a collision
      * PIFS after transmitting a STBC CTS frame
      */
    uint32_t dot11_stbc_cts_success_count;
    /**
      * MIB element to count the number of times the AP detects a collision PIFS after
      * transmitting a STBC CTS frame
      */
    uint32_t dot11_stbc_cts_failure_count;
    /**
      * MIB element to count the number of times the AP does not detect a collision PIFS
      * after transmitting a non-STBC CTS frame
      */
    uint32_t dot11_non_stbc_cts_success_count;
    /**
      * MIB element to count the number of times the AP detects a collision PIFS after
      * transmitting a non-STBC CTS frame
      */
    uint32_t dot11_non_stbc_cts_failure_count;

};


/// @}


#endif //_HAL_MIB_H_

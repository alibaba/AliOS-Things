/**
 ****************************************************************************************
 *
 * @file rwnx.h
 *
 * @brief Main nX MAC definitions.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @mainpage RW-WLAN-nX MAC SW project index page
 *
 * @section intro_sec Introduction
 *
 * RW-WLAN-nX is the RivieraWaves 802.11a/b/g/n/ac IP.\n
 * This document presents the Low Level Design of the RW-WLAN-nX MAC SW.\n
 * A good entry point to this document is the @ref MACSW "MACSW" module page.
 ****************************************************************************************
 */

#ifndef _RWNXL_H_
#define _RWNXL_H_

#include "ke_msg.h"
#include "lwip/pbuf.h"

#define RW_BAK_REG_LEN               (103)

typedef struct rw_rx_info_st {
    /// Index of the station that sent the frame. 0xFF if unknown.
    uint8_t sta_idx;
    /// Index of the VIF that received the frame. 0xFF if unknown.
    uint8_t vif_idx;
    /// Index of the destination STA, if known (AP mode only)
    uint8_t dst_idx;
    /// RSSI of the received frame.
    int8_t rssi;
    
    /// Center frequency on which we received the packet
    uint16_t center_freq;    
    /// Length of the frame
    uint16_t length;

    /// Frame payload.
    void *data;    
} RW_RXIFO_ST, *RW_RXIFO_PTR;

typedef struct rw_tx_info_st {
    /// Index of the station that sent the frame. 0xFF if unknown.
    uint8_t sta_idx;
    /// Index of the VIF that received the frame. 0xFF if unknown.
    uint8_t vif_idx;
    /// Length of the frame
    uint16_t length;

    /// Frame payload.
    void *data;    
} RW_TXIFO_ST, *RW_TXIFO_PTR;

typedef UINT32 (*pf_msg_outbound)(struct ke_msg *msg);
typedef UINT32 (*pf_data_outbound)(RW_RXIFO_PTR rx_info);
typedef UINT32 (*pf_rx_alloc)(struct pbuf **p, UINT32 len);
typedef UINT32 (*pf_get_rx_valid_status)(void);
typedef void (*pf_tx_confirm)(void);

typedef struct _rw_connector_
{
    pf_msg_outbound msg_outbound_func;
    pf_data_outbound data_outbound_func;
    pf_rx_alloc rx_alloc_func;
    pf_get_rx_valid_status get_rx_valid_status_func;
    pf_tx_confirm tx_confirm_func;
}RW_CONNECTOR_T, *RW_CONNECTOR_PTR;

/*
 * STRUCT DEFINITIONS
 ****************************************************************************************
 */
#if NX_POWERSAVE
/// RWNX Context
struct rwnx_env_tag
{
    /// Previous state of the HW, prior to go to DOZE
    uint8_t prev_hw_state;
    /// Flag indicating if the HW is in Doze, when waking-up
    bool hw_in_doze;
};
#endif

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */
#if NX_POWERSAVE
extern struct rwnx_env_tag rwnx_env;
#endif
extern RW_CONNECTOR_T g_rwnx_connector;
/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @defgroup MACSW MACSW
 * @brief RW-WLAN-nX root module.
 * @{
 ****************************************************************************************
 */

void rwnxl_init(void);

/**
 ****************************************************************************************
 * @brief NX reset event handler.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It performs the full LMAC reset, and restarts the operation.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void rwnxl_reset_evt(int dummy);

#if NX_POWERSAVE
bool rwnxl_get_status_in_doze(void);
/**
 ****************************************************************************************
 * @brief This function performs all the initializations of the MAC SW.
 *
 * It first initializes the heap, then the message queues and the events. Then if required
 * it initializes the trace.
 *
 ****************************************************************************************
 */
typedef void (*IDLE_FUNC)(void);
extern bool rwnxl_sleep(IDLE_FUNC wait_func,IDLE_FUNC do_func);
extern void rwnxl_wakeup(IDLE_FUNC wait_func);
extern void rwnxl_set_nxmac_timer_value(void);
extern void wifi_mac_state_set_idle(void);
extern void wifi_mac_state_set_active(void);
extern void wifi_mac_state_set_prev(void);

#endif

extern void rwnxl_register_connector(RW_CONNECTOR_T *intf);

/// @}
#endif // _RWNXL_H_


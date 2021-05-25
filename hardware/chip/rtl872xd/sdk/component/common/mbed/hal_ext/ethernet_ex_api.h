/* mbed Microcontroller Library
 *******************************************************************************
 * Copyright (c) 2015, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#ifndef ETHERNET_EX_API_H
#define ETHERNET_EX_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup ethernet_ex ETHERNET_EX
 *  @ingroup    hal
 *  @brief      ethernet extended functions
 *  @{
 */
#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
/// the selection of Ethernet pinmux
#define ETH_PIN_SEL         (Eth_Pin_Sel0)
/// the selection of the interface between MAC and PHY
#define ETH_IF_SEL          (Eth_If_Mii)
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"
/// the size (unit: Bytes) of each Tx descriptor
#define ETH_TX_DESC_SIZE	20  // 20 Bytes
/// the size (unit: Bytes) of each Rx descriptor
#define ETH_RX_DESC_SIZE	16  // 16 Bytes
/// the size of the packet buffer
#define ETH_PKT_BUF_SIZE	1600

#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
/**
  \brief  Defines Ethernet Pin mux selection.
*/
typedef enum {
    Eth_Pin_Sel0 = EthPinSel0,
    Eth_Pin_Sel1 = EthPinSel1
} EthPinmuxSel;

/**
  \brief  Defines the interface between MAC and PHY.
*/
typedef enum {
    Eth_If_Mii = EthMiiMode,
    Eth_If_Rmii = EthRmiiMode
} EthInterfaceSel;

/**
  \brief  Defines the link speed.
*/
typedef enum {
    Eth_Spd_Auto    = -1,
    Eth_Spd_10M     = 0,
    Eth_Spd_100M    = 1
} EthSpeedSel;

/**
  \brief  Defines the duplex mode.
*/
typedef enum {
    Eth_Duplex_Auto = -1,
    Eth_Duplex_Half = 0,
    Eth_Duplex_Full = 1
} EthDuplexMode;
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"


/**
  \brief  The function type of the Ethernet MAC controller interrupt callback function.
*/
typedef void (*ethernet_callback)(uint32_t event, uint32_t data);

#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
/**
  \brief  The function type of the OS task yield callback function.
*/
typedef void (*ethernet_task_yield)(void);
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"
/** 
 *  @brief To hook a callback function for Ethernet MAC controller interrupt.
 *
 *  @param[in]  callback The callback function.
 *
 *  @returns    void
 */
void ethernet_irq_hook(ethernet_callback callback);

#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
/** 
 *  @brief To hook a callback function to make OS do a context-switch while waiting.
 *
 *  @param[in]  task_yield The callback function.
 *
 *  @returns    void.
 */
void ethernet_task_yield_hook (ethernet_task_yield task_yield);
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"

/** 
 *  @brief To set the Tx/Rx descriptor number.
 *
 *  @param[in]  txdescCnt The specified Tx descriptor number.
 *  @param[in]  rxdescCnt The specified Rx descriptor number.
 *
 *  @returns    void.
 */
void ethernet_set_descnum(uint8_t txdescCnt, uint8_t rxdescCnt);

/** 
 *  @brief To set the start address of Tx/Rx descriptor ring and Tx/Rx packet buffer.
 *
 *  @param[in]  TxDescAddr The start address of Tx descriptor ring.
 *  @param[in]  RxDescAddr The start address of Rx descriptor ring.
 *  @param[in]  pTxPktBuf The start address of Tx packet buffer.
 *  @param[in]  pRxPktBuf The start address of Rx packet buffer.
 *
 *  @returns    void.
 */
void ethernet_trx_pre_setting(uint8_t *TxDescAddr, uint8_t *RxDescAddr, uint8_t *pTxPktBuf, uint8_t *pRxPktBuf);

#if defined(CONFIG_PLATFORM_8195BHP) && (CONFIG_PLATFORM_8195BHP == 1)
///@name AmebaPro Only 
///@{
/** 
 *  @brief To set the ethernet MAC address.
 *
 *  @param[in]  mac The specified MAC address.
 *
 *  @returns    void.
 */
void ethernet_set_address(char *mac);
///@}
#endif  // end of "#if defined(CONFIG_PLATFORM_8195BHP)"

#ifdef __cplusplus
}
#endif

#endif  // #ifndef ETHERNET_EX_API_H


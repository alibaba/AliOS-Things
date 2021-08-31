#ifndef DBG_TRC_CONFIG_H_
#define DBG_TRC_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup TRACER
 * @{
 * @name Tracer module configuration
 * @{
 ****************************************************************************************
 */

#if defined CFG_TRC_ALL
#define TRC_KE_MSG             1
#define TRC_KE_TMR             1
#define TRC_KE_EVT             1
#define TRC_MEM                1
#define TRC_SLEEP              1
#define TRC_SW_ASS             1
#define TRC_PROG               1
#define TRC_RX_DESC            1
#define TRC_CUSTOM             1
#define TRC_EA                 1

#if (BLE_EMB_PRESENT)
#define TRC_CS_BLE             1
#define TRC_LLCP               1
#define TRC_LLC_STATE_TRANS    1

#if (BLE_HOST_PRESENT)
#define TRC_HCI                1
#define TRC_L2CAP              1
#else
#define TRC_HCI                0
#define TRC_L2CAP              0
#endif /*(BLE_HOST_PRESENT)*/

#else
#define TRC_CS_BLE             0
#define TRC_LLCP               0
#define TRC_LLC_STATE_TRANS    0
#define TRC_HCI                0
#define TRC_L2CAP              0
#endif /*(BLE_EMB_PRESENT)*/

#if (BT_EMB_PRESENT)
#define TRC_CS_BT              1
#define TRC_LMP                1
#define TRC_LC_STATE_TRANS     1
#else
#define TRC_CS_BT              0
#define TRC_LMP                0
#define TRC_LC_STATE_TRANS     0
#endif /*(BT_EMB_PRESENT)*/

#else /*(CFG_TRC_ALL)*/

#if defined CFG_TRC_KE_MSG
#define TRC_KE_MSG             1
#else
#define TRC_KE_MSG             0
#endif /*(CFG_TRC_KE_MSG)*/

#if defined CFG_TRC_KE_TMR
#define TRC_KE_TMR             1
#else
#define TRC_KE_TMR             0
#endif /*CFG_TRC_KE_TMR*/

#if defined CFG_TRC_KE_EVT
#define TRC_KE_EVT             1
#else
#define TRC_KE_EVT             0
#endif /*CFG_TRC_KE_EVT*/

#if defined CFG_TRC_MEM
#define TRC_MEM                1
#else
#define TRC_MEM                0
#endif /*CFG_TRC_MEM*/

#if defined CFG_TRC_SLEEP
#define TRC_SLEEP              1
#else
#define TRC_SLEEP              0
#endif /*CFG_TRC_SLEEP*/

#if defined CFG_TRC_SW_ASS
#define TRC_SW_ASS             1
#else
#define TRC_SW_ASS             0
#endif /*CFG_TRC_SW_ASS*/

#if defined CFG_TRC_PROG
#define TRC_PROG               1
#else
#define TRC_PROG               0
#endif /*CFG_TRC_PROG*/

#if defined CFG_TRC_RX_DESC
#define TRC_RX_DESC            1
#else
#define TRC_RX_DESC            0
#endif /*CFG_TRC_RX_DESC*/

#if defined CFG_TRC_CUSTOM
#define TRC_CUSTOM             1
#else
#define TRC_CUSTOM             0
#endif /*CFG_TRC_CUSTOM*/

#if defined CFG_TRC_EA
#define TRC_EA                 1
#else
#define TRC_EA                 0
#endif /*CFG_TRC_EA*/

#if (BLE_EMB_PRESENT)
#if defined CFG_TRC_CS_BLE
#define TRC_CS_BLE             1
#else
#define TRC_CS_BLE             0
#endif /*(CFG_TRC_CS_BLE)*/

#if defined CFG_TRC_LLCP
#define TRC_LLCP               1
#else
#define TRC_LLCP               0
#endif /*(CFG_TRC_LLCP)*/

#if defined CFG_TRC_LLC_STATE_TRANS

#define TRC_LLC_STATE_TRANS    1
#else
#define TRC_LLC_STATE_TRANS    0
#endif /*(CFG_TRC_LLC_STATE_TRANS)*/

#if (BLE_HOST_PRESENT)

#if defined CFG_TRC_L2CAP
#define TRC_L2CAP              1
#else
#define TRC_L2CAP              0
#endif /*CFG_TRC_L2CAP*/

#if defined CFG_TRC_HCI
#define TRC_HCI                1
#else
#define TRC_HCI                0
#endif /*CFG_TRC_HCI*/

#else /*BLE_HOST_PRESENT*/

#define TRC_L2CAP              0
#define TRC_HCI                0
#endif /*BLE_HOST_PRESENT*/

#else /*BLE_EMB_PRESENT*/

#define TRC_CS_BLE             0
#define TRC_LLCP               0
#define TRC_LLC_STATE_TRANS    0
#define TRC_L2CAP              0
#define TRC_HCI                0

#endif /*BLE_EMB_PRESENT*/

#if (BT_EMB_PRESENT)

#if defined CFG_TRC_CS_BT
#define TRC_CS_BT              1
#else
#define TRC_CS_BT              0
#endif /*CFG_TRC_CS_BT*/

#if defined CFG_TRC_LMP
#define TRC_LMP                1
#else
#define TRC_LMP                0
#endif /*CFG_TRC_LMP*/

#if defined CFG_TRC_LC_STATE_TRANS
#define TRC_LC_STATE_TRANS     1
#else
#define TRC_LC_STATE_TRANS     0
#endif /*CFG_TRC_LC_STATE_TRANS*/

#else /*BT_EMB_PRESENT*/

#define TRC_CS_BT              0
#define TRC_LMP                0
#define TRC_LC_STATE_TRANS     0

#endif /*BT_EMB_PRESENT*/

#endif /* CFG_TRC_ALL */
/// @} TRACER
#endif /* DBG_TRC_CONFIG_H_ */

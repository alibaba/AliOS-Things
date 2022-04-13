#ifndef DBG_SWDIAG_H_
#define DBG_SWDIAG_H_

/**
 ****************************************************************************************
 * @addtogroup DBGSWDIAG Diag
 * @ingroup DBG
 * @brief Debug SW profiling module
 *
 * SW profiling is a debug feature that provides user a configurable way to analyze SW execution performance or
 * behavior, such as timings, state machines, bit field values and so on.
 * It manages the SW state representation over HW digital diagnostic signals (diagports), from the signals selection to
 * the toggling of real HW signals.
 * HW signals representing the SW execution could then be watched on a logic analyzer in parallel with HW internal
 * signals, that could be very useful for low level debugging.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"       // stack configuration

#if (!RW_SWDIAG)

/// Macro used to set a SW diag to specific value
#define DBG_SWDIAG(bank , field , value)

#else //RW_SWDIAG

#if (BT_EMB_PRESENT) // TODO not fully agree with this
#include "reg_btcore.h"          // bt core registers
#elif (BLE_EMB_PRESENT)
#include "reg_blecore.h"         // ble core registers
#endif //BT_EMB_PRESENT / BLE_EMB_PRESENT


/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of HW diagport banks
#define DBG_SWDIAG_NB_HW_BANKS        4

/// Undefined configuration
#define DBG_SWDIAG_UNDEFINED        0xFF


/*
 * MACROS
 ****************************************************************************************
 */
#if (BT_EMB_PRESENT)
#define SW_DIAG_SET     bt_swprofiling_set
#define SW_DIAG_GET     bt_swprofiling_get
#elif (BLE_EMB_PRESENT)
#define SW_DIAG_SET     ble_swprofiling_set
#define SW_DIAG_GET     ble_swprofiling_get
#endif //BT_EMB_PRESENT / BLE_EMB_PRESENT

/// Macro used to set a SW diag to specific value
#define DBG_SWDIAG(bank , field , value)                                              \
    if(sw_to_hw[DBG_SWDIAG_##bank] != DBG_SWDIAG_UNDEFINED)                           \
    {                                                                                 \
        SW_DIAG_SET( ((SW_DIAG_GET()) & (~(DBG_SWDIAG_##bank##_##field##_MASK << (8 * sw_to_hw[DBG_SWDIAG_##bank])))) | ((((value << DBG_SWDIAG_##bank##_##field##_OFFSET) & DBG_SWDIAG_##bank##_##field##_MASK) << (8*sw_to_hw[DBG_SWDIAG_##bank])))  );      \
    }


/*
 * SW DIAGS MAP
 ****************************************************************************************
 */

enum dbg_swdiag
{
/**
******************************************************************************************
* @brief BANK 0 : BT_ISR
******************************************************************************************
*/
    DBG_SWDIAG_BT_ISR                          = 0,

    DBG_SWDIAG_BT_ISR_RX_OFFSET                = 0,
    DBG_SWDIAG_BT_ISR_RX_MASK                  = 0x01,

    DBG_SWDIAG_BT_ISR_FRAME_OFFSET             = 1,
    DBG_SWDIAG_BT_ISR_FRAME_MASK               = 0x02,

    DBG_SWDIAG_BT_ISR_SKET_OFFSET              = 3,
    DBG_SWDIAG_BT_ISR_SKET_MASK                = 0x04,


/**
******************************************************************************************
* @brief BANK 1 : SLEEP
******************************************************************************************
*/
    DBG_SWDIAG_SLEEP                           = 1,

    DBG_SWDIAG_SLEEP_SLEEP_OFFSET              = 0,
    DBG_SWDIAG_SLEEP_SLEEP_MASK                = 0x01,

    DBG_SWDIAG_SLEEP_FUNC_OFFSET               = 1,
    DBG_SWDIAG_SLEEP_FUNC_MASK                 = 0x02,

    DBG_SWDIAG_SLEEP_ALGO_OFFSET               = 4,
    DBG_SWDIAG_SLEEP_ALGO_MASK                 = 0xF0,

//    DBG_SWDIAG_SLEEP_PREVENT_OFFSET            = 4,
//    DBG_SWDIAG_SLEEP_PREVENT_MASK              = 0xF0,


/**
******************************************************************************************
* @brief BANK 2 : ISR
******************************************************************************************
*/
    DBG_SWDIAG_ISR                             = 2,

    DBG_SWDIAG_ISR_UART_OFFSET                 = 0,
    DBG_SWDIAG_ISR_UART_MASK                   = 0x01,

    DBG_SWDIAG_ISR_BT_OFFSET                   = 1,
    DBG_SWDIAG_ISR_BT_MASK                     = 0x02,

    DBG_SWDIAG_ISR_PS2_OFFSET                  = 1,
    DBG_SWDIAG_ISR_PS2_MASK                    = 0x02,

    DBG_SWDIAG_ISR_BLE_OFFSET                  = 2,
    DBG_SWDIAG_ISR_BLE_MASK                    = 0x04,

    DBG_SWDIAG_ISR_RESERVED_OFFSET              = 3,
    DBG_SWDIAG_ISR_RESERVED_MASK                = 0x08,

    DBG_SWDIAG_ISR_GPIO_OFFSET                 = 4,
    DBG_SWDIAG_ISR_GPIO_MASK                   = 0x10,

    DBG_SWDIAG_ISR_RTC0_OFFSET                 = 5,
    DBG_SWDIAG_ISR_RTC0_MASK                   = 0x20,

    DBG_SWDIAG_ISR_SPI_OFFSET                  = 6,
    DBG_SWDIAG_ISR_SPI_MASK                    = 0x40,

    DBG_SWDIAG_ISR_WFI_OFFSET                  = 7,
    DBG_SWDIAG_ISR_WFI_MASK                    = 0x80,


/**
******************************************************************************************
* @brief BANK 3 : BLEISR
******************************************************************************************
*/
    DBG_SWDIAG_BLE_ISR                         = 3,

    DBG_SWDIAG_BLE_ISR_RXINT_OFFSET            = 0,
    DBG_SWDIAG_BLE_ISR_RXINT_MASK              = 0x01,

    DBG_SWDIAG_BLE_ISR_TXINT_OFFSET            = 1,
    DBG_SWDIAG_BLE_ISR_TXINT_MASK              = 0x02,

    DBG_SWDIAG_BLE_ISR_EVENTINT_OFFSET         = 2,
    DBG_SWDIAG_BLE_ISR_EVENTINT_MASK           = 0x04,

    DBG_SWDIAG_BLE_ISR_EVENTAPFMINT_OFFSET     = 3,
    DBG_SWDIAG_BLE_ISR_EVENTAPFMINT_MASK       = 0x08,

    DBG_SWDIAG_BLE_ISR_ERRORINT_OFFSET         = 4,
    DBG_SWDIAG_BLE_ISR_ERRORINT_MASK           = 0x10,

    DBG_SWDIAG_BLE_ISR_AUDIO0INT_OFFSET        = 5,
    DBG_SWDIAG_BLE_ISR_AUDIO0INT_MASK          = 0x20,

    DBG_SWDIAG_BLE_ISR_AUDIO1INT_OFFSET        = 6,
    DBG_SWDIAG_BLE_ISR_AUDIO1INT_MASK          = 0x40,

    DBG_SWDIAG_BLE_ISR_AUDIO2INT_OFFSET        = 7,
    DBG_SWDIAG_BLE_ISR_AUDIO2INT_MASK          = 0x80,



/**
******************************************************************************************
* @brief BANK 4 : FLASH
******************************************************************************************
*/
    DBG_SWDIAG_FLASH                           = 4,

    DBG_SWDIAG_FLASH_STATE_OFFSET              = 0,
    DBG_SWDIAG_FLASH_STATE_MASK                = 0x07,

    DBG_SWDIAG_FLASH_SUBSTATE_OFFSET           = 3,
    DBG_SWDIAG_FLASH_SUBSTATE_MASK             = 0x38,

    DBG_SWDIAG_FLASH_MANAGE_OFFSET             = 6,
    DBG_SWDIAG_FLASH_MANAGE_MASK               = 0x40,

    DBG_SWDIAG_FLASH_CALLBACK_OFFSET           = 7,
    DBG_SWDIAG_FLASH_CALLBACK_MASK             = 0x80,

/**
******************************************************************************************
* @brief BANK 5 : Reserved
******************************************************************************************
*/



/**
******************************************************************************************
* @brief BANK 6 : Event execution overview.
******************************************************************************************
*/
    DBG_SWDIAG_EVT                         = 6,

    DBG_SWDIAG_EVT_BLE_SCHEDULE_OFFSET     = 0,
    DBG_SWDIAG_EVT_BLE_SCHEDULE_MASK       = 0x01,

    DBG_SWDIAG_EVT_BLE_RX_OFFSET           = 1,
    DBG_SWDIAG_EVT_BLE_RX_MASK             = 0x02,

    DBG_SWDIAG_EVT_BLE_END_OFFSET          = 2,
    DBG_SWDIAG_EVT_BLE_END_MASK            = 0x04,

    DBG_SWDIAG_EVT_BLE_RESTART_OFFSET      = 3,
    DBG_SWDIAG_EVT_BLE_RESTART_MASK        = 0x08,

    DBG_SWDIAG_EVT_BLE_PROG_OFFSET         = 4,
    DBG_SWDIAG_EVT_BLE_PROG_MASK           = 0x10,

    DBG_SWDIAG_EVT_BLE_CRYPT_OFFSET        = 5,
    DBG_SWDIAG_EVT_BLE_CRYPT_MASK          = 0x20,

    DBG_SWDIAG_EVT_TIMER_OFFSET            = 6,
    DBG_SWDIAG_EVT_TIMER_MASK              = 0x40,

    DBG_SWDIAG_EVT_MESSAGE_OFFSET          = 7,
    DBG_SWDIAG_EVT_MESSAGE_MASK            = 0x80,


/**
******************************************************************************************
* @brief BANK 8 : AFH
******************************************************************************************
*/
    DBG_SWDIAG_AFH                               = 8,

    DBG_SWDIAG_AFH_TO_OFFSET                     = 0,
    DBG_SWDIAG_AFH_TO_MASK                       = 0x01,

    DBG_SWDIAG_AFH_LMP_TX_OFFSET                 = 1,
    DBG_SWDIAG_AFH_LMP_TX_MASK                   = 0x02,

    DBG_SWDIAG_AFH_CLASS_RX_OFFSET               = 2,
    DBG_SWDIAG_AFH_CLASS_RX_MASK                 = 0x04,

    DBG_SWDIAG_AFH_HOST_OFFSET                   = 3,
    DBG_SWDIAG_AFH_HOST_MASK                     = 0x08,

    DBG_SWDIAG_AFH_REPORT_TO_OFFSET              = 4,
    DBG_SWDIAG_AFH_REPORT_TO_MASK                = 0x10,

    DBG_SWDIAG_AFH_HSSI_OFFSET                   = 5,
    DBG_SWDIAG_AFH_HSSI_MASK                     = 0x20,

    DBG_SWDIAG_AFH_CFM_OFFSET                    = 6,
    DBG_SWDIAG_AFH_CFM_MASK                      = 0x40,

    DBG_SWDIAG_AFH_LMP_RX_OFFSET                 = 7,
    DBG_SWDIAG_AFH_LMP_RX_MASK                   = 0x80,


/**
******************************************************************************************
* @brief BANK 9 : SWITCH
******************************************************************************************
*/
    DBG_SWDIAG_SWITCH                            = 9,

    DBG_SWDIAG_SWITCH_LOOP_OFFSET                = 0,
    DBG_SWDIAG_SWITCH_LOOP_MASK                  = 0x01,

    DBG_SWDIAG_SWITCH_SM_OFFSET                  = 1,
    DBG_SWDIAG_SWITCH_SM_MASK                    = 0x06,

    DBG_SWDIAG_SWITCH_RSW_FAIL_OFFSET            = 3,
    DBG_SWDIAG_SWITCH_RSW_FAIL_MASK              = 0x08,


/**
******************************************************************************************
* @brief BANK 11 : VALUE8
******************************************************************************************
*/
    DBG_SWDIAG_VALUE8                            = 11,

    DBG_SWDIAG_VALUE8_VALUE_OFFSET               = 0,
    DBG_SWDIAG_VALUE8_VALUE_MASK                 = 0xFF,

/**
******************************************************************************************
* @brief BANK 12 : VALUE16
******************************************************************************************
*/
    DBG_SWDIAG_VALUE16                           = 12,

    DBG_SWDIAG_VALUE16_VALUE_OFFSET              = 0,
    DBG_SWDIAG_VALUE16_VALUE_MASK                = 0xFF,


/**
******************************************************************************************
* @brief BANK 13 : SNIFF
******************************************************************************************
*/
    DBG_SWDIAG_SNIFF                             = 13,

    DBG_SWDIAG_SNIFF_EVT_START_OFFSET            = 0,
    DBG_SWDIAG_SNIFF_EVT_START_MASK              = 0x01,

    DBG_SWDIAG_SNIFF_EVT_CANCELED_OFFSET         = 1,
    DBG_SWDIAG_SNIFF_EVT_CANCELED_MASK           = 0x02,

    DBG_SWDIAG_SNIFF_FRM_ISR_OFFSET              = 2,
    DBG_SWDIAG_SNIFF_FRM_ISR_MASK                = 0x04,

    DBG_SWDIAG_SNIFF_RX_ISR_OFFSET               = 3,
    DBG_SWDIAG_SNIFF_RX_ISR_MASK                 = 0x08,

    DBG_SWDIAG_SNIFF_SCHED_OFFSET                = 4,
    DBG_SWDIAG_SNIFF_SCHED_MASK                  = 0x10,


/**
******************************************************************************************
* @brief BANK 14 : SCO
******************************************************************************************
*/
    DBG_SWDIAG_SCO                               = 14,

    DBG_SWDIAG_SCO_EVT_START_OFFSET              = 0,
    DBG_SWDIAG_SCO_EVT_START_MASK                = 0x01,

    DBG_SWDIAG_SCO_EVT_STOP_OFFSET               = 1,
    DBG_SWDIAG_SCO_EVT_STOP_MASK                 = 0x02,

    DBG_SWDIAG_SCO_EVT_CANCELED_OFFSET           = 2,
    DBG_SWDIAG_SCO_EVT_CANCELED_MASK             = 0x04,

    DBG_SWDIAG_SCO_FRM_ISR_OFFSET                = 3,
    DBG_SWDIAG_SCO_FRM_ISR_MASK                  = 0x08,

    DBG_SWDIAG_SCO_RESCHED_OFFSET                = 4,
    DBG_SWDIAG_SCO_RESCHED_MASK                  = 0x10,

    DBG_SWDIAG_SCO_UPDATE_OFFSET                 = 5,
    DBG_SWDIAG_SCO_UPDATE_MASK                   = 0x60,

    DBG_SWDIAG_SCO_AUDIO_ISR_OFFSET              = 7,
    DBG_SWDIAG_SCO_AUDIO_ISR_MASK                = 0x80,


/**
******************************************************************************************
* @brief BANK 15 : CHNL_ASSESS (NB_CHNL)
******************************************************************************************
*/
    DBG_SWDIAG_CHNL_ASSESS                       = 15,

    DBG_SWDIAG_CHNL_ASSESS_NB_CHNL_OFFSET        = 0,
    DBG_SWDIAG_CHNL_ASSESS_NB_CHNL_MASK          = 0x3F,

    DBG_SWDIAG_CHNL_ASSESS_REM_CHNL_OFFSET       = 6,
    DBG_SWDIAG_CHNL_ASSESS_REM_CHNL_MASK         = 0x40,

    DBG_SWDIAG_CHNL_ASSESS_ADD_CHNL_OFFSET       = 7,
    DBG_SWDIAG_CHNL_ASSESS_ADD_CHNL_MASK         = 0x80,

/**
******************************************************************************************
* @brief BANK 16 : ASSESS_MECH
******************************************************************************************
*/
    DBG_SWDIAG_ASSESS_MECH                       = 16,

    DBG_SWDIAG_ASSESS_MECH_ATIMER_OFFSET         = 0,
    DBG_SWDIAG_ASSESS_MECH_ATIMER_MASK           = 0x01,

    DBG_SWDIAG_ASSESS_MECH_RTIMER_OFFSET         = 1,
    DBG_SWDIAG_ASSESS_MECH_RTIMER_MASK           = 0x02,

    DBG_SWDIAG_ASSESS_MECH_BAD_PKT_OFFSET        = 2,
    DBG_SWDIAG_ASSESS_MECH_BAD_PKT_MASK          = 0x04,

    DBG_SWDIAG_ASSESS_MECH_SYNC_ERR_OFFSET       = 3,
    DBG_SWDIAG_ASSESS_MECH_SYNC_ERR_MASK         = 0x08,

/**
******************************************************************************************
* @brief BANK 17 : PAGE
******************************************************************************************
*/
    DBG_SWDIAG_PAGE                              = 17,

    DBG_SWDIAG_PAGE_PSCAN_EVT_START_OFFSET       = 0,
    DBG_SWDIAG_PAGE_PSCAN_EVT_START_MASK         = 0x01,

    DBG_SWDIAG_PAGE_PSCAN_EVT_CANCELED_OFFSET    = 1,
    DBG_SWDIAG_PAGE_PSCAN_EVT_CANCELED_MASK      = 0x02,

    DBG_SWDIAG_PAGE_PSCAN_FRM_ISR_OFFSET         = 2,
    DBG_SWDIAG_PAGE_PSCAN_FRM_ISR_MASK           = 0x04,

    DBG_SWDIAG_PAGE_PSCAN_KE_EVT_OFFSET          = 3,
    DBG_SWDIAG_PAGE_PSCAN_KE_EVT_MASK            = 0x08,

    DBG_SWDIAG_PAGE_PAGE_EVT_START_OFFSET        = 4,
    DBG_SWDIAG_PAGE_PAGE_EVT_START_MASK          = 0x10,

    DBG_SWDIAG_PAGE_PAGE_EVT_CANCELED_OFFSET     = 5,
    DBG_SWDIAG_PAGE_PAGE_EVT_CANCELED_MASK       = 0x20,

    DBG_SWDIAG_PAGE_PAGE_FRM_ISR_OFFSET          = 6,
    DBG_SWDIAG_PAGE_PAGE_FRM_ISR_MASK            = 0x40,

    DBG_SWDIAG_PAGE_PAGE_STEP_OFFSET             = 7,
    DBG_SWDIAG_PAGE_PAGE_STEP_MASK               = 0x80,

/**
******************************************************************************************
* @brief BANK 18 : INQ
******************************************************************************************
*/
    DBG_SWDIAG_INQ                               = 18,

    DBG_SWDIAG_INQ_ISCAN_EVT_START_OFFSET        = 0,
    DBG_SWDIAG_INQ_ISCAN_EVT_START_MASK          = 0x01,

    DBG_SWDIAG_INQ_ISCAN_EVT_CANCELED_OFFSET     = 1,
    DBG_SWDIAG_INQ_ISCAN_EVT_CANCELED_MASK       = 0x02,

    DBG_SWDIAG_INQ_ISCAN_FRM_ISR_OFFSET          = 2,
    DBG_SWDIAG_INQ_ISCAN_FRM_ISR_MASK            = 0x04,

    DBG_SWDIAG_INQ_INQ_EVT_START_OFFSET          = 3,
    DBG_SWDIAG_INQ_INQ_EVT_START_MASK            = 0x08,

    DBG_SWDIAG_INQ_INQ_EVT_CANCELED_OFFSET       = 4,
    DBG_SWDIAG_INQ_INQ_EVT_CANCELED_MASK         = 0x10,

    DBG_SWDIAG_INQ_INQ_FRM_ISR_OFFSET            = 5,
    DBG_SWDIAG_INQ_INQ_FRM_ISR_MASK              = 0x20,

/**
******************************************************************************************
* @brief BANK 19 : ACL
******************************************************************************************
*/
    DBG_SWDIAG_ACL                               = 19,

    DBG_SWDIAG_ACL_EVT_START_OFFSET              = 0,
    DBG_SWDIAG_ACL_EVT_START_MASK                = 0x01,

    DBG_SWDIAG_ACL_EVT_STOP_OFFSET               = 1,
    DBG_SWDIAG_ACL_EVT_STOP_MASK                 = 0x02,

    DBG_SWDIAG_ACL_EVT_CANCELED_OFFSET           = 2,
    DBG_SWDIAG_ACL_EVT_CANCELED_MASK             = 0x04,

    DBG_SWDIAG_ACL_FRM_ISR_OFFSET                = 3,
    DBG_SWDIAG_ACL_FRM_ISR_MASK                  = 0x08,

    DBG_SWDIAG_ACL_CLK_ISR_OFFSET                = 4,
    DBG_SWDIAG_ACL_CLK_ISR_MASK                  = 0x10,

    DBG_SWDIAG_ACL_PROG_EN_OFFSET                = 5,
    DBG_SWDIAG_ACL_PROG_EN_MASK                  = 0x20,

    DBG_SWDIAG_ACL_SKET_ISR_OFFSET               = 6,
    DBG_SWDIAG_ACL_SKET_ISR_MASK                 = 0x40,

    DBG_SWDIAG_ACL_PARITY_OFFSET                 = 7,
    DBG_SWDIAG_ACL_PARITY_MASK                   = 0x80,

/**
******************************************************************************************
* @brief BANK 20 : RSW
******************************************************************************************
*/
    DBG_SWDIAG_RSW                               = 20,

    DBG_SWDIAG_RSW_EVT_START_OFFSET              = 0,
    DBG_SWDIAG_RSW_EVT_START_MASK                = 0x01,

    DBG_SWDIAG_RSW_EVT_CANCELED_OFFSET           = 1,
    DBG_SWDIAG_RSW_EVT_CANCELED_MASK             = 0x02,

    DBG_SWDIAG_RSW_FRM_ISR_OFFSET                = 2,
    DBG_SWDIAG_RSW_FRM_ISR_MASK                  = 0x04,

    DBG_SWDIAG_RSW_STEP_OFFSET                   = 3,
    DBG_SWDIAG_RSW_STEP_MASK                     = 0x08,

    DBG_SWDIAG_RSW_TX_SLOTOFF_OFFSET             = 4,
    DBG_SWDIAG_RSW_TX_SLOTOFF_MASK               = 0x10,

    DBG_SWDIAG_RSW_TX_PARITY_OFFSET              = 5,
    DBG_SWDIAG_RSW_TX_PARITY_MASK                = 0x20,

    DBG_SWDIAG_RSW_TDD_PARITY_OFFSET             = 6,
    DBG_SWDIAG_RSW_TDD_PARITY_MASK               = 0x40,

/**
******************************************************************************************
* @brief BANK 21 : TX
******************************************************************************************
*/
    DBG_SWDIAG_TX                                = 21,

    DBG_SWDIAG_TX_LMP_TX_OFFSET                  = 0,
    DBG_SWDIAG_TX_LMP_TX_MASK                    = 0x01,

    DBG_SWDIAG_TX_ACL_TX_OFFSET                  = 1,
    DBG_SWDIAG_TX_ACL_TX_MASK                    = 0x02,

    DBG_SWDIAG_TX_SW_IDX_OFFSET                  = 2,
    DBG_SWDIAG_TX_SW_IDX_MASK                    = 0x04,

    DBG_SWDIAG_TX_HW_IDX_OFFSET                  = 3,
    DBG_SWDIAG_TX_HW_IDX_MASK                    = 0x08,

    DBG_SWDIAG_TX_TX_CNT_OFFSET                  = 4,
    DBG_SWDIAG_TX_TX_CNT_MASK                    = 0x30,

/**
******************************************************************************************
* @brief BANK 22 : DATA
******************************************************************************************
*/
    DBG_SWDIAG_DATA                              = 22,

    DBG_SWDIAG_DATA_LMP_TX_OFFSET                = 0,
    DBG_SWDIAG_DATA_LMP_TX_MASK                  = 0x01,

    DBG_SWDIAG_DATA_ACL_TX_OFFSET                = 1,
    DBG_SWDIAG_DATA_ACL_TX_MASK                  = 0x02,

    DBG_SWDIAG_DATA_LMP_RX_OFFSET                = 2,
    DBG_SWDIAG_DATA_LMP_RX_MASK                  = 0x04,

    DBG_SWDIAG_DATA_ACL_RX_OFFSET                = 3,
    DBG_SWDIAG_DATA_ACL_RX_MASK                  = 0x08,

    DBG_SWDIAG_DATA_LMP_ACK_OFFSET               = 4,
    DBG_SWDIAG_DATA_LMP_ACK_MASK                 = 0x10,

    DBG_SWDIAG_DATA_ACL_ACK_OFFSET               = 5,
    DBG_SWDIAG_DATA_ACL_ACK_MASK                 = 0x20,

    /**
    ******************************************************************************************
    * @brief BANK 23 : ECDH
    ******************************************************************************************
    */
    DBG_SWDIAG_ECDH                              = 23,

    DBG_SWDIAG_ECDH_BUSY_OFFSET                  = 0,
    DBG_SWDIAG_ECDH_BUSY_MASK                    = 0x01,

    DBG_SWDIAG_ECDH_COMPUTE_OFFSET               = 1,
    DBG_SWDIAG_ECDH_COMPUTE_MASK                 = 0x02,

    DBG_SWDIAG_ECDH_MULT_OFFSET                  = 2,
    DBG_SWDIAG_ECDH_MULT_MASK                    = 0x04,

    DBG_SWDIAG_ECDH_END_OFFSET                   = 3,
    DBG_SWDIAG_ECDH_END_MASK                     = 0x08,

    DBG_SWDIAG_ECDH_INV_OFFSET                   = 4,
    DBG_SWDIAG_ECDH_INV_MASK                     = 0x10,

    /**
     ******************************************************************************************
     * @brief BANK 24 : PCA
     ******************************************************************************************
     */
    DBG_SWDIAG_PCA                               = 24,

    DBG_SWDIAG_PCA_FRAME_SYNC_INT_OFFSET         = 0,
    DBG_SWDIAG_PCA_FRAME_SYNC_INT_MASK           = 0x01,

    DBG_SWDIAG_PCA_MTOFF_LT_INT_OFFSET           = 1,
    DBG_SWDIAG_PCA_MTOFF_LT_INT_MASK             = 0x02,

    DBG_SWDIAG_PCA_SLOT_INT_OFFSET               = 2,
    DBG_SWDIAG_PCA_SLOT_INT_MASK                 = 0x04,

    DBG_SWDIAG_PCA_COARSE_CLK_ADJ_OFFSET         = 3,
    DBG_SWDIAG_PCA_COARSE_CLK_ADJ_MASK           = 0x08,

    DBG_SWDIAG_PCA_EVT_START_OFFSET              = 4,
    DBG_SWDIAG_PCA_EVT_START_MASK                = 0x10,

    DBG_SWDIAG_PCA_EVT_CANCELED_OFFSET           = 5,
    DBG_SWDIAG_PCA_EVT_CANCELED_MASK             = 0x20,

    DBG_SWDIAG_PCA_ALARM_INSTANT_OFFSET          = 6,
    DBG_SWDIAG_PCA_ALARM_INSTANT_MASK            = 0x40,

    DBG_SWDIAG_PCA_CLK_DRAG_OFFSET               = 7,
    DBG_SWDIAG_PCA_CLK_DRAG_MASK                 = 0x80,

    /**
     ******************************************************************************************
     * @brief BANK 25 : BCST
     ******************************************************************************************
     */
    DBG_SWDIAG_BCST                              = 25,

    DBG_SWDIAG_BCST_FRM_ISR_OFFSET               = 0,
    DBG_SWDIAG_BCST_FRM_ISR_MASK                 = 0x01,

    DBG_SWDIAG_BCST_EVT_START_OFFSET             = 1,
    DBG_SWDIAG_BCST_EVT_START_MASK               = 0x02,

    DBG_SWDIAG_BCST_SKET_ISR_OFFSET              = 2,
    DBG_SWDIAG_BCST_SKET_ISR_MASK                = 0x04,

    DBG_SWDIAG_BCST_EVT_CANCELED_OFFSET          = 3,
    DBG_SWDIAG_BCST_EVT_CANCELED_MASK            = 0x08,

    DBG_SWDIAG_BCST_ACL_TX_OFFSET                = 4,
    DBG_SWDIAG_BCST_ACL_TX_MASK                  = 0x10,

    DBG_SWDIAG_BCST_ACL_ACK_OFFSET               = 5,
    DBG_SWDIAG_BCST_ACL_ACK_MASK                 = 0x20,

    DBG_SWDIAG_BCST_LMP_TX_OFFSET                = 6,
    DBG_SWDIAG_BCST_LMP_TX_MASK                  = 0x40,

    DBG_SWDIAG_BCST_LMP_ACK_OFFSET               = 7,
    DBG_SWDIAG_BCST_LMP_ACK_MASK                 = 0x80,



    /**
    ******************************************************************************************
    * @brief IPISR - 2
    ******************************************************************************************
    */
    DBG_SWDIAG_IP_ISR                          = 26,

    DBG_SWDIAG_IP_ISR_CLKNINT_OFFSET           = 0,
    DBG_SWDIAG_IP_ISR_CLKNINT_MASK             = 0x01,

    DBG_SWDIAG_IP_ISR_FINETGTIMINT_OFFSET      = 1,
    DBG_SWDIAG_IP_ISR_FINETGTIMINT_MASK        = 0x02,

    DBG_SWDIAG_IP_ISR_GROSSTGTIMINT_OFFSET     = 2,
    DBG_SWDIAG_IP_ISR_GROSSTGTIMINT_MASK       = 0x04,

    DBG_SWDIAG_IP_ISR_SLPINT_OFFSET            = 3,
    DBG_SWDIAG_IP_ISR_SLPINT_MASK              = 0x08,

    DBG_SWDIAG_IP_ISR_CRYPTINT_OFFSET          = 4,
    DBG_SWDIAG_IP_ISR_CRYPTINT_MASK            = 0x10,

    DBG_SWDIAG_IP_ISR_SWINT_OFFSET             = 5,
    DBG_SWDIAG_IP_ISR_SWINT_MASK               = 0x20,

    /**
    ******************************************************************************************
    * @brief RSA Algorithm
    ******************************************************************************************
    */

    DBG_SWDIAG_RSA                              = 27,

    DBG_SWDIAG_RSA_BUSY_OFFSET                  = 0,
    DBG_SWDIAG_RSA_BUSY_MASK                    = 0x01,

    DBG_SWDIAG_RSA_COMPUTE_OFFSET               = 1,
    DBG_SWDIAG_RSA_COMPUTE_MASK                 = 0x02,

    /**
     ******************************************************************************************
     * @brief BANK 28 : SYNC SCAN
     ******************************************************************************************
     */
    DBG_SWDIAG_SSCAN                              = 28,

    DBG_SWDIAG_SSCAN_FRM_ISR_OFFSET               = 0,
    DBG_SWDIAG_SSCAN_FRM_ISR_MASK                 = 0x01,

    DBG_SWDIAG_SSCAN_EVT_START_OFFSET             = 1,
    DBG_SWDIAG_SSCAN_EVT_START_MASK               = 0x02,

    DBG_SWDIAG_SSCAN_START_OFFSET                 = 2,
    DBG_SWDIAG_SSCAN_START_MASK                   = 0x04,

    DBG_SWDIAG_SSCAN_EVT_CANCELED_OFFSET          = 3,
    DBG_SWDIAG_SSCAN_EVT_CANCELED_MASK            = 0x08,

    DBG_SWDIAG_SSCAN_FRM_CBK_OFFSET               = 4,
    DBG_SWDIAG_SSCAN_FRM_CBK_MASK                 = 0x10,

    DBG_SWDIAG_SSCAN_STP_RX_OFFSET                = 5,
    DBG_SWDIAG_SSCAN_STP_RX_MASK                  = 0x20,

    DBG_SWDIAG_SSCAN_STP_RX_OK_OFFSET             = 6,
    DBG_SWDIAG_SSCAN_STP_RX_OK_MASK               = 0x40,

    DBG_SWDIAG_SSCAN_TIMEOUT_OFFSET               = 7,
    DBG_SWDIAG_SSCAN_TIMEOUT_MASK                 = 0x80,

    /**
     ******************************************************************************************
     * @brief BANK 29 : LE SCAN
     ******************************************************************************************
     */
    DBG_SWDIAG_LESCAN                              = 29,

    DBG_SWDIAG_LESCAN_START_OFFSET                 = 0,
    DBG_SWDIAG_LESCAN_START_MASK                   = 0x01,

    DBG_SWDIAG_LESCAN_STOP_OFFSET                  = 1,
    DBG_SWDIAG_LESCAN_STOP_MASK                    = 0x02,

    DBG_SWDIAG_LESCAN_EVT_START_OFFSET             = 2,
    DBG_SWDIAG_LESCAN_EVT_START_MASK               = 0x04,

    DBG_SWDIAG_LESCAN_EVT_CANCELED_OFFSET          = 3,
    DBG_SWDIAG_LESCAN_EVT_CANCELED_MASK            = 0x08,

    DBG_SWDIAG_LESCAN_FRM_ISR_RX_OFFSET            = 4,
    DBG_SWDIAG_LESCAN_FRM_ISR_RX_MASK              = 0x10,

    DBG_SWDIAG_LESCAN_FRM_ISR_EOF_OFFSET           = 5,
    DBG_SWDIAG_LESCAN_FRM_ISR_EOF_MASK             = 0x20,

    DBG_SWDIAG_LESCAN_SCHED_OFFSET                 = 6,
    DBG_SWDIAG_LESCAN_SCHED_MASK                   = 0x40,

    DBG_SWDIAG_LESCAN_PKT_RX_OFFSET                = 7,
    DBG_SWDIAG_LESCAN_PKT_RX_MASK                  = 0x80,

    /**
     ******************************************************************************************
     * @brief BANK 30 : LLD FM
     ******************************************************************************************
     */
    DBG_SWDIAG_LLDFM                              = 30,

    DBG_SWDIAG_LLDFM_RX_ISR_OFFSET                = 0,
    DBG_SWDIAG_LLDFM_RX_ISR_MASK                  = 0x01,

    DBG_SWDIAG_LLDFM_END_ISR_OFFSET               = 1,
    DBG_SWDIAG_LLDFM_END_ISR_MASK                 = 0x02,

    DBG_SWDIAG_LLDFM_PROG_PUSH_OFFSET             = 2,
    DBG_SWDIAG_LLDFM_PROG_PUSH_MASK               = 0x04,
     
    DBG_SWDIAG_LLDFM_INIT_OFFSET                  = 3,
    DBG_SWDIAG_LLDFM_INIT_MASK                    = 0x08,
    
    DBG_SWDIAG_LLDFM_TX_ISR_OFFSET                = 4,
    DBG_SWDIAG_LLDFM_TX_ISR_MASK                  = 0x10,

    /**
     ******************************************************************************************
     * @brief BANK 31 : LE INITIATING
     ******************************************************************************************
     */
    DBG_SWDIAG_LEINIT                             = 31,

    DBG_SWDIAG_LEINIT_START_OFFSET                = 0,
    DBG_SWDIAG_LEINIT_START_MASK                  = 0x01,

    DBG_SWDIAG_LEINIT_STOP_OFFSET                 = 1,
    DBG_SWDIAG_LEINIT_STOP_MASK                   = 0x02,

    DBG_SWDIAG_LEINIT_EVT_START_OFFSET            = 2,
    DBG_SWDIAG_LEINIT_EVT_START_MASK              = 0x04,

    DBG_SWDIAG_LEINIT_EVT_CANCELED_OFFSET         = 3,
    DBG_SWDIAG_LEINIT_EVT_CANCELED_MASK           = 0x08,

    DBG_SWDIAG_LEINIT_FRM_CBK_OFFSET               = 4,
    DBG_SWDIAG_LEINIT_FRM_CBK_MASK                 = 0x10,

    DBG_SWDIAG_LEINIT_FRM_ISR_EOF_OFFSET          = 5,
    DBG_SWDIAG_LEINIT_FRM_ISR_EOF_MASK            = 0x20,

    DBG_SWDIAG_LEINIT_SCHED_OFFSET                = 6,
    DBG_SWDIAG_LEINIT_SCHED_MASK                  = 0x40,

    DBG_SWDIAG_LEINIT_PKT_RX_OFFSET               = 7,
    DBG_SWDIAG_LEINIT_PKT_RX_MASK                 = 0x80,

    /**
     ******************************************************************************************
     * @brief BANK 32 : LE ADVERTISING
     ******************************************************************************************
     */
    DBG_SWDIAG_LEADV                              = 32,

    DBG_SWDIAG_LEADV_START_OFFSET                 = 0,
    DBG_SWDIAG_LEADV_START_MASK                   = 0x01,

    DBG_SWDIAG_LEADV_STOP_OFFSET                  = 1,
    DBG_SWDIAG_LEADV_STOP_MASK                    = 0x02,

    DBG_SWDIAG_LEADV_EVT_START_OFFSET             = 2,
    DBG_SWDIAG_LEADV_EVT_START_MASK               = 0x04,

    DBG_SWDIAG_LEADV_EVT_CANCELED_OFFSET          = 3,
    DBG_SWDIAG_LEADV_EVT_CANCELED_MASK            = 0x08,

    DBG_SWDIAG_LEADV_RX_ISR_OFFSET                = 4,
    DBG_SWDIAG_LEADV_RX_ISR_MASK                  = 0x10,

    DBG_SWDIAG_LEADV_FRM_ISR_OFFSET               = 5,
    DBG_SWDIAG_LEADV_FRM_ISR_MASK                 = 0x20,

    DBG_SWDIAG_LEADV_SCHED_OFFSET                 = 6,
    DBG_SWDIAG_LEADV_SCHED_MASK                   = 0x40,

    DBG_SWDIAG_LEADV_PKT_RX_OFFSET                = 7,
    DBG_SWDIAG_LEADV_PKT_RX_MASK                  = 0x80,

    /**
     ******************************************************************************************
     * @brief BANK 33 : LE CONNECTION
     ******************************************************************************************
     */
    DBG_SWDIAG_LECON                              = 33,

    DBG_SWDIAG_LECON_START_OFFSET                 = 0,
    DBG_SWDIAG_LECON_START_MASK                   = 0x01,

    DBG_SWDIAG_LECON_STOP_OFFSET                  = 1,
    DBG_SWDIAG_LECON_STOP_MASK                    = 0x02,

    DBG_SWDIAG_LECON_EVT_START_OFFSET             = 2,
    DBG_SWDIAG_LECON_EVT_START_MASK               = 0x04,

    DBG_SWDIAG_LECON_EVT_CANCELED_OFFSET          = 3,
    DBG_SWDIAG_LECON_EVT_CANCELED_MASK            = 0x08,

    DBG_SWDIAG_LECON_RX_ISR_OFFSET                = 4,
    DBG_SWDIAG_LECON_RX_ISR_MASK                  = 0x10,

    DBG_SWDIAG_LECON_FRM_ISR_OFFSET               = 5,
    DBG_SWDIAG_LECON_FRM_ISR_MASK                 = 0x20,

    DBG_SWDIAG_LECON_SCHED_OFFSET                 = 6,
    DBG_SWDIAG_LECON_SCHED_MASK                   = 0x40,

    DBG_SWDIAG_LECON_TX_ISR_OFFSET                = 7,
    DBG_SWDIAG_LECON_TX_ISR_MASK                  = 0x80,

	/**
	******************************************************************************************
	* @brief BANK 34 : LE DATA
	******************************************************************************************
	*/
	DBG_SWDIAG_LEDATA                             = 34,

	DBG_SWDIAG_LEDATA_LLCP_TX_OFFSET              = 0,
	DBG_SWDIAG_LEDATA_LLCP_TX_MASK                = 0x01,

	DBG_SWDIAG_LEDATA_ACL_TX_OFFSET               = 1,
	DBG_SWDIAG_LEDATA_ACL_TX_MASK                 = 0x02,

	DBG_SWDIAG_LEDATA_LLCP_RX_OFFSET              = 2,
	DBG_SWDIAG_LEDATA_LLCP_RX_MASK                = 0x04,

	DBG_SWDIAG_LEDATA_ACL_RX_OFFSET               = 3,
	DBG_SWDIAG_LEDATA_ACL_RX_MASK                 = 0x08,

	DBG_SWDIAG_LEDATA_LLCP_ACK_OFFSET             = 4,
	DBG_SWDIAG_LEDATA_LLCP_ACK_MASK               = 0x10,

	DBG_SWDIAG_LEDATA_ACL_ACK_OFFSET              = 5,
	DBG_SWDIAG_LEDATA_ACL_ACK_MASK                = 0x20,

	/**
	******************************************************************************************
	* @brief BANK 35 : LETX
	******************************************************************************************
	*/
    DBG_SWDIAG_LETX                               = 35,

    DBG_SWDIAG_LETX_LLCP_TX_OFFSET                = 0,
    DBG_SWDIAG_LETX_LLCP_TX_MASK                  = 0x01,

    DBG_SWDIAG_LETX_ACL_TX_OFFSET                 = 1,
    DBG_SWDIAG_LETX_ACL_TX_MASK                   = 0x02,

    DBG_SWDIAG_LETX_SW_IDX_OFFSET                 = 2,
    DBG_SWDIAG_LETX_SW_IDX_MASK                   = 0x04,

    DBG_SWDIAG_LETX_HW_IDX_OFFSET                 = 3,
    DBG_SWDIAG_LETX_HW_IDX_MASK                   = 0x08,

    DBG_SWDIAG_LETX_TX_CNT_OFFSET                 = 4,
    DBG_SWDIAG_LETX_TX_CNT_MASK                   = 0x30,

    /// Number of SW profiles - to increase when new bank are added
    DBG_SWDIAG_NB_PROFILES                      = 36,
};


/*
 * GLOBAL VARIABLE DECLARATION
 ****************************************************************************************
 */

/// Allocate HW banks to SW profiles
extern uint8_t sw_to_hw[DBG_SWDIAG_NB_PROFILES];


/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
******************************************************************************************
* @brief Initialize SW profiling module
******************************************************************************************
*/
void dbg_swdiag_init(void);

/**
******************************************************************************************
* @brief Read the SW profiling configuration.
*
* @return SW profile configuration
******************************************************************************************
*/
uint32_t dbg_swdiag_read(void);

/**
******************************************************************************************
* @brief Write the SW profiling configuration.
*
* @param[in] profile  SW profiling configuration to apply
******************************************************************************************
*/
void dbg_swdiag_write(uint32_t profile);

#endif //RW_SWDIAG

/// @} DBGSWDIAG

#endif // DBG_SWDIAG_H_

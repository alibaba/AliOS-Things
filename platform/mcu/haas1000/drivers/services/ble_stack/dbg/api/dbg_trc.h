#ifndef DBG_TRC_H_
#define DBG_TRC_H_

/**
****************************************************************************************
* @addtogroup TRACER
* @ingroup TRACER
* @brief Tracer module sends traces to the PC through UART
* @{
****************************************************************************************
*/

#include "rwip_config.h"
#if (TRACER_PRESENT)

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>

#if (TRC_KE_MSG || TRC_KE_TMR)
#include "ke_msg.h"
#endif /*(TRC_KE_MSG || TRC_KE_TMR)*/

#if (TRC_EA)
#include "ea.h"
#endif /*(TRC_EA) */

/*
 * DEFINES
 ****************************************************************************************
 */
#define TRC_GRP_B(__t) (__t##_BIT)
#define TRC_GRP_P(__t) (__t##_POS)

#define TRC_MSG_TYPE 0x06

#define TRC_OPCODE_LEN      (1)
#define TRC_PDULEN_LEN      (2)
#define TRC_MSG_HDR_LEN     (TRC_OPCODE_LEN + TRC_PDULEN_LEN)

#define TRC_CFG_WORD_LEN    (4)

/*
 * ENUMERATION DEFINITION
 ****************************************************************************************
 */
/// Possible types of trace
enum trace_types
{
    KE_MSG_SEND_QUEUE   = 0x00,
    KE_MSG_HANDLED      = 0x01,
    KE_TMR_SET          = 0x02,
    KE_TMR_CLR          = 0x03,
    KE_TMR_EXP          = 0x04,
    KE_EVT_SET          = 0x05,
    KE_EVT_HANDLED      = 0x06,
    MEM_ALLOC           = 0x07,
    MEM_FREE            = 0x08,
    SLEEP_ENTER         = 0x09,
    SLEEP_WAKEUP        = 0x0A,
    ASSERT_WARNING      = 0x0B,
    ASSERT_ERROR        = 0x0C,
    ET_PROG             = 0x0D,
    CONN_EVT_CNT        = 0x0E,
    FRM_CMP_BT          = 0x0F,
    FRM_CMP_BLE         = 0x10,
    CS_BLE              = 0x11,
    CS_BT               = 0x12,
    RX_DESC             = 0x13,
    LLCP_TX             = 0x14,
    LLCP_RX             = 0x15,
    LLCP_ACK            = 0x16,
    LMP_TX              = 0x17,
    LMP_RX              = 0x18,
    LMP_ACK             = 0x19,
    L2CAP_TX            = 0x1A,
    L2CAP_RX            = 0x1B,
    L2CAP_ACK           = 0x1C,
    EA_REQ              = 0x1D,
    EA_CANC             = 0x1E,
    EA_REM              = 0x1F,
    EA_START            = 0x20,
    EA_SHIFT            = 0x21,
    LC_ST               = 0x22,
    LLC_ST              = 0x23,
    HCI_CMD             = 0x24,
    HCI_EVT             = 0x25,
    CUSTOM              = 0x26,
};

/// Configuration word bit field
///
///     7       6       5       4       3       2       1       0
/// +-------+-------+-------+-------+-------+-------+-------+-------+
/// |CS_BLE | PROG  |SW_ASS | SLEEP |  MEM  |KE_EVT |KE_TMR |KE_MSG |
/// +-------+-------+-------+-------+-------+-------+-------+-------+
///
///    15      14      13      12      11       10      9       8
/// +-------+-------+-------+-------+-------+-------+-------+-------+
/// | LC_ST |LLC_ST |  EA   | L2CAP |  LMP  | LLCP  |RX_DESC| CS_BT |
/// +-------+-------+-------+-------+-------+-------+-------+-------+
///
///    23      22      21      20      19       18      17      16
/// +-------+-------+-------+-------+-------+-------+-------+-------+
/// |       |       |       |       |       |       |CUSTOM |  HCI  |
/// +-------+-------+-------+-------+-------+-------+-------+-------+
///
///    31      30      29      28       27      26      25      24
/// +-------+-------+-------+-------+-------+-------+-------+-------+
/// |       |       |       |       |       |       |       |       |
/// +-------+-------+-------+-------+-------+-------+-------+-------+
///
enum dbg_trc_cfg_fields
{
    /// Kernel message
    TRC_KE_MSG_POS              = 0,
    TRC_KE_MSG_BIT              = 0x00000001,

    /// Kernel timer
    TRC_KE_TMR_POS              = 1,
    TRC_KE_TMR_BIT              = 0x00000002,

    /// Kernel event
    TRC_KE_EVT_POS              = 2,
    TRC_KE_EVT_BIT              = 0x00000004,

    /// Memory allocation and deallocation
    TRC_MEM_POS                 = 3,
    TRC_MEM_BIT                 = 0x00000008,

    /// Sleep mode
    TRC_SLEEP_POS               = 4,
    TRC_SLEEP_BIT               = 0x00000010,

    /// Software Assert
    TRC_SW_ASS_POS              = 5,
    TRC_SW_ASS_BIT              = 0x00000020,

    /// Programming of the exchange table, updating of the event counter and handling of "End of Event" interrupt
    TRC_PROG_POS                = 6,
    TRC_PROG_BIT                = 0x00000040,

    /// BLE Control structure
    TRC_CS_BLE_POS              = 7,
    TRC_CS_BLE_BIT              = 0x00000080,

    /// BT Control structure
    TRC_CS_BT_POS               = 8,
    TRC_CS_BT_BIT               = 0x00000100,

    /// Processing of RX descriptors at each LLD driver
    TRC_RX_DESC_POS             = 9,
    TRC_RX_DESC_BIT             = 0x00000200,

    /// LLCP transmission, reception and acknowledgment
    TRC_LLCP_POS                = 10,
    TRC_LLCP_BIT                = 0x00000400,

    /// LMP transmission, reception and acknowledgment
    TRC_LMP_POS                 = 11,
    TRC_LMP_BIT                 = 0x00000800,

    /// L2CAP transmission, reception and acknowledgment
    TRC_L2CAP_POS               = 12,
    TRC_L2CAP_BIT               = 0x00001000,

    /// Scheduling request, cancellation, shift and remove
    TRC_EA_POS                  = 13,
    TRC_EA_BIT                  = 0x00002000,

    /// LLC state transition
    TRC_LLC_STATE_TRANS_POS     = 14,
    TRC_LLC_STATE_TRANS_BIT     = 0x00004000,

    /// LC state transition
    TRC_LC_STATE_TRANS_POS      = 15,
    TRC_LC_STATE_TRANS_BIT      = 0x00008000,

    /// HCI messages (in Full embedded mode)
    TRC_HCI_POS                 = 16,
    TRC_HCI_BIT                 = 0x00010000,

    /// Custom trace
    TRC_CUSTOM_POS              = 17,
    TRC_CUSTOM_BIT              = 0x00020000,
};

/// Tracer packet types
enum dbg_trc_pkt_types
{
    /// Trace message
    TRACE       = 0x01,
    /// Configuration message
    TRACER_CFG  = 0x02,
    /// Acknowledgment message
    TRACER_ACK  = 0x03
};

#if (TRC_RX_DESC)
/// LLD driver types
enum driver_types
{
    LLD_SCAN    = 0x01,
    LLD_INIT    = 0x02,
    LLD_ADV     = 0x03,
    LLD_CON     = 0x04
};
#endif /*(TRC_RX_DESC)*/

#if (TRC_CUSTOM)
/// Custom trace types
enum custom_trace_types
{
    UINT8_T     =   0x01,
    UINT16_T    =   0x02,
    UINT32_T    =   0x03,
    VOID        =   0x04
};
#endif /*(TRC_CUSTOM)*/
#endif /*(TRACER_PRESENT)*/

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************************************
 */


/*
 * FUNCTION DECLARATION
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * @brief Request a new trace
 *
 * This function checks the current configuration of the tracer
 *
 ****************************************************************************************
 */
#if (TRACER_PRESENT && TRC_KE_MSG)
/**
 ****************************************************************************************
 * @brief Trace request API for kernel message pushed in the send queue
 *
 * This function is called when a kernel message is pushed in the send queue.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "kernel message" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   msg     Kernel message
 ****************************************************************************************
 */
void dbg_trc_req_ke_msg_send(struct ke_msg const *msg);

/// Macro used to send trace request API for kernel message send
#define TRC_REQ_KE_MSG_SEND(msg) \
        dbg_trc_req_ke_msg_send(msg);
        
/**
 ****************************************************************************************
 * @brief Trace request API for kernel message handled
 *
 * This function is called when a task handles a message
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "kernel message" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   id          Kernel message id
 * @param[in]   dest_id     Destination kernel identifier
 * @param[in]   src_id      Source kernel identifier
 * @param[in]   msg_status  Status returned by the kernel message handler
 ****************************************************************************************
 */
void dbg_trc_req_ke_msg_handled(ke_msg_id_t const id, ke_task_id_t const dest_id, ke_task_id_t const src_id, uint8_t const msg_status);

/// Macro used to send trace request API for kernel message handled
#define TRC_REQ_KE_MSG_HANDLED(id, dest_id, src_id, msg_status) \
        dbg_trc_req_ke_msg_handled(id, dest_id, src_id, msg_status);
#else /*TRACER_PRESENT && TRC_KE_MSG*/
#define TRC_REQ_KE_MSG_SEND(msg)
#define TRC_REQ_KE_MSG_HANDLED(id, dest_id, src_id, msg_status)
#endif /*TRACER_PRESENT && TRC_KE_MSG*/

#if (TRACER_PRESENT && TRC_KE_TMR)
/**
 ****************************************************************************************
 * @brief Trace request API for kernel timer
 *
 * This function is called when a kernel timer is set, cleared or expired.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "kernel timer" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   tmr_trc_type        Trace type for kernel timer (timer set, cleared and expired of @see enum trace_types)
 * @param[in]   target_time         Timer time value
 * @param[in]   task_id             Timer task identifier
 * @param[in]   msg_id              Timer message identifier
 ****************************************************************************************
 */
void dbg_trc_req_ke_tmr(uint8_t const tmr_trc_type, uint32_t const target_time, ke_task_id_t const task_id, ke_msg_id_t const msg_id);

/// Macro used to send trace request API for kernel timer set
#define TRC_REQ_KE_TMR_SET(target_time, task_id, msg_id) \
        dbg_trc_req_ke_tmr(KE_TMR_SET, target_time, task_id, msg_id);

/// Macro used to send trace request API for kernel timer cleared
#define TRC_REQ_KE_TMR_CLR(target_time, task_id, msg_id) \
        dbg_trc_req_ke_tmr(KE_TMR_CLR,target_time, task_id, msg_id);

/// Macro used to send trace request API for kernel timer expired
#define TRC_REQ_KE_TMR_EXP(target_time, task_id, msg_id) \
        dbg_trc_req_ke_tmr(KE_TMR_EXP,target_time, task_id, msg_id);
#else /*(TRACER_PRESENT && TRC_KE_TMR)*/
#define TRC_REQ_KE_TMR_SET(target_time, task_id, msg_id)
#define TRC_REQ_KE_TMR_CLR(target_time, task_id, msg_id)
#define TRC_REQ_KE_TMR_EXP(target_time, task_id, msg_id)
#endif /*(TRACER_PRESENT && TRC_KE_TMR)*/

#if (TRACER_PRESENT && TRC_KE_EVT)
/**
 ****************************************************************************************
 * @brief Trace request API for kernel event
 *
 * This function is called when a kernel event is set or handled.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "kernel event" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   evt_req_type        Trace type for kernel event (event set and handled of @see enum trace_types)
 * @param[in]   event_type          Type of event
 ****************************************************************************************
 */
void dbg_trc_req_ke_evt(uint8_t const evt_trc_type, uint8_t const event_type);

/// Macro used to send trace request API for kernel event set
#define TRC_REQ_KE_EVT_SET(evt_type)                    \
        dbg_trc_req_ke_evt(KE_EVT_SET, evt_type);

/// Macro used to send trace request API for kernel event handled
#define TRC_REQ_KE_EVT_HANDLED(evt_type)                \
        dbg_trc_req_ke_evt(KE_EVT_HANDLED, evt_type);
#else /*(TRACER_PRESENT && TRC_KE_EVT)*/
#define TRC_REQ_KE_EVT_SET(evt_type)
#define TRC_REQ_KE_EVT_HANDLED(evt_type)
#endif /*(TRACER_PRESENT && TRC_KE_EVT)*/

#if (TRACER_PRESENT && TRC_MEM)
/**
 ****************************************************************************************
 * @brief Trace request API for memory management
 *
 * This function is called when the memory is allocated or freed.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "memory" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   mem_trc_type    Trace type for memory management (memory alloc and free of @see enum trace_types)
 * @param[in]   heap_id         Heap identifier
 * @param[in]   blk_address     Address of the used memory block
 * @param[in]   size            Size of the memory area that has been allocated or freed.
 ****************************************************************************************
 */
void dbg_trc_req_mem(uint8_t const mem_trc_type, uint8_t const heap_id, void const *blk_address, uint32_t const size);

/// Macro used to send trace request API for memory allocation
#define TRC_REQ_MEM_ALLOC(heap_id, blk_address, size)   \
        dbg_trc_req_mem(MEM_ALLOC, heap_id, blk_address, size);

/// Macro used to send trace request API for memory deallocation
#define TRC_REQ_MEM_FREE(heap_id, blk_address, size)    \
        dbg_trc_req_mem(MEM_FREE, heap_id, blk_address, size);

#else /*(TRACER_PRESENT && TRC_MEM)*/
#define TRC_REQ_MEM_ALLOC(heap_id, blk_address, size)
#define TRC_REQ_MEM_FREE(heap_id, blk_address, size)
#endif/*(TRACER_PRESENT && TRC_MEM)*/

#if (TRACER_PRESENT && TRC_SLEEP)
/**
 ****************************************************************************************
 * @brief Trace request API for sleep mode
 *
 * This function is called when the IP enter in deep sleep and when it wakes up.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "sleep" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   sleep_trc_type  Trace type for sleep mode (sleep enter and wakeup of @see enum trace_types)
 ****************************************************************************************
 */
void dbg_trc_req_deep_sleep(uint8_t const sleep_trc_type);

/// Macro used to send trace request API when the IP enters in deep sleep
#define TRC_REQ_SLEEP()                 \
        dbg_trc_req_deep_sleep(SLEEP_ENTER);

/// Macro used to send trace request API when the IP wakes up from deep sleep
#define TRC_REQ_WAKEUP()                \
        dbg_trc_req_deep_sleep(SLEEP_WAKEUP);

#else /*(TRACER_PRESENT && TRC_SLEEP)*/
#define TRC_REQ_SLEEP()
#define TRC_REQ_WAKEUP()
#endif /*(TRACER_PRESENT && TRC_SLEEP)*/

#if (TRACER_PRESENT && TRC_SW_ASS)
/**
 ****************************************************************************************
 * @brief Trace request API for software asserts
 *
 * This function is called when an assertion statement is evaluated as true.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "sw assertions" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   ass_trc_type    Trace type for assertions (assert warning and assert error of @see enum trace_types)
 * @param[in]   filename        Name of the file that triggered the assertion
 * @param[in]   code_line       Line of code where the assertion was triggered
 * @param[in]   param0          Parameter value 0
 * @param[in]   param1          Parameter value 1
 ****************************************************************************************
 */
void dbg_trc_req_sw_ass(uint8_t const ass_trc_type,  char const *filename, uint16_t const code_line, uint32_t const param0, uint32_t const param1);

/// Macro used to send trace request API when an assert warning is evaluated as true
#define TRC_REQ_SW_ASS_WARN(filename, code_line, param0, param1)                     \
        dbg_trc_req_sw_ass(ASSERT_WARNING, filename, code_line, param0, param1);

/// Macro used to send trace request API when an assert warning is evaluated as true
#define TRC_REQ_SW_ASS_ERR(filename, code_line, param0, param1)                     \
        dbg_trc_req_sw_ass(ASSERT_ERROR, filename, code_line, param0, param1);

#else /*(TRACER_PRESENT && TRC_SW_ASS)*/
#define TRC_REQ_SW_ASS_WARN(filename, code_line, param0, param1)
#define TRC_REQ_SW_ASS_ERR(filename, code_line, param0, param1)
#endif /*(TRACER_PRESENT && TRC_SW_ASS)*/

#if (TRACER_PRESENT && TRC_PROG)
/**
 ****************************************************************************************
 * @brief Trace request API for exchange table programming
 *
 * This function is called when the exchange table is programmed.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "programming" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   elt_idx     Index of the programmed entry in the ET
 ****************************************************************************************
 */
void dbg_trc_req_et_prog(uint8_t const elt_idx);

#define TRC_REQ_ET_PROG(elt_idx) \
		dbg_trc_req_et_prog(elt_idx);

/**
 ****************************************************************************************
 * @brief Trace request API for connection event counter programming
 *
 * This function is called when the connection event counter is set.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "programming" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   conhdl       Connection handle
 * @param[in]   value        Value of connection event counter
 ****************************************************************************************
 */
void dbg_trc_req_evt_cnt(uint16_t const conhdl, uint16_t const value);
#define TRC_REQ_EVT_CNT(conhdl, value) \
		dbg_trc_req_evt_cnt(conhdl, value);
/**
 ****************************************************************************************
 * @brief Trace request API for frame completion
 *
 * This function is called when a frame is completed.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "programming" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   timestamp   Timestamp of the programmed frame (in BLE slots, based on local clock)
 * @param[in]   frm_id      Frame identifier
 * @param[in]   frm_cbk     Callback for handling interrupts related to the frame
 ****************************************************************************************
 */
void dbg_trc_req_frm_cmp(uint32_t const timestamp, uint8_t const frm_id, uint32_t const frm_cbk);

#define TRC_REQ_FRM_CMP(timestamp, frm_id, frm_cbk) \
		dbg_trc_req_frm_cmp(timestamp, frm_id, frm_cbk);

#else /*(TRACER_PRESENT && TRC_PROG)*/
#define TRC_REQ_ET_PROG(elt_idx)
#define TRC_REQ_EVT_CNT(conhdl, value)
#define TRC_REQ_FRM_CMP(timestamp, frm_id, frm_cbk)
#endif /*(TRACER_PRESENT && TRC_PROG)*/

#if (TRACER_PRESENT && TRC_CS_BLE)
/**
 ****************************************************************************************
 * @brief Trace request API for control structure programming in BLE
 *
 * This function is called when a control structure is modified.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "BLE control structure" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   cs_addr     Address of the control structure in the exchange memory
 ****************************************************************************************
 */
void dbg_trc_req_cs_ble(uint16_t const cs_addr);

#define TRC_REQ_CS_BLE(cs_addr) \
		dbg_trc_req_cs_ble(cs_addr);
		
#else /*(TRACER_PRESENT && TRC_CS_BLE)*/
#define TRC_REQ_CS_BLE(cs_addr)
#endif /*(TRACER_PRESENT && TRC_CS_BLE)*/

#if (TRACER_PRESENT && TRC_CS_BT)
/**
 ****************************************************************************************
 * @brief Trace request API for control structure programming in BT
 *
 * This function is called when a control structure is modified.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "BT control structure" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   cs_addr     Address of the control structure in the exchange memory
 ****************************************************************************************
 */
void dbg_trc_req_cs_bt(uint16_t const cs_addr);

#define TRC_REQ_CS_BT(cs_addr) \
		dbg_trc_req_cs_bt(cs_addr);
#else /*(TRACER_PRESENT && TRC_CS_BT)*/
#define TRC_REQ_CS_BT(cs_addr)
#endif /*(TRACER_PRESENT && TRC_CS_BT)*/

#if (TRACER_PRESENT && TRC_RX_DESC)
/**
 ****************************************************************************************
 * @brief Trace request API for processing of rx descriptors
 *
 * This function is called when a packet is received and a driver processes a rx descriptor.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "Rx descriptor" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   drv_type    LLD driver (@see enum driver_type)
 * @param[in]   act_id      Activity identifier
 * @param[in]   rx_desc_add Address of the rx descriptor in the exchange memory
 ****************************************************************************************
 */
void dbg_trc_req_rx_desc(uint8_t const drv_type, uint8_t const act_id, uint16_t const rx_desc_add);

#define TRC_REQ_RX_DESC(drv_type, act_id, rx_desc_add) \
		dbg_trc_req_rx_desc(drv_type, act_id, rx_desc_add);

#else /*(TRACER_PRESENT && TRC_RX_DESC)*/
#define TRC_REQ_RX_DESC(drv_type, act_id, rx_desc_add)
#endif /*(TRACER_PRESENT && TRC_RX_DESC) */

#if (TRACER_PRESENT && TRC_LLCP)
/**
 ****************************************************************************************
 * @brief Trace request API for llcp packets transmission, reception and acknowledgment
 *
 * This function is called when a llcp packet is transmitted, received or acknowledged.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "LLCP" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   llcp_trc_type   Trace type for llcp (llcp tx, llcp rx and llcp ack of @see enum trace_types)
 * @param[in]   conhdl        Connection handle
 * @param[in]   data_len        Data length (in bytes)
 * @param[in]   pdu_data        PDU Data
 ****************************************************************************************
 */
void dbg_trc_req_llcp(uint8_t const llcp_trc_type, uint16_t const conhdl, uint8_t const data_len, uint8_t const *pdu_data);

/// Macro used to send trace request API when an llcp packet is transmitted
#define TRC_REQ_LLCP_TX(conhdl, data_len, pdu_data)               \
        dbg_trc_req_llcp(LLCP_TX, conhdl, data_len, pdu_data);

/// Macro used to send trace request API when an llcp packet is received
#define TRC_REQ_LLCP_RX(conhdl, data_len, pdu_data)               \
        dbg_trc_req_llcp(LLCP_RX, conhdl, data_len, pdu_data);

/// Macro used to send trace request API when an llcp packet is acknowledged
#define TRC_REQ_LLCP_ACK(conhdl, data_len, pdu_data)              \
        dbg_trc_req_llcp(LLCP_ACK, conhdl, data_len, pdu_data);
#else /*(TRACER_PRESENT && TRC_LLCP)*/
#define TRC_REQ_LLCP_TX(conhdl, data_len, pdu_data)
#define TRC_REQ_LLCP_RX(conhdl, data_len, pdu_data)
#define TRC_REQ_LLCP_ACK(conhdl, data_len, pdu_data)
#endif /*(TRACER_PRESENT && TRC_LLCP) */

#if (TRACER_PRESENT && TRC_LMP)
/**
 ****************************************************************************************
 * @brief Trace request API for lmp packets transmission, reception and acknowledgment
 *
 * This function is called when a lmp packet is transmitted, received or acknowledged.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "LMP" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   llcp_trc_type   Trace type for lmp (lmp tx, lmp rx and lmp ack of @see enum trace_types)
 * @param[in]   conhdl        Connection handle
 * @param[in]   data_len        Data length (in bytes)
 * @param[in]   pdu_data        PDU Data
 ****************************************************************************************
 */
void dbg_trc_req_lmp(uint8_t const lmp_trc_type, uint16_t const conhdl, uint8_t const data_len, uint8_t const *pdu_data);

/// Macro used to send trace request API when an lmp packet is transmitted
#define TRC_REQ_LMP_TX(conhdl, data_len, pdu_data)               \
        dbg_trc_req_lmp(LMP_TX, conhdl, data_len, pdu_data);

/// Macro used to send trace request API when an lmp packet is received
#define TRC_REQ_LMP_RX(conhdl, data_len, pdu_data)               \
        dbg_trc_req_lmp(LMP_RX, conhdl, data_len, pdu_data);

/// Macro used to send trace request API when an lmp packet is acknowledged
#define TRC_REQ_LMP_ACK(conhdl, data_len, pdu_data)              \
        dbg_trc_req_lmp(LMP_ACK, conhdl, data_len, pdu_data);

#else /*(TRACER_PRESENT && TRC_LMP)*/
#define TRC_REQ_LMP_TX(conhdl, data_len, pdu_data)
#define TRC_REQ_LMP_RX(conhdl, data_len, pdu_data)
#define TRC_REQ_LMP_ACK(conhdl, data_len, pdu_data)
#endif /*(TRACER_PRESENT && TRC_LMP)*/

#if (TRACER_PRESENT && TRC_L2CAP)
/**
 ****************************************************************************************
 * @brief Trace request API for l2cap packets transmission and reception
 *
 * This function is called when a l2cap packet is transmitted or received.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "L2CAP" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   l2cap_trc_type  Trace type for l2cap (l2cap tx and l2cap rx of @see enum trace_types)
 * @param[in]   conhdl        Connection handle
 * @param[in]   data_len        Data length (in bytes)
 * @param[in]   buf_ptr         Memory pointer address
 ****************************************************************************************
 */
void dbg_trc_req_l2cap(uint8_t const l2cap_trc_type, uint16_t const conhdl, uint16_t const data_len, uint32_t const buf_ptr);

/// Macro used to send trace request API when an l2cap packet is transmitted
#define TRC_REQ_L2CAP_TX(conhdl, tx_len, buf_ptr)               \
        dbg_trc_req_l2cap(L2CAP_TX, conhdl, tx_len, buf_ptr);

/// Macro used to send trace request API when an l2cap packet is received
#define TRC_REQ_L2CAP_RX(conhdl, tx_len, buf_ptr)               \
        dbg_trc_req_l2cap(L2CAP_RX, conhdl, tx_len, buf_ptr);

/**
 ****************************************************************************************
 * @brief Trace request API for l2cap packet acknowledgment
 *
 * This function is called when l2cap packets are acknowledged.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "L2CAP" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   conhdl        Connection handle
 * @param[in]   num_ack_pkts    Number of acknowledged l2cap packets
 ****************************************************************************************
 */
void dbg_trc_req_l2cap_ack(uint16_t const conhdl, uint16_t const num_ack_pkts);

#define TRC_REQ_L2CAP_ACK(conhdl, num_ack_pkts)               \
        dbg_trc_req_l2cap_ack(conhdl, num_ack_pkts);

#else /*(TRACER_PRESENT && TRC_L2CAP)*/
#define TRC_REQ_L2CAP_TX(conhdl, tx_len, buf_ptr)
#define TRC_REQ_L2CAP_RX(conhdl, tx_len, buf_ptr)
#define TRC_REQ_L2CAP_ACK(conhdl, num_ack_pkts)
#endif/*(TRACER_PRESENT && TRC_L2CAP) */

#if (TRACER_PRESENT && TRC_EA)
/**
 ****************************************************************************************
 * @brief Trace request API for cancellation, removal and starting of events
 *
 * This function is called when an event starts, is removed or is cancelled.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "EA" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   ea_trc_type     Trace type for event arbiter (cancel, remove and start of @see enum trace_types)
 * @param[in]   ea_elt          Pointer to the event arbiter element
 ****************************************************************************************
 */
void dbg_trc_req_ea(uint8_t const ea_trc_type,const struct ea_elt_tag *ea_elt);

/// Macro used to send trace request API when an event is cancelled
#define TRC_REQ_EA_CANC(ea_elt)                 \
        dbg_trc_req_ea(EA_CANC, ea_elt);

/// Macro used to send trace request API when an event is removed
#define TRC_REQ_EA_REM(ea_elt)                  \
        dbg_trc_req_ea(EA_REM, ea_elt);

/// Macro used to send trace request API when an event starts
#define TRC_REQ_EA_START(ea_elt)                \
        dbg_trc_req_ea(EA_START, ea_elt);

/**
 ****************************************************************************************
 * @brief Trace request API for shifting of events
 *
 * This function is called when an event is shifted.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "EA" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   ea_elt          Pointer to the event arbiter element
 ****************************************************************************************
 */
void dbg_trc_req_ea_shift(const struct ea_elt_tag *ea_elt);

#define TRC_REQ_EA_SHIFT(ea_elt)                \
		dbg_trc_req_ea_shift(ea_elt);
/**
 ****************************************************************************************
 * @brief Trace request API for pushing of event in the waiting queue
 *
 * This function is called when an event is pushed in the waiting queue.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "EA" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   ea_elt          Pointer to the event arbiter element
 ****************************************************************************************
 */
void dbg_trc_req_ea_insert(uint32_t const target_time, uint8_t const status, const struct ea_elt_tag *ea_elt);

#define TRC_REQ_EA_INSERT(target_time, status, ea_elt)		\
		dbg_trc_req_ea_insert(target_time, status, ea_elt);

#else /*(TRACER_PRESENT && TRC_EA)*/
#define TRC_REQ_EA_CANC(ea_elt)
#define TRC_REQ_EA_REM(ea_elt)
#define TRC_REQ_EA_START(ea_elt)
#define TRC_REQ_EA_SHIFT(ea_elt)
#define TRC_REQ_EA_INSERT(target_time, status, ea_elt)
#endif /*(TRACER_PRESENT && TRC_EA)*/

#if (TRACER_PRESENT && TRC_LC_STATE_TRANS)
/**
 ****************************************************************************************
 * @brief Trace request API for lc procedure state transition
 *
 * This function is called when an lc procedure changes its state.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "LC state transition" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   conhdl        Connection handle
 * @param[in]   proc_type       LC procedure
 * @param[in]   prev_state      Previous procedure state
 * @param[in]   curr_state      Current procedure state
 ****************************************************************************************
 */
void dbg_trc_req_lc_state_trans(uint16_t const conhdl, uint8_t const proc_type, uint8_t const prev_state, uint8_t const curr_state);

#define TRC_REQ_LC_STATE_TRANS(conhdl, proc_type, prev_state, curr_state)		\
		dbg_trc_req_lc_state_trans(conhdl, proc_type, prev_state, curr_state);

#else /*(TRACER_PRESENT && TRC_LC_STATE_TRANS)*/
#define TRC_REQ_LC_STATE_TRANS(conhdl, proc_type, prev_state, curr_state)
#endif /*(TRACER_PRESENT && TRC_LC_STATE_TRANS)*/

#if (TRACER_PRESENT && TRC_LLC_STATE_TRANS)
/**
 ****************************************************************************************
 * @brief Trace request API for llc procedure state transition
 *
 * This function is called when an llcp procedure changes its state.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "LLC state transition" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   conhdl        Connection handle
 * @param[in]   proc_type       LLC procedure
 * @param[in]   prev_state      Previous procedure state
 * @param[in]   curr_state      Current procedure state
 ****************************************************************************************
 */
void dbg_trc_req_llc_state_trans(uint16_t const conhdl, uint8_t const proc_type, uint8_t const prev_state, uint8_t const curr_state);

#define TRC_REQ_LLC_STATE_TRANS(conhdl, proc_type, prev_state, curr_state)		\
		dbg_trc_req_llc_state_trans(conhdl, proc_type, prev_state, curr_state);

#else /*(TRACER_PRESENT && TRC_LLC_STATE_TRANS)*/
#define TRC_REQ_LLC_STATE_TRANS(conhdl, proc_type, prev_state, curr_state)
#endif /*(TRACER_PRESENT && TRC_LLC_STATE_TRANS)*/

#if (TRACER_PRESENT && TRC_HCI)
/**
 ****************************************************************************************
 * @brief Trace request API for HCI messages (in FE mode)
 *
 * This function is called when a HCI command is received or a HCI event is sent.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "HCI" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   hci_msg_type    Type of hci message (HCI command and event of @see enum trace_types)
 * @param[in]   opcode          HCI opcode
 * @param[in]   par_len         HCI parameter length
 * @param[in]   params          unpacked HCI parameters
 ****************************************************************************************
 */
void dbg_trc_req_hci(uint8_t const hci_msg_type, uint16_t const opcode, uint16_t const par_len, uint32_t const *params);

/// Macro used to send trace request API when an HCI command is received
#define TRC_REQ_HCI_CMD(opcode, par_len, params)    \
    dbg_trc_req_hci(HCI_CMD, opcode, par_len, params);

/// Macro used to send trace request API when an HCI event is sent
#define TRC_REQ_HCI_EVT(opcode, par_len, params)    \
    dbg_trc_req_hci(HCI_EVT, opcode, par_len, params);
    
#else /*(TRACER_PRESENT && TRC_HCI)*/
#define TRC_REQ_HCI_CMD(opcode, par_len, params)
#define TRC_REQ_HCI_EVT(opcode, par_len, params)
#endif /*(TRACER_PRESENT && TRC_HCI)*/

#if (TRACER_PRESENT && TRC_CUSTOM)
/**
 ****************************************************************************************
 * @brief Trace request API for custom traces
 *
 * This function is called to send a custom trace.
 *
 * It checks the current tracer configuration and writes the trace in memory if the trace
 * group "custom" (@see enum tracer_cfg_fields) is enabled.
 *
 * @param[in]   trc_id      Trace request identifier (progressive number that identifies the caller)
 * @param[in]   data_type   Type of data (@see enum custom_trace_type)
 * @param[in]   data        Pointer to the data to send
 ****************************************************************************************
 */
void dbg_trc_req_custom(const uint8_t trc_id, const uint8_t data_type, void const *data);
#define TRC_REQ_CUSTOM(trc_id, data_type, data)    \
        dbg_trc_req_custom(trc_id, data_type, data);
#else /*(TRACER_PRESENT && TRC_CUSTOM)*/
#define TRC_REQ_CUSTOM(trc_id, data_type, data)
#endif /*(TRACER_PRESENT && TRC_CUSTOM)*/

#if (TRACER_PRESENT)
/**
 ****************************************************************************************
 * @brief Configuration received
 *
 * This function checks if the received configuration can be adopted and acknowledges it
 *
 ****************************************************************************************
 */
void dbg_trc_cfg_received();

/**
 ****************************************************************************************
 * @brief Get the reception buffer for configuration word
 *
 * @return  Buffer for payload reception of tracer configuration packets
 ****************************************************************************************
 */
uint8_t* dbg_trc_pay_buff_get();
#endif /*(TRACER_PRESENT)*/
///@} TRACER
#endif // DBG_TRC_H_

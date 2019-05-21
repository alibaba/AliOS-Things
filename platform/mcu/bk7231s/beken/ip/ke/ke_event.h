/**
 ****************************************************************************************
 *
 * @file ke_event.h
 *
 * @brief This file contains the definition related to kernel events.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _KE_EVENT_H_
#define _KE_EVENT_H_

/// Format of an event handler function
typedef void (*evt_ptr_t)(int);

/// Structure defining an event callback
struct ke_evt_tag
{
    /// Pointer to the event function to call
    evt_ptr_t func;
    /// Parameter to pass to the event function
    int       param;
};

/**
 ****************************************************************************************
 * @defgroup KE_EVT KE_EVT
 * @ingroup KERNEL
 * @brief Event scheduling module.
 *
 * The KE_EVT module implements event scheduling functions. It can be used to
 * implement deferred actions:
 * @msc
 *  xxx_isr, KE_EVT, xxx_deferred;
 *  ---                     [label="ISR context of xxx module sets the event"];
 *  xxx_isr=>KE_EVT         [label="ke_evt_set(event_yyy)", URL="\ref ke_evt_set()"];
 *  ---                     [label="In high priority background context, events are scheduled"];
 *  KE_EVT=>KE_EVT          [label="ke_evt_schedule()", URL="\ref ke_evt_schedule()"];
 *  KE_EVT=>xxx_deferred    [label="xxx_event_hdlr()"];
 *  xxx_deferred=>KE_EVT    [label="ke_evt_get()", URL="\ref ke_evt_get()"];
 *  KE_EVT>>xxx_deferred    [label="event_yyy"];
 *  xxx_deferred=>KE_EVT    [label="ke_evt_clear(event_yyy)", URL="\ref ke_evt_clear()"];
 *  xxx_deferred=>xxx_deferred [label="process event_yyy"];
 * @endmsc
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwnx_config.h"
#include "ke_env.h"
#include "co_math.h"

#include "include.h"

/*
 * CONSTANTS
 ****************************************************************************************
 */
/** Offset of the different events in the event bit field.  0 is the highest priority and
 *  31 is the lowest.
 *  THESE ENUMS MUST NOT BE USED WITH ke_evt_xxx()! USE xxx_BIT BLOW.
 */
enum
{
    KE_EVT_RESET = 0,
    
    #if NX_MM_TIMER
    KE_EVT_MM_TIMER,
    #endif
    
    KE_EVT_KE_TIMER,
		
	#if NX_BEACONING
	KE_EVT_TXL_PAYLOAD_BCN,
	#endif
	
	KE_EVT_TXL_PAYLOAD_AC3,
	KE_EVT_TXL_PAYLOAD_AC2,
	KE_EVT_TXL_PAYLOAD_AC1,
	KE_EVT_TXL_PAYLOAD_AC0,
	
    KE_EVT_KE_MESSAGE,
    KE_EVT_HW_IDLE,
    
    #if NX_BEACONING || (!NX_MULTI_ROLE)
    KE_EVT_PRIMARY_TBTT,
    #endif
    
    #if NX_BEACONING
    KE_EVT_SECONDARY_TBTT,
    #endif
    
    KE_EVT_RXUREADY,

    #if NX_TX_FRAME
    KE_EVT_TXFRAME_CFM,
    #endif
    
    #if NX_BEACONING
    KE_EVT_TXCFM_BCN,
    #endif
    
    KE_EVT_TXCFM_AC3,
    KE_EVT_TXCFM_AC2,
    KE_EVT_TXCFM_AC1,
    KE_EVT_TXCFM_AC0,

    #if CFG_SDIO
    KE_EVT_SDIO_RXED_DATA,
    #endif
    #if CFG_SDIO_TRANS    
    KE_EVT_SDIO_TRANS_DATA,
    #endif


    #if NX_GP_DMA
    KE_EVT_GP_DMA_DL,
    #endif

	#if CFG_TX_EVM_TEST
    KE_EVT_EVM_VIA_MAC_TEST,
	#endif 

    KE_EVT_MAX
};

#define KE_EVT_RESET_BIT          CO_BIT(31 - KE_EVT_RESET)          ///< Reset event
#if NX_MM_TIMER
#define KE_EVT_MM_TIMER_BIT       CO_BIT(31 - KE_EVT_MM_TIMER)       ///< MM timer event
#endif
#define KE_EVT_KE_TIMER_BIT       CO_BIT(31 - KE_EVT_KE_TIMER)       ///< Kernel timer event
#define KE_EVT_KE_MESSAGE_BIT     CO_BIT(31 - KE_EVT_KE_MESSAGE)     ///< Kernel message event
#define KE_EVT_HW_IDLE_BIT        CO_BIT(31 - KE_EVT_HW_IDLE)        ///< IDLE state event

#if NX_BEACONING || (!NX_MULTI_ROLE)
#define KE_EVT_PRIMARY_TBTT_BIT   CO_BIT(31 - KE_EVT_PRIMARY_TBTT)   ///< Primary TBTT event
#else
#define KE_EVT_PRIMARY_TBTT_BIT   0
#endif

#if NX_BEACONING
#define KE_EVT_SECONDARY_TBTT_BIT CO_BIT(31 - KE_EVT_SECONDARY_TBTT) ///< Secondary TBTT event
#endif

#define KE_EVT_RXUREADY_BIT       CO_BIT(31 - KE_EVT_RXUREADY)       ///< RXU ready event

#define KE_EVT_RXUPLOADED_BIT     CO_BIT(31 - KE_EVT_RXUPLOADED)     ///< RX DMA event

#if NX_TX_FRAME
#define KE_EVT_TXFRAME_CFM_BIT    CO_BIT(31 - KE_EVT_TXFRAME_CFM)    ///< Internal frame confirmation event
#endif

#define KE_EVT_SDIO_RXED_DATA_BIT    CO_BIT(31 - KE_EVT_SDIO_RXED_DATA)
#if CFG_SDIO_TRANS
#define KE_EVT_SDIO_TRANS_DATA_BIT    CO_BIT(31 - KE_EVT_SDIO_TRANS_DATA)
#endif

#if NX_BEACONING
#define KE_EVT_TXL_PAYLOAD_BCN_BIT    CO_BIT(31 - KE_EVT_TXL_PAYLOAD_BCN)
#endif

#define KE_EVT_TXL_PAYLOAD_AC3_BIT    CO_BIT(31 - KE_EVT_TXL_PAYLOAD_AC3)
#define KE_EVT_TXL_PAYLOAD_AC2_BIT    CO_BIT(31 - KE_EVT_TXL_PAYLOAD_AC2)
#define KE_EVT_TXL_PAYLOAD_AC1_BIT    CO_BIT(31 - KE_EVT_TXL_PAYLOAD_AC1)
#define KE_EVT_TXL_PAYLOAD_AC0_BIT    CO_BIT(31 - KE_EVT_TXL_PAYLOAD_AC0)

#if NX_BEACONING
#define KE_EVT_TXCFM_BCN_BIT      CO_BIT(31 - KE_EVT_TXCFM_BCN)      ///< Tx confirmation event
#endif

#define KE_EVT_TXCFM_AC0_BIT      CO_BIT(31 - KE_EVT_TXCFM_AC0)      ///< Tx confirmation event
#define KE_EVT_TXCFM_AC1_BIT      CO_BIT(31 - KE_EVT_TXCFM_AC1)      ///< Tx confirmation event
#define KE_EVT_TXCFM_AC2_BIT      CO_BIT(31 - KE_EVT_TXCFM_AC2)      ///< Tx confirmation event
#define KE_EVT_TXCFM_AC3_BIT      CO_BIT(31 - KE_EVT_TXCFM_AC3)      ///< Tx confirmation event

#if NX_GP_DMA
#define KE_EVT_GP_DMA_DL_BIT      CO_BIT(31 - KE_EVT_GP_DMA_DL)      ///< General purpose DMA DL event
#endif

#if CFG_TX_EVM_TEST
#define KE_EVT_EVM_MAC_BIT      CO_BIT(31 - KE_EVT_EVM_VIA_MAC_TEST)      ///< General purpose DMA DL event
#endif

#if NX_BEACONING
#define KE_EVT_TXCFM_MASK   ( KE_EVT_TXCFM_AC0_BIT  \
                            | KE_EVT_TXCFM_AC1_BIT  \
                            | KE_EVT_TXCFM_AC2_BIT  \
                            | KE_EVT_TXCFM_AC3_BIT  \
                            | KE_EVT_TXCFM_BCN_BIT)
#else
#define KE_EVT_TXCFM_MASK   ( KE_EVT_TXCFM_AC0_BIT  \
                            | KE_EVT_TXCFM_AC1_BIT  \
                            | KE_EVT_TXCFM_AC2_BIT  \
                            | KE_EVT_TXCFM_AC3_BIT)
#endif

/// @}

/*
 * MACROS
 ****************************************************************************************
 */
/// Retrieves the pending events bit field
__INLINE evt_field_t ke_evt_get(void)
{
    return ke_env.evt_field;
}

/*
 * FUNCTION PROTOTYPES
 ****************************************************************************************
 */
void ke_evt_set(evt_field_t const);
void ke_evt_clear(evt_field_t const);
void ke_evt_schedule(void);
void ke_evt_core_scheduler(void);
void ke_evt_none_core_scheduler(void);
void ke_init(void);
void ke_flush(void);
uint32_t ke_get_ac_payload_bit(uint32_t ac);
#endif //_KE_EVENT_H_


/**
 ****************************************************************************************
 *
 * @file hal_machw.h
 *
 * @brief Definition of the API used to initialize and control the MAC HW
 *
 * File contains the API to perform the functions of the MAC HW initialization and
 * interrupt handling. This module initializes the MAC HW registers and handles the
 * different interrupts raised by MAC HW.
 *
 * Copyright (C) RivieraWaves 2011-2016
 *
 ****************************************************************************************
 */

#ifndef _HAL_MACHW_H_
#define _HAL_MACHW_H_

/**
 *****************************************************************************************
 * @defgroup HAL HAL
 * @ingroup LMAC
 * @brief MAC HW Abstraction Module
 * @{
 *****************************************************************************************
 */

/**
 *****************************************************************************************
 * @defgroup MACHW MACHW
 * @ingroup HAL
 * @brief MAC HW Control Module
 * @{
 *****************************************************************************************
 */
#include "hal_desc.h"
#include "reg_mac_core.h"
#include "reg_mac_pl.h"
#include "reg_mac_core.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#define KEY_ENTRY_MIN_ID		 24
#define KEY_ENTRY_MAX_ID		 63

#define MONITOR_FAILURE         ((UINT32)-1)
#define MONITOR_SUCCESS         (0)

/// Duration between AP TBTT and Beacon Transmission (in us, should be a multiple of 128us)
#define HAL_MACHW_BCN_TX_DELAY_US       (2048)

#if RW_MUMIMO_TX_EN
#define MU_MIMO_MASTER_TX_IRQ NXMAC_SEC_USER_TX_TRIGGER_BIT
/// Bits indicating activity on secondary users (separated trigger and buffer)
#define TX_SEC_IRQ_BITS  ( NXMAC_SEC_U_3AC_3_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_3AC_2_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_3AC_1_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_3AC_0_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_3AC_3_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_3AC_2_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_3AC_1_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_3AC_0_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_2AC_3_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_2AC_2_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_2AC_1_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_2AC_0_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_2AC_3_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_2AC_2_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_2AC_1_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_2AC_0_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_1AC_3_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_1AC_2_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_1AC_1_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_1AC_0_TX_BUF_TRIGGER_BIT |             \
                           NXMAC_SEC_U_1AC_3_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_1AC_2_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_1AC_1_TX_TRIGGER_BIT     |             \
                           NXMAC_SEC_U_1AC_0_TX_TRIGGER_BIT )
#else
#define MU_MIMO_MASTER_TX_IRQ 0
#endif

/**
 * ENUMERATIONS
 ****************************************************************************************
 */

/// For MAC HW States.
enum
{
    /// MAC HW IDLE State.
    HW_IDLE = 0,
    /// MAC HW RESERVED State.
    HW_RESERVED,
    /// MAC HW DOZE State.
    HW_DOZE,
    /// MAC HW ACTIVE State.
    HW_ACTIVE
};

enum
{
    /// 802.ll b
    MODE_802_11B = 0,
    /// 802.11 a
    MODE_802_11A,
    /// 802.11 g
    MODE_802_11G,
    /// 802.11n at 2.4GHz
    MODE_802_11N_2_4,
    /// 802.11n at 5GHz
    MODE_802_11N_5,
    /// Reserved for future use
    MODE_RESERVED,
    /// 802.11ac at 5GHz
    MODE_802_11AC_5
};

/// Mapping of HW timers
enum
{
    /// AC0 TX timeout
    HAL_AC0_TIMER = 0,
    /// AC1 TX timeout
    HAL_AC1_TIMER,
    /// AC2 TX timeout
    HAL_AC2_TIMER,
    /// AC3 TX timeout
    HAL_AC3_TIMER,
    /// BCN TX timeout
    HAL_BCN_TIMER,
    /// Go to IDLE timeout
    HAL_IDLE_TIMER,
    /// RX interrupt mitigation timeout
    HAL_RX_TIMER,
    #if NX_MM_TIMER
    /// MM timeout
    HAL_MM_TIMER,
    #endif
    /// Kernel timer
    HAL_KE_TIMER,

    /// Number of HW timers used
    HAL_TIMER_MAX,
};

/// Bits associated to HW timers
enum
{
    /// AC0 TX timeout bit
    HAL_AC0_TIMER_BIT = CO_BIT(HAL_AC0_TIMER),
    /// AC1 TX timeout bit
    HAL_AC1_TIMER_BIT = CO_BIT(HAL_AC1_TIMER),
    /// AC2 TX timeout bit
    HAL_AC2_TIMER_BIT = CO_BIT(HAL_AC2_TIMER),
    /// AC3 TX timeout bit
    HAL_AC3_TIMER_BIT = CO_BIT(HAL_AC3_TIMER),
    /// BCN TX timeout bit
    HAL_BCN_TIMER_BIT = CO_BIT(HAL_BCN_TIMER),
    /// Go to IDLE timeout bit
    HAL_IDLE_TIMER_BIT = CO_BIT(HAL_IDLE_TIMER),
    /// RX interrupt mitigation timeout bit
    HAL_RX_TIMER_BIT = CO_BIT(HAL_RX_TIMER),
    #if NX_MM_TIMER
    /// MM timeout bit
    HAL_MM_TIMER_BIT = CO_BIT(HAL_MM_TIMER),
    #endif
    /// Kernel timer bit
    HAL_KE_TIMER_BIT = CO_BIT(HAL_KE_TIMER),
};

typedef struct
{
	uint32_t  count;
	uint32_t  group_cipher_type;
	uint64_t  mac_addr;
}MONITOR_PTH_T;

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */
#ifdef CFG_RWTL
extern uint32_t tl_diff;
#endif

/**
 ****************************************************************************************
 * @brief Get the current HW time.
 *
 * @return The value of nxmac_monotonic_counter_2_lo register.
 ****************************************************************************************
 */
__INLINE uint32_t hal_machw_time(void)
{
    return nxmac_monotonic_counter_2_lo_get();
}

/**
 ****************************************************************************************
 * @brief Compare timer absolute expiration time.
 *
 * @param[in] time1 First time to compare.
 * @param[in] time2 Second time to compare.
 *
 * @return true if time1 is earlier than time2.
 ****************************************************************************************
 */
__INLINE bool hal_machw_time_cmp(uint32_t time1, uint32_t time2)
{
    uint32_t diff = time1 - time2;

    return (((int32_t)diff) < 0);
}

/**
 ****************************************************************************************
 * @brief Compare timer absolute expiration time.
 *
 * @param[in] time Time to compare.
 *
 * @return true if time is in the past, false otherwise
 ****************************************************************************************
 */
__INLINE bool hal_machw_time_past(uint32_t time)
{
    return (hal_machw_time_cmp(time, hal_machw_time()));
}

/**
 ****************************************************************************************
 * @brief Block execution for the given number of us.
 *
 * @param[in] us Number of us to wait for.
 ****************************************************************************************
 */
__INLINE void hal_machw_udelay(uint32_t us)
{
    uint32_t e = hal_machw_time() + us;
    while (!hal_machw_time_past(e));
}

#if NX_MULTI_ROLE
extern const uint8_t rxv2macrate[];

__INLINE uint32_t hal_machw_rx_duration(struct rx_hd *rhd, uint16_t len)
{
    // Fill-in the TimeOnAir parameter registers
    nxmac_ppdu_mcs_index_setf(rxv2macrate[(rhd->recvec1a >> 12) & 0x0F]);
    nxmac_time_on_air_param_1_pack(0, 0, 0, (rhd->recvec1b >> 15) & 0x01, 0, len);

    // Compute the duration
    nxmac_compute_duration_setf(1);
    #ifdef CFG_RWTL
    // Add a fake read to ensure previous write is performed
    hal_machw_time();
    #endif
    while(nxmac_time_on_air_valid_getf() == 0);
	
    ASSERT_REC_VAL(nxmac_time_on_air_valid_getf() != 0, 500);

    // Retrieve the duration
    return ((uint32_t)nxmac_time_on_air_getf());
}
#endif

/**
 ****************************************************************************************
 * @brief Check whether BFMEE is supported by the MAC and PHY HW.
 *
 * @return true if supported, false otherwise
 ****************************************************************************************
 */
__INLINE bool hal_machw_bfmee_support(void)
{
    return (nxmac_bfmee_getf() && phy_bfmee_supported()) != 0;
}

/**
 ****************************************************************************************
 * @brief Check whether BFMER is supported by the MAC and PHY HW.
 *
 * @return true if supported, false otherwise
 ****************************************************************************************
 */
__INLINE bool hal_machw_bfmer_support(void)
{
    return (nxmac_bfmer_getf() && phy_bfmer_supported()) != 0;
}

/**
 ****************************************************************************************
 * @brief Check whether Mu-MIMO RX is supported by the MAC and PHY HW.
 *
 * @return true if supported, false otherwise
 ****************************************************************************************
 */
__INLINE bool hal_machw_mu_mimo_rx_support(void)
{
    return (nxmac_bfmee_getf() && phy_mu_mimo_rx_supported()) != 0;
}

/**
 ****************************************************************************************
 * @brief Check whether Mu-MIMO TX is supported by the MAC and PHY HW.
 *
 * @return true if supported, false otherwise
 ****************************************************************************************
 */
__INLINE bool hal_machw_mu_mimo_tx_support(void)
{
    return (nxmac_mu_mimo_tx_getf() && phy_mu_mimo_tx_supported()) != 0;
}

/**
 ****************************************************************************************
 * @brief Interrupt Controller initialization.
 * This function initializes the interrupt control registers and enables the interrupts.
 ****************************************************************************************
 */
void hal_machw_init(void);

/**
 ****************************************************************************************
 * @brief MAC HW reset function.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It resets the MAC HW state machines, which in consequence resets the PHY.
 ****************************************************************************************
 */
void hal_machw_reset(void);

/**
 ****************************************************************************************
 * @brief Function accessing the HW address search engine.
 *
 * @param[in] addr Pointer to the MAC address to search
 *
 * @return The index of the station found in the address list if any, @ref INVALID_STA_IDX
 * otherwise
 ****************************************************************************************
 */
uint8_t hal_machw_search_addr(struct mac_addr *addr);

/**
 ****************************************************************************************
 * @brief MAC HW interrupt disable function.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It disables the MAC HW interrupts.
 ****************************************************************************************
 */
void hal_machw_disable_int(void);


/**
 ****************************************************************************************
 * @brief Stop the operation of the MAC HW.
 * This function stops the MAC HW operation and resets it.
 ****************************************************************************************
 */
void hal_machw_stop(void);

/**
 ****************************************************************************************
 * @brief Put back the MAC HW in monitoring mode.
 * This function has to be called when the last configured interface is removed.
 ****************************************************************************************
 */
extern void hal_machw_enter_monitor_mode(void);
extern void hal_machw_exit_monitor_mode(void);
extern uint32_t hal_monitor_record_count(struct mac_hdr *machdr);
extern uint32_t hal_monitor_is_including_mac_address(uint64_t address);
extern void hal_update_secret_key(uint64_t, uint8_t);

/**
 ****************************************************************************************
 * @brief Check if the HW timers don expire too early to go to sleep.
 *
 * @return true if we can sleep, false otherwise.
 ****************************************************************************************
 */
bool hal_machw_sleep_check(void);

/**
 ****************************************************************************************
 * @brief Interrupt Handling.
 * This function handles the general interrupts raised by the MAC HW.
 ****************************************************************************************
 */
extern void hal_machw_gen_handler(void);
extern void hal_init_cipher_keys(void);
extern void hal_machw_allow_rx_rts_cts(void);
extern void hal_machw_disallow_rx_rts_cts(void);
/**
 ****************************************************************************************
 * @brief Ask to HW to move to IDLE state.
 * This function assumes that the HW is not in IDLE state when it is called.
 ****************************************************************************************
 */
void hal_machw_idle_req(void);

void hal_assert_rec(void);
#define HAL_FATAL_ERROR_RECOVER(cond)    \
    do {                         \
        if (!(cond)) {           \
            hal_assert_rec();    \
        }                        \
    } while(0)

/// @}  // end of group HAL_MACHW
/// @}  // end of group HAL
#endif // _HAL_MACHW_H_

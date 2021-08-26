#ifndef DBG_MWSGEN_H_
#define DBG_MWSGEN_H_

/**
 ****************************************************************************************
 * @addtogroup DBGMWSGEN
 * @ingroup DBG
 * @brief Debug SW - MWS/WLAN Generator.
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwip_config.h"

#if (RW_WLAN_COEX_TEST) || (RW_MWS_COEX_TEST)

#include "reg_mwsgen.h"       // MWS Event Generator register functions

/*
 * CONSTANT DEFINITIONS
 ****************************************************************************************
 */

#if (RW_WLAN_COEX)
/// WLAN coexistence disabled
#define DBG_COEX_WLAN_DISABLED    0
/// WLAN coexistence enabled
#define DBG_COEX_WLAN_ENABLED     1
#endif

#if (RW_MWS_COEX)
/// MWS coexistence disabled
#define DBG_COEX_MWS_DISABLED     0
/// MWS coexistence enabled
#define DBG_COEX_MWS_ENABLED      1
#endif

/*
 * VARIABLE DECLARATION
 ****************************************************************************************
 */
#if (RW_WLAN_COEX_TEST)
extern uint32_t dbg_coex_scenario;
#endif // RW_WLAN_COEX_TEST

#if (RW_MWS_COEX_TEST)
extern uint32_t dbg_coex_scenario;
#endif // RW_MWS_COEX_TEST

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

#if (RW_MWS_COEX_TEST)
/**
 ****************************************************************************************
 * @brief Set the scenario for the unitary testing.
 *
 * @param[in] scenario    Scenario type
 *
 * @return none
 *
 ****************************************************************************************
 */
uint8_t dbg_mwscoex_scen_set(uint32_t scenario);

/**
 ****************************************************************************************
 * @brief Initialize and configure MWS event generator registers to be in MWS mode.
 ****************************************************************************************
 */
void dbg_mwsgen_init(void);

/**
 ****************************************************************************************
 * @brief Configure MWS generator
 *
 * @param[in]  period       Period of the mws signal (us)
 * @param[in]  duty_cycle   Duration of the high level (us)
 * @param[in]  tx_act       Duration of the tx activity (us)
 * @param[in]  rx_act       Duration of the rx activity (us)
 *
 ****************************************************************************************
 */
void dbg_mwsgen_config(uint32_t period, uint32_t duty_cycle, uint32_t tx_act, uint32_t rx_act);

/**
 ****************************************************************************************
 * @brief Configure the WLAN COEX mode for BT.
 *
 * @param[in]  txmode
 * @param[in]  rxmode
 * @param[in]  txmsk
 * @param[in]  rxmsk
 * @param[in]  txfmsk
 * @param[in]  rxfmsk
 * @param[in]  scanfmsk
 * @param[in]  knudge
 *
 ****************************************************************************************
 */
void dbg_mws_config(uint8_t txmode, uint8_t rxmode, uint8_t txmsk, uint8_t rxmsk, uint8_t txfms, uint8_t rxfmsk, uint8_t scanfmsk, uint8_t knudge);

/**
 ****************************************************************************************
 * @brief Start the MWS signal generator.
 *
 ****************************************************************************************
 */
void dbg_mwsgen_start(void);

/**
 ****************************************************************************************
 * @brief Stop the MWS signal generator.
 *
 ****************************************************************************************
 */
void dbg_mwsgen_stop(void);
#endif // RW_MWS_COEX_TEST

#if (RW_WLAN_COEX_TEST)
/**
 ****************************************************************************************
 * @brief Set the scenario for the unitary testing.
 *
 * @param[in] scenario    Scenario type
 *
 * @return none
 *
 ****************************************************************************************
 */

uint8_t dbg_wlcoex_scen_set(uint32_t scenario);

/**
 ****************************************************************************************
 * @brief Initialize and configure MWS event generator registers to be in WLAN mode.
 ****************************************************************************************
 */
void dbg_wlangen_init(void);

/**
 ****************************************************************************************
 * @brief Set the period and duty cycle for the wlrxbsy signal.
 *
 * @param[in]  period       Period of the wlrxbsy signal (us)
 * @param[in]  duty_cycle   Duration of the high level (us)
 * @param[in]  tx_act       Duration of the tx activity (us)
 * @param[in]  rx_act       Duration of the rx activity (us)
 *
 ****************************************************************************************
 */
void dbg_wlangen_config(uint32_t period, uint32_t duty_cycle, uint32_t tx_act, uint32_t rx_act);

/**
 ****************************************************************************************
 * @brief Configure the WLAN COEX mode for BT.
 *
 * @param[in]  txmode
 * @param[in]  rxmode
 * @param[in]  txmsk
 * @param[in]  rxmsk
 * @param[in]  txthr
 * @param[in]  rxthr
 * @param[in]  pduration
 * @param[in]  pdelay
 *
 ****************************************************************************************
 */
void dbg_wlan_config(uint8_t txmode, uint8_t rxmode, uint8_t txmsk, uint8_t rxmsk, uint8_t txthr, uint8_t rxthr, uint8_t pduration, uint8_t pdelay);

/**
 ****************************************************************************************
 * @brief Start the wlrxbs signal generator.
 *
 ****************************************************************************************
 */
void dbg_wlangen_start(void);

/**
 ****************************************************************************************
 * @brief Stop the wlrxbs signal generator.
 *
 ****************************************************************************************
 */
void dbg_wlangen_stop(void);
#endif // RW_WLAN_COEX_TEST

#endif // (RW_WLAN_COEX_TEST) || (RW_MWS_COEX_TEST)

/// @} DBGMWSGEN

#endif // DBG_MWSGEN_H_

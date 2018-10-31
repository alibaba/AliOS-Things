/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 *  @ingroup  BCDS_HAL_BSP_IF
 *
 *  @defgroup BCDS_HAL_BSP_CHARGER_BQ2407X CHARGER_BQ2407X
 *  @brief Generic interface to the CHARGER peripheral TI BQ2407X
 *
 *
 *  @{
 *
 * @details A prior call to BSP_Board_Initialize(param1, param2) function is necessary to initialize
 * the MCU resources needed by the charger device. Once the charger is requested
 * a call to BSP_Charger_BQ2407X_Connect() function is required to map the internal
 * MCU resources to their associated function.
 *
 * The application calls the BSP_Charger_BQ2407X_Enable() function to safely
 * start the charging process and will to make sure that this process is
 * stopped whenever the charging prerequisites are not met anymore.
 *
 * The BSP_Charger_BQ2407X_Disable() will stop the charging process and disable
 * the BSP functionalities related to the charger module
 *
 * The BSP_Charger_BQ2407X_Disconnect() will put the GPIO pins associated to the
 * charger peripheral in low power consumption mode.
 *  @file
 */

#ifndef BCDS_BSP_Charger_BQ2407X_H
#define BCDS_BSP_Charger_BQ2407X_H

#include "BCDS_HAL.h"
#include "BSP_BoardType.h"

/* Code is only effective if feature is enabled in configuration */
#if BCDS_FEATURE_BSP_CHARGER_BQ2407X

/*
 * enumeration of custom return codes
 */
enum BSP_CHARGER_BQ2407X_Retcode_E
{
    RETCODE_BSP_CHARGER_NOT_ENABLED = RETCODE_FIRST_CUSTOM_CODE,
};

/**
 * @brief Maps the GPIO pins to their desired function.
 *
 * @details This function once called will map the GPIO pins to
 * their desired operating mode to drive the TI(R) BQ2407X Charger peripheral.
 * It is needed to call this function prior to the BSP_Charger_BQ2407X_Enable().
 *
 * @param
 */
Retcode_T BSP_Charger_BQ2407X_Connect(void);

/**
 * @brief Enables the control over the peripheral
 *
 * @details Calling this function will enable the charger module and unlock the
 * other BSP charger functionalities.
 *
 * @param control Options for the enable function depending from
 * the charger hardware implementation.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Charger_BQ2407X_Enable(uint32_t control);

/**
 * @brief Returns the charging status
 *
 * @details: The status of the charger and its associated battery are to be
 * fetched by this function
 *
 * @param[out] 	state:  returns the charge state depending on the board specific
 * implementation enum #BSP_ChargeState_E. The state values are located in the BSP implementation under
 * #BSP_BoardType.h file.
 *
 * @retval RETCODE_OK in case of successfully getting the charge status.
 * @retval RETCODE_NULL_POINTER in case of NULL pointer passed.
 * @retval RETCODE_BSP_CHARGER_NOT_ENABLED in case of accessing before enabling the charger.
 *
 */
Retcode_T BSP_Charger_BQ2407X_GetStatus(BSP_ChargeState_T* state);

/**
 * @brief Measures the specified voltage signal
 *
 * @details: the analog signals from the charger routed to the MCU could be fetched using this function
 *
 * This function once called will return the value of the specified signal in millivolts.
 *
 * @param[in] signal to be measured defined by the BSP implementation in
 * BSP_BoardType.h file.
 *
 * @param[out] 	output: measured battery voltage in millivolts defined by the BSP implementation in
 * BSP_BoardType.h file.
 *
 * @retval 	RETCODE_OK in case of successfully getting the battery voltage.
 * @retval RETCODE_NULL_POINTER in case of NULL pointer passed.
 * @retval RETCODE_BSP_CHARGER_NOT_ENABLED in case of accessing before enabling the charger.
 */
Retcode_T BSP_Charger_BQ2407X_MeasureSignal(uint32_t signal, uint32_t* output);
/**
 * @brief Returns the charging status of the battery.
 *
 * @param[out]  state:  returns the charge state depending on the board specific
 * implementation enum #BSP_ChargeState_E. The state values are located in the BSP implementation under
 * #BSP_BoardType.h file.
 *
 * @param[in]  BatteryVoltage: measured battery voltage in millivolts.
 *
 * @retval RETCODE_OK in case of successfully getting the charge status.
 * @retval RETCODE_NULL_POINTER in case of NULL pointer passed.
 */
Retcode_T BSP_Charger_BQ2407X_CheckStatus(BSP_ChargeState_T* state, uint32_t BatteryVoltage);

/**
 * @brief Disables the control over the peripheral.
 *
 * @details This function once called will disable control over the TI(R)
 * BQ2407X Charger peripheral by executing the Disabling Procedure as prescribed
 * in the vendors datasheets.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 *
 */
Retcode_T BSP_Charger_BQ2407X_Disable(void);

/**
 * @brief Disconnects the Charger peripheral.
 *
 * @details This function disconnects the GPIO pins dedicated to the TI(R)
 * BQ2407X Charger peripheral and put them into low power consumption status.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 *
 */
Retcode_T BSP_Charger_BQ2407X_Disconnect(void);

/**
 * @brief sets the sysoff pin to logic high.
 *
 * @details This function sets the sysoff pin of the TI(R)
 * BQ2407X Charger peripheral to high.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Charger_BQ2407X_SetSysoffHigh(void);

/**
 * @brief sets the sysoff pin to logic low.
 *
 * @details This function sets the sysoff pin of the TI(R)
 * BQ2407X Charger peripheral to low.
 *
 * @retval RETCODE_OK in the case of success or an error code otherwise.
 */
Retcode_T BSP_Charger_BQ2407X_SetSysoffLow(void);

#endif /* BCDS_FEATURE_BSP_CHARGER_BQ2407X */
#endif /* BCDS_BSP_Charger_BQ2407X_H */
/**  @} */

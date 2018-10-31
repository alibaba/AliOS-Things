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
 * @file
 */
/* header definition ******************************************************** */
#ifndef WLANCONNECT_H_
#define WLANCONNECT_H_

/* local interface declaration ********************************************** */

/* local type and macro definitions */
#define WLANCONNECT_SCAN_TABLE_SIZE         UINT8_C(20)
#define WLANCONNECT_SCAN_ENABLE          	UINT8_C(1)
#define WLANCONNECT_SCAN_DISABLE         	UINT8_C(0)
#define WLANCONNECT_FAILURE                 INT16_C(-1) /**< Macro for defining Failure*/
#define WLANCONNECT_SUCCESS                 UINT16_C(0) /**< Macro for defining SUCCESS*/
#define WLANCONNECT_ZERO                    UINT8_C(0) /**< Macro for defining 0*/
#define WLANCONNECT_ONE                     UINT8_C(1) /**< Macro for defining 1*/
#define WLANCONNECT_TIMEOUT_VAL             UINT8_C(0xFF) /**< Macro for timeout value*/
#define WLANCONNECT_ALL_PROFILES            UINT8_C(0xFF) /**< Macro for deleting all profiles */
#define WLANCONNECT_MAX_BUFFER              UINT8_C(10) /**< Macro for Simple Link status buffer */
#define WLANCONNECT_NO_OF_ENTRIES           UINT8_C(5) /**< Macro for scan function number of entries*/
#define WLANCONNECT_ONE_SEC_DELAY        1000 /**< Macro for 1 second delay*/
#define WLANCONNECT_MAX_TRIES               UINT8_C(15) /**< Macro for Simple Link status buffer */
#define WLANCONNECT_TIMER_TICKS             UINT8_C(1000) /**< Macro for Simple Link status buffer */
#define TIMER_AUTORELOAD_ON                 UINT8_C(1)  /**< Auto reload of timer is disabled*/
#define TIMERBLOCKTIME                      UINT32_MAX     /**< Macro used to define blocktime of a timer*/

/* local function prototype declarations */

/* local module global variable declarations */

/* public global variable declarations */

/* local inline function definitions */

#endif /* WLANCONNECT_H_ */

/** ************************************************************************* */

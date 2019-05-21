/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2004              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/


/*****************************************************************************
 * FILE NAME    : mu_capi.h
 * DATE         :
 * DESCRIPTION  : It Contains Function prototypes of all functions exported
 *                by compliance device driver to other layers.
 ****************************************************************************/

#ifndef	__MUSB_COMPLIANCE_API_H__
#define	__MUSB_COMPLIANCE_API_H__


/**
 * Connect Call Handler.
 * This function is called by UCD when this device driver is selected for the connected compliance
 * device.
 * It gets the active configuration number from the descriptors provided by device and does a
 * set config. If this call is made due to a disconnect/connect test it calls the state handler
 * to start next test iteration.
 * @param pPrivateData Pointer to private data of compliance device driver.
 * @param hBus Session Handle.
 * @param pUsbDevice Pointer to device Information.
 * @param pPeripheralList pointer to the peripheral list entry which caused the
 * driver to be selected
 * @return MUSB_SUCCESS if the endpoint capability table is validated.
 * @return MUSB_FAILURE Otherwise.
 */
extern uint8_t MUSB_ComplConnect(void              *pPrivateData,
                                 MUSB_BusHandle    hBus,
                                 MUSB_Device       *pUsbDevice,
                                 const uint8_t     *pPeripheralList);

/**
 * Disconnect Function for compliance device driver.
 * This function is called by UCD when compliance device is disconnected.
 * It releases all the resources held by the device driver and goes back to init state.
 * @param pPrivateData Pointer to private data of compliance device driver.
 * @param hBus Session Handle.
 * @param pUsbDevice Pointer to device Information.
 * @return None.
 */

extern void MUSB_ComplDisconnect(void           *pPrivateData,
                                 MUSB_BusHandle  hBus,
                                 MUSB_Device     *pUsbDevice);

/**
 * Initilaization function for compliance device driver.
 * This function is called when device is connected.
 * This function initalizes different test patterns and device driver information.
 * @param None.
 * @return MUSB_STATUS_OK if all pointers required for compliance driver context are allocated.
 * @return MUSB_FAILURE Otherwise.
 */
extern uint32_t MUSB_ComplDriverInit(void);

/**
 * Timer Expiry Callback Function.
 * This function is called by UCD when timer started by device driver expires.
 * Timer is started when device driver starts Disconnect/Connect or, Remtoe Wakeup Test.
 * In this function device driver checks bTestConnect or, bTestRemoteWakeup flag to check whether
 * connect/bus resume callback has come due to disconnect/connect or, remote wakeup test or, not.
 * If not it means device is physically disconnected and it calls disconnect callback.
 * Otherwise it returns without doing any thing.
 * @pPrivateData Pointer to private data of compliance device driver.
 * @param hBus Session Handle.
 * @return None.
 */
extern void MUSB_ComplTimerExpired(void *pPrivateData, MUSB_BusHandle hBus);

/**
 * Bus suspend callback.
 * This function is called by UCD when bus is suspended.
 * This function does nothing.
 * @pPrivateData Pointer to private data of compliance device driver.
 * @param hBus Session Handle.
 * @return None.
 */
extern void MUSB_ComplBusSuspended(void *pPrivateData, MUSB_BusHandle hBus);

/**
 * Bus Resume callback.
 * This function is called by UCD when bus is resumed by the device.
 * This function updates the test data for next test iteration and calls state handler to start
 * next set of transfer tests.
 * @pPrivateData Pointer to private data of compliance device driver.
 * @param hBus Session Handle.
 * @return None.
 */

extern void MUSB_ComplBusResumed(void *pPrivateData, MUSB_BusHandle hBus);

#endif

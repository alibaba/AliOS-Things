/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2003-2004                      *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * Hub driver functionality for use by upper layers
 * (power management, TT error handling, selective suspend/resume)
 * $Revision: 1.9 $
 */

#ifndef __MUSB_HUB_API_H__
#define __MUSB_HUB_API_H__

/** Hub TT types */
typedef enum
{
    /** Invalid TT type */
    MUSB_HUB_INVALID_TT     = -1,
    /** Hub Supports signle TT */
    MUSB_HUB_SINGLE_TT      = 0x00,
    /** Hub Supports Multiple TT */
    MUSB_HUB_MULTIPLE_TT    = 0x01,
    /** Full-/Low-Speed Operating Hub */
    MUSB_HUB_NO_TT          = 0x02
} MUSB_HubTtTypes;


/*
 * Information(parameters) should be stored in Hub device Information
 * for TT handling and Power Management are
 *
 * @field bTtType This field represents the TT type.
 *        Lower Nible first Bit(0) inidcates the presently Configured TT type
 *        ( It can have MUSB_HUB_SINGLE_TT(0) or MUSB_HUB_MULTIPLE_TT(1) )
 *        Upper Nible first Bit(1) indicates the TT type
 *        ( It can have MUSB_HUB_SINGLE_TT or MUSB_HUB_MULTIPLE_TT )
 *        If it doesn't have TT, then its value is MUSB_HUB_INVALID_TT
 *
 * @field bSelfPower This tells the power status of a hub
 * It can have MUSB_HUB_SELF_AND_BUS_POWERED(2),
 * MUSB_HUB_SELF_POWERED(1) or MUSB_HUB_BUS_POWERED(0).
 *
 * @field bLocalPower This indicates the present power status of the hub
 * This values is MUSB_HUB_LOCAL_POWERED(1) if local power supply is good;
 * MUSB_HUB_NO_LOCAL_POWERED(0) If local power supply is not good.
 *
 * @field bAllocatedPower In terms of units (unit = 100mA), This field is
 * valid only incase of bus powered hub, because bus powered hub
 * has to take care about the number of devices connected to the hub.
 */

/**
 * This needs to be called by the UCD when it wants this information.
 * This should not be called before Hub Connect callback completion
 *
 * This returns the TT related information, which will be used for
 * programming the MUSBMHDRC's TT registers.
 *
 * @param pUsbDevice Pointer to USB device
 *
 * @return TT type on success and the values are
 *         Lower Nibble first Bit(0) inidcates the presently Configured TT type
 *         ( It can have MUSB_HUB_SINGLE_TT(0) or MUSB_HUB_MULTIPLE_TT(1) )
 *         Upper Nibble first Bit(4) indicates the TT type
 *         ( It can have MUSB_HUB_SINGLE_TT or MUSB_HUB_MULTIPLE_TT )
 *         CD will to use this value while scheduling the split transactions
 * @return MUSB_HUB_INVALID_TT on Failure
 */
extern int8_t MUSB_HubGetTtInfo(MUSB_Device  *pUsbDevice);

/**
 * This needs to be called by the CD when it wants to change the TT
 * operating mode of the hub
 *
 * This changes the TT operating mode of the hub by setting the corresponding
 * alternate setting through Set Interface Request
 *
 * @param bNewTtMode New TT mode to be set.It should be "MUSB_HUB_SINGLE_TT(0)"
 * for Single-TT, MUSB_HUB_MULTIPLE_TT(1) for Multiple-TT.
 *
 * @param pUsbDevice USB device information.
 *
 * @param ControlIrpComplete Callback function for control IRP
 *
 * @param pCompleteParam Call back call with this parameter
 *
 * @return TRUE on Success
 * @return FALSE on failure
 */
extern uint8_t MUSB_HubChangeTtMode(
    int8_t          bNewTtMode,
    MUSB_Device    *pUsbDevice,
    MUSB_pfControlIrpComplete ControlIrpComplete,
    void           *pCompleteParam);

/**
 * This needs to be called by the CD when any error occurs during
 * Non-periodic(Control/Bulk) Split transactions.
 *
 * Clears TT Errors occurred during the Control/Bulk Split transactions.
 *
 * @param bEndpointAddress  Endpoint address to which this transfer is scheduled
 *
 * @param bDeviceAdress Device address
 *
 * @param bEndpointType Type of endpoint as per usb2.0 specification(Bulk/Control)
 *
 * @param pUsbDevice USB device information.
 *
 * @param ControlIrpComplete Callback function for control IRP
 *
 * @param pCompleteParam Call back call with this parameter
 *
 * @return TRUE on Success
 * @return FALSE on failure
 */
extern uint8_t MUSB_HubClearTtBuffer(
    uint8_t         bEndpointAddress,
    uint8_t         bDeviceAddress,
    uint8_t         bEndpointType,
    MUSB_Device    *pUsbDevice,
    MUSB_pfControlIrpComplete ControlIrpComplete,
    void           *pCompleteParam);

/**
 * This needs to be called by the CD when it identfies that
 * the TT is in unknown state. Hub driver also needs to call this
 * after set configuration to keep the TT in known state.
 *
 * Under some circumstances, a TT in a hub may be in an unknown state such
 * that it is no longer functioning correctly.
 * This request allows the TT to be returned
 * to the state it is in immediately in after the hub configured.
 *
 * @param bPortNumber If the hub supports multiple TTs, then this must
 * specify the port number of the TT that is to be reset. If the hub provides
 * only a single TT, then Port must be set to 1
 *
 * @param pUsbDevice USB device information.
 *
 * @param ControlIrpComplete Callback function for control IRP
 *
 * @param pCompleteParam Call back call with this parameter
 *
 * @param wPortNumber Port number of the TT. If the Hub is Multiple TT, then
 * This value should be the port number of the MUSB_Device.bHubPort
 * of device(While communicating with which device, TT error occurs )
 * connected to the Hub.
 *
 * This value should be 0x01, in case of Single-TT Hub.
 *
 * @return TRUE on Success
 * @return FALSE on failure
 */
extern uint8_t MUSB_HubResetTt(
    MUSB_Device   *pUsbDevice,
    MUSB_pfControlIrpComplete ControlIrpComplete,
    void          *pCompleteParam,
    uint16_t       wPortNumber);

/**
 * @memo GET_TT_STATE
 * Debugging command, vendor specific so, it may not required
 */

/**
 * @memo STOP_TT
 * Prior to retrieving GET_TT_STATE for debugging, use this to stop it.
 * This should be implemented if GET_TT_STATE is implmented
 */

/*
 * CD needs to call this API with First hub connected to the XDRC core.
 * CD call this API whenever CD wants to do Global suspend.
 * This API should not be called when application wants to do global suspend
 * to save the power and during that time CD has do Gloabl suspend by stopping the SOFs
 *
 * This verifies that the Global suspend is possible or not
 * Check for remote wakeup feature for each device.
 * If any device is not supporting then do the selective suspend for all the
 * which are supporting remote wakeup
 * If all the devices in the tree supporting remote wakeup, then return TRUE
 * to the CD. CD will do global suspend.
 *
 * @param pUsbDevice Pointer to the USB Device of the First Hub connected to the XDRC.
 *
 * @return TRUE on Success, then CD will do the Global suspend
 *         FALSE on Failure, CD won't do the Global suspend
 */
extern uint8_t MUSB_HubVerifyGlobalSuspend(MUSB_Device    *pUsbDevice);

/**
 * This needs to be called by the Application/class driver when
 * they want to suspend a particular device
 *
 * Selective suspend: Suspending a bus between a hub and device or port
 * This sets the PORT_SUSPEND feature causes bus traffic to cease on
 * that port and, consequently, the device connected on the port to suspend.
 *
 * @param pUsbDevice USB device information.
 *
 * @param ControlIrpComplete Callback function for control IRP
 *
 * @param pContext Callbac parameter
 *
 * @return TRUE on Success
 * @return FALSE on failure
 */

extern uint8_t MUSB_HubSelectiveSuspend (
    MUSB_Device     *pUsbDevice,
    MUSB_pfControlIrpComplete ControlIrpComplete,
    void            *pContext);

/**
 * This needs to be called by the CD/Application/class driver when
 * they want to resume a particular device
 *
 * Selective Resume: Clearing a port suspend
 * This resumes the bus between the device and the hub.
 * This function will be called whenever CD/Application/class driver
 * wants to resume given port/bus/device
 *
 * @param pUsbDevice USB device information.
 *
 * @param ControlIrpComplete Callback function for control IRP
 *
 * @param pContext Callbac parameter
 *
 * @return TRUE on Success
 * @return FALSE on failure
 */
extern uint8_t MUSB_HubSelectiveResume(
    MUSB_Device     *pUsbDevice,
    MUSB_pfControlIrpComplete ControlIrpComplete,
    void            *pContext);

/**
 * Fill an array with the targetted peripheral list entry appropriate
 * for the mass-storage class driver, ending with the MUSB_TARGET_ACCEPT.
 * @param pList array
 * @param wListLength how many bytes are available in the array
 * @return how many bytes were filled in the array.
 * If this equals bListLength, the caller should assume there is insufficient space
 * in the array and the list entry is incomplete.
 * @deprecated please use MUSB_FillHubPeripheralList
 * @see MUSB_FillHubPeripheralList
 */
extern uint16_t MGC_FillHubClassPeripheralList(uint8_t *pList, uint16_t wListLength);

/**
 * Fill an array with the targetted peripheral list entry appropriate
 * for the hub driver.
 * @param bDriverIndex the driver's index for use with MUSB_TARGET_ACCEPT
 * @param pList array
 * @param wListLength how many bytes are available in the array
 * @return how many bytes were filled in the array.
 * If this equals wListLength, the caller should assume there is insufficient space
 * in the array and the list entry is incomplete.
 */
extern uint16_t MUSB_FillHubPeripheralList(uint8_t bDriverIndex, uint8_t *pList,
        uint16_t wListRemain);

/**
 * Get a pointer to the hub class driver
 */
extern MUSB_DeviceDriver *MUSB_GetHubClassDriver(void);

#define MGC_GetHubClassDriver MUSB_GetHubClassDriver
#endif /* End of __MUSB_HUB_API_H__ */



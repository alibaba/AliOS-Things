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
 * Header related to hub or port status change handling
 * $Revision: 1.11 $
 */

#ifndef _MU_HCORE_H_
#define _MU_HCORE_H_

#include "mu_arch.h"

/**
 * This data structure is used to get information that which device is connected
 * to which port. Since 255 port can be active but only 4 device are allowed to
 * connect to the system (Its configurable option and more devices can be
 * connected if MUSB_HUB_MAX_PORTS value will be increased).
 *
 * @field bPortNum Port number at which device is connected.
 *
 * @field bStatus  It tells that child device present or not.
 */

typedef struct _MUSB_HubChildPortMap_
{
    uint8_t bPortNum;
    uint8_t bStatus;
} MUSB_HubChildPortMap;

/**
 *  MUSB_PortDebounceParam
 *
 *  @field dwWaitingTime: waiting time of Hub driver before checking connection
 *                        has settle down or not.
 *
 *  @field dwIntermediateDelay:  Increment waiting time by this amount.
 *
 *  @field bErrorCount Number of times disconnect allowed dring debounce processing.
 *
 */
typedef struct
{
    uint32_t dwWaitingTime;
    uint32_t dwIntermediateDelay;
    uint8_t  bErrorCount;
    MUSB_HubPortStatus   debouncePortStatus;
} MUSB_PortDebounceParam;


/**
 *  MUSB_PortResetParam:
 *
 *  @field dwNumOfTry: Maximum number of time hub driver will try to reset the device.
 *                     If driver able to reset device in first then try return success
 *                     from there itself.
 *
 *  @field dwWaitingTime: Hub driver waits before checking that reset has completed or not.
 *
 */
typedef struct
{
    uint32_t dwNumOfTry;
    uint32_t dwWaitingTime;
    MUSB_HubPortStatus   resetPortStatus;
} MUSB_PortResetParam;


/**
 * MUSB_Hub (Hub specific data structure). When Hub device is connected then
 * Hub connect function is called. This function allocates as well as
 * initializes few fields of this data structure.
 *
 * @field pUcdPrivateData : Data given by STACK/UCD.
 *
 * @field busHandle : session "handle" used for different pipe operation.
 *
 * @field pUsbDev   : Pointer to USB device i.e. USB device representation of HUB.
 *
 * @field pInfDscr   : Interface for which HUB driver is selected. This information
 *                     will be given by UCD and hub driver will store just reference
 *                     pointer. There will not be any allocation.
 *
 * @field pEndpDscr  : Interrupt endpoint for which IRP is going to submit.
 *                     This pointer is pointing to location where Endpoint
 *                     descriptor is stored by UCD.
 *                     For this field there will not be any pointer allocation.
 *
 * @aHubRawDscr     : This will act as buffer for hub descriptor in raw format.
 *                    Later data will be taken from here and stored in proper
 *                    format of hub descriptor.
 *
 * @field hubDscr   : HUB descriptor in proper format will be store here.
 *
 * @field ctrlIrp   : IRP used by Hub for all control transaction.
 *
 * @field setupPacket : Setup packet as per USB 2.0 specification.
 *
 * @field aHubPortStatus : This field is used in control IRP as IN buffer to store
 *                         either hub or port status.
 *
 * @field hubStatus   : Status of the hub will be stored here.
 *
 * @field portStatus  : Status of a port will be store here.
 *
 * @field intrInIrp : IntrIrp which gives information about Hub and Port status change.
 *
 * @field intrInPipe: Interrupt pipe handler.
 *
 * @field aStatusBitMap : Contain status change information of all the ports as well as
 *                        hub.
 *
 * @field pChild    : pointer of Connected USB devices at different ports of the HUB.
 *                    Hub Class driver will update this information when enumeration is
 *                    completed by UCD.
 *
 * @field bState     : State of the HUB.
 *
 * @field bNextState : Next state of hub if the given request executed properly.
 *                     When IRP is executed properly then first statement of any
 *                     callback would be bState = bNextState;
 *
 * @field bPortState : State of the active port i.e. port for which currently requests are
 *                     sent or received.
 *
 * @field bPortNextState : Next state of port if the given request executed properly.
 *                     When IRP is executed properly then first statement of any
 *                     callback would be bPortState = bPortNextState;
 *
 * @field bActivePortNum : Port number used for current request or presently all the request
 *                         will go to this port.
 *
 * @field bSpeed : Speed of newly connected device to the hub's port.
 *
 * @field debounceParam  : Parameter used during debounce period.
 *
 * @field resetParam     : Parameter used during Reseting a port.
 *
 * @field bAlternateSelling Current alternate setting value
 *
 * @field bTtType This field represents the TT type.
 *        Lower Nible first Bit(0-Bit) inidcates the presently Configured TT type
 *        ( It can have MUSB_HUB_SINGLE_TT(0) or MUSB_HUB_MULTIPLE_TT(1) )
 *        Upper Nible first Bit(4-Bit) indicates the TT type
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

typedef struct _MUSB_Hub_
{
    void                                *pUcdPrivateData;
    MUSB_BusHandle                       busHandle;
    MUSB_Device                         *pUsbDevice;
    MUSB_DeviceDriver *pDriver;

    const MUSB_InterfaceDescriptor      *pInfDscr;
    const MUSB_EndpointDescriptor       *pEndpDscr;

    uint8_t                             aHubRawDscr[MUSB_HUB_MAX_HUB_DSCR_SIZE];
    MUSB_HubDescriptor                        hubDscr;

    MUSB_ControlIrp                     ctrlIrp;
    MUSB_DeviceRequest                  setupPacket;
    uint8_t                             aHubPortStatus[MUSB_HUB_PORT_STATUS_LENGTH];

    MUSB_HubPortStatus                  hubStatus;
    MUSB_HubPortStatus                  portStatus;

    MUSB_Irp                            intrInIrp;
    MUSB_PipePtr                           intrInPipe;
    uint8_t                             aHubPortStatusChangeBuffer[(MUSB_HUB_MAX_PORTS / 8) + 1];
    uint32_t                            dwHubPortStatusChange;

    MUSB_Device                         *pChild[MUSB_HUB_MAX_PORTS];
    MUSB_HubChildPortMap                aHubChildPortMap[MUSB_HUB_MAX_PORTS];

    MUSB_PortDebounceParam              debounceParam;
    MUSB_PortResetParam                 resetParam;

    uint8_t                             bNumOfChildActive;

    uint8_t                             bState;
    uint8_t                             bNextState;

    uint8_t                             bPortState;
    uint8_t                             bPortNextState;

    uint8_t                             bActivePortNum;
    uint8_t                             bSpeed;

    uint8_t                             bIndexAtHubDeviceList;

    uint8_t                             bAlternateSetting;

    int8_t                              bTtType;
    uint8_t                             bSelfPower;
    uint8_t                             bLocalPower;
    uint8_t                             bAllocatedPower;
    uint8_t                             bDepthInHubChain;
    uint8_t                             bIntrIrpExecutionErrorCount;
    uint8_t                             bCurrentChildIndex;

} MUSB_Hub;

/* Random value has taken  */
#define MUSB_HUB_MAX_ERROR_COUNT_FOR_INTR_IRP_EXECUTION  0x05

#define MUSB_HUB_MAX_HUB_SUPPORTED     0x06

typedef struct
{

    uint8_t    bRootHub;
    MUSB_Hub   hubDeviceList[MUSB_HUB_MAX_HUB_SUPPORTED];
    uint8_t    bNumOfHubActive;
    MUSB_DeviceDriver *pDriver;

} MUSB_HubDriverContext;





/**
 * @doc This function initializes control IRP and prepare setup packet
 *      for SET CONFIG request and start control transfer.
 * @param pUsbHubDevice: Pointer to Hub device.
 * @param pUsbDevice   : USB representation of Hub.
 * @param bBusAddress  : Address given by UCD to hub device.
 * @param pPort        : Given by UCD.
 * @return MUSB_SUCCESS or MUSB_FAILURE.
 */
extern uint32_t MGC_HubConnectHandler (MUSB_Hub *pHubDevice,
                                       MUSB_Device *pUsbDevice,
                                       uint8_t bBusAddress,
                                       MUSB_Port *pPort);
/**
 * @doc This function contains state machine which is used during
 *      initialization of hub.
 * @param pContext: Context given conrol IRP initialization.
 * @param pControlIrp : Pointer to control IRP.
 * @return None.
 */
extern void MGC_HubInitializeCallback (void *pContext, MUSB_ControlIrp *pControlIrp);

extern  uint32_t  MGC_HubFillAndSubmitIntrIrp (MUSB_Hub *pHubDevice, uint8_t bBusAddress);

extern void MGC_HubIntrIrpHandler(MUSB_Hub  *pHubDevice, MUSB_Irp *pIntrIrp);

extern void MGC_HubIntrIrpCallback ( void *pCtxt, MUSB_Irp *pCtrlIrp);

extern void MGC_HubDisconnectHandler(MUSB_Hub *pHubDevice, uint32_t dwHubType,
                                     MUSB_HubDriverContext *pDriverContext);



/**
 * @doc This function find hub device for Given USB device information.
 *      This function will tell that given usb device pointer represent
 *      hub or not.
 *
 * @param pUsbDevice Pointer to usb device
 *
 * @param pHubDevice Pointer to Hub device will be store here.
 *
 * @return MUSB_SUCCESS when pUsbDevice belongs to HUB  otherwiseMUSB_FAILURE
 */
extern uint32_t MGC_HubFindHubDevice (MUSB_Device *pUsbDevice, MUSB_Hub **pHubDevice);

/**
 * @doc This function finds index at which free hub device structure is available.
 *
 * @param pDriverContext Pointer to driver context at which information about
 *                       all hub device is available.
 * @param pIndex Index at which free hub device is present will be store here.
 *
 * @return MUSB_SUCCESS when free index is found or MUSB_FAILURE when free device
 *         is not available.
 */
extern uint32_t MGC_HubFindFreeHubDeviceIndex(MUSB_HubDriverContext *pDriverContext,
        uint8_t *pIndex);




/**
 * @doc This function will be registered by HUB driver as timer expiry callback.
 * This function will be called by UCD whenever timer is expired.
 *
 * @param pDriverPrivateData Pointer to driver's private data.
 *
 * @param hBus Bus Handle
 *
 * @return None
 */
extern void MUSB_HubDriverTimerExpired(void *pDriverPrivateData, MUSB_BusHandle hBus);

/**
 * This needs to be called by the Hub class driver after set configuration
 *
 * This interprets the Operating speed of the Hub and validates the
 * descriptors fields accordingly. It also Finds whether it supports
 * multiple TT or Single TT or Invalid device and stores the
 * TT information at HUB device information
 *
 * @param pHubDevice Pointer to the HUB device information
 *
 * @return MUSB_SUCCESS on success
 * @return MUSB_FAILURE on failure
 */
extern uint32_t MGC_HubValidateUpdateTtInfo(MUSB_Hub *pHubDevice);

/**
 * Hub driver has to call this after getting the Hub status
 * (through Get status Request)
 *
 * It does the verification against the power requirements(usb2.0 spec, 11.13)
 * and initialize the parameters related to power management
 *
 * This validates/compares the power requirements given in the
 * "bmAttributes" field of the configuration descriptor and "Hub device status"
 *
 * @param pHubDevice Pointer to the Hub device information
 *
 * @return MUSB_SUCCESS on success
 * @return MUSB_FAILURE On failure
 */
extern uint32_t MGC_HubPowerValidateInit(MUSB_Hub   *pHubDevice);


/**
 * Assumption: Hub always gets 5 units of load from upstream port
 * and Bus Powered hub cannot be connected to a Bus powered hub.
 *
 * This needs to be called by the hub class driver whenever any of
 * the following events occur on any port of the hub.
 * 1.New connection, 2.Disconnection, 3.Port Suspend, 4.Port Resume
 * Note: This function should not be called whenever a hub is connected
 * to this
 *
 * Power scheduling is required for Bus-Powered hubs only since
 * there are some limits on the available power from upstream and number
 * of devices to be connected.
 * Self-Powered hubs can drive any number of ports, so no need to
 * schedule the power.
 *
 * It updates the available power and checks the newly connected device
 * working feasibility
 *
 * @param pHubDevice Pointer to Hub device information
 *
 * @param bPowerUpdateType This is any one of type MUSB_HubPowerUpdateTypes
 *
 * @return MUSB_SUCCESS on Success
 * @return MUSB_FALIURE on Failure (This is valid incase of connect event only )
 */
extern uint32_t MGC_HubUpdatePower(MUSB_Hub *pHubDevice, uint8_t bPowerUpdateType);

/**
 * This Initializes the TT information of the hub and
 * Reset the TT of the Hub to keep the TT in known state
 *
 * @param pUsbDevice USB device information.
 *
 * @param ControlIrpComplete Callback function for control IRP
 *
 * @param pHubDevice Pointer to Hub Device information
 *
 * @return MUSB_SUCCESS on Success
 * @return MUSB_FALIURE on Failure
 */
extern uint32_t MGC_HubInitAndResetTt(MUSB_Device  *pUsbDevice,
                                      MUSB_pfControlIrpComplete ControlIrpComplete,
                                      MUSB_Hub     *pHubDevice);


/*
 * This checks the remote wakeup feature in the total USB_Tree
 *
 * @param pUsbDevice Pointer to the USB device of a HUB
 *
 * @retunr TRUE if all the devices connected to the given hub are supporting
 *         Remote wakeup.
 * @return FALSE In other cases it
 */
extern uint8_t MGC_HubIsAnyPortNotSupportsRemoteWakeup(MUSB_Device *pUsbDevice);

extern const MUSB_EndpointDescriptor *MGC_GetIntrInEndpointDescriptor(
    MUSB_ConfigurationDescriptor *pConfig,
    const MUSB_InterfaceDescriptor *pInterfaceDescriptor,
    uint8_t bEndpointType,
    uint8_t bEndpointDirection);
extern void MGC_HubDisplayHubDescriptor(MUSB_HubDescriptor *pHubDscr);
extern void MGC_HubDisplayDeviceDescriptors(MUSB_DeviceDescriptor  *pDeviceDescriptor);
extern void MGC_HubDisplayConfigurationDescriptors(MUSB_ConfigurationDescriptor *pConfiguration);
extern void MGC_HubDisplayInterfaceDescriptor(MUSB_InterfaceDescriptor *pInterface);

/*********************Function Related to Hub Status  *******************/
extern void MUSB_HubStatusChange (MUSB_Hub *pHubDevice);
extern void MGC_HubGetHubStatusCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp);
extern void MGC_HubOverCurrentTimerCallback (void *pParam, MUSB_BusHandle hBus);
extern void MGC_HubHandleHubOverCurrentTimer (MUSB_Hub *pHubDevice, uint32_t dwWaitingTime);
extern void MGC_HubPowerOnGoodTimerCallback (void *pParam, MUSB_BusHandle hBus);
extern void MGC_HubPowerOnGoodTimer (MUSB_Hub *pHubDevice, uint32_t dwWaitingTime);
extern void MGC_HubHandleOverCurrent (MUSB_Hub *pHubDevice, uint8_t *pHubNextState);



/**************Function Used  for Hub as well as Port***********************/
extern void MGC_HubStoreStatus (uint8_t *pStatusData, MUSB_HubPortStatus *pHubStatus);
extern void MGC_HubHandlePowerOnPorts(MUSB_Hub *pHubDevice, uint8_t *pHubNextState);

extern void MUSB_HubPrepareSetupPkt (MUSB_DeviceRequest *pSetup,
                                     uint8_t bmRequestType,
                                     uint8_t bRequest, uint16_t wValue,
                                     uint16_t wIndex, uint16_t wLength);

extern uint32_t MGC_HubGetChildIndexForGivenPort (MUSB_Hub *pHubDevice, uint8_t bPortNum, uint8_t *pChildIndex);

extern void MGC_DeEnumerateHub (MUSB_Hub *pHubDevice, uint32_t dwHubType);

extern void MUSB_HubDeviceEnumerateCallback (MUSB_Device *pHubUsbDevice, MUSB_Device *pChildUsbDevice);



/*****************Funtion Related to ports status Change ***************/
extern void MGC_HubPortStatusChange(MUSB_Hub *pHubDevice);
extern void MGC_HubGetPortStatusCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp);
extern void MGC_HubHandleOverCurrentPortContinue (MUSB_Hub *pHubDevice, uint8_t *pPortNextState, uint8_t bPortNum);
extern void MGC_HubHandleEnablePort (MUSB_Hub *pHubDevice, uint8_t *pPortNextState, uint8_t bPortNum);
extern void MGC_HubHandleSuspendPort (MUSB_Hub *pHubDevice, uint8_t *pPortNextState, uint8_t bPortNum);
extern void MGC_HubOverCurrentPortTimer (MUSB_Hub *pHubDevice, uint32_t dwWaitingTime);
extern void  MGC_HubOverCurrentPortTimerCallback (void *pParam, MUSB_BusHandle hBus);

extern void MGC_HubHandlePortConnect (MUSB_Hub *pHubDevice, uint8_t *pPortNextState, uint8_t bPortNum);
extern void MGC_HubHandlePortDisconnect (MUSB_Hub *pHubDev, uint8_t bPortNum);
extern void MGC_HubHandleOverCurrentPortComplete (MUSB_Hub *pHubDevice, uint8_t *pPortNextState, uint8_t bPortNum);
extern void MGC_HubPortConnectCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp);


/**********************Function related to DEBOUCE ***********/
extern void MGC_HubHandlePortDebounceTimer (MUSB_Hub *pHubDevice, uint32_t dwWaitingTime);
extern void MGC_HubPortDebounceTimerCallback (void *pParam, MUSB_BusHandle hBus);
extern void MGC_HubPortDebounceCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp);



/**********************Function Used for RESET ***********/
extern void MGC_HubPortResetCallback (void *pCtxt, MUSB_ControlIrp *pCtrlIrp);
extern void MGC_HubPortResetTimerCallback (void *pParam, MUSB_BusHandle hBus);
extern void MGC_HubHandlePortResetTimer (MUSB_Hub *pHubDevice, uint32_t dwWaitingTime);

#endif




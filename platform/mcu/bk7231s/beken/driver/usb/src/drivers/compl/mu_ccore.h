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
 * FILE NAME    : mu_ccore.h
 * DATE         :
 * DESCRIPTION  : It Contains data structure and Function prototypes of core
 *                functions used by compliance device driver.
 ****************************************************************************/

#ifndef __MUSB_COMPLIANCE_CORE_H__
#define __MUSB_COMPLIANCE_CORE_H__

/** Configurables variables for Compliance Device Driver. */

/** Maximum Buffer size allocated to receive/send data from/to an endpoint */
#define MUSB_COMPL_DATA_BUFFER_SIZE                     8*1024

/** Maximum Length of configuration descriptor to be received from the device */
#define MUSB_COMPL_MAX_DSCR_LENGTH                      0xFF

/** Maximum Number of packets required to complete an ISO Transfer */
#define MUSB_COMP_MAX_ISO_PKTS                          (MUSB_COMPL_DATA_BUFFER_SIZE/64)

/** Maximum Number of endpoints on a compliance device */
#define MUSB_COMPL_NUM_EPS                              256

/** Number of iterations to be executed for Loopback test */
#define MGC_COMPL_MAX_NUM_LOOPBACK_ITERATIONS           128

/** After each of transfer test iteration dwModfifier will be increased by this value. */
#define MGC_COMPL_DWMODIFIER_STEP_SIZE                  0xFFFF

/** When dwModifier reaches ZERO, dwPattern will be increased by this value */
#define MGC_COMPL_DWPATTERN_STEP_SIZE                   0xFFFFFF

#define  MUSB_SUCCESS                                   0x00
#define  MUSB_FAILURE                                   0xFFFFFFFF

#define MUSB_COMPL_ERROR                                0x01
#define MUSB_COMPL_SUCCESS                              0x02


/** Definitions for USB compliance test device */
#define MGC_COMPL_VENDOR_ID                             0x00
#define MGC_COMPL_PRODUCT_ID                            0x00
#define MGC_COMPL_BCD_DEVICE_LO                         0x00
#define MGC_COMPL_BCD_DEVICE_HI                         0x00
#define MGC_COMPL_DEVICE_CLASS                          0xDC
#define MGC_COMPL_DEVICE_SUBCLASS                       0x01
#define MGC_COMPL_DEVICE_PROTOCOL                       0x01

/** Compliance Device Class Requests */
#define MUSB_COMPL_REQ_VEN_CHANGE_EP                    0x51
#define MUSB_COMPL_REQ_VEN_DO_DISCONNECT                0x52
#define MUSB_COMPL_REQ_VEN_GET_EP_TABLE                 0x53
#define MUSB_COMPL_REQ_VEN_SET_PID                      0x54
#define MUSB_COMPL_REQ_VEN_SET_RMTWK_TO                 0x55
#define MUSB_COMPL_REQ_VEN_GET_COUNT                    0x58
#define MUSB_COMPL_REQ_VEN_GET_VERSION                  0x59
#define MUSB_COMPL_REQ_VEN_SET_CHIRP                    0x5A
#define MUSB_COMPL_REQ_VEN_CONROL_WRITE_DATA            0x5B
#define MUSB_COMPL_REQ_VEN_CONROL_READ_DATA             0x5C
#define MUSB_COMPL_REQ_VEN_GET_EP_BUFFERSIZE            0x5D

/** validation mask for endpoint capability. */
#define MUSB_COMPL_S_EPCHANGE_BFLAGS_VALID              0

/** do loopback mask for endpoint capability */
#define MUSB_COMPL_S_EPCHANGE_BFLAGS_LOOPBACK           1

/** endpoint type */
#define MUSB_COMPL_S_EPCHANGE_BFLAGS_EP_TYPE            2

/** number of additional transaction opportunities per micro-frame */
#define MUSB_COMPL_S_EPCHANGE_BFLAGS_EXTRATRANSACTIONS  4

/** Different response mode */
#define MUSB_COMPL_RESPONSE_NORMAL                      0
#define MUSB_COMPL_RESPONSE_STALL                       1
#define MUSB_COMPL_RESPONSE_NAK                         2
#define MUSB_COMPL_RESPONSE_TIMEOUT                     4
#define MUSB_COMPL_RESPONSE_CRCERROR                    8

/** Different masks/shift values used for masking compliance device properties */
#define MUSB_COMPL_S_ATTIBUTES_HIGHBANDWIDTH            0
#define MUSB_COMPL_S_ATTIBUTES_LOOPBACK                 1
#define MUSB_COMPL_S_ATTIBUTES_PERFECTSOURCESINK        2
#define MUSB_COMPL_S_ATTIBUTES_TRANSACTIONCOUNT         5
#define MUSB_COMPL_DEV_SOUR_SINK_MASK                   (1<<2)
#define MUSB_COMPL_DEV_LOOPBACK_MASK                    (1<<1)

#define MGC_COMPL_SOURCESINK_EXECUTED                   0x01
#define MGC_COMPL_LOOPBACK_EXECUTED                     0x02


/** Different types of transfer test. */
#define MGC_COMPL_SOURCE_TEST                           0x01
#define MGC_COMPL_SINK_TEST                             0x02
#define MGC_COMPL_LOOPBACK_TEST                         0x03

/** Return status when transfer test is completed */
#define MGC_COMPL_STATUS_XFER_COMPL                     0x01


/** Different states maintained by compliance device driver to decide next action to be taken. */
typedef enum
{
    /* At startup driver is in this state. */
    MGC_COMPL_STATE_INIT,

    /* In this state, Driver sends Set Config request to the device. */
    MGC_COMPL_STATE_CONFIGURED,

    /* In this state driver submits Get Endpoint Capability table request. */
    MGC_COMPL_STATE_RECV_CAP_TABLE,

    /**
     * In this state driver submits request extra buffer allocated for the endpoint. This is a repeatative
     * state. Driver remains in this state until all the endpoint is exausted.
     */
    MGC_COMPL_STATE_GET_EXTRA_BUFFER,

    /* In this sate driver keeps on executing transfer tests on different endpoint until all of the
     * endpoints are exausted. */
    MGC_COMPL_STATE_TRANSFER_TEST,

    /* In this state driver exeuctes disconnect/connect test.*/
    MGC_COMPL_STATE_DISCONNECT_TEST,

    /* In this state driver executes remote wakeup test. */
    MGC_COMPL_STATE_REMOTEWK_TEST,

    /* In this state driver does nothing and remains in this state until a reconnect happens.
     * When driver faces an unrecoverable error, it goes in this state.*/
    MGC_COMPL_STATE_ERROR
} MGC_ComplDriverState;

/**
 * MUSB_EndpointChangeData (for MUSB_COMPL_REQ_VEN_CHANGE_EP).
 * @field bFlags 0 : Valid
 *       1 : do loopback
 *       3:2 endpoint type
 *       5:4 number of transaction oppurtunities per micro-frame.
 *       7:6 reserved.
 * @field bReserved0 Reserved for future use.
 * @field wMaxPacketSize The max, modifiable.
 * @field dwPattern Base data pattern by device.
 * @field dwModifier Data pattern modifier.
 * @field bPeriod Poll interval, required for interrupt transfers.
 * @field bFixedRespMode For setting fixed Response mode.
 *           0x00 : Normal Response mode.
 *           0x01 : Always Stall handshake.
 *           0x02 : Always NAK handshake.
 *           0x04 : Always timeout response.
 *           0x08 : Always CRCERROR
 * @field bReserved1 Reserved for future use.
 */
typedef struct
{
    uint8_t     bFlags;
    uint8_t     bReserved0;
    uint16_t    wMaxPacketSize;
    uint32_t    dwPattern;
    uint32_t    dwModifier;
    uint8_t     bPeriod;
    uint8_t     bFixedRespMode;
    uint16_t    bReserved1;
} MUSB_ComplEndpointChangeData;

/**
 * Transfer type capability
 * @field bValid TRUE if transfer type is supported
 * @field bMaxExtraBuffer max for this transfer type
 * @field wMaxPacketSize max for this transfer type
 */
typedef struct
{
    uint8_t     bValid;
    uint8_t     bMaxExtraBuffer;
    uint16_t    wMaxPacketSize;
} MUSB_ComplTransferTypeCapability;


/**
 * Compliance device capabilities
 * @field bAddress endpoint address ([3:0]) and direction ([7])
 * @field bLoopbackIndex index of loopback partner
 * @field bmAttributes see MUSB_COMPL_S_COMPL_ATTIBUTES_*
 * @field bFixedRespCaps for setting fixed response mode
 * @field BulkCapabilities capabilities as a bulk endpoint
 * @field InterruptCapabilities capabilities as an interrupt endpoint
 * @field IsochCapabilities capabilities as an isochronous endpoint
 * @field ControlCapabilities capabilities as a control endpoint
 */
typedef struct
{
    uint8_t                         bAddress;
    uint8_t                         bLoopBackEpIndex;
    uint8_t                         bAttributes;
    uint8_t                         bFixedRespCaps;
    MUSB_ComplTransferTypeCapability bulkCapabilities;
    MUSB_ComplTransferTypeCapability interruptCapabilities;
    MUSB_ComplTransferTypeCapability isochCapabilities;
    MUSB_ComplTransferTypeCapability controlCapabilities;
} MUSB_ComplCapabilities;


/**
 * Compliance device capabilities Table.
 * @field bLength number of table entries.
 * @field pEpCapabilities Pointer to starting address of endpoint capabilities array.
 */
typedef struct
{
    uint8_t                 bLength;
    MUSB_ComplCapabilities  *pEpCapabilities;
} MUSB_ComplCapabilityTable;


/**
 * Test Data PAttern for different tests.
 * @field bCurTest Current transfer test in execution. (Control/Bulk/Interrupt/Iscoch).
 * @field bCurEndpoint Endpoint on which current transfer tests are being executed.
 * @field bCurTransferType Currently execution transfer test type (Source/Sink/Loopback test).
 * @field bLoopbackPartnerEnabled Flag used to disable loopback partner in Loopback Test.
 * @field bGetConfigDscr A flag used to indicate to receive new config descriptor after an endpoint change data class request.
 * @field bPeriod bPeriod value for the currently executing transfer.
 * @field bAdditionalTransactions This flag is set when driver generates a VEN_Do_Disconnect Request. When driver gets
 *                        gets a disconnect callback this flag indicates whether this disconnect is due to
 *                        disconnect/connect test or, a physical disconnect has happened.
 * @field bTestConnect This flag is used to differentiate between a test connect and Physcial connect.
 * @field bTestRemoteWakeup When a resume singal comes this flag indicates this resume has come due to remote wakeup test.
 *                     and Physical connect.
 * @field bDisconnectTime This value is used in VEN_Do_Disconnect request, to tell device that it should for
 *                        bDisconnect amount of time before generating Disconnect Message.
 * @field bConnectTime This value is used in VEN_Do_Disconnect request, to tell device that it should for
 *                        bConnect amount of time before generating Connect Message after disconnect.
 * @field bRemoteWakeupTime Amount of time with which remote wakeup test should be executed. After each iteration this
 *                   value is modified.
 * @field wCurMaxPacketSize Max Packet size for the currently executing transfer test.
 * @field wCurMaxBufferLength Extra buffer provided by device for this endpoint.
 * @field dwCurTransferIndex index pointing to current iteration for test in execution.
 * @field dwPattern dwPattern of endpoint change data request.
 * @field dwModifier dwModifier of endpoint change data request.
 */

typedef struct
{
    uint8_t                     bCurTest;
    uint8_t                     bCurEndpoint;
    uint8_t                     bCurTransferType;
    uint8_t                     bLoopbackPartnerEnabled;
    uint8_t                     bGetConfigDscr;
    uint8_t                     bPeriod;
    uint8_t                     bAdditionalTransactions;
    uint8_t                     bTestConnect;
    uint8_t                     bTestRemoteWakeup;
    uint8_t                     bDisconnectTime;
    uint8_t                     bConnectTime;
    uint8_t                     bRemoteWakeupTime;
    uint16_t                    wCurMaxPacketSize;
    uint16_t                    wCurMaxBufferLength;
    uint32_t                    dwCurTransferIndex;
    uint32_t                    dwPattern;
    uint32_t                    dwModifier;
} MGC_ComplTestData;

/**
 * Data structure used to communicate with the connected compliance device.
 * @field pUsbDevice Pointer to device information.
 * @field hBus Session Handle.
 * @field connectionSpeed Current operating speed of the device.
 * @field setup Setup Packet used in control transfers.
 * @field controlIrp Control Irp used for control transfer.
 * @field bulkIrp Bulk Irp used for bulk transfer.
 * @field interruptIrp Interrupt Irp used for interrupt transfer.
 * @field isochIrp Isochronus Irp used for Isochronus Transfer.
 * @field ePCapTable Endpoint Capability Table.
 * @field testData Different test data. This get modified for each of the test iteration.
 * @field dwInActualLength Amount of data received during a data transfer request.
 * @field dwOutActualLength Amount of data sent during a data transfer request.
 * @field dwStatus Status of previous data transfer.
 * @field driverState State of the compliance device driver. Depending on the state Device driver will decide its next action.
 * @field bDisconnectSupported A flag used to indicate whenther device supports disconnect test or, not.
 * @field inPipe Pipe used to receive data from the device.
 * @field outPipe Pipe used to send data to device.
 * @field adwBufferSize Array of buffer sizes allocated for different endpoints.
 * @field aEpTransferSupported This is an array of supported transfer types for endpoints. First four bits of each
 *                    byte in the array is used as a bit map to indicate whether this transfer type is
 *                    supported or, not.
 *                    First Nibble is interpreted as,
 *                    b0 : Control transfer is supported on this endpoint or, not.
 *                    b1 : Interrupt transfer is supported on this endpoint or, not.
 *                    b2 : Bulk transfer is supported on this endpoint or, not.
 *                    b3 : Isochronus transfer is supported on this endpoint or, not.
 *
 *                    Second nibble tells the loopback support for different transfers.
 *                    b4 : Loopback for Control transfer is supported on this endpoint or, not.
 *                    b5 : Loopback for Interrupt transfer is supported on this endpoint or, not.
 *                    b6 : Loopback for Bulk transfer is supported on this endpoint or, not.
 *                    b7 : Loopback for Isochronus transfer is supported on this endpoint or, not.
 * @field aEpTransferExecuted This is an array of executed transfer tests (Source/Sink/Loopback) test for endpoints. Bits are interprated as,
 *                   b0 : Source/Sink Test has been executed for this endpoint or, not.
 *                   b1 : Loopback test is executed on this endpoint or, not.
 *                   b2-b7 : Reserved
 * @field aCfgDscr Current Configuration Descriptor of the connected device.
 * @field wCfgDscrLength Total Length of the config Descriptor.
 * @field aInBuffer Buffer used to hold incoming data from the device.
 * @field aOutBuffer Buffer used to send data to device.
 */
typedef struct
{
    MUSB_DeviceDriver           *pDriver;
    MUSB_Device                 *pUsbDevice;
    MUSB_BusHandle              hBus;
    MUSB_ConnectionSpeed        connectionSpeed;
    MUSB_DeviceRequest          setup;
    MUSB_ControlIrp             controlIrp;
    MUSB_Irp                    bulkIrp;
    MUSB_Irp                    interruptIrp;
    MUSB_IsochIrp               isochIrp;
    MUSB_ComplCapabilityTable   ePCapabilityTable;
    MGC_ComplTestData           testData;
    uint32_t                    dwInActualLength;
    uint32_t                    dwOutActualLength;
    uint32_t                    dwStatus;
    MGC_ComplDriverState        driverState;
    uint8_t                     bDisconnectSupported;
    uint8_t                     bUpdateTestData;
    MUSB_PipePtr                   inPipe;
    MUSB_PipePtr                   outPipe;
    uint32_t                    adwBufferSize[MUSB_COMPL_NUM_EPS];
    uint8_t                     aEpTransferSupported[MUSB_COMPL_NUM_EPS];
    uint8_t                     aEpTransferExecuted[MUSB_COMPL_NUM_EPS];
    uint16_t                    wCfgDscrLength;
    uint8_t                     aCfgDscr[MUSB_COMPL_MAX_DSCR_LENGTH];
    uint8_t                     aInBuffer[MUSB_COMPL_DATA_BUFFER_SIZE];
    uint8_t                     aOutBuffer[MUSB_COMPL_DATA_BUFFER_SIZE];
} MUSB_ComplDeviceInfo;


/**
 * Function used to send endpoint change data control request to the device.
 * @param pComplDeviceInfo Pointer to top level device driver context.
 * @param bEndpoint Endpoint index for which this request is being sent.
 * @param bTransferType Transfer type for change endpoint data request.
 * @param bLoopback Bit indicates whether endpoint is configured for loopback transfer.
 * @param wMaxPacketSize Max Packet size of transfer.
 * @param bEnable Flag indicates whether to enable endpoint for the transfer of, disable it after executing transfer tests.
 * @param pfCallback Callback function for this request.
 * @return status returned by submission of control Irp for endpoint change class request.
 */
extern uint32_t MGC_ComplSendChangeEpData(MUSB_ComplDeviceInfo *pComplDeviceInfo,
        uint8_t       bEndpoint,
        uint8_t       bTransferType,
        uint8_t       bLoopback,
        uint8_t       bAdditionalTransactions,
        uint16_t      wMaxPacketSize,
        uint8_t       bEnable,
        MUSB_pfControlIrpComplete pfCallback
                                         );

/**
 * Callback function for change endpoint class request when endpoint is configured before a starting a transfer test.
 * @param pContext Pointer to context for this callback.
 * @param pControlIrp Pointer to control Irp for which this callback is called.
 * @return None.
 */
extern void MGC_ComplEpDataChangeCallback(void *pContext, MUSB_ControlIrp *pControlIrp);

/**
 * Function used to get max packet size and extra buffer for a transfer for given endpoint capability.
 * @param pEpCapability Pointer to endpoint capability.
 * @param bTransferType Trnasfer type.
 * @param pWMaxPacketSize Pointer to wMaxPacketSize.
 * @param pExtraBufferSupported Pointer to flag indicating whether this transfer supports extra buffer or, not.
 * @return None.
 */
extern void
MGC_ComplGetEndpointDetails(MUSB_ComplCapabilities  *pEpCapability,
                            uint8_t                 bTransferType,
                            uint16_t                *pWMaxPacketSize,
                            uint8_t                 *pExtraBufferSupported );


/**
 * Decodes Endpoint capability table from given byte stream.
 * This fuction is called by state handler after receving the endpoint capability table in
 * byte array form.
 * This function decodes the given byte strean into endpoint capability table.
 * @param pBuffer Pointer to character array.
 * @param pEpCapTable Pointer to endpoint capability table in which this buffer is decoded.
 * @return MUSB_STATUS_OK if buffer is decoded properly.
 * @return MUSB_FAILURE otherwise.
 */
extern uint32_t MGC_ComplDecodeEpCapTable(uint8_t *pBuffer, MUSB_ComplCapabilityTable *pEpCapTable);


/*
 * Fills the device endpoint.
 * This function is called by various function before claiming a pipe.
 * This function fills the device endpoint information with endpoint descriptor, Bus address,
 * Nak Limit and Timeout information.
 * @param pDeviceEndpoint Pointer to device endpoint.
 * @param pEpDscr Pointer to endpoint descriptor.
 * @param pUsbDevice Pointer to USB Device.
 * @param wNakLimit Max NAK Limit.
 */
extern void MGC_ComplFillDeviceEndpoint(MUSB_DeviceEndpoint *pDeviceEndpoint,

                                        const MUSB_EndpointDescriptor *pEpDscr,

                                        MUSB_Device                  *pUsbDevice,

                                        uint16_t     wNakLimit);

/**
 * Finds endpoint descriptor.
 * This function is called by various functions to find an endpoint descriptor.
 * This function finds the endpoint descriptor for the given endpoint address from the
 * given descriptor. If no such endpoint descriptor exists it returns NULL.
 * @param pConfig Pointer to configuration descriptor.
 * @param pInterfaceDescriptor Pointer to the interface descriptor.
 * @param bEndpointAddress Endpoint address to be searched.
 * @return Pointer to endpoint address if address found.
 * @return NULL Otherwise.
 */
extern const MUSB_EndpointDescriptor *MGC_ComplFindEndpointDescriptor(
    const MUSB_ConfigurationDescriptor *pConfig,
    const MUSB_InterfaceDescriptor *pInterfaceDescriptor,
    uint8_t bEndpointAddress);

#endif  /* multiple inclusion protection */

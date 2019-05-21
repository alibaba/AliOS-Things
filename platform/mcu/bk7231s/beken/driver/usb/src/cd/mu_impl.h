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

/*
 * Controller/port implementation definitions
 * $Revision: 1.48 $
 */

#ifndef __MUSB_IMPL_H__
#define __MUSB_IMPL_H__

#include "mu_cntlr.h"
#include "mu_cdi.h"
#include "mu_list.h"
#include "mu_hset.h"
#include "mu_dci.h"
#include "mu_arch.h"
#include "mu_diag.h"

/* (function mode) maximum supported interfaces in a configuration */
#define MGC_MAX_CONFIG_IFACES           16

/* finish conditional-compilation defs as needed */
#if defined(MUSB_FDRC) && !defined(MUSB_OTG)
#define MUSB_OTG
#endif

#if defined(MUSB_HDRC) && !defined(MUSB_OTG)
#define MUSB_OTG
#endif

#if defined(MUSB_MHDRC) && !defined(MUSB_OTG)
#define MUSB_OTG
#endif

#if defined(MUSB_FSFC) && !defined(MUSB_FUNCTION)
#define MUSB_FUNCTION
#endif

#if defined(MUSB_HSFC) && !defined(MUSB_FUNCTION)
#define MUSB_FUNCTION
#endif

#define MGC_MIN(_n1_, _n2_) (_n2_ < _n1_ ? _n2_ : _n1_)
#define MGC_MAX(_n1_, _n2_) (_n2_ > _n1_ ? _n2_ : _n1_)

/*
 * Diagnostics
 */
#ifndef MGC_DIAG

/* Print diagnostic */
#define MGC_DIAG(_bLevel, _pC, _pMsg)

#define MGC_DIAG1(_bLevel, _pC, _pText1, _dwNum1, _bBase, _bJust)
#define MGC_DIAG2(_bLevel, _pC, _pText1, _dwNum1, _pText2, _dwNum2, _bBase, _bJust)

#endif

/**
 * BSR queue item.  An item for the queue from ISR to BSR.
 * NOTE: members are not declared volatile; code will cast as necessary
 *
 * @field dwTime timestamp when interrupt received
 * @field bCause cause (see MGC_BSR_CAUSE_*)
 * @field bLocalEnd local endpoint index (counting from 0)
 * @field bStatus status code
 * @field pData (IRP)
 */
typedef struct
{
#ifdef MUSB_OVERHEAD
    uint32_t dwTime;
#endif
    void *pData;
    uint8_t bCause;
    uint8_t bLocalEnd;
    uint8_t bStatus;
} MGC_BsrItem;

/** None (maybe cancelled) */
#define MGC_BSR_CAUSE_NONE		        0x00
/** Session request */
#define MGC_BSR_CAUSE_SESSION_REQ	    0x01
/** Connect */
#define MGC_BSR_CAUSE_CONNECT		    0x02
/** Disconnect */
#define MGC_BSR_CAUSE_DISCONNECT 	    0x03
/** bus reset (peripheral only) */
#define MGC_BSR_CAUSE_RESET		        0x05
/** suspend detected */
#define MGC_BSR_CAUSE_SUSPEND		    0x06
/** resume detected */
#define MGC_BSR_CAUSE_RESUME		    0x07
/** Vbus error */
#define MGC_BSR_CAUSE_VBUS_ERROR	    0x08

#define MGC_BSR_CAUSE_ENUMERATION_OK	0x09

/** Tx IRP complete; use pData */
#define MGC_BSR_CAUSE_IRP_COMPLETE	    0x23
/** Rx IRP complete; use pData */
#define MGC_BSR_CAUSE_RX_IRP_COMPLETE	0x24

/** Maximum bytes possible to receive from peripheral on SETUP IN */
#define MGC_MAX_SETUP_IN	    256

/*
 *  OTG mode definitions. ABxx used when mode, not A or B, needed
 */
#define MGC_AB_MODE(x)  ((x) & 0xf) /* mask off A-B indicator */

#define MGC_AB_PERIPHERAL           0x1
#define MGC_AB_WAIT_CON             0x2
#define MGC_AB_HOST                 0x3

/*  end OTG  mode defs */

/*
 *  USB states, modified for meaning in an OTG environment
 */
#define MGC_POWERED     MUSB_POWERED   /* CONNECT int in, Reset pending */
#define MGC_DEFAULT     MUSB_DEFAULT   /* CONNECT in, Reset completed */
#define MGC_ADDRESS     MUSB_ADDRESS   /* Above,+ peripheral SET_ADDRESS completed */
#define MGC_CONFIGURED  4   /* Above,+ enumeration completed. OK for traffic*/

/*
 *  Note on above: Suspend depends on A or B device mode.  B doesn't
 *  allow for a suspend mode, even while being a host.  a_suspend
 *  defined explicitly by OTG spec and so is reflected in OTGState.
 */

/*
 *  OTG State timers, per OTG spec Chapter 5
 *  Unless noted otherwise, constants expressed in milliseconds
 */
#define MGC_TA_SRP_RSPNS        5000    /* Max */
#define MGC_TA_WAIT_VRISE       100     /* Max */
#define MGC_TA_BCON_LDB         100     /* Min */
#define MGC_TA_AIDL_BDIS        250     /* Min */
#define MGC_TA_BDIS_ACON        3000    /* Max */
#define MGC_TA_BIDL_ADIS_MIN    3       /* Min */
#define MGC_TA_BIDL_ADIS_MAX    200     /* Max */
#define MGC_TLDIS_DSCHG         0.025   /* Min */
#define MGC_TA_BCON_SDB         0.0025  /* Min */
#define MGC_TA_BCON_SDB_WIN     100     /* Max */

#define MGC_TB_ASE0_BRST        3.125   /* Min */
/*  End, OTG State Timer defs */

/*
 *  USB timers
 */
#define MGC_TM_PERIPHERAL_RESUME    10  /* MS for signalling RESUME */
#define MGC_TM_HOST_RESUME          20  /* ditto for host */
/*  End, USB Timers */

/*
 *  DRC timers
 */
#define MGC_B_DEV_VBUSSESS_BOUNCE   500 /* 0.5 sec allowed for VBus to drop */
#define MGC_DRC_DISCONNECT          250 /* 250 msec for DML */

/*  End, DRC timers */

struct _MGC_Controller;
struct _MGC_Port;
struct _MGC_ScheduleSlot;

/**
 * MGC_EndpointResource.
 * A local endpoint.
 * MUSB_EndpointResource's pPrivateData points to this
 * @field bLocalEnd core's endpoint index
 * @field bIsFifoShared TRUE if FIFO is shared between Tx/Rx
 * @field wMaxPacketSizeTx Tx FIFO size
 * @field wMaxPacketSizeRx Rx FIFO size
 * @field bBusAddress target (host role only)
 * @field bBusEnd target endpoint (host role) or local endpoint (function role)
 * @field bInterval USB 2.0
 * @field bTrafficType USB 2.0
 * @field bIsTx TRUE if current direction is transmit (used in shared FIFO endpoints)
 * @field bIsHalted TRUE if halted
 * @field wPacketSize actual packet size
 * @field bIsClaimed TRUE if claimed
 * @field pIrp pending IRP if any
 * @field dwWaitFrameCount how many frames to wait before first isoch IRP
 * @field IrpList linked list of pending IRPs
 * @field dwOffset current offset in user buffer
 * @field dwIsrTime total ISR time for current transfer
 * @field pPrivateData controller-specific code can use this
 * @field bMoveFifo set during dynamic FIFO compaction
 */
typedef struct
{
    /* NOTE: BOOLs can be changed to bitfields for smaller data, larger code */

    /* hardware state */
    uint8_t bLocalEnd;
    uint8_t bIsFifoShared;
    uint16_t wMaxPacketSizeTx;
    uint16_t wMaxPacketSizeRx;

    /* software state */
    uint8_t bBusAddress;
    uint8_t bBusEnd;
    uint8_t bInterval;
    uint8_t bTrafficType;
    uint8_t bRxBusAddress;
    uint8_t bRxBusEnd;
    uint8_t bRxInterval;
    uint8_t bRxTrafficType;
    uint8_t bIsTx;
    uint8_t bIsHalted;
    uint8_t bIsClaimed;
    uint8_t bRxClaimed;
    uint8_t bStopTx;
    uint8_t bIsRxHalted;
    uint16_t wPacketSize;
    uint16_t wRxPacketSize;
    uint32_t dwTxSize;

    void *pTxIrp;
    MUSB_LinkedList TxIrpList;
    uint32_t dwTxOffset;
    void *pRxIrp;
    MUSB_LinkedList RxIrpList;
    uint32_t dwRxOffset;
#ifdef MUSB_OVERHEAD
    uint32_t dwIsrTime;
#endif

#ifdef MUSB_DMA
    MUSB_DmaChannel *pDmaChannel;
    MUSB_DmaChannel *pRxDmaChannel;
    uint16_t wDmaBufferCount;
    uint16_t wRxDmaBufferCount;
    uint8_t bDmaTx;
    uint8_t bDmaResidual;
#endif

    void *pPrivateData;
    uint32_t dwWaitFrameCount;

    /* dynamic FIFO support */
    uint8_t bTxFifoSize;    /* save since core won't read it */
    uint8_t bRxFifoSize;    /* save since core won't read it */

    uint8_t bIsrResidual;
    uint8_t bBsrResidual;
} MGC_EndpointResource;

/**
 * MGC_Pipe.
 * Pipe implementation.
 * MUSB_PipePtr points to this.
 * NOTE: members are not declared volatile; code will cast as necessary
 * @field hSession session handle
 * @field pLocalEnd local endpoint resource pointer
 * @field pPort port (implementation) pointer
 * @field pSlot pointer to schedule slot, if applicable
 * @field pDevice associated device; only valid if host
 * @field bTrafficType from USB 2.0
 * @field wMaxPacketSize from USB 2.0
 * @field wInterval interval, in (micro)frames
 * @field dwMaxBusTime computed maximum bus time, in nanoseconds
 * @field bmFlags bitmask of flags; see MGC_PIPEFLAGS_*
 */
typedef struct _MGC_Pipe
{
    void *hSession;
    MGC_EndpointResource *pLocalEnd;
    struct _MGC_Port *pPort;
    struct _MGC_ScheduleSlot *pSlot;
    const MUSB_Device *pDevice;
    uint16_t wMaxPacketSize;
    uint16_t wInterval;
    uint32_t dwMaxBusTime;
    uint32_t bmFlags;
    uint8_t bTrafficType;
} MGC_Pipe;
/** opened as host */
#define MGC_PIPEFLAGS_HOST	(1 << 0)
/** closing (app request or device disconnected) */
#define MGC_PIPEFLAGS_CLOSING	(1 << 1)
/** suspended */
#define MGC_PIPEFLAGS_SUSPENDED	(1 << 2)
/** transmit */
#define MGC_PIPEFLAGS_TRANSMIT	(1 << 7)

/**
 * MGC_ScheduleSlot.
 * A slot in the periodic transfer schedule.
 *
 * @field bIsActive TRUE when active
 * @field wInterval interval
 * @field wFrameOffset offset from schedule reset (imaginary reference frame)
 * @field wDuration how many consecutive (micro)frames
 * @field wFramesRemaining consecutive (micro)frames remaining
 * @field PipeList pipes with actual transfers
 */
typedef struct _MGC_ScheduleSlot
{
    MUSB_LinkedList PipeList;
#ifdef MUSB_SCHEDULER
    uint16_t wInterval;
    uint16_t wFrameOffset;
    uint16_t wDuration;
    uint16_t wFramesRemaining;
    uint8_t bIsActive;
#endif
} MGC_ScheduleSlot;

/**
 * MGC_Schedule.
 * A periodic transfer schedule.
 * @field wSlotCount schedule is normalized to this
 * @field ScheduleSlots sparse list of scheduled transfers (host only)
 */
typedef struct
{
    uint32_t dwTotalTime;
    MUSB_LinkedList ScheduleSlots;
    uint16_t wSlotCount;
} MGC_Schedule;

/**
 * MGC_Device.
 * Implementation device.
 * MUSB_Device's pPrivateData points to this.
 * @field pDriver device driver
 * @field pSchedule schedule (hubs only)
 * @field PipeList open pipes
 * @field Device device contained here to reduce mallocs
 */
typedef struct
{
    MUSB_DeviceDriver *pDriver;
    MGC_Schedule *pSchedule;
    MUSB_LinkedList PipeList;
    MUSB_Device Device;
#ifdef MUSB_HUB
    uint16_t wbmTxToggle;
    uint16_t wbmRxToggle;
    uint8_t bIsMultiTt;
#endif
} MGC_Device;

/*
 * Functions that may be controller-specific
 * This is where registers get twiddled; no place above.
 */

/**
 * Start controller.
 * Program the underlying controller to start
 * (enable interrupts, bus reset, etc.).
 * @param 1 controller pointer
 * @return status code
 */
typedef uint32_t (*MGC_pfProgramStartController)(struct _MGC_Controller *);

/**
 * Stop controller.
 * Program the underlying controller to stop
 * (disable interrupts, bus reset, etc.).
 * @param 1 controller pointer
 * @return status code
 */
typedef uint32_t (*MGC_pfProgramStopController)(struct _MGC_Controller *);

/**
 * Destroy controller.
 * Perform any controller-specific cleanup.
 * @param 1 controller pointer
 * @return status code
 */
typedef uint32_t (*MGC_pfDestroyController)(struct _MGC_Controller *);

/**
 * Set controller's power delivery capability in host mode
 * @param 1 controller pointer
 * @param 2 maximum current in units of 2 mA
 * @return status code
 */
typedef uint32_t (*MGC_pfSetControllerHostPower)(struct _MGC_Controller *, uint16_t);

/**
 * Dump controller state.
 * Dump info global to the controller
 * @param 1 controller pointer
 * @return how many characters were placed in buffer
 */
typedef int (*MGC_pfDumpControllerState)(struct _MGC_Controller *);

/**
 * Dump pipe state.
 * Dump info relevant to the given pipe
 * @param 1 controller pointer
 * @param 2 pipe pointer
 * @return how many characters were placed in buffer
 */
typedef int (*MGC_pfDumpPipeState)(struct _MGC_Controller *, MGC_Pipe *);

/**
 * Dump endpoint state.
 * Dump info relevant to the given local endpoint resource
 * @param 1 controller pointer
 * @param 2 endpoint resource pointer
 * @return how many characters were placed in buffer
 */
typedef int (*MGC_pfDumpLocalEndState)(struct _MGC_Controller *,
                                       MUSB_EndpointResource *);

/**
 * MGC_Controller.
 * Controller implementation.
 * MUSB_Controller's pPrivateData points to this
 *
 * @field wControllerType type (see MUSB_CONTROLLER_*)
 * @field pControllerAddressIsr Address at interrupt time
 * @field pControllerAddressIst Address at non-interrupt-time
 * @field pUtils system utilities callbacks pointer
 * @field pSystemServices system services callbacks pointer
 * @field pPrivateData "subclasses" could use this
 * @field pPort all current cores are single-port
 * @field pDmaControllerFactory DMA controller factory
 *
 * @field pfProgramStartController start the controller
 * @field pfProgramStopController stop the controller
 * @field pfSetControllerHostPower set host-mode power delivery capability
 * @field pfDumpControllerState dump controller state
 * @field pfDumpPipeState dump pipe state
 * @field pfDumpLocalEndState dump endpoint state
 */
typedef struct _MGC_Controller
{
    void *pControllerAddressIsr;
    void *pControllerAddressIst;
    MUSB_SystemUtils *pUtils;
    MUSB_SystemServices *pSystemServices;
    void *pPrivateData;
    struct _MGC_Port *pPort;
#ifdef MUSB_DMA
    MUSB_DmaControllerFactory *pDmaControllerFactory;
#endif

    MGC_pfProgramStartController pfProgramStartController;
    MGC_pfProgramStopController pfProgramStopController;
    MGC_pfDestroyController pfDestroyController;
    MGC_pfSetControllerHostPower pfSetControllerHostPower;

#if MUSB_DIAG > 0
    MGC_pfDumpControllerState pfDumpControllerState;
    MGC_pfDumpPipeState pfDumpPipeState;
    MGC_pfDumpLocalEndState pfDumpLocalEndState;
#endif	/* diagnostics enabled */
    uint16_t wControllerType;
} MGC_Controller;

/*
 * Functions that may be controller- and port-specific
 * This is where registers get twiddled; no place above.
 */

/**
 * Read bus state.
 * Fill the bus state info from the port (see MGC_Port).
 * This is where registers get twiddled; no place above.
 * @param 1 port pointer
 * @return status code
 */
typedef uint32_t (*MGC_pfReadBusState)(struct _MGC_Port *);

/**
 * Set bus state.
 * Program the bus state from the port (see MGC_Port).
 * This is where registers get twiddled; no place above.
 * @param 1 port pointer
 * @return status code
 */
typedef uint32_t (*MGC_pfProgramBusState)(struct _MGC_Port *);

/**
 * Reset port.
 * Reset the port (e.g. unbind/flush all endpoints, etc.).
 * This is where registers get twiddled; no place above.
 * @param 1 port pointer
 * @return status code
 */
typedef uint32_t (*MGC_pfResetPort)(struct _MGC_Port *);

/**
 * Find local endpoint resource.
 * Find (maybe bind) the underlying port's first (host) or required (function)
 * local endpoint resource to communicate with the given remote endpoint (host)
 * or to be identified with the given endpoint descriptor (function).
 * @param 1 port pointer
 * @param 2 remote endpoint (host) or advertised endpoint (function)
 * @param 3 desired attributes of local endpoint resource
 * @param 4 TRUE to bind; FALSE to just check
 * @return pointer to resource on success
 * @return NULL on failure
 */
typedef MGC_EndpointResource *(*MGC_pfBindEndpoint)(struct _MGC_Port *,
        const MUSB_DeviceEndpoint *,
        MUSB_EndpointResource *, uint8_t);

/**
 * Start a receive.
 * Program the underlying port to initiate a receive.
 * The implementation is responsible for filling the buffer
 * with any data already received before this call,
 * and also must set the endpoint's current IRP pointer
 * whenever additional packets are required to fulfill the IRP.
 * This is where registers get twiddled; no place above.
 * @param 1 port pointer
 * @param 2 local endpoint resource, as returned from bind
 * @param 3 buffer pointer
 * @param 4 TOTAL byte count expected
 * @param 5 IRP pointer
 * @param 6 TRUE to allow DMA; FALSE otherwise
 * @return status code
 */
typedef uint32_t (*MGC_pfProgramStartReceive)(struct _MGC_Port *,
        MGC_EndpointResource *, uint8_t *, uint32_t, void *, uint8_t);

/**
 * Start a transmit.
 * Program the underlying port to initiate a transmit.
 * This is where registers get twiddled; no place above.
 * @param 1 port pointer
 * @param 2 local endpoint resource, as returned from bind
 * @param 3 buffer pointer
 * @param 4 TOTAL byte count to send
 * @param 5 IRP pointer
 * @return status code
 */
typedef uint32_t (*MGC_pfProgramStartTransmit)(struct _MGC_Port *,
        MGC_EndpointResource *, const uint8_t *, uint32_t, void *);

/**
 * Flush endpoint.
 * Program the underlying port to flush the given endpoint resource,
 * with respect to the given USB direction (IN or OUT).
 * The resource should be ready for normal operation by return time.
 * This is where registers get twiddled; no place above.
 * @param 1 port pointer
 * @param 2 local endpoint resource, as returned from bind
 * @param 3 direction, a la USB device framework
 * @param 4 TRUE to prepare to re-use the endpoint; FALSE otherwise
 * @return status code
 */
typedef uint32_t (*MGC_pfProgramFlushEndpoint)(struct _MGC_Port *, MGC_EndpointResource *,
        uint8_t, uint8_t);

/**
 * Program stall state (device mode) or stop/restart traffic (host mode).
 * In device mode, program the stall state for the underlying port's given endpoint.
 * In host mode, stop or restart traffic on the given local endpoint
 * (since immediate stopping may not be possible,
 * callers must wait for the endpoint's bIsHalted flag to become true).
 * This is where registers get twiddled; no place above.
 * @param 1 port pointer
 * @param 2 local endpoint resource, as returned from bind
 * @param 3 direction, a la USB device framework
 * @param 4 TRUE to set halt or stop traffic; FALSE to clear halt or restart traffic
 * @return status code
 */
typedef uint32_t (*MGC_pfProgramHaltEndpoint)(struct _MGC_Port *,
        MGC_EndpointResource *,
        uint8_t, uint8_t);

/**
 * Respond on the default endpoint.
 * pPort->pSetupData should point to IN data, or NULL if none.
 * pPort->wSetupDataSize should be length of IN data.
 * @param 1 port pointer
 * @param 2 TRUE to stall; FALSE to respond
 * @return status code
 */
typedef uint32_t (*MGC_pfDefaultEndResponse)(struct _MGC_Port *, uint8_t);

/**
 * Service EP0 interrupt.
 * Service default endpoint interrupt
 * @param 1 port pointer
 * @param 2 work queue item pointer
 * @return TRUE to queue work
 */
typedef uint8_t (*MGC_pfServiceDefaultEnd)(struct _MGC_Port *, MGC_BsrItem *);

/**
 * Service non-default Tx.
 * Service an endpoint Tx interrupt
 * @param 1 port pointer
 * @param 2 endpoint local resource index
 * @param 3 work queue item pointer
 * @return TRUE to queue work
 */
typedef uint8_t (*MGC_pfServiceTransmitAvail)(struct _MGC_Port *, uint16_t, MGC_BsrItem *);

/**
 * Service non-default Rx.
 * Service an endpoint Rx interrupt
 * @param 1 port pointer
 * @param 2 endpoint local resource index
 * @param 3 work queue item pointer
 * @return TRUE to queue work
 */
typedef uint8_t (*MGC_pfServiceReceiveReady)(struct _MGC_Port *, uint16_t, MGC_BsrItem *);

/**
 * Load an endpoint FIFO
 * @param 1 port pointer
 * @param 2 endpoint local resource index
 * @param 3 byte count to load
 * @param 4 where to get data
 * @return TRUE on success
 */
typedef uint8_t (*MGC_pfLoadFifo)(struct _MGC_Port *, uint8_t, uint32_t, const uint8_t *);

/**
 * Unload an endpoint FIFO
 * @param 1 port pointer
 * @param 2 endpoint local resource index
 * @param 3 byte count to unload
 * @param 4 where to store data
 * @return TRUE on success
 */
typedef uint8_t (*MGC_pfUnloadFifo)(struct _MGC_Port *, uint8_t, uint32_t, uint8_t *);

/**
 * Accept/reject device.
 * Check if device is acceptable; take appropriate action if not.
 * The enumerator calls this after gathering all configuration data
 * and attempting a driver match.
 * @param 1 port pointer
 * @param 2 device pointer
 * @param 3 pointer to driver that matched, or NULL if none
 * @return TRUE if device is acceptable
 * @return FALSE if not (and appropriate action taken)
 */
typedef uint8_t (*MGC_pfAcceptDevice)(struct _MGC_Port *, MUSB_Device *, MUSB_DeviceDriver *);

/**
 * Set a port to a test mode.
 * @param 1 port pointer
 * @param 2 mode
 */
typedef void (*MGC_pfSetPortTestMode)(struct _MGC_Port *, MUSB_HsetPortMode eMode);

/**
 * Map a dynamic FIFO.
 * @param 1 port pointer
 */
typedef void (*MGC_pfDynamicFifoLocation)(struct _MGC_Port *pPort,
        uint8_t bEnd,
        uint8_t bIsTx,
        uint8_t bSet,
        uint32_t *pdwBase,
        uint8_t *pbSize83,
        uint8_t *pbDoubleBuffered);

/**
 * MGC_OverheadStats.
 * Statistics on overheads (time units depend on measurement method)
 * @field dwOverheadMin best overhead
 * @field dwOverheadMax worst overhead
 */
typedef struct
{
    uint32_t dwOverheadMin;
    uint32_t dwOverheadMax;
} MGC_OverheadStats;

/**
 * MGC_EndpointStats.
 * Endpoint statistics
 * @field dwInterruptCount how many interrupts seen
 * @field dwRxByteCount how many bytes received
 * @field dwTxByteCount how many bytes sent
 */
typedef struct
{
    uint32_t dwInterruptCount;
    uint32_t dwRxByteCount;
    uint32_t dwTxByteCount;
} MGC_EndpointStats;

/**
 * MGC_EnumerationData.
 * Data for the CDI's enumerator.
 * @field bAddress the address being assigned
 * @field bState current state of enumeration attempt
 * @field bIndex data pertinent to enumeration state
 * @field bCount data pertinent to enumeration state
 * @field wData data pertinent to enumeration state
 * @field dwData data pertinent to enumeration state
 * @field pfEnumerationComplete callback for enumeration complete
 * @field pDevice device currently being enumerated
 * @field aSetupTx current/last request
 * @field aSetupRxEP0 Rx data
 * @field Irp IRP for enumeration
 * @field abmAddress bitmask of used addresses
 * @field bRetries current retry count
 * @field bFatalRetries retry count for fatal conditions
 */
typedef struct
{
    MUSB_pfHubEnumerationComplete pfEnumerationComplete;
    MUSB_Device *pDevice;
    uint8_t aSetupTx[8];
    uint8_t aSetupRx[MGC_MAX_SETUP_IN];
    uint8_t abmAddress[16];
    MUSB_ControlIrp Irp;
    uint32_t dwData;
    uint16_t wData;
    uint8_t bAddress;
    uint8_t bState;
    uint8_t bIndex;
    uint8_t bCount;
    uint8_t bRetries;
    uint8_t bFatalRetries;
} MGC_EnumerationData;

/**
 * MGC_Port.
 * Implementation port.
 * MUSB_Port's pPrivateData points to this.
 * @field pController parent controller
 * @field bSessionActive TRUE if session is active
 * @field bUsbState current USB state
 * @field bOtgState current OTG state
 * @field bRelinquish TRUE if we suspended to relinquish host role
 * @field bWantHighSpeed TRUE to enable high-speed
 * @field bWantReset (host/OTG) TRUE to assert bus reset
 * @field bWantSuspend TRUE to suspend (host/OTG) or enable suspend (function)
 * @field bWantResume TRUE to assert resume
 * @field bFuncAddr function address (+ some DRCs as host)
 * @field bWantSession (OTG only) TRUE to request session
 * @field bWantHost (OTG only) TRUE to request host
 * @field bWantTestMode TRUE to enter a test mode
 * @field bTestMode which test mode
 * @field dwFrame current frame number
 * @field bIsHighSpeed TRUE if high-speed detected
 * @field bIsLowSpeed TRUE if low-speed detected
 * @field bIsReset TRUE if bus is in reset
 * @field bIsSuspend TRUE if bus is suspended
 * @field bIsResume TRUE if resume signalling detected
 * @field bIsSession (OTG only) TRUE if session is active
 * @field bIsHost (OTG only) TRUE if we are host
 * @field bConnectorId (OTG only) connector ID
 * @field bBusVoltage (OTG only) a VBUS_ constant
 * @field bEndCount count of endpoints
 * @field wEndMask mask of endpoints
 * @field pInterfacePort interface-level port
 * @field pPrivateData a "subclass" could use this
 * @field LocalEnds local endpoint resources
 * @field pOtgClient OTG client pointer
 * @field bIsHnpSupported TRUE if we can HNP
 * @field bIsHnpAllowed TRUE if HNP is allowed by host
 * @field EnumerationData enumeration data
 * @field pHostClient host client pointer
 * @field pRootDevice root device
 * @field ConnectedDeviceList connected devices
 * @field pFunctionClient function client pointer
 * @field apConfigDescriptors
 * @field pCurrentConfig
 * @field apPipe automatically-opened pipes on config
 * @field bMaxFunctionEnds size of above array
 * @field bCanWakeup TRUE if remote wakeup allowed
 * @field dwSequenceNumber sequence number for request/response
 * @field wSetupRxIndex current index in above
 * @field wSetupDataSize size of extended data phase
 * @field pSetupData SETUP IN data, if any
 * @field wSetupDataIndex current index in above
 * @field bSetAddress TRUE to set address after current transaction
 * @field bConfigValue current configuration value
 * @field IsrOverhead ISR overhead measurements
 * @field OverheadStats measured overheads
 * @field EndpointStats endpoint statistics
 * @field dwResetCount
 * @field dwSuspendCount
 * @field dwResumeCount
 * @field dwResponseTimeoutCount
 * @field pDmaController
 * @field abIfaceOffsets (function mode) array of offsets for endpoint pipes per interface
 */
typedef struct _MGC_Port
{
    MGC_Controller *pController;
    MUSB_Port *pInterfacePort;
    void *pPrivateData;
    MUSB_Array LocalEnds;

    /* NOTE: pfProgramBusState must act on these values: */
    uint8_t bWantHighSpeed;
    uint8_t bWantReset;
    uint8_t bWantSuspend;
    uint8_t bWantResume;
    uint8_t bFuncAddr;
    uint8_t bWantSession;
    uint8_t bWantHost;
    uint8_t bWantTestMode;
    uint8_t bTestMode;
    /* END pfProgramBusState note */

    uint8_t bSessionActive;

    /* NOTE: specific to Inventra cores: */
    uint8_t bHasBulkSplit;
    uint8_t bHasBulkCombine;
    uint8_t bHasHbIsoTx;
    uint8_t bHasHbIsoRx;
    uint8_t bIsMultipoint;
    uint8_t bEndCount;

    /* NOTE: pfReadBusState must update these values: */
    uint32_t dwFrame;
    uint8_t bIsHighSpeed;
    uint8_t bIsLowSpeed;
    uint8_t bIsReset;
    uint8_t bIsSuspend;
    uint8_t bIsResume;
    uint8_t bIsSession;
    uint8_t bIsHost;
    uint8_t bConnectorId;
    uint8_t bBusVoltage;
    /* END pfReadBusState note */

    uint8_t bEnd0Status;
    uint16_t wEndMask;
    uint8_t bDmaCompleted;
    uint8_t bVbusErrorRetries;
    uint8_t bHasDisablePing;
    uint8_t bBabbleRetries;
    uint8_t bHasRqPktCount;
    uint8_t bPad1;
    uint8_t bPad2;
    uint8_t bPad3;

#ifdef MUSB_OTG
    uint8_t bOtgState;
    uint8_t bRelinquish;
    uint8_t bIsHnpSupported;
    uint8_t bIsHnpAllowed;
    MUSB_OtgClient *pOtgClient;
#endif

#if defined(MUSB_HOST) || defined(MUSB_OTG)
    MGC_EnumerationData EnumerationData;
    MUSB_HostClient *pHostClient;
    MUSB_pfDriverTimerExpired pfDriverTimerExpired;
    void *pDriverTimerData;
    MUSB_Device *pRootDevice;
    MUSB_LinkedList ConnectedDeviceList;
    MGC_Schedule Schedule;
    uint16_t wHostMaxPower;
    uint16_t wHostUsedPower;
#endif	/* host or OTG */

#if defined(MUSB_FUNCTION) || defined(MUSB_OTG)
    MUSB_FunctionClient *pFunctionClient;
    const MUSB_ConfigurationDescriptor **apConfigDescriptors;
    const MUSB_ConfigurationDescriptor *pCurrentConfig;
    MGC_Pipe **apPipe;
    uint8_t bUsbState;
    uint8_t bMaxFunctionEnds;
    uint8_t bCanWakeup;
    uint8_t bSetAddress;
    uint8_t bConfigValue;
    uint8_t bParse;
    uint16_t wSetupDataIndex;
    uint16_t wSetupRxIndex;
    uint16_t wSetupDataSize;
    uint8_t *pSetupData;
    uint32_t dwSequenceNumber;
    uint8_t abIfaceOffsets[MGC_MAX_CONFIG_IFACES];
#endif	/* function or OTG */

#ifdef MUSB_OVERHEAD
    MGC_OverheadStats IsrOverhead;
    MUSB_Array OverheadStats;
#endif
#ifdef MUSB_STATS
    MUSB_Array EndpointStats;
    uint32_t dwResetCount;
    uint32_t dwSuspendCount;
    uint32_t dwResumeCount;
    uint32_t dwResponseTimeoutCount;
#endif

#ifdef MUSB_DMA
    MUSB_DmaController *pDmaController;
#endif

    MGC_pfReadBusState pfReadBusState;
    MGC_pfProgramBusState pfProgramBusState;
    MGC_pfResetPort pfResetPort;
    MGC_pfBindEndpoint pfBindEndpoint;
    MGC_pfProgramStartReceive pfProgramStartReceive;
    MGC_pfProgramStartTransmit pfProgramStartTransmit;
    MGC_pfProgramFlushEndpoint pfProgramFlushEndpoint;
    MGC_pfProgramHaltEndpoint pfProgramHaltEndpoint;
    MGC_pfDefaultEndResponse pfDefaultEndResponse;
    MGC_pfServiceDefaultEnd pfServiceDefaultEnd;
    MGC_pfServiceTransmitAvail pfServiceTransmitAvail;
    MGC_pfServiceReceiveReady pfServiceReceiveReady;
    MGC_pfLoadFifo pfLoadFifo;
    MGC_pfUnloadFifo pfUnloadFifo;
    MGC_pfAcceptDevice pfAcceptDevice;
    MUSB_pfDmaChannelStatusChanged pfDmaChannelStatusChanged;
    MGC_pfSetPortTestMode pfSetPortTestMode;
    MGC_pfDynamicFifoLocation pfDynamicFifoLocation;
} MGC_Port;

/*
 * Generic functions
 */

extern void MGC_ChangeUsbState(uint8_t bNewState, MGC_Port *pPort);

/**
 * Find descriptor.
 * Find a descriptor of a given type
 *
 * @param pBuffer buffer in which to search
 * @param wBufferLength size, in bytes, to assume for buffer
 * @param bDescriptorType the type of descriptor desired
 * @param bIndex the index (counting from 0) of the instance of the desired descriptor
 * (e.g. if there are 3 HID descriptors, bIndex==2 retrieves the third)
 *
 * @return descriptor pointer if found; NULL on failure
 */
extern const uint8_t *MGC_FindDescriptor(const uint8_t *pBuffer,
        uint16_t wBufferLength,
        uint8_t bDescriptorType,
        uint8_t bIndex);

/**
 * Allocate a dynamic FIFO suitable for the given conditions.
 * @param pPort port pointer
 * @param pUsbEnd USB remote endpoint (host mode) or local endpoint (device mode)
 * @param pRequest pointer to requested endpoint attributes
 * @param bBind TRUE to actually allocate; FALSE to check for available space only
 * @param pbIsTx pointer to storage for whether the implied direction is transmit
 * @param bEndCount the number of endpoints in the core
 * @param dwTotalSize total RAM size, in bytes
 * @param dwHighWater the high-water mark (the initial allocation strategy
 * tries to use double-buffering; once allocation reaches this mark,
 * the allocation strategy becomes more conservative)
 */
extern MGC_EndpointResource *MGC_AllocateDynamicFifo(MGC_Port *pPort,
        const MUSB_DeviceEndpoint *pUsbEnd,
        MUSB_EndpointResource *pRequest,
        uint8_t bBind,
        uint8_t *pbIsTx,
        uint8_t bEndCount,
        uint32_t dwTotalSize,
        uint32_t dwHighWater);

#endif	/* multiple inclusion protection */

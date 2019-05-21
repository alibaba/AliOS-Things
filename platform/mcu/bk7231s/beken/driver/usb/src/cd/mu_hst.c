/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2006              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/

/*
 * MUSBMicroSW host-specific functionality.
 * $Revision: 1.50 $
 */
#include "mu_mem.h"
#include "mu_impl.h"
#include "mu_diag.h"
#include "mu_cntlr.h"


/**
 * Undefine this to avoid embedded HSET overheads
 * (but this would preclude USB-IF high-speed OTG certification)
 */
#define MUSB_EHSET

#if defined(MUSB_HOST) || defined(MUSB_OTG)

/** For now, always define */
#define MUSB_ENUMERATOR

/*************************** CONSTANTS ****************************/
#define MGC_FRAME_OFFSET                (1)
#define MGC_MAX_POLLING_INTERVAL        (255)
#define MGC_DRC_DELAY			         1000L	/* just a guess! */

/** How many times to retry enumeration transfers */
#define MGC_MAX_ENUM_RETRIES		    10

/** How many times to retry enumeration of the root device on fatal errors */
#define MGC_MAX_FATAL_RETRIES		    3

/*
 * Enumeration state
 */
enum MGC_EnumerationState
{
    MGC_EnumStateIdle,
    MGC_EnumStateGetDeviceZeroAddress,
    MGC_EnumStateSetAddress,
    MGC_EnumStateGetMinDevice,
    MGC_EnumStateGetFullDevice,
    MGC_EnumStateSizeConfigs,
    MGC_EnumStateGetConfigs,
    MGC_EnumStateGetDeviceStatus
};

/***************************** TYPES ******************************/

/*
 * Bus timing info (times represent nanoseconds)
 */
typedef struct
{
    uint32_t dwFrameTotal;	/* total frame time */
    uint32_t dwControllerSetup;	/* controller overhead */
    uint32_t dwInOverhead;	/* non-ISO IN overhead */
    uint32_t dwOutOverhead;	/* non-ISO OUT overhead */
    uint32_t dwIsoInOverhead;	/* ISO IN overhead */
    uint32_t dwIsoOutOverhead;	/* ISO OUT overhead */
    uint32_t dwPayloadScale;	/* scaling factor for payload-dependent term, * 10 */
    uint32_t dwMaxPeriodic;	/* maximum allowable periodic allocation */
} MGC_BusTimeInfo;

/*************************** FORWARDS *****************************/
static void MGC_HostSetDeviceAddress(void *pParam);
static void MGC_HostGetShortDescriptor(void *pParam);
static void MGC_HostGetFullDescriptor(void *pParam);
static void MGC_HostGetConfig(void *pParam);
static uint16_t MGC_SkipEntry(const uint8_t *pOperand, uint16_t wInputIndex,
                              uint16_t wLength);
uint8_t MGC_EnumerateDevice(MGC_Port *pPort, MUSB_Device *pHubDevice,
                            uint8_t bAddress, uint8_t bHubPort, uint8_t bSpeed,
                            MUSB_pfHubEnumerationComplete
                            pfHubEnumerationComplete);
MUSB_DeviceDriver *MGC_HostFindDriver(MUSB_HostClient *pClient,
                                      const MUSB_Device *pDevice,
                                      const MUSB_ConfigurationDescriptor *pConfig,
                                      const uint8_t **ppEntry);
static uint32_t MGC_HostEnumerator(void *pParam, MUSB_ControlIrp *pIrp);
uint8_t MGC_AllocateAddress(MGC_EnumerationData *pEnumData);
void MGC_ReleaseAddress(MGC_EnumerationData *pEnumData, uint8_t bAddress);
static void MGC_FreeScheduleContents(MGC_Schedule *pSchedule);
#ifdef MUSB_SCHEDULER
static uint32_t MGC_GetSlotTime(MGC_ScheduleSlot *pSlot);
#endif
static MGC_ScheduleSlot *MGC_FindScheduleSlot(MGC_Schedule *pSchedule,
        MGC_BusTimeInfo *pBusTimeInfo,
        uint16_t wInterval,
        uint16_t wDuration,
        uint32_t dwBusTime);
static uint32_t MGC_ComputeBandwidth(MGC_BusTimeInfo *pBusTimeInfo,
                                     uint8_t bTrafficType, uint8_t bIsIn, uint16_t wMaxPacketSize);
static uint8_t MGC_CommitBandwidth(MGC_Port *pPort, MGC_Pipe *pPipe,
                                   const MUSB_DeviceEndpoint *pRemoteEnd);
static void MGC_DriverTimerExpired(void *pControllerPrivateData);

/**************************** GLOBALS *****************************/

/** Timing of low-speed frames */
static MGC_BusTimeInfo MGC_LowSpeedFrame =
{
    1000000L,
    MGC_DRC_DELAY,
    64060 + 2000,   /* 2000 is 2*Hub_LS_Setup, which we are guessing! */
    64107 + 2000,   /* 2000 is 2*Hub_LS_Setup, which we are guessing! */
    0,
    0,
    6767,
    1000000L * 90 / 100
};

/** Timing of full-speed frames */
static MGC_BusTimeInfo MGC_FullSpeedFrame =
{
    1000000L,
    MGC_DRC_DELAY,
    9107,
    9107,
    7268,
    6265,
    836,
    1000000L * 90 / 100
};

/** Timing of high-speed frames */
static MGC_BusTimeInfo MGC_HighSpeedFrame =
{
    125000L,
    MGC_DRC_DELAY,
    917,
    917,
    634,
    634,
    21,
    125000L * 80 / 100
};

/*
 * Enumerator is really optional since some OSs do this
 */
#ifdef MUSB_ENUMERATOR
/** SET_ADDRESS request template */
const static uint8_t MGC_aSetDeviceAddress[] =
{
    MUSB_DIR_OUT | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE,
    MUSB_REQ_SET_ADDRESS,
    0, 0, /* address */
    0, 0,
    0, 0
};

/** GET_DESCRIPTOR(DEVICE) request template */
const static uint8_t MGC_aGetDeviceDescriptor[] =
{
    MUSB_DIR_IN | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE,
    MUSB_REQ_GET_DESCRIPTOR,
    0, MUSB_DT_DEVICE,
    0, 0,
    0, 0 /* allowed descriptor length */
};

/** GET_DESCRIPTOR(CONFIGURATION) request template */
const static uint8_t MGC_aGetConfigDescriptor[] =
{
    MUSB_DIR_IN | MUSB_TYPE_STANDARD | MUSB_RECIP_DEVICE,
    MUSB_REQ_GET_DESCRIPTOR,
    0, MUSB_DT_CONFIG,
    0, 0,
    0, 0 /* allowed descriptor length */
};


/*
 * Issue a USB set_address from stack's IRP.
 */
static void MGC_HostSetDeviceAddress(void *pParam)
{
    uint32_t ret;
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pEnumData->aSetupTx;

    /* program the address (some DRCs need it even in host mode) */
    pPort->bFuncAddr = 0;
    pPort->pfProgramBusState(pPort);

    MUSB_PRT("[MGC] HostSetDeviceAddress\r\n");
    /* create and start request */
    MUSB_MemCopy(pEnumData->aSetupTx, MGC_aSetDeviceAddress, sizeof(MGC_aSetDeviceAddress));
    pRequest->wValue = MUSB_SWAP16(pEnumData->bAddress);//此处已经把setup数据包中的地址设置为1了
    pEnumData->Irp.pOutBuffer = pEnumData->aSetupTx;
    pEnumData->Irp.dwOutLength = sizeof(MGC_aSetDeviceAddress);
    pEnumData->bState = (uint8_t)MGC_EnumStateSetAddress;

    ret = MUSB_StartControlTransfer(pPort->pInterfacePort, &(pEnumData->Irp));
    if(ret)
    {
        MUSB_PRT("[MGC] HostSetDeviceAddress_failed\r\n");
    }
}

/*
 * Get 1st 8 bytes of EP0 on the peripheral.
 * Do this so we can throttle our reads if the
 * peripheral only has an 8-byte EP0 Max EP Size.
 */
void MGC_HostGetShortDescriptorZeroAddress(void *pParam)
{
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pEnumData->aSetupTx;

    MUSB_PRT("[MGC] HostGetShortDescriptorZeroAddress\r\n");

    MUSB_MemCopy(pEnumData->aSetupTx, MGC_aGetDeviceDescriptor,
                 sizeof(MGC_aGetDeviceDescriptor));
    pRequest->wLength = MUSB_SWAP16(0x12);

    pEnumData->Irp.pOutBuffer = pEnumData->aSetupTx;
    pEnumData->Irp.dwOutLength = sizeof(MGC_aGetDeviceDescriptor);
    pEnumData->Irp.pInBuffer = pEnumData->aSetupRx;
    pEnumData->Irp.dwInLength = 0x12;
    pEnumData->bState = (uint8_t)MGC_EnumStateGetDeviceZeroAddress;

    MUSB_StartControlTransfer(pPort->pInterfacePort, &(pEnumData->Irp));
}

static void MGC_HostGetShortDescriptor(void *pParam)
{
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pEnumData->aSetupTx;

    MUSB_PRT("[MGC] HostGetShortDescriptor\r\n");

    MUSB_MemCopy(pEnumData->aSetupTx, MGC_aGetDeviceDescriptor,
                 sizeof(MGC_aGetDeviceDescriptor));
    pRequest->wLength = MUSB_SWAP16(0x12);

    pEnumData->Irp.pOutBuffer = pEnumData->aSetupTx;
    pEnumData->Irp.dwOutLength = sizeof(MGC_aGetDeviceDescriptor);
    pEnumData->Irp.pInBuffer = pEnumData->aSetupRx;
    pEnumData->Irp.dwInLength = 0x12;
    pEnumData->bState = (uint8_t)MGC_EnumStateGetMinDevice;

    MUSB_StartControlTransfer(pPort->pInterfacePort, &(pEnumData->Irp));
}

/*
 * Get full device descriptor on the peripheral.
 */
static void MGC_HostGetFullDescriptor(void *pParam)
{
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pEnumData->aSetupTx;

    MUSB_PRT("[MGC] HostGetFullDescriptor\r\n");

    MUSB_MemCopy(pEnumData->aSetupTx, MGC_aGetDeviceDescriptor,
                 sizeof(MGC_aGetDeviceDescriptor));
    pRequest->wLength = MUSB_SWAP16(sizeof(MUSB_DeviceDescriptor));
    pEnumData->Irp.pOutBuffer = pEnumData->aSetupTx;
    pEnumData->Irp.dwOutLength = sizeof(MGC_aGetDeviceDescriptor);
    pEnumData->Irp.pInBuffer = pEnumData->aSetupRx;
    pEnumData->Irp.dwInLength = sizeof(MUSB_DeviceDescriptor);
    pEnumData->bState = (uint8_t)MGC_EnumStateGetFullDevice;
    MUSB_StartControlTransfer(pPort->pInterfacePort, &(pEnumData->Irp));
}

/*
 * Get a config descriptor
 */
static void MGC_HostGetConfig(void *pParam)
{
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pEnumData->aSetupTx;

    MUSB_PRT("[MGC] HostGetConfig\r\n");

    MUSB_MemCopy(pEnumData->aSetupTx, MGC_aGetConfigDescriptor,
                 sizeof(MGC_aGetConfigDescriptor));
    pEnumData->aSetupTx[2] = pEnumData->bIndex;
    pRequest->wLength = MUSB_SWAP16((uint16_t)pEnumData->Irp.dwInLength);
    pEnumData->Irp.pOutBuffer = pEnumData->aSetupTx;
    pEnumData->Irp.dwOutLength = sizeof(MGC_aGetConfigDescriptor);

    MUSB_StartControlTransfer(pPort->pInterfacePort, &(pEnumData->Irp));
}

#ifdef MUSB_EHSET
static void MGC_EhsetResume(void *pParam)
{
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;

    pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_RESUME);
}

static void MGC_EhsetGetDescriptorSetup(void *pParam)
{
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pEnumData->aSetupTx;

    MGC_DIAG(2, pPort->pController, "Getting complete device descriptor");

    MUSB_MemCopy(pEnumData->aSetupTx, MGC_aGetDeviceDescriptor,
                 sizeof(MGC_aGetDeviceDescriptor));
    pRequest->wLength = MUSB_SWAP16(sizeof(MUSB_DeviceDescriptor));

    pPort->pfLoadFifo(pPort, 0, sizeof(MGC_aGetDeviceDescriptor),
                      MGC_aGetDeviceDescriptor);
    pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_SETUP_START);
}

static void MGC_EhsetGetDescriptorIn(void *pParam)
{
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;

    pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_SETUP_IN);
}

static void MGC_Ehset(MGC_Port *pPort, uint8_t bBusAddress, uint16_t wPid)
{
    MGC_Controller *pController = pPort->pController;

    switch(wPid)
    {
    case 0x0101:
        /* SE0_NAK */
        pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_TEST_SE0_NAK);
        break;

    case 0x0102:
        /* J */
        pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_TEST_J);
        break;

    case 0x0103:
        /* K */
        pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_TEST_K);
        break;

    case 0x0104:
        /* PACKET */
        pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_TEST_PACKET);
        break;

    case 0x0105:
        /* FORCE_ENABLE */
        break;

    case 0x0106:
        /* suspend, wait 20 secs, resume */
        pPort->pfSetPortTestMode(pPort, MUSB_HSET_PORT_SUSPEND);
        pController->pSystemServices->pfArmTimer(
            pController->pSystemServices->pPrivateData, 0, 2000, FALSE,
            MGC_EhsetResume);
        break;

    case 0x0107:
        /* wait 15 secs, perform setup phase of GET_DESC */
        pController->pSystemServices->pfArmTimer(
            pController->pSystemServices->pPrivateData, 0, 1500, FALSE,
            MGC_EhsetGetDescriptorSetup);
        break;

    case 0x0108:
        /* perform setup phase of GET_DESC, wait 15 secs, perform IN data phase */
        MGC_EhsetGetDescriptorSetup(pController);
        pController->pSystemServices->pfArmTimer(pController->pSystemServices->pPrivateData,
                0,
                1500,
                FALSE,
                MGC_EhsetGetDescriptorIn);
        break;
    }
}
#endif

/**
 * Skip an entry in the peripheral list, by searching for the next ACCEPT or REJECT
 * @param pOperand list
 * @param wInputIndex current index
 * @return the index in the list for the ACCEPT or REJECT, or one beyond the list
 * if the list is missing this (malformed)
 */
static uint16_t MGC_SkipEntry(const uint8_t *pOperand, uint16_t wInputIndex,
                              uint16_t wLength)
{
    uint16_t wIndex = wInputIndex;

    while(wIndex < wLength)
    {
        switch(pOperand[wIndex])
        {
        case MUSB_TARGET_UNKNOWN:
        case MUSB_TARGET_CONFIG:
        case MUSB_TARGET_INTERFACE:
        case MUSB_TARGET_CLASS:
        case MUSB_TARGET_SUBCLASS:
        case MUSB_TARGET_PROTOCOL:
            wIndex += 2;
            break;

        case MUSB_TARGET_VID:
        case MUSB_TARGET_PID:
        case MUSB_TARGET_DEVICE_BCD:
            wIndex += 3;
            break;

        case MUSB_TARGET_ACCEPT:
            wIndex += 2;
            return wIndex;

        case MUSB_TARGET_REJECT:
            wIndex++;
            return wIndex;
        }
    }
    return wIndex;
}

MUSB_DeviceDriver *MGC_HostFindDriver(MUSB_HostClient *pClient,
                                      const MUSB_Device *pDevice,
                                      const MUSB_ConfigurationDescriptor *pConfig,
                                      const uint8_t **ppEntry)
{
    uint8_t bData;
    uint16_t wData;
    uint8_t bInterface = FALSE;
    uint16_t wIndex = 0;
    uint16_t wEntryIndex = 0;
    const MUSB_InterfaceDescriptor *pInterface = NULL;
    uint16_t wConfigLength = MUSB_SWAP16P((uint8_t *) & (pConfig->wTotalLength));
    uint16_t wLength = pClient->wPeripheralListLength;
    const uint8_t *pOperand = pClient->pPeripheralList;

    MUSB_PRT("[MGC] HostFindDriver\r\n");
    while(wIndex < wLength)
    {
        switch(pOperand[wIndex])
        {
        case MUSB_TARGET_UNKNOWN:
            wIndex++;
            break;

        case MUSB_TARGET_VID:
            wData = pOperand[++wIndex];
            wData |= (pOperand[++wIndex] << 8);
            wIndex++;
            if(wData != MUSB_SWAP16P((uint8_t *) & (pDevice->DeviceDescriptor.idVendor)))
            {
                /* Reset and continue on to next list */
                bInterface = FALSE;
                wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                wEntryIndex = wIndex;
            }
            break;

        case MUSB_TARGET_PID:
            wData = pOperand[++wIndex];
            wData |= (pOperand[++wIndex] << 8);
            wIndex++;
            if(wData != MUSB_SWAP16P((uint8_t *) & (pDevice->DeviceDescriptor.idProduct)))
            {
                /* Reset and continue on to next list */
                bInterface = FALSE;
                wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                wEntryIndex = wIndex;
            }
            break;

        case MUSB_TARGET_DEVICE_BCD:
            wData = pOperand[++wIndex];
            wData |= (pOperand[++wIndex] << 8);
            wIndex++;
            if(wData != MUSB_SWAP16P((uint8_t *) & (pDevice->DeviceDescriptor.bcdDevice)))
            {
                /* Reset and continue on to next list */
                bInterface = FALSE;
                wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                wEntryIndex = wIndex;
            }
            break;

        case MUSB_TARGET_CONFIG:
            bData = pOperand[++wIndex];
            wIndex++;
            bInterface = FALSE;
            if(bData < pDevice->DeviceDescriptor.bNumConfigurations)
            {
                pConfig = pDevice->apConfigDescriptors[bData];
                wConfigLength = MUSB_SWAP16P((uint8_t *) & (pConfig->wTotalLength));
            }
            else
            {
                /* Reset and continue on to next list */
                bInterface = FALSE;
                wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                wEntryIndex = wIndex;
            }
            break;

        case MUSB_TARGET_INTERFACE:
            bData = pOperand[++wIndex];
            wIndex++;
            bInterface = TRUE;
            pInterface = (MUSB_InterfaceDescriptor *)MGC_FindDescriptor(
                             (uint8_t *)pConfig, wConfigLength, MUSB_DT_INTERFACE, bData);
            if(!pInterface)
            {
                /* Reset and continue on to next list */
                bInterface = FALSE;
                wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                wEntryIndex = wIndex;
            }
            break;

        case MUSB_TARGET_CLASS:
            bData = pOperand[++wIndex];
            wIndex++;
            if(bInterface)
            {
                if (bData != pInterface->bInterfaceClass)
                {
                    /* Reset and continue on to next list */
                    bInterface = FALSE;
                    wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                    wEntryIndex = wIndex;
                }
            }
            else
            {
                if (bData != pDevice->DeviceDescriptor.bDeviceClass)
                {
                    /* Reset and continue on to next list */
                    bInterface = FALSE;
                    wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                    wEntryIndex = wIndex;
                }
            }
            break;

        case MUSB_TARGET_SUBCLASS:
            bData = pOperand[++wIndex];
            wIndex++;
            if(bInterface)
            {
                if (bData != pInterface->bInterfaceSubClass)
                {
                    /* Reset and continue on to next list */
                    bInterface = FALSE;
                    wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                    wEntryIndex = wIndex;
                }
            }
            else
            {
                if (bData != pDevice->DeviceDescriptor.bDeviceSubClass)
                {
                    /* Reset and continue on to next list */
                    bInterface = FALSE;
                    wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                    wEntryIndex = wIndex;
                }
            }
            break;

        case MUSB_TARGET_PROTOCOL:
            bData = pOperand[++wIndex];
            wIndex++;
            if(bInterface)
            {
                if (bData != pInterface->bInterfaceProtocol)
                {
                    /* Reset and continue on to next list */
                    bInterface = FALSE;
                    wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                    wEntryIndex = wIndex;
                }
            }
            else
            {
                if (bData != pDevice->DeviceDescriptor.bDeviceProtocol)
                {
                    /* Reset and continue on to next list */
                    bInterface = FALSE;
                    wIndex = MGC_SkipEntry(pOperand, wIndex, wLength);
                    wEntryIndex = wIndex;
                }
            }
            break;

        case MUSB_TARGET_ACCEPT:
            /* At this point, all the criteria have been met with success and so
               this list is a match. */
            bData = pOperand[++wIndex];
            if(bData < pClient->bDeviceDriverListLength)
            {
                *ppEntry = &(pOperand[wEntryIndex]);
                return &(pClient->aDeviceDriverList[bData]);
            }
            return NULL;

        case MUSB_TARGET_REJECT:
            return NULL;
        }
    }

    return NULL;
}

/*
 * Callback used during enumeration process
 */
static uint32_t MGC_HostEnumerator(void *pParam, MUSB_ControlIrp *pIrp)
{
    uint32_t Ret = 1;
    MGC_Device *pDevice;
    MUSB_DeviceDriver *pDriver = NULL;
    MGC_EndpointResource *pEnd;
    MUSB_DeviceDescriptor *pDeviceDesc;
    MUSB_ConfigurationDescriptor *pConfigDesc;
    MUSB_ConfigurationDescriptor *pConfig;
    uint8_t *pDescriptorBuffer;
    uint8_t *pEntry;
    uint8_t bPower, bMaxPower, bIndex, bCount, bConfig;
    uint16_t wValue, wIndex, wLength;
    MGC_Controller *pController = (MGC_Controller *)pParam;
    MGC_Port *pPort = pController->pPort;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);
    MUSB_SystemServices *pServices = pController->pSystemServices;

    /* NOTE: assumes EP0 is first */
    pEnd = MUSB_ArrayFetch(&(pPort->LocalEnds), 0);

    if(!pEnd)
    {
        MGC_DIAG(1, pController, "Internal error during enumeration");
        return Ret;
    }

    if(pEnumData->Irp.dwStatus)
    {
        MUSB_PRT("error :irp.dwstatus=%x\r\n", pEnumData->Irp.dwStatus);

        wValue = pEnumData->Irp.pOutBuffer[2] | (pEnumData->Irp.pOutBuffer[3] << 8);
        wIndex = pEnumData->Irp.pOutBuffer[4] | (pEnumData->Irp.pOutBuffer[5] << 8);
        wLength = pEnumData->Irp.pOutBuffer[6] | (pEnumData->Irp.pOutBuffer[7] << 8);

        if(++pEnumData->bRetries > MGC_MAX_ENUM_RETRIES)
        {
            MUSB_PRT("error retry\r\n");
            pEnumData->bState = MGC_EnumStateIdle;
            MGC_ReleaseAddress(pEnumData, pEnumData->pDevice->bBusAddress);
            if(pEnumData->pDevice->apConfigDescriptors)
            {
                MUSB_MemFree(pEnumData->pDevice->apConfigDescriptors);
                pEnumData->pDevice->apConfigDescriptors = NULL;
            }
            if(pEnumData->pDevice->pDescriptorBuffer)
            {
                MUSB_MemFree(pEnumData->pDevice->pDescriptorBuffer);
                pEnumData->pDevice->pDescriptorBuffer = NULL;
            }
            MUSB_MemFree(pEnumData->pDevice->pPrivateData);
            pEnumData->pDevice->pPrivateData = NULL;

            return Ret;
        }
        else
        {
            Ret = 0;
            MGC_DIAG(1, pController, "Retrying transfer");
            MUSB_StartControlTransfer((MUSB_Port *)pPort->pInterfacePort, &(pEnumData->Irp));
            return Ret;
        }
    }
    pEnumData->bRetries = 0;
    pEnumData->bFatalRetries = 0;

    /* Decide what to do next based on what we just did */
    switch(pEnumData->bState)
    {
    case MGC_EnumStateGetDeviceZeroAddress:
        pServices->pfArmTimer(pServices->pPrivateData, 0, 2, FALSE,
                              MGC_HostSetDeviceAddress);
        break;

    case MGC_EnumStateSetAddress:
        pEnumData->pDevice->bBusAddress = pEnumData->bAddress;
        if(!pPort->bIsMultipoint)
        {
            /* program the address (some DRCs need it even in host mode) */
            pPort->bFuncAddr = pEnumData->bAddress;
            pPort->pfProgramBusState(pPort);
        }
        /* get first 18 bytes of device descriptor after 10ms recovery */
        pServices->pfArmTimer(pServices->pPrivateData, 0, 2, FALSE,
                              MGC_HostGetShortDescriptor);
        break;

    case MGC_EnumStateGetMinDevice:
        /* lock EP0 */
        pServices->pfLock(pServices->pPrivateData, 1);

        /* set new max packet size */
        pDeviceDesc = (MUSB_DeviceDescriptor *)pEnumData->aSetupRx;
        pEnd->wPacketSize = pDeviceDesc->bMaxPacketSize0;

        /* unlock EP0 */
        pServices->pfUnlock(pServices->pPrivateData, 1);

#ifdef MUSB_EHSET
        wIndex = MUSB_SWAP16P(&(pDeviceDesc->idVendor));
        wValue = MUSB_SWAP16P(&(pDeviceDesc->idProduct));
        if((6666 == wIndex) && ((wValue >= 0x0101) && (wValue <= 0x0108)))
        {
            MGC_Ehset(pPort, pEnumData->pDevice->bBusAddress, wValue);
            return 0;//???????
        }
#endif

        /* get full descriptor, now that we know the peripheral's packet size */
        pServices->pfArmTimer(pServices->pPrivateData, 0, 2, FALSE,
                              MGC_HostGetFullDescriptor);
        break;

    case MGC_EnumStateGetFullDevice:
        pDeviceDesc = (MUSB_DeviceDescriptor *)pEnumData->aSetupRx;

        MGC_DIAG1(2, pController, "Got device descriptor; config count=",
                  pDeviceDesc->bNumConfigurations, 16, 2);

        pEnumData->bCount = pDeviceDesc->bNumConfigurations;
        pEnumData->bIndex = 0;
        pEnumData->dwData = 0L;
        MUSB_MemCopy((void *) & (pEnumData->pDevice->DeviceDescriptor),
                     (void *)pDeviceDesc, sizeof(MUSB_DeviceDescriptor));

        /* allocate convenience pointers (we temporarily use them to remember sizes) */
        pEnumData->pDevice->apConfigDescriptors = (MUSB_ConfigurationDescriptor **)MUSB_MemAlloc(
                    pDeviceDesc->bNumConfigurations * sizeof(uint8_t *));

        if(pEnumData->pDevice->apConfigDescriptors)
        {
            /* get min config descriptors to gather full sizes */
            pEnumData->bState = MGC_EnumStateSizeConfigs;
            pEnumData->Irp.pInBuffer = pEnumData->aSetupRx;
            pEnumData->Irp.dwInLength = sizeof(MUSB_ConfigurationDescriptor);
            pServices->pfArmTimer(pServices->pPrivateData, 0, 2, FALSE,
                                  MGC_HostGetConfig);
            break;
        }
        MUSB_MemFree(pEnumData->pDevice->pPrivateData);
        pEnumData->pDevice = NULL;
        pEnumData->bState = MGC_EnumStateIdle;
        MGC_DIAG(1, pController, "Insufficient memory for new device");
        break;

    case MGC_EnumStateSizeConfigs:
        /* collect size */
        pConfigDesc = (MUSB_ConfigurationDescriptor *)pEnumData->Irp.pInBuffer;
        wLength = MUSB_SWAP16P((uint8_t *) & (pConfigDesc->wTotalLength));
        pEnumData->dwData += wLength;

        MGC_DIAG2(2, pController, "Config ", pEnumData->bIndex, " length=",
                  wLength, 16, 0);
        /* size collected; allocate buffer for all configs */
        MGC_DIAG1(2, pController, "Total config length=",
                  pEnumData->dwData, 16, 0);

        pEnumData->bIndex = 0;
        pEnumData->bState = MGC_EnumStateGetConfigs;

        pDescriptorBuffer = (uint8_t *)MUSB_MemAlloc(pEnumData->dwData);
        if(!pDescriptorBuffer)
        {
            MGC_DIAG(1, pController, "Insufficient memory for new device");
            MUSB_MemFree(pEnumData->pDevice->pPrivateData);
            pEnumData->pDevice->pPrivateData = NULL;
            break;
        }
        //获得完整的configuration descriptor
        /* start gathering full config info */
        pEnumData->pDevice->pDescriptorBuffer = pDescriptorBuffer;
        pEnumData->pDevice->wDescriptorBufferLength = (uint16_t)pEnumData->dwData;
        pEnumData->pDevice->apConfigDescriptors[0] =
            (MUSB_ConfigurationDescriptor *)pDescriptorBuffer;
        pEnumData->Irp.pInBuffer = pEnumData->pDevice->pDescriptorBuffer;
        pEnumData->Irp.dwInLength = wLength;
        pServices->pfArmTimer(pServices->pPrivateData, 0, 2, FALSE,
                              MGC_HostGetConfig);
        break;

    case MGC_EnumStateGetConfigs:
        /* replace saved lengths with actual pointers as we go */
        if(pEnumData->bIndex)
        {
            pConfigDesc = pEnumData->pDevice->apConfigDescriptors[pEnumData->bIndex - 1];
            wLength = MUSB_SWAP16P((uint8_t *) & (pConfigDesc->wTotalLength));
            pEnumData->pDevice->apConfigDescriptors[pEnumData->bIndex] =
                (MUSB_ConfigurationDescriptor *)
                ((uint8_t *)pEnumData->pDevice->apConfigDescriptors[pEnumData->bIndex - 1] + wLength);
        }

        if(++pEnumData->bIndex >= pEnumData->bCount)
        {
            bMaxPower = 0;
            bCount = pEnumData->pDevice->DeviceDescriptor.bNumConfigurations;
            for(bIndex = 0; bIndex < bCount; bIndex++)
            {
                bPower = pEnumData->pDevice->apConfigDescriptors[bIndex]->bMaxPower;
                if(bPower > bMaxPower)
                {
                    bMaxPower = bPower;
                }
            }

            /* device is ready for a driver */
            for(bConfig = 0;
                    bConfig < pEnumData->pDevice->DeviceDescriptor.bNumConfigurations;
                    bConfig++)
            {
                pConfig = pEnumData->pDevice->apConfigDescriptors[bConfig];
                pDriver = MGC_HostFindDriver(pPort->pHostClient,
                                             pEnumData->pDevice,
                                             pConfig,
                                             (const uint8_t **) &pEntry);
                if(pDriver)
                {
                    pEnumData->pDevice->pCurrentConfiguration = pConfig;
                    break;
                }
            }
            /* dump basic device info */
            if(pDriver)
            {
                MUSB_PRT("Found driver for device revision = %x\r\n", pEnumData->pDevice->DeviceDescriptor.bcdDevice);
            }
            else
            {
                MUSB_PRT("No driver for device revision = %x\r\n", pEnumData->pDevice->DeviceDescriptor.bcdDevice);
            }

            /* perform controller-specific device acceptance */
            if (pPort->pfAcceptDevice)
            {
                if(pPort->pfAcceptDevice(pPort, pEnumData->pDevice, pDriver))
                {
                    MGC_BsrItem qItem;

                    pDevice = (MGC_Device *)pEnumData->pDevice->pPrivateData;
                    pDevice->pDriver = pDriver;
                    MUSB_ListAppendItem(&(pPort->ConnectedDeviceList),
                                        pEnumData->pDevice, 0);
                    if(!pPort->pRootDevice)
                    {
                        pPort->pRootDevice = pEnumData->pDevice;
                    }
                    if(pEnumData->pfEnumerationComplete)
                    {
                        pEnumData->pfEnumerationComplete(pEnumData->pDevice->pParentUsbDevice,
                                                         pEnumData->pDevice);
                        pEnumData->pfEnumerationComplete = NULL;
                    }

                    /*MUSB_MsdConnect()*/
                    MUSB_PRT("[MUSB] usb_pfDeviceConnected\r\n");
                    pDriver->pfDeviceConnected(pDriver->pPrivateData, pPort,
                                               pEnumData->pDevice, pEntry);

                    qItem.bCause = MGC_BSR_CAUSE_ENUMERATION_OK;
                    pServices->pfQueueBackgroundItem(pServices->pPrivateData, &qItem);
                }
                else
                {
                    MGC_ReleaseAddress(pEnumData, pEnumData->pDevice->bBusAddress);
                    if(pEnumData->pDevice->apConfigDescriptors)
                    {
                        MUSB_MemFree(pEnumData->pDevice->apConfigDescriptors);
                    }

                    if(pEnumData->pDevice->pDescriptorBuffer)
                    {
                        MUSB_MemFree(pEnumData->pDevice->pDescriptorBuffer);
                    }

                    MUSB_MemFree(pEnumData->pDevice->pPrivateData);
                }
            }

            pEnumData->bState = MGC_EnumStateIdle;
            break;
        }

        /* continue gathering */
        wLength = *((uint16_t *) & (pEnumData->pDevice->apConfigDescriptors[pEnumData->bIndex]));
        pEnumData->Irp.pInBuffer += pEnumData->Irp.dwInLength;
        pEnumData->Irp.dwInLength = wLength;
        pServices->pfArmTimer(pServices->pPrivateData, 0, 2, FALSE,
                              MGC_HostGetConfig);
        break;

    case MGC_EnumStateGetDeviceStatus:
    default:
        break;
    }
    return 0;//Ret;
}

uint8_t MGC_AllocateAddress(MGC_EnumerationData *pEnumData)
{
    uint8_t bIndex, bBit;
    uint8_t bmAddress;
    uint8_t bAddress = 0;

    for(bIndex = 0; bIndex < 16; bIndex++)
    {
        bmAddress = pEnumData->abmAddress[bIndex];
        if(bmAddress < 0xff)
        {
            break;
        }
    }
    if(bmAddress < 0xff)
    {
        bmAddress = ~bmAddress;
        for(bBit = 0; bBit < 8; bBit++)
        {
            if(bmAddress & 1)
            {
                break;
            }
            bmAddress >>= 1;
        }
        bAddress = (bIndex << 3) | bBit;
        pEnumData->abmAddress[bIndex] |= (1 << bBit);
    }
    if(++bAddress > 127)
    {
        bAddress = 0;
    }
    if((1 == bAddress) && bIndex)
    {
        bAddress = 0;
    }

    return bAddress;
}

void MGC_ReleaseAddress(MGC_EnumerationData *pEnumData, uint8_t bAddress)
{
    uint8_t bIndex = (bAddress - 1) >> 3;
    uint8_t bBit = (bAddress - 1) & 0x7;

    if(bIndex < 16)
    {
        pEnumData->abmAddress[bIndex] &= ~(1 << bBit);
    }
}
/*对于我们的应用:
 pHubDevice和pfHubEnumerationComplete都为NULL
 bHubPort=0
*/
uint8_t MGC_EnumerateDevice(MGC_Port *pPort, MUSB_Device *pHubDevice,
                            uint8_t bAddress, uint8_t bHubPort, uint8_t bSpeed,
                            MUSB_pfHubEnumerationComplete pfHubEnumerationComplete)
{
    MGC_Device *pDevice;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;
    MGC_EnumerationData *pEnumData = &(pPort->EnumerationData);

    /* allocate new device struct */
    pDevice = (MGC_Device *)MUSB_MemAlloc(sizeof(MGC_Device));
    if(pDevice)
    {
        MUSB_MemSet(pDevice, 0, sizeof(MGC_Device));
        MUSB_ListInit(&(pDevice->PipeList));
        pDevice->Device.pPrivateData = pDevice;
        pEnumData->dwData = 0;
        pEnumData->bAddress = bAddress;
        pEnumData->pDevice = &(pDevice->Device);
        pEnumData->pDevice->pPort = pPort->pInterfacePort;
        pEnumData->pDevice->bHubPort = bHubPort;
        pEnumData->pDevice->ConnectionSpeed = (MUSB_ConnectionSpeed)bSpeed;
        pEnumData->pDevice->pParentUsbDevice = pHubDevice;

#ifdef MUSB_HUB
        if(pHubDevice)
        {
            pDevice->bIsMultiTt = (2 == pEnumData->pDevice->pParentUsbDevice->DeviceDescriptor.bDeviceProtocol);
        }
#endif

        pEnumData->pfEnumerationComplete = pfHubEnumerationComplete;
        pEnumData->Irp.pDevice        = pEnumData->pDevice;
        pEnumData->Irp.pfIrpComplete  = MGC_HostEnumerator;
        pEnumData->Irp.pCompleteParam = pPort->pController;
        pEnumData->Irp.pInBuffer      = pEnumData->aSetupRx;
        pEnumData->Irp.dwInLength     = sizeof(pEnumData->aSetupRx);
        pEnumData->bRetries           = 0;
        pEnumData->bFatalRetries      = 0;

        pServices->pfArmTimer(pServices->pPrivateData,
                              0, 40, FALSE, MGC_HostGetShortDescriptorZeroAddress);
        return TRUE;
    }
    return FALSE;
}

uint8_t MGC_HostDestroy(MGC_Port *pPort)
{
    MUSB_Device *pDevice;
    uint16_t wIndex, wCount;
    MGC_EnumerationData *pEnumData;

    wCount = MUSB_ListLength(&(pPort->ConnectedDeviceList));
    MUSB_PRT("wcount = %x \r\n", wCount);
    pEnumData = &(pPort->EnumerationData);

    if(wCount == 0)
    {
        if(pEnumData != NULL)
        {
            pEnumData->bState = MGC_EnumStateIdle;
            if(NULL == pEnumData->pDevice)
                return TRUE;
            MGC_ReleaseAddress(pEnumData, pEnumData->pDevice->bBusAddress);
            if(pEnumData->pDevice->apConfigDescriptors)
            {
                MUSB_MemFree(pEnumData->pDevice->apConfigDescriptors);
                pEnumData->pDevice->apConfigDescriptors = NULL;
            }
            if(pEnumData->pDevice->pDescriptorBuffer)
            {
                MUSB_MemFree(pEnumData->pDevice->pDescriptorBuffer);
                pEnumData->pDevice->pDescriptorBuffer = NULL;
            }
            if(pEnumData->pDevice->pPrivateData)
            {
                MUSB_MemFree(pEnumData->pDevice->pPrivateData);
                pEnumData->pDevice->pPrivateData = NULL;
            }
        }
    }
    else
    {
        for(wIndex = 0; wIndex < wCount; wIndex++)
        {
            pDevice = (MUSB_Device *)MUSB_ListFindItem(&(pPort->ConnectedDeviceList), 0);
            MUSB_DeviceDisconnected(pDevice);
            MUSB_ListRemoveItem(&(pPort->ConnectedDeviceList), pDevice);
        }
        pEnumData->pDevice = NULL;
    }
    return TRUE;
}

#ifdef MUSB_HUB
uint32_t MUSB_EnumerateDevice(MUSB_Device *pHubDevice, uint8_t bHubPort,
                              uint8_t bSpeed, MUSB_pfHubEnumerationComplete pfHubEnumerationComplete)
{
    uint8_t bAddress;
    uint32_t dwStatus = MUSB_STATUS_OK;
    MGC_Port *pImplPort = (MGC_Port *)pHubDevice->pPort->pPrivateData;
    MGC_EnumerationData *pEnumData = &(pImplPort->EnumerationData);
    MUSB_SystemServices *pServices = pImplPort->pController->pSystemServices;

    /* lock */
    pServices->pfLock(pServices->pPrivateData, 0);

    bAddress = MGC_AllocateAddress(pEnumData);

    if(bAddress)
    {
        if(MGC_EnumStateIdle != pEnumData->bState)
        {
            dwStatus = MUSB_STATUS_ENDPOINT_BUSY;
            MGC_ReleaseAddress(pEnumData, bAddress);
        }
        else
        {
            MGC_EnumerateDevice(pImplPort, pHubDevice, bAddress, bHubPort,
                                bSpeed, pfHubEnumerationComplete);
        }
    }
    else
    {
        dwStatus = MUSB_STATUS_NO_MEMORY;
    }

    /* unlock */
    pServices->pfUnlock(pServices->pPrivateData, 0);

    return dwStatus;
}
#endif

void MUSB_RejectDevice(MUSB_BusHandle hBus, MUSB_Device *pDevice)
{
    MGC_Pipe *pPipe;
    uint16_t wCount, wIndex;
    MGC_Port *pImplPort = (MGC_Port *)hBus;
    MGC_Device *pImplDevice = (MGC_Device *)pDevice->pPrivateData;
    MUSB_SystemServices *pServices = pImplPort->pController->pSystemServices;

#ifdef MUSB_HUB
    if(pImplDevice && pImplDevice->pSchedule)
    {
        /* free all schedule slots */
        MGC_FreeScheduleContents(pImplDevice->pSchedule);
        MUSB_MemFree(pImplDevice->pSchedule);
        pImplDevice->pSchedule = NULL;
    }
#endif
    if(pDevice == pImplPort->pRootDevice)
    {
        MGC_FreeScheduleContents(&(pImplPort->Schedule));
    }

    MGC_ReleaseAddress(&(pImplPort->EnumerationData), pDevice->bBusAddress);

    /* lock */
    pServices->pfLock(pServices->pPrivateData, 0);

    if(pImplDevice)
    {
        /* release bandwidth for current config by walking PipeList */
        wCount = MUSB_ListLength(&(pImplDevice->PipeList));
        for(wIndex = 0; wIndex < wCount; wIndex++)
        {
            pPipe = (MGC_Pipe *)MUSB_ListFindItem(&(pImplDevice->PipeList), 0);
            if(pPipe->pSlot)
            {
                MUSB_ListRemoveItem(&(pPipe->pSlot->PipeList), pPipe);
            }
        }
    }

    /* remove from list */
    MUSB_ListRemoveItem(&(pImplPort->ConnectedDeviceList), pDevice);

    /* release memory */
    if(pDevice->apConfigDescriptors)
    {
        MUSB_MemFree((void *)pDevice->apConfigDescriptors);
        pDevice->apConfigDescriptors = NULL;
    }
    if(pDevice->pDescriptorBuffer)
    {
        MUSB_MemFree((void *)pDevice->pDescriptorBuffer);
        pDevice->pDescriptorBuffer = NULL;
    }
    pDevice->pPrivateData = NULL;
    if(pImplDevice)
    {
        MUSB_MemFree(pImplDevice);
    }

    /* unlock */
    pServices->pfUnlock(pServices->pPrivateData, 0);

}

void MUSB_RejectInterfaces(MUSB_BusHandle hBus, MUSB_Device *pDevice,
                           uint8_t *abInterfaceNumber, uint8_t bInterfaceCount)
{
    MGC_Device *pVirtualDevice;
    uint8_t *pEntry;
    MUSB_DeviceDriver *pDriver;
    uint8_t bIndex, bSearchIndex, bNumber;
    const MUSB_InterfaceDescriptor *pInterface;
    uint8_t *pDest;
    uint16_t wLength, wNewTotalLength;
    MGC_Port *pPort = (MGC_Port *)hBus;
    const MUSB_InterfaceDescriptor *pPrevInterface = NULL;
    const MUSB_ConfigurationDescriptor *pConfigDesc =
        pDevice->pCurrentConfiguration ? pDevice->pCurrentConfiguration : pDevice->apConfigDescriptors[0];
    uint16_t wTotalLength = MUSB_SWAP16(pConfigDesc->wTotalLength);

    /* create clone */
    pVirtualDevice = (MGC_Device *)MUSB_MemAlloc(sizeof(MGC_Device));
    if(!pVirtualDevice)
    {
        MUSB_DIAG_STRING(1, "Memory allocation error for virtual device");
        return;
    }
    MUSB_MemSet(pVirtualDevice, 0, sizeof(MGC_Device));
    MUSB_MemCopy(&(pVirtualDevice->Device), pDevice, sizeof(MUSB_Device));
    MUSB_ListInit(&(pVirtualDevice->PipeList));
    pVirtualDevice->Device.pPrivateData = pVirtualDevice;

    /* but make private copy of descriptors (first config only for now) */
    pVirtualDevice->Device.apConfigDescriptors = (MUSB_ConfigurationDescriptor **)MUSB_MemAlloc(
                sizeof(MUSB_ConfigurationDescriptor *));
    if(!pVirtualDevice->Device.apConfigDescriptors)
    {
        MUSB_MemFree(pVirtualDevice);
        MUSB_DIAG_STRING(1, "Memory allocation error for virtual device");
        return;
    }

    /* to avoid an extra loop to accumulate required size, just make it the total size */
    pVirtualDevice->Device.pDescriptorBuffer = (uint8_t *)MUSB_MemAlloc(wTotalLength);
    if(!pVirtualDevice->Device.pDescriptorBuffer)
    {
        MUSB_MemFree(pVirtualDevice->Device.apConfigDescriptors);
        MUSB_MemFree(pVirtualDevice);
        MUSB_DIAG_STRING(1, "Memory allocation error for virtual device");
        return;
    }
    pVirtualDevice->Device.apConfigDescriptors[0] =
        (MUSB_ConfigurationDescriptor *)pVirtualDevice->Device.pDescriptorBuffer;
    pVirtualDevice->Device.pCurrentConfiguration = pVirtualDevice->Device.apConfigDescriptors[0];
    pVirtualDevice->Device.wDescriptorBufferLength = wTotalLength;
    pDest = pVirtualDevice->Device.pDescriptorBuffer;
    MUSB_MemCopy(pDest, pConfigDesc, MUSB_DT_CONFIG_SIZE);
    pDest += MUSB_DT_CONFIG_SIZE;

    /* do the job */
    wNewTotalLength = MUSB_DT_CONFIG_SIZE;
    /* to avoid depending on descriptor analysis library, use the primitive finder */
    for(bIndex = bSearchIndex = 0; bIndex < bInterfaceCount; )
    {
        bNumber = abInterfaceNumber[bIndex];
        pInterface = (MUSB_InterfaceDescriptor *)MGC_FindDescriptor(
                         (const uint8_t *)pConfigDesc, wTotalLength, MUSB_DT_INTERFACE, bSearchIndex);
        /* continue until desired interface is found */
        if(!pInterface)
        {
            /* no such interface; give up */
            break;
        }
        if(pInterface->bInterfaceNumber != bNumber)
        {
            bSearchIndex++;
            continue;
        }

        /* find subsequent non-matching interface */
        pPrevInterface = pInterface;
        while(pInterface && (pInterface->bInterfaceNumber == bNumber))
        {
            bSearchIndex++;
            pInterface = (MUSB_InterfaceDescriptor *)MGC_FindDescriptor(
                             (const uint8_t *)pConfigDesc, wTotalLength, MUSB_DT_INTERFACE, bSearchIndex);
        }
        if(!pInterface)
        {
            /* must be tail case */
            break;
        }

        /* copy all descriptors possibly associated with the interface */
        wLength = (uint16_t)((intptr_t)pInterface - (intptr_t)pPrevInterface);
        MUSB_MemCopy(pDest, pPrevInterface, wLength);
        pDest += wLength;
        wNewTotalLength += wLength;

        /* get ready to find next one */
        pPrevInterface = NULL;
        bIndex++;
    }

    /* handle tail case */
    if(pPrevInterface)
    {
        /* copy all descriptors possibly associated with the interface */
        wLength = wTotalLength - (uint16_t)((intptr_t)pPrevInterface - (intptr_t)pConfigDesc);
        MUSB_MemCopy(pDest, pPrevInterface, wLength);
        wNewTotalLength += wLength;
    }

    /* adjust virtual config's total length */
    pVirtualDevice->Device.pCurrentConfiguration->wTotalLength = MUSB_SWAP16(wNewTotalLength);

    pDriver = MGC_HostFindDriver(pPort->pHostClient,
                                 &(pVirtualDevice->Device), pConfigDesc, (const uint8_t **) &pEntry);
    if(pDriver)
    {
        /* device accepted; add to list */
        pVirtualDevice->pDriver = pDriver;
        MUSB_ListAppendItem(&(pPort->ConnectedDeviceList),
                            &(pVirtualDevice->Device), 0);
        /* inform driver */
        pDriver->pfDeviceConnected(pDriver->pPrivateData, pPort,
                                   &(pVirtualDevice->Device), pEntry);
    }
    else
    {
        MUSB_MemFree(pVirtualDevice->Device.pDescriptorBuffer);
        MUSB_MemFree(pVirtualDevice->Device.apConfigDescriptors);
        MUSB_MemFree(pVirtualDevice);
    }
}

/*
 * A device has been disconnected
 */
void MUSB_DeviceDisconnected(MUSB_Device *pDevice)
{
    MUSB_Port *pPort;
    MGC_Port *pImplPort;
    MGC_Device *pImplDevice;
    MUSB_DeviceDriver *pDriver = NULL;

    if(pDevice)
    {
        pPort = pDevice->pPort;

        pImplPort = (MGC_Port *)pPort->pPrivateData;
        pImplDevice = (MGC_Device *)pDevice->pPrivateData;
        if(pImplDevice)
        {
            pDriver = pImplDevice->pDriver;
        }

        /* callback */
        if(pDriver && pDriver->pfDeviceDisconnected)
        {
            pDriver->pfDeviceDisconnected(pDriver->pPrivateData,
                                          (MUSB_BusHandle)pImplPort, pDevice);
        }

        MUSB_RejectDevice(pImplPort, pDevice);
    }
}

#endif	/* MUSB_ENUMERATOR */

/*
 * Selective suspend
 */
uint32_t MUSB_SetTreeSuspend(MUSB_Device *pHubDevice, uint8_t bHubPort,
                             uint8_t bSuspend)
{
    /* TODO: figure out which driver is the hub driver and call it */
    return MUSB_STATUS_UNSUPPORTED;
}

/*
 * Start the next queued control transfer, if any
 */
uint8_t MGC_StartNextControlTransfer(MGC_Port *pPort)
{
    uint8_t bOk = FALSE;
    MUSB_ControlIrp *pIrp = NULL;
    MGC_EndpointResource *pEnd = NULL;

    /* NOTE: assumes EP0 is first */
    pEnd = MUSB_ArrayFetch(&(pPort->LocalEnds), 0);
    if(pEnd)
    {
        pIrp = (MUSB_ControlIrp *)MUSB_ListFindItem(&(pEnd->TxIrpList), 0);
        if(pIrp)
        {
            bOk = TRUE;
            MUSB_ListRemoveItem(&(pEnd->TxIrpList), pIrp);
            pIrp->dwActualInLength = pIrp->dwActualOutLength = 0;
            pIrp->dwStatus = MUSB_STATUS_OK;
            pEnd->bIsTx = TRUE;
            pPort->pfProgramStartTransmit(pPort, pEnd, pIrp->pOutBuffer, 8, pIrp);
        }
        else
        {
            pEnd->pTxIrp = NULL;
        }
    }

    return bOk;
}

/*
 * Run scheduled transfers.
 * This function needs to be relatively quick,
 * so it assumes the schedule is properly maintained.
 */
uint8_t MGC_RunScheduledTransfers(MGC_Port *pPort)
{
#ifdef MUSB_SCHEDULER
    MUSB_LinkedList *pSlotElement;
    MGC_ScheduleSlot *pSlot;
    MUSB_LinkedList *pPipeElement;
    MGC_Pipe *pPipe;
    MUSB_Irp *pIrp;
    MUSB_IsochIrp *pIsochIrp;
    uint32_t dwFrameBits;
    uint32_t dwEffectiveFrame;
    MGC_EndpointResource *pEnd;
    void *pGenIrp;
    uint8_t bIsTx, bTrafficType;
    uint8_t bAllowDma = FALSE;
    uint8_t *pBuffer = NULL;
    uint32_t dwLength = 0;

    pSlotElement = &(pPort->Schedule.ScheduleSlots);
    while(pSlotElement)
    {
        pSlot = (MGC_ScheduleSlot *)pSlotElement->pItem;
        if(pSlot)
        {
            /* set active if transfer is required */
            dwEffectiveFrame = pPort->dwFrame - pSlot->wFrameOffset;
            /* NOTE: wInterval is guaranteed to be a power of 2 */
            dwFrameBits = pSlot->wInterval | (pSlot->wInterval - 1);
            if(!pSlot->bIsActive && (pSlot->wInterval == (dwEffectiveFrame & dwFrameBits)))
            {
                pSlot->bIsActive = TRUE;
            }
            if(pSlot->bIsActive)
            {
                /* active; prepare slot transfers */
                pPipeElement = &(pSlot->PipeList);
                while(pPipeElement)
                {
                    pPipe = (MGC_Pipe *)pPipeElement->pItem;
                    if(pPipe)
                    {
                        pEnd = pPipe->pLocalEnd;
                        bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;
                        bTrafficType = bIsTx ? pEnd->bTrafficType : pEnd->bRxTrafficType;
                        switch(bTrafficType)
                        {
                        case MUSB_ENDPOINT_XFER_INT:
                            pIrp = bIsTx ? (MUSB_Irp *)pEnd->pTxIrp : (MUSB_Irp *)pEnd->pRxIrp;
                            pGenIrp = pIrp;
                            bAllowDma = pIrp->bAllowDma;
                            pBuffer = pIrp->pBuffer;
                            dwLength = pIrp->dwLength;
                            break;
#ifdef MUSB_ISO
                        case MUSB_ENDPOINT_XFER_ISOC:
                            pIsochIrp = bIsTx ? (MUSB_IsochIrp *)pEnd->pTxIrp : (MUSB_IsochIrp *)pEnd->pRxIrp;
                            pGenIrp = pIsochIrp;
                            bAllowDma = pIsochIrp->bAllowDma;
                            if(pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT)
                            {
                                pBuffer = pIsochIrp->pBuffer + pEnd->dwTxOffset;
                            }
                            else
                            {
                                pBuffer = pIsochIrp->pBuffer + pEnd->dwRxOffset;
                            }
                            dwLength = pIsochIrp->adwLength[pIsochIrp->wCurrentFrame];
                            break;
#endif
                        }
                        if(bIsTx)
                        {
                            pPort->pfProgramStartTransmit(pPort, pEnd, pBuffer, dwLength, pGenIrp);
                        }
                        else
                        {
                            pPort->pfProgramStartReceive(pPort, pEnd, pBuffer, dwLength,
                                                         pGenIrp, bAllowDma);
                        }
                    }
                    pPipeElement = pPipeElement->pNext;
                }
                /* check if last frame */
                if(0 == --pSlot->wFramesRemaining)
                {
                    /* last frame in slot; mark as inactive */
                    pSlot->bIsActive = FALSE;
                    pSlot->wFramesRemaining = pSlot->wDuration;
                }
            }
        }
        pSlotElement = pSlotElement->pNext;
    }
#endif
    return TRUE;
}

/*
 * Free a schedule's contents
 */
static void MGC_FreeScheduleContents(MGC_Schedule *pSchedule)
{
    MGC_ScheduleSlot *pSlot;
    MGC_Pipe *pPipe;
    uint16_t wSlotCount, wPipeCount;
    uint16_t wSlotIndex, wPipeIndex;

    wSlotCount = MUSB_ListLength(&(pSchedule->ScheduleSlots));
    for(wSlotIndex = 0; wSlotIndex < wSlotCount; wSlotIndex++)
    {
        pSlot = (MGC_ScheduleSlot *)MUSB_ListFindItem(&(pSchedule->ScheduleSlots), 0);
        MUSB_ListRemoveItem(&(pSchedule->ScheduleSlots), pSlot);
        /* remove pipes but do not free (close frees them) */
        wPipeCount = MUSB_ListLength(&(pSlot->PipeList));
        for(wPipeIndex = 0; wPipeIndex < wPipeCount; wPipeIndex++)
        {
            pPipe = (MGC_Pipe *)MUSB_ListFindItem(&(pSlot->PipeList), 0);
            MUSB_ListRemoveItem(&(pSlot->PipeList), pPipe);
        }
        MUSB_MemSet(pSlot, 0, sizeof(MGC_ScheduleSlot));
        MUSB_MemFree(pSlot);
    }
    pSchedule->dwTotalTime = 0L;
    pSchedule->wSlotCount = 0;
}

#ifdef MUSB_SCHEDULER
/*
* Get the time in a slot
*/
static uint32_t MGC_GetSlotTime(MGC_ScheduleSlot *pSlot)
{
    MGC_Pipe *pPipe;
    uint16_t wPipeIndex;
    uint32_t dwSlotTime = 0L;

    /* accumulate time taken by all pipes in this slot */
    wPipeIndex = 0;
    pPipe = MUSB_ListFindItem(&(pSlot->PipeList), wPipeIndex++);
    while(pPipe)
    {
        dwSlotTime += pPipe->dwMaxBusTime;
        pPipe = MUSB_ListFindItem(&(pSlot->PipeList), wPipeIndex++);
    }
    return dwSlotTime;
}
#endif

#ifdef MUSB_SCHEDULER
/*
 * Find a slot in the schedule for the given bus time.
 * Returns: NULL on failure; otherwise slot pointer
 */
static MGC_ScheduleSlot *MGC_FindScheduleSlot(MGC_Schedule *pSchedule,
        MGC_BusTimeInfo *pBusTimeInfo,
        uint16_t wInterval,
        uint16_t wDuration,
        uint32_t dwBusTime)
{
    MUSB_LinkedList *pElement;
    MGC_ScheduleSlot *pSlot;
    uint32_t dwSlotTime;
    uint32_t dwOffset = 0L;
    uint16_t wMaxOffset = 0;

    /* walk schedule looking for space in existing slot */
    pElement = &(pSchedule->ScheduleSlots);
    while(pElement)
    {
        /* walk slots looking for compatible interval/duration and available time */
        pSlot = (MGC_ScheduleSlot *)pElement->pItem;
        if(pSlot)
        {
            dwOffset = pSlot->wFrameOffset + pSlot->wDuration;
            /* track maximum offset in case we need to try a new slot */
            if((dwOffset - 1) > wMaxOffset)
            {
                wMaxOffset = (uint16_t)(dwOffset - 1);
            }
            if((pSlot->wInterval == wInterval) &&
                    (pSlot->wDuration == wDuration))
            {
                dwSlotTime = MGC_GetSlotTime(pSlot);
                /* if time remains in this slot, use it */
                if((dwSlotTime + dwBusTime) <= pBusTimeInfo->dwMaxPeriodic)
                {
                    return pSlot;
                }
            }
        }
        pElement = pElement->pNext;
    }

    /* see if we can create a new slot */
    if(((dwOffset + wDuration) < pSchedule->wSlotCount) &&
            (dwBusTime <= pBusTimeInfo->dwMaxPeriodic) &&
            (wMaxOffset <= (wInterval >> 1)))
    {
        pSlot = MUSB_MemAlloc(sizeof(MGC_ScheduleSlot));
        if(pSlot)
        {
            pSlot->bIsActive = FALSE;
            pSlot->wDuration = wDuration;
            pSlot->wFrameOffset = (uint16_t)dwOffset;
            pSlot->wInterval = wInterval;
            MUSB_ListInit(&(pSlot->PipeList));
            if(MUSB_ListAppendItem(&(pSchedule->ScheduleSlots), pSlot, 0))
            {
                /* update slot count if needed */
                if(wInterval < pSchedule->wSlotCount)
                {
                    pSchedule->wSlotCount = wInterval;
                }
                return pSlot;
            }
        }
    }

    /* can't schedule or out of memory */
    return NULL;
}
#else
/*
 * Find a slot in the schedule for the given bus time.
 * Returns: NULL on failure; otherwise slot pointer
 */
static MGC_ScheduleSlot *MGC_FindScheduleSlot(MGC_Schedule *pSchedule,
        MGC_BusTimeInfo *pBusTimeInfo,
        uint16_t wInterval,
        uint16_t wDuration,
        uint32_t dwBusTime)
{
    MGC_ScheduleSlot *pSlot = NULL;

    if(!pSchedule->wSlotCount)
    {
        /* no slots; make first one */
        pSlot = MUSB_MemAlloc(sizeof(MGC_ScheduleSlot));
        if(pSlot)
        {
            MUSB_MemSet(pSlot, 0, sizeof(MGC_ScheduleSlot));
            MUSB_ListInit(&(pSlot->PipeList));
            if(MUSB_ListAppendItem(&(pSchedule->ScheduleSlots), pSlot, 0))
            {
                pSchedule->wSlotCount = 1;
            }
            else
            {
                MUSB_MemFree(pSlot);
                pSlot = NULL;
            }
        }
    }
    else
    {
        /* find existing slot */
        pSlot = (MGC_ScheduleSlot *)MUSB_ListFindItem(&(pSchedule->ScheduleSlots), 0);
    }

    if(pSlot)
    {
        MUSB_PRT("dwTotalTime[%x]dwBusTime[%x]dwMaxPeriodic[%x]\r\n", pSchedule->dwTotalTime, dwBusTime, pBusTimeInfo->dwMaxPeriodic);
        if((pSchedule->dwTotalTime + dwBusTime) <= pBusTimeInfo->dwMaxPeriodic)
        {
            pSchedule->dwTotalTime += dwBusTime;
        }
        else
        {
            /* can't schedule */
            pSlot = NULL;
        }
    }

    return pSlot;
}
#endif	/* MUSB_SCHEDULER */

static uint32_t MGC_ComputeBandwidth(MGC_BusTimeInfo *pBusTimeInfo,
                                     uint8_t bTrafficType, uint8_t bIsIn, uint16_t wMaxPacketSize)
{
    uint32_t dwBusTime;
    uint32_t dwBitStuffTime;

    dwBusTime = pBusTimeInfo->dwControllerSetup;

    /*
     * Compute Floor(3.167 * BitStuffTime(wMaxPacketSize)),
     * where BitStuffTime(n) = 7 * 8 * n / 6,
     * ASSUMES max payload per USB 2.0 (3072); larger than this may overflow numerator
     */
    dwBitStuffTime = (3167L * 56 * wMaxPacketSize) / 6000;

    /* Add payload-dependent time term */
    dwBusTime += (pBusTimeInfo->dwPayloadScale * dwBitStuffTime) / 10;

    switch(bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_ISOC:
        /* Additional time term */
        if(bIsIn)
        {
            dwBusTime += pBusTimeInfo->dwIsoInOverhead;
        }
        else
        {
            dwBusTime += pBusTimeInfo->dwIsoOutOverhead;
        }
        break;
    default:
        /* Additional time term */
        if(bIsIn)
        {
            dwBusTime += pBusTimeInfo->dwInOverhead;
        }
        else
        {
            dwBusTime += pBusTimeInfo->dwOutOverhead;
        }
        break;
    }

    return dwBusTime;
}

/**
 * Verify that the bandwidth required for the given pipe fits,
 * both on its local bus (if hub support is enabled) and
 * on the global bus rooted at the port.
 * Update the pipe's slot, bus time, and traffic type & max packet size
 * (since they are computed here anyway)
 * @return TRUE on success (bandwidth available and committed,
 * and pipe updated)
 * @return FALSE on failure (insufficient available bandwidth
 * or out of memory on allocating a bookkeeping struct)
 */
static uint8_t MGC_CommitBandwidth(MGC_Port *pPort, MGC_Pipe *pPipe,
                                   const MUSB_DeviceEndpoint *pRemoteEnd)
{
#ifdef MUSB_HUB
    MGC_Device *pImplDevice;
#endif
    MGC_Schedule *pSchedule;
    uint32_t dwBusTime;
    uint32_t dwLocalBusTime;
    uint16_t wMaxPacketSize;
    MGC_BusTimeInfo *pBusTimeInfo;
    MGC_BusTimeInfo *pLocalBusTimeInfo = NULL;
    uint16_t wDuration = 1;
    uint8_t bOk = FALSE;
#ifdef MUSB_HUB
    const MUSB_Device *pDevice = pRemoteEnd->pDevice;
    MGC_ScheduleSlot *pLocalSlot = NULL;
#endif
    MGC_ScheduleSlot *pSlot = NULL;
    uint8_t bInterval = pRemoteEnd->UsbDescriptor.bInterval & 0xf;
    uint8_t bTrafficType = pRemoteEnd->UsbDescriptor.bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK;
    uint8_t bIsIn = (pRemoteEnd->UsbDescriptor.bEndpointAddress & MUSB_DIR_IN) ? TRUE : FALSE;
    uint16_t wInterval = bInterval;

    /* compute needed bandwidth */
    if(pPort->bIsHighSpeed)
    {
        pBusTimeInfo = &MGC_HighSpeedFrame;
    }
    else if(pPort->bIsLowSpeed)
    {
        pBusTimeInfo = &MGC_LowSpeedFrame;
    }
    else
    {
        pBusTimeInfo = &MGC_FullSpeedFrame;
    }

    switch(pRemoteEnd->pDevice->ConnectionSpeed)
    {
    case MUSB_CONNECTION_SPEED_HIGH:
        pLocalBusTimeInfo = &MGC_HighSpeedFrame;
        wDuration = (MUSB_SWAP16P((uint8_t *) & (pRemoteEnd->UsbDescriptor.wMaxPacketSize)) & MUSB_M_ENDPOINT_PACKETS_PER_FRAME) >>
                    MUSB_S_ENDPOINT_PACKETS_PER_FRAME;
        if(!wDuration)
        {
            wDuration = 1;
        }

        if((bTrafficType == MUSB_ENDPOINT_XFER_ISOC) || (bTrafficType == MUSB_ENDPOINT_XFER_INT))
            wInterval = 1 << (bInterval - 1);
        break;
    case MUSB_CONNECTION_SPEED_FULL:
        pLocalBusTimeInfo = &MGC_FullSpeedFrame;
        if(bTrafficType == MUSB_ENDPOINT_XFER_ISOC)
            wInterval = 1 << (bInterval - 1);
        break;
    case MUSB_CONNECTION_SPEED_LOW:
        pLocalBusTimeInfo = &MGC_LowSpeedFrame;
        break;
    default:
        break;
    }

    wMaxPacketSize = wDuration * (MUSB_SWAP16P((uint8_t *) & (pRemoteEnd->UsbDescriptor.wMaxPacketSize)) & MUSB_M_ENDPOINT_MAX_PACKET_SIZE);

    dwBusTime = MGC_ComputeBandwidth(pBusTimeInfo, bTrafficType, bIsIn,
                                     wMaxPacketSize);
    dwLocalBusTime = MGC_ComputeBandwidth(pLocalBusTimeInfo, bTrafficType, bIsIn,
                                          wMaxPacketSize);
    dwLocalBusTime ++;//avoid warning
    /*
     * Verify that the needed bandwidth fits.
     */
    switch(bTrafficType)
    {
    case MUSB_ENDPOINT_XFER_ISOC:
    case MUSB_ENDPOINT_XFER_INT:
        pSchedule = &(pPort->Schedule);
        pSlot = MGC_FindScheduleSlot(pSchedule, pBusTimeInfo, wInterval,
                                     wDuration, dwBusTime);
        if(pSlot)
        {
            bOk = TRUE;
#ifdef MUSB_HUB
            if(pDevice->pParentUsbDevice)
            {
                pImplDevice = (MGC_Device *)pDevice->pParentUsbDevice->pPrivateData;
                pSchedule = pImplDevice->pSchedule;
                if(!pSchedule)
                {
                    pSchedule = MUSB_MemAlloc(sizeof(MGC_Schedule));
                    if(pSchedule)
                    {
                        pImplDevice->pSchedule = pSchedule;
                        MUSB_MemSet(pSchedule, 0, sizeof(MGC_Schedule));
                        MUSB_ListInit(&(pSchedule->ScheduleSlots));
                    }
                }
                if(pSchedule)
                {
                    pLocalSlot = MGC_FindScheduleSlot(pSchedule,
                                                      pLocalBusTimeInfo, wInterval, wDuration, dwBusTime);
                    if(pLocalSlot)
                    {
                        bOk = MUSB_ListAppendItem(&(pLocalSlot->PipeList), pPipe, 0);
                    }
                    else
                    {
                        bOk = FALSE;
                    }
                }
                else
                {
                    bOk = FALSE;
                }
            }
#endif
            if(bOk)
            {
                bOk = MUSB_ListAppendItem(&(pSlot->PipeList), pPipe, 0);
                pPipe->pSlot = pSlot;
            }
        }
        break;
    default:
        /*
         * We assume the periodic bandwidth limits in the spec are
         * intended to allow control/bulk transfers to fit somewhere
         */
        bOk = TRUE;
        break;
    }

    if(bOk)
    {
        pPipe->dwMaxBusTime = dwBusTime;
        pPipe->bTrafficType = bTrafficType;
        pPipe->wMaxPacketSize = wMaxPacketSize;
    }
    return bOk;
}

/*
 * Open a pipe
 */
MUSB_PipePtr MUSB_OpenPipe(MUSB_BusHandle hBus,
                           const MUSB_DeviceEndpoint *pRemoteEnd,
                           MUSB_EndpointResource *pEndpointResource)
{
    MUSB_EndpointResource end;
    uint8_t bIsTx;
    uint8_t bBandwidthOk = FALSE;
    void *pResult = NULL;
    MGC_Pipe *pPipe = NULL;
    MUSB_EndpointResource *pLocalEnd = pEndpointResource;
    MGC_EndpointResource *pResource = NULL;
    MGC_Port *pPort = (MGC_Port *)hBus;
    const MUSB_Device *pDevice = pRemoteEnd->pDevice;
    MGC_Device *pImplDevice = (MGC_Device *)pDevice->pPrivateData;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;

    /* lock */
    pServices->pfLock(pServices->pPrivateData, 0);

    /* update bus speed reading */
    pPort->pfReadBusState(pPort);

    /* try to allocate */
    pPipe = (MGC_Pipe *)MUSB_MemAlloc(sizeof(MGC_Pipe));
    if(pPipe)
    {
        /* allocated pipe; fill info */
        MUSB_MemSet(pPipe, 0, sizeof(MGC_Pipe));
        pPipe->hSession = hBus;
        pPipe->pPort = pPort;
        if(pRemoteEnd->UsbDescriptor.bEndpointAddress & MUSB_DIR_IN)
        {
            pPipe->bmFlags = MGC_PIPEFLAGS_HOST;
            bIsTx = FALSE;
        }
        else
        {
            pPipe->bmFlags = MGC_PIPEFLAGS_HOST | MGC_PIPEFLAGS_TRANSMIT;
            bIsTx = TRUE;
        }
        pPipe->pDevice = pDevice;

        /* try to get core resource */
        if(!pLocalEnd)
        {
            end.bmFlags = 0;
            end.dwBufferSize = pRemoteEnd->UsbDescriptor.wMaxPacketSize;
            pLocalEnd = &end;
        }

        if (pPort->pfBindEndpoint)
        {
            pResource = pPort->pfBindEndpoint(pPort, pRemoteEnd, pLocalEnd, TRUE);
        }

        if(pResource)
        {
            /* got resource; try to get bandwidth if applicable */
            bBandwidthOk = MGC_CommitBandwidth(pPort, pPipe, pRemoteEnd);
        }

        /* if all OK so far, try to add pipe to device pipe list */
        if(pResource && bBandwidthOk &&
                MUSB_ListAppendItem(&(pImplDevice->PipeList), pPipe, 0))
        {
            /* success */
            pPipe->pLocalEnd = pResource;
            pResult = pPipe;
        }
        else
        {
            /* failure cleanup */
            if(pResource)
            {
                if(bIsTx)
                {
                    pResource->bIsClaimed = FALSE;
                }
                else
                {
                    pResource->bRxClaimed = FALSE;
                }
                if (pPort->pfProgramFlushEndpoint)
                {
                    pPort->pfProgramFlushEndpoint(pPort, pResource,
                                                  pRemoteEnd->UsbDescriptor.bEndpointAddress & MUSB_DIR_IN, FALSE);
                }
            }
            MUSB_MemFree(pPipe);
        }
    }

    /* unlock */
    pServices->pfUnlock(pServices->pPrivateData, 0);

    return pResult;
}

/*
 * Close a pipe
 */
uint32_t MUSB_ClosePipe(MUSB_PipePtr hPipe)
{
    uint8_t bDirection;
    uint8_t bIsTx;
    uint32_t dwStatus = MUSB_STATUS_OK;
    MGC_Pipe *pPipe = (MGC_Pipe *)hPipe;
    const MUSB_Device *pDevice = pPipe->pDevice;
    MGC_Device *pImplDevice = (MGC_Device *)pDevice->pPrivateData;
    MGC_Port *pPort = (MGC_Port *)pPipe->hSession;
    MGC_EndpointResource *pEnd = pPipe->pLocalEnd;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;

    /* lock */
    pServices->pfLock(pServices->pPrivateData, 0);

    pPipe->bmFlags |= MGC_PIPEFLAGS_CLOSING;

    bIsTx = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ? TRUE : FALSE;
    if(pPipe->bmFlags & MGC_PIPEFLAGS_HOST)
    {
        bDirection = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ?
                     MUSB_DIR_OUT : MUSB_DIR_IN;
    }
    else
    {
        bDirection = (pPipe->bmFlags & MGC_PIPEFLAGS_TRANSMIT) ?
                     MUSB_DIR_IN : MUSB_DIR_OUT;
    }

    if (pPort->pfProgramFlushEndpoint)
    {
        dwStatus = pPort->pfProgramFlushEndpoint(pPort, pEnd, bDirection, TRUE);
    }

    if(pPipe->pSlot)
    {
        MUSB_ListRemoveItem(&(pPipe->pSlot->PipeList), pPipe);
    }
    MUSB_ListRemoveItem(&(pImplDevice->PipeList), pPipe);
    MUSB_MemSet(pPipe, 0, sizeof(MGC_Pipe));
    MUSB_MemFree(pPipe);

    if(bIsTx)
    {
        pEnd->bIsClaimed = FALSE;
    }
    else
    {
        pEnd->bRxClaimed = FALSE;
    }

    /* unlock */
    pServices->pfUnlock(pServices->pPrivateData, 0);

    return dwStatus;
}

/*
 * Start a single control transfer
 */
uint32_t MUSB_StartControlTransfer(MUSB_Port *pPort, MUSB_ControlIrp *pIrp)
{
    uint32_t status = MUSB_STATUS_NO_RESOURCES;
    MGC_Port *pImplPort = (MGC_Port *)pPort->pPrivateData;
    MUSB_SystemServices *pServices = pImplPort->pController->pSystemServices;
    MGC_EndpointResource *pEnd = NULL;

    MUSB_PRT("\r\n MUSB_StartControlTransfer\r\n");

    pIrp->dwActualInLength = pIrp->dwActualOutLength = 0;
    pIrp->dwStatus = MUSB_STATUS_OK;

    /* lock EP0 */
    pServices->pfLock(pServices->pPrivateData, 1);

    /* NOTE: assumes EP0 is first */
    pEnd = MUSB_ArrayFetch(&(pImplPort->LocalEnds), 0);
    if(pEnd)
    {
        if(!pEnd->pTxIrp)
        {
            pEnd->bIsTx = TRUE;
            pEnd->wPacketSize = pIrp->pDevice->DeviceDescriptor.bMaxPacketSize0;
            if(!pEnd->wPacketSize)
            {
                pEnd->wPacketSize = 8;
            }
            if (pImplPort->pfProgramStartTransmit)
            {
                status = pImplPort->pfProgramStartTransmit(pImplPort, pEnd,
                         pIrp->pOutBuffer, 8, pIrp);
            }
        }
        else
        {
            if(!MUSB_ListAppendItem(&(pEnd->TxIrpList), pIrp, 0))
            {
                status = MUSB_STATUS_NO_MEMORY;
            }
        }
    }
    /* unlock EP0 */
    pServices->pfUnlock(pServices->pPrivateData, 1);

    return status;
}

/*
 * Cancel a pending control transfer
 */
uint32_t MUSB_CancelControlTransfer(MUSB_Port *pPort, MUSB_ControlIrp *pIrp)
{
    void *pListIrp;
    uint16_t wCount, wIndex;
    uint32_t status = MUSB_STATUS_OK;
    MGC_Port *pImplPort = (MGC_Port *)pPort->pPrivateData;
    MUSB_SystemServices *pServices = pImplPort->pController->pSystemServices;
    MGC_EndpointResource *pEnd = NULL;

    pIrp->dwActualInLength = pIrp->dwActualOutLength = 0;
    pIrp->dwStatus = MUSB_STATUS_OK;

    /* lock EP0 */
    pServices->pfLock(pServices->pPrivateData, 1);

    /* NOTE: assumes EP0 is first */
    pEnd = MUSB_ArrayFetch(&(pImplPort->LocalEnds), 0);
    if(pEnd)
    {
        if(pEnd->pTxIrp == pIrp)
        {
            /* it is the current IRP: flush, remove, start next */
            if (pImplPort->pfProgramFlushEndpoint)
            {
                pImplPort->pfProgramFlushEndpoint(pImplPort, pEnd, 0, FALSE);
            }

            pEnd->pTxIrp = NULL;
            MGC_StartNextControlTransfer(pImplPort);
        }
        else
        {
            /* not the current IRP; try to find in list */
            wCount = MUSB_ListLength(&(pEnd->TxIrpList));
            for(wIndex = 0; wIndex < wCount; wIndex++)
            {
                pListIrp = MUSB_ListFindItem(&(pEnd->TxIrpList), wIndex);
                if(pListIrp == pIrp)
                {
                    MUSB_ListRemoveItem(&(pEnd->TxIrpList), pListIrp);
                }
            }
        }
    }

    /* unlock EP0 */
    pServices->pfUnlock(pServices->pPrivateData, 1);

    return status;
}

#ifdef MUSB_ISO
uint32_t MUSB_AdjustIsochTransfer(
    int16_t wFrameAdjustment, MUSB_IsochIrp *pIsochIrp)
{
    MGC_Pipe *pPipe = (MGC_Pipe *)pIsochIrp->hPipe;

    if(wFrameAdjustment >= 0)
    {
        pPipe->pLocalEnd->dwWaitFrameCount += wFrameAdjustment;
    }
    else if(pPipe->pLocalEnd->dwWaitFrameCount > wFrameAdjustment)
    {
        pPipe->pLocalEnd->dwWaitFrameCount -= wFrameAdjustment;
    }
    else
    {
        return MUSB_STATUS_UNSUPPORTED;
    }
    return MUSB_STATUS_OK;
}
#endif

static void MGC_DriverTimerExpired(void *pControllerPrivateData)
{
    MGC_Controller *pController = (MGC_Controller *)pControllerPrivateData;
    MGC_Port *pPort = pController->pPort;
    void *pParam = pPort->pDriverTimerData;

    if(pPort->pfDriverTimerExpired)
    {
        pPort->pDriverTimerData = NULL;
        pPort->pfDriverTimerExpired(pParam, pPort);
    }
}

uint32_t MUSB_ArmTimer(MUSB_BusHandle hBus, MUSB_DeviceDriver *pDriver,
                       uint8_t bTimerIndex, uint32_t dwTime,
                       MUSB_pfDriverTimerExpired pfDriverTimerExpired, void *pParam)
{
    uint8_t bOk;
    MGC_Port *pPort = (MGC_Port *)hBus;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;

    /*
     * For now, support only one, and borrow the OTG timer
     * (since it better not be active at this time)
     */
    if(pPort->pDriverTimerData)
    {
        return MUSB_STATUS_NO_RESOURCES;
    }
    pPort->pfDriverTimerExpired = pfDriverTimerExpired;
    pPort->pDriverTimerData = pParam;
    bOk = pServices->pfArmTimer(pServices->pPrivateData, 0, dwTime, FALSE,
                                MGC_DriverTimerExpired);

    return bOk ? 0 : MUSB_STATUS_NO_RESOURCES;
}

uint32_t MUSB_CancelTimer(MUSB_BusHandle hBus,
                          MUSB_DeviceDriver *pDriver, uint8_t bTimerIndex)
{
    MGC_Port *pPort = (MGC_Port *)hBus;
    MUSB_SystemServices *pServices = pPort->pController->pSystemServices;

    pServices->pfCancelTimer(pServices->pPrivateData, 0);

    return 0;
}

uint32_t MGC_HostSetMaxPower(MGC_Port *pPort, uint16_t wPower)
{
#ifdef MUSB_OTG
    pPort->pfReadBusState(pPort);
    if(!pPort->bConnectorId)
    {
#endif

        /* TODO: check new power requirements */

#ifdef MUSB_OTG
    }
#endif
    return MUSB_STATUS_UNSUPPORTED;
}

#endif	/* host or OTG */

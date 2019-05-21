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
#include "include.h"

#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_impl.h"
#include "mu_stdio.h"
#include "mu_strng.h"
#include "mu_hfi.h"
#include "mu_spi.h"
#include "mu_none.h"
#include "mu_hidif.h"
#include "mu_kii.h"
#include "mu_nci.h"

#if CFG_SUPPORT_HID
static void MGC_HidNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State);
static void MGC_HidOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus);

/**************************** GLOBALS *****************************/
static MUSB_Port *MGC_pCdiPort = NULL;
static MUSB_BusHandle MGC_hCdiBus = NULL;
static uint8_t MGC_bDesireHostRole = TRUE;
static uint8_t MGC_aHidPeripheralList[256];
static MUSB_DeviceDriver MGC_aDeviceDriver[2];

static MUSB_HostClient MGC_HidHostClient =
{
    MGC_aHidPeripheralList,		/* peripheral list */
    0,			    /* filled in main */
    MGC_aDeviceDriver,
    0					/* filled in main */
};

static MUSB_OtgClient MGC_HidOtgClient =
{
    NULL,	/* no instance data; we are singleton */
    &MGC_bDesireHostRole,
    MGC_HidNewOtgState,
    MGC_HidOtgError
};

static MUSB_HfiDevice *MGC_pHfiDevice = NULL;
static uint8_t MediaIsOk = FALSE;
static MUSB_LinkedList MGC_TestNciPortList;

/*************************** FUNCTIONS ****************************/
MUSB_GhiStatus MUSB_GhiAddDevice(MUSB_GhiDeviceHandle *phDevice,
                                 const MUSB_GhiDeviceInfo *pInfo, MUSB_GhiDevice *pDevice)
{
    return MUSB_GHI_ERROR_UNSUPPORTED_DEVICE;
}

void MUSB_GhiDeviceRemoved(MUSB_GhiDeviceHandle hDevice)
{
}

MUSB_KiiStatus MUSB_KiiAddDevice(MUSB_KiiDeviceHandle *phDevice,
                                 const MUSB_KiiDeviceInfo *pInfo,
                                 MUSB_KiiDevice *pDevice)
{
    MUSB_PRT("Keyboard added\r\n");
    return MUSB_KII_SUCCESS;
}

void MUSB_KiiModifiersChanged(MUSB_KiiDeviceHandle hDevice,
                              uint32_t dwModifierMask)
{
    MUSB_PRT("Modifiers changed\r\n");
}

void MUSB_KiiCharacterKeyPressed(MUSB_KiiDeviceHandle hDevice,
                                 uint16_t wCode)
{
    MUSB_PRT("Char pressed\r\n");
}

void MUSB_KiiCharacterKeyReleased(MUSB_KiiDeviceHandle hDevice,
                                  uint16_t wCode)
{
    MUSB_PRT("Char released\r\n");
}

void MUSB_KiiSpecialKeyPressed(MUSB_KiiDeviceHandle hDevice,
                               MUSB_KiiSpecialKey Key)
{
    MUSB_PRT("Special pressed\r\n");
}

void MUSB_KiiSpecialKeyReleased(MUSB_KiiDeviceHandle hDevice,
                                MUSB_KiiSpecialKey Key)
{
    MUSB_PRT("Special released\r\n");
}

void MUSB_KiiDeviceRemoved(MUSB_KiiDeviceHandle hDevice)
{
    MUSB_PRT("Keyboard removed\r\n");
}

MUSB_SpiStatus MUSB_SpiAddDevice(MUSB_SpiDeviceHandle *phDevice,
                                 const MUSB_SpiDeviceInfo *pInfo, MUSB_SpiDevice *pDevice)
{
    MUSB_PRT("Mouse added\r\n");
    return MUSB_SPI_SUCCESS;
}

void MUSB_SpiButtonPressed(MUSB_SpiDeviceHandle hDevice,
                           uint8_t bButtonIndex)
{
    char aLine[80];
    char aNumber[12];

    aLine[0] = (char)0;
    MUSB_StringConcat(aLine, 80, "Mouse button ");
    MUSB_Stringize(aNumber, 12, bButtonIndex + 1, 10, 0);
    MUSB_StringConcat(aLine, 80, aNumber);
    MUSB_StringConcat(aLine, 80, " pressed");
    MUSB_PrintLine(aLine);
}

void MUSB_SpiButtonReleased(MUSB_SpiDeviceHandle hDevice,
                            uint8_t bButtonIndex)
{
    char aLine[80];
    char aNumber[12];

    aLine[0] = (char)0;
    MUSB_StringConcat(aLine, 80, "Mouse button ");
    MUSB_Stringize(aNumber, 12, bButtonIndex + 1, 10, 0);
    MUSB_StringConcat(aLine, 80, aNumber);
    MUSB_StringConcat(aLine, 80, " released");
    MUSB_PrintLine(aLine);
}

void MUSB_SpiWheelsMoved(MUSB_SpiDeviceHandle hDevice,
                         const int8_t *pMotions)
{
    char aLine[80];
    char aNumber[12];

    if(pMotions[0] > 127)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse X wheel moved -");
        MUSB_Stringize(aNumber, 12, 0x100 - pMotions[0], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
    else if(pMotions[0] != 0)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse X wheel moved +");
        MUSB_Stringize(aNumber, 12, pMotions[0], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
    if(pMotions[1] > 127)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse Y wheel moved -");
        MUSB_Stringize(aNumber, 12, 0x100 - pMotions[1], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
    else if(pMotions[1] != 0)
    {
        aLine[0] = (char)0;
        MUSB_StringConcat(aLine, 80, "Mouse Y wheel moved +");
        MUSB_Stringize(aNumber, 12, pMotions[1], 10, 0);
        MUSB_StringConcat(aLine, 80, aNumber);
        MUSB_PrintLine(aLine);
    }
}

void MUSB_SpiDeviceRemoved(MUSB_SpiDeviceHandle hDevice)
{
    MUSB_PRT("Mouse removed\r\n");
}

/* NCI implementations */
MUSB_NciStatus MUSB_NciAddPort(MUSB_NciClientHandle *phClient,
                               const MUSB_NciPortInfo *pPortInfo,
                               const MUSB_NciConnectionInfo *pConnectionInfo,
                               MUSB_NciPortServices *pPortServices)
{
    return MUSB_NCI_NO_MEMORY;
}

void MUSB_NciPortConnected(MUSB_NciClientHandle hClient,
                           const MUSB_NciConnectionInfo *pConnectionInfo)
{
}

void MUSB_NciPortPacketReceived(MUSB_NciClientHandle hClient,
                                uint8_t *pBuffer, uint16_t wLength, uint8_t bMustCopy)
{
}

void MUSB_NciReturnBuffer(MUSB_NciClientHandle hClient,
                          MUSB_NciRxBuffer *pBuffer)
{
}

void MUSB_NciPortDisconnected(MUSB_NciClientHandle hClient)
{
}

void MUSB_NciPortRemoved(MUSB_NciClientHandle hClient)
{
}
/* OTG client */
static void MGC_HidNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State)
{
    char aAnswer[4];

    switch(State)
    {
    case MUSB_AB_IDLE:
        MUSB_PrintLine("S - Start Session");
        MUSB_GetLine(aAnswer, 4);
        if(('s' == aAnswer[0]) || ('S' == aAnswer[0]))
        {
            MUSB_RequestBus(MGC_hCdiBus);
        }
        break;

    case MUSB_A_SUSPEND:
        MUSB_PrintLine("R - Resume bus");
        MUSB_GetLine(aAnswer, 4);
        if(('r' == aAnswer[0]) || ('R' == aAnswer[0]))
        {
            MUSB_ResumeBus(MGC_hCdiBus);
        }
        break;

    default:
        break;
    }
}

static void MGC_HidOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus)
{
    switch(dwStatus)
    {
    case MUSB_STATUS_UNSUPPORTED_DEVICE:
        MUSB_PRT("Device not supported\r\n");
        break;

    case MUSB_STATUS_UNSUPPORTED_HUB:
        MUSB_PRT("Hubs not supported\r\n");
        break;

    case MUSB_STATUS_OTG_VBUS_INVALID:
        MUSB_PRT("Vbus error\r\n");
        break;

    case MUSB_STATUS_OTG_NO_RESPONSE:
        MUSB_PRT("Device not responding\r\n");
        break;

    case MUSB_STATUS_OTG_SRP_FAILED:
        MUSB_PRT("Device not responding (SRP failed)\r\n");
        break;

    default:
        break;
    }
}

MUSB_HfiStatus MUSB_HfiAddDevice(MUSB_HfiVolumeHandle *phVolume,
                                 const MUSB_HfiDeviceInfo *pInfo,
                                 MUSB_HfiDevice *pDevice)
{
    MGC_pHfiDevice = pDevice;
    MediaIsOk = TRUE;
    return MUSB_HFI_SUCCESS;

}

void MUSB_HfiMediumInserted(MUSB_HfiVolumeHandle 	 hVolume,
                            const MUSB_HfiMediumInfo *pMediumInfo)
{
}

void MUSB_HfiMediumRemoved(MUSB_HfiVolumeHandle hVolume)
{
}

void MUSB_HfiDeviceRemoved(void)
{
    MGC_pHfiDevice = NULL;
    MediaIsOk = FALSE;
}

uint8_t MGC_HidGetMediumstatus(void)
{
    return MediaIsOk;
}

int usb_sw_init(void)
{
    uint8_t *pList;
    uint8_t bDriver = 0;
    uint16_t wCount = 0;
    uint16_t wSize = 0;
    uint16_t wRemain;
    MUSB_DeviceDriver *pDriver;

    wRemain = (uint16_t)sizeof(MGC_aHidPeripheralList);
    pList = MGC_aHidPeripheralList;

    wSize = MUSB_FillHidPeripheralList(bDriver, pList, wRemain);
    if(wSize < wRemain)
    {
        pDriver = MUSB_GetHidClassDriver();
        if(pDriver)
        {
            MUSB_MemCopy(&(MGC_HidHostClient.aDeviceDriverList[bDriver]),
                         pDriver,
                         sizeof(MUSB_DeviceDriver));

            pList += wSize;
            wCount += wSize;
            wRemain -= wSize;

            bDriver++;
        }
    }

    MGC_HidHostClient.wPeripheralListLength = wCount;
    MGC_HidHostClient.bDeviceDriverListLength = bDriver;

    if(!MUSB_InitSystem(5))
    {
        MUSB_PRT("[MGC] InitSystem failed\r\n");
        return -1;
    }

    /* find first CDI port */
    MGC_pCdiPort = MUSB_GetPort(0);

    /* start session */
    MGC_hCdiBus = MUSB_RegisterOtgClient(MGC_pCdiPort,
                                         0,
                                         &MGC_HidHostClient,
                                         &MGC_HidOtgClient);

    MUSB_NoneRunBackground();

    return 0;
}

int usb_sw_uninit(void)
{
    return 0;
}
#endif // CFG_SUPPORT_HID

// eof


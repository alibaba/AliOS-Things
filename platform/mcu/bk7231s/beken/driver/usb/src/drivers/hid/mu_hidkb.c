/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2006                           *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * Boot keyboard functions.
 * Supports AT-101 keys.
 * $Revision: 1.4 $
 */
#include "include.h"

#include "mu_arch.h"
#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_bits.h"
#include "mu_descs.h"
#include "mu_stdio.h"
#include "mu_diag.h"
#include "mu_kii.h"
#include "mu_hid.h"
#include "mu_hidif.h"

/****************************** CONSTANTS ********************************/
#if CFG_SUPPORT_HID
/* lookup table: USB to KII special codes */
static const MUSB_KiiSpecialKey abUsbToSpecial[] =
{
    MUSB_KII_KEY_CAPS_LOCK, /* keycode 0x39 */
    MUSB_KII_KEY_F1, MUSB_KII_KEY_F2, MUSB_KII_KEY_F3, MUSB_KII_KEY_F4,
    MUSB_KII_KEY_F5, MUSB_KII_KEY_F6, MUSB_KII_KEY_F7, MUSB_KII_KEY_F8,
    MUSB_KII_KEY_F9, MUSB_KII_KEY_F10, MUSB_KII_KEY_F11, MUSB_KII_KEY_F12,
    MUSB_KII_KEY_PRINT_SCREEN,
    MUSB_KII_KEY_SCROLL_LOCK,
    MUSB_KII_KEY_BREAK, /* called pause in USB */
    MUSB_KII_KEY_INSERT,
    MUSB_KII_KEY_HOME,
    MUSB_KII_KEY_PAGE_UP,
    MUSB_KII_KEY_DELETE,
    MUSB_KII_KEY_END,
    MUSB_KII_KEY_PAGE_DOWN,
    MUSB_KII_KEY_RIGHT,
    MUSB_KII_KEY_LEFT,
    MUSB_KII_KEY_DOWN,
    MUSB_KII_KEY_UP,
    MUSB_KII_KEY_NUM_LOCK /* keycode 0x53 */

};

/* lookup table: USB to numeric when shift is in effect */
static const uint8_t abUsbToShiftedNum[] =
{
    (uint8_t)'!', /* keycode 0x1e */
    (uint8_t)'@',
    (uint8_t)'#',
    (uint8_t)'$',
    (uint8_t)'%',
    (uint8_t)'^',
    (uint8_t)'&',
    (uint8_t)'*',
    (uint8_t)'(',
    (uint8_t)')'  /* keycode 0x27 */
};

/* lookup table: USB to other special characters */
static const uint8_t abUsbToOther[] =
{
    (uint8_t)'-', /* keycode 0x2d */
    (uint8_t)'=',
    (uint8_t)'[',
    (uint8_t)']',
    (uint8_t)'\\',
    (uint8_t)'#',
    (uint8_t)';',
    (uint8_t)'\'',
    (uint8_t)'`',
    (uint8_t)',',
    (uint8_t)'.',
    (uint8_t)'/'  /* keycode 0x38 */
};

/* lookup table: USB to other special characters when shift is in effect */
static const uint8_t abUsbToShiftedOther[] =
{
    (uint8_t)'_', /* keycode 0x2d */
    (uint8_t)'+',
    (uint8_t)'{',
    (uint8_t)'}',
    (uint8_t)'|',
    (uint8_t)'~',
    (uint8_t)':',
    (uint8_t)'"',
    (uint8_t)'~',
    (uint8_t)'<',
    (uint8_t)'>',
    (uint8_t)'?'  /* keycode 0x38 */
};

/* lookup table: USB to special characters for scroll-lock or no num-lock */
static const MUSB_KiiSpecialKey abUsbToSpecialScrollpad[] =
{
    MUSB_KII_KEY_END,   /* keycode 0x59 */
    MUSB_KII_KEY_DOWN,
    MUSB_KII_KEY_PAGE_DOWN,
    MUSB_KII_KEY_LEFT,
    MUSB_KII_KEY_END,
    MUSB_KII_KEY_RIGHT,
    MUSB_KII_KEY_HOME,
    MUSB_KII_KEY_UP,
    MUSB_KII_KEY_PAGE_UP,
    MUSB_KII_KEY_INSERT,
    MUSB_KII_KEY_DELETE /* keycode 0x63 (also .) */
};

/******************************** TYPES **********************************/

typedef struct
{
    MGC_HidDevice *pHidDevice;
    MUSB_KiiDeviceInfo KiiDeviceInfo;
    MUSB_KiiDevice KiiDevice;
    MUSB_KiiDeviceHandle KiiHandle;
    MUSB_Irp Irp;
    uint8_t abReport[8];
    uint8_t abLastReport[8];
    uint8_t bLastModifiers;
    uint8_t bCapsLock;
    uint8_t bNumLock;
    uint8_t bScrollLock;
} MGC_HidKeyboardData;

/******************************* FORWARDS ********************************/

static uint32_t MGC_HidKeyboardSetProtocol(MGC_HidKeyboardData *pKbd);
static uint32_t MGC_HidSetProtocolCallback(void *pContext, MUSB_ControlIrp *pControlIrp);
static uint32_t MGC_HidKeyboardIntrIrpCallback(void *pContext, MUSB_Irp *pIrp);
static MUSB_KiiStatus MGC_HidKeyboardSetLed(void *pDeviceData,
        uint8_t bIndex, uint8_t bLight);

/******************************* GLOBALS *********************************/

/****************************** FUNCTIONS ********************************/

static void MGC_HidReportKey(MGC_HidKeyboardData *pKbd,
                             uint8_t bKey,
                             uint8_t bPressed)
{
    uint8_t bShift;
    MUSB_KiiSpecialKey bSpecialKey;
    uint16_t wCode = 0xffff;

    bShift = (pKbd->abReport[0] & 0x22) ? 1 : 0;
    if((bKey >= 4) && (bKey <= 0x1d))
    {
        /* letter */
        if(bShift ^ pKbd->bCapsLock) wCode = (uint16_t)('A' + bKey - 4);
        else wCode = (uint16_t)('a' + bKey - 4);
    }
    else if((bKey >= 0x1e) && (bKey <= 0x27))
    {
        /* number */
        if(bShift) wCode = (uint16_t)abUsbToShiftedNum[bKey - 0x1e];
        else if(bKey == 0x27) wCode = (uint16_t)'0';
        else wCode = (uint16_t)('1' + bKey - 0x1e);
    }
    else if((bKey == 0x28) || (bKey == 0x58)) wCode = (uint16_t)'\r';
    else if(bKey == 0x29) wCode = (uint16_t)27;
    else if(bKey == 0x2a) wCode = (uint16_t)8;
    else if(bKey == 0x2b) wCode = (uint16_t)9;
    else if(bKey == 0x2c) wCode = (uint16_t)' ';
    else if((bKey >= 0x2d) && (bKey <= 0x38))
    {
        if(bShift) wCode = (uint16_t)abUsbToShiftedOther[bKey - 0x2d];
        else wCode = (uint16_t)abUsbToOther[bKey - 0x2d];
    }
    else if((bKey >= 0x39) && (bKey <= 0x53))
    {
        bSpecialKey = abUsbToSpecial[bKey - 0x39];
        if(bPressed) MUSB_KiiSpecialKeyPressed(pKbd->KiiHandle, bSpecialKey);
        else MUSB_KiiSpecialKeyReleased(pKbd->KiiHandle, bSpecialKey);
    }
    else if(bKey == 0x54) wCode = (uint16_t)'/';
    else if(bKey == 0x55) wCode = (uint16_t)'*';
    else if(bKey == 0x56) wCode = (uint16_t)'-';
    else if(bKey == 0x57) wCode = (uint16_t)'+';
    else if((bKey >= 0x59) && (bKey <= 0x63))
    {
        if(pKbd->bScrollLock || !pKbd->bNumLock)
        {
            bSpecialKey = abUsbToSpecialScrollpad[bKey - 0x59];
            if(bPressed) MUSB_KiiSpecialKeyPressed(pKbd->KiiHandle, bSpecialKey);
            else MUSB_KiiSpecialKeyReleased(pKbd->KiiHandle, bSpecialKey);
        }
        else
        {
            switch(bKey)
            {
            case 0x63:
                wCode = (uint16_t)'.';
                break;
            case 0x62:
                wCode = (uint16_t)'0';
                break;
            default:
                wCode = (uint16_t)('1' + bKey - 0x59);
            }
        }
    }
    else if(bKey == 0x67) wCode = (uint16_t)'=';

    if(wCode != 0xffff)
    {
        if(bPressed) MUSB_KiiCharacterKeyPressed(pKbd->KiiHandle, wCode);
        else MUSB_KiiCharacterKeyReleased(pKbd->KiiHandle, wCode);
    }
}

void *MGC_HidKeyboardCreateInstance(MGC_HidDevice *pHidDevice,
                                    const MUSB_ConfigurationDescriptor *pConfig,
                                    const MUSB_InterfaceDescriptor *pIface)
{
    uint8_t bEnd;
    uint32_t dwStatus;
    const MUSB_EndpointDescriptor *pEnd;
    const MUSB_EndpointDescriptor *pInEnd = NULL;
    MUSB_DeviceEndpoint end;

    /* try to allocate */
    MGC_HidKeyboardData *pKbd = (MGC_HidKeyboardData *)MUSB_MemAlloc(
                                    sizeof(MGC_HidKeyboardData));
    if(pKbd)
    {
        /* good allocation; initialize */
        MUSB_MemSet(pKbd, 0, sizeof(MGC_HidKeyboardData));
        pKbd->pHidDevice = pHidDevice;
        pKbd->KiiDeviceInfo.wLanguage = 0x0409; /* TODO */
        pKbd->KiiDeviceInfo.wVendorId = pHidDevice->pUsbDevice->DeviceDescriptor.idVendor;
        pKbd->KiiDeviceInfo.wProductId = pHidDevice->pUsbDevice->DeviceDescriptor.idProduct;
        pKbd->KiiDeviceInfo.bcdDevice = pHidDevice->pUsbDevice->DeviceDescriptor.bcdDevice;
        pKbd->KiiDeviceInfo.bBusAddress = pHidDevice->pUsbDevice->bBusAddress;
        pKbd->KiiDevice.pPrivateData = pKbd;
        pKbd->KiiDevice.pfSetLed = MGC_HidKeyboardSetLed;

        /* find suitable endpoint */
        for(bEnd = 0; bEnd < pIface->bNumEndpoints; bEnd++)
        {
            pEnd = MUSB_FindEndpointDescriptor(pConfig, pIface, bEnd);
            if(pEnd &&
                    (MUSB_ENDPOINT_XFER_INT == (pEnd->bmAttributes & MUSB_ENDPOINT_XFERTYPE_MASK)))
            {
                if(MUSB_ENDPOINT_DIR_MASK & pEnd->bEndpointAddress)
                {
                    pInEnd = pEnd;
                }
            }
        }
        /* bail if endpoint not found */
        if(!pInEnd)
        {
            MUSB_DIAG1(1, "HID: insufficient endpoints for keyboard in interface ",
                       pIface->bInterfaceNumber, 10, 0);
            MUSB_MemFree(pKbd);
            pKbd = NULL;
        }
        else
        {
            /* endpoint found; try to open pipe */
            end.pDevice = pHidDevice->pUsbDevice;
            MUSB_MemCopy(&(end.UsbDescriptor), pInEnd, sizeof(end.UsbDescriptor));
            end.wNakLimit = 0xffff;
            pKbd->Irp.hPipe = MUSB_OpenPipe(pHidDevice->hBus, &end, NULL);
            if(!pKbd->Irp.hPipe)
            {
                /* bail on open failure */
                MUSB_DIAG1(1, "HID: pipe open error on endpoint ",
                           pInEnd->bEndpointAddress, 10, 0);
                MUSB_MemFree(pKbd);
                pKbd = NULL;
            }
            else
            {
                dwStatus = MGC_HidKeyboardSetProtocol(pKbd);
                if (dwStatus)
                {
                    /* bail on transfer start error */
                    MUSB_DIAG1(3, "HID: MUSB_StartTransfer() status=", dwStatus, 16, 0);
                    MUSB_MemFree(pKbd);
                    pKbd = NULL;
                }
            }
        }
    }
    return pKbd;
}

void MGC_HidKeyboardDestroyInstance(void *pInstance)
{
    MGC_HidKeyboardData *pKbd = (MGC_HidKeyboardData *)pInstance;

    MUSB_ClosePipe(pKbd->Irp.hPipe);
    MUSB_KiiDeviceRemoved(&(pKbd->KiiHandle));
    MUSB_MemFree(pInstance);
}

/** Set boot protocol */
static uint32_t MGC_HidKeyboardSetProtocol(MGC_HidKeyboardData *pKbd)
{
    MUSB_DeviceRequest      *pSetup;
    MUSB_ControlIrp         *pControlIrp;
    uint32_t                 dwStatus;
    MGC_HidDevice *pHidDevice = pKbd->pHidDevice;

    pSetup          = &(pHidDevice->Setup);
    pControlIrp     = &(pHidDevice->ControlIrp);

    /** Prepare the Setup Packet for sending Request */
    pSetup->bmRequestType = (uint8_t)(MUSB_DIR_OUT | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE);
    pSetup->bRequest      = (uint8_t)MUSB_REQ_SET_PROTOCOL;
    pSetup->wValue        = (uint16_t)0;
    pSetup->wIndex        = (uint16_t)MUSB_SWAP16(pHidDevice->pIfaceDescriptor->bInterfaceNumber);
    pSetup->wLength       = (uint16_t)0;

    /** Fill Control Irp */
    pControlIrp->pDevice           = pHidDevice->pUsbDevice;
    pControlIrp->pOutBuffer        = (uint8_t *)pSetup;
    pControlIrp->dwOutLength       = sizeof(MUSB_DeviceRequest);
    pControlIrp->pInBuffer         = NULL;
    pControlIrp->dwInLength        = 0;
    pControlIrp->dwStatus          = 0;
    pControlIrp->dwActualOutLength = 0;
    pControlIrp->dwActualInLength  = 0;
    pControlIrp->pfIrpComplete     = MGC_HidSetProtocolCallback;
    pControlIrp->pCompleteParam    = (void *)pKbd;

    dwStatus = MUSB_StartControlTransfer (pHidDevice->pUsbDevice->pPort, pControlIrp);
    if (dwStatus)
    {
        /* Log an Error and return state */
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: Set Protocol Request failed, dwStatus: ", dwStatus, 16, FALSE);
    }
    return (dwStatus);
}

/** Callback function when device acknowledges set protocol request */
static uint32_t MGC_HidSetProtocolCallback(void *pContext, MUSB_ControlIrp *pControlIrp)
{
    uint32_t dwStatus;
    MGC_HidDevice            *pHidDevice;
    MGC_HidKeyboardData *pKbd = (MGC_HidKeyboardData *)pContext;

    pHidDevice = pKbd->pHidDevice;

    if (MUSB_STATUS_OK != pControlIrp->dwStatus)
    {
        MUSB_DIAG1(MUSB_HID_DIAG_ERROR,
                   "HID Error: Set Protocol Callback Status", pControlIrp->dwStatus, 10, FALSE);
        MUSB_RejectDevice(pHidDevice->hBus, pHidDevice->pUsbDevice);
        return FALSE;
    }

    /* try to start transfer */
    pKbd->Irp.pBuffer = pKbd->abReport;
    pKbd->Irp.dwLength = sizeof(pKbd->abReport);
    pKbd->Irp.pfIrpComplete = MGC_HidKeyboardIntrIrpCallback;
    pKbd->Irp.pCompleteParam = pKbd;
    pKbd->Irp.bAllowShortTransfer = FALSE;
    dwStatus = MUSB_StartTransfer(&(pKbd->Irp));
    if (dwStatus)
    {
        /* bail on transfer start error */
        MUSB_DIAG1(3, "HID: MUSB_StartTransfer() status=", dwStatus, 16, 0);
        MUSB_MemFree(pKbd);
        pKbd = NULL;
    }
    else
    {
        /* announce to KII implementation */
        if(MUSB_KII_SUCCESS !=
                MUSB_KiiAddDevice(&(pKbd->KiiHandle), &(pKbd->KiiDeviceInfo), &(pKbd->KiiDevice)))
        {
            /* bail on KII failure */
            MUSB_MemFree(pKbd);
            pKbd = NULL;
        }
    }

    return TRUE;
}

static uint32_t MGC_HidKeyboardIntrIrpCallback(void *pContext, MUSB_Irp *pIrp)
{
    uint8_t modNew, modOld, modChanged;
    uint32_t modMask;
    uint8_t bKey, bIndex, bFindIndex, bFound;
    uint8_t abFound[8];
    MGC_HidKeyboardData *pKbd = (MGC_HidKeyboardData *)pIrp->pCompleteParam;

    if(pKbd->abReport[0] != pKbd->abLastReport[0])
    {
        /* modifiers changed */
        modNew = pKbd->abReport[0];
        modOld = pKbd->bLastModifiers;
        pKbd->bLastModifiers = pKbd->abReport[0];
        modChanged = modNew ^ modOld;
        if(modChanged)
        {
            modMask = 0L;
            if(modNew & 0x01) modMask |= (1 << MUSB_KII_KEY_L_CONTROL);
            if(modNew & 0x02) modMask |= (1 << MUSB_KII_KEY_L_SHIFT);
            if(modNew & 0x04) modMask |= (1 << MUSB_KII_KEY_L_ALT);
            if(modNew & 0x88) modMask |= (1 << MUSB_KII_KEY_ALT_GRAPH);
            if(modNew & 0x10) modMask |= (1 << MUSB_KII_KEY_R_CONTROL);
            if(modNew & 0x20) modMask |= (1 << MUSB_KII_KEY_R_SHIFT);
            if(modNew & 0x40) modMask |= (1 << MUSB_KII_KEY_R_ALT);
            MUSB_KiiModifiersChanged(&(pKbd->KiiHandle), modMask);
        }
    }

    if(pKbd->abReport[1] >= 4)
    {
        /* "good" report; update lock states first */
        for(bIndex = 1; bIndex < 8; bIndex++)
        {
            bKey = pKbd->abReport[bIndex];
            switch(bKey)
            {
            case 0x39:
                pKbd->bCapsLock = pKbd->bCapsLock ? FALSE : TRUE;
                break;
            case 0x47:
                pKbd->bScrollLock = pKbd->bScrollLock ? FALSE : TRUE;
                break;
            case 0x53:
                pKbd->bNumLock = pKbd->bNumLock ? FALSE : TRUE;
                break;
            }
        }

        /* scan for keys, track changes, signal pressed or released */
        MUSB_MemSet(abFound, 0, sizeof(abFound));
        for(bIndex = 1; bIndex < 8; bIndex++)
        {
            bKey = pKbd->abReport[bIndex];
            if(bKey >= 4)
            {
                bFound = FALSE;
                for(bFindIndex = 1; bFindIndex < 8; bFindIndex++)
                {
                    if(pKbd->abLastReport[bFindIndex] == bKey)
                    {
                        bFound = TRUE;
                        abFound[bFindIndex] = TRUE;
                    }
                }
                /* if not in previous, report press */
                if(!bFound)
                {
                    MGC_HidReportKey(pKbd, bKey, TRUE);
                }
            }
        }

        /* report releases for all previous ones not found now */
        for(bIndex = 1; bIndex < 8; bIndex++)
        {
            bKey = pKbd->abReport[bIndex];
            if((bKey >= 4) && (!abFound[bIndex]))
            {
                MGC_HidReportKey(pKbd, bKey, FALSE);
            }
        }

        /* copy current into last for next time */
        MUSB_MemCopy(pKbd->abLastReport, pKbd->abReport, 8);
    }

    return TRUE;
}

static MUSB_KiiStatus MGC_HidKeyboardSetLed(void *pDeviceData,
        uint8_t bIndex, uint8_t bLight)
{
    /* TODO */
    return MUSB_KII_SUCCESS;
}
#endif // CFG_SUPPORT_HID
// eof

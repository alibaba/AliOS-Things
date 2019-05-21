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
 * MUSBStack-S mass-storage BOT Protocol implementation
 * $Revision: 1.39 $
 */
#include "include.h"

#include "mu_arch.h"
#include "mu_bot.h"
#include "mu_cdi.h"
#include "mu_diag.h"
#include "mu_hfi.h"
#include "mu_mem.h"
#include "mu_impl.h"

#include "mu_mpcsi.h"
#include "mu_mapi.h"

/****************************** CONSTANTS ********************************/

typedef enum
{
    MGC_MSD_BOT_RESET_STAGE_NONE,
    MGC_MSD_BOT_RESET_STAGE_CMD,
    MGC_MSD_BOT_RESET_STAGE_CLEARIN,
    MGC_MSD_BOT_RESET_STAGE_CLEAROUT
} MGC_MsdBotResetStage;

typedef enum
{
    MGC_MSD_BOT_TRANSFER_STAGE_NONE,
    MGC_MSD_BOT_TRANSFER_STAGE_CBW,
    MGC_MSD_BOT_TRANSFER_STAGE_DATAIN,
    MGC_MSD_BOT_TRANSFER_STAGE_DATAOUT,
    MGC_MSD_BOT_TRANSFER_STAGE_CSW
} MGC_MsdBotTransferStage;

#define MGC_BOT_CBW_TAG_INIT_VALUE   1
#define MGC_BOT_DATA_MAX_RETRIES	16

/******************************** TYPES **********************************/

struct _MGC_MsdBotVolume;

/**
 * BOT procotol data.
 * @field Protocol PCSI data
 * @field ControlIrp IRP for control transfers (GetMaxLUN and Reset)
 * @field aSetup setup data for control transfers
 * @field aData receive data from control transfers
 * @field pCmdSet command set to use
 * @field dwTag CBW tag
 * @field hInPipe bulk-in pipe handle
 * @field hOutPipe bulk-out pipe handle
 * @field Csw CSW (keep here since only one command can be outstanding)
 * @field CswIrp IRP for CSW
 * @field pCurrentVolume pointer to volume currently being serviced
 * @field aVolume array of volume data
 * @field bResetStage what stage of reset (if any) we are in
 * @field bInit TRUE during init
 * @field bLunCount number of LUNs
 */
typedef struct
{
    MGC_MsdProtocol Protocol;
    const void *pSelf;
    MUSB_ControlIrp ControlIrp;
    uint8_t aSetup[8];
    uint8_t aData[32];
    MUSB_Device *pDevice;
    MUSB_BusHandle hBus;
    const MUSB_InterfaceDescriptor *pIfaceDesc;
    const MUSB_EndpointDescriptor *pInEnd;
    const MUSB_EndpointDescriptor *pOutEnd;
    MGC_MsdCmdSet *pCmdSet;
    uint32_t dwTag;
    MUSB_PipePtr hInPipe;
    MUSB_PipePtr hOutPipe;
    MGC_MsdCsw Csw;
    MUSB_Irp CswIrp;
    struct _MGC_MsdBotVolume *pCurrentVolume;
    struct _MGC_MsdBotVolume *aVolume;
    MGC_MsdBotResetStage bResetStage;
    uint8_t bInit;
    uint8_t bLunCount;
    uint8_t bGetLang;
    uint8_t bGetSerial;
    uint8_t bCswRetry;
} MGC_MsdBotProtocol;

/**
 * BOT volume data.
 * A volume (logical unit).
 * @field pBot BOT data pointer
 * @field pCmdSetInstance instance of command set for volume
 * @field pfCmdComplete PCSI callback
 * @field pCompleteParam parameter for PCSI callback
 * @field Cbw CBW for volume
 * @field CbwIrp IRP for CBW
 * @field DataIrp IRP for data
 * @field DeviceInfo HFI device info
 * @field Device HFI device (dispatch table)
 * @field pfTransferComplete HFI transfer completion callback
 * @field hVolume HFI volume handle (callback parameter)
 * @field bTransferStage current transfer stage
 * @field bLun LUN
 * @field bIsReady TRUE if unit is ready; FALSE otherwise
 * @field bBusy TRUE if volume command in progress
 * @field bError TRUE if error on last/current command
 * @field bFormatProgress current format progress
 */
typedef struct _MGC_MsdBotVolume
{
    MGC_MsdBotProtocol *pBot;
    void *pCmdSetInstance;
    MGC_pfMsdProtocolCmdComplete pfCmdComplete;
    void *pCompleteParam;
    MGC_MsdCbw Cbw;
    MUSB_Irp CbwIrp;
    MUSB_Irp DataIrp;
    MUSB_HfiDeviceInfo DeviceInfo;
    MUSB_HfiMediumInfo MediumInfo;
    MUSB_HfiDevice Device;
    MUSB_pfHfiMountComplete pfMountComplete;
    MUSB_pfHfiTransferComplete pfHfiTransferComplete;
    MUSB_pfHfiMediumCheckComplete pfMediumCheckComplete;
    MUSB_HfiVolumeHandle hVolume;
    uint8_t *pbFormatSuccess;
    MGC_MsdBotTransferStage bTransferStage;
    uint8_t bLun;
    uint8_t bAnnounced;
    uint8_t bIsReady;
    uint8_t bBusy;
    uint8_t bError;
    uint8_t bGotMedium;
    uint8_t bFormatting;
    uint8_t bFormatProgress;
    uint8_t bMediumAnnounced;
    uint8_t bRetries;
} MGC_MsdBotVolume;

/******************************* FORWARDS ********************************/
#if CFG_SUPPORT_MSD
static uint8_t MGC_BotProtocolStartDevice(void *pProtocolData,
        MUSB_Device *pDevice);

static uint8_t MGC_BotProtocolStopDevice(void *pProtocolData,
        MUSB_Device *pDevice);

static uint8_t MGC_BotProtocolSendCmd(void *pProtocolData,
                                      void *pPrivateData,
                                      uint8_t bLun,
                                      const uint8_t *pCmdBuffer,
                                      uint8_t bCmdLength,
                                      uint8_t *pDataBuffer,
                                      uint32_t dwDataLength,
                                      uint8_t bSend,
                                      MGC_pfMsdProtocolCmdComplete pfCmdComplete,
                                      uint8_t bAllowDma,
                                      uint8_t bDelay);

static uint8_t MGC_BotProtocolSetDeviceInfo(void *pProtocolData,
        uint8_t bLun,
        const MUSB_HfiDeviceInfo *pInfo);

static uint8_t MGC_BotProtocolSetMediumInfo(void *pProtocolData,
        uint8_t bLun,
        const MUSB_HfiMediumInfo *pInfo);

static void MGC_BotProtocolSetReady(void *pProtocolData, uint8_t bLun, uint8_t bIsReady);

static void MGC_BotProtocolSetFormatProgress(void *pProtocolData, uint8_t bLun, uint8_t bProgress);

static uint32_t MGC_BotControlIrpComplete(void *pParam, MUSB_ControlIrp *pIrp);

static uint32_t MGC_BotCbwComplete(void *pParam, MUSB_Irp *pIrp);

static uint32_t MGC_BotCswComplete(void *pParam, MUSB_Irp *pIrp);

static uint32_t MGC_BotDataComplete(void *pParam, MUSB_Irp *pIrp);

static MUSB_HfiStatus MGC_MsdBotMountVolume(void *pDeviceData,
        MUSB_pfHfiMountComplete pfMountComplete);

static MUSB_HfiStatus MGC_MsdBotGetMediumInfo(void *pDeviceData,
        MUSB_HfiMediumInfo *pMediumInfo);

static MUSB_HfiStatus MGC_MsdBotReadDevice(void *pDeviceData,
        uint32_t dwStartBlockLo,
        uint32_t dwStartBlockHi,
        uint16_t wBlockCount,
        uint8_t *pBuffer,
        MUSB_pfHfiTransferComplete pfTransferComplete,
        uint8_t bAllowDma);

static MUSB_HfiStatus MGC_MsdBotWriteDevice(void *pDeviceData,
        uint32_t dwStartBlockLo,
        uint32_t dwStartBlockHi,
        uint16_t wBlockCount,
        uint8_t *pBuffer,
        uint8_t bVerify,
        MUSB_pfHfiTransferComplete pfTransferComplete,
        uint8_t bAllowDma);

static MUSB_HfiStatus MGC_MsdBotFlushDevice(void *pDeviceData);

static MUSB_HfiStatus MGC_MsdBotFormatMedium(void *pDeviceData,
        uint32_t dwBlockSize,
        uint8_t *pbSuccess);

static uint8_t MGC_MsdBotGetFormatProgress(void *pDeviceData);

static MUSB_HfiStatus MGC_MsdBotAbortFormat(void *pDeviceData);

static MUSB_HfiStatus MGC_MsdBotUnmountVolume(void *pDeviceData);

static MUSB_HfiStatus MGC_MsdBotCheckMedium(void *pDeviceData);

static MUSB_HfiStatus MGC_MsdBotCheckMediumNotify(void *pDeviceData,
        MUSB_pfHfiMediumCheckComplete pfMediumCheckComplete);

/******************************** GLOBALS ********************************/

static const uint8_t MGC_aMsdBotGetMaxLunData[] =
{
    (uint8_t)(MUSB_DIR_IN | MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE),
    MGC_MSD_BOT_GET_MAXLUN,
    0, 0,   /* wValue */
    0, 0,   /* wIndex=interface number (set in run-time copy) */
    1, 0,   /* wLength */
};

static const uint8_t MGC_aMsdBotResetData[] =
{
    (uint8_t)(MUSB_TYPE_CLASS | MUSB_RECIP_INTERFACE),
    MGC_MSD_BOT_RESET,
    0, 0,   /* wValue */
    0, 0,   /* wIndex=interface number (set in run-time copy) */
    1, 0,   /* wLength */
};

static const uint8_t MGC_aMsdBotClearHaltData[] =
{
    (uint8_t)MUSB_RECIP_ENDPOINT,
    MUSB_REQ_CLEAR_FEATURE,
    MUSB_FEATURE_ENDPOINT_HALT, 0,
    0, 0,   /* endpoint (set in run-time copy) */
    0, 0,   /* wLength */
};

/******************************** FUNCTIONS ******************************/
MGC_MsdProtocol *MGC_CreateBotInstance(MUSB_Device *pDevice,
                                       MUSB_BusHandle hBus,
                                       const MUSB_InterfaceDescriptor *pIfaceDesc,
                                       const MUSB_EndpointDescriptor *pInEnd,
                                       const MUSB_EndpointDescriptor *pOutEnd,
                                       MGC_MsdCmdSet *pCmdSet)
{
    MUSB_DeviceEndpoint InEnd;
    MUSB_DeviceEndpoint OutEnd;
    MUSB_PipePtr hInPipe;
    MUSB_PipePtr hOutPipe;
    MGC_MsdBotProtocol *pBot;

    InEnd.pDevice = OutEnd.pDevice = pDevice;
    switch(pDevice->ConnectionSpeed)
    {
    case MUSB_CONNECTION_SPEED_HIGH:
        InEnd.wNakLimit = 16;
        OutEnd.wNakLimit = 16;
        break;

    default:
        InEnd.wNakLimit = 255;
        OutEnd.wNakLimit = 255;
        break;
    }

    InEnd.wNakLimit = 0;
    OutEnd.wNakLimit = 0;
    MUSB_MemCopy(&(InEnd.UsbDescriptor), pInEnd, sizeof(MUSB_EndpointDescriptor));
    MUSB_MemCopy(&(OutEnd.UsbDescriptor), pOutEnd, sizeof(MUSB_EndpointDescriptor));
    hInPipe = MUSB_OpenPipe(hBus, &InEnd, NULL);
    if(!hInPipe)
    {
        MUSB_PRT("!!!!!!!!!!!MGC_CreateBotInstance fail !!!!!!!!!!!!!!!!!!\r\n");
        return NULL;
    }

    hOutPipe = MUSB_OpenPipe(hBus, &OutEnd, NULL);
    if(!hOutPipe)
    {
        MUSB_PRT("!!!!!!!!!!!MGC_CreateBotInstance fail !!!!!!!!!!!!!!!!!!\r\n");
        MUSB_ClosePipe(hInPipe);
        return NULL;
    }

    pBot = (MGC_MsdBotProtocol *)MUSB_MemAlloc(sizeof(MGC_MsdBotProtocol));
    if(pBot)
    {
        MUSB_MemSet(pBot, 0, sizeof(MGC_MsdBotProtocol));

        pBot->pSelf = pBot;
        pBot->pDevice = pDevice;
        pBot->hBus = hBus;
        pBot->pIfaceDesc = pIfaceDesc;
        pBot->pCmdSet = pCmdSet;
        pBot->pInEnd = pInEnd;
        pBot->pOutEnd = pOutEnd;
        pBot->hInPipe = hInPipe;
        pBot->hOutPipe = hOutPipe;

        /* set all immutable parts of protocol data and IRPs */
        pBot->CswIrp.hPipe = hInPipe;
        pBot->CswIrp.pBuffer = (uint8_t *) & (pBot->Csw);
        pBot->CswIrp.dwLength = 13;
        pBot->CswIrp.pfIrpComplete = MGC_BotCswComplete;
        pBot->CswIrp.pCompleteParam = pBot;

        /* fill PCSI-side dispatch table */
        pBot->Protocol.pProtocolData = pBot;
        pBot->Protocol.pfStartDevice = MGC_BotProtocolStartDevice;
        pBot->Protocol.pfStopDevice = MGC_BotProtocolStopDevice;
        pBot->Protocol.pfSendCmd = MGC_BotProtocolSendCmd;
        pBot->Protocol.pfSetDeviceInfo = MGC_BotProtocolSetDeviceInfo;
        pBot->Protocol.pfSetMediumInfo = MGC_BotProtocolSetMediumInfo;
        pBot->Protocol.pfSetReady = MGC_BotProtocolSetReady;
        pBot->Protocol.pfSetFormatProgress = MGC_BotProtocolSetFormatProgress;

        return &(pBot->Protocol);
    }

    MUSB_ClosePipe(hInPipe);
    MUSB_ClosePipe(hOutPipe);
    MUSB_PRT("MSD/BOT: failed to allocate instance data\r\n");

    return NULL;
}

void MGC_DestroyBotInstance(MGC_MsdProtocol *pProtocol)
{
    uint8_t bIndex;
    MGC_MsdBotVolume *pVolume;
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pProtocol->pProtocolData;

    MUSB_CancelControlTransfer(pBot->ControlIrp.pDevice->pPort,
                               &(pBot->ControlIrp));

    MUSB_ClosePipe(pBot->hOutPipe);
    pBot->hOutPipe = NULL;
    MUSB_ClosePipe(pBot->hInPipe);
    pBot->hInPipe = NULL;

    /* loop through volumes and inform HFI implementation of destruction */
    for(bIndex = 0; bIndex < pBot->bLunCount; bIndex++)
    {
        pVolume = &(pBot->aVolume[bIndex]);
        MUSB_HfiDeviceRemoved();
        pBot->pCmdSet->pfDestroyInstance(pVolume->pCmdSetInstance);
    }

    if(pBot->aVolume)
    {
        MUSB_MemFree(pBot->aVolume);
    }
    pBot->pSelf = NULL;
    MUSB_MemFree(pBot);
}

static uint8_t MGC_MsdBotReset(MGC_MsdBotProtocol *pBot)
{
    uint32_t dwStatus;

    if(!pBot || (pBot != pBot->pSelf)) return FALSE;

    MUSB_DIAG_STRING(1, "MSD/BOT: performing BOT reset");

    pBot->bResetStage = MGC_MSD_BOT_RESET_STAGE_CMD;
    MUSB_MemCopy(pBot->aSetup, MGC_aMsdBotResetData,
                 sizeof(MGC_aMsdBotResetData));
    pBot->aSetup[4] = pBot->pIfaceDesc->bInterfaceNumber;
    pBot->ControlIrp.pInBuffer = NULL;
    pBot->ControlIrp.dwInLength = 0;
    dwStatus = MUSB_StartControlTransfer(pBot->ControlIrp.pDevice->pPort,
                                         &(pBot->ControlIrp));

    return (MUSB_STATUS_OK == dwStatus) ? TRUE : FALSE;
}

static uint8_t MGC_BotProtocolStartDevice(void *pProtocolData,
        MUSB_Device *pDevice)
{
    uint32_t dwStatus;
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pProtocolData;

    if(!pBot || (pBot != pBot->pSelf)) return FALSE;

    MUSB_PRT("start get max lun\r\n");
    MUSB_MemCopy(pBot->aSetup, MGC_aMsdBotGetMaxLunData,
                 sizeof(MGC_aMsdBotGetMaxLunData));
    pBot->aSetup[4] = pBot->pIfaceDesc->bInterfaceNumber;
    pBot->ControlIrp.pDevice = pDevice;
    pBot->ControlIrp.pOutBuffer = pBot->aSetup;
    pBot->ControlIrp.dwOutLength = 8;
    pBot->ControlIrp.pInBuffer = pBot->aData;
    pBot->ControlIrp.dwInLength = 1;
    pBot->ControlIrp.pfIrpComplete = MGC_BotControlIrpComplete;
    pBot->ControlIrp.pCompleteParam = pBot;
    pBot->bInit = TRUE;
    dwStatus = MUSB_StartControlTransfer(pDevice->pPort, &(pBot->ControlIrp));

    return (MUSB_STATUS_OK == dwStatus) ? TRUE : FALSE;
}

static uint8_t MGC_BotProtocolStopDevice(void *pProtocolData,
        MUSB_Device *pDevice)
{
    /* nothing to do */
    return TRUE;
}

static uint8_t MGC_BotProtocolSendCmd(void *pProtocolData,
                                      void *pPrivateData,
                                      uint8_t bLun,
                                      const uint8_t *pCmdBuffer,
                                      uint8_t bCmdLength,
                                      uint8_t *pDataBuffer,
                                      uint32_t dwDataLength,
                                      uint8_t bSend,
                                      MGC_pfMsdProtocolCmdComplete pfCmdComplete,
                                      uint8_t bAllowDma,
                                      uint8_t bDelay)
{
    uint32_t dwStatus;
    MGC_MsdBotVolume *pVolume;
    uint8_t bCmdSize = bCmdLength > 16 ? 16 : bCmdLength;
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pProtocolData;

    if(!pBot || (pBot != pBot->pSelf)) return FALSE;

    pVolume = &(pBot->aVolume[bLun]);
    if(!pVolume->bBusy)
    {
        pVolume->bBusy          = TRUE;
        pVolume->bError         = FALSE;
        pVolume->bTransferStage = MGC_MSD_BOT_TRANSFER_STAGE_CBW;

        pVolume->Cbw.dCbwTag = MUSB_SWAP32(++(pBot->dwTag));
        pVolume->Cbw.dCbwDataTransferLength = MUSB_SWAP32(dwDataLength);
        pVolume->Cbw.bmCbwFlags   = bSend ? MGC_MSD_BOT_DIR_OUT : MGC_MSD_BOT_DIR_IN;
        pVolume->Cbw.bCbwLun      = bLun;
        pVolume->Cbw.bCbwCbLength = bCmdSize;

        MUSB_MemCopy(pVolume->Cbw.aCbwCb, pCmdBuffer, bCmdSize);
        pVolume->CbwIrp.pCompleteParam = pVolume;
        pVolume->pfCmdComplete         = pfCmdComplete;
        pVolume->pCompleteParam        = pPrivateData;
        pVolume->DataIrp.pBuffer       = pDataBuffer;
        pVolume->DataIrp.dwLength      = dwDataLength;
        pVolume->CbwIrp.bAllowDma      = FALSE;
        pVolume->DataIrp.bAllowDma     = bAllowDma;

        pBot->CswIrp.bAllowDma = FALSE;
        pBot->pCurrentVolume = pVolume;

        dwStatus = MUSB_StartTransfer(&(pVolume->CbwIrp));
        if(MUSB_STATUS_OK == dwStatus)
        {
            return TRUE;
        }

    }
    return FALSE;
}

static uint8_t MGC_BotProtocolSetDeviceInfo(void *pProtocolData,
        uint8_t bLun,
        const MUSB_HfiDeviceInfo *pInfo)
{
    uint8_t bIndex, bCount;
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pProtocolData;
    MGC_MsdBotVolume *pVolume = &(pBot->aVolume[bLun]);

    if(!pBot || (pBot != pBot->pSelf)) return FALSE;

    /* copy info */
    MUSB_MemCopy(&(pVolume->DeviceInfo), pInfo, sizeof(MUSB_HfiDeviceInfo));

    /* copy USB device info */
    pVolume->DeviceInfo.wVendorId = pBot->pDevice->DeviceDescriptor.idVendor;
    pVolume->DeviceInfo.wProductId = pBot->pDevice->DeviceDescriptor.idProduct;
    pVolume->DeviceInfo.bcdDevice = pBot->pDevice->DeviceDescriptor.bcdDevice;
    pVolume->DeviceInfo.bBusAddress = pBot->pDevice->bBusAddress;

    /* serial # from the USB string descriptor, since we cannot rely on SCSI inquiry */
    MUSB_MemSet(pVolume->DeviceInfo.awSerialNumber, 0, MUSB_HFI_MAX_VOLUME_SERIAL << 1);
    bCount = MUSB_MIN(MUSB_HFI_MAX_VOLUME_SERIAL, (pBot->aData[0] >> 1));
    for(bIndex = 0; bIndex < bCount; bIndex++)
    {
        pVolume->DeviceInfo.awSerialNumber[bIndex] =
            MUSB_SWAP16P(&(pBot->aData[2 + (bIndex << 1)]));
    }
    pVolume->DeviceInfo.awSerialNumber[bIndex] = 0;

    MUSB_PRT("pvolume->bAnnounced = %x,bLun=%x,pbot->bLuncount=%x\r\n", pVolume->bAnnounced, bLun, pBot->bLunCount);
    if(pVolume->pCmdSetInstance && !pVolume->bAnnounced && (bLun < (pBot->bLunCount - 1)))
    {
        pBot->pCmdSet->pfDiscoverDevice(
            pVolume->pCmdSetInstance, &(pBot->Protocol),
            bLun + 1);
    }
    return TRUE;
}

static uint8_t MGC_BotProtocolSetMediumInfo(void *pProtocolData,
        uint8_t bLun,
        const MUSB_HfiMediumInfo *pInfo)
{
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pProtocolData;
    MGC_MsdBotVolume *pVolume = &(pBot->aVolume[bLun]);

    if(!pBot || (pBot != pBot->pSelf)) return FALSE;

    pVolume->bGotMedium = TRUE;
    MUSB_MemCopy(&(pVolume->MediumInfo), pInfo, sizeof(MUSB_HfiMediumInfo));
    return TRUE;
}

static void MGC_BotProtocolSetReady(void *pProtocolData,
                                    uint8_t bLun,
                                    uint8_t bIsReady)
{
    MUSB_HfiStatus Status;
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pProtocolData;
    MGC_MsdBotVolume *pVolume = &(pBot->aVolume[bLun]);

    if(!pBot || (pBot != pBot->pSelf)) return;

    pVolume->bIsReady = bIsReady;

    if(bIsReady)
    {
        /* ready */
        if(pVolume->bFormatting)
        {
            /* was formatting, so must be format completion */
            pVolume->bFormatting = FALSE;
            if(pVolume->pbFormatSuccess)
            {
                *(pVolume->pbFormatSuccess) = TRUE;
            }
        }
        else if(!pVolume->bAnnounced)
        {
            /* ready and volume not yet announced */
            pVolume->bAnnounced = TRUE;
            MUSB_PRT("[MUSB]HfiAddDevice fun\r\n");
            Status = MUSB_HfiAddDevice(&(pVolume->hVolume), &(pVolume->DeviceInfo),
                                       &(pVolume->Device));
            if(MUSB_HFI_SUCCESS != Status)
            {
                MUSB_DIAG_STRING(1, "MSD/BOT: HfiAddDevice failed");
                pVolume->bIsReady = FALSE;
            }
            /* if we got medium info, announce medium also */
            MUSB_PRT("[MUSB]bGotMedium = %x +++\r\n", pVolume->bGotMedium);
            if(pVolume->bGotMedium)
            {
                MUSB_PRT("[MUSB]HfiMediumInserted fun\r\n");
                pVolume->bMediumAnnounced = TRUE;
                MUSB_HfiMediumInserted(pVolume->hVolume, &(pVolume->MediumInfo));
            }
        }
        else if(pVolume->bAnnounced && pVolume->bGotMedium && !pVolume->bMediumAnnounced)
        {
            /* already announced and ready, must be medium inserted */
            pVolume->bMediumAnnounced = TRUE;
            MUSB_HfiMediumInserted(pVolume->hVolume, &(pVolume->MediumInfo));
        }
    }
    else if(!pVolume->bFormatting)
    {
        /* not ready and not formatting */
        if(pVolume->bAnnounced)
        {
            /* already announced and not ready, so must be medium removal */
            if(pVolume->bMediumAnnounced)
            {
                MUSB_HfiMediumRemoved(pVolume->hVolume);
            }
            pVolume->bGotMedium = FALSE;
            pVolume->bMediumAnnounced = FALSE;
        }
    }
    if(pVolume->pfMediumCheckComplete)
    {
        pVolume->pfMediumCheckComplete(pVolume->hVolume);
        pVolume->pfMediumCheckComplete = NULL;
    }
}

static void MGC_BotProtocolSetFormatProgress(void *pProtocolData,
        uint8_t bLun,
        uint8_t bProgress)
{
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pProtocolData;
    MGC_MsdBotVolume *pVolume = &(pBot->aVolume[bLun]);

    if(!pBot || (pBot != pBot->pSelf)) return;

    pVolume->bFormatProgress = bProgress;
}

static void MGC_MsdBotVolumeInit(MGC_MsdBotVolume *pVolume)
{
    /* set all immutable parts of protocol data and IRPs */
    pVolume->Cbw.dCbwSignature = MUSB_SWAP32(MGC_MSD_BOT_CBW_SIGNATURE);
    pVolume->Cbw.dCbwTag = MUSB_SWAP32(MGC_BOT_CBW_TAG_INIT_VALUE);
    pVolume->CbwIrp.hPipe = pVolume->pBot->hOutPipe;
    pVolume->CbwIrp.pBuffer = (uint8_t *) & (pVolume->Cbw);
    pVolume->CbwIrp.dwLength = 31;
    pVolume->CbwIrp.pfIrpComplete = MGC_BotCbwComplete;
    pVolume->CbwIrp.pCompleteParam = pVolume;

    pVolume->DataIrp.pfIrpComplete = MGC_BotDataComplete;
    pVolume->DataIrp.pCompleteParam = pVolume;

    /* fill HFI-side dispatch table */
    pVolume->Device.pPrivateData = pVolume;
    pVolume->Device.pfMountVolume = MGC_MsdBotMountVolume;
    pVolume->Device.pfGetMediumInfo = MGC_MsdBotGetMediumInfo;
    pVolume->Device.pfReadDevice = MGC_MsdBotReadDevice;
    pVolume->Device.pfWriteDevice = MGC_MsdBotWriteDevice;
    pVolume->Device.pfFlushDevice = MGC_MsdBotFlushDevice;
    pVolume->Device.pfFormatMedium = MGC_MsdBotFormatMedium;
    pVolume->Device.pfGetFormatProgress = MGC_MsdBotGetFormatProgress;
    pVolume->Device.pfAbortFormat = MGC_MsdBotAbortFormat;
    pVolume->Device.pfUnmountVolume = MGC_MsdBotUnmountVolume;
    pVolume->Device.pfCheckMedium = MGC_MsdBotCheckMedium;
    pVolume->Device.pfCheckMediumNotify = MGC_MsdBotCheckMediumNotify;
}

static uint32_t MGC_BotControlIrpComplete(void *pParam, MUSB_ControlIrp *pIrp)
{
    uint8_t bIndex;
    MGC_MsdBotVolume *pVolume = NULL;
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pParam;
    uint32_t dwStatus = pIrp->dwStatus;

    if(!pBot || (pBot != pBot->pSelf))
        return 1;

    if(pBot->bInit)
    {
        /* we must be initializing, so it is GetMaxLUN */
        pBot->bInit = FALSE;
        pBot->bLunCount = 1;
        pBot->aVolume = (MGC_MsdBotVolume *)MUSB_MemAlloc(
                            pBot->bLunCount * sizeof(MGC_MsdBotVolume));
        if(pBot->aVolume)
        {
            MUSB_MemSet(pBot->aVolume, 0,
                        pBot->bLunCount * sizeof(MGC_MsdBotVolume));
            for(bIndex = 0; bIndex < pBot->bLunCount; bIndex++)
            {
                pVolume = &(pBot->aVolume[bIndex]);
                //即函数 : MGC_ScsiCmdSetCreateInstance()
                pVolume->pCmdSetInstance = pBot->pCmdSet->pfCreateInstance(pBot->bLunCount);
                if(pVolume->pCmdSetInstance)
                {
                    pVolume->pBot = pBot;
                    pVolume->bLun = bIndex;
                    MGC_MsdBotVolumeInit(pVolume);
                }
            }
            if(pVolume != NULL)
            {
                if(pVolume->pCmdSetInstance)
                {
                    /* kick off command-set-specific device discovery for first LUN */
                    // MGC_ScsiCmdSetDiscoverDevice()，发送cmd12命令
                    pBot->pCmdSet->pfDiscoverDevice(
                        pVolume->pCmdSetInstance, &(pBot->Protocol), 0);
                    //发送完成后,会产生中断,在BSR中则会调用 pVolume->CbwIrp.pfIrpComplete = MGC_BotCbwComplete;
                }
            }
        }
    }
    else if(pBot->bCswRetry)
    {
        /* data was stalled, so try CSW after clearing */
        MUSB_FlushPipe(pBot->hInPipe);
        MUSB_StartTransfer(&(pBot->CswIrp));
    }
    else
    {
        /* must be reset */
        switch(pBot->bResetStage)
        {
        case MGC_MSD_BOT_RESET_STAGE_CMD:
            /* start clear-in stage */
            pBot->bResetStage = MGC_MSD_BOT_RESET_STAGE_CLEARIN;
            MUSB_MemCopy(pBot->aSetup, MGC_aMsdBotClearHaltData,
                         sizeof(MGC_aMsdBotClearHaltData));
            pBot->aSetup[4] = pBot->pInEnd->bEndpointAddress;
            pBot->ControlIrp.pInBuffer = NULL;
            pBot->ControlIrp.dwInLength = 0;
            dwStatus = MUSB_StartControlTransfer(
                           pBot->ControlIrp.pDevice->pPort,
                           &(pBot->ControlIrp));
            break;

        case MGC_MSD_BOT_RESET_STAGE_CLEARIN:
            /* start clear-out stage */
            pBot->bResetStage = MGC_MSD_BOT_RESET_STAGE_CLEAROUT;
            MUSB_MemCopy(pBot->aSetup, MGC_aMsdBotClearHaltData,
                         sizeof(MGC_aMsdBotClearHaltData));
            pBot->aSetup[4] = pBot->pOutEnd->bEndpointAddress;
            pBot->ControlIrp.pInBuffer = NULL;
            pBot->ControlIrp.dwInLength = 0;
            dwStatus = MUSB_StartControlTransfer(
                           pBot->ControlIrp.pDevice->pPort,
                           &(pBot->ControlIrp));
            break;

        case MGC_MSD_BOT_RESET_STAGE_CLEAROUT:
            pBot->bResetStage = MGC_MSD_BOT_RESET_STAGE_NONE;
            MUSB_FlushPipe(pBot->hInPipe);
            MUSB_FlushPipe(pBot->hOutPipe);
            pBot->pCurrentVolume->bBusy = FALSE;

            /* call completion handler if present */
            if(pBot->pCurrentVolume->pfCmdComplete)
            {
                switch(pBot->pCurrentVolume->bTransferStage)
                {
                case MGC_MSD_BOT_TRANSFER_STAGE_DATAIN:
                case MGC_MSD_BOT_TRANSFER_STAGE_DATAOUT:
                    pBot->pCurrentVolume->pfCmdComplete(
                        pBot->pCurrentVolume->pCompleteParam,
                        pBot->pCurrentVolume->DataIrp.pBuffer,
                        pBot->pCurrentVolume->DataIrp.dwActualLength,
                        TRUE, 1, NULL, 0);
                    break;

                case MGC_MSD_BOT_TRANSFER_STAGE_CSW:
                    pBot->pCurrentVolume->pfCmdComplete(
                        pBot->pCurrentVolume->pCompleteParam,
                        pBot->pCurrentVolume->DataIrp.pBuffer,
                        pBot->pCurrentVolume->DataIrp.dwActualLength,
                        TRUE, 1, NULL, 0);
                    break;

                default:
                    pBot->pCurrentVolume->pfCmdComplete(
                        pBot->pCurrentVolume->pCompleteParam,
                        NULL, 0, TRUE, 1, NULL, 0);
                }
            }
            break;

        default:
            break;
        }	/* END: switch on reset stage */
        if(MUSB_STATUS_OK != dwStatus)
        {
            MUSB_DIAG_STRING(1, "MSD/BOT: reset command submit failed; probably fatal");
        }
    }
    return 0;
}

static uint32_t MGC_BotCbwComplete(void *pParam, MUSB_Irp *pIrp)
{
    uint32_t dwStatus, RetVal = 1;
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pParam;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf))
        return RetVal;
    /* handle any USB error */
    if(pIrp->dwStatus)
    {
        pVolume->bError = TRUE;
        MUSB_DIAG1(1, "MSD/BOT: USB error on CBW transfer: ", pIrp->dwStatus, 16, 0);
        MGC_MsdBotReset(pBot);
        return RetVal;
    }

    /* by virtue of a volume's CBW getting sent, it becomes current */
    pBot->pCurrentVolume = pVolume;

    if(pVolume->Cbw.dCbwDataTransferLength)
    {
        /* data, so start it */
        pVolume->DataIrp.dwStatus = 0;
        if(pVolume->Cbw.bmCbwFlags & MGC_MSD_BOT_DIR_IN)
        {
            pVolume->bTransferStage = MGC_MSD_BOT_TRANSFER_STAGE_DATAIN;
            pVolume->DataIrp.hPipe = pBot->hInPipe;
            pVolume->DataIrp.bAllowShortTransfer = FALSE;
        }
        else
        {
            pVolume->bTransferStage = MGC_MSD_BOT_TRANSFER_STAGE_DATAOUT;
            pVolume->DataIrp.hPipe = pBot->hOutPipe;
            pVolume->DataIrp.bAllowShortTransfer = FALSE;
        }
        dwStatus = MUSB_StartTransfer(&(pVolume->DataIrp));
    }
    else
    {
        /* no data, so get status */
        dwStatus = MUSB_StartTransfer(&(pBot->CswIrp));
    }
    dwStatus++;//for warning
    RetVal = 0;
    return RetVal;
}

static uint32_t MGC_BotDataComplete(void *pParam, MUSB_Irp *pIrp)
{
    uint32_t dwStatus, RetVal = 1;
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pParam;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf)) return RetVal;
    /* handle any USB error */
    if(pIrp->dwStatus)
    {
        pVolume->bError = TRUE;
        if(MUSB_STATUS_STALLED == pIrp->dwStatus)
        {
            /* STALL; initiate recovery */
            pBot->bCswRetry = TRUE;
            RetVal = MUSB_STATUS_STALLED;
            if(pVolume->Cbw.bmCbwFlags & MGC_MSD_BOT_DIR_IN)
            {
                MUSB_MemCopy(pBot->aSetup, MGC_aMsdBotClearHaltData,
                             sizeof(MGC_aMsdBotClearHaltData));
                pBot->aSetup[4] = pBot->pInEnd->bEndpointAddress;
                pBot->ControlIrp.pInBuffer = NULL;
                pBot->ControlIrp.dwInLength = 0;
                dwStatus = MUSB_StartControlTransfer(
                               pBot->ControlIrp.pDevice->pPort,
                               &(pBot->ControlIrp));
            }
            else
            {
                MUSB_MemCopy(pBot->aSetup, MGC_aMsdBotClearHaltData,
                             sizeof(MGC_aMsdBotClearHaltData));
                pBot->aSetup[4] = pBot->pOutEnd->bEndpointAddress;
                pBot->ControlIrp.pInBuffer = NULL;
                pBot->ControlIrp.dwInLength = 0;
                dwStatus = MUSB_StartControlTransfer(
                               pBot->ControlIrp.pDevice->pPort,
                               &(pBot->ControlIrp));
            }
            return RetVal;
        }
        else
        {
            /* other error, retry or give up */
            if(pVolume->bRetries++ < MGC_BOT_DATA_MAX_RETRIES)
            {
                MUSB_StartTransfer(&(pVolume->DataIrp));
            }
            else if(pVolume->pfCmdComplete)
            {
                pVolume->pfCmdComplete(pVolume->pCompleteParam,
                                       pVolume->DataIrp.pBuffer,
                                       pVolume->DataIrp.dwActualLength, TRUE,
                                       pBot->Csw.bCswStatus, NULL, 0);
            }
            return RetVal;
        }
    }

    pVolume->bRetries = 0;
    RetVal = 0;
    /* if it looks like a CSW, maybe it is a CSW */
    if((pVolume->bTransferStage == MGC_MSD_BOT_TRANSFER_STAGE_DATAIN) &&
            (13 == pIrp->dwActualLength))
    {
        MUSB_MemCopy(pBot->CswIrp.pBuffer, pIrp->pBuffer, 13);
        MGC_BotCswComplete(pParam, pIrp);
        return RetVal;
    }

    /* get status */
    pVolume->bTransferStage = MGC_MSD_BOT_TRANSFER_STAGE_CSW;
    dwStatus = MUSB_StartTransfer(&(pBot->CswIrp));
    dwStatus++;//for warning
    return RetVal;
}

static uint32_t MGC_BotCswComplete(void *pParam, MUSB_Irp *pIrp)
{
    uint32_t dwStatus, RetVal;
    MGC_MsdBotProtocol *pBot = (MGC_MsdBotProtocol *)pParam;
    MGC_MsdBotVolume *pVolume = pBot->pCurrentVolume;
    RetVal = 1;

    if(!pBot || (pBot != pBot->pSelf))
        return RetVal;
    pVolume->bTransferStage = MGC_MSD_BOT_TRANSFER_STAGE_NONE;

    /* handle any USB error */
    if(pIrp->dwStatus)
    {
        pVolume->bError = TRUE;
        if(!pBot->bCswRetry)
        {
            /* not retried yet */
            pBot->bCswRetry = TRUE;
            if(MUSB_STATUS_STALLED == pIrp->dwStatus)
            {
                RetVal = MUSB_STATUS_STALLED;
                /* STALL; clear it and then retry */
                MUSB_MemCopy(pBot->aSetup, MGC_aMsdBotClearHaltData,
                             sizeof(MGC_aMsdBotClearHaltData));
                pBot->aSetup[4] = pBot->pInEnd->bEndpointAddress;
                pBot->ControlIrp.pInBuffer = NULL;
                pBot->ControlIrp.dwInLength = 0;
                dwStatus = MUSB_StartControlTransfer(
                               pBot->ControlIrp.pDevice->pPort,
                               &(pBot->ControlIrp));
            }
            else
            {
                /* other error, just retry */
                pVolume->bTransferStage = MGC_MSD_BOT_TRANSFER_STAGE_CSW;
                dwStatus = MUSB_StartTransfer(&(pBot->CswIrp));
            }
            return RetVal;
        }
        MUSB_DIAG1(1, "MSD/BOT: USB error on CSW: ", pIrp->dwStatus, 16, 0);
        pBot->bCswRetry = FALSE;
        MGC_MsdBotReset(pBot);
        return RetVal;
    }

    if(13 != pIrp->dwActualLength)
    {
        pBot->bCswRetry = FALSE;
        pVolume->bError = TRUE;
        MUSB_DIAG1(1, "MSD/BOT: CSW length was ", pIrp->dwActualLength, 16, 0);
        /* due to buggy devices, the safest thing is a reset */
        MGC_MsdBotReset(pBot);
        return RetVal;
    }

    /* check dwTag */
    if(pBot->Csw.dCswTag != pVolume->Cbw.dCbwTag)
    {
        if(pBot->bCswRetry)
        {
            pVolume->bTransferStage = MGC_MSD_BOT_TRANSFER_STAGE_CSW;
            dwStatus = MUSB_StartTransfer(&(pBot->CswIrp));
        }
        else
        {
            pVolume->bError = TRUE;
            MUSB_DIAG2(1, "MSD/BOT: sequencing error; sent tag ",
                       pVolume->Cbw.dCbwTag, " got ", pBot->Csw.dCswTag, 16, 8);
            /* due to buggy devices, the safest thing is a reset */
            MGC_MsdBotReset(pBot);
        }
        pBot->bCswRetry = FALSE;
        return RetVal;
    }

    /* check for explicitly-signalled phase error */
    if(2 == pBot->Csw.bCswStatus)
    {
        pBot->bCswRetry = FALSE;
        pVolume->bError = TRUE;
        MUSB_DIAG_STRING(1, "MSD/BOT: phase error");
        MGC_MsdBotReset(pBot);
        return RetVal;
    }

    dwStatus++;
    pBot->bCswRetry = FALSE;
    pVolume->bBusy = FALSE;

    /* call completion handler if present */
    if(pVolume->pfCmdComplete)
    {
        return pVolume->pfCmdComplete(pVolume->pCompleteParam,
                                      pVolume->DataIrp.pBuffer,
                                      pVolume->DataIrp.dwActualLength, pVolume->bError,
                                      pBot->Csw.bCswStatus, NULL, 0);
    }
    return 0;
}

static void MGC_MsdBotMountComplete(void *pPrivateData, uint8_t bSuccess)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pPrivateData;

    pVolume->pfMountComplete(pVolume->hVolume, bSuccess);
}

/**************************************************************************
HFI functions
**************************************************************************/
static MUSB_HfiStatus MGC_MsdBotMountVolume(void *pDeviceData,
        MUSB_pfHfiMountComplete pfMountComplete)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf) || !pVolume->bIsReady)
    {
        return MUSB_HFI_ERROR_MEDIA_REMOVED;
    }
    if(pBot->pCmdSet->pfMountDevice)
    {
        pVolume->pfMountComplete = pfMountComplete;
        if(!pBot->pCmdSet->pfMountDevice(pVolume->pCmdSetInstance,
                                         MGC_MsdBotMountComplete, pVolume, &(pBot->Protocol), pVolume->bLun))
        {
            return MUSB_HFI_ERROR_TRANSFER;
        }
    }
    else
    {
        pfMountComplete(pVolume->hVolume, TRUE);
    }
    return MUSB_HFI_SUCCESS;
}

static MUSB_HfiStatus MGC_MsdBotGetMediumInfo(void *pDeviceData,
        MUSB_HfiMediumInfo *pMediumInfo)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;

    MUSB_MemCopy(pMediumInfo, &(pVolume->MediumInfo),
                 sizeof(MUSB_HfiMediumInfo));

    return MUSB_HFI_SUCCESS;
}

static uint32_t MGC_MsdBotDataTransferComplete(void *pPrivateData,
        const uint8_t *pDataBuffer,
        uint32_t dwDataLength,
        uint8_t bUsbError,
        uint8_t bWrapperStatus,
        const uint8_t *pStatusBuffer,
        uint16_t wStatusLength)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pPrivateData;
    MUSB_pfHfiTransferComplete pfTransferComplete = pVolume->pfHfiTransferComplete;

    /* TODO: replace divide with shift */
    return pfTransferComplete(pVolume->hVolume,
                              (uint16_t)(dwDataLength / pVolume->DeviceInfo.dwBlockSize));

}

static MUSB_HfiStatus MGC_MsdBotReadDevice(void *pDeviceData,
        uint32_t dwStartBlockLo,
        uint32_t dwStartBlockHi,
        uint16_t wBlockCount,
        uint8_t *pBuffer,
        MUSB_pfHfiTransferComplete pfTransferComplete,
        uint8_t bAllowDma)
{
    uint8_t bOk;
    uint8_t bCmdLength;
    uint8_t aCmdBuffer[16];
    uint32_t dwBlockSize;
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf) || !pVolume->bIsReady)
    {
        return MUSB_HFI_ERROR_MEDIA_REMOVED;
    }
    bCmdLength = pBot->pCmdSet->pfGetReadCmd(pVolume->pCmdSetInstance,
                 aCmdBuffer, 16, dwStartBlockLo, dwStartBlockHi, wBlockCount,
                 pVolume->bLun);
    if(bCmdLength)
    {
        pVolume->pfHfiTransferComplete = pfTransferComplete;
        dwBlockSize = pVolume->MediumInfo.dwBlockSize;
        if(!dwBlockSize) dwBlockSize = pVolume->DeviceInfo.dwBlockSize;
        bOk = MGC_BotProtocolSendCmd(pBot, pVolume, pVolume->bLun,
                                     aCmdBuffer, bCmdLength, pBuffer,
                                     wBlockCount * dwBlockSize,
                                     FALSE, MGC_MsdBotDataTransferComplete,
                                     bAllowDma, 0);
        return bOk ? MUSB_HFI_SUCCESS : MUSB_HFI_ERROR_TRANSFER;
    }

    return MUSB_HFI_NO_MEMORY;
}

static MUSB_HfiStatus MGC_MsdBotWriteDevice(void *pDeviceData,
        uint32_t dwStartBlockLo,
        uint32_t dwStartBlockHi,
        uint16_t wBlockCount,
        uint8_t *pBuffer,
        uint8_t bVerify,
        MUSB_pfHfiTransferComplete pfTransferComplete,
        uint8_t bAllowDma)
{
    uint8_t bOk;
    uint8_t bCmdLength;
    uint8_t aCmdBuffer[16];
    uint32_t dwBlockSize;
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    /* TODO: support bVerify */
    if(bVerify)
    {
        return MUSB_HFI_ERROR_VERIFY;
    }
    if(!pBot || (pBot != pBot->pSelf) || !pVolume->bIsReady)
    {
        return MUSB_HFI_ERROR_MEDIA_REMOVED;
    }

    bCmdLength = pBot->pCmdSet->pfGetWriteCmd(pVolume->pCmdSetInstance,
                 aCmdBuffer, 16, dwStartBlockLo, dwStartBlockHi, wBlockCount,
                 pVolume->bLun);
    if(bCmdLength)
    {
        pVolume->pfHfiTransferComplete = pfTransferComplete;
        dwBlockSize = pVolume->MediumInfo.dwBlockSize;
        if(!dwBlockSize) dwBlockSize = pVolume->DeviceInfo.dwBlockSize;
        bOk = MGC_BotProtocolSendCmd(pBot, pVolume, pVolume->bLun, aCmdBuffer,
                                     bCmdLength, pBuffer, wBlockCount * dwBlockSize,
                                     TRUE, MGC_MsdBotDataTransferComplete, bAllowDma, 0);
        return bOk ? MUSB_HFI_SUCCESS : MUSB_HFI_ERROR_TRANSFER;
    }
    return MUSB_HFI_NO_MEMORY;
}

static MUSB_HfiStatus MGC_MsdBotFlushDevice(void *pDeviceData)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf) || !pVolume->bIsReady)
    {
        return MUSB_HFI_ERROR_MEDIA_REMOVED;
    }
    if(pBot->pCmdSet->pfFlushDevice)
    {
        if(!pBot->pCmdSet->pfFlushDevice(pVolume->pCmdSetInstance,
                                         &(pBot->Protocol), pVolume->bLun))
        {
            return MUSB_HFI_ERROR_TRANSFER;
        }
    }
    return MUSB_HFI_SUCCESS;
}

static MUSB_HfiStatus MGC_MsdBotFormatMedium(void *pDeviceData,
        uint32_t dwBlockSize,
        uint8_t *pbSuccess)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf) || !pVolume->bIsReady)
    {
        return MUSB_HFI_ERROR_MEDIA_REMOVED;
    }
    if(pVolume->DeviceInfo.bCanFormat &&
            pBot->pCmdSet->pfFormatDevice(pVolume->pCmdSetInstance,
                                          &(pBot->Protocol), dwBlockSize, pVolume->bLun))
    {
        pVolume->bFormatting = TRUE;
        pVolume->pbFormatSuccess = pbSuccess;
        return MUSB_HFI_SUCCESS;
    }
    return MUSB_HFI_ERROR_UNSUPPORTED_BLOCKSIZE;
}

static uint8_t MGC_MsdBotGetFormatProgress(void *pDeviceData)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;

    return pVolume->bFormatProgress;
}

static MUSB_HfiStatus MGC_MsdBotAbortFormat(void *pDeviceData)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf)) return MUSB_HFI_SUCCESS;

    if(pBot->pCmdSet->pfAbortFormat(pVolume->pCmdSetInstance,
                                    &(pBot->Protocol), pVolume->bLun))
    {
        pVolume->bFormatting = FALSE;
        return MUSB_HFI_SUCCESS;
    }
    return MUSB_HFI_ERROR_UNSUPPORTED_BLOCKSIZE;
}

static MUSB_HfiStatus MGC_MsdBotUnmountVolume(void *pDeviceData)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf) || !pVolume->bIsReady)
    {
        return MUSB_HFI_ERROR_MEDIA_REMOVED;
    }
    if(pBot->pCmdSet->pfUnmountDevice)
    {
        if(!pBot->pCmdSet->pfUnmountDevice(pVolume->pCmdSetInstance,
                                           &(pBot->Protocol), pVolume->bLun))
        {
            return MUSB_HFI_ERROR_TRANSFER;
        }
    }
    return MUSB_HFI_SUCCESS;
}

static MUSB_HfiStatus MGC_MsdBotCheckMedium(void *pDeviceData)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf)) return MUSB_HFI_ERROR_TRANSFER;

    pVolume->pfMediumCheckComplete = NULL;
    if(pBot->pCmdSet->pfCheckMedium)
    {
        if(!pBot->pCmdSet->pfCheckMedium(pVolume->pCmdSetInstance,
                                         &(pBot->Protocol), pVolume->bLun))
        {
            return MUSB_HFI_ERROR_TRANSFER;
        }
    }
    return MUSB_HFI_SUCCESS;
}

static MUSB_HfiStatus MGC_MsdBotCheckMediumNotify(void *pDeviceData,
        MUSB_pfHfiMediumCheckComplete pfMediumCheckComplete)
{
    MGC_MsdBotVolume *pVolume = (MGC_MsdBotVolume *)pDeviceData;
    MGC_MsdBotProtocol *pBot = pVolume->pBot;

    if(!pBot || (pBot != pBot->pSelf)) return MUSB_HFI_ERROR_TRANSFER;

    pVolume->pfMediumCheckComplete = pfMediumCheckComplete;
    if(pBot->pCmdSet->pfCheckMedium)
    {
        if(!pBot->pCmdSet->pfCheckMedium(pVolume->pCmdSetInstance,
                                         &(pBot->Protocol), pVolume->bLun))
        {
            return MUSB_HFI_ERROR_TRANSFER;
        }
    }
    return MUSB_HFI_SUCCESS;
}
#endif // CFG_SUPPORT_MSD
// eof

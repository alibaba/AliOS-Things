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
#include "class/mu_msd.h"
#include "mu_mapi.h"
#include "mu_none.h"
#include "usb_msd.h"
#include "rtos_pub.h"

#if CFG_SUPPORT_MSD
extern MUSB_FunctionClient MGC_xxxFunctionClient;
static void MGC_MsdNewOtgState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_OtgState State);
static void MGC_MsdOtgError(void *hClient, MUSB_BusHandle hBus,
                            uint32_t dwStatus);

/**************************** GLOBALS *****************************/
static MUSB_Port *MGC_pCdiPort = NULL;
static MUSB_BusHandle MGC_hCdiBus = NULL;
static uint8_t MGC_bDesireHostRole = TRUE;
static uint8_t MGC_aMsdPeripheralList[256];
static MUSB_DeviceDriver MGC_aDeviceDriver[2];

static MUSB_HostClient MGC_MsdHostClient =
{
    MGC_aMsdPeripheralList,		/* peripheral list */
    0,			    /* filled in main */
    MGC_aDeviceDriver,
    0					/* filled in main */
};

static MUSB_OtgClient MGC_MsdOtgClient =
{
    NULL,	/* no instance data; we are singleton */
    &MGC_bDesireHostRole,
    MGC_MsdNewOtgState,
    MGC_MsdOtgError
};

static MUSB_HfiDevice *MGC_pHfiDevice = NULL;
static const MUSB_HfiMediumInfo *MGC_pHfiMediumInfo = NULL;
static uint8_t MediaIsOk = FALSE;
FUNCPTR trx_callback_ptr = NULL;

#ifdef CFG_ENABLE_SYC_OP
beken_semaphore_t msd_rd_sema = NULL;
beken_semaphore_t msd_wr_sema = NULL;
#endif

/*************************** FUNCTIONS ****************************/
/* OTG client */
static void MGC_MsdNewOtgState(void *hClient, MUSB_BusHandle hBus,
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

static void MGC_MsdOtgError(void *hClient, MUSB_BusHandle hBus,
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
    MGC_pHfiMediumInfo = pMediumInfo;
}

void MUSB_HfiMediumRemoved(MUSB_HfiVolumeHandle hVolume)
{
    MGC_pHfiMediumInfo = NULL;
}

void MUSB_HfiDeviceRemoved(void)
{
    MGC_pHfiDevice = NULL;
    MediaIsOk = FALSE;
}

uint8_t MGC_MsdGetMediumstatus(void)
{
    return MediaIsOk;
}

void MGC_StartupAppEvent(void)
{
    if(trx_callback_ptr)
    {
        (*trx_callback_ptr)();
    }
}

void MGC_RegisterCBTransferComplete(FUNCPTR func)
{
    trx_callback_ptr = func;
}

uint32_t MGC_MsdTransferComplete(MUSB_HfiVolumeHandle hVolume,
                                 uint16_t wActualBlocks)

{
    if(trx_callback_ptr)
    {
        (*trx_callback_ptr)();
    }
    MUSB_PRT("MGC_MsdTransferComplete:0x%x\r\n", trx_callback_ptr);

    return 2;
}

uint32_t get_HfiMedium_size(void)
{
    if(MGC_pHfiMediumInfo)
        return MGC_pHfiMediumInfo->dwBlockCountLo;
    else
        return 0;
}
uint32_t get_HfiMedium_blksize(void)
{
    if(MGC_pHfiMediumInfo)
        return MGC_pHfiMediumInfo->dwBlockSize;
    else
        return 0;
}

uint32_t MUSB_HfiRead( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice && MGC_pHfiDevice->pfReadDevice)
    {
        RetValue = MGC_pHfiDevice->pfReadDevice(MGC_pHfiDevice->pPrivateData,
                                                first_block, 0, block_num, dest,
                                                MGC_MsdTransferComplete, TRUE);
    }

    return RetValue;
}

uint32_t MUSB_HfiWrite( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice)
    {
        MUSB_PRT("pfWriteDevice0\r\n");
        RetValue = MGC_pHfiDevice->pfWriteDevice(MGC_pHfiDevice->pPrivateData,
                   first_block, 0, block_num, dest,
                   0, MGC_MsdTransferComplete, FALSE);
        MUSB_PRT("pfWriteDevice1\r\n");
    }

    return RetValue;
}

#ifdef CFG_ENABLE_SYC_OP
uint32_t MGC_MsdReadSyncInit(void)
{
    OSStatus ret = kNoErr;

    if(NULL == msd_rd_sema)
    {
        ret = rtos_init_semaphore(&msd_rd_sema, 1);
        if (kNoErr != ret)
        {
            MUSB_PRT("MGC_MsdReadSyncInit failed\r\n");
        }
    }

    return ret;
}

uint32_t MGC_MsdWriteSyncInit(void)
{
    OSStatus ret = kNoErr;

    if(NULL == msd_wr_sema)
    {
        ret = rtos_init_semaphore(&msd_wr_sema, 1);
        if (kNoErr != ret)
        {
            MUSB_PRT("MGC_MsdReadSyncInit failed\r\n");
        }
    }

    return ret;
}

OSStatus MGC_MsdReadSyncWaiting(uint32_t timeout)
{
    return rtos_get_semaphore(&msd_rd_sema, timeout);
}

OSStatus MGC_MsdWriteSyncWaiting(uint32_t timeout)
{
    return rtos_get_semaphore(&msd_wr_sema, timeout);
}


uint32_t MGC_MsdReadComplete(MUSB_HfiVolumeHandle hVolume,
                             uint16_t wActualBlocks)

{
    MUSB_PRT("MGC_MsdReadComplete\r\n");
    rtos_set_semaphore(&msd_rd_sema);

    return 0;
}

uint32_t MGC_MsdWriteComplete(MUSB_HfiVolumeHandle hVolume,
                              uint16_t wActualBlocks)

{
    MUSB_PRT("MGC_MsdWriteComplete\r\n");
    rtos_set_semaphore(&msd_wr_sema);

    return 0;
}

uint32_t MUSB_HfiRead_sync( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice && MGC_pHfiDevice->pfReadDevice)
    {
        RetValue = MGC_pHfiDevice->pfReadDevice(MGC_pHfiDevice->pPrivateData,
                                                first_block, 0, block_num, dest,
                                                MGC_MsdReadComplete, TRUE);
        MGC_MsdReadSyncWaiting(BEKEN_WAIT_FOREVER);
    }

    return RetValue;
}

uint32_t MUSB_HfiWrite_sync( uint32_t first_block, uint32_t block_num, uint8_t *dest)
{
    uint32_t RetValue = 1;

    if(MGC_pHfiDevice)
    {
        RetValue = MGC_pHfiDevice->pfWriteDevice(MGC_pHfiDevice->pPrivateData,
                   first_block, 0, block_num, dest,
                   0, MGC_MsdWriteComplete, FALSE);
        MGC_MsdWriteSyncWaiting(BEKEN_WAIT_FOREVER);
    }

    return RetValue;
}
#endif // CFG_ENABLE_SYC_OP

int usb_sw_init(void)
{
    uint8_t *pList;
    uint16_t ret;
    uint8_t bDriver = 0;
    uint16_t wCount = 0;
    uint16_t wSize = 0;
    uint16_t wRemain;
    MUSB_DeviceDriver *pDriver;

#ifdef CFG_ENABLE_SYC_OP
    ret = MGC_MsdReadSyncInit();
    ASSERT(0 == ret);

    ret = MGC_MsdWriteSyncInit();
    ASSERT(0 == ret);
#endif

    wRemain = (uint16_t)sizeof(MGC_aMsdPeripheralList);
    pList = MGC_aMsdPeripheralList;

    wSize = MUSB_FillMsdPeripheralList(bDriver, pList, wRemain);
    if(wSize < wRemain)
    {
        pDriver = MUSB_GetStorageClassDriver();
        if(pDriver)
        {
            MUSB_MemCopy(&(MGC_MsdHostClient.aDeviceDriverList[bDriver]),
                         pDriver,
                         sizeof(MUSB_DeviceDriver));

            pList += wSize;
            wCount += wSize;
            wRemain -= wSize;

            bDriver++;
        }
    }

    MGC_MsdHostClient.wPeripheralListLength = wCount;
    MGC_MsdHostClient.bDeviceDriverListLength = bDriver;

    if(!MUSB_InitSystem(5))
    {
        MUSB_PRT("[MGC] InitSystem failed\r\n");
        return -1;
    }

    /* find first CDI port */
    MGC_pCdiPort = MUSB_GetPort(0);

    /* start session */
    MGC_hCdiBus = MUSB_RegisterOtgClient(MGC_pCdiPort,
                                         &MGC_xxxFunctionClient,
                                         &MGC_MsdHostClient,
                                         &MGC_MsdOtgClient);

    MUSB_NoneRunBackground();

    return 0;
}

int usb_sw_uninit(void)
{
    return 0;
}
#endif // CFG_SUPPORT_MSD

// eof


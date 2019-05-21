/*****************************************************************************
 *                                                                           *
 *      Copyright Mentor Graphics Corporation 2003-2006                      *
 *                                                                           *
 *                All Rights Reserved.                                       *
 *                                                                           *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION            *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS              *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                               *
 *                                                                           *
 ****************************************************************************/

/*
 * SCSI client functions (driver interfacing, command transfer, and
 * command preparation)
 * $Revision: 1.25 $
 */
#include "include.h"

#include "mu_cdi.h"
#include "mu_mem.h"
#include "mu_impl.h"
#include "mu_descs.h"
#include "mu_stdio.h"
#include "mu_diag.h"
#include "mu_hfi.h"
#include "mu_scsi.h"
#include "mu_mpcsi.h"

/****************************** CONSTANTS ********************************/

/* how many time to retry TEST_UNIT_READY: */
#define MGC_MSD_SCSI_TEST_UNIT_RETRIES	100

/* general retry count: */
#define MGC_MSD_SCSI_RETRIES	100

/* how many times to retry READ_CAPACITY if it fails but READ(10) of block 0 passes: */
#define MGC_MSD_CAPACITY_RETRIES	400

/* how many times to retry READ_FMT_CAPACITY: */
#define MGC_MSD_FMT_CAPACITY_RETRIES	2

typedef enum
{
    MGC_SCSI_STATE_NONE,
    /* device discovery sequence: */
    MGC_SCSI_STATE_INQUIRY,
    MGC_SCSI_STATE_READ_FMT_CAPACITY,
    MGC_SCSI_STATE_READ_CAPACITY,
    MGC_SCSI_STATE_FIRST_TEST,
    MGC_SCSI_STATE_READ_BLOCK0,
    MGC_SCSI_STATE_MODE_SENSE_EXCEPTIONS,
    MGC_SCSI_STATE_MODE_SENSE_ALL,
    MGC_SCSI_STATE_TEST_UNIT_READY,
    /* mount sequence: */
    MGC_SCSI_STATE_PREVENT_MEDIUM_REMOVE,
    /* unmount sequence: */
    MGC_SCSI_STATE_ALLOW_MEDIUM_REMOVE,
    MGC_SCSI_STATE_STOP_UNIT
} MGC_ScsiState;

/******************************** TYPES **********************************/

typedef struct
{
    MGC_MsdProtocol *pProtocol;
    MUSB_pfHfiMountComplete pfMountComplete;
    void *pMountCompleteParam;
    uint8_t aBlock0[512];
    uint8_t aCmd[16];
    uint8_t aInquiryData[36];
    uint8_t aSenseData[20];
    uint8_t aFormatCapacityData[0xfc];
    uint8_t aCapacityData[8];
    uint8_t aData[64];
    uint32_t dwRetries;
    uint32_t dwCapacityRetries;
    uint8_t bFmtCapacityRetries;
    MGC_ScsiState bState;
    uint8_t bError;
    uint8_t bRemovable;
    uint8_t bAnnounced;
    uint8_t bLun;
    uint8_t bLunCount;
    uint8_t bLunScan;
    uint8_t bLunIndex;
    uint8_t bNoMedium;
} MGC_ScsiCmdSetData;

/******************************* FORWARDS ********************************/
#if CFG_SUPPORT_MSD
extern void MGC_StartupAppEvent(void);
static void *MGC_ScsiCmdSetCreateInstance(uint8_t bLunCount);
static void MGC_ScsiCmdSetDestroyInstance(void *pInstance);
static uint8_t MGC_ScsiCmdSetDiscoverDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetMountDevice(void *pInstance,
        MGC_pfMsdMountComplete pfMountComplete,
        void *pPrivateData,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetUnmountDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetGetReadCmd(void *pInstance,
                                        uint8_t *pCmdBuffer,
                                        uint8_t bMaxCmdLength,
                                        uint32_t dwBlockLo,
                                        uint32_t dwBlockHi,
                                        uint16_t wBlockCount,
                                        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetGetWriteCmd(void *pInstance,
        uint8_t *pCmdBuffer,
        uint8_t bMaxCmdLength,
        uint32_t dwBlockLo,
        uint32_t dwBlockHi,
        uint16_t wBlockCount,
        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetFlushDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetFormatDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint32_t dwBlockSize,
        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetAbortFormat(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);
static void MGC_ScsiCmdSetParseInterrupt(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        const uint8_t *pMessage,
        uint16_t wMessageLength,
        uint8_t bLun);
static uint8_t MGC_ScsiCmdSetCheckMedium(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun);

/******************************* GLOBALS *********************************/
static MGC_MsdCmdSet MGC_ScsiCmdSet =
{
    MGC_ScsiCmdSetCreateInstance,
    MGC_ScsiCmdSetDestroyInstance,
    MGC_ScsiCmdSetDiscoverDevice,
    MGC_ScsiCmdSetMountDevice,
    MGC_ScsiCmdSetUnmountDevice,
    MGC_ScsiCmdSetGetReadCmd,
    MGC_ScsiCmdSetGetWriteCmd,
    MGC_ScsiCmdSetFlushDevice,
    MGC_ScsiCmdSetFormatDevice,
    MGC_ScsiCmdSetAbortFormat,
    MGC_ScsiCmdSetParseInterrupt,
    MGC_ScsiCmdSetCheckMedium
};

/****************************** FUNCTIONS ********************************/
MGC_MsdCmdSet *MGC_GetScsiCmdSet()
{
    return &MGC_ScsiCmdSet;
}

static void *MGC_ScsiCmdSetCreateInstance(uint8_t bLunCount)
{
    MGC_ScsiCmdSetData *pScsi = (MGC_ScsiCmdSetData *)MUSB_MemAlloc(
                                    sizeof(MGC_ScsiCmdSetData));
    if(pScsi)
    {
        MUSB_MemSet(pScsi, 0, sizeof(MGC_ScsiCmdSetData));
        pScsi->bLunCount = bLunCount;
    }
    return pScsi;
}

static void MGC_ScsiCmdSetDestroyInstance(void *pInstance)
{
    MUSB_MemFree(pInstance);
}

uint32_t MGC_ScsiCmdComplete(void *pPrivateData,
                             const uint8_t *pDataBuffer,
                             uint32_t dwDataLength,
                             uint8_t bUsbError,
                             uint8_t bWrapperStatus,
                             const uint8_t *pStatusBuffer,
                             uint16_t wStatusLength)
{
    MUSB_HfiDeviceInfo DeviceInfo;
    MUSB_HfiMediumInfo MediumInfo;
    MUSB_HfiAccessType AccessType = MUSB_HFI_ACCESS_RANDOM;
    uint8_t bIndex, bCount;
    uint32_t dwBlockCount;
    const MGC_MsdStandardInquiryData *pInquiry = NULL;
    uint8_t bSend = FALSE;
    uint8_t bMediumChange = FALSE;
    uint8_t bAnnounce = FALSE;
    uint8_t bOpcode = 0;
    uint8_t bCmdLength = 0;
    int dwLength = 0;
    uint8_t *pBuffer = NULL;
    uint8_t bSuccess = FALSE;
    uint8_t bReady = FALSE;
    MGC_ScsiCmdSetData *pScsi = (MGC_ScsiCmdSetData *)pPrivateData;
    uint8_t bError = pScsi->bError;
    MGC_MsdProtocol *pProtocol = pScsi->pProtocol;
    uint8_t bLun = pScsi->bLun;
    uint8_t bRet = 0;
    /* prepare for next command */
    MUSB_MemSet(pScsi->aCmd, 0, 16);

    /*
     * Tevion 6-in-1 card reader problem (fails READ_FORMAT_CAPACITY for connected LUN):
     * Someone added a hack for Linux to work with it.  Add to unusual_devs.h:
     *   UNUSUAL_DEV(  0x0483, 0x1307, 0x0000, 0x9999, "Tevion", "Card Reader 6in1", US_SC_SCSI, US_PR_TEV6IN1, NULL, 0),
     * and to usb.c:
     *   case US_PR_TEV6IN1:
     *   ss->transport_name = "Bulk";
     *   ss->transport = usb_stor_Bulk_transport;
     *   ss->transport_reset = usb_stor_Bulk_reset;
     *   ss->max_lun = 3;
     *   break;
     * That is, if VID=0x0483, PID=0x1307, and 0<=bcdDevice<=0x9999,
     * then set max LUN to 3 (presumably skips asking the device).
     * But, this has never made it into the official kernel...
     *
     * Windows works fine.  It always issues GET_MAX_LUN.  It then does, for each LUN:
     * 1. INQUIRY
     * 2. READ_FMT_CAPACITY (trying up to 3 times on errors, ultimately taking error to mean no medium)
     * Then, for each LUN which seems to have media:
     * 1. READ_CAPACITY
     * 2. READ(10) block 0
     * 3. MODE_SENSE page 1A
     * 4. MODE_SENSE page 3F
     * 5. READ_CAPACITY
     * 6. READ(10) block 0
     * 7. three READ_CAPACITY's
     * 8. two READ(10) block 0
     * 9. two READ_CAPACITY's
     * - it just goes on and on, with different repeats of the READ_CAPACITY and READ(10)
     */

    if(pScsi->bError)
    {
        pScsi->bError = FALSE;
        if(!bWrapperStatus)
        {
            /* analyze sense codes */
            if(MGC_SCSI_SC_NOT_READY == (pScsi->aSenseData[2] & 0xf) &&
                    (MGC_SCSI_ASC_UNIT_NOT_READY == pScsi->aSenseData[12]))
            {
                pScsi->bRemovable = TRUE;
                if(2 == pScsi->aSenseData[13])
                {
                    /* needs a start unit */
                    bCmdLength = 6;
                    bOpcode = MGC_SCSI_START_STOP_UNIT;
                    pScsi->aCmd[4] = 3;
                }
                else
                {
                    bAnnounce = TRUE;
                }
            }
            else if(MGC_SCSI_SC_UNIT_ATTENTION == (pScsi->aSenseData[2] & 0xf) &&
                    (MGC_SCSI_ASC_MEDIUM_CHANGE == pScsi->aSenseData[12]))
            {
                bMediumChange = TRUE;
            }
            else if(MGC_SCSI_SC_UNIT_ATTENTION == (pScsi->aSenseData[2] & 0xf) &&
                    ((MGC_SCSI_ASC_MEDIUM_NOT_PRESENT == pScsi->aSenseData[12]) ||
                     (MGC_SCSI_ASC_MEDIUM_ERROR == pScsi->aSenseData[12])))
            {
                pScsi->bNoMedium = TRUE;
            }
        }
    }
    if(bWrapperStatus)
    {
        /* command failed: find out why */
        pScsi->bError = TRUE;

        /* if the device failed a read, circumvent the retry loop */
        if(pScsi->bState == MGC_SCSI_STATE_READ_BLOCK0)
        {
            pScsi->dwCapacityRetries = MGC_MSD_CAPACITY_RETRIES;
        }

        /* send a REQUEST_SENSE */
        bCmdLength = 6;
        bOpcode = MGC_SCSI_REQUEST_SENSE;
        pBuffer = pScsi->aSenseData;
        dwLength = 20;
        pScsi->aCmd[0] = bOpcode;
        pScsi->aCmd[4] = (uint8_t)(dwLength & 0xff);
        bSuccess = pProtocol->pfSendCmd(pProtocol->pProtocolData, pScsi, bLun,
                                        pScsi->aCmd, bCmdLength, pBuffer, dwLength, FALSE,
                                        MGC_ScsiCmdComplete, FALSE, 2);
        return 0;
    }

    /* process result of last action and setup next one */
    switch(pScsi->bState)
    {
    case MGC_SCSI_STATE_INQUIRY:
        MUSB_PRT("******SCSI_0x12 Standard Inquiry complete***\r\n\r\n");

        /* read format capacity */
        bCmdLength = 10;
        bOpcode = MGC_SCSI_RD_FMT_CAPC;
        pBuffer = pScsi->aFormatCapacityData;
        dwLength = sizeof(pScsi->aFormatCapacityData);
        pScsi->bState = MGC_SCSI_STATE_READ_FMT_CAPACITY;
        /* KLUDGE for certain devices */
        if(pScsi->bLunScan)
        {
            bLun = pScsi->bLunIndex;
        }
        break;

    case MGC_SCSI_STATE_READ_FMT_CAPACITY:
        MUSB_PRT("MSD: Read Format Capacity complete\r\n");

        /* if error, re-zero data for check later */
        if(bError || bUsbError)
        {
            MUSB_DIAG_STRING(2, "MSD: Read Format Capacity failed");
            MUSB_MemSet(pScsi->aFormatCapacityData, 0,
                        sizeof(pScsi->aFormatCapacityData));
            pScsi->bError = FALSE;
        }
        /* retry this command if needed, up to given count */
        if((bError || bUsbError) && (pScsi->bFmtCapacityRetries < MGC_MSD_FMT_CAPACITY_RETRIES))
        {
            pScsi->bFmtCapacityRetries++;
            /* read format capacity again */
            bCmdLength = 10;
            bOpcode = MGC_SCSI_RD_FMT_CAPC;
            pBuffer = pScsi->aFormatCapacityData;
            dwLength = sizeof(pScsi->aFormatCapacityData);
            pScsi->bState = MGC_SCSI_STATE_READ_FMT_CAPACITY;
        }
        else if(pScsi->bLunScan)
        {
            /* KLUDGE for certain devices: do INQUIRY/READ_FMT_CAP on each LUN, then back to LUN 0 */
            pScsi->bState = MGC_SCSI_STATE_INQUIRY;
            bCmdLength = 6;
            bOpcode = MGC_SCSI_INQUIRY;
            pBuffer = pScsi->aInquiryData;
            dwLength = 36;
            if(pScsi->bLunIndex < (pScsi->bLunCount - 1))
            {
                bLun = ++pScsi->bLunIndex;
            }
            else
            {
                pScsi->bLunScan = FALSE;
            }
        }
        else
        {
            /* the normal next step: read capacity */
            bCmdLength = 10;
            bOpcode = MGC_SCSI_READ_CAPACITY;
            pBuffer = pScsi->aCapacityData;
            dwLength = sizeof(pScsi->aCapacityData);
            pScsi->bState = MGC_SCSI_STATE_READ_CAPACITY;
        }
        break;

    case MGC_SCSI_STATE_READ_CAPACITY:
        MUSB_PRT("MSD: Read Capacity complete\r\n");

        /* if error, re-zero data for check later */
        if(bError || bUsbError)
        {
            MUSB_DIAG_STRING(2, "MSD: Read Capacity failed");
            MUSB_MemSet(pScsi->aCapacityData, 0,
                        sizeof(pScsi->aCapacityData));
        }
        if(bMediumChange)
        {
            /* if medium changed, re-read capacity */
            bCmdLength = 16;
            bOpcode = MGC_SCSI_READ_CAPACITY;
            pBuffer = pScsi->aCapacityData;
            dwLength = sizeof(pScsi->aCapacityData);
            pScsi->bState = MGC_SCSI_STATE_READ_CAPACITY;
        }
        else if(pScsi->bNoMedium || bUsbError)
        {
            /* skip read if definitive no-medium or stall from device */
            bCmdLength = 6;
            bOpcode = MGC_SCSI_TEST_UNIT_READY;
            pScsi->bState = MGC_SCSI_STATE_TEST_UNIT_READY;
        }
        else
        {
            /* test unit ready */
            bCmdLength = 6;
            bOpcode = MGC_SCSI_TEST_UNIT_READY;
            pScsi->bState = MGC_SCSI_STATE_FIRST_TEST;
        }
        break;

    case MGC_SCSI_STATE_FIRST_TEST:
        MUSB_PRT("[SCSI]bError=%d,bUsbError=%dr\n", bError, bUsbError);
        if(bError || bUsbError)
        {
            /* retry if needed */
            if(pScsi->dwRetries < MGC_MSD_SCSI_TEST_UNIT_RETRIES)
            {
                pScsi->dwRetries++;
                pScsi->bState = MGC_SCSI_STATE_FIRST_TEST;
            }
            else
            {
                /* retry count exhausted; skip read of block 0 */
                pScsi->dwRetries = 0;
                pScsi->bState = MGC_SCSI_STATE_TEST_UNIT_READY;
            }
            bCmdLength = 6;
            bOpcode = MGC_SCSI_TEST_UNIT_READY;
        }
        else if(pScsi->aCapacityData[0] || pScsi->aCapacityData[1] ||
                pScsi->aCapacityData[2] || pScsi->aCapacityData[3])
        {
            ///此处表示已U盘已经成功可以读取
            /* if ready and we got good capacity data, we're done */
            bAnnounce = TRUE;
            bReady = TRUE;
            pScsi->bState = MGC_SCSI_STATE_TEST_UNIT_READY;
        }
        else
        {
            //表示U盘的容量为0,异常?????

            /* read block 0, as another way to check for medium present */
            pScsi->dwRetries = 0;
            bCmdLength = 16;
            bOpcode = MGC_SCSI_READ10;
            pBuffer = pScsi->aBlock0;
            /* fool code below */
            dwLength = 0;
            MGC_ScsiCmdSetGetReadCmd(pScsi, pScsi->aCmd, 16, 0, 0, 1, bLun);
            pScsi->bState = MGC_SCSI_STATE_READ_BLOCK0;
        }
        break;

    case MGC_SCSI_STATE_READ_BLOCK0:
        MUSB_PRT("MSD: Read block 0 complete\r\n");
        if(bError)
        {
            pScsi->bRemovable = TRUE;
        }
        else if((pScsi->dwCapacityRetries < MGC_MSD_CAPACITY_RETRIES) &&
                !pScsi->aCapacityData[0] && !pScsi->aCapacityData[1] &&
                !pScsi->aCapacityData[2] && !pScsi->aCapacityData[3])
        {
            /* if read capacity failed but read passed, try read capacity again */
            pScsi->dwCapacityRetries++;
            bCmdLength = 10;
            bOpcode = MGC_SCSI_READ_CAPACITY;
            pBuffer = pScsi->aCapacityData;
            dwLength = sizeof(pScsi->aCapacityData);
            pScsi->bState = MGC_SCSI_STATE_READ_CAPACITY;
            break;
        }
        /* test unit ready */
        bCmdLength = 6;
        bOpcode = MGC_SCSI_TEST_UNIT_READY;
        pScsi->bState = MGC_SCSI_STATE_TEST_UNIT_READY;
        break;

    case MGC_SCSI_STATE_TEST_UNIT_READY:
        MUSB_PRT("MSD: Test Unit Ready complete\r\n");
        /* if ready, done; otherwise try again */
        if(!bError)
        {
            bAnnounce = TRUE;
            bReady = pScsi->bNoMedium ? FALSE : TRUE;
        }
        else if(pScsi->dwRetries > MGC_MSD_SCSI_TEST_UNIT_RETRIES)
        {
            /* retries exhausted; assume removable medium */
            pScsi->dwRetries = 0;
            pScsi->bRemovable = TRUE;
            bAnnounce = TRUE;
        }
        else
        {
            pScsi->dwRetries++;
            bCmdLength = 6;
            bOpcode = MGC_SCSI_TEST_UNIT_READY;
        }
        break;

    case MGC_SCSI_STATE_PREVENT_MEDIUM_REMOVE:
        MUSB_PRT("[MGC] SCSI_STATE_PREVENT_MEDIUM_REMOVE\r\n");
        /* inform client */
        pScsi->pfMountComplete(pScsi->pMountCompleteParam, TRUE);
        return 0;

    case MGC_SCSI_STATE_ALLOW_MEDIUM_REMOVE:
        MUSB_PRT("[MGC] SCSI_STATE_ALLOW_MEDIUM_REMOVE\r\n");
        /* stop unit */
        bCmdLength = 6;
        bOpcode = MGC_SCSI_START_STOP_UNIT;
        pScsi->bState = MGC_SCSI_STATE_STOP_UNIT;
        break;

    case MGC_SCSI_STATE_STOP_UNIT:
        MUSB_PRT("[MGC] SCSI_STATE_STOP_UNIT\r\n");
        /* done */
        return 0;

    default:
        MUSB_PRT("MSD/SCSI: Internal error; bad state:%d\r\n", pScsi->bState);
        return 0;
    }

    /* set common fields */
    pScsi->aCmd[0] = bOpcode;
    switch(bCmdLength)
    {
    case 6:
        pScsi->aCmd[4] = (uint8_t)(dwLength & 0xff);
        break;
    case 10:
        pScsi->aCmd[8] = (uint8_t)(dwLength & 0xff);
        break;
    }

    if(bAnnounce)
    {
        /* ready to announce volume */

        /* fill DeviceInfo */
        MUSB_MemSet(&DeviceInfo, 0, sizeof(DeviceInfo));
        switch(pScsi->aInquiryData[0] & 0x1f)
        {
        case MGC_SCSI_DEVICE_TYPE_DIRECT:
        case MGC_SCSI_DEVICE_TYPE_RBC:
            AccessType = MUSB_HFI_ACCESS_RANDOM;
            DeviceInfo.bmAccessType = 1 << AccessType;
            /* believe the RMB bit (this is a problem on some devices!) */
            pScsi->bRemovable = (pScsi->aInquiryData[1] & MGC_M_MSD_INQUIRY_RMB) ? TRUE : FALSE;
            break;
        case MGC_SCSI_DEVICE_TYPE_CARDREADER:
            AccessType = MUSB_HFI_ACCESS_RANDOM;
            DeviceInfo.bmAccessType = 1 << AccessType;
            /* correct removability in case we didn't infer it right */
            pScsi->bRemovable = TRUE;
            break;
        case MGC_SCSI_DEVICE_TYPE_SEQUENTIAL:
        case MGC_SCSI_DEVICE_TYPE_WORM:
        case MGC_SCSI_DEVICE_TYPE_OPTICAL:
            AccessType = MUSB_HFI_ACCESS_RANDOM_WRITE_ONCE;
            DeviceInfo.bmAccessType = 1 << AccessType;
            /* correct removability in case we didn't infer it right */
            pScsi->bRemovable = TRUE;
            break;
        case MGC_SCSI_DEVICE_TYPE_CDROM:
        case MGC_SCSI_DEVICE_TYPE_SCANNER:
            AccessType = MUSB_HFI_ACCESS_RANDOM_READ;
            DeviceInfo.bmAccessType = 1 << AccessType;
            /* correct removability in case we didn't infer it right */
            pScsi->bRemovable = TRUE;
            break;
        default:
            DeviceInfo.bmAccessType = 0;
            break;
        }
        DeviceInfo.MediaType = pScsi->bRemovable ? MUSB_HFI_MEDIA_REMOVABLE : MUSB_HFI_MEDIA_FIXED;

        /* TODO: set this correctly */
        DeviceInfo.bCanFormat = FALSE;
        /* TODO: set this correctly */
        DeviceInfo.bHasCache = FALSE;

        DeviceInfo.dwBlockSize = ((uint32_t)pScsi->aCapacityData[4] << 24) |
                                 ((uint32_t)pScsi->aCapacityData[5] << 16) | ((uint32_t)pScsi->aCapacityData[6] << 8) |
                                 pScsi->aCapacityData[7];

        if(!DeviceInfo.dwBlockSize)
        {
            DeviceInfo.dwBlockSize = ((uint32_t)pScsi->aFormatCapacityData[10] << 8) |
                                     pScsi->aFormatCapacityData[11];
        }

        if(!DeviceInfo.dwBlockSize)
        {
            DeviceInfo.dwBlockSize = 512;
        }
        else
        {
            if(DeviceInfo.dwBlockSize != 512)
                bRet = 1;
        }
        /* generate awDiskVendor, awDiskProduct, awDiskRevision */
        pInquiry = (const MGC_MsdStandardInquiryData *)pScsi->aInquiryData;
        bCount = (uint8_t)MUSB_MIN(MUSB_HFI_MAX_DISK_VENDOR, 8);
        for(bIndex = 0; bIndex < bCount; bIndex++)
        {
            DeviceInfo.awDiskVendor[bIndex] = pInquiry->aVid[bIndex];
        }
        DeviceInfo.awDiskVendor[bIndex] = 0;
        bCount = (uint8_t)MUSB_MIN(MUSB_HFI_MAX_DISK_PRODUCT, 16);
        for(bIndex = 0; bIndex < bCount; bIndex++)
        {
            DeviceInfo.awDiskProduct[bIndex] = pInquiry->aPid[bIndex];
        }
        DeviceInfo.awDiskProduct[bIndex] = 0;
        bCount = (uint8_t)MUSB_MIN(MUSB_HFI_MAX_DISK_REVISION, 4);
        for(bIndex = 0; bIndex < bCount; bIndex++)
        {
            DeviceInfo.awDiskRevision[bIndex] = pInquiry->aRevision[bIndex];
        }
        DeviceInfo.awDiskRevision[bIndex] = 0;

        /* generate dwBlockCount from READ_CAPACITY response */
        dwBlockCount = ((uint32_t)pScsi->aCapacityData[0] << 24) |
                       ((uint32_t)pScsi->aCapacityData[1] << 16) |
                       ((uint32_t)pScsi->aCapacityData[2] << 8) |
                       pScsi->aCapacityData[3];
        if(!dwBlockCount)
        {
            bReady = FALSE;
        }

        if(bReady)
        {
            /* fill InitialMedium if medium is present */
            DeviceInfo.InitialMedium.AccessType = AccessType;
            DeviceInfo.InitialMedium.dwBlockSize = DeviceInfo.dwBlockSize;
            DeviceInfo.InitialMedium.dwBlockCountLo = dwBlockCount;
            DeviceInfo.InitialMedium.dwBlockCountHi = 0;
            DeviceInfo.InitialMedium.awSerialNumber[0] = 0;
        }

        /* set device info */
        pScsi->pProtocol->pfSetDeviceInfo(pScsi->pProtocol->pProtocolData,
                                          pScsi->bLun, &DeviceInfo);

        if(pScsi->bRemovable && bReady)
        {
            MUSB_PRT("pfsetmedium info\r\n");
            /* prepare & set medium info */
            MediumInfo.AccessType = AccessType;
            MediumInfo.dwBlockSize = DeviceInfo.dwBlockSize;
            MediumInfo.dwBlockCountLo = dwBlockCount;
            MediumInfo.dwBlockCountHi = 0;
            MediumInfo.awSerialNumber[0] = 0;

            pScsi->pProtocol->pfSetMediumInfo(pScsi->pProtocol->pProtocolData,
                                              pScsi->bLun, &MediumInfo);
        }

        if(pScsi->bAnnounced)
        {
            /* if already announced, we are updating medium status */
            MUSB_PRT("pfsetready0--\r\n");
            pScsi->pProtocol->pfSetReady(pScsi->pProtocol->pProtocolData,
                                         pScsi->bLun, bReady);
        }
        else if ( pScsi->bLun == pScsi->bLunCount - 1 )
        {
            /* not yet announced and all LUNs discovered, so announce all of them now */
            pScsi->bAnnounced = TRUE;
            MUSB_PRT("pfsetready1--\r\n");
            for (bLun = 0; bLun < pScsi->bLunCount; bLun++)
            {
                /*MGC_BotProtocolSetReady()*/
                pScsi->pProtocol->pfSetReady(pScsi->pProtocol->pProtocolData, bLun, TRUE);
            }

            MGC_StartupAppEvent();
        }
        else
        {
            MUSB_PRT("NO pfsetready--\r\n");
            pScsi->bAnnounced = TRUE; /* for each LUN < LunCount-1 mark as announced but do it only together with the last LUN */
        }
    }
    else
    {
        if(bOpcode == MGC_SCSI_READ10)
            MUSB_PRT("cmd 28, dwLength=%d\r\n", dwLength);
        else
            MUSB_PRT("\r\nsend scsi cmd : %x\r\n", bOpcode);

        /* not announcing yet; send next command */
        bSuccess = pProtocol->pfSendCmd(pProtocol->pProtocolData, pScsi, bLun,
                                        pScsi->aCmd, bCmdLength, pBuffer, dwLength, bSend,
                                        MGC_ScsiCmdComplete, FALSE, 2);

        if(!bSuccess)
        {
            MUSB_DIAG_STRING(1, "MSD/SCSI: Protocol SendCmd failed");
        }
    }
    return bRet;
}

/*
 * Determine device characteristics with standard inquiry,
 * VPD S/N page, etc. and wait until device is ready
 */
static uint8_t MGC_ScsiCmdSetDiscoverDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun)
{
    MGC_ScsiCmdSetData *pScsi = (MGC_ScsiCmdSetData *)pInstance;

    pScsi->pProtocol = pProtocol;
    pScsi->bLun = bLun;

    /* clear retry counters etc. */
    pScsi->dwRetries = 0;
    pScsi->dwCapacityRetries = 0;
    pScsi->bFmtCapacityRetries = 0;
    pScsi->bRemovable = FALSE;
    pScsi->bAnnounced = FALSE;
    pScsi->bNoMedium = FALSE;

    MUSB_PRT("send cmd 0x12\r\n");
    /* start things by sending INQUIRY command */
    pScsi->bState = MGC_SCSI_STATE_INQUIRY;
    MUSB_MemSet(pScsi->aCmd, 0, 16);
    pScsi->aCmd[0] = MGC_SCSI_INQUIRY;
    pScsi->aCmd[4] = 36;

    /* special handling for multi-LUN devices */
    if(!bLun && (pScsi->bLunCount > 1))
    {
        pScsi->bLunScan = TRUE;
        pScsi->bLunIndex = 0;
    }
    else
    {
        pScsi->bLunScan = FALSE;
    }
    //即函数MGC_BotProtocolSendCmd()
    return pProtocol->pfSendCmd(pProtocol->pProtocolData, pScsi, bLun,
                                pScsi->aCmd, 6,
                                pScsi->aInquiryData, 36,
                                FALSE, MGC_ScsiCmdComplete,
                                FALSE, 0);
}

/*
 * FS wants to mount the device, so start unit & attempt to prevent medium removal
 */
static uint8_t MGC_ScsiCmdSetMountDevice(void *pInstance,
        MGC_pfMsdMountComplete pfMountComplete,
        void *pPrivateData,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun)
{
    MGC_ScsiCmdSetData *pScsi = (MGC_ScsiCmdSetData *)pInstance;
    MGC_MsdStandardInquiryData *pInquiryData = (MGC_MsdStandardInquiryData *)pScsi->aInquiryData;

    /* prevent medium removal to the maximum extent possible */
    if(pInquiryData->bRmb & MGC_M_MSD_INQUIRY_RMB)
    {
        pScsi->pfMountComplete = pfMountComplete;
        pScsi->pMountCompleteParam = pPrivateData;
        pScsi->bState = MGC_SCSI_STATE_PREVENT_MEDIUM_REMOVE;
        MUSB_MemSet(pScsi->aCmd, 0, 16);
        pScsi->aCmd[0] = MGC_SCSI_PREVENT_ALLOW_MED_REMOVE;
        pScsi->aCmd[4] = (pInquiryData->abmFlags[1] & MGC_M_MSD_INQUIRY_M_CHNGR) ? 3 : 1;
        return pProtocol->pfSendCmd(pProtocol->pProtocolData, pScsi, bLun,
                                    pScsi->aCmd, 6, NULL, 0, TRUE, MGC_ScsiCmdComplete, FALSE, 2);
    }
    pfMountComplete(pPrivateData, TRUE);
    return TRUE;
}

/*
 * FS wants to unmount device, so allow medium removal & stop unit
 */
static uint8_t MGC_ScsiCmdSetUnmountDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun)
{
    MGC_ScsiCmdSetData *pScsi = (MGC_ScsiCmdSetData *)pInstance;

    /* allow medium removal */
    pScsi->bState = MGC_SCSI_STATE_ALLOW_MEDIUM_REMOVE;
    MUSB_MemSet(pScsi->aCmd, 0, 16);
    pScsi->aCmd[0] = MGC_SCSI_PREVENT_ALLOW_MED_REMOVE;
    return pProtocol->pfSendCmd(pProtocol->pProtocolData, pScsi, bLun,
                                pScsi->aCmd, 6, NULL, 0, TRUE, MGC_ScsiCmdComplete, FALSE, 2);
}

/* Implementation */
static uint8_t MGC_ScsiCmdSetGetReadCmd(void *pInstance,
                                        uint8_t *pCmdBuffer,
                                        uint8_t bMaxCmdLength,
                                        uint32_t dwBlockLo,
                                        uint32_t dwBlockHi,
                                        uint16_t wBlockCount,
                                        uint8_t bLun)
{
    uint8_t bIndex = 0;

    /* only use Read10, so only low 32-bits of block */
    pCmdBuffer[bIndex++] = MGC_SCSI_READ10;
    pCmdBuffer[bIndex++] = 0;
    pCmdBuffer[bIndex++] = (uint8_t)((dwBlockLo >> 24) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)((dwBlockLo >> 16) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)((dwBlockLo >> 8) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)(dwBlockLo & 0xff);
    pCmdBuffer[bIndex++] = 0;
    pCmdBuffer[bIndex++] = (uint8_t)((wBlockCount >> 8) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)(wBlockCount & 0xff);
    pCmdBuffer[bIndex++] = 0;

    return 10;
}

/* Implementation */
static uint8_t MGC_ScsiCmdSetGetWriteCmd(void *pInstance,
        uint8_t *pCmdBuffer,
        uint8_t bMaxCmdLength,
        uint32_t dwBlockLo,
        uint32_t dwBlockHi,
        uint16_t wBlockCount,
        uint8_t bLun)
{
    uint8_t bIndex = 0;

    /* only use Write10, so only low 32-bits of block */
    pCmdBuffer[bIndex++] = MGC_SCSI_WRITE10;
    pCmdBuffer[bIndex++] = 0;
    pCmdBuffer[bIndex++] = (uint8_t)((dwBlockLo >> 24) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)((dwBlockLo >> 16) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)((dwBlockLo >> 8) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)(dwBlockLo & 0xff);
    pCmdBuffer[bIndex++] = 0;
    pCmdBuffer[bIndex++] = (uint8_t)((wBlockCount >> 8) & 0xff);
    pCmdBuffer[bIndex++] = (uint8_t)(wBlockCount & 0xff);
    pCmdBuffer[bIndex++] = 0;

    return 10;
}

static uint8_t MGC_ScsiCmdSetFlushDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun)
{
    /*MGC_ScsiCmdSetData* pScsi = (MGC_ScsiCmdSetData*)pInstance;*/

    /* TODO: sync cache command, but it wants a range of blocks! */
    return TRUE;
}

static uint8_t MGC_ScsiCmdSetFormatDevice(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint32_t dwBlockSize,
        uint8_t bLun)
{
    /*MGC_ScsiCmdSetData* pScsi = (MGC_ScsiCmdSetData*)pInstance;*/

    /* TODO: mode select to set block size, then format unit */
    return TRUE;
}

static uint8_t MGC_ScsiCmdSetAbortFormat(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun)
{
    /*MGC_ScsiCmdSetData* pScsi = (MGC_ScsiCmdSetData*)pInstance;*/

    /* TODO */
    return TRUE;
}

static void MGC_ScsiCmdSetParseInterrupt(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        const uint8_t *pMessage,
        uint16_t wMessageLength,
        uint8_t bLun)
{
    /* TODO: only relevant if CBI added in the future */
}

static uint8_t MGC_ScsiCmdSetCheckMedium(void *pInstance,
        MGC_MsdProtocol *pProtocol,
        uint8_t bLun)
{
    MGC_ScsiCmdSetData *pScsi = (MGC_ScsiCmdSetData *)pInstance;

    /* clear retry counters etc. */
    pScsi->dwRetries = 0;
    pScsi->dwCapacityRetries = 0;
    pScsi->bFmtCapacityRetries = 0;
    pScsi->bRemovable = FALSE;
    pScsi->bNoMedium = FALSE;
    pScsi->bLunScan = FALSE;
    pScsi->bLun = bLun;

    /* start things by sending INQUIRY command */
    pScsi->bState = MGC_SCSI_STATE_INQUIRY;
    MUSB_MemSet(pScsi->aCmd, 0, 16);
    pScsi->aCmd[0] = MGC_SCSI_INQUIRY;
    pScsi->aCmd[4] = 36;
    return pProtocol->pfSendCmd(pProtocol->pProtocolData, pScsi, bLun,
                                pScsi->aCmd, 6, pScsi->aInquiryData, 36, FALSE, MGC_ScsiCmdComplete,
                                FALSE, 0);
}
#endif // CFG_SUPPORT_MSD
// eof

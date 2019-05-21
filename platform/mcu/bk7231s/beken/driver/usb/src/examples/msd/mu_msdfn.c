/******************************************************************
*                                                                *
*        Copyright Mentor Graphics Corporation 2005              *
*                                                                *
*                All Rights Reserved.                            *
*                                                                *
*    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
*  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
*  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
*                                                                *
******************************************************************/
#include "include.h"

#include "mu_arch.h"
#include "mu_cdi.h"
#include "mu_diag.h"
#include "mu_impl.h"
#include "mu_mem.h"
#include "class/mu_msd.h"
#include "class/mu_bot.h"
#include "class/mu_scsi.h"

/**
 * You may define this if your core is configured for a bidirectional endpoint 1
 */
#define MGC_MSD_BIDIR_ENDPOINTS

/**
 * MGC_MsdCommandUsage.
 * For table lookup of command usage data
 * @field bCommand command
 * @field bDataSize size of buffer
 * @field pData buffer
 */
typedef struct
{
    uint8_t bCommand;
    uint8_t bDataSize;
    const uint8_t *pData;
} MGC_MsdCommandUsage;

#include "mu_pkon.h"
/**
 * MGC_MsdRbcParams.
 * For static declaration of RBC mode parameters page
 */
typedef struct
{
    const MGC_MsdModeParamHeader6 Header6;
    MGC_MsdRbcDeviceParams RbcParams;
} MGC_MsdRbcParams;

#if CFG_SUPPORT_MSD

#include "mu_pkoff.h"
/******************************************************************
Forwards
******************************************************************/
static uint8_t MGC_MsdDeviceRequest(void *hClient, MUSB_BusHandle hBus,
                                    uint32_t dwSequenceNumber,
                                    const uint8_t *pSetup,
                                    uint16_t wRequestLength);
static uint8_t MGC_MsdDeviceConfigSelected(void *hClient,
        MUSB_BusHandle hBus,
        uint8_t bConfigurationValue,
        MUSB_PipePtr *ahPipe);
static void MGC_MsdNewUsbState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_State State);

static uint32_t MGC_MsdRxCbwComplete(void *pCompleteParam, MUSB_Irp *pIrp);
static uint32_t MGC_MsdRxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp);
static uint32_t MGC_MsdTxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp);
static uint32_t MGC_MsdTxCswComplete(void *pCompleteParam, MUSB_Irp *pIrp);

/******************************************************************
Globals
******************************************************************/

/* UCDI variables */
static uint8_t MGC_bMsdSelfPowered = TRUE;
static uint8_t MGC_aControlData[1];//[256];

static uint8_t MGC_aDescriptorData[] =
{
    /* Device Descriptor */
    0x12,                      /* bLength              */
    MUSB_DT_DEVICE,            /* DEVICE               */
    0x00, 0x02,                /* USB 2.0              */
    0x00,                      /* CLASS                */
    0x00,                      /* Subclass             */
    0x00,                      /* Protocol             */
    0x40,                      /* bMaxPktSize0         */
    0xd6, 0x04,                /* idVendor             */
    0x33, 0x22,                /* idProduct            */
    0x00, 0x02,                /* bcdDevice            */
    0x01,                      /* iManufacturer        */
    0x02,                      /* iProduct             */
    0x03,                      /* iSerial Number       */
    0x01,                      /* One configuration    */

    /* strings */
    2 + 4,
    MUSB_DT_STRING,
    0x09, 0x04,			/* English (U.S.) */
    0x09, 0x0c,			/* English (Australia) */

    /* TODO: make tool to generate strings and eventually whole descriptor! */
    /* English (U.S.) strings */
    2 + 30,			/* Manufacturer: Mentor Graphics */
    MUSB_DT_STRING,
    'M', 0, 'e', 0, 'n', 0, 't', 0, 'o', 0, 'r', 0, ' ', 0,
    'G', 0, 'r', 0, 'a', 0, 'p', 0, 'h', 0, 'i', 0, 'c', 0, 's', 0,

    2 + 8,			/* Product ID: Demo */
    MUSB_DT_STRING,
    'D', 0, 'e', 0, 'm', 0, 'o', 0,

    2 + 24,			/* Serial #: 123412341234 */
    MUSB_DT_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0,
    '1', 0, '2', 0, '3', 0, '4', 0,
    '1', 0, '2', 0, '3', 0, '4', 0,

    /* English (Australia) strings */
    2 + 30,			/* Manufacturer: Mentor Graphics */
    MUSB_DT_STRING,
    'M', 0, 'e', 0, 'n', 0, 't', 0, 'o', 0, 'r', 0, ' ', 0,
    'G', 0, 'r', 0, 'a', 0, 'p', 0, 'h', 0, 'i', 0, 'c', 0, 's', 0,

    2 + 20,			/* Product ID: Good on ya */
    MUSB_DT_STRING,
    'G', 0, 'o', 0, 'o', 0, 'd', 0, ' ', 0, 'o', 0, 'n', 0, ' ', 0, 'y', 0, 'a', 0,

    2 + 24,			/* Serial #: 123412341234 */
    MUSB_DT_STRING,
    '1', 0, '2', 0, '3', 0, '4', 0,
    '1', 0, '2', 0, '3', 0, '4', 0,
    '1', 0, '2', 0, '3', 0, '4', 0,

    /* configuration */
    0x09,                                   /* bLength              */
    0x02,                                   /* CONFIGURATION        */
    0x23,                                   /* length               */
    0x0,                                    /* length               */
    0x01,                                   /* bNumInterfaces       */
    0x01,                                   /* bConfigurationValue  */
    0x00,                                   /* iConfiguration       */
    0xC0,                                   /* bmAttributes (required + self-powered) */
    0x2,                                    /* power                */

    /* interface */
    0x09,                                   /* bLength              */
    0x04,                                   /* INTERFACE            */
    0x0,                                    /* bInterfaceNumber     */
    0x0,                                    /* bAlternateSetting    */
    0x02,                                   /* bNumEndpoints        */
    0x08,                                   /* bInterfaceClass      */
    0x06,                                   /* bInterfaceSubClass (1=RBC, 6=SCSI) */
    0x50,                                   /* bInterfaceProtocol (BOT) */
    0x00,                                   /* iInterface           */

    /* Endpoint Descriptor  : Bulk-In */
    0x07,                                   /* bLength              */
    0x05,                                   /* ENDPOINT             */
    0x81,                                   /* bEndpointAddress     */
    0x02,                                   /* bmAttributes         */
    0x40, 0x00,                             /* wMaxPacketSize       */
    0x10,                                   /* bInterval            */

    /* Endpoint Descriptor  : Bulk-Out */
    0x07,                                   /* bLength              */
    0x05,                                   /* ENDPOINT             */
#ifdef MGC_MSD_BIDIR_ENDPOINTS
    0x01,                                   /* bEndpointAddress     */
#else
    0x02,                                   /* bEndpointAddress     */
#endif
    0x02,                                   /* bmAttributes         */
    0x40, 0x00,                             /* wMaxPacketSize       */
    0x10,                                   /* bInterval            */

    /* OTG */
    MUSB_DT_OTG_SIZE,
    MUSB_DT_OTG,
    MUSB_OTG_SRP | MUSB_OTG_HNP
};

static uint8_t MGC_aHighSpeedDescriptorData[] =
{
    /* device qualifier */
    0x0a,                      /* bLength              */
    MUSB_DT_DEVICE_QUALIFIER,  /* DEVICE Qualifier     */
    0x00, 0x02,                /* USB 2.0              */
    0,                         /* CLASS                */
    0,                         /* Subclass             */
    0x00,                      /* Protocol             */
    0x40,                      /* bMaxPacketSize0      */
    0x01,                      /* One configuration    */
    0x00,                      /* bReserved            */

    /* configuration */
    0x09,                                   /* bLength              */
    MUSB_DT_OTHER_SPEED,                    /* CONFIGURATION        */
    0x23,                                   /* length               */
    0x0,                                    /* length               */
    0x01,                                   /* bNumInterfaces       */
    0x01,                                   /* bConfigurationValue  */
    0x00,                                   /* iConfiguration       */
    0xC0,                                   /* bmAttributes (required + self-powered) */
    0x2,                                    /* power                */

    /* interface */
    0x09,                                   /* bLength              */
    0x04,                                   /* INTERFACE            */
    0x0,                                    /* bInterfaceNumber     */
    0x0,                                    /* bAlternateSetting    */
    0x02,                                   /* bNumEndpoints        */
    0x08,                                   /* bInterfaceClass      */
    0x06,                                   /* bInterfaceSubClass (1=RBC, 6=SCSI) */
    0x50,                                   /* bInterfaceProtocol (BOT) */
    0x00,                                   /* iInterface           */

    /* Endpoint Descriptor  : Bulk-In */
    0x07,                                   /* bLength              */
    0x05,                                   /* ENDPOINT             */
    0x81,                                   /* bEndpointAddress     */
    0x02,                                   /* bmAttributes         */
    0x40, 0x00,                             /* wMaxPacketSize       */
    0x04,                                   /* bInterval            */

    /* Endpoint Descriptor  : Bulk-Out */
    0x07,                                   /* bLength              */
    0x05,                                   /* ENDPOINT             */
#ifdef MGC_MSD_BIDIR_ENDPOINTS
    0x01,                                   /* bEndpointAddress     */
#else
    0x02,                                   /* bEndpointAddress     */
#endif
    0x02,                                   /* bmAttributes         */
    0x40, 0x00,                             /* wMaxPacketSize       */
    0x04,                                   /* bInterval            */

    /* OTG */
    MUSB_DT_OTG_SIZE,
    MUSB_DT_OTG,
    MUSB_OTG_SRP | MUSB_OTG_HNP
};

static MGC_MsdStandardInquiryData MGC_MsdRbcInquiryData =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,
    0x80,   /* removable (though this doesn't seem so) */
    0,	    /* 0=no comformance to any standard */
    0,	    /* 2=required response data format */
    0x1f,	    /* extra length */
    {0, 0, 0},
    { 'M', 'e', 'n', 't', 'o', 'r', ' ', ' ' },
    { 'D' , 'e', 'm', 'o', ' ', 'D', 'i', 's', 'k', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
    { '1', ' ', ' ', ' ' }
};

static uint8_t MGC_MsdSupportedVpdPagesData[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    MGC_SCSI_PAGE_SUPPORTED_VPD,    /* page code */
    0,	    /* reserved */
    3,	    /* length of: */
    MGC_SCSI_PAGE_SUPPORTED_VPD,
    MGC_SCSI_PAGE_UNIT_SERIAL_NUM,
    MGC_SCSI_PAGE_DEVICE_ID
};

static uint8_t MGC_MsdUnitSerialNumberPageData[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    MGC_SCSI_PAGE_UNIT_SERIAL_NUM,  /* page code */
    0,	    /* reserved */
    4,	    /* length of: */
    '1', '2', 'f', 'e'
};

static uint8_t MGC_MsdDeviceIdPageData[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    MGC_SCSI_PAGE_DEVICE_ID,	    /* page code */
    0,	    /* reserved */
    0,	    /* length of: */
};

static uint8_t MGC_aUnsupportedCommandData[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    1,	    /* unsupported */
    0,
    0,
    0,
    0
};

static uint8_t MGC_aRead10Support[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    10,
    MGC_SCSI_READ10,
    0,
    0xff, 0xff, 0xff, 0xff,
    0,
    0xff, 0xff,
    0
};

static uint8_t MGC_aWrite10Support[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    10,
    MGC_SCSI_WRITE10,
    0,
    0xff, 0xff, 0xff, 0xff,
    0,
    0xff, 0xff,
    0
};

static uint8_t MGC_aVerifySupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    10,
    MGC_SCSI_VERIFY,
    0,
    0xff, 0xff, 0xff, 0xff,
    0,
    0xff, 0xff,
    0
};

static uint8_t MGC_aWriteBufferSupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    10,
    MGC_SCSI_WRITE_BUFFER,
    7,
    0xff, 0xff, 0xff, 0xff,
    0,
    0xff, 0xff,
    0
};

static uint8_t MGC_aReadCapacitySupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    10,
    MGC_SCSI_READ_CAPACITY,
    0,
    0xff, 0xff, 0xff, 0xff,
    0,
    0xff, 0xff,
    0
};

static uint8_t MGC_aTestUnitReadySupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    6,
    MGC_SCSI_TEST_UNIT_READY,
    0,
    0, 0,
    0xff,
    0
};

static uint8_t MGC_aRequestSenseSupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    6,
    MGC_SCSI_REQUEST_SENSE,
    0,
    0, 0,
    0xff,
    0
};

static uint8_t MGC_aInquirySupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    6,
    MGC_SCSI_INQUIRY,
    3,
    0xff,
    0,
    0xff,
    0
};

static uint8_t MGC_aModeSenseSupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    6,
    MGC_SCSI_MODE_SENSE,
    8,
    0xff,
    0,
    0xff,
    0
};

static uint8_t MGC_aModeSelectSupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    6,
    MGC_SCSI_MODE_SELECT,
    0x11,
    0,
    0,
    0xff,
    0
};

static uint8_t MGC_aStartStopUnitSupport[] =
{
    MGC_SCSI_DEVICE_TYPE_DIRECT,   /* device type */
    5,	    /* supported, vendor-specific */
    0,	    /* no compliance claimed */
    0, 0,
    6,
    MGC_SCSI_START_STOP_UNIT,
    0x01,
    0,
    0,
    0xf3,
    0
};

static MGC_MsdCommandUsage MGC_aCommandUsageData[] =
{
    { MGC_SCSI_READ10, (uint8_t)sizeof(MGC_aRead10Support), MGC_aRead10Support },
    { MGC_SCSI_WRITE10, (uint8_t)sizeof(MGC_aWrite10Support), MGC_aWrite10Support },
    { MGC_SCSI_VERIFY, (uint8_t)sizeof(MGC_aVerifySupport), MGC_aVerifySupport },
    { MGC_SCSI_WRITE_BUFFER, (uint8_t)sizeof(MGC_aWriteBufferSupport), MGC_aWriteBufferSupport },
    { MGC_SCSI_READ_CAPACITY, (uint8_t)sizeof(MGC_aReadCapacitySupport), MGC_aReadCapacitySupport },
    { MGC_SCSI_TEST_UNIT_READY, (uint8_t)sizeof(MGC_aTestUnitReadySupport), MGC_aTestUnitReadySupport },
    { MGC_SCSI_REQUEST_SENSE, (uint8_t)sizeof(MGC_aRequestSenseSupport), MGC_aRequestSenseSupport },
    { MGC_SCSI_INQUIRY, (uint8_t)sizeof(MGC_aInquirySupport), MGC_aInquirySupport },
    { MGC_SCSI_MODE_SENSE, (uint8_t)sizeof(MGC_aModeSenseSupport), MGC_aModeSenseSupport },
    { MGC_SCSI_MODE_SELECT, (uint8_t)sizeof(MGC_aModeSelectSupport), MGC_aModeSelectSupport },
    { MGC_SCSI_START_STOP_UNIT, (uint8_t)sizeof(MGC_aStartStopUnitSupport), MGC_aStartStopUnitSupport },

};

static uint8_t MGC_bIsReady = TRUE;
static uint8_t MGC_bMsdMaxLun = 0;
static uint8_t MGC_bMsdInterface = 0;
static uint16_t MGC_wBlockSize = 512;
static uint32_t MGC_dwRxCount;
static uint32_t MGC_dwGoodCmdCount = 0;

/** Current sense data */
static MGC_MsdScsiSenseData MGC_MsdSenseData;

/** Capacity response */
static MGC_MsdCapacityResponse MGC_MsdCapacity;
static MGC_MsdFormatCapacityResponse MGC_MsdFormatCapacity;

/** Current CBW */
static MGC_MsdCbw MGC_Cbw;

/** Current CSW */
static MGC_MsdCsw MGC_Csw;

static uint8_t MGC_aJunk[1];//[512];

/** IRP for CBW reception */
static MUSB_Irp MGC_MsdRxCbwIrp =
{
    NULL,
    (uint8_t *) &MGC_Cbw,
    sizeof(MGC_Cbw),
    0,
    0,
    MGC_MsdRxCbwComplete,
    NULL,
    FALSE,	/* bAllowShortTransfer */
    FALSE,	/* bIsrCallback */
    FALSE	/* bAllowDma */
};

/** IRP for data reception */
static MUSB_Irp MGC_MsdRxDataIrp =
{
    NULL,
    NULL,
    0,
    0,
    0,
    MGC_MsdRxDataComplete,
    NULL,
    FALSE,	/* bAllowShortTransfer */
    FALSE,	/* bIsrCallback */
    TRUE	/* bAllowDma */
};

/** IRP for data transmission */
static MUSB_Irp MGC_MsdTxDataIrp =
{
    NULL,
    NULL,
    0,
    0,
    0,
    MGC_MsdTxDataComplete,
    NULL,
    FALSE,	/* bAllowShortTransfer */
    FALSE,	/* bIsrCallback */
    TRUE	/* bAllowDma */
};

/** IRP for CSW transmission */
static MUSB_Irp MGC_MsdTxCswIrp =
{
    NULL,
    (uint8_t *) &MGC_Csw,
    13,
    0,
    0,
    MGC_MsdTxCswComplete,
    NULL,
    FALSE,	/* bAllowShortTransfer */
    FALSE,	/* bIsrCallback */
    FALSE	/* bAllowDma */
};

/*
* registration
*/
MUSB_FunctionClient MGC_xxxFunctionClient =
{
    NULL,	/* no instance data; we are singleton */
    MGC_aDescriptorData,
    sizeof(MGC_aDescriptorData),
    3,		/* strings per language */
    MGC_aHighSpeedDescriptorData,
    sizeof(MGC_aHighSpeedDescriptorData),
    sizeof(MGC_aControlData),
    MGC_aControlData,
    &MGC_bMsdSelfPowered,
    MGC_MsdDeviceRequest,
    MGC_MsdDeviceConfigSelected,
    NULL,
    MGC_MsdNewUsbState
};

static uint8_t MGC_aMsdData[1];//[1024+512];
static uint32_t MGC_dwMsdBlockCount = 1024 * 8192 / 512;

/******************************************************************
CDI callbacks
******************************************************************/
static void MGC_MsdNewUsbState(void *hClient, MUSB_BusHandle hBus,
                               MUSB_State State)
{
    MUSB_PRT("[MGC] MsdNewUsbState:state=%x\r\n", State);
}

static uint8_t MGC_MsdDeviceRequest(void *hClient, MUSB_BusHandle hBus,
                                    uint32_t dwSequence, const uint8_t *pSetup,
                                    uint16_t wLength)
{
    uint32_t dwStatus;
    MUSB_DeviceRequest *pRequest = (MUSB_DeviceRequest *)pSetup;
    uint8_t bOk = FALSE;

    if(MUSB_TYPE_STANDARD == (pRequest->bmRequestType & MUSB_TYPE_MASK))
    {
        switch(pRequest->bRequest)
        {
        case MUSB_REQ_GET_INTERFACE:
            MUSB_DeviceResponse(hBus, dwSequence, &MGC_bMsdInterface, 1, FALSE);
            bOk = TRUE;
            break;
        case MUSB_REQ_SET_INTERFACE:
            MUSB_DeviceResponse(hBus, dwSequence, NULL, 0, FALSE);
            bOk = TRUE;
            break;
        }
    }
    else if((pRequest->bmRequestType & MUSB_TYPE_CLASS)
            && (pRequest->bmRequestType & MUSB_RECIP_INTERFACE))
    {
        switch(pRequest->bRequest)
        {
        case MGC_MSD_BOT_RESET:
            /* reset */
            MUSB_CancelTransfer(&MGC_MsdTxCswIrp);
            MUSB_CancelTransfer(&MGC_MsdTxDataIrp);
            MUSB_CancelTransfer(&MGC_MsdRxDataIrp);
            MUSB_CancelTransfer(&MGC_MsdRxCbwIrp);
            MUSB_FlushPipe(MGC_MsdTxDataIrp.hPipe);
            MUSB_FlushPipe(MGC_MsdRxDataIrp.hPipe);
            dwStatus = MUSB_StartTransfer(&MGC_MsdRxCbwIrp);
            MUSB_DeviceResponse(hBus, dwSequence, NULL, 0, FALSE);
            bOk = TRUE;
            break;

        case MGC_MSD_BOT_GET_MAXLUN:
            /* get max lun */
            MUSB_DeviceResponse(hBus, dwSequence, &MGC_bMsdMaxLun, 1, FALSE);
            bOk = TRUE;
            break;
        }
    }
    dwStatus++;
    return bOk;
}

static uint8_t MGC_MsdDeviceConfigSelected(void *hClient, MUSB_BusHandle hBus,
        uint8_t bConfigurationValue,
        MUSB_PipePtr *ahPipe)
{
    uint32_t dwStatus;

    MGC_MsdTxDataIrp.hPipe = ahPipe[0];
    MGC_MsdTxCswIrp.hPipe = ahPipe[0];
    MGC_MsdRxCbwIrp.hPipe = ahPipe[1];
    MGC_MsdRxDataIrp.hPipe = ahPipe[1];
    dwStatus = MUSB_StartTransfer(&MGC_MsdRxCbwIrp);
    if(MUSB_STATUS_OK == dwStatus)
    {
        MGC_bIsReady = TRUE;
        MGC_MsdSenseData.bResponseCode = MGC_M_MSD_SCSI_SENSE_DATA_VALID | 0x70;
        MGC_MsdSenseData.bSenseKey = (uint8_t)MGC_SCSI_SC_NONE;
        MGC_MsdSenseData.bAsc = 0;
        MGC_MsdSenseData.bAscq = 0;
        return TRUE;
    }
    /* TODO: log error? */
    return FALSE;
}

/* handle an INQUIRY command */
static void MGC_MsdInquiry(uint8_t bmFlags, uint32_t dwLength, uint8_t bPage,
                           uint8_t *pbStartTx, uint8_t *pbStatus)
{
    uint16_t wIndex, wCount;

    if((bmFlags & (MGC_M_MSD_INQUIRY_CMD_DT | MGC_M_MSD_INQUIRY_EVPD)) ==
            (MGC_M_MSD_INQUIRY_CMD_DT | MGC_M_MSD_INQUIRY_EVPD))
    {
        /* set sense codes */
        MGC_MsdSenseData.bResponseCode = MGC_M_MSD_SCSI_SENSE_DATA_VALID | 0x70;
        MGC_MsdSenseData.bSenseKey = (uint8_t)MGC_SCSI_SC_ILLEGAL_REQUEST;
        MGC_MsdSenseData.bAsc = MGC_SCSI_ASC_INVALID_CDB_FIELD;
        MGC_MsdSenseData.bAdditionalLength = 5;
        *pbStatus = 1;
    }
    else if(bmFlags & MGC_M_MSD_INQUIRY_CMD_DT)
    {
        /* command support data */
        wCount = sizeof(MGC_aCommandUsageData) / sizeof(MGC_MsdCommandUsage);
        for(wIndex = 0; wIndex < wCount; wIndex++)
        {
            if(MGC_aCommandUsageData[wIndex].bCommand == bPage)
            {
                break;
            }
        }
        if(wIndex < wCount)
        {
            MGC_MsdTxDataIrp.pBuffer = (uint8_t *)MGC_aCommandUsageData[wIndex].pData;
            MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength,
                                                 MGC_aCommandUsageData[wIndex].bDataSize);
        }
        else
        {
            MGC_MsdTxDataIrp.pBuffer = (uint8_t *)MGC_aUnsupportedCommandData;
            MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength,
                                                 sizeof(MGC_aUnsupportedCommandData));
        }
        *pbStartTx = TRUE;
        *pbStatus = 0;
    }
    else if(bmFlags & MGC_M_MSD_INQUIRY_EVPD)
    {
        /* vital product data pages */
        switch(bPage)
        {
        case MGC_SCSI_PAGE_SUPPORTED_VPD:
            MGC_MsdTxDataIrp.pBuffer = (uint8_t *)&MGC_MsdSupportedVpdPagesData;
            MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength,
                                                 sizeof(MGC_MsdSupportedVpdPagesData));
            *pbStartTx = TRUE;
            *pbStatus = 0;
            break;
        case MGC_SCSI_PAGE_UNIT_SERIAL_NUM:
            MGC_MsdTxDataIrp.pBuffer = (uint8_t *)&MGC_MsdUnitSerialNumberPageData;
            MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength,
                                                 sizeof(MGC_MsdUnitSerialNumberPageData));
            *pbStartTx = TRUE;
            *pbStatus = 0;
            break;
        case MGC_SCSI_PAGE_DEVICE_ID:
            MGC_MsdTxDataIrp.pBuffer = (uint8_t *)&MGC_MsdDeviceIdPageData;
            MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength,
                                                 sizeof(MGC_MsdDeviceIdPageData));
            *pbStartTx = TRUE;
            *pbStatus = 0;
            break;
        }
    }
    else
    {
        /* standard data */
        MGC_MsdTxDataIrp.pBuffer = (uint8_t *)&MGC_MsdRbcInquiryData;
        MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength,
                                             sizeof(MGC_MsdRbcInquiryData));
        *pbStartTx = TRUE;
        *pbStatus = 0;
    }
}

static void MGC_MsdCommand(const MGC_MsdCbw *pCbw)
{
    const MGC_MsdCommand6 *pCommand6;
    const MGC_MsdInquiryCommand *pInquiryCommand;
    uint8_t bmFlags, bPage;
    uint32_t dwLength;
    uint8_t bStartTx = FALSE;
    uint8_t bStartRx = FALSE;
    uint8_t bStatus = 1;
    uint32_t dwBlock = 0;
    uint32_t dwDataLength = MUSB_SWAP32(pCbw->dCbwDataTransferLength);
    MGC_MsdCsw *pCsw = &MGC_Csw;
    uint8_t bOpcode = pCbw->aCbwCb[0];

    MGC_Cbw.dCbwDataTransferLength = dwDataLength;

    /* seed failed sense data */
    MGC_MsdSenseData.bResponseCode = MGC_M_MSD_SCSI_SENSE_DATA_VALID | 0x70;
    MGC_MsdSenseData.bSenseKey = (uint8_t)MGC_SCSI_SC_ILLEGAL_REQUEST;
    MGC_MsdSenseData.bAsc = MGC_SCSI_ASC_INVALID_CDB_FIELD;
    MGC_MsdSenseData.bAdditionalLength = 5;

    /* parse command */

    pCommand6 = (const MGC_MsdCommand6 *) & (pCbw->aCbwCb[0]);

    switch(bOpcode)
    {
    case MGC_SCSI_START_STOP_UNIT:
        if(pCbw->aCbwCb[4] & 1)
        {
            MGC_bIsReady = TRUE;
        }
        else
        {
            MGC_bIsReady = FALSE;
        }
        MGC_MsdSenseData.bResponseCode = MGC_M_MSD_SCSI_SENSE_DATA_VALID | 0x70;
        MGC_MsdSenseData.bSenseKey = (uint8_t)MGC_SCSI_SC_NONE;
        MGC_MsdSenseData.bAsc = 0;
        MGC_MsdSenseData.bAscq = 0;
        bStatus = 0;
        break;

    case MGC_SCSI_REQUEST_SENSE:
        dwLength = pCbw->aCbwCb[4];
        MGC_MsdTxDataIrp.pBuffer = (uint8_t *)&MGC_MsdSenseData;
        MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength,
                                             (uint32_t)(8 + MGC_MsdSenseData.bAdditionalLength));
        pCsw->dCswDataResidue = dwLength - MGC_MsdTxDataIrp.dwLength;
        bStartTx = TRUE;
        MGC_MsdSenseData.bResponseCode = 0x70;
        bStatus = 0;
        break;

    case MGC_SCSI_TEST_UNIT_READY:
        pCsw->dCswDataResidue = 0L;
        MGC_MsdSenseData.bAdditionalLength = 5;
        if(MGC_bIsReady)
        {
            MGC_MsdSenseData.bResponseCode = MGC_M_MSD_SCSI_SENSE_DATA_VALID | 0x70;
            MGC_MsdSenseData.bSenseKey = (uint8_t)MGC_SCSI_SC_NONE;
            MGC_MsdSenseData.bAsc = 0;
            MGC_MsdSenseData.bAscq = 0;
            bStatus = 0;
        }
        else
        {
            MGC_MsdSenseData.bResponseCode =
                MGC_M_MSD_SCSI_SENSE_DATA_VALID | MGC_SCSI_STATUS_CHECK_CONDITION;
            MGC_MsdSenseData.bSenseKey = (uint8_t)MGC_SCSI_SC_NOT_READY;
            MGC_MsdSenseData.bAsc = 4;
            MGC_MsdSenseData.bAscq = 2;
            bStatus = 1;
        }
        break;

    case MGC_SCSI_INQUIRY:
        pInquiryCommand = (const MGC_MsdInquiryCommand *)pCommand6;
        bmFlags = pInquiryCommand->bmFlags;
        dwLength = pCbw->aCbwCb[4];
        bPage = pCbw->aCbwCb[2];
        MGC_MsdInquiry(bmFlags, dwLength, bPage, &bStartTx, &bStatus);
        pCsw->dCswDataResidue = dwLength - MGC_MsdTxDataIrp.dwLength;
        break;

    case MGC_SCSI_READ_CAPACITY:
        dwLength = pCbw->aCbwCb[4];
        MGC_MsdCapacity.dwLastBlock = MGC_SCSI_SWAP32(MGC_dwMsdBlockCount - 1);
        MGC_MsdCapacity.dwBytesPerBlock = MGC_SCSI_SWAP32(MGC_wBlockSize);
        MGC_MsdTxDataIrp.pBuffer = (uint8_t *)&MGC_MsdCapacity;
        MGC_MsdTxDataIrp.dwLength = sizeof(MGC_MsdCapacity);
        pCsw->dCswDataResidue = dwLength ? (dwLength - MGC_MsdTxDataIrp.dwLength) : 0;
        MGC_MsdSenseData.bResponseCode = 0x70;
        bStartTx = TRUE;
        bStatus = 0;
        break;

    case MGC_SCSI_RD_FMT_CAPC:
        dwLength = pCbw->aCbwCb[4];
        MGC_MsdFormatCapacity.dwListLength = MGC_SCSI_SWAP32(8);
        MGC_MsdFormatCapacity.dwBlockCount = MGC_SCSI_SWAP32(MGC_dwMsdBlockCount);
        MGC_MsdFormatCapacity.dwBytesPerBlock = MGC_SCSI_SWAP32(MGC_wBlockSize);
        MGC_MsdTxDataIrp.pBuffer = (uint8_t *)&MGC_MsdFormatCapacity;
        MGC_MsdTxDataIrp.dwLength = sizeof(MGC_MsdFormatCapacity);
        pCsw->dCswDataResidue = dwLength ? (dwLength - MGC_MsdTxDataIrp.dwLength) : 0;
        MGC_MsdSenseData.bResponseCode = 0x70;
        bStartTx = TRUE;
        bStatus = 0;
        break;

    case MGC_SCSI_PREVENT_ALLOW_MED_REMOVE:
        pCsw->dCswDataResidue = 0L;
        MGC_MsdSenseData.bResponseCode = 0x70;
        bStatus = 0;
        break;

    case MGC_SCSI_MODE_SENSE:
        dwLength = pCbw->aCbwCb[4];
        bPage = pCbw->aCbwCb[2];
        switch(bPage & 0x3f)
        {
        case 0x1c:
        case 0x3f:
            MGC_aJunk[0] = 2;

            MGC_MsdTxDataIrp.pBuffer = (uint8_t *)MGC_aJunk;
            MGC_MsdTxDataIrp.dwLength = MUSB_MIN(dwLength, 4);
            MGC_MsdSenseData.bResponseCode = 0x70;
            bStartTx = TRUE;
            bStatus = 0;
            break;
        default:
            MGC_MsdTxDataIrp.dwLength = 0L;
            break;
        }
        pCsw->dCswDataResidue = dwLength - MGC_MsdTxDataIrp.dwLength;
        break;

    case MGC_SCSI_MODE_SELECT:
        dwLength = pCbw->aCbwCb[4];
        MGC_MsdRxDataIrp.pBuffer = MGC_aJunk;
        MGC_MsdRxDataIrp.dwLength = MUSB_MIN(dwLength, sizeof(MGC_aJunk));
        MGC_MsdSenseData.bResponseCode = 0x70;
        bStartRx = TRUE;
        bStatus = 0;
        break;

    case MGC_SCSI_READ6:
        pCsw->dCswDataResidue = 0L;
        dwLength = pCbw->aCbwCb[4];
        dwBlock = (pCbw->aCbwCb[2] << 8) | pCbw->aCbwCb[3];
        MGC_MsdTxDataIrp.pBuffer = &(MGC_aMsdData[dwBlock * MGC_wBlockSize]);
        MGC_MsdTxDataIrp.dwLength = dwDataLength;
        MUSB_DIAG2(3, "READ(6) @", dwBlock, ", count=", dwLength, 16, 0);
        /* TODO: check for overrun */
        MGC_MsdSenseData.bResponseCode = 0x70;
        bStartTx = TRUE;
        bStatus = 0;
        break;

    case MGC_SCSI_READ10:
        pCsw->dCswDataResidue = 0L;
        dwLength = (pCbw->aCbwCb[7] << 8) | pCbw->aCbwCb[8];
        dwBlock = (pCbw->aCbwCb[2] << 24) | (pCbw->aCbwCb[3] << 16) |
                  (pCbw->aCbwCb[4] << 8) | pCbw->aCbwCb[5];
        MGC_MsdTxDataIrp.pBuffer = &(MGC_aMsdData[dwBlock * MGC_wBlockSize]);
        MGC_MsdTxDataIrp.dwLength = dwDataLength;
        MUSB_DIAG2(3, "READ(10) @", dwBlock, ", count=", dwLength, 16, 0);
        /* TODO: check for overrun */
        MGC_MsdSenseData.bResponseCode = 0x70;
        bStartTx = TRUE;
        bStatus = 0;
        break;

    case MGC_SCSI_WRITE6:
        MGC_dwRxCount = 0L;
        pCsw->dCswDataResidue = 0L;
        dwLength = pCbw->aCbwCb[4];
        dwBlock = (pCbw->aCbwCb[2] << 8) | pCbw->aCbwCb[3];
        MGC_MsdRxDataIrp.pBuffer = &(MGC_aMsdData[dwBlock * MGC_wBlockSize]);
        MGC_MsdRxDataIrp.dwLength = dwDataLength;
        MUSB_DIAG2(3, "WRITE(6) @", dwBlock, ", count=", dwLength, 16, 0);
        /* check for overrun */
        if((dwBlock + dwLength) > MGC_dwMsdBlockCount)
        {
            bStatus = 1;
            MUSB_SetPipeHalt(MGC_MsdRxDataIrp.hPipe, TRUE);
        }
        else
        {
            MGC_MsdSenseData.bResponseCode = 0x70;
            bStartRx = TRUE;
            bStatus = 0;
        }
        break;

    case MGC_SCSI_WRITE10:
        MGC_dwRxCount = 0L;
        pCsw->dCswDataResidue = 0L;
        dwLength = (pCbw->aCbwCb[7] << 8) | pCbw->aCbwCb[8];
        dwBlock = (pCbw->aCbwCb[2] << 24) | (pCbw->aCbwCb[3] << 16) |
                  (pCbw->aCbwCb[4] << 8) | pCbw->aCbwCb[5];
        MGC_MsdRxDataIrp.pBuffer = &(MGC_aMsdData[dwBlock * MGC_wBlockSize]);
        MGC_MsdRxDataIrp.dwLength = dwDataLength;
        MUSB_DIAG2(3, "WRITE(10) @", dwBlock, ", count=", dwLength, 16, 0);
        /* check for overrun */
        if((dwBlock + dwLength) > MGC_dwMsdBlockCount)
        {
            bStatus = 1;
            MUSB_SetPipeHalt(MGC_MsdRxDataIrp.hPipe, TRUE);
        }
        else
        {
            MGC_MsdSenseData.bResponseCode = 0x70;
            bStartRx = TRUE;
            bStatus = 0;
        }
        break;

    case MGC_SCSI_VERIFY:
        dwLength = (pCbw->aCbwCb[7] << 8) | pCbw->aCbwCb[8];
        if(pCbw->aCbwCb[1] & 2)
        {
            MGC_dwRxCount = 0L;
            pCsw->dCswDataResidue = 0L;
            dwBlock = (pCbw->aCbwCb[2] << 24) | (pCbw->aCbwCb[3] << 16) |
                      (pCbw->aCbwCb[4] << 8) | pCbw->aCbwCb[5];
            MGC_MsdRxDataIrp.pBuffer = &(MGC_aMsdData[dwBlock * MGC_wBlockSize]);
            MGC_MsdRxDataIrp.dwLength = dwDataLength;
            MUSB_DIAG1(3, "Verify, length=", dwDataLength, 16, 8);
            MGC_MsdSenseData.bResponseCode = 0x70;
            bStartRx = TRUE;
        }
        bStatus = 0;
        break;

    case MGC_SCSI_WRITE_BUFFER:
        /* TODO */
        dwLength = (pCbw->aCbwCb[7] << 8) | pCbw->aCbwCb[8];
        MGC_MsdSenseData.bResponseCode &= ~MGC_M_MSD_SCSI_SENSE_DATA_VALID;
        bStatus = 0;
        break;

    default:
        MUSB_DIAG1(2, "MSD: unknown SCSI command ", bOpcode, 16, 02);
        bStatus = 1;
        break;
    }

    /* if bad status and host expecting data, stall IN pipe */
    if(bStatus && (pCbw->bmCbwFlags & MGC_MSD_BOT_DIR_IN))
    {
        MUSB_SetPipeHalt(MGC_MsdTxDataIrp.hPipe, TRUE);
    }

    /* start data transfer or CSW transfer */
    pCsw->dCswSignature = MGC_MSD_BOT_CSW_SIGNATURE;
    pCsw->dCswTag = pCbw->dCbwTag;
    pCsw->bCswStatus = bStatus;

    if(bStartTx)
    {
        MUSB_DIAG_STRING(3, "MSD: starting Data Tx");
        MGC_MsdTxDataIrp.dwActualLength = 0L;
        MUSB_StartTransfer(&MGC_MsdTxDataIrp);
    }
    else if(bStartRx)
    {
        MUSB_DIAG_STRING(3, "MSD: starting Data Rx");
        MGC_MsdRxDataIrp.dwActualLength = 0L;
        MUSB_StartTransfer(&MGC_MsdRxDataIrp);
    }
    else
    {
        MUSB_DIAG_STRING(3, "MSD: starting CSW Tx");
        MGC_MsdTxCswIrp.dwActualLength = 0L;
        MUSB_StartTransfer(&MGC_MsdTxCswIrp);
    }
}

static uint32_t MGC_MsdRxCbwComplete(void *pCompleteParam, MUSB_Irp *pIrp)
{
    const MGC_MsdCbw *pCbw;

    if((MUSB_STATUS_OK == pIrp->dwStatus)
            && (31 == pIrp->dwActualLength))
    {
        pCbw = (const MGC_MsdCbw *)pIrp->pBuffer;
        if(MGC_MSD_BOT_CBW_SIGNATURE == pCbw->dCbwSignature)
        {
            /* process the valid CBW */
            MGC_MsdCommand(pCbw);
            MGC_dwGoodCmdCount ++;

            return (uint32_t) - 1;
        }
    }

    /* invalid CBW: stall pipes */
    MUSB_SetPipeHalt(MGC_MsdRxCbwIrp.hPipe, TRUE);
    MUSB_SetPipeHalt(MGC_MsdTxCswIrp.hPipe, TRUE);

    /* prepare for post-stall command */
    MGC_MsdRxCbwIrp.dwActualLength = 0L;
    MUSB_StartTransfer(&MGC_MsdRxCbwIrp);
    return 0;
}

/** data reception callback */
static uint32_t MGC_MsdRxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp)
{
    /* data recvd; so send status */
    MGC_dwRxCount += MGC_MsdRxDataIrp.dwActualLength;
    /* TODO: check for data recv error */

    /* send CSW */
    MUSB_DIAG1(3, "MSD: Sending CSW after recv'd data len=", MGC_dwRxCount, 16, 8);
    MGC_Csw.dCswDataResidue = MGC_Cbw.dCbwDataTransferLength - MGC_dwRxCount;
    MGC_MsdTxCswIrp.dwActualLength = 0L;
    MUSB_StartTransfer(&MGC_MsdTxCswIrp);
    return 0;
}

/** data transmit complete callback */
static uint32_t MGC_MsdTxDataComplete(void *pCompleteParam, MUSB_Irp *pIrp)
{
    /* send CSW */
    MGC_MsdTxCswIrp.dwActualLength = 0L;
    MUSB_DIAG_STRING(3, "MSD: Starting CSW Tx");
    MUSB_StartTransfer(&MGC_MsdTxCswIrp);
    return 0;
}

/** CSW transmit complete callback */
static uint32_t MGC_MsdTxCswComplete(void *pCompleteParam, MUSB_Irp *pIrp)
{
    /* prepare for next command */
    MGC_MsdRxCbwIrp.dwActualLength = 0L;
    MUSB_DIAG_STRING(3, "MSD: Starting CBW Rx");
    MUSB_StartTransfer(&MGC_MsdRxCbwIrp);
    return 0;
}
#endif // CFG_SUPPORT_MSD
// eof


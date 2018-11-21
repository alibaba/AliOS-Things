/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb_host.h"
#include "usb_host_types.h"
#include "usb_host_core.h"
#include "usb_host_transfer.h"
#include "usb_host_request.h"
#include "usb_host_msd.h"

#define USB_MSD_CLASS    0x08
#define USB_MSD_SUBCLASS 0x06 /* Subclass Code : Transparent */
#define USB_MSD_PROTOCOL 0x50 /* Protocol Code : BOT */

#define USB_MSD_REQUEST_RESET       0xFF
#define USB_MSD_REQUEST_GET_MAX_LUN 0xFE

#define MSD_CBW_SIGNATURE   0x43425355
#define MSD_CBW_TAG         0x20304050
#define MSD_CSW_SIGNATURE   0x53425355
#define MSD_CBW_LENGTH      31
#define MSD_CSW_LENGTH      13
#define MSD_CSW_CMD_PASSED  0x00
#define MSD_CSW_CMD_FAILED  0x01
#define MSD_CSW_PHASE_ERROR 0x02

#define SCSI_CMD_TEST_UNIT_READY  0x00
#define SCSI_CMD_READ_CAPACITY10  0x25
#define SCSI_CMD_READ10           0x28
#define SCSI_CMD_WRITE10          0x2A
#define SCSI_CMD_REQUEST_SENSE    0x03
#define SCSI_CMD_INQUIRY          0x12
#define SCSI_DATA_LEN_TEST_UNIT_READY   0
#define SCSI_DATA_LEN_READ_CAPACITY10   8
#define SCSI_DATA_LEN_INQUIRY           36  
#define SCSI_DATA_LEN_REQUEST_SENSE     18

typedef struct scsi_inquiry_response {
    uint8_t DeviceType          : 5;
    uint8_t PeripheralQualifier : 3;

    uint8_t Reserved            : 7;
    uint8_t Removable           : 1;

    uint8_t Version;

    uint8_t ResponseDataFormat  : 4;
    uint8_t Reserved2           : 1;
    uint8_t NormACA             : 1;
    uint8_t TrmTsk              : 1;
    uint8_t AERC                : 1;

    uint8_t AdditionalLength;
    uint8_t Reserved3[2];

    uint8_t SoftReset           : 1;
    uint8_t CmdQue              : 1;
    uint8_t Reserved4           : 1;
    uint8_t Linked              : 1;
    uint8_t Sync                : 1;
    uint8_t WideBus16Bit        : 1;
    uint8_t WideBus32Bit        : 1;
    uint8_t RelAddr             : 1;

    uint8_t VendorID[8];
    uint8_t ProductID[16];
    uint8_t RevisionID[4]; 
} scsi_inquiry_response_t;

typedef struct msd_cbw {
    uint32_t signature;
    uint32_t tag;
    uint32_t data_length;
    uint8_t flags;
    uint8_t lun;
    uint8_t cb_length;
    uint8_t cb[16];
} msd_cbw_t;

typedef struct msd_csw {
    uint32_t signature;
    uint32_t tag;
    uint32_t data_residue;
    uint8_t status;
} msd_csw_t;

typedef struct usb_host_msd {
    uint8_t     inited;
    uint8_t     dev_connected;
    uint8_t     dev_found;
    uint8_t     nb_pipe;
    uint8_t     intf_nb;
    uint8_t     disk_inited;
    usb_host_t *phandle;
    usbh_pipe_t *bulk_in;
    usbh_pipe_t *bulk_out;
    msd_cbw_t   cbw;
    msd_csw_t   csw;
} usbh_msd_t;

/**
 * @brief Check MSD Vid and Pid
 * 
 * @param[in] vid vendor id
 * @param[in] pid product id
 *
 * @retrun NULL
 */
void usbh_msd_set_vid_pid(uint16_t vid, uint16_t pid);

/**
 * @brief Parse Interface For MSD Class
 * 
 * @param[in]  intf_nb       interface index
 * @param[in]  intf_class    interface class code
 * @param[in]  intf_subclass interface subclass code
 * @param[in]  intf_protocol interface protocol code
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_msd_parse_interface(uint8_t intf_nb, uint8_t intf_class, 
                             uint8_t intf_subclass, uint8_t intf_protocol);

/**
 * @brief Parse Endpoint(Pipe) For MSD Class
 * 
 * @param[in]  intf_nb interface index
 * @param[in]  ep_type endpoint type
 * @param[in]  ep_addr endpoint address
 * 
 * @return 0:success, otherwise is failed
 */
int usbh_msd_parse_endpoint(uint8_t intf_nb, uint8_t ep_type, uint8_t ep_addr);

/**
 * @brief MSD: Send Request Sense Request
 * 
 * @param[in]  msd_handle pointer to msd class handler
 * 
 * @return 0:success, otherwise is failed
 */
static int usbh_msd_req_request_sense(usbh_msd_t *msd_handle);

static usbh_msd_t g_usb_msd;
static usbh_os_lock_t g_rwlock;

enum_helper_t msd_helper = {
    usbh_msd_set_vid_pid,
    usbh_msd_parse_interface,
    usbh_msd_parse_endpoint,
};

static usbh_pipe_t *usbh_msd_get_pipe(usbh_msd_t *msd_handle, 
                                      uint8_t ep_type, uint8_t direction)
{
    int i;
    usbh_intf_t *intf;

    if (msd_handle->intf_nb >= USBH_MAX_NUM_INTERFACE) {
        return NULL;
    }

    intf = &(msd_handle->phandle->device.intf[msd_handle->intf_nb]);

    for (i = 0; i < USBH_MAX_PIPE_PER_INTERFACE; i++) {
        if ((intf->pipe[i]->ep_type == ep_type) && \
            ((intf->pipe[i]->ep_addr & USB_DIRECTION_MASK) == direction)) {
            return intf->pipe[i];
        }
    }

    return NULL;
}

static int usbh_msd_init(void)
{
    int res = USBH_OK;
    usb_host_t *phandle;
    usbh_msd_t *msd_handle = &g_usb_msd;

    /* If usb msd class had already been inited, just return. */
    if (msd_handle->inited) {
        return res;
    }

    /* Start to initialize the usb msd class handler structure */
    memset(msd_handle, 0, sizeof(usbh_msd_t));

    /* Get the usb host core handler */
    phandle = usbh_get_instance();
    if ((phandle == NULL) || (phandle->inited == 0)) {
        res = usbh_init();
        RETURN_ASSERT(res, "init usb host handler error");
    }

    res = usbh_os_lock_create(&g_rwlock);
    RETURN_ASSERT(res, "MSD: create r/w lock failed");

    msd_handle->phandle = phandle;
    msd_handle->intf_nb = 0xFF;
    msd_handle->inited = 1;

    return res;
}

static usbh_msd_t *usbh_msd_get_instance(void)
{
    return &g_usb_msd;
}

static void usbh_msd_disconnect(void)
{
    usbh_msd_t *msd_handle = usbh_msd_get_instance();

    usbh_os_lock_del(&g_rwlock);

    msd_handle->inited = 0;
    usbh_msd_init();

    usb_info("Unplug MSD device: VID:%04x PID:%04x [dev: %p - intf: %d]", 
            msd_handle->phandle->device.vid, msd_handle->phandle->device.pid,
            &msd_handle->phandle->device, msd_handle->intf_nb);

    usb_info("If MSD device re-plugin, please re-connect the MSD device!");
}

static int usbh_msd_req_reset(usbh_msd_t *msd_handle)
{
    usb_host_t *phandle = msd_handle->phandle;
    usb_setup_pkt_t setup;
    int res;

    usbh_os_lock(&(phandle->lock));

    setup.bmRequestType = (USB_REQUEST_TYPE_CLASS | USB_HOST_TO_DEVICE | \
                           USB_REQUEST_RECIPIENT_INTERFACE);

    setup.bRequest      =  USB_MSD_REQUEST_RESET;
    setup.wValue        =  0;
    setup.wIndex        =  msd_handle->intf_nb;
    setup.wLength       =  0;

    res = usbh_ctrl_transfer(phandle, &setup, NULL);

    usbh_os_unlock(&(phandle->lock));
    return res;
}

static int usbh_msd_req_max_lun(usbh_msd_t *msd_handle, uint8_t *buf, 
                                uint8_t length)
{
    usb_host_t *phandle = msd_handle->phandle;
    usb_setup_pkt_t setup;
    int res;

    usbh_os_lock(&(phandle->lock));

    setup.bmRequestType = (USB_REQUEST_TYPE_CLASS | USB_DEVICE_TO_HOST | \
                           USB_REQUEST_RECIPIENT_INTERFACE);

    setup.bRequest      =  USB_MSD_REQUEST_GET_MAX_LUN;
    setup.wValue        =  0;
    setup.wIndex        =  msd_handle->intf_nb;
    setup.wLength       =  length;

    res = usbh_ctrl_transfer(phandle, &setup, buf);

    usbh_os_unlock(&(phandle->lock));
    return res;
}

static int usbh_msd_bulk_transfer(usb_host_t *phandle, usbh_pipe_t *pipe, 
                                  uint8_t *data, uint16_t length)
{
    int res, resend = 0;

    do {
        res = usbh_bulk_transfer(phandle, pipe, data, length);
        switch (res) {
            case USBH_OK:
                return res;
            case USB_URB_NOTREADY:
                resend = 1;
                break;
            case USB_URB_STALL:
                return usbh_req_clear_feature(phandle, pipe->index);
            default:
                return USBH_FAIL;
        }
    } while (resend == 1);

    return USBH_OK;
}

static int usbh_msd_scsi_transfer(usbh_msd_t *msd_handle, uint8_t *cmd, 
                                  uint8_t cmd_len, int flags, uint8_t *data, 
                                  uint32_t transfer_len)
{
    int res = USBH_OK;
    usb_host_t *phandle;
    msd_cbw_t *cbw;
    msd_csw_t *csw;

    phandle = msd_handle->phandle;
    if (phandle == NULL) {
        usb_err("MSD: Host Handler is invaild");
        return USBH_FAIL;
    }

    usbh_os_lock(&g_rwlock);

    cbw = &(msd_handle->cbw);

    cbw->signature   = MSD_CBW_SIGNATURE;
    cbw->tag         = MSD_CBW_TAG;
    cbw->data_length = transfer_len;
    cbw->flags       = flags;
    cbw->lun         = 0;
    cbw->cb_length   = cmd_len;
    memset(cbw->cb, 0, sizeof(cbw->cb));

    if (cmd != NULL) {
        memcpy(cbw->cb, cmd, cmd_len);
    }

    /* Send the CBW */
    usb_dbg("MSD: Send CBW");
    res = usbh_msd_bulk_transfer(phandle, msd_handle->bulk_out, (uint8_t *)cbw, 
                                 MSD_CBW_LENGTH);

    if (res != USBH_OK) {
        goto error;
    }

    /* Data Stage if needed */
    if (data) {
        usb_dbg("SCSI Transfer Data Stage");
        if (flags == USB_HOST_TO_DEVICE) {
            while (1) {
                res = usbh_msd_bulk_transfer(phandle, msd_handle->bulk_out, 
                                             data, msd_handle->bulk_out->mps);

                if (res != USBH_OK) {
                    goto error;
                }
                if (transfer_len > msd_handle->bulk_out->mps) {
                    data += msd_handle->bulk_out->mps;
                    transfer_len -= msd_handle->bulk_out->mps;
                } else {
                    break;
                }
            }

        } else if (flags == USB_DEVICE_TO_HOST) {
            while (1) {
                res = usbh_msd_bulk_transfer(phandle, msd_handle->bulk_in, data, 
                                             msd_handle->bulk_in->mps);

                if (res != USBH_OK) {
                    goto error;
                }
                if (transfer_len > msd_handle->bulk_in->mps) {
                    data += msd_handle->bulk_in->mps;
                    transfer_len -= msd_handle->bulk_in->mps;
                } else {
                    break;
                }
            }
        }
    }

    /* Status Stage */
    csw = &(msd_handle->csw);
    csw->signature = 0;
    usb_dbg("Read CSW");

    res = usbh_msd_bulk_transfer(phandle, msd_handle->bulk_in, (uint8_t *)csw, 
                                 MSD_CSW_LENGTH);

    if (res != USBH_OK) {
        goto error;
    }

    if (csw->signature != MSD_CSW_SIGNATURE) {
        goto error;
    }

    usb_dbg("Receive CSW: Status: %d", csw->status);

    /* Mode Sense ? */
    if ((csw->status == MSD_CSW_CMD_FAILED) && (cmd[0] != SCSI_CMD_REQUEST_SENSE)) {
        usb_dbg("Request mode sense");

        usbh_os_unlock(&g_rwlock);
        return usbh_msd_req_request_sense(msd_handle);
    }

    /* Perform reset recovery */
    if ((csw->status == MSD_CSW_PHASE_ERROR) && (cmd[0] != SCSI_CMD_REQUEST_SENSE)) {
        /* Send Bulk-Only Mass Storage Reset Request */
        usbh_msd_req_reset(msd_handle);

        /* Unstall both Device Endpoint */
        usbh_req_clear_feature(phandle, msd_handle->bulk_in->index);
        usbh_req_clear_feature(phandle, msd_handle->bulk_out->index);
    }

    return csw->status;

error:
    usbh_os_unlock(&g_rwlock);
    return USBH_FAIL;
}

static int usbh_msd_req_request_sense(usbh_msd_t *msd_handle)
{
    uint8_t cmd[10] = {0};
    uint8_t result[SCSI_DATA_LEN_REQUEST_SENSE] = {0};

    cmd[0] = SCSI_CMD_REQUEST_SENSE;
    cmd[4] = SCSI_DATA_LEN_REQUEST_SENSE;

    return usbh_msd_scsi_transfer(msd_handle, cmd, sizeof(cmd), 
                                  USB_DEVICE_TO_HOST, result, 
                                  SCSI_DATA_LEN_REQUEST_SENSE);
}

static int usbh_msd_data_transfer(usbh_msd_t *msd_handle, uint8_t *buf, 
                                  uint32_t block, uint32_t nb_block, 
                                  uint32_t block_size, int direction)
{
    int res;
    uint8_t cmd[10] = {0};

    cmd[0] = (direction == USB_HOST_TO_DEVICE) ? SCSI_CMD_WRITE10 : SCSI_CMD_READ10;
    cmd[2] = (((uint8_t *)&block)[3]);
    cmd[3] = (((uint8_t *)&block)[2]);
    cmd[4] = (((uint8_t *)&block)[1]);
    cmd[5] = (((uint8_t *)&block)[0]);

    cmd[7] = (((uint8_t *)&nb_block)[1]);
    cmd[8] = (((uint8_t *)&nb_block)[0]);

    res = usbh_msd_scsi_transfer(msd_handle, cmd, sizeof(cmd), direction, buf, 
                                 block_size * nb_block);
    return res;
}

void usbh_msd_set_vid_pid(uint16_t vid, uint16_t pid)
{
    usb_dbg("we don't check VID/PID for MSD driver!");
}

int usbh_msd_parse_interface(uint8_t intf_nb, uint8_t intf_class, 
                             uint8_t intf_subclass, uint8_t intf_protocol)
{
    usbh_msd_t *msd_handle = usbh_msd_get_instance();

    if ((msd_handle->intf_nb == 0xFF) && 
        (intf_class == USB_MSD_CLASS) &&
        (intf_subclass == USB_MSD_SUBCLASS) &&
        (intf_protocol == USB_MSD_PROTOCOL)) {
        msd_handle->intf_nb = intf_nb;
        return USBH_OK;
    }

    return USBH_FAIL;
}

int usbh_msd_parse_endpoint(uint8_t intf_nb, uint8_t ep_type, uint8_t ep_addr)
{
    usbh_msd_t *msd_handle = usbh_msd_get_instance();

    if (msd_handle->intf_nb == intf_nb) {
        if (ep_type == USB_HOST_ENDPOINT_BULK) {
            msd_handle->nb_pipe++;
            if (msd_handle->nb_pipe == 2) {
                msd_handle->dev_found = 1;
            }
            return USBH_OK;
        }
    }

    return USBH_FAIL;
}

int usbh_msd_connect(void)
{
    int res = USBH_OK;
    usbh_msd_t *msd_handle;

    msd_handle = usbh_msd_get_instance();

    if (msd_handle->inited == 0) {
        res = usbh_msd_init();
        RETURN_ASSERT(res, "USB MSD Class Init Failed");
    }

    if (msd_handle->dev_connected) {
        return USBH_OK;
    }


    usb_dbg("Trying to connect MSD device.");

    res = usbh_enumerate(msd_handle->phandle, &msd_helper);
    RETURN_ASSERT(res, "USB MSD Device enumerate failed");

    if (msd_handle->dev_found == 1) {
        msd_handle->bulk_in  = usbh_msd_get_pipe(msd_handle, 
                                                 USB_HOST_ENDPOINT_BULK, 
                                                 USB_DEVICE_TO_HOST);

        msd_handle->bulk_out = usbh_msd_get_pipe(msd_handle, 
                                                 USB_HOST_ENDPOINT_BULK, 
                                                 USB_HOST_TO_DEVICE); 

        usbh_register_disconnect_callback(msd_handle->phandle, usbh_msd_disconnect);

        usb_info("New MSD device: VID:%04x PID:%04x [dev: %p - intf: %d]", 
            msd_handle->phandle->device.vid, msd_handle->phandle->device.pid,
            &msd_handle->phandle->device, msd_handle->intf_nb);

        msd_handle->dev_connected = 1;
        return USBH_OK;
    }

    return USBH_FAIL;
}

uint8_t usbh_msd_get_max_lun(void)
{
    int res;
    uint8_t buf[1] = {0};
    usbh_msd_t *msd_handle;

    msd_handle = usbh_msd_get_instance();

    res = usbh_msd_req_max_lun(msd_handle, buf, sizeof(buf));

    if (res == USBH_OK) {
        usb_dbg("USB MSD Device Max Lun: %d", buf[0]);
    } else {
        usb_err("USB MSD Device Get Max Lun failed.");
        return 0xFF;
    }

    return buf[0];
}

int usbh_msd_unit_is_ready(uint8_t lun)
{
    usbh_msd_t *msd_handle = usbh_msd_get_instance();

    usb_dbg("USB MSD: Test Unit Ready")
    return usbh_msd_scsi_transfer(msd_handle, NULL, 6, USB_DEVICE_TO_HOST, NULL, 
                                  SCSI_DATA_LEN_TEST_UNIT_READY);
}

int usbh_msd_get_inquiry(uint8_t lun, void *inquiry)
{
    int res;
    uint8_t cmd[10] = {0};
    usbh_msd_t *msd_handle = usbh_msd_get_instance();
    scsi_inquiry_response_t *response = (scsi_inquiry_response_t *)inquiry;

    cmd[0] = SCSI_CMD_INQUIRY;
    cmd[1] = (uint8_t)(lun << 5);

    cmd[4] = SCSI_DATA_LEN_INQUIRY;

    res = usbh_msd_scsi_transfer(msd_handle, cmd, sizeof(cmd), 
                                 USB_DEVICE_TO_HOST, inquiry, 
                                 SCSI_DATA_LEN_INQUIRY);
    if (res == USBH_OK) {
        usb_info("MSD - Vendor ID: %s \n", response->VendorID);
        usb_info("MSD - Product ID: %s \n", response->ProductID);
        usb_info("MSD - Product REV: %s \n", response->RevisionID);
    }

    return res;
}

int usbh_msd_disk_init(void)
{
    uint16_t i, timeout = 10;
    uint8_t lun, res;
    uint8_t inquiry[SCSI_DATA_LEN_INQUIRY] = {0};
    usbh_msd_t *msd_handle;

    msd_handle = usbh_msd_get_instance();
    if (msd_handle->dev_connected == 0) {
        usb_err("Please call usb_host_msd_connect function first!");
        return USBH_FAIL;
    }

    if (msd_handle->disk_inited == 1) {
        return USBH_OK;
    }

    lun = usbh_msd_get_max_lun();
    if (lun == 0xFF) {
        usb_err("Lun is invalid.");
        return USBH_FAIL;
    }

    for (i = 0; i < timeout; i++) {
        usbh_os_delay(100);

        if (usbh_msd_unit_is_ready(lun) == USBH_OK) {
            break;
        }
    }

    if (i == timeout) {
        msd_handle->disk_inited = 0;
        return USBH_FAIL;
    }

    res = usbh_msd_get_inquiry(lun, inquiry);
    RETURN_ASSERT(res, "USB MSD Get Inquiry Failed");

    msd_handle->disk_inited = 1;
    return USBH_OK;
}

int usbh_msd_read_capacity(uint32_t *block_size, uint32_t *block_count)
{
    uint8_t cmd[10] = {0};
    uint8_t result[SCSI_DATA_LEN_READ_CAPACITY10] = {0};
    usbh_msd_t *msd_handle;
    int status;

    usb_dbg("USB MSD: Read Capacity");
    msd_handle = usbh_msd_get_instance();
    if (msd_handle->disk_inited == 0) {
        int res = usbh_msd_disk_init();
        RETURN_ASSERT(res, "USB MSD Disk Inited Failed")
    }

    cmd[0] = SCSI_CMD_READ_CAPACITY10;

    status = usbh_msd_scsi_transfer(msd_handle, cmd, sizeof(cmd), 
                                    USB_DEVICE_TO_HOST, result, 
                                    SCSI_DATA_LEN_READ_CAPACITY10);
    if (status == USBH_OK) {
        *block_count = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3]; 
        *block_size  = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];
        usb_info("USB MSD: - block count: %u, block size: %u, Capacity: %u\r\n", 
                 (uint32_t)*block_count, (uint32_t)*block_size, 
                 (uint32_t)((*block_count) * (*block_size)));
    }

    return status;
}

int usbh_msd_block_read(uint32_t block_addr, uint8_t *buff, 
                        uint32_t block_size, uint32_t count)
{
    uint32_t idx, res;
    usbh_msd_t *msd_handle;

    msd_handle = usbh_msd_get_instance();
    if (msd_handle->disk_inited == 0) {
        res = usbh_msd_disk_init();
        RETURN_ASSERT(res, "USB MSD Disk Inited Failed")
    }

    for (idx = block_addr; idx < block_addr + count; idx++) {
        res = usbh_msd_data_transfer(msd_handle, buff, idx, 1, block_size, 
                                     USB_DEVICE_TO_HOST);
        if (res != USBH_OK) {
            return res;
        }
        buff += block_size;
    }

    return USBH_OK;
}

int usbh_msd_block_write(uint32_t block_addr, uint8_t *buff, 
                         uint32_t block_size, uint32_t count)
{
    uint32_t idx, res;
    usbh_msd_t *msd_handle;

    msd_handle = usbh_msd_get_instance();
    if (msd_handle->disk_inited == 0) {
        res = usbh_msd_disk_init();
        RETURN_ASSERT(res, "USB MSD Disk Inited Failed")
    }

    for (idx = block_addr; idx < block_addr + count; idx++) {
        res = usbh_msd_data_transfer(msd_handle, buff, idx, 1, block_size, 
                                     USB_HOST_TO_DEVICE);
        if (res != USBH_OK) {
            return res;
        }
        buff += block_size;
    }

    return USBH_OK;
}

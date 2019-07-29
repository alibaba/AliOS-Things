/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb/usb_host.h"
#include "usb/usb_host_rndis.h"

#include "usb_host_types.h"
#include "usb_host_core.h"
#include "usb_host_transfer.h"

#define USB_RNDIS_WLAN_CLASS            0xE0
#define USB_RNDIS_WLAN_SUBCLASS         0x01
#define USB_RNDIS_WLAN_PROTOCOL         0x03
#define USB_RNDIS_WLAN_DATA_CLASS       0x0A
#define USB_RNDIS_WLAN_DATA_SUBCLASS    0x00
#define USB_RNDIS_WLAN_DATA_PROTOCOL    0x00

#define USB_RNDIS_MAX_XFER_SIZE 0x00004000

typedef struct usb_host_rndis {
    uint8_t     inited;
    uint8_t     dev_connected;
    uint8_t     dev_found;
    uint8_t     nb_pipe;
    uint8_t     ctrl_intf;
    uint8_t     data_intf;
    uint8_t     dev_inited;
    usb_host_t *phandle;
    usbh_pipe_t *bulk_in;
    usbh_pipe_t *bulk_out;
    usbh_pipe_t *intr_in;
    uint32_t    request_id;
    uint32_t    dev_mps;
} usbh_rndis_t;

typedef struct rndis_message_header {
    uint32_t MessageType;
    uint32_t MessageLength;   
} rndis_message_header_t;

typedef struct rndis_keep_live_message {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
} rndis_keep_live_message_t;

typedef struct rndis_keep_live_complete {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} rndis_keep_live_complete_t;

typedef struct rndis_initialize_message {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t MaxTransferSize;
} rndis_initialize_message_t;

typedef struct rndis_initialize_complete {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    uint32_t DeviceFlags;
    uint32_t Medium;
    uint32_t MaxPacketsPerTransfer;
    uint32_t MaxTransferSize;
    uint32_t PacketAlignmentFactor;
    uint32_t Reserved[2];
} rndis_initialize_complete_t;

typedef struct rndis_set_message {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t Reserved;
    uint8_t  data[];
} rndis_set_message_t;

typedef struct rndis_set_complete {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
} rndis_set_complete_t;

typedef struct rndis_query_message {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Oid;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint32_t Reserved;
} rndis_query_message_t;

typedef struct rndis_query_complete {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t RequestId;
    uint32_t Status;
    uint32_t InformationBufferLength;
    uint32_t InformationBufferOffset;
    uint8_t  data[];
} rndis_query_complete_t;

typedef struct rndis_packet_message {
    uint32_t MessageType;
    uint32_t MessageLength;
    uint32_t DataOffset;
    uint32_t DataLength;
    uint32_t OOBDataOffset;
    uint32_t OOBDataLength;
    uint32_t NumOOBDatatElements;
    uint32_t PerPacketInfoOffset;
    uint32_t PerPacketInfoLength;
    uint32_t VcHandle;
    uint32_t Reserved;
    uint8_t  data[];
} rndis_packet_message_t;

void usbh_rndis_set_vid_pid(uint16_t vid, uint16_t pid);

int usbh_rndis_parse_interface(uint8_t intf_nb, uint8_t intf_class, 
                               uint8_t intf_subclass, uint8_t intf_protocol);

int usbh_rndis_parse_endpoint(uint8_t intf_nb, uint8_t ep_type, uint8_t ep_addr);

static usbh_rndis_t g_usb_rndis;

enum_helper_t rndis_helper = {
    usbh_rndis_set_vid_pid,
    usbh_rndis_parse_interface,
    usbh_rndis_parse_endpoint,
};

static usbh_pipe_t *usbh_rndis_get_pipe(usbh_rndis_t *rndis_handle,
                                        uint8_t ep_type, uint8_t direction)
{
    int i;
    usbh_intf_t *intf;
    uint8_t intf_nb = 0;

    if (ep_type == USB_HOST_ENDPOINT_INTERRUPT) {
        intf_nb = rndis_handle->ctrl_intf;
    } else {
        intf_nb = rndis_handle->data_intf;
    }

    if (intf_nb >= USBH_MAX_NUM_INTERFACE) {
        return NULL;
    }

    intf = &(rndis_handle->phandle->device.intf[intf_nb]);

    for (i = 0; i < USBH_MAX_PIPE_PER_INTERFACE; i++) {
        if ((intf->pipe[i]->ep_type == ep_type) && \
            ((intf->pipe[i]->ep_addr & USB_DIRECTION_MASK) == direction)) {
            return intf->pipe[i];
        }
    }

    return NULL;
}

static int usbh_rndis_init(void)
{
    int res = USBH_OK;
    usb_host_t *phandle;
    usbh_rndis_t *rndis_handle = &g_usb_rndis;

    if (rndis_handle->inited) {
        return res;
    }

    memset(rndis_handle, 0, sizeof(usbh_rndis_t));

    phandle = usbh_get_instance();
    if ((phandle == NULL) || (phandle->inited == 0)) {
        res = usbh_init();
        RETURN_ASSERT(res, "init usb host handler error");
    }

    rndis_handle->phandle = phandle;
    rndis_handle->ctrl_intf = 0xFF;
    rndis_handle->data_intf = 0xFF;
    rndis_handle->inited  = 1;

    return res;
}

static usbh_rndis_t *usbh_rndis_get_instance(void)
{
    return &g_usb_rndis;
}

static void usbh_rndis_disconnect(void)
{
    usbh_rndis_t *rndis_handle = usbh_rndis_get_instance();

    rndis_handle->inited = 0;
    usbh_rndis_init();

    usb_info("Unplug RNDIS device: VID:%04x PID:%04x [dev: %p - intf: %d]", 
            rndis_handle->phandle->device.vid, rndis_handle->phandle->device.pid,
            &rndis_handle->phandle->device, rndis_handle->ctrl_intf);

    usb_info("If RNDIS device re-plugin, please re-connect the RNDIS device!");  
}

static int usbh_rndis_send_cmd(void *cmd, uint16_t length)
{
    int res;
    usb_host_t *phandle;
    usbh_rndis_t *rndis_handle;
    usb_setup_pkt_t setup;

    rndis_handle = usbh_rndis_get_instance();
    phandle = rndis_handle->phandle;

    usbh_os_lock(&(phandle->lock));

    setup.bmRequestType = (USB_REQUEST_TYPE_CLASS | USB_HOST_TO_DEVICE | \
                           USB_REQUEST_RECIPIENT_INTERFACE);

    setup.bRequest = 0x00;
    setup.wValue = 0;
    setup.wIndex = rndis_handle->ctrl_intf;
    setup.wLength = length;

    res = usbh_ctrl_transfer(phandle, &setup, cmd);

    usbh_os_unlock(&(phandle->lock));

    return res;
}

static int usbh_rndis_recv_resp(void *resp, uint16_t length)
{
    int res;
    usb_host_t *phandle;
    usbh_rndis_t *rndis_handle;
    usb_setup_pkt_t setup;

    rndis_handle = usbh_rndis_get_instance();
    phandle = rndis_handle->phandle;

    usbh_os_lock(&(phandle->lock));

    setup.bmRequestType = (USB_REQUEST_TYPE_CLASS | USB_DEVICE_TO_HOST | \
                           USB_REQUEST_RECIPIENT_INTERFACE);

    setup.bRequest = 0x01;
    setup.wValue = 0;
    setup.wIndex = rndis_handle->ctrl_intf;
    setup.wLength = length;

    res = usbh_ctrl_transfer(phandle, &setup, resp);

    usbh_os_unlock(&(phandle->lock));

    return res;
}

static int usbh_rndis_bulk_transfer(usb_host_t *phandle, usbh_pipe_t *pipe,
                                    uint8_t *data, uint32_t length)
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
            default:
                return USBH_FAIL;
        }
    } while (resend == 1);

    return USBH_OK;
}

int usbh_rndis_connect(void)
{
    int res = USBH_OK;
    usbh_rndis_t *rndis_handle;

    rndis_handle = usbh_rndis_get_instance();

    if (rndis_handle->inited == 0) {
        res = usbh_rndis_init();
        RETURN_ASSERT(res, "USB RNDIS Class Init Failed");
    }

    if (rndis_handle->dev_connected) {
        return USBH_OK;
    }

    usb_dbg("Trying to connect RNDIS device.");

    res = usbh_enumerate(rndis_handle->phandle, &rndis_helper);
    RETURN_ASSERT(res, "USB RNDIS Device enumerate failed");

    if (rndis_handle->dev_found == 1) {
        rndis_handle->bulk_in = usbh_rndis_get_pipe(rndis_handle,
                                                    USB_HOST_ENDPOINT_BULK,
                                                    USB_DEVICE_TO_HOST);

        rndis_handle->bulk_out = usbh_rndis_get_pipe(rndis_handle,
                                                     USB_HOST_ENDPOINT_BULK,
                                                     USB_HOST_TO_DEVICE);

        rndis_handle->intr_in = usbh_rndis_get_pipe(rndis_handle,
                                                    USB_HOST_ENDPOINT_INTERRUPT,
                                                    USB_DEVICE_TO_HOST);

        usbh_register_disconnect_callback(rndis_handle->phandle, 
                                          usbh_rndis_disconnect);

        usb_info("New RNDIS device: VID:%04x PID:%04x [dev: %p - intf: %d]",
            rndis_handle->phandle->device.vid, rndis_handle->phandle->device.pid,
            &rndis_handle->phandle->device, rndis_handle->ctrl_intf);

        rndis_handle->dev_connected = 1;
        return USBH_OK;
    }

    return USBH_FAIL;
}

void usbh_rndis_set_vid_pid(uint16_t vid, uint16_t pid)
{
    usb_dbg("we don't check VID/PID for RNDIS driver!");
}

int usbh_rndis_parse_interface(uint8_t intf_nb, uint8_t intf_class,
                               uint8_t intf_subclass, uint8_t intf_protocol)
{
    usbh_rndis_t *rndis_handle = usbh_rndis_get_instance();

    if ((rndis_handle->ctrl_intf == 0xFF) && \
        (intf_class == USB_RNDIS_WLAN_CLASS) && \
        (intf_subclass == USB_RNDIS_WLAN_SUBCLASS) && \
        (intf_protocol == USB_RNDIS_WLAN_PROTOCOL)) {
        rndis_handle->ctrl_intf = intf_nb;
        return USBH_OK;
    }

    if ((rndis_handle->data_intf == 0xFF) && \
        (intf_class == USB_RNDIS_WLAN_DATA_CLASS) && \
        (intf_subclass == USB_RNDIS_WLAN_DATA_SUBCLASS) && \
        (intf_protocol == USB_RNDIS_WLAN_DATA_PROTOCOL)) {
        rndis_handle->data_intf = intf_nb;
        return USBH_OK;
    }

    return USBH_FAIL;
}

int usbh_rndis_parse_endpoint(uint8_t intf_nb, uint8_t ep_type, uint8_t ep_addr)
{
    usbh_rndis_t *rndis_handle = usbh_rndis_get_instance();

    if (rndis_handle->ctrl_intf == intf_nb) {
        if (ep_type == USB_HOST_ENDPOINT_INTERRUPT) {
            rndis_handle->nb_pipe++;
        } else {
            return USBH_FAIL;
        }
    } else if (rndis_handle->data_intf == intf_nb) {
        if (ep_type == USB_HOST_ENDPOINT_BULK) {
            rndis_handle->nb_pipe++;
        } else {
            return USBH_FAIL;
        }
    } else {
        return USBH_FAIL;
    }

    if (rndis_handle->nb_pipe == 3) {
        rndis_handle->dev_found = 1;
    }

    return USBH_OK;
}

int usbh_rndis_send_keep_alive(void)
{
    rndis_keep_live_message_t message;
    rndis_keep_live_complete_t response;
    usbh_rndis_t *rndis_handle;
    int res = USBH_OK;

    rndis_handle = usbh_rndis_get_instance();

    message.MessageType   = RNDIS_KEEPALIVE_MSG;
    message.MessageLength = (uint32_t)(sizeof(rndis_keep_live_message_t));
    message.RequestId     = rndis_handle->request_id++;

    res = usbh_rndis_send_cmd(&message, sizeof(rndis_keep_live_message_t));
    RETURN_ASSERT(res, "RNDIS: send keep alive command failed");

    res = usbh_rndis_recv_resp(&response, sizeof(rndis_keep_live_complete_t));
    RETURN_ASSERT(res, "RNDIS: receive keep alive response failed");

    if (response.Status != RNDIS_STATUS_SUCCESS) {
        return USBH_FAIL;
    }

    return USBH_OK;
}

int usbh_rndis_device_initialize(void)
{
    rndis_initialize_message_t message;
    rndis_initialize_complete_t response;
    usbh_rndis_t *rndis_handle;
    int res = USBH_OK;

    rndis_handle = usbh_rndis_get_instance();

    message.MessageType     = RNDIS_INITIALIZE_MSG;
    message.MessageLength   = (uint32_t)(sizeof(rndis_initialize_message_t));
    message.RequestId       = rndis_handle->request_id++;
    message.MajorVersion    = RNDIS_VERSION_MAJOR;
    message.MinorVersion    = RNDIS_VERSION_MINOR;
    message.MaxTransferSize = USB_RNDIS_MAX_XFER_SIZE;

    res = usbh_rndis_send_cmd(&message, sizeof(rndis_initialize_message_t));
    RETURN_ASSERT(res, "RNDIS: send initialize command failed");

    res = usbh_rndis_recv_resp(&response, sizeof(rndis_initialize_complete_t));
    RETURN_ASSERT(res, "RNDIS: recv initialize response failed");

    if (response.Status != RNDIS_STATUS_SUCCESS) {
        return USBH_FAIL;
    }

    rndis_handle->dev_mps = response.MaxTransferSize;
    return USBH_OK;
}

int usbh_rndis_set_property(uint32_t oid, void *buffer, uint32_t length)
{
    rndis_set_complete_t response;
    usbh_rndis_t *rndis_handle;
    rndis_set_message_t *msg;
    int res = USBH_OK;

    rndis_handle = usbh_rndis_get_instance();

    msg = (rndis_set_message_t *)usbh_os_malloc(sizeof(rndis_set_message_t) + \
                                                length);
    if (msg == NULL) {
        return USBH_FAIL;
    }

    msg->MessageType = RNDIS_SET_MSG;
    msg->MessageLength = (uint32_t)(sizeof(rndis_set_message_t) + length);
    msg->RequestId = rndis_handle->request_id++;
    msg->Oid = oid;
    msg->InformationBufferLength = length;
    msg->InformationBufferOffset = (uint32_t)(sizeof(rndis_set_message_t) - \
                                       sizeof(rndis_message_header_t));

    msg->Reserved = 0x00000000;
    memcpy(msg->data, buffer, length);

    res = usbh_rndis_send_cmd(msg, msg->MessageLength);
    if (res != USBH_OK) {
        goto exit;
    }

    res = usbh_rndis_recv_resp(&response, sizeof(rndis_set_complete_t));
    if (res != USBH_OK) {
        goto exit;
    }

    if (response.Status != RNDIS_STATUS_SUCCESS) {
        res = USBH_FAIL;
    }

exit:
    usbh_os_free(msg);
    return res;
}

int usbh_rndis_query_property(uint32_t oid, void *buffer, uint32_t length)
{
    rndis_query_message_t message;
    rndis_query_complete_t* resp;
    usbh_rndis_t *rndis_handle;
    uint32_t data_length;
    int res = USBH_OK;

    rndis_handle = usbh_rndis_get_instance();

    message.MessageType             = RNDIS_QUERY_MSG;
    message.MessageLength           = (uint32_t)(sizeof(rndis_query_message_t));
    message.RequestId               = rndis_handle->request_id++;
    message.Oid                     = oid;
    message.InformationBufferLength = 0x00000000;
    message.InformationBufferOffset = 0x00000000;
    message.Reserved                = 0x00000000;

    data_length = length + sizeof(rndis_query_complete_t);
    resp = (rndis_query_complete_t *)usbh_os_malloc(data_length);
    if (resp == NULL) {
        return USBH_FAIL;
    }

    res = usbh_rndis_send_cmd(&message, sizeof(rndis_query_message_t));
    if (res != USBH_OK) {
        goto exit;
    }

    res = usbh_rndis_recv_resp(resp, data_length);
    if (res != USBH_OK) {
        goto exit;
    }

    if (resp->Status != RNDIS_STATUS_SUCCESS) {
        res = USBH_FAIL;
        goto exit;
    }

    memcpy(buffer, resp->data, length);

exit:
    usbh_os_free(resp);
    return USBH_OK;
}

int usbh_rndis_read_packet(void *data, uint32_t *length)
{
    int res, size = 0;
    uint8_t *buffer;
    usb_host_t *phandle;
    usbh_pipe_t *pipe;
    usbh_rndis_t *rndis_handle;
    rndis_packet_message_t *msg;

    rndis_handle = usbh_rndis_get_instance();
    phandle = rndis_handle->phandle;
    pipe = rndis_handle->bulk_in;

    size = sizeof(rndis_packet_message_t) + ETHERNET_FRAME_SIZE_MAX;
    buffer = (uint8_t *)usbh_os_malloc(size);
    if (buffer == NULL) {
        return USBH_FAIL;
    }

    res = usbh_rndis_bulk_transfer(phandle, pipe, buffer, pipe->mps);
    if (res != USBH_OK) {
        goto error;
    }

    msg = (rndis_packet_message_t *)buffer;
    size = msg->DataLength + sizeof(rndis_packet_message_t) - pipe->mps;
    if (size < 0) {
        goto exit;
    }
    buffer += pipe->mps;

    while (1) {
        res = usbh_rndis_bulk_transfer(phandle, pipe, buffer, pipe->mps);
        if (res != USBH_OK) {
            goto error;
        }

        if (size > pipe->mps) {
            buffer += pipe->mps;
            size -= pipe->mps;
        } else {
            break;
        }
    }

exit:
    *length = msg->DataLength;
    memcpy(data, msg->data, msg->DataLength);

error:
    usbh_os_free(buffer);
    return res;
}

int usbh_rndis_send_packet(void *data, uint32_t data_size)
{
    rndis_packet_message_t *msg;
    usbh_rndis_t *rndis_handle;
    usb_host_t *phandle;
    usbh_pipe_t *pipe;
    uint32_t offset, length = 0;
    int res = USBH_OK;

    length = data_size + sizeof(rndis_packet_message_t);
    offset = sizeof(rndis_packet_message_t) - sizeof(rndis_message_header_t);
    rndis_handle = usbh_rndis_get_instance();
    phandle = rndis_handle->phandle;
    pipe = rndis_handle->bulk_out;

    msg = (rndis_packet_message_t *)usbh_os_malloc(length);
    if (msg == NULL) {
        return USBH_FAIL;
    }

    msg->MessageType = RNDIS_PACKET_MSG;
    msg->MessageLength = length;
    msg->DataOffset = offset;
    msg->DataLength = data_size;
    msg->OOBDataOffset = 0;
    msg->OOBDataLength = 0;
    msg->NumOOBDatatElements = 0;
    msg->PerPacketInfoOffset = 0;
    msg->PerPacketInfoLength = 0;
    msg->VcHandle = 0;
    msg->Reserved = 0;

    memcpy(msg->data, data, data_size);

    while (1) {
        res = usbh_rndis_bulk_transfer(phandle, pipe, (uint8_t *)msg, pipe->mps);
        if (res != USBH_OK) {
            goto exit;
        }

        if (length > pipe->mps) {
            data += pipe->mps;
            length -= pipe->mps;
        } else {
            break;
        }
    }

exit:
    usbh_os_free(msg);
    return res;
}

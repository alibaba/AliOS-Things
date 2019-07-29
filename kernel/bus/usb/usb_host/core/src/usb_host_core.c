/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb/usb_host.h"

#include "usb_host_types.h"
#include "usb_host_core.h"
#include "usb_host_pipe.h"
#include "usb_host_request.h"
#include "usb_host_device.h"

#define USB_HOST_DEVICE_CONNECTED_EVENT (1 << 0)
#define USB_HOST_DEVICE_DISCONNECTED_EVENT (1 << 1)

static usb_host_t g_usb_host;

usb_host_t *usbh_get_instance(void)
{
    return &g_usb_host;
}

static int _process_device_connected_event(usb_host_t *phandle)
{
    int i, res, timeout = 10;
    usb_dev_desc_t dev_desc;

    usbh_os_lock(&(phandle->lock));

    /* Alloc Control Pipe */
    if (phandle->control == NULL) {
        phandle->control = usbh_pipe_alloc(phandle);
        if (phandle->control == NULL) {
            return USBH_FAIL;
        }
        phandle->control->mps = USBH_PIPE_DEFAULT_MPS;
    }

    for (i = 0; i < timeout; i++) {
        /* Reset the host controller port */
        hal_usbh_port_reset(phandle->hcd);

        /* Wait for 100 ms after port reset */
        usbh_os_delay(100);

        /* Get connected device's speed */
        phandle->device.speed = hal_usbh_get_speed(phandle->hcd);

        /* Get Device Descriptor (First 8 bits) */
        res = usbh_req_get_descriptor(phandle, USB_DESC_TYPE_DEVICE, 8, (uint8_t *)&dev_desc);
        if (res != USBH_OK) {
            usb_err("Usb host main task could not read device descriptor");
            continue;
        }

        /* Update the control pipe max packet size */
        phandle->control->mps = dev_desc.bMaxPacketSize;

        /* Set an address to the device */
        res = usbh_req_set_address(phandle, USB_HOST_DEVICE_ADDRESS);
        if (res != USBH_OK) {
            usb_err("Usb host main task set device address failed");
            continue;
        }

        usbh_os_delay(10);
        phandle->device.address = USB_HOST_DEVICE_ADDRESS;

        /* 
         * Try to read again the device descriptor to check 
         * if the device answer to its new address 
         */
        res = usbh_req_get_descriptor(phandle, USB_DESC_TYPE_DEVICE, 8, (uint8_t *)&dev_desc);
        if (res == USBH_OK) {
            break;
        }

        usbh_os_delay(100);
    }

    usb_info("New Device Connected !");

    usbh_os_unlock(&(phandle->lock));
    return res;
}

static int _process_device_disconnected_event(usb_host_t *phandle)
{
    uint8_t i, j;
    usbh_intf_t *intf;

    usbh_os_lock(&(phandle->lock));

    /* Free the pipes which are alloced for the device */
    for (i = 0; i < USBH_MAX_NUM_INTERFACE; i++) {
        intf = &phandle->device.intf[i];
        if (intf->inuse == USB_HOST_INUSE) {
            for (j = 0; j < USBH_MAX_PIPE_PER_INTERFACE; j++) {
                if (intf->pipe[j]->inuse == USB_HOST_INUSE) {
                    usbh_pipe_free(phandle, intf->pipe[j]->index);
                }
            }
        }
    }

    if (phandle->device.disconnect_cb != NULL) {
        phandle->device.disconnect_cb();
    }

    memset(&(phandle->device), 0, sizeof(usbh_device_t));

    usb_info("Device Disconnected !");

    usbh_os_unlock(&(phandle->lock));

    return USBH_OK;
}

static void usbh_main_task(void *arg)
{
    usb_host_t *phandle = (usb_host_t *)arg;
    usbh_os_event_t event;

    while (1) {
        int res = usbh_os_queue_recv(&(phandle->queue), &event, 
                                        sizeof(usbh_os_event_t), 
                                        USB_HOST_WAIT_FOREVER);

        if (res != USBH_OK) {
            usb_err("Usb host receive message failed: %d", res);
            break;
        } else {
            switch (event) {
                case USB_HOST_DEVICE_CONNECTED_EVENT:
                    res = _process_device_connected_event(phandle);
                    break;
                case USB_HOST_DEVICE_DISCONNECTED_EVENT:
                    res = _process_device_disconnected_event(phandle);
                    break;
                default:
                    break;
            }
            if (res != USBH_OK) {
                usb_err("Usb host process event failed: %d", res);
                break;
            }
        }      
    }

    usbh_finalize();
}

int usbh_enumerate(usb_host_t *phandle, enum_helper_t *phelper)
{
    int res;

    usbh_os_lock(&(phandle->lock));

    if (phandle->device.attached != 1) {
        res = USBH_FAIL;
        goto exit;
    }

    /* Get the whole device descriptor to see pid, vid. */
    res = usbh_req_get_descriptor(phandle, USB_DESC_TYPE_DEVICE, 
                                  USB_DEVICE_DESCRIPTOR_LENGTH, phandle->device.data);
    if (res != USBH_OK) {
        usb_dbg("Enumerate: get whole device descriptor failed");
        goto exit;
    }

    usbh_parse_dev_desc(&(phandle->device), phandle->device.data);

    if ((phelper != NULL) && (phelper->set_vid_pid != NULL)) {
        phelper->set_vid_pid(phandle->device.vid, phandle->device.pid);
    }

    /* Get the beginning of the config descriptor to have the total length */
    res = usbh_req_get_descriptor(phandle, USB_DESC_TYPE_CONFIGURATION,
                                  USB_CONFIGURATION_DESCRIPTOR_LENGTH, phandle->device.data);
    if (res != USBH_OK) {
        usb_dbg("Enumerate: get configuration descriptor failed");
        goto exit;
    }

    uint16_t total_length = (phandle->device.data[2] | 
                            (phandle->device.data[3] << 8));

    total_length = MIN(total_length, USBH_MAX_DATA_BUFFER);
    usb_dbg("Total config descriptor length: %d \t number of interfaces: %d",
            total_length, phandle->device.data[4]);

    /* Get the whole configuration descriptor */
    res = usbh_req_get_descriptor(phandle, USB_DESC_TYPE_CONFIGURATION,
                                  total_length, phandle->device.data);
    if (res != USBH_OK) {
        usb_dbg("Enumerate: get whole configuration descriptor failed");
        goto exit;
    }

    /* Parse the configuration descriptor */
    usbh_parse_conf_desc(&(phandle->device), phandle->device.data, total_length, phelper);

    /* Set configuration if not enumerated before */
    if (!phandle->device.enumerated) {
        res = usbh_req_set_configuration(phandle, 1);
        if (res != USBH_OK) {
            usb_err("Enumerate: set configuration failed");
            goto exit;
        }
    }

    /* Mark device has already been enumerated */
    phandle->device.enumerated = 1;

exit:
    usbh_os_unlock(&(phandle->lock));
    usbh_os_delay(100);
    return res;
}

int usbh_init(void)
{
    int res = USBH_OK;
    usb_host_t *phandle = &g_usb_host;

    /* If usb host had already been inited, just return. */
    if (phandle->inited) {
        return res;
    }

    /* Start to initialize the usb host handler */
    memset(phandle, 0, sizeof(usb_host_t));

    /* Create usb host OS lock */
    res = usbh_os_lock_create(&(phandle->lock));
    RETURN_ASSERT(res, "Create usb host OS lock failed");

    /* Create usb host OS queue */
    res = usbh_os_queue_create(&(phandle->queue));
    RETURN_ASSERT(res, "Create usb host OS queue failed");

    /* Create usb host main task */
    res = usbh_os_start_task(&(phandle->task), usbh_main_task, phandle);
    RETURN_ASSERT(res, "Create usb host main task failed");

    /* Initialize HCD driver */
    res = hal_usbh_init(phandle, &phandle->hcd);
    RETURN_ASSERT(res, "HCD driver initialize failed");

    phandle->inited = true;
    return res;
}

void usbh_finalize(void)
{
    usb_host_t *phandle = &g_usb_host;

    /* Finalize HCD driver */
    hal_usbh_finalize(phandle->hcd);

    /* Delete OS Queue */
    usbh_os_queue_del(&(phandle->queue));

    /* Delete OS Lock */
    usbh_os_lock_del(&(phandle->lock));

    /* Clear the usb host handle structure */
    memset(phandle, 0, sizeof(usb_host_t));
}

void usbh_send_device_connected_event(void)
{
    usbh_os_event_t event = USB_HOST_DEVICE_CONNECTED_EVENT;
    usb_host_t *phandle = usbh_get_instance();

    if (phandle->device.attached == 0) {
        usbh_os_queue_send(&(phandle->queue), &event, sizeof(usbh_os_event_t));

        phandle->device.attached = 1;
    }
}

void usbh_send_device_disconnected_event(void)
{
    usbh_os_event_t event = USB_HOST_DEVICE_DISCONNECTED_EVENT;
    usb_host_t *phandle = usbh_get_instance();

    usbh_os_queue_send(&(phandle->queue), &event, 
                           sizeof(usbh_os_event_t));

    phandle->device.attached = 0;
}

int usbh_register_disconnect_callback(usb_host_t *phandle, void (*device_cb)(void))
{
    usbh_os_lock(&(phandle->lock));

    phandle->device.disconnect_cb = device_cb;

    usbh_os_unlock(&(phandle->lock));

    return USBH_OK;
}

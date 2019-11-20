/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb_host_types.h"
#include "usb_host_transfer.h"
#include "usb_host_pipe.h"

static int _wait_for_transfer_complete(usb_host_t *phandle, usbh_pipe_t *pipe)
{
    int res;

    do {
        res = hal_usbh_get_urb_state(phandle->hcd, pipe->index);
        if (res == USB_URB_DONE) {
            return USBH_OK;
        } else if (res == USB_URB_IDLE) {
            continue;
        } else {
            return res;
        }
    } while (1);

    return USBH_OK;
}

int usbh_ctrl_transfer(usb_host_t *phandle, usb_setup_pkt_t *setup,
                          uint8_t *buff)
{
    int res;
    usbh_pipe_t *pipe = phandle->control;
    uint8_t direction = USB_HOST_TO_DEVICE;

    /* Setup Stage */
    res = usbh_pipe_configure(phandle, pipe, direction, 
                              phandle->device.address, phandle->device.speed, 
                              USB_HOST_ENDPOINT_CONTROL, pipe->mps);
    RETURN_ASSERT(res, "setup pipe configure error");

    res = hal_usbh_submit_urb(phandle->hcd, pipe->index, direction,
                              pipe->ep_type, USBH_TOKEN_SETUP,
                              (uint8_t *)setup, sizeof(usb_setup_pkt_t));
    RETURN_ASSERT(res, "setup urb submit error");

    res = _wait_for_transfer_complete(phandle, pipe);
    RETURN_ASSERT(res, "setup stage error");

    /* Data Stage */
    if (setup->wLength > 0) {
        direction = (setup->bmRequestType & USB_DIRECTION_MASK);
        res = usbh_pipe_configure(phandle, pipe, direction,
                                  phandle->device.address, 
                                  phandle->device.speed,
                                  USB_HOST_ENDPOINT_CONTROL, pipe->mps);
        RETURN_ASSERT(res, "data pipe configure error");

        res = hal_usbh_submit_urb(phandle->hcd, pipe->index, direction,
                                  pipe->ep_type, USBH_TOKEN_DATA,
                                  buff, setup->wLength);
        RETURN_ASSERT(res, "data urb submit error");

        res = _wait_for_transfer_complete(phandle, pipe);
        RETURN_ASSERT(res, "data stage error");
    }

    /* Status Stage */
    if (setup->bmRequestType & USB_DIRECTION_MASK) {
        direction = USB_HOST_TO_DEVICE;
    } else {
        direction = USB_DEVICE_TO_HOST;
    }

    res = usbh_pipe_configure(phandle, pipe, direction,
                              phandle->device.address, phandle->device.speed,
                              USB_HOST_ENDPOINT_CONTROL, pipe->mps);
    RETURN_ASSERT(res, "status pipe configure error");

    res = hal_usbh_submit_urb(phandle->hcd, pipe->index, direction, 
                              pipe->ep_type, USBH_TOKEN_DATA, NULL, 0);
    RETURN_ASSERT(res, "status urb submit error");

    res = _wait_for_transfer_complete(phandle, pipe);
    RETURN_ASSERT(res, "status stage error");

    return USBH_OK;
}


int usbh_bulk_transfer(usb_host_t *phandle, usbh_pipe_t *pipe, uint8_t *buff, 
                       uint16_t length)
{
    int res;

    if (pipe->ep_type != USB_HOST_ENDPOINT_BULK) {
        usb_err("bulk: pipe type error");
        return USBH_FAIL;
    }

    res = hal_usbh_submit_urb(phandle->hcd, pipe->index, pipe->ep_addr, 
                              pipe->ep_type, USBH_TOKEN_DATA, buff, length);
    RETURN_ASSERT(res, "bulk: urb submit error");

    res = _wait_for_transfer_complete(phandle, pipe);
    RETURN_ASSERT(res, "bulk: transfer error");

    return USBH_OK;
}


int usbh_intr_transfer(usb_host_t *phandle, usbh_pipe_t *pipe, uint8_t *buff, 
                       uint16_t length)
{
    int res;

    if (pipe->ep_type != USB_HOST_ENDPOINT_INTERRUPT) {
        usb_err("intr: pipe type error");
        return USBH_FAIL;
    }

    res = hal_usbh_submit_urb(phandle->hcd, pipe->index, pipe->ep_addr, 
                              pipe->ep_type, USBH_TOKEN_DATA, buff, length);
    RETURN_ASSERT(res, "intr: urb submit error");

    res = _wait_for_transfer_complete(phandle, pipe);
    RETURN_ASSERT(res, "intr: transfer error");

    return USBH_OK;

}


int usbh_isoc_transfer(usb_host_t *phandle, usbh_pipe_t *pipe, uint8_t *buff, 
                       uint16_t length)
{
    int res;

    if (pipe->ep_type != USB_HOST_ENDPOINT_ISOCHRONOUS) {
        usb_err("isoc: pipe type error");
        return USBH_FAIL;
    }

    res = hal_usbh_submit_urb(phandle->hcd, pipe->index, pipe->ep_addr, 
                              pipe->ep_type, USBH_TOKEN_DATA, buff, length);
    RETURN_ASSERT(res, "isoc: urb submit error");

    res = _wait_for_transfer_complete(phandle, pipe);
    RETURN_ASSERT(res, "isoc: transfer error");

    return USBH_OK;
}

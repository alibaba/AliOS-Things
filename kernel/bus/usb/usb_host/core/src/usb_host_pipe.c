/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb_host_types.h"
#include "usb_host_pipe.h"

usbh_pipe_t *usbh_pipe_alloc(usb_host_t *phandle)
{
    uint8_t idx;

    for (idx = 0; idx < USBH_MAX_PIPE_NUM; idx++) {
        if (phandle->pipes[idx].inuse != USB_HOST_INUSE) {
            phandle->pipes[idx].index = idx;
            phandle->pipes[idx].inuse = USB_HOST_INUSE;
            return &(phandle->pipes[idx]);
        }
    }

    return NULL;
}


int usbh_pipe_free(usb_host_t *phandle, uint8_t pipe_num)
{
    int res = USBH_OK;

    if (pipe_num >= USBH_MAX_PIPE_NUM) {
        return USBH_FAIL;
    }

    res = hal_usbh_pipe_free(phandle->hcd, phandle->pipes[pipe_num].index);
    if (res != USBH_OK) {
        return USBH_FAIL;
    }

    phandle->pipes[pipe_num].inuse = USB_HOST_UNUSE;
    return res;
}



int usbh_pipe_configure(usb_host_t *phandle, usbh_pipe_t *pipe, 
                        uint8_t ep_addr, uint8_t dev_addr, 
                        uint8_t speed, uint8_t ep_type, uint16_t mps)
{
    if ((phandle == NULL) || (pipe == NULL)) {
        return USBH_FAIL;
    }

    pipe->ep_addr   = ep_addr;
    pipe->dev_addr  = dev_addr;
    pipe->speed     = speed;
    pipe->ep_type   = ep_type;
    pipe->mps       = mps;

    return hal_usbh_pipe_configure(phandle->hcd, pipe->index, 
                                   pipe->ep_addr, pipe->dev_addr, 
                                   pipe->speed, pipe->ep_type, pipe->mps);
}

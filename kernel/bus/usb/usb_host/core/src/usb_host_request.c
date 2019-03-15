/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb_host_types.h"
#include "usb_host_transfer.h"
#include "usb_host_pipe.h"

int usbh_req_clear_feature(usb_host_t *phandle, uint8_t pipe)
{
    usb_setup_pkt_t setup;
    int res;

    setup.bmRequestType = (USB_REQUEST_TYPE_STANDARD | USB_HOST_TO_DEVICE | \
                           USB_REQUEST_RECIPIENT_ENDPOINT);
    setup.bRequest      =  USB_REQUEST_CLEAR_FEATURE;
    setup.wValue        =  USB_FEATURE_SELECTOR_ENDPOINT;
    setup.wIndex        =  pipe;
    setup.wLength       =  0;

    res = usbh_ctrl_transfer(phandle, &setup, NULL);
    return res;
}


int usbh_req_get_descriptor(usb_host_t *phandle, uint16_t type,
                            uint8_t length, uint8_t *buff)
{
    usb_setup_pkt_t setup;
    int res;

    setup.bmRequestType = (USB_REQUEST_TYPE_STANDARD | USB_DEVICE_TO_HOST | \
                           USB_REQUEST_RECIPIENT_DEVICE);
    setup.bRequest      =  USB_REQUEST_GET_DESCRIPTOR;
    setup.wValue        = (type << 8);
    setup.wIndex        =  0;
    setup.wLength       = length;

    res = usbh_ctrl_transfer(phandle, &setup, buff);
    return res;

}

int usbh_req_set_address(usb_host_t *phandle, uint16_t address)
{
    usb_setup_pkt_t setup;
    int res;

    setup.bmRequestType = (USB_REQUEST_TYPE_STANDARD | USB_HOST_TO_DEVICE | \
                           USB_REQUEST_RECIPIENT_DEVICE);
    setup.bRequest      =  USB_REQUEST_SET_ADDRESS;
    setup.wValue        =  address;
    setup.wIndex        =  0;
    setup.wLength       =  0;

    res = usbh_ctrl_transfer(phandle, &setup, NULL);
    return res;
}

int usbh_req_set_configuration(usb_host_t *phandle, uint16_t conf)
{
    usb_setup_pkt_t setup;
    int res;

    setup.bmRequestType = (USB_REQUEST_TYPE_STANDARD | USB_HOST_TO_DEVICE | \
                           USB_REQUEST_RECIPIENT_DEVICE);
    setup.bRequest      =  USB_REQUEST_SET_CONFIGURATION;
    setup.wValue        =  conf;
    setup.wIndex        =  0;
    setup.wLength       =  0;

    res = usbh_ctrl_transfer(phandle, &setup, NULL);
    return res; 
}

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "usb_host_types.h"
#include "usb_host_device.h"
#include "usb_host_core.h"
#include "usb_host_pipe.h"

static usb_desc_header_t *usbh_get_next_desc(uint8_t *buf, uint8_t *ptr)
{
    usb_desc_header_t *pnext;

    *ptr += ((usb_desc_header_t *)buf)->bLength;
    pnext = (usb_desc_header_t *)((uint8_t *)buf + \
            ((usb_desc_header_t *)buf)->bLength);

    return pnext;
}

static int usbh_device_attach_interface(usbh_device_t *dev, uint16_t idx, 
                                        usb_itf_desc_t *pif)
{
    if (dev->intf[idx].inuse != USB_HOST_UNUSE)
        return USBH_FAIL;

    dev->intf[idx].inuse         = USB_HOST_INUSE;
    dev->intf[idx].intf_class    = pif->bInterfaceClass;
    dev->intf[idx].intf_subclass = pif->bInterfaceSubClass;
    dev->intf[idx].intf_protocol = pif->bInterfaceProtocol;

    return USBH_OK;
}

static int usbh_device_attach_pipe(usbh_device_t *dev, uint16_t if_idx, 
                                   uint16_t pipe_idx, usbh_pipe_t *pipe)
{
    dev->intf[if_idx].pipe[pipe_idx] = pipe;
    return USBH_OK;
}

void usbh_parse_dev_desc(usbh_device_t *device, uint8_t *data)
{
    device->dev_class    = data[4];
    device->dev_subclass = data[5];
    device->dev_protocol = data[6];
    device->vid          = LE16(data + 8);
    device->pid          = LE16(data + 10);

    usb_dbg("CLASS: %02X \t VID: %04X \t PID: %04X", device->dev_class,
            device->vid, device->pid);
}

void usbh_parse_conf_desc(usbh_device_t *dev, uint8_t *data, uint16_t len, 
                          enum_helper_t *phelper)
{
    usb_desc_header_t *pdesc;
    usb_itf_desc_t *pif;
    usb_ep_desc_t *pep;
    uint16_t ptr, parsing_intf;
    uint32_t total_len;
    int if_idx, ep_idx, res;

    if_idx = ep_idx = 0;

    /* Parse Configuration Descriptor */
    pdesc = (usb_desc_header_t *)data;
    if(pdesc->bDescriptorType == USB_DESC_TYPE_CONFIGURATION) {
        dev->nb_intf = data[4];
        total_len = LE16(data + 2);
        usb_dbg("dev: %p has %d interface", dev, dev->nb_intf);
    } else {
        usb_dbg("the buffer is not configuration descriptor");
        return;
    }

    /* Parse Interface Descriptor */
    if (len > USB_CONFIGURATION_DESCRIPTOR_LENGTH) {
        ptr = USB_CONFIGURATION_DESCRIPTOR_LENGTH;

        while ((if_idx < USBH_MAX_NUM_INTERFACE) && (ptr < total_len)) {
            pdesc = usbh_get_next_desc((uint8_t *)pdesc, (uint8_t *)&ptr);
            if (pdesc->bDescriptorType == USB_DESC_TYPE_INTERFACE) {
                pif = (usb_itf_desc_t *)pdesc;

                if (phelper->parse_interface != NULL) {
                    res = phelper->parse_interface(pif->bInterfaceNumber, 
                                                   pif->bInterfaceClass,
                                                   pif->bInterfaceSubClass,
                                                   pif->bInterfaceProtocol);
                } else {
                    res = USBH_FAIL;
                }

                if (res == USBH_OK) {
                    usbh_device_attach_interface(dev, if_idx, pif);
                    parsing_intf = 1;
                } else {
                    parsing_intf = 0;
                }

                ep_idx = 0;
                while ((ep_idx < pif->bNumEndpoints) && (ptr < total_len)) {
                    pdesc = usbh_get_next_desc((uint8_t *)pdesc, (uint8_t *)&ptr);
                    if (pdesc->bDescriptorType == USB_DESC_TYPE_ENDPOINT) {
                        if (parsing_intf) {
                            pep = (usb_ep_desc_t *)pdesc;
                            uint8_t ep_type = pep->bmAttributes & 0x03;
                            uint8_t ep_addr = pep->bEndpointAddress;
                            uint16_t mps = LE16(((uint8_t *)pep) + 4);
                            if (phelper->parse_endpoint != NULL) {
                                res = phelper->parse_endpoint(if_idx, ep_type, 
                                                              ep_addr);
                            } else {
                                res = USBH_FAIL;
                            }

                            if (res == USBH_OK) {
                                usb_host_t *phandle = usbh_get_instance();
                                usbh_pipe_t *pipe = usbh_pipe_alloc(phandle);
                                if (pipe == NULL) {
                                    usb_err("pipe alloc error");
                                }

                                res = usbh_pipe_configure(phandle, pipe, 
                                                          pep->bEndpointAddress,
                                                          dev->address,
                                                          dev->speed,
                                                          ep_type, mps);

                                if (res != USBH_OK) {
                                    usb_err("pipe configure error");
                                }

                                if ((pipe != NULL) && (dev != NULL)) {
                                    usbh_device_attach_pipe(dev, if_idx, ep_idx, 
                                                            pipe);
                                } else {
                                    usb_dbg("pipe NULL!");
                                }
                            }
                        }
                        ep_idx++;
                    }
                }
                if_idx++;
            }
        }
    }
}
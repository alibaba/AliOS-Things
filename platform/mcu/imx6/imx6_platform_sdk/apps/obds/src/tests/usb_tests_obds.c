/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file contains the basic usb test routines which include a hub test and 
 * obtaining the device descriptor mounted on the usb bus.
 *
 */
#include <string.h>
#include "io.h"
#include "sdk_types.h"
#include "obds.h"
#include "usb/usb.h"

extern void usbh_get_dev_desc(usb_module_t * port, usbhQueueHead_t * usbh_qh_ep0,
                              usbDeviceDescriptor_t * device_descriptor);
const char g_usb_otg_dev_enum_test_name[] = "USB OTG Device Enumeration Test";
const char g_usb_host1_dev_enum_test_name[] = "USB Host1 Device Enumeration Test";

int32_t usb_enum_device(usb_module_t * usbPort);

test_return_t usbo_dev_enum_test(void)
{
    usb_module_t usbModuleInfo, *usbPort;

    usbPort = &usbModuleInfo;

#ifdef CHIP_MX6SL
    usbPort->moduleName = "OTG controller1";
    usbPort->controllerID = OTG1;
#else
    usbPort->moduleName = "OTG controller";
    usbPort->controllerID = OTG;
#endif
    usbPort->phyType = Utmi;

    if (0 == usb_enum_device(usbPort))
        return TEST_PASSED;
    else
        return TEST_FAILED;
}

test_return_t usbh_dev_enum_test(void)
{
    usb_module_t usbModuleInfo, *usbPort;

    usbPort = &usbModuleInfo;

#ifdef CHIP_MX6SL
#ifdef BOARD_TYPE_EVK
    /* For EVK board, the USB HOST connecdter was routed from OTG2 */
    usbPort->moduleName = "OTG controller2";
    usbPort->controllerID = OTG2;
#endif
#else
    usbPort->moduleName = "Host controller1";
    usbPort->controllerID = Host1;
#endif
    usbPort->phyType = Utmi;

    if (0 == usb_enum_device(usbPort))
        return TEST_PASSED;
    else
        return TEST_FAILED;
}

int32_t usb_enum_device(usb_module_t * usbPort)
{
    usbPortSpeed_t usb_port_speed;  // Speed of the interface
    usbDeviceDescriptor_t *dev_descriptor;  // Pointer to the Device descriptor
    uint32_t core = (uint32_t) usbPort->controllerID;

    usbhQueueHead_t *usb_qh_ep0;    // Pointers to Queue Heads for the endpoints

    usbh_init(usbPort);
    printf("Waiting USB device connected...\n");
    while (!(HW_USBC_PORTSC1_RD(core) & BM_USBC_UH1_PORTSC1_CCS)) ;
    printf("Connect detected.\n");

    //! Reset the device
    usbh_bus_reset(usbPort);

    //! Get current operating speed.
    usb_port_speed = usb_get_port_speed(usbPort);

    /*!
     * Create a QueueHead to use for EndPoint0. This single QH will be the\n
     * asynchronous schedule during enumeration.
     */
    switch (usb_port_speed) {
    case usbSpeedFull:
        usb_qh_ep0 = usbh_qh_init(0x40, 1, EPS_FULL, 0, 0, 0);
        printf("Device connected at FULL speed\n");
        break;
    case usbSpeedLow:
        usb_qh_ep0 = usbh_qh_init(0x8, 1, EPS_LOW, 0, 0, 0);
        printf("Device connected at LOW speed\n");
        break;
    case usbSpeedHigh:
        usb_qh_ep0 = usbh_qh_init(0x40, 1, EPS_HIGH, 0, 0, 0);
        printf("Device connected at HIGH speed\n");
        break;
    default:
        return -1;
    }
    /*!
     * Put this queue head on the Asynchronous Schedule.\n
     * This is our first queue head on the AS so we point the controller to this QH\n
     * Any further queue heads will be linked to this HQ.
     */
    HW_USBC_ASYNCLISTADDR_WR(core, (uint32_t) usb_qh_ep0);

    //! Enable the asynchronous schedule
    usbh_enable_asynchronous_schedule(usbPort);
    usbh_bus_reset(usbPort);

    /*!
     * Read the first 8 bytes of the device descriptor as we do not yet know the max packet size\n
     * Our new device has address 0 at this time.
     */
    dev_descriptor = (usbDeviceDescriptor_t *) malloc(MAX_USB_DESC_SIZE);
    memset(dev_descriptor, 0, MAX_USB_DESC_SIZE);
    usbh_get_dev_desc(usbPort, usb_qh_ep0, dev_descriptor);

    /*!
     * Update the maximum packet size in the queue head
     */

    // Stop schedule to avoid contention with controller
    usbh_disable_asynchronous_schedule(usbPort);

    if (dev_descriptor->bDescriptorType == DEVICE_DESCRIPTOR) {
        printf("Device Descriptor : \n");
        printf("   bcdUSB             : 0x%02X\n", dev_descriptor->bcdUSB);
        printf("   bDeviceClass       : 0x%02X\n", dev_descriptor->bDeviceClass);
        printf("   bDeviceSubClass    : 0x%02X\n", dev_descriptor->bDeviceSubClass);
        printf("   bDeviceProtocol    : 0x%02X\n", dev_descriptor->bDeviceProtocol);
        printf("   bMaxPacketSize0    : 0x%02X\n", dev_descriptor->bMaxPacketSize);
        printf("   idVendor           : 0x%04X\n", dev_descriptor->idVendor);
        printf("   idProduct          : 0x%04X\n", dev_descriptor->idProduct);
        printf("   bchDevice          : 0x%04X\n", dev_descriptor->bcdDevice);
        printf("   iManufacturer      : %d\n", dev_descriptor->iManufacturer);
        printf("   iProduct           : %d\n", dev_descriptor->iProduct);
        printf("   iSerialNumber      : %d\n", dev_descriptor->iSerialNumber);
        printf("   bNumConfigurations : %d\n", dev_descriptor->bNumConfigurations);
    } else {
        printf("can't enumerate the attached device\n");
        free(dev_descriptor);

        return -1;
    }

    free(dev_descriptor);

    return 0;
}

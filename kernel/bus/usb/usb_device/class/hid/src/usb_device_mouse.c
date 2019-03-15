/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stdint.h"
#include "usb_device_mouse.h"
#include <assert.h>


#define MOUSE_TYPE_DEF REL_MOUSE
//#define MOUSE_TYPE_DEF ABS_MOUSE

enum MOUSE_TYPE mouse_type = MOUSE_TYPE_DEF;
static uint8_t button = 0;
static uint16_t reportLength;

#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) \
                               + (1 * INTERFACE_DESCRIPTOR_LENGTH) \
                               + (1 * HID_DESCRIPTOR_LENGTH) \
                               + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

extern uint8_t* usbd_mouse_device_desc();
extern uint8_t *usbd_mouse_string_langid_desc();
extern uint8_t *usbd_mouse_string_manufacturer_desc();
extern uint8_t * usbd_mouse_string_product_desc();
extern uint8_t *usbd_mouse_string_serial_desc();
extern uint8_t *usbd_mouse_string_configuration_desc();
extern uint8_t * usbd_mouse_string_interface_desc();
extern uint16_t usbd_mouse_report_desc_length();
extern bool usbd_mouse_send(usbd_handle_type_def *pdev, int8_t x, int8_t y, uint8_t buttons, int8_t z);
extern uint8_t *usbd_mouse_configuration_desc();
extern uint8_t * usbd_mouse_report_desc();
extern uint16_t usbd_mouse_report_desc_length();


usbd_class_type_def MOUSE_CLASS_DEF = 
{
  usbd_mouse_init,
  usbd_mouse_deinit,
  usbd_hid_callback_request,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  NULL,
  NULL,
  NULL, /*SOF */ 
  NULL,
  NULL,
  usbd_mouse_configuration_desc,
  usbd_hid_callback_set_configuration,
  NULL,
  usbd_mouse_report_desc,
  usbd_mouse_report_desc_length,
};

usbd_descriptors_type_def MOUSE_DESC = {
  usbd_mouse_device_desc,
  usbd_mouse_string_langid_desc, 
  usbd_mouse_string_manufacturer_desc,
  usbd_mouse_string_product_desc,
  usbd_mouse_string_serial_desc,
  usbd_mouse_string_configuration_desc,
  usbd_mouse_string_interface_desc,
};


void usbd_mouse_init(usbd_handle_type_def *pdev, void *data)
{
    usbd_hid_data hid_data = {
        .output_report_length = 8,
        .input_report_length = 8,
        .connect = false
    };

    usbd_hid_init(pdev, &hid_data);

    button = 0;
    usbd_connect(pdev, true);
}

uint8_t usbd_mouse_deinit(usbd_handle_type_def *pdev, void *data)
{
    return 0;
}

bool usbd_mouse_update(usbd_handle_type_def *pdev, int16_t x, int16_t y, uint8_t button, int8_t z)
{
    hid_report report;

    switch (mouse_type) {
        case REL_MOUSE:
            while (x > 127) {
                if (!usbd_mouse_send(pdev, 127, 0, button, z)) return false;
                x = x - 127;
            }
            while (x < -128) {
                if (!usbd_mouse_send(pdev, -128, 0, button, z)) return false;
                x = x + 128;
            }
            while (y > 127) {
                if (!usbd_mouse_send(pdev, 0, 127, button, z)) return false;
                y = y - 127;
            }
            while (y < -128) {
                if (!usbd_mouse_send(pdev, 0, -128, button, z)) return false;
                y = y + 128;
            }
            return usbd_mouse_send(pdev, x, y, button, z);
        case ABS_MOUSE:
            //hid_report report;

            report.data[0] = x & 0xff;
            report.data[1] = (x >> 8) & 0xff;
            report.data[2] = y & 0xff;
            report.data[3] = (y >> 8) & 0xff;
            report.data[4] = -z;
            report.data[5] = button & 0x07;

            report.length = 6;

            return usbd_hid_send(pdev, &report);
        default:
            return false;
    }
}

bool usbd_mouse_send(usbd_handle_type_def *pdev, int8_t x, int8_t y, uint8_t buttons, int8_t z) {
    hid_report report;
    report.data[0] = buttons & 0x07;
    report.data[1] = x;
    report.data[2] = y;
    report.data[3] = -z; // >0 to scroll down, <0 to scroll up

    report.length = 4;

    return usbd_hid_send(pdev, &report);
}

bool usbd_mouse_move(usbd_handle_type_def *pdev, int16_t x, int16_t y) {
    return usbd_mouse_update(pdev, x, y, button, 0);
}

bool usbd_mouse_scroll(usbd_handle_type_def *pdev, int8_t z) {
    return usbd_mouse_update(pdev, 0, 0, button, z);
}


bool usbd_mouse_double_click(usbd_handle_type_def *pdev) {
    if (!usbd_mouse_click(pdev, MOUSE_LEFT))
        return false;
    aos_msleep(100);
    return usbd_mouse_click(pdev, MOUSE_LEFT);
}

bool usbd_mouse_click(usbd_handle_type_def *pdev, uint8_t button) {
    if (!usbd_mouse_update(pdev, 0, 0, button, 0))
        return false;
    aos_msleep(10);
    return usbd_mouse_update(pdev, 0, 0, 0, 0);
}

bool usbd_mouse_press(usbd_handle_type_def *pdev, uint8_t button_) {
    button = button_ & 0x07;
    return usbd_mouse_update(pdev, 0, 0, button, 0);
}

bool usbd_mouse_release(usbd_handle_type_def *pdev, uint8_t button_) {
    button = (button & (~button_)) & 0x07;
    return usbd_mouse_update(pdev, 0, 0, button, 0);
}

#if (MOUSE_TYPE_DEF == REL_MOUSE)
static uint8_t reportDescriptor[] = {
            USAGE_PAGE(1),      0x01,       // Genric Desktop
            USAGE(1),           0x02,       // Mouse
            COLLECTION(1),      0x01,       // Application
            USAGE(1),           0x01,       // Pointer
            COLLECTION(1),      0x00,       // Physical

            REPORT_COUNT(1),    0x03,
            REPORT_SIZE(1),     0x01,
            USAGE_PAGE(1),      0x09,       // Buttons
            USAGE_MINIMUM(1),       0x1,
            USAGE_MAXIMUM(1),       0x3,
            LOGICAL_MINIMUM(1),     0x00,
            LOGICAL_MAXIMUM(1),     0x01,
            INPUT(1),           0x02,
            REPORT_COUNT(1),    0x01,
            REPORT_SIZE(1),     0x05,
            INPUT(1),           0x01,

            REPORT_COUNT(1),    0x03,
            REPORT_SIZE(1),     0x08,
            USAGE_PAGE(1),      0x01,
            USAGE(1),           0x30,       // X
            USAGE(1),           0x31,       // Y
            USAGE(1),           0x38,       // scroll
            LOGICAL_MINIMUM(1),     0x81,
            LOGICAL_MAXIMUM(1),     0x7f,
            INPUT(1),           0x06,       // Relative data

            END_COLLECTION(0),
            END_COLLECTION(0),
};
#elif (MOUSE_TYPE_DEF == ABS_MOUSE)
static uint8_t reportDescriptor[] = {
            USAGE_PAGE(1), 0x01,           // Generic Desktop
            USAGE(1), 0x02,                // Mouse
            COLLECTION(1), 0x01,           // Application
            USAGE(1), 0x01,                // Pointer
            COLLECTION(1), 0x00,           // Physical

            USAGE_PAGE(1), 0x01,            // Generic Desktop
            USAGE(1), 0x30,                 // X
            USAGE(1), 0x31,                 // Y
            LOGICAL_MINIMUM(1), 0x00,       // 0
            LOGICAL_MAXIMUM(2), 0xff, 0x7f, // 32767
            REPORT_SIZE(1), 0x10,
            REPORT_COUNT(1), 0x02,
            INPUT(1), 0x02,                 // Data, Variable, Absolute

            USAGE_PAGE(1), 0x01,            // Generic Desktop
            USAGE(1), 0x38,                 // scroll
            LOGICAL_MINIMUM(1), 0x81,       // -127
            LOGICAL_MAXIMUM(1), 0x7f,       // 127
            REPORT_SIZE(1), 0x08,
            REPORT_COUNT(1), 0x01,
            INPUT(1), 0x06,                 // Data, Variable, Relative

            USAGE_PAGE(1), 0x09,            // Buttons
            USAGE_MINIMUM(1), 0x01,
            USAGE_MAXIMUM(1), 0x03,
            LOGICAL_MINIMUM(1), 0x00,       // 0
            LOGICAL_MAXIMUM(1), 0x01,       // 1
            REPORT_COUNT(1), 0x03,
            REPORT_SIZE(1), 0x01,
            INPUT(1), 0x02,                 // Data, Variable, Absolute
            REPORT_COUNT(1), 0x01,
            REPORT_SIZE(1), 0x05,
            INPUT(1), 0x01,                 // Constant

            END_COLLECTION(0),
            END_COLLECTION(0)
};
#endif

uint8_t * usbd_mouse_report_desc()
{
    if ((mouse_type == REL_MOUSE) || (mouse_type == ABS_MOUSE)) {
        reportLength = sizeof(reportDescriptor);
        return reportDescriptor;
    }
    return NULL;
}

uint16_t usbd_mouse_report_desc_length() {
    usbd_mouse_report_desc();
    return reportLength;
}

uint8_t * usbd_mouse_string_interface_desc() {
    static uint8_t stringIinterfaceDescriptor[] = {
        0x0A,               //bLength
        STRING_DESCRIPTOR,  //bDescriptorType 0x03
        'M',0,'O',0,'U',0,'S',0,'E',0,  //bString iInterface
    };
    return stringIinterfaceDescriptor;
}

uint8_t *usbd_mouse_string_product_desc() {
    static uint8_t stringIproductDescriptor[] = {
        0x18,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'M',0,'O',0,'U',0,'S',0,'E',0,' ',0,'D',0,'E',0,'V',0,'I',0,'C',0,'E',0 //bString iProduct - MOUSE device
    };
    return stringIproductDescriptor;
}

uint8_t *usbd_mouse_string_langid_desc() {
    static const uint8_t stringLangidDescriptor[] = {
        0x04,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        0x09,0x04,          /*bString Lang ID - 0x0409 - English*/
    };
    return (uint8_t *)stringLangidDescriptor;
}

uint8_t *usbd_mouse_string_manufacturer_desc() {
    static const uint8_t stringImanufacturerDescriptor[] = {
        0x12,                                            /*bLength*/
        STRING_DESCRIPTOR,                               /*bDescriptorType 0x03*/
        'a',0,'b',0,'c',0,'d',0,'.',0,'o',0,'r',0,'g',0, /*bString iManufacturer*/
    };
    return stringImanufacturerDescriptor;
}

uint8_t *usbd_mouse_string_serial_desc() {
    static const uint8_t stringIserialDescriptor[] = {
        0x16,                                                           /*bLength*/
        STRING_DESCRIPTOR,                                              /*bDescriptorType 0x03*/
        '0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,    /*bString iSerial - 0123456789*/
    };
    return stringIserialDescriptor;
}

uint8_t *usbd_mouse_string_configuration_desc() {
    static const uint8_t stringIconfigurationDescriptor[] = {
        0x06,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        '0',0,'1',0,        /*bString iConfiguration - 01*/
    };
    return stringIconfigurationDescriptor;
}

uint8_t *usbd_mouse_device_desc()
{
    static uint8_t deviceDescriptor[] = {
        DEVICE_DESCRIPTOR_LENGTH,       /* bLength */
        DEVICE_DESCRIPTOR,              /* bDescriptorType */
        LSB(USB_VERSION_2_0),           /* bcdUSB (LSB) */
        MSB(USB_VERSION_2_0),           /* bcdUSB (MSB) */
        0x00,                           /* bDeviceClass */
        0x00,                           /* bDeviceSubClass */
        0x00,                           /* bDeviceprotocol */
        MAX_PACKET_SIZE_EP0,            /* bMaxPacketSize0 */
        (uint8_t)(LSB(MOUSE_VENDOR_ID)),                 /* idVendor (LSB) */
        (uint8_t)(MSB(MOUSE_VENDOR_ID)),                 /* idVendor (MSB) */
        (uint8_t)(LSB(MOUSE_PRODUCT_ID)),                /* idProduct (LSB) */
        (uint8_t)(MSB(MOUSE_PRODUCT_ID)),                /* idProduct (MSB) */
        (uint8_t)(LSB(MOUSE_PRODUCT_RELEASE)),           /* bcdDevice (LSB) */
        (uint8_t)(MSB(MOUSE_PRODUCT_RELEASE)),           /* bcdDevice (MSB) */
        STRING_OFFSET_IMANUFACTURER,    /* iManufacturer */
        STRING_OFFSET_IPRODUCT,         /* iProduct */
        STRING_OFFSET_ISERIAL,          /* iSerialNumber */
        0x01                            /* bNumConfigurations */
    };
    return deviceDescriptor;
}

uint8_t *usbd_mouse_configuration_desc() {
    static uint8_t configurationDescriptor[] = {
        CONFIGURATION_DESCRIPTOR_LENGTH,    // bLength
        CONFIGURATION_DESCRIPTOR,           // bDescriptorType
        LSB(TOTAL_DESCRIPTOR_LENGTH),       // wTotalLength (LSB)
        MSB(TOTAL_DESCRIPTOR_LENGTH),       // wTotalLength (MSB)
        0x01,                               // bNumInterfaces
        DEFAULT_CONFIGURATION,              // bConfigurationValue
        0x00,                               // iConfiguration
        C_RESERVED | C_SELF_POWERED,        // bmAttributes
        C_POWER(0),                         // bMaxPower

        INTERFACE_DESCRIPTOR_LENGTH,        // bLength
        INTERFACE_DESCRIPTOR,               // bDescriptorType
        0x00,                               // bInterfaceNumber
        0x00,                               // bAlternateSetting
        0x02,                               // bNumEndpoints
        HID_CLASS,                          // bInterfaceClass
        HID_SUBCLASS_BOOT,                  // bInterfaceSubClass
        HID_PROTOCOL_MOUSE,                 // bInterfaceProtocol
        0x00,                               // iInterface

        HID_DESCRIPTOR_LENGTH,              // bLength
        HID_DESCRIPTOR,                     // bDescriptorType
        LSB(HID_VERSION_1_11),              // bcdHID (LSB)
        MSB(HID_VERSION_1_11),              // bcdHID (MSB)
        0x00,                               // bCountryCode
        0x01,                               // bNumDescriptors
        REPORT_DESCRIPTOR,                  // bDescriptorType
        (uint8_t)(LSB(sizeof(reportDescriptor))), // wDescriptorLength (LSB)
        (uint8_t)(MSB(sizeof(reportDescriptor))), // wDescriptorLength (MSB)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        PHY_TO_DESC(EPINT_IN),              // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (LSB)
        MSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        PHY_TO_DESC(EPINT_OUT),             // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (LSB)
        MSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)
    };
    return configurationDescriptor;
}


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "stdint.h"
#include "aos/hal/usbd.h"
#include "usb_device_hid.h"
#include <assert.h>


static hid_report outputReport;
static uint8_t output_length = 64;
static uint8_t input_length = 64;
static uint16_t reportLength;

#define OUTPUT_LENGTH   8
#define INPUT_LENGTH    8


#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) \
                               + (1 * INTERFACE_DESCRIPTOR_LENGTH) \
                               + (1 * HID_DESCRIPTOR_LENGTH) \
                               + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

extern uint8_t* usbd_hid_device_desc();
extern uint8_t *usbd_hid_string_langid_desc();
extern uint8_t *usbd_hid_string_manufacturer_desc();
extern uint8_t * usbd_hid_string_product_desc();
extern uint8_t *usbd_hid_string_serial_desc();
extern uint8_t *usbd_hid_string_configuration_desc();
extern uint8_t * usbd_hid_string_interface_desc();
extern uint8_t *usbd_hid_configuration_desc();
extern uint8_t *usbd_hid_report_desc();
extern uint16_t usbd_hid_report_desc_length();


usbd_class_type_def HID_CLASS_DEF = 
{
  usbd_hid_init,
  usbd_hid_deinit,
  usbd_hid_callback_request,
  NULL, /*EP0_TxSent*/  
  NULL, /*EP0_RxReady*/
  NULL,
  NULL,
  NULL, /*SOF */ 
  NULL,
  NULL,
  usbd_hid_configuration_desc,
  usbd_hid_callback_set_configuration,
  NULL,
  usbd_hid_report_desc,
  usbd_hid_report_desc_length,
};

usbd_descriptors_type_def HID_DESC = {
  usbd_hid_device_desc,
  usbd_hid_string_langid_desc, 
  usbd_hid_string_manufacturer_desc,
  usbd_hid_string_product_desc,
  usbd_hid_string_serial_desc,
  usbd_hid_string_configuration_desc,
  usbd_hid_string_interface_desc,
};

void usbd_hid_init(usbd_handle_type_def *pdev, void *data)
{
    usbd_hid_data *hid_data = (usbd_hid_data *)data;

    output_length = hid_data->output_report_length;
    input_length = hid_data->input_report_length;
    if(hid_data->connect) {
        usbd_connect(pdev, true);
    }
}

uint8_t usbd_hid_deinit(usbd_handle_type_def *pdev, void *data)
{
    return 0;
}

bool usbd_hid_send(usbd_handle_type_def *pdev, hid_report *report)
{
    return usbd_write(pdev, EPINT_IN, report->data, report->length, MAX_HID_REPORT_SIZE);
}

bool usbd_hid_send_nb(usbd_handle_type_def *pdev, hid_report *report)
{
    return usbd_write_nb(pdev, EPINT_IN, report->data, report->length, MAX_HID_REPORT_SIZE);
}

bool usbd_hid_read(usbd_handle_type_def *pdev, hid_report *report)
{
    uint32_t bytesRead = 0;
    bool result;
    result = usbd_read_ep(pdev, EPINT_OUT, report->data, &bytesRead, MAX_HID_REPORT_SIZE);
    if(!usbd_read_start(pdev, EPINT_OUT, MAX_HID_REPORT_SIZE))
        return false;
    report->length = bytesRead;
    return result;
}

bool usbd_hid_read_nb(usbd_handle_type_def *pdev, hid_report *report)
{
    uint32_t bytesRead = 0;
    bool result;
    result = usbd_read_ep_nb(pdev, EPINT_OUT, report->data, &bytesRead, MAX_HID_REPORT_SIZE);
    // if readEP_NB did not succeed, does not issue a readStart
    if (!result)
        return false;
    report->length = bytesRead;
    if(!usbd_read_start(pdev, EPINT_OUT, MAX_HID_REPORT_SIZE))
        return false;
    return result;
}

uint16_t usbd_hid_report_desc_length() {
    usbd_hid_report_desc();
    return reportLength;
}


/* Called in ISR context
   Called by USBDevice on Endpoint0 request
   This is used to handle extensions to standard requests
   and class specific requests
   Return true if class handles this request
*/
bool usbd_hid_callback_request(usbd_handle_type_def *pdev)
{
    bool success = false;
    control_transfer_t * transfer = &pdev->transfer;
    uint8_t *hidDescriptor;

    // Process additional standard requests

    if ((transfer->setup.bmRequestType.Type == STANDARD_TYPE)) {
        switch (transfer->setup.bRequest) {
            case GET_DESCRIPTOR:
                switch (DESCRIPTOR_TYPE(transfer->setup.wValue)) {
                    case REPORT_DESCRIPTOR:
                        if ((pdev->pClass->report_desc() != NULL) \
                            && (pdev->pClass->report_desc_length() != 0)) {
                            transfer->remaining = pdev->pClass->report_desc_length();
                            transfer->ptr = pdev->pClass->report_desc();
                            transfer->direction = DEVICE_TO_HOST;
                            success = true;
                        }
                        break;
                    case HID_DESCRIPTOR:
                            // Find the HID descriptor, after the configuration descriptor
                            hidDescriptor = usbd_find_descriptor(pdev, HID_DESCRIPTOR);
                            if (hidDescriptor != NULL) {
                                transfer->remaining = HID_DESCRIPTOR_LENGTH;
                                transfer->ptr = hidDescriptor;
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                            }
                            break;

                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    // Process class-specific requests

    if (transfer->setup.bmRequestType.Type == CLASS_TYPE) {
        switch (transfer->setup.bRequest) {
             case SET_REPORT:
                // First byte will be used for report ID
                outputReport.data[0] = transfer->setup.wValue & 0xff;
                outputReport.length = transfer->setup.wLength + 1;

                transfer->remaining = sizeof(outputReport.data) - 1;
                transfer->ptr = &outputReport.data[1];
                transfer->direction = HOST_TO_DEVICE;
                transfer->notify = true;
                success = true;
            default:
                break;
        }
    }

    return success;
}

/* Called in ISR context
   Set configuration. Return false if the
   configuration is not supported
*/
bool usbd_hid_callback_set_configuration(usbd_handle_type_def *pdev, uint8_t configuration) {
    if (configuration != DEFAULT_CONFIGURATION) {
        return false;
    }

    // Configure endpoints > 0
    usbd_add_endpoint(pdev, EPINT_IN, MAX_PACKET_SIZE_EPINT);
    usbd_add_endpoint(pdev, EPINT_OUT, MAX_PACKET_SIZE_EPINT);

    // We activate the endpoint to be able to recceive data
    usbd_read_start(pdev, EPINT_OUT, MAX_PACKET_SIZE_EPINT);
    return true;
}

uint8_t *usbd_hid_string_interface_desc() {
    static uint8_t stringIinterfaceDescriptor[] = {
        0x08,               //bLength
        STRING_DESCRIPTOR,  //bDescriptorType 0x03
        'H',0,'I',0,'D',0,  //bString iInterface - HID
    };
    return stringIinterfaceDescriptor;
}

uint8_t *usbd_hid_string_product_desc() {
    static uint8_t stringIproductDescriptor[] = {
        0x16,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'H',0,'I',0,'D',0,' ',0,'D',0,'E',0,'V',0,'I',0,'C',0,'E',0 //bString iProduct - HID device
    };
    return stringIproductDescriptor;
}

uint8_t *usbd_hid_string_langid_desc() {
    static const uint8_t stringLangidDescriptor[] = {
        0x04,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        0x09,0x04,          /*bString Lang ID - 0x0409 - English*/
    };
    return (uint8_t *)stringLangidDescriptor;
}

uint8_t *usbd_hid_string_manufacturer_desc() {
    static const uint8_t stringImanufacturerDescriptor[] = {
        0x12,                                            /*bLength*/
        STRING_DESCRIPTOR,                               /*bDescriptorType 0x03*/
        'a',0,'b',0,'c',0,'d',0,'.',0,'o',0,'r',0,'g',0, /*bString iManufacturer*/
    };
    return stringImanufacturerDescriptor;
}

uint8_t *usbd_hid_string_serial_desc() {
    static const uint8_t stringIserialDescriptor[] = {
        0x16,                                                           /*bLength*/
        STRING_DESCRIPTOR,                                              /*bDescriptorType 0x03*/
        '0',0,'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,    /*bString iSerial - 0123456789*/
    };
    return stringIserialDescriptor;
}

uint8_t *usbd_hid_string_configuration_desc() {
    static const uint8_t stringIconfigurationDescriptor[] = {
        0x06,               /*bLength*/
        STRING_DESCRIPTOR,  /*bDescriptorType 0x03*/
        '0',0,'1',0,        /*bString iConfiguration - 01*/
    };
    return stringIconfigurationDescriptor;
}

uint8_t *usbd_hid_device_desc()
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
        (uint8_t)(LSB(VENDOR_ID)),                 /* idVendor (LSB) */
        (uint8_t)(MSB(VENDOR_ID)),                 /* idVendor (MSB) */
        (uint8_t)(LSB(PRODUCT_ID)),                /* idProduct (LSB) */
        (uint8_t)(MSB(PRODUCT_ID)),                /* idProduct (MSB) */
        (uint8_t)(LSB(PRODUCT_RELEASE)),           /* bcdDevice (LSB) */
        (uint8_t)(MSB(PRODUCT_RELEASE)),           /* bcdDevice (MSB) */
        STRING_OFFSET_IMANUFACTURER,    /* iManufacturer */
        STRING_OFFSET_IPRODUCT,         /* iProduct */
        STRING_OFFSET_ISERIAL,          /* iSerialNumber */
        0x01                            /* bNumConfigurations */
    };
    return deviceDescriptor;
}

static uint8_t reportDescriptor[] = {
        USAGE_PAGE(2), LSB(0xFFAB), MSB(0xFFAB),
        USAGE(2), LSB(0x0200), MSB(0x0200),
        COLLECTION(1), 0x01, // Collection (Application)

        REPORT_SIZE(1), 0x08, // 8 bits
        LOGICAL_MINIMUM(1), 0x00,
        LOGICAL_MAXIMUM(1), 0xFF,

        REPORT_COUNT(1), INPUT_LENGTH,
        USAGE(1), 0x01,
        INPUT(1), 0x02, // Data, Var, Abs

        REPORT_COUNT(1), OUTPUT_LENGTH,
        USAGE(1), 0x02,
        OUTPUT(1), 0x02, // Data, Var, Abs

        END_COLLECTION(0),
};

uint8_t *usbd_hid_report_desc() {
    reportLength = sizeof(reportDescriptor);
    return reportDescriptor;
}

uint8_t *usbd_hid_configuration_desc() {
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
        HID_SUBCLASS_NONE,                  // bInterfaceSubClass
        HID_PROTOCOL_NONE,                  // bInterfaceProtocol
        0x00,                               // iInterface

        HID_DESCRIPTOR_LENGTH,              // bLength
        HID_DESCRIPTOR,                     // bDescriptorType
        LSB(HID_VERSION_1_11),              // bcdHID (LSB)
        MSB(HID_VERSION_1_11),              // bcdHID (MSB)
        0x00,                               // bCountryCode
        0x01,                               // bNumDescriptors
        REPORT_DESCRIPTOR,                  // bDescriptorType
        (uint8_t)(LSB(sizeof(reportDescriptor))), // wDescriptorLength (LSB) 0x1b
        (uint8_t)(MSB(sizeof(reportDescriptor))), // wDescriptorLength (MSB)usbd_hid_report_desc_length

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


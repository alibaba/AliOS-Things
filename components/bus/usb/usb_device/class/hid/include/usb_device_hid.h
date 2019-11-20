/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef USB_DEVICE_HID_H
#define USB_DEVICE_HID_H


#ifdef __cplusplus
 extern "C" {
#endif


/* These headers are included for child class. */
#include "usb_device_endpoints.h"
#include "usb_device_descriptor.h"
#include "usb_device_types.h"
#include "usb_device_hid_types.h"
#include "usb_device.h"


#define VENDOR_ID        0x1234
#define PRODUCT_ID       0x0006
#define PRODUCT_RELEASE  0x0001

typedef struct _usbd_hid_data_def
{
  uint8_t output_report_length;
  uint8_t input_report_length;
  bool connect;
} usbd_hid_data;


/**
 * @brief Initialize hid device
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	data	point to user data
 * 
 * @return none
 */
void usbd_hid_init(usbd_handle_type_def *pdev, void *data);

/**
 * @brief Deinitialize hid device
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	data	point to user data
 * 
 * @return true is ok, false is false
 */
uint8_t usbd_hid_deinit(usbd_handle_type_def *pdev, void *data);

/**
 * @brief Send a report with blocking
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	report	report which will be sent
 * 
 * @return true is ok, false is false
 */
bool usbd_hid_send(usbd_handle_type_def *pdev, hid_report *report);

/**
 * @brief Send a report with non blocking
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	report	report which will be sent
 * 
 * @return true is ok, false is false
 */
bool usbd_hid_send_nb(usbd_handle_type_def *pdev, hid_report *report);

/**
 * @brief Read a report with blocking
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	report	report pointer to the report to fill
 * 
 * @return true is ok, false is false
 */
bool usbd_hid_read(usbd_handle_type_def *pdev, hid_report *report);

/**
 * @brief Read a report with non blocking
 * 
 * @param[in]	pdev	point to usb device handler
 * @param[in]	report	report pointer to the report to fill
 * 
 * @return true is ok, false is false
 */
bool usbd_hid_read_nb(usbd_handle_type_def *pdev, hid_report *report);

/**
 * @brief Called by usb device on Endpoint0 request
 * 
 * @param[in]	pdev	point to usb device handler
 * 
 * @return true is ok, false is false
 */
bool usbd_hid_callback_request(usbd_handle_type_def *pdev);

/**
 * @brief Called by usb device layer, set configuration of the device.
 * 
 * @param[in]	pdev			point to usb device handler
 * @param[in]	configuration	number of the configuration
 * 
 * @return true is ok, false is false
 */
bool usbd_hid_callback_set_configuration(usbd_handle_type_def *pdev, uint8_t configuration);

#ifdef __cplusplus
}
#endif

#endif /* USB_DEVICE_HID_H */
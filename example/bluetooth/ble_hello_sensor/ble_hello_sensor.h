/**
 ******************************************************************************
 * @file    ble_hello_sensor.h
 * @author  William Xu
 * @version V1.0.0
 * @date    17-Feb-2016
 * @file     This file provides GATT UUID and handler definitions for BLE
 *           peripheral demo device
 * ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 **/

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *                                Constants
 ******************************************************************************/

#define SPP_UART_ONE_PACKAGE_LENGTH         (128)

#define SPP_UART_DRIVER_BUFFER_LENGTH       (1024)

#define SPP_UART_RECV_TIMEOUT               (50)


/******************************************************************************
 *                          Constants
 ******************************************************************************/

/* UUID value of the Hello Sensor Service */
#define UUID_HELLO_SERVICE                      0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x51, 0x82, 0x7e, 0x1b

/* UUID value of the Hello Sensor Characteristic, Value Notification */
#define UUID_HELLO_CHARACTERISTIC_NOTIFY        0x26, 0xf6, 0x69, 0x91, 0x68, 0xee, 0xc2, 0xbe, 0x44, 0x4d, 0xb9, 0x5c, 0x3f, 0x2d, 0xc3, 0x8a

/* UUID value of the Hello Sensor Characteristic, Color Configuration */
#define UUID_HELLO_CHARACTERISTIC_COLOR         0x1a, 0x89, 0x07, 0x4a, 0x2f, 0x3b, 0x7e, 0xa6, 0x81, 0x44, 0x3f, 0xf9, 0xa8, 0xf2, 0x9b, 0x5e

/* UUID value of the SPP Service */
#define UUID_SPP_SERVICE                        0x5E, 0x67, 0x21, 0x8A, 0x3f, 0x4b, 0x4D, 0x32, 0x91, 0x36, 0x38, 0xE3, 0xD8, 0xED, 0x63, 0x71

/* UUID value of the SPP Characteristic, Data In */
#define UUID_SPP_SERVICE_CHARACTERISTIC_IN      0x45, 0x39, 0x3E, 0x90, 0x24, 0x1D, 0x21, 0x78, 0x32, 0x70, 0x21, 0x35, 0xB4, 0xBA, 0xAE, 0xE2

/* UUID value of the SPP Characteristic, Data OUT */
#define UUID_SPP_SERVICE_CHARACTERISTIC_OUT     0x32, 0x15, 0x1a, 0x5e, 0x82, 0x2e, 0x12, 0x2a, 0x91, 0x43, 0x27, 0x52, 0xba, 0x1d, 0xf3, 0x30

/******************************************************************************
 *                         Type Definitions
 ******************************************************************************/

typedef enum {
    HDLS_GENERIC_ATTRIBUTE = 0x1,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED,
    HDLC_GENERIC_ATTRIBUTE_SERVICE_CHANGED_VALUE,

    HDLS_GENERIC_ACCESS = 0x14,
    HDLC_GENERIC_ACCESS_DEVICE_NAME,
    HDLC_GENERIC_ACCESS_DEVICE_NAME_VALUE,

    HDLC_GENERIC_ACCESS_APPEARANCE,
    HDLC_GENERIC_ACCESS_APPEARANCE_VALUE,

    HDLS_HELLO_SENSOR = 0x30,
    HDLC_HELLO_SENSOR_NOTIFY,
    HDLC_HELLO_SENSOR_NOTIFY_VALUE,
    HDLC_HELLO_SENSOR_NOTIFY_CFG_DESC,

    HDLC_HELLO_SENSOR_COLOR,
    HDLC_HELLO_SENSOR_COLOR_VALUE,

    HDLS_SPP = 0x40,
    HDLC_SPP_IN,
    HDLC_SPP_IN_VALUE,
    HDLC_SPP_IN_DESCRIPTION,

    HDLC_SPP_OUT,
    HDLC_SPP_OUT_VALUE,
    HDLC_SPP_OUT_DESCRIPTION,

    HDLS_DEV_INFO = 0x50,
    HDLC_DEV_INFO_MFR_NAME,
    HDLC_DEV_INFO_MFR_NAME_VALUE,

    HDLC_DEV_INFO_MODEL_NUM,
    HDLC_DEV_INFO_MODEL_NUM_VALUE,

    HDLC_DEV_INFO_SYSTEM_ID,
    HDLC_DEV_INFO_SYSTEM_ID_VALUE,

    HDLS_BAT = 0x60,
    HDLC_BAT_LEVEL,
    HDLC_BAT_LEVEL_VALUE,

} hello_sensor_db_tags;




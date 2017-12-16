/**
 ******************************************************************************
 * @file    ble_advertisements.h
 * @author  Jian Zhang
 * @version V1.1.0
 * @date    24-Aug-2016
 * @file    BLE advertisement demonstration via command line interface
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
 *  BLE Vendor Specific Device
 *
 * Features demonstrated
 *  - GATT database and Device configuration initialization
 *  - Processing user command over UART to control BLE Advertisements.
 *  - Demonstrate advertisement type and filter policy by UART command line.
 *
 *   To demonstrate the demo, work through the following steps.
 *   1. Plug the MiCOKit board into your computer, open a serial terminal
 *   2. Build and download the application (to the MiCOKit board)
 *   3. Type command "advert start" and "advert stop" on serial terminal to control
 *      BLE Advertisements.
 *   4. Turn on your a Central device and connect to this borad. This Central device
 *      is added to the white list now.
 *   5. Disconnect and re-advert.
 *   6. Type command "policy" on serial terminal to control BLE Advertisements
 *      filter policy - using white list or not. You can
 *   7. Now you can observe the filter policy.
 *   8. Type command "wlsize" to get a size of white list.
 *
 ******************************************************************************
 **/
#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 *                                Constants
 ******************************************************************************/

/******************************************************************************
 *                                Constants
 ******************************************************************************/

/******************************************************************************
 *                             Type Definitions
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

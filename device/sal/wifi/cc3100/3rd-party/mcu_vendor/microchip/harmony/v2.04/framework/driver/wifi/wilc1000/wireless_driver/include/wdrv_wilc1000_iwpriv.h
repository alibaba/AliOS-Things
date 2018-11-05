/*******************************************************************************
  WILC1000 Private Configuration Support

  Company:
    Microchip Technology Inc.
  
  File Name:
    wdrv_wilc1000_iwpriv.h

  Summary:
    Configure optional (private) parameters of WILC1000 driver.

  Description:
    Functions in this module support the connection process for the
    WILC1000.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc. All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef _WDRV_WILC1000_IWPRIV_H
#define _WDRV_WILC1000_IWPRIV_H

#include "wdrv_wilc1000_main.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

typedef enum {
    IWPRIV_ERROR = -1,
    IWPRIV_READY,
    IWPRIV_IN_PROGRESS
} IWPRIV_STATUS;

typedef enum {
    IWPRIV_SCAN_SUCCESSFUL,
    IWPRIV_SCAN_IDLE,
    IWPRIV_SCAN_IN_PROGRESS,
    IWPRIV_SCAN_NO_AP_FOUND
} IWPRIV_SCAN_STATUS;

typedef enum {
    IWPRIV_CONNECTION_FAILED = -1,
    IWPRIV_CONNECTION_SUCCESSFUL,
    IWPRIV_CONNECTION_IDLE,
    IWPRIV_CONNECTION_IN_PROGRESS,
    IWPRIV_CONNECTION_REESTABLISHED
} IWPRIV_CONN_STATUS;

typedef struct {
    void *result;
    bool prescanAllowed;
    bool prescanFinished;
    IWPRIV_SCAN_STATUS scanStatus;
    uint16_t numberOfResults;
    uint16_t index;
    IWPRIV_STATUS displayStatus;
    IWPRIV_STATUS saveStatus;
} IWPRIV_PARAM_SCAN;

typedef struct {
    void *config;
} IWPRIV_PARAM_CONFIG;

typedef struct {
    uint8_t *ssid;
    uint8_t ssidLen;
} IWPRIV_PARAM_SSID;

typedef struct {
    uint8_t type;
} IWPRIV_PARAM_NETWORKTYPE;

typedef struct {
    bool initConnAllowed;
    IWPRIV_CONN_STATUS status;
} IWPRIV_PARAM_CONNECT;

typedef struct {
    uint8_t *addr; // it usually points to a MAC address, which is an array of 6 uint8_t elements
    bool updated;
} IWPRIV_PARAM_CLIENTINFO;

typedef struct {
    void *info;
} IWPRIV_PARAM_DEVICEINFO;

typedef struct {
    bool isOpen;
} IWPRIV_PARAM_DRIVERSTATUS;

typedef struct {
    bool requested;
} IWPRIV_PARAM_FWUPGRADE;

typedef struct {
    bool isServer;
} IWPRIV_PARAM_OPERATIONMODE;

typedef struct {
    void *context;
} IWPRIV_PARAM_CONTEXT;

typedef struct {
    uint8_t *addr; // it usually points to a MAC address, which is an array of 6 uint8_t elements
    IWPRIV_STATUS status;
} IWPRIV_PARAM_MULTICASTFILTER;

typedef struct {
    bool enabled;
} IWPRIV_PARAM_POWERSAVE;

typedef union {
    IWPRIV_PARAM_SCAN scan;
    IWPRIV_PARAM_CONFIG cfg;
    IWPRIV_PARAM_SSID ssid;
    IWPRIV_PARAM_NETWORKTYPE netType;
    IWPRIV_PARAM_CONNECT conn;
    IWPRIV_PARAM_CLIENTINFO clientInfo;
    IWPRIV_PARAM_DEVICEINFO devInfo;
    IWPRIV_PARAM_DRIVERSTATUS driverStatus;
    IWPRIV_PARAM_FWUPGRADE fwUpgrade;
    IWPRIV_PARAM_OPERATIONMODE opMode;
} IWPRIV_GET_PARAM;

typedef union {
    IWPRIV_PARAM_SCAN scan;
    IWPRIV_PARAM_CONFIG cfg;
    IWPRIV_PARAM_SSID ssid;
    IWPRIV_PARAM_NETWORKTYPE netType;
    IWPRIV_PARAM_CONNECT conn;
    IWPRIV_PARAM_CONTEXT ctx;
    IWPRIV_PARAM_MULTICASTFILTER multicast;
    IWPRIV_PARAM_POWERSAVE powerSave;
} IWPRIV_SET_PARAM;

typedef union {
    IWPRIV_PARAM_SCAN scan;
} IWPRIV_EXECUTE_PARAM;

typedef enum {
    PRESCAN_OPTION_GET,
    PRESCAN_OPTION_SET,
    PRESCAN_START,
    PRESCAN_ISFINISHED_GET,
    SCAN_START,
    SCANSTATUS_GET,
    SCANRESULT_GET,
    SCANRESULTS_COUNT_GET,
    SCANRESULTS_DISPLAY,
    SCANRESULTS_SAVE,
    CONFIG_GET,
    CONFIG_SET,
    SSID_GET,
    SSID_SET,
    NETWORKTYPE_GET,
    NETWORKTYPE_SET,
    CONNSTATUS_GET,
    CLIENTINFO_GET,
    DEVICEINFO_GET,
    DRVSTATUS_GET,
    FWUPGRADEREQUEST_GET,
    OPERATIONMODE_GET,
    INITCONN_OPTION_SET,
    ADHOCCTX_SET,
    MULTICASTFILTER_SET,
    POWERSAVE_SET,
} IWPRIV_CMD;

void iwpriv_get(IWPRIV_CMD cmd, IWPRIV_GET_PARAM *params);
void iwpriv_set(IWPRIV_CMD cmd, IWPRIV_SET_PARAM *params);
void iwpriv_execute(IWPRIV_CMD cmd, IWPRIV_EXECUTE_PARAM *params);

#if defined(WDRV_WILC1000_IWPRIV_TASK)

//*******************************************************************************
/*
  Function:
        void iwpriv_prescan_start(void)

  Summary:
    Starts prescan.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function directs the Wi-Fi driver to start a prescan.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_prescan_start(void);

//*******************************************************************************
/*
  Function:
        bool iwpriv_prescan_isfinished(void)

  Summary:
    Checks if the prescan is complete.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function checks if the prescan is complete.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    None.

  Remarks:
    None.
 */
bool iwpriv_prescan_isfinished(void);

//*******************************************************************************
/*
  Function:
        void iwpriv_prescan_option_set(bool scan)

  Summary:
    To run prescan or not.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function controls whether or not to run prescan.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    scan - true: run prescan before next connection
           false: do not run prescan before next connection

  Returns:
    None.

  Remarks:
    Prescan means the scan runs before the module is connected. It needs
    to use multiple functions in this file. Please refer to the Easy Configuration
    demonstration to see the correct usage of prescan.

    After the the module is connected, WILC1000 module can also do regular scans.
    But it cannot perform a scan when the connection is in progress.
 */
void iwpriv_prescan_option_set(bool scan);

//*******************************************************************************
/*
  Function:
        bool iwpriv_prescan_option_get(void)

  Summary:
    To see if prescan will run before next connection.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function checks whether or not the prescan will run before next connection.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    None.

  Remarks:
    None.
 */
bool iwpriv_prescan_option_get(void);

//*******************************************************************************
/*
  Function:
        void iwpriv_scan_start(void)

  Summary:
    Starts scan.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    The function starts a Wi-Fi scan.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_scan_start(void);

//*******************************************************************************
/*
  Function:
        IWPRIV_SCAN_STATUS iwpriv_scanstatus_get(void)

  Summary:
    Gets the prescan status.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the prescan status.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    Prescan status: IWPRIV_SCAN_IDLE, IWPRIV_SCAN_IN_PROGRESS,
    IWPRIV_SCAN_NO_AP_FOUND or IWPRIV_SCAN_SUCCESSFUL. See the definition for the
    IWPRIV_SCAN_STATUS structure.

  Remarks:
    None.
 */
IWPRIV_SCAN_STATUS iwpriv_scanstatus_get(void);

//*******************************************************************************
/*
  Function:
        uint16_t iwpriv_numberofscanresults_get(void)

  Summary:
    Gets the number of scan results.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the number of scan results.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    Number of scan results.

  Remarks:
    None.
 */
uint16_t iwpriv_numberofscanresults_get(void);

//*******************************************************************************
/*
  Function:
        void *iwpriv_scanresult_get(uint16_t index)

  Summary:
    Gets one scan result.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets one scan result, which includes the detailed information
    of an AP.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    index - index of the scan result

  Returns:
    Pointer to where the scan result is written. It uses the g_scanResults[]
    array to store results, which eliminates the need to allocate additional memory.

  Remarks:
    None.
 */
void *iwpriv_scanresult_get(uint16_t index);

//*******************************************************************************
/*
  Function:
        IWPRIV_STATUS iwpriv_scanresults_display(void)

  Summary:
    Outputs all of the scan results.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function outputs all of the scan results to the console terminal.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    Status of the output operation, IWPRIV_IN_PROGRESS or IWPRIV_READY. See
    definition for the IWPRIV_STATUS structure.

  Remarks:
    None.
 */
IWPRIV_STATUS iwpriv_scanresults_display(void);

//*******************************************************************************
/*
  Function:
        IWPRIV_STATUS iwpriv_scanresults_save(void)

  Summary:
    Saves all of the scan results.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function saves all of the scan results to the console terminal.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    Status of the save operation, IWPRIV_IN_PROGRESS or IWPRIV_READY. See
    definition for the IWPRIV_STATUS structure.

  Remarks:
    None.
 */
IWPRIV_STATUS iwpriv_scanresults_save(void);

//*******************************************************************************
/*
  Function:
        IWPRIV_CONN_STATUS iwpriv_connstatus_get(void)

  Summary:
    Gets the Wi-Fi connection status.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the Wi-Fi connection status.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    Status of current Wi-Fi connection. See the definition for the
    IWPRIV_CONN_STATUS structure.

  Remarks:
    IWPRIV_CONNECTION_FAILED does not necessarily mean that the module fails
    to connect to the network. It stands on the application's perspective,
    and actually can be customized. For example, in the Web Server demonstrations's use case,
    WDRV_CSTATE_CONNECTION_NOT_CONNECTED is treated as a fail case and
    will trigger the application to restart.
 */
IWPRIV_CONN_STATUS iwpriv_connstatus_get(void);

//*******************************************************************************
/*
  Function:
        void iwpriv_config_read(void *wifi_cfg)

  Summary:
    Reads the Wi-Fi context configuration.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the current Wi-Fi context configuration, copies and stores
    the whole structure to the pointer passed to the function.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    wifi_cfg - pointer to where the context configuration is written

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_config_read(void *wifi_cfg);

//*******************************************************************************
/*
  Function:
        void iwpriv_config_write(void *wifi_cfg)

  Summary:
    Writes to the Wi-Fi context configuration which is currently used by Wi-Fi
    driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads from a passed pointer, copies everything from it, and
    writes to the Wi-Fi context configuration, which is currently used by the Wi-Fi
    driver.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    wifi_cfg - pointer to where the context configuration is stored

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_config_write(void *wifi_cfg);

//*******************************************************************************
/*
  Function:
        bool iwpriv_is_servermode(void)

  Summary:
    Checks if the passed Wi-Fi context configuration is operating in server
    mode.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function checks if the passed Wi-Fi context configuration is operating
    in server mode, which includes Ad-Hoc mode and Soft AP mode.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    - true  - Wi-Fi context configuration is operating in server mode
    - false - Wi-Fi context configuration is not operating in server mode

  Remarks:
    None.
 */
bool iwpriv_is_servermode(void);

//*******************************************************************************
/*
  Function:
        void iwpriv_devinfo_get(void *info)

  Summary:
    Gets the device information.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the device information.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    info - pointer to where the device information is written

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_devinfo_get(void *info);

//*******************************************************************************
/*
  Function:
        void iwpriv_ssid_set(uint8_t *ssid, uint8_t ssidLen)

  Summary:
    Sets the current SSID.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the current SSID.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    ssid - pointer to where the SSID is stored
    ssidLen - pointer to where the SSID length is stored

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_ssid_set(uint8_t *ssid, uint8_t ssidLen);

//*******************************************************************************
/*
  Function:
        void iwpriv_ssid_get(uint8_t *ssid, uint8_t *ssidLen)

  Summary:
    Gets the current SSID.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the current SSID.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    ssid - pointer to where the SSID is written
    ssidLen - pointer to where the SSID length is written

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_ssid_get(uint8_t *ssid, uint8_t *ssidLen);

//*******************************************************************************
/*
  Function:
        void iwpriv_nettype_set(uint8_t netType)

  Summary:
    Sets the current network type.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the current network type.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    netType - the network type to set

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_nettype_set(uint8_t netType);

//*******************************************************************************
/*
  Function:
        void iwpriv_nettype_get(uint8_t *netType)

  Summary:
    Gets the current network type.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the current network type.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    netType - pointer to where the network type is written

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_nettype_get(uint8_t *netType);

//*******************************************************************************
/*
  Function:
        IWPRIV_STATUS iwpriv_mcastfilter_set(uint8_t *addr)

  Summary:
    Adds a MAC address to the multi-cast filter.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function adds a MAC address to the multi-cast filter.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    addr - pointer to where the MAC address is stored

  Returns:
    Status of the set operation, IWPRIV_READY or IWPRIV_ERROR. See definition for
    the IWPRIV_STATUS structure.

  Remarks:
    None.
 */
IWPRIV_STATUS iwpriv_mcastfilter_set(uint8_t *addr);

//*******************************************************************************
/*
  Function:
        void iwpriv_powersave_config(bool enabled)

  Summary:
    Enables or disables Power Save mode in Wi-Fi driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables or disables Power Save mode in Wi-Fi driver, which depends on
    the passed boolean value.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    enabled - boolean value which indicates to enable or disable Power Save mode
              in Wi-Fi driver

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_powersave_config(bool enabled);

//*******************************************************************************
/*
  Function:
        void iwpriv_initialconn_set(bool connect)

  Summary:
    Sets the initial connection status of Wi-Fi driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the initial connection status of Wi-Fi driver. After
    Wi-Fi initialization, it decides whether or not to start the Wi-Fi connection.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    connect - boolean value which indicates whether or not to start an initial connect

  Returns:
    None.

  Remarks:
    This function is mainly used to implement prescan. It has to be called
    before Wi-Fi driver's initialization is finished to be effective.
 */
void iwpriv_initialconn_set(bool connect);

//*******************************************************************************
/*
  Function:
        void iwpriv_leftclient_get(bool *updated, TCPIP_MAC_ADDR *addr)

  Summary:
    Gets the left client's information.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the left client's information when the Wi-Fi module works
    in server mode and has the DHCP Server enabled.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    updated - if the left client's information needs to be updated
    addr - MAC address of the left client

  Returns:
    None.

  Remarks:
    None.
 */
void iwpriv_leftclient_get(bool *updated, TCPIP_MAC_ADDR *addr);

//*******************************************************************************
/*
  Function:
        uint8_t iwpriv_initstatus_get(void)

  Summary:
    Gets the initialization status of Wi-Fi driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the initialization status of the Wi-Fi driver.

  Precondition:
    None.

  Returns:
    Current initialization status of the Wi-Fi driver (IWPRIV_READY or IWPRIV_IN_PROGRESS).

  Remarks:
    None.
 */
uint8_t iwpriv_initstatus_get(void);

#endif /* WDRV_WILC1000_IWPRIV_TASK */

// DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif /* _WDRV_WILC1000_IWPRIV_H */

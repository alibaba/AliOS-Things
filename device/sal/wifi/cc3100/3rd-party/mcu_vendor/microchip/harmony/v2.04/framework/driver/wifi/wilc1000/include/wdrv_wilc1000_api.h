/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    wdrv_wilc1000_api.h

  Summary:
    WILC1000 wireless driver APIs.

  Description:
    WILC1000 wireless driver APIs.
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

#ifndef _WDRV_WILC1000_API_H
#define _WDRV_WILC1000_API_H

#include "wdrv_wilc1000_common.h"
#include "wdrv_wilc1000_stub.h"

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_Initialize(WDRV_HOOKS const *const ehooks, bool initWait)

  Summary:
    Initializes the WILC1000 Wi-Fi driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function initializes the WILC1000 Wi-Fi driver, making it ready for
    clients to use.

  Precondition:
    None.

  Parameters:
    ehooks - pointer to WDRV layer hooks
    initWait - true will put WDRV in wait during initialization

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_Initialize(WDRV_HOOKS const *const ehooks, bool initWait);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_Initialize_bare(WDRV_HOOKS const *const hooks)

  Summary:
    Initializes the WILC1000 Wi-Fi driver when used with external TCP/IP.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function initializes the WILC1000 Wi-Fi driver, making it ready for
    clients to use.

  Precondition:
    None.

  Parameters:
    hooks - pointer to WDRV layer hooks

  Returns:
    success (0) or fail (-1).

  Remarks:
    None.
 */
int32_t WDRV_EXT_Initialize_bare(WDRV_HOOKS const *const hooks);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_Deinitialize(void)

  Summary:
    Deinitializes the WILC1000 Wi-Fi driver.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function deinitializes the WILC1000 driver.

  Precondition:
    None.

  Returns:
    None.

  Remarks:
    None
 */
void WDRV_EXT_Deinitialize(void);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_DataSend(uint16_t segSize, uint8_t *p_segData)

  Summary:
    Sends data packets to WILC1000 module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sends data packets to the WILC1000 module.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    seqSize - data size
    p_seqData - pointer to the data buffer

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_DataSend(uint16_t segSize, uint8_t *p_segData);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdSSIDSet(uint8_t *ssid, uint16_t len)

  Summary:
    Sets the SSID.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the SSID and SSID length.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    ssid - pointer to SSID buffer
    len - number of bytes in SSID

  Returns:
    None.

  Remarks:
    Do not include a string terminator in the SSID length. SSIDs are
    case-sensitive. SSID length must be less than or equal to 32.
 */
void WDRV_EXT_CmdSSIDSet(uint8_t *ssid, uint16_t len);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdNetModeBSSSet(void)

  Summary:
    Sets the Wi-Fi network type to Infrastructure.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the Wi-Fi network type to Infrastructure.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdNetModeBSSSet(void);


//*******************************************************************************
/*
  Function:
        void WDRV_IsDisconnectRequestedSet(bool bValue)

  Summary:
    Sets the IsDisconnect requested indicator
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the IsDisconnect requested indicator to indicate if reconnect is needed.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_IsDisconnectRequestedSet(bool bValue);

//*******************************************************************************
/*
  Function:
        void WDRV_ConnectionStateSet(WDRV_CONNECTION_STATES eState)

  Summary:
    Sets the connection state
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the current connection state.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_ConnectionStateSet(WDRV_CONNECTION_STATES eState);


//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdNetModeAPSet(void)

  Summary:
    Sets the Wi-Fi network type to SoftAP.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the Wi-Fi network type to SoftAP.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdNetModeAPSet(void);

//*******************************************************************************
/*
  Function:
        int8_t WDRV_EXT_CmdNetModeGet(void)

  Summary:
    Get the Wi-Fi network type .
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function get  the Wi-Fi network type.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    network type: 1 (AP mode), 0 (Sta mode)..

  Remarks:
    None.
 */
int8_t WDRV_EXT_CmdNetModeGet(void);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdConnect(void)

  Summary:
    Directs the WILC1000 to connect to a Wi-Fi network.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function causes the WILC1000 to connect to a Wi-Fi network. Upon
    connection, or a failure to connect, an event will be generated.

  Precondition:
    Wi-Fi initialization must be complete and relevant connection parameters
    must have been set.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdConnect(void);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdDisconnect(void)

  Summary:
    Directs the WILC1000 to disconnect from a Wi-Fi network.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function causes the WILC1000 to disconnect from a Wi-Fi network.

  Precondition:
    Wi-Fi initialization must be complete and a connection must be in
    progress.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdDisconnect(void);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdSecNoneSet(void)

  Summary:
    Sets Wi-Fi security to open (no security).
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the Wi-Fi security to open. One can only connect to
    an AP that is running in open mode.

  Precondition:
    Wi-Fi initialization must be complete and in an unconnected state.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdSecNoneSet(void);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdSecWEPSet(uint8_t *key, uint16_t len)

  Summary:
    Sets Wi-Fi security to use WEP.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the Wi-Fi security to WEP. One can only connect to
    an AP that is running the same WEP mode.

  Precondition:
    Wi-Fi initialization must be complete and in an unconnected state.

  Parameters:
    key - pointer to the WEP key buffer
    len - WEP key length

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdSecWEPSet(uint8_t *key, uint16_t len);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdSecWPASet(uint8_t *key, uint16_t len)

  Summary:
    Sets Wi-Fi security to use WPA/WPA2.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the Wi-Fi security to WPA/WPA2. One can only
    connect to an AP that is running the same WPA/WPA2 mode.

  Precondition:
    Wi-Fi initialization must be complete and in an unconnected state.

  Parameters:
    key - pointer to the WPA key buffer
    len - WPA key length

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdSecWPASet(uint8_t *key, uint16_t len);

//*******************************************************************************
/*
  Function:
        int32_t WDRV_EXT_CmdSecWpsSet(bool pinMode, uint8_t *key, uint16_t keyLen)

  Summary:
    Sets Wi-Fi security to use WPS.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the Wi-Fi security to WPS. One can only connect to
    an AP that supports WPS.

  Precondition:
    Wi-Fi initialization must be complete and in an unconnected state.

  Parameters:
    pinMode - 0: PBC mode; 1: PIN mode
    key - pointer of the PIN buffer
    keyLen - PIN length

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None
 */
uint32_t WDRV_EXT_CmdSecWpsSet(bool pinMode, uint8_t *key, uint16_t keyLen);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdPowerSavePut(bool enable, uint8_t mode, uint16_t listenInterval)

  Summary:
    Puts the module in power save mode.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    The function places the module in power save mode.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    enable - true will put the module in power save mode.
    mode - 0 : manual mode - not synchronized to AP beacon ; 1. deep automatic mode - ieee802.11 power save mode.
    listenInterval - STA wakes up per this beacon interval.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    This works only with Infrastructure mode.  Do not call this in other modes.
 */
uint32_t WDRV_EXT_CmdPowerSavePut(bool enable, uint8_t mode, uint16_t listenInterval);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdTxPowerSet(uint32_t level)

  Summary:
    Sets the TX Power at 3 levels, high, medium and low.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    The function sets the module's TX power.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    level - 1 : high - 18 dBm PA gain , 2 : medium - 12 dBm PA gain, 3 : low - 6 dBm PA gain.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdTxPowerSet(uint32_t level);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdScanOptionSet(uint8_t numOfSlots, uint8_t slotTime, uint8_t probesPerSlot, uint8_t rssiThreshold);

  Summary:
    Sets scan options.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    The function sets scan options.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    numOfSlots - The min number of slots is 2 for every channel, every slot the module will send Probe Request on air, and wait/listen for PROBE RESP/BEACONS for the slotTime.
    slotTime - The time that the module will wait on every channel listening to the frames on air.
    probesPerSlot - Number of probe requests to be sent per channel scan slot.
    rssiThreshold - The RSSI threshold of the AP which will be connected to directly.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdScanOptionSet(uint8_t numOfSlots, uint8_t slotTime, uint8_t probesPerSlot, uint8_t rssiThreshold);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdSSIDGet(uint8_t *ssid, uint8_t *length)

  Summary:
    Gets the SSID.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function returns the SSID and SSID Length.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    ssid - pointer to buffer where SSID will be written
    length - number of bytes in SSID

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdSSIDGet(uint8_t *ssid, uint8_t *length);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdConnectContextBssidGet(uint8_t *bssId)

  Summary:
    Gets the BSSID
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function gets the current AP's BSSID.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    bssId - pointer where the current AP's BSSID will be written

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdConnectContextBssidGet(uint8_t *bssId);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdMacAddressGet(uint8_t *MacAddr)

  Summary:
    Retrieves the WILC1000 MAC address.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function retrieves the WILC1000 MAC address.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    MacAddr - Pointer where MAC address will be written (must point to a
              6 bytes buffer)

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdMacAddressGet(uint8_t *MacAddr);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdFWVersionGet(uint32_t *major, uint32_t *minor, uint32_t *patch);

  Summary:
    Retrieves FW version information.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function retrieves the module FW version information.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    major - pointer where the major number will be written
    minor - pointer where the minor number will be written
    patch - pointer where the patch number will be written

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdFWVersionGet(uint32_t *major, uint32_t *minor, uint32_t *patch);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdChannelSet(uint16_t channel)

  Summary:
    Sets the channel on which to operate.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function sets the channel on which to operate.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    channel - target channel

  Returns:
    None.

  Remarks:
    This works only with SoftAP mode.  Do not call this in other modes.
 */
void WDRV_EXT_CmdChannelSet(uint16_t channel);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_MulticastFilterSet(uint8_t *addr)

  Summary:
     Sets a multicast address filter.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function allows the application to configure up to 8 Multicast
    address filters on the WILC1000 module.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    addr - the pointer of the multicast mac address.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_MulticastFilterSet(uint8_t *addr);

//*******************************************************************************
/*
  Function:
        uint32_t WDRV_EXT_CmdScanStart(void)

  Summary:
    Directs the WILC1000 module to start a scan.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function directs the WILC1000 module to start a scan.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_CmdScanStart(void);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdScanGet(uint16_t *numOfResults)

  Summary:
    Reads the number of scan results from the WILC1000 module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function reads the number of scan results from the WILC1000 module.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    numOfResults - pointer where the number of scan results will be written

  Returns:
   None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdScanGet(uint16_t *numOfResults);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_ScanResultGet(uint8_t listIndex, WDRV_SCAN_RESULT *p_scanResult)

  Summary:
    Reads the selected scan results back from the WILC1000 module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    After a scan has completed this function is used to read one scan
    result at a time from the WILC1000 module.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    listIndex - index (0 based list) of the scan entry to retrieve
    p_scanResult - pointer to where scan result is written

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
void WDRV_EXT_ScanResultGet(uint8_t listIndex, WDRV_SCAN_RESULT *p_scanResult);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_ScanDoneSet(void)

  Summary:
    Indicates when a scan has completed.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function indicates when a scan has completed.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_ScanDoneSet(void);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_ScanIsInProgress(void)

  Summary:
    Check whether host scan is now in progress or not.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    Check whether host scan is now in progress or not.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    - true  - Host scan is in progress
    - false - Host scan is not in progress

  Remarks:
    None.
 */
bool WDRV_EXT_ScanIsInProgress(void);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_WPSResultsRead(WDRV_CONFIG *config, uint32_t *status)

  Summary:
    Reads the WPS process results back from the WILC1000 module and updates the
    configuration data.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    After the WPS process has completed, this function is used to read the WPS process
    results from the WILC1000 module and update the configuration data.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    config - pointer to where configuration data will be updated
    status - pointer to where WPS process status will be written

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_WPSResultsRead(WDRV_CONFIG *config, uint32_t *status);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_CmdFWUpdate(void)

  Summary:
    Directs the module to start firmware download and upgrade.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function directs the module to start the firmware download and upgrade.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_CmdFWUpdate(void);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_ModuleUpDown(uint32_t up)

  Summary:
    Enables or disables WILC1000 module.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function enables or disables WILC1000 module.

  Precondition:
    Wi-Fi initialization must be complete.

  Parameters:
    up - 1: enable; 0: disable.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_ModuleUpDown(uint32_t up);

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_HWInterruptHandler(void)

  Summary:
    Wi-Fi driver (WILC1000-specific) interrupt service routine.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is the Wi-Fi driver (WILC1000-specific) interrupt service routine.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_EXT_HWInterruptHandler(void);

//*******************************************************************************
/*
  Function:
        bool WDRV_CLI_Init(void)

  Summary:
    Initializes the console CLI interface.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function initializes the console CLI interface.

  Precondition:
    The TCP/IP stack should be initialized.

  Parameters:
    None.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
bool WDRV_CLI_Init(void);

//*******************************************************************************
/*
  Function:
        void WDRV_WILC1000_ISR()

  Summary:
    Wi-Fi driver (WILC1000-specific) interrupt service routine.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function is the Wi-Fi driver (WILC1000-specific) interrupt service routine.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    None.

  Remarks:
    None.
 */
void WDRV_WILC1000_ISR();

//*******************************************************************************
/*
  Function:
        void WDRV_EXT_RssiRead(void)

  Summary:
    Requests RSSI for the connected AP.
    <p><b>Implementation:</b> Dynamic</p>

  Description:
    This function requests RSSI for the connected AP.

  Precondition:
    Wi-Fi initialization must be complete.

  Returns:
    - 0              - Indicates success
    - Non-zero value - Indicates failure

  Remarks:
    None.
 */
uint32_t WDRV_EXT_RssiRead(void);

//*******************************************************************************
/*
Wi-Fi Commands Help

iwconfig
  Wi-Fi configuration
  Usage:
    iwconfig
    iwconfig ssid <ssid>
    iwconfig mode <mode>
    iwconfig security <security_mode> <key>/<pin>
    iwconfig power <enable/disable>
    iwconfig scan
    iwconfig scanget <scan_index>
  <ssid>:
    32 characters string - no blank or space allowed in this demo
  <mode>:
    managed/idle
  <security_mode>:
    open/wep40/wep104/wpa/wpa2/pin/pbc
    No blank or space allowed in <key> in current console commands
    Ex: iwconfig security open
        iwconfig security wep40 5AFB6C8E77
        iwconfig security wep104 90E96780C739409DA50034FCAA
        iwconfig security wpa microchip_psk
        iwconfig security wpa2 microchip_psk
        iwconfig security pin 12390212
        iwconfig security pbc

mac
  Get MAC address

readconf
  Read from storage

erase conf
  Erase storage

saveconf
  Save storage
 */

#endif /* _WDRV_WILC1000_API_H */

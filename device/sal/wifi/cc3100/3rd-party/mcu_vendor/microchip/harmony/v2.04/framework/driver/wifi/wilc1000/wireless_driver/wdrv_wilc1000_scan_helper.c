/*******************************************************************************
  WILC1000 Wi-Fi Driver Scan Helper Implementation

  File Name:
    wdrv_wilc1000_scan_helper.c

  Summary:
    WILC1000 Wi-Fi Driver Scan Helper Implementation

  Description:
    -Provides access to WILC1000 WiFi controller
    -Reference: WILC1000 Datasheet, IEEE 802.11 Standard
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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

#include "wdrv_wilc1000_main.h"
#include "wdrv_wilc1000_scan_helper.h"

WDRV_SCAN_RESULT g_scanResults[16];
WDRV_SCAN_STATUS g_scanStatus;

uint8_t _WDRV_ScanResultsBufferSize_Get(void)
{
    return sizeof(g_scanResults) / sizeof(g_scanResults[0]);
}

uint16_t WDRV_ScanStart(void)
{
    if (IS_SCAN_IN_PROGRESS(g_scanStatus.scanState))
    {
        return 0;
    }

    WDRV_EXT_CmdScanStart();

    SCAN_SET_IN_PROGRESS(g_scanStatus.scanState);

    return WDRV_SUCCESS;
}

void WDRV_ScanResultsDisplayManager(void)
{
    static bool displayHeader = true;
    uint8_t i;
    uint8_t ssid[32 + 1];

    if ((g_scanStatus.numberOfResults == 0) ||
        (!IS_SCAN_STATE_DISPLAY(g_scanStatus.scanState)) ||
        (IS_SCAN_IN_PROGRESS(g_scanStatus.scanState)) ||
        (!IS_SCAN_STATE_VALID(g_scanStatus.scanState)))
    {
       SCAN_CLEAR_DISPLAY(g_scanStatus.scanState);
       return;
    }

    if (displayHeader)
    {
        g_scanStatus.displayIdx = 0;
        SYS_CONSOLE_MESSAGE((const char *)"\nWi-Fi Scan Results:\r\n\n");
        if (g_scanStatus.numberOfResults == 0)
        {
            /* No AP found, re-initialize the scan context state. Scan is not allowed in Soft AP mode.
             * However, g_scanStatus.numberOfResults needs to be preserved for webpage using.
             */
            SYS_CONSOLE_MESSAGE((const char *)"No AP found\r\n ");
            SCAN_CLEAR_DISPLAY(g_scanStatus.scanState);
            if (gp_wdrv_cfg->networkType == WDRV_NETWORK_TYPE_SOFT_AP) {
                SCAN_CLEAR_VALID(g_scanStatus.scanState);
                return;
            } else {
                return;
            }
        }
        else
        {
            SYS_CONSOLE_MESSAGE((const char *)"     SSID                              RSSI  Channel\r\n");
            SYS_CONSOLE_MESSAGE((const char *)"     --------------------------------  ----  -------\r\n");
            displayHeader = false;
        }
    }

    // If the g_scanResults[] buffer is full, don't try to store more scan results to it.
    if (g_scanStatus.displayIdx > sizeof(g_scanResults) / sizeof(g_scanResults[0]) - 1)
    {
        SCAN_CLEAR_DISPLAY(g_scanStatus.scanState);
        g_scanStatus.displayIdx = 0;
        displayHeader = true;
        return;
    }
    WDRV_EXT_ScanResultGet(g_scanStatus.displayIdx, &(g_scanResults[g_scanStatus.displayIdx]));

    /* Display SSID */
    memset(ssid, ' ', sizeof(ssid));
    for (i = 0; i < g_scanResults[g_scanStatus.displayIdx].ssidLen; ++i)
    {
        if (!isprint(g_scanResults[g_scanStatus.displayIdx].ssid[i]))
            ssid[i] = '*';
        else
            ssid[i] = g_scanResults[g_scanStatus.displayIdx].ssid[i];
    }
    ssid[32] = 0;

    /* RSSI_MAX : 128, RSSI_MIN : 43 */
    SYS_CONSOLE_PRINT(" %2d) %s  %2u    %u\r\n",
            g_scanStatus.displayIdx + 1,
            ssid,
            g_scanResults[g_scanStatus.displayIdx].rssi,
            g_scanResults[g_scanStatus.displayIdx].channel);

    // If all the scan results have been displayed and stored, clear status and return.
    if (++g_scanStatus.displayIdx >= g_scanStatus.numberOfResults)
    {
        SCAN_CLEAR_DISPLAY(g_scanStatus.scanState);
        g_scanStatus.displayIdx = 0;
        displayHeader = true;
    }
}

void WDRV_ScanResultsSaveManager(uint8_t saveIdx)
{
    if ((g_scanStatus.numberOfResults == 0) ||
        (IS_SCAN_IN_PROGRESS(g_scanStatus.scanState)) ||
        (!IS_SCAN_STATE_VALID(g_scanStatus.scanState)))
    {
       return;
    }

    // If the g_scanResults[] buffer is full, don't try to store more scan results to it.
    if (saveIdx > sizeof(g_scanResults) / sizeof(g_scanResults[0]) - 1)
        return;
    WDRV_EXT_ScanResultGet(saveIdx, &(g_scanResults[saveIdx]));
}

//DOM-IGNORE-END

/*******************************************************************************
  WILC1000 Wi-Fi Driver Scan Helper Implementation

  File Name:
    wdrv_wilc1000_scan_helper.h

  Summary:
    WILC1000 Wi-Fi Driver Scan Helper Implementation

  Description:
    -Provides access to WILC1000 WiFi controller
    -Reference: WILC1000 Datasheet, IEEE 802.11 Standard
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc. All rights reserved.

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

#ifndef _WDRV_WILC1000_SCAN_HELPER_H
#define _WDRV_WILC1000_SCAN_HELPER_H

/* Scan status/control bits */
#define SCAN_STATE_IN_PROGRESS 0x0001 /* If scan is in progress */
#define SCAN_STATE_VALID_RESULTS 0x0002 /* If we have the valid scan results */
#define SCAN_STATE_DISPLAY_RESULTS 0x0004 /* This flag is only used to control WDRV_ScanResultsDisplayManager() */

#define IS_SCAN_IN_PROGRESS(x)    ((x) & SCAN_STATE_IN_PROGRESS)
#define IS_SCAN_STATE_VALID(x)    ((x) & SCAN_STATE_VALID_RESULTS)
#define IS_SCAN_STATE_DISPLAY(x)  ((x) & SCAN_STATE_DISPLAY_RESULTS)
#define SCAN_SET_IN_PROGRESS(x)   ((x) |= SCAN_STATE_IN_PROGRESS)
#define SCAN_SET_VALID(x)         ((x) |= SCAN_STATE_VALID_RESULTS)
#define SCAN_SET_DISPLAY(x)       ((x) |= SCAN_STATE_DISPLAY_RESULTS)
#define SCAN_CLEAR_IN_PROGRESS(x) ((x) &= ~SCAN_STATE_IN_PROGRESS)
#define SCAN_CLEAR_VALID(x)       ((x) &= ~SCAN_STATE_VALID_RESULTS)
#define SCAN_CLEAR_DISPLAY(x)     ((x) &= ~SCAN_STATE_DISPLAY_RESULTS)
#define DRV_WIFI_DEFAULT_ADHOC_HIDDEN_SSID false
#define DRV_WIFI_DEFAULT_ADHOC_BEACON_PERIOD (100) // ms
#define DRV_WIFI_DEFAULT_ADHOC_MODE DRV_WIFI_ADHOC_CONNECT_THEN_START

uint8_t _WDRV_ScanResultsBufferSize_Get(void);
uint16_t WDRV_ScanStart(void);
void WDRV_ScanResultsDisplayManager(void);
void WDRV_ScanResultsSaveManager(uint8_t saveIdx);

#endif /* _WDRV_WILC1000_SCAN_HELPER_H */

// DOM-IGNORE-END

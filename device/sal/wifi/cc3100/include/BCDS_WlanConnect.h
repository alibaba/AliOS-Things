/*----------------------------------------------------------------------------*/
/*
 * Copyright (C) Bosch Connected Devices and Solutions GmbH.
 * All Rights Reserved. Confidential.
 *
 * Distribution only to people who need to know this information in
 * order to do their job.(Need-to-know principle).
 * Distribution to persons outside the company, only if these persons
 * signed a non-disclosure agreement.
 * Electronic transmission, e.g. via electronic mail, must be made in
 * encrypted form.
 */
/*----------------------------------------------------------------------------*/

/**
 * @ingroup BCDS_WLAN
 *
 * @defgroup BCDS_WLAN_CONNECT Wlan Connection Interfaces
 * @{
 * @brief Module Describes the usage Of Wlan Interface
 *
 * @details  The interface header exports High-level Wlan Interface API's for integrating with the Wlan Interface which are used to Initialize Wlan connection
 * 			 Open Wlan Connection with the WEP & Wlan Connection Get status by deleting profiles and de-initialize the wlan connection.
 *
 * @file
 *
 */

/* header definition ******************************************************** */
#ifndef BCDS_WLANCONNECT_H_
#define BCDS_WLANCONNECT_H_

/* public interface declaration ********************************************* */
#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"

/* public type and macro definitions **************************************** */
#define WLANCONNECT_MAX_SCAN_INFO_BUF              UINT8_C(20) /**< The max buffer for storing the scan data */
#define WLANCONNECT_MAX_SSID_LENGTH                UINT8_C(32) /**< The max buffer for storing the SSID length */
#define WLANCONNECT_MAX_MAC_ADDR_LEN               UINT8_C(6) /**< The max buffer for storing the MAC address length */
#define WLANCONNECT_ENROLLEE_PIN                   "35072317" /**< This is the Enrollee PIN that must be entered in the router for WPS PIN Connection. The pin code is
																    stored in the SFlash. The default pin code is 35072317. */
#define WLANCONNECT_DUMMY_SSID                     "XDK110" /**< macro to represent the ssid of the device in WPS_PBC and WPS_PIN Security mode*/
#define WLANCONNECT_INITIALZED                     UINT8_C(1) /**< Macro to represent Initialized state of Wlan*/
#define WLANCONNECT_NOT_INITIALZED                 UINT8_C(0) /**< Macro to represent un-Initialized state of Wlan*/

/**
 * @brief Enumeration to represent the return codes of WLAN module.
 */
enum WlanConnect_Retcode_E
{
    RETCODE_NO_NW_AVAILABLE = RETCODE_FIRST_CUSTOM_CODE,
    RETCODE_ALREADY_DISCONNECTED,
    RETCODE_CONNECTION_ERROR,
    RETCODE_ERROR_WRONG_PASSWORD,
    RETCODE_ERROR_IP_NOT_ACQ,
    RETCODE_DISCONNECT_ERROR,
    RETCODE_SIMPLELINK_ERROR,
    RETCODE_TIMERCREATE_FAILURE,
    RETCODE_TIMERSTART_FAILURE,
    RETCODE_TIME_NULL,
	RETCODE_SIMPLELINK_STOP_FAILED,
	RETCODE_SIMPLELINK_STA_MODE_FAILED
};

/******************************************************************************/
/**  Type defining callback status.
 *   This type is used for different connection status for the WLAN Connect
 *   function callback.
 ******************************************************************************/
enum WlanConnect_Status_E
{
    WLAN_CONNECTED,
    WLAN_DISCONNECTED,
    WLAN_CONNECTION_ERROR,
    WLAN_CONNECTION_PWD_ERROR,
    WLAN_DISCONNECT_ERROR,
};
typedef enum WlanConnect_Status_E WlanConnect_Status_T;

/** Different status for the Current Network Configuration function callback */
enum WlanConnect_CurrentStatus_E
{
    CONNECTED_AND_IPV4_ACQUIRED = 0x00,
    CONNECTED_AND_IP_NOT_ACQUIRED = 0x01,
    DISCONNECTED_AND_IPV4_ACQUIRED = 0x02,
    DISCONNECTED_IP_NOT_ACQUIRED = 0x03,
};
typedef enum WlanConnect_CurrentStatus_E WlanConnect_CurrentNwStatus_T;

/******************************************************************************/
/**  Type defining a pointer to char.
 *   This type is used in order to pass the network SSID
 ******************************************************************************/
typedef signed char (*WlanConnect_SSID_T);

/******************************************************************************/
/**  Type defining a pointer to char.
 *   This type is used in order to pass the network Username
 ******************************************************************************/
typedef signed char (*WlanConnect_Username_T);

/******************************************************************************/
/**  Type defining a pointer to char
 *   This type is used in order to pass the network Pass Phrase
 ******************************************************************************/
typedef signed char (*WlanConnect_PassPhrase_T);

/******************************************************************************/
/**  Type defining scan interval.
 *   This type is used for WlanConnect_scanNetworks function
 ******************************************************************************/
typedef unsigned long (WlanConnect_ScanInterval_T);

/******************************************************************************/
/** Type defining scan information.
 *  Structure to store single network data after scanning
 ******************************************************************************/
struct WlanConnect_ScanInfo_S
{
    uint8_t Ssid[WLANCONNECT_MAX_SSID_LENGTH]; /**< Name of the access point*/
    uint8_t SsidLength; /**< Name of the access point*/
    uint8_t SecurityType; /**< Security type of the access point */
    uint8_t Bssid[WLANCONNECT_MAX_MAC_ADDR_LEN]; /**< MAC address of the access point*/
    int8_t Rssi; /**< RSSI value of the access point*/
};
typedef struct WlanConnect_ScanInfo_S WlanConnect_ScanInfo_T;

/******************************************************************************/
/** Type defining scan list
 *  Structure to store the scan list of all networks, which has the scan
 *  info data structure  as well as other scan information
 ******************************************************************************/
struct WlanConnect_ScanList_S
{
    int8_t NumOfScanEntries; /**< Number of access points after scan */
    uint32_t TimeStamp; /**< The last time stamp (seconds from last reset) when the scan was done*/
    WlanConnect_ScanInfo_T ScanData[WLANCONNECT_MAX_SCAN_INFO_BUF]; /**< The structure containing the scan data */
};
typedef struct WlanConnect_ScanList_S WlanConnect_ScanList_T;

/******************************************************************************/
/** Type defining the prototype which connection call back must conform to.
 *  This call back is called to notify the user when connection is established
 *  Returns the connection status as a parameter
 ******************************************************************************/
typedef void (*WlanConnect_Callback_T)(WlanConnect_Status_T connectStatus);

/******************************************************************************/
/** Type defining the prototype which disconnection call back must conform to.
 *  This call back is called to notify the user when connection is terminated
 *  following the call to WlanConnect_disconnect().
 *  Returns the connection status as a parameter
 ******************************************************************************/
typedef void (*WlanConnect_DisconnectCallback_T)(
        WlanConnect_Status_T discconnectStatus);

/* public function prototype declarations */

/******************************************************************************/
/**
 * @brief        WLAN driver initialization function.\n
 *               This function initializes WLAN driver context and
 *               Hardware interfaces; Also initializes the WLAN Stack.
 *               This API should be called first before calling any other API.
 *
 *  @note        By default device is configured to STA mode even if device is in different modes (Like AP, P2P). If user want to change the mode,
 *               they need to use the Simple link API directly
 *
 * @param  [in]  none
 *
 * @return      Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * @return       RETCODE_OK      - Successfully WLAN driver initialized.
 * @return       RETCODE_SIMPLELINK_ERROR - Not initialized WLAN driver.
 *
 * Example1 usage:
 *               Initialize the WLAN Stack and the WLAN Driver
 * @code
 *               // Initialize WLAN driver and WLAN stack
 *               WlanConnect_Init();
 * @endcode
 *
 * Example2 usage:
 *               Restart Device
 * @code
 *               // Local API return status
 *               Retcode_T retStatus[2];
 *
 *               retStatus[0] = WlanConnect_DeInit();
 *               retStatus[1] = WlanConnect_Init();
 *
 *               // Check if device restart has finished with error
 *               if ((RETCODE_OK != retStatus[0]) || (RETCODE_OK != retStatus[1]))
 *               {
 *                  printf("Restart device failed");
 *               }
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_Init(void);

/******************************************************************************/
/**
 * @brief        WLAN driver de-initialization function.\n
 *               Performs memory deallocation, closes WLAN driver context and
 *               shuts down the hardware interfaces.

 * @param  [in]  none
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * @return       RETCODE_OK      - Successfully WLAN driver de-initialized.
 * @return       RETCODE_SIMPLELINK_ERROR - De-initializing WLAN driver failed
 *
 * Example1 usage:
 *               Deinitialize the WLAN Stack
 * @code
 *               // Deinitialize WLAN stack
 *               WlanConnect_DeInit();
 * @endcode
 *
 * Example2 usage:
 *               Restart Device
 * @code
 *               // Local API return status
 *               Retcode_T retStatus[2];
 *
 *               retStatus[0] = WlanConnect_DeInit();
 *               retStatus[1] = WlanConnect_Init();
 *
 *               // Check if device restart has finished with error
 *               if ((RETCODE_OK != retStatus[0]) || (RETCODE_OK != retStatus[1]))
 *               {
 *                  printf("Restart device failed");
 *               }
 * @endcode
 ******************************************************************************/
Retcode_T WlanConnect_DeInit(void);

/******************************************************************************/
/**
 * @brief        WLI Connect to Open Network interface.\n
 *               This function connects to an open network.\n
 *               Function is configurable in two ways:
 *               - blocking : callback parameter is set to NULL(maximum blocking in error case will be 15seconds)
 *               - non-blocking : callback parameter is set to the address of
 *               the user function
 *
 * @param  [in]  connectSSID
 *               Pointer to char containing the network SSID to connect to.
 *
 * @param  [in]  connectCallback
 *               Pointer to user function callback.\n
 *               Returns the WLI connection status as a parameter.
 *
 * @return       RETCODE_OK - Successfully connected to the AP.In case of API success return value after calling a blocking
 *                              function, the WLI_SUCCESS implicitly confirms to the user that the WLI connection is done
 * @return       RETCODE_SIMPLELINK_ERROR - Failure returned by simplelink API.
 * @return       WLAN_CONNECTION_ERROR - Failure because of incorrect SSID.
 * @return       RETCODE_ERROR_IP_NOT_ACQ     - Connected but ip not acquired.
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 *
 * Example1 usage:
 *               Connect to an open WLAN network with callback (non-blocking)
 * @code
 *               // Callback function
 *               void myConnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *               {
 *                   if (WLI_CONNECTED == returnStatus)
 *                   {
 *                       // Network connection successfully
 *                       printf("Callback Function : Connect OK!\n\r");
 *                       // User may add logic once the connection is done
 *                       // E.g launch another OS Timer
 *                   }
 *                   else
 *                   {
 *                         // user may retry to connect
 *                   }
 *               }
 *
 *               // local variables
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *               WlanConnect_Callback_T connectCallback;
 *
 *               // Open wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"Name-Of-Open-Network";
 *
 *               // Set callback functions
 *               connectCallback = myConnectCallbackFunc;
 *
 *               // Connect to network --> non blocking function with callback
 *               retStatus = WlanConnect_Open(connectSSID, connectCallback);
 *
 * @endcode
 *
 * Example2 usage:
 *               Connect to an open WLAN network in blocking mode
 * @code
 *               // local variables
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *
 *               // Open wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"Name-Of-Open-Network";
 *
 *               // Connect to network --> blocking function without callback
 *               // Task will be blocked here until connection is done and IP acquired
 *               retStatus = WlanConnect_Open(connectSSID, NULL);
 *
 * @endcode
 ******************************************************************************/
Retcode_T WlanConnect_Open(WlanConnect_SSID_T connectSSID,
        WlanConnect_Callback_T connectCallback);

/******************************************************************************/
/**
 * @brief        WLI Connect to WEP Open Network interface.\n
 *               This function connects to an WEP Open network.\n
 *               Function is configurable in two ways:
 *               - blocking : callback parameter is set to NULL(maximum blocking in error case will be 15seconds)
 *               - non-blocking : callback parameter is set to the address
 *               of the user function
 *
 * @param  [in]  connectionSSID
 *               Pointer to char containing the network SSID to connect to.
 * @param  [in]  connectionPass
 *               Pointer to char containing the network Pass Phrase.
 * @param  [in]  passPhraseLength
 *               Length of the Pass Phrase
 * @param  [in]  connectionCallback
 *               Pointer to char containing the connection callback.
 * @return       RETCODE_OK -  API return code for success. In case of API success return value after calling a blocking
 *                              function, the WLI_SUCCESS implicitly confirms to the user that the WLI connection is done
 * @return       RETCODE_SIMPLELINK_ERROR - Failure returned by simplelink API.
 * @return       RETCODE_ERROR_SSID_NOT_FOUND - Failure because of incorrect SSID.
 * @return       RETCODE_ERROR_WRONG_PASSWORD -  Failure because of incorrect Pass phrase.
 * @return       RETCODE_ERROR_IP_NOT_ACQ     - Connected but ip not acquired
 * @return       RETCODE_INVALID_PARAM        - passPhraseLength is zero
 *
 * @return       Retcode_T         The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 *
 * Example1 usage:
 *               Connect to an WEP Open network with callback (non-blocking)
 * @code
 *               // Callback function
 *               void myConnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *               {
 *                   if (WLI_CONNECTED == returnStatus)
 *                   {
 *                       // Network connection successfully
 *                       printf("Callback Function : Connect OK!\n\r");
 *                       // User may add logic once the connection is done
 *                       // E.g launch another OS Timer
 *                   }
 *                    else
 *                   {
 *                         // user may retry to connect
 *                   }
 *               }
 *
 *               // local variables
 *               //For example if AP is configured to WEP with 40bit Hex password of: "FFFFFFFFFF"
 *               char passPhrase[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *               WlanConnect_Callback_T connectCallback;
 *               WlanConnect_PassPhrase_T connectPassPhrase;
 *               uint8_t passPhraseLength;[Don't use strlen() in order not to bump on zero/NULL in the passPhrase]
 *
 *               // WEP encrypted wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"WEP-OPEN-NETWORK-NAME";
 *               connectPassPhrase = (WlanConnect_PassPhrase_T)passPhrase;
 *               passPhraseLength = UINT8_C(5);
 *               // Set callback functions
 *               connectCallback = myConnectCallbackFunc;
 *
 *               // Connect to network --> non blocking function with callback
 *               retStatus = WlanConnect_WEP_Open(connectSSID, connectPassPhrase,passPhraseLength, connectCallback);
 * @endcode
 *
 * Example2 usage:
 *               Connect to an WEP Open network in blocking mode
 * @code
 *               // local variables
 *               //For example if AP is configured to WEP with 40bit Hex password of: "FFFFFFFFFF"
 *               char passPhrase[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *               WlanConnect_PassPhrase_T connectPassPhrase;
 *               uint8_t passPhraseLength;[Don't use strlen() in order not to bump on zero/NULL in the passPhrase]
 *
 *               // WEP encrypted wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"WEP-OPEN-NETWORK-NAME";
 *               connectPassPhrase = (WlanConnect_PassPhrase_T)passPhrase;
 *               passPhraseLength = UINT8_C(5);
 *
 *               // Connect to network --> blocking function without callback
 *               retStatus = WlanConnect_WEP_Open(connectSSID, connectPassPhrase,passPhraseLength, NULL);
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_WEP_Open(WlanConnect_SSID_T connectSSID,
        WlanConnect_PassPhrase_T connectPass,uint8_t passPhraseLength,
        WlanConnect_Callback_T connectCallback);

/******************************************************************************/
/**
 * @brief        WLI Connect to WPA Network interface.\n
 *               This function connects to an WPA2-PSK or WPA-PSK network.\n
 *               Function is configurable in two ways:
 *               - blocking : callback parameter is set to NULL(maximum blocking in error case will be 15seconds)
 *               - non-blocking : callback parameter is set to the address of
 *               the user function
 *
 * @param  [in]  connectionSSID
 *               Pointer to char containing the network SSID to connect to.
 * @param  [in]  connectionPass
 *               Pointer to char containing the network Pass Phrase.
 * @param  [in]  connectionCallback
 *               Pointer to char containing the connection callback.
 *
 * @return       RETCODE_OK      - API return code for success. In case of API success return value after calling a blocking
 *                              function, the WLI_SUCCESS implicitly confirms to the user that the WLI connection is done
 * @return       RETCODE_SIMPLELINK_ERROR - Failure returned by simplelink API.
 * @return       RETCODE_ERROR_SSID_NOT_FOUND - Failure because of incorrect SSID.
 * @return       RETCODE_ERROR_WRONG_PASSWORD -  Failure because of incorrect Pass phrase.
 * @return       RETCODE_ERROR_IP_NOT_ACQ     - Connected but ip not acquired
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 *
 * Example1 usage:
 *               Connect to WPA/WPA2 network with callback (non-blocking)
 * @code
 *               // Callback function
 *               void myConnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *               {
 *                   if (WLI_CONNECTED == returnStatus)
 *                   {
 *                       // Network connection successfully
 *                       printf("Callback Function : Connect OK!\n\r");
 *                       // User may add logic once the connection is done
 *                       // E.g launch another OS Timer
 *                   }
 *                   else
 *                   {
 *                         // user may retry to connect
 *                   }
 *               }
 *
 *               // local variables
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *               WlanConnect_Callback_T connectCallback;
 *               WlanConnect_PassPhrase_T connectPassPhrase;
 *
 *               // WPA/WPA2 encrypted wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"WPA-NETWORK-NAME";
 *               connectPassPhrase = (WlanConnect_PassPhrase_T)"WPA-PASSWORD";
 *
 *               // Set callback functions
 *               connectCallback = myConnectCallbackFunc;
 *
 *               // Connect to network --> non blocking function with callback
 *               retStatus = WlanConnect_WPA(connectSSID, connectPassPhrase, connectCallback);
 * @endcode
 *
 * Example2 usage:
 *               Connect to WPA/WPA2 network in blocking mode
 * @code
 *               // local variables
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *               WlanConnect_PassPhrase_T connectPassPhrase;
 *
 *               // WPA/WPA2 encrypted wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"WPA-NETWORK-NAME";
 *               connectPassPhrase = (WlanConnect_PassPhrase_T)"WPA-PASSWORD";
 *
 *               // Connect to network --> blocking function without callback
 *               retStatus = WlanConnect_WPA(connectSSID, connectPassPhrase, NULL);
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_WPA(WlanConnect_SSID_T connectSSID,
        WlanConnect_PassPhrase_T connectPass,
        WlanConnect_Callback_T connectCallback);

/******************************************************************************/
/**
 * @brief        WLI Connect using WPS PBC interface.\n
 *               This function connects to a network by using the  Wi-Fi
 *               Protected Setup. After calling the function the user must
 *               push the connect button of the Router for around 5 seconds.\n
 *               Function is configurable in two ways:
 *               - blocking : callback parameter is set to NULL(maximum blocking in error case will be 15seconds)
 *               - non-blocking : callback parameter is set to the address of
 *               the user function

 * @param  [in]  connectionCallback
 *               Pointer to char containing the connection callback.
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * @return       RETCODE_OK -  API return code for success. In case of API success return value after calling a blocking
 *                              function, the WLI_SUCCESS implicitly confirms to the user that the WLI connection is done.
 * @return       RETCODE_SIMPLELINK_ERROR - Failure returned by simplelink API.
 * @return       RETCODE_CONNECTION_ERROR -  connection Failure .
 * @return       RETCODE_ERROR_IP_NOT_ACQ     - Connected but ip not acquired
 *
 * Example1 usage:
 *               Connect to a wireless network using WPS (WiFi Protected Setup)
 *               PBC (Push Button Connect) with callback (non-blocking)
 * @code
 *               // Callback function
 *               void myConnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *               {
 *                   if (WLI_CONNECTED == returnStatus)
 *                   {
 *                       // Network connection successfully
 *                       printf("Callback Function : Connect OK!\n\r");
 *                       // User may add logic once the connection is done
 *                       // E.g launch another OS Timer
 *                   }
 *                    else
 *                   {
 *                         // user may retry to connect
 *                   }
 *               }
 *
 *               // local variables
 *               Retcode_T retStatus[3];
 *               WlanConnect_Callback_T connectCallback;
 *
 *               // Set callback functions
 *               connectCallback = myConnectCallbackFunc;
 *
 *              // Because the board connects to network very fast, the user
 *               // must disconnect and delete all stored profiles before
 *               // retrying to connect with WPS PBC
 *               retStatus[0] = WlanConnect_Disconnect(NULL);
 *
 *               // Delete all stored profiles in order to connect WPS by pushing
 *               // the button all the time. Remove this line if you want to
 *               // keep profile after running once the function.
 *               retStatus[1] = WlanConnect_DeleteAllProfiles();
 *
 *               // Connect to network --> non blocking function with callback
 *               retStatus[2] = WlanConnect_WPS_PBC(connectCallback);
 * @endcode
 *
 * Example2 usage:
 *               Connect to a wireless network using WPS (WiFi Protected Setup)
 *               PBC (Push Button Connect) in blocking mode (without callback)
 * @code
 *               // local variables
 *               Retcode_T retStatus[3];
 *
 *               // Because the board connects to network very fast, the user
 *               // must disconnect and delete all stored profiles before
 *               // retrying to connect with WPS PBC
 *               retStatus[0] = WlanConnect_Disconnect(NULL);
 *
 *               // Delete all stored profiles in order to connect WPS by pushing
 *               // the button all the time. Remove this line if you want to
 *               // keep profile after running once the function.
 *               retStatus[1] = WlanConnect_DeleteAllProfiles();
 *
 *               // Connect to network --> blocking function without callback
 *               retStatus[2] = WlanConnect_WPS_PBC(NULL);
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_WPS_PBC(WlanConnect_Callback_T connectCallback);

/******************************************************************************/
/**
 * @brief        WLI Connect using WPS PIN interface.\n
 *               This function connects to a network by using the  Wi-Fi
 *               Protected Setup. The function is configured with the following
 *               enrollee PIN code: 35072317. This PIN must be entered in the
 *               router interface after calling the function.\n
 *               Function is configurable in two ways:
 *               - blocking : callback parameter is set to NULL(maximum blocking in error case will be 15seconds)
 *               - non-blocking : callback parameter is set to the address of
 *               the user function
 *
 * @param  [in]  connectionCallback - Pointer to char containing the connection callback.
 *
 * @return       RETCODE_OK - API return code for success. In case of API success return value after calling a blocking
 *                              function, the WLI_SUCCESS implicitly confirms to the user that the WLI connection is done
 * @return       RETCODE_SIMPLELINK_ERROR - Failure returned by simplelink API.
 * @return       RETCODE_CONNECTION_ERROR - Connection Failure because of incorrect Pin.
 * @return       RETCODE_ERROR_IP_NOT_ACQ     - Connected but IP not acquired
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * Example1 usage:
 *               Connect to a wireless network using WPS (WiFi Protected Setup)
 *               PIN (Personal Identify. Number) with callback (non-blocking)
 * @code
 *               // Callback function
 *               void myConnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *               {
 *                   if (WLI_CONNECTED == returnStatus)
 *                   {
 *                       // Network connection successfully
 *                       printf("Callback Function : Connect OK!\n\r");
 *                       // User may add logic once the connection is done
 *                       // E.g launch another OS Timer
 *                   }
 *                    else
 *                   {
 *                         // user may retry to connect
 *                   }
 *               }
 *
 *               // local variables
 *               Retcode_T retStatus[3];
 *               WlanConnect_Callback_T connectCallback;
 *
 *               // Set callback functions
 *               connectCallback = myConnectCallbackFunc;
 *
 *              // Because the board connects to network very fast, the user
 *               // must disconnect and delete all stored profiles before
 *               // retrying to connect with WPS PIN
 *               retStatus[0] = WlanConnect_Disconnect(NULL);
 *
 *               // Delete all stored profiles in order to connect WPS using
 *               // enrollee PIN all the time. Remove this line if you want to
 *               // keep profile after running once the function.
 *               retStatus[1] = WlanConnect_DeleteAllProfiles();
 *
 *               // Connect to network --> non blocking function with callback
 *               retStatus[2] = WlanConnect_WPS_PIN(connectCallback);
 * @endcode
 *
 * Example2 usage:
 *               Connect to a wireless network using WPS (WiFi Protected Setup)
 *               PIN (Personal Identification Number) in blocking mode
 * @code
 *               // local variables
 *               Retcode_T retStatus[3];
 *
 *               // Because the board connects to network very fast, the user
 *               // must disconnect and delete all stored profiles before
 *               // retrying to connect with WPS PIN
 *               retStatus[0] = WlanConnect_Disconnect(NULL);
 *
 *               // Delete all stored profiles in order to connect WPS using
 *               // enrollee PIN all the time. Remove this line if you want to
 *               // keep profile after running once the function.
 *               retStatus[1] = WlanConnect_DeleteAllProfiles();
 *
 *               // Connect to network --> blocking function without callback
 *               retStatus[2] = WlanConnect_WPS_PIN(NULL);
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_WPS_PIN(WlanConnect_Callback_T connectCallback);

/******************************************************************************/
/**
 * @brief        WLI Connect to Enterprise WPA Network interface.\n
 *               This function connects to an WPA2-PSK or WPA-PSK network.\n
 *               Function is configurable in two ways:
 *               - blocking : callback parameter is set to NULL(maximum blocking in error case will be 15seconds)
 *               - non-blocking : callback parameter is set to the address of
 *               the user function
 *
 * @param  [in]  connectionSSID
 *               Pointer to char containing the network SSID to connect to.
 * @param  [in]  connectUsername
 *               Pointer to char containing the network Username to connect to.
 * @param  [in]  connectionPass
 *               Pointer to char containing the network Pass Phrase.
 * @param  [in]  blockingCall
 *               bool value to define the calling type as blocking or non-blocking
 *
 * @return       RETCODE_OK      - API return code for success. In case of API success return value after calling a blocking
 *                              function, the WLI_SUCCESS implicitly confirms to the user that the WLI connection is done
 * @return       RETCODE_SIMPLELINK_ERROR - Failure returned by simplelink API.
 * @return       RETCODE_ERROR_SSID_NOT_FOUND - Failure because of incorrect SSID.
 * @return       RETCODE_ERROR_WRONG_PASSWORD -  Failure because of incorrect Pass phrase.
 * @return       RETCODE_ERROR_IP_NOT_ACQ     - Connected but ip not acquired
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 *
 * @note        It uses index 0 for certificate. The Host must have been programmed with the certificate in this Index prior.
 *              It uses PEAP0-MSCHAPv2 as EAP method.
 *              It uses the connectUsername for default and anonymous user names
 *
 * Example1 usage:
 *               Connect to Enterprise WPA/WPA2 network with callback (non-blocking)
 * @code
 *               // Callback function
 *               void myConnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *               {
 *                   if (WLI_CONNECTED == returnStatus)
 *                   {
 *                       // Network connection successfully
 *                       printf("Callback Function : Connect OK!\n\r");
 *                       // User may add logic once the connection is done
 *                       // E.g launch another OS Timer
 *                   }
 *                   else
 *                   {
 *                         // user may retry to connect
 *                   }
 *               }
 *
 *               // local variables
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *               WlanConnect_PassPhrase_T connectPassPhrase;
 *
 *               // WPA/WPA2 encrypted wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"WPA-NETWORK-NAME";
 *               connectUsername = (WlanConnect_Username_T)"WPA-USERNAME-NAME";
 *               connectPassPhrase = (WlanConnect_PassPhrase_T)"WPA-PASSWORD";
 *
 *               // Set callback functions
 *               connectCallback = myConnectCallbackFunc;
 *
 *               // Connect to network --> non blocking function with callback
 *               retStatus = WlanConnect_EnterpriseWPA(connectSSID, connectUsername, connectPassPhrase, myConnectCallbackFunc);
 * @endcode
 *
 * Example2 usage:
 *               Connect to Enterprise WPA/WPA2 network in blocking mode
 * @code
 *               // local variables
 *               WlanConnect_SSID_T connectSSID;
 *               Retcode_T retStatus;
 *               WlanConnect_PassPhrase_T connectPassPhrase;
 *
 *               // WPA/WPA2 encrypted wireless network settings
 *               connectSSID = (WlanConnect_SSID_T)"WPA-NETWORK-NAME";
 *               connectUsername = (WlanConnect_Username_T)"WPA-USERNAME-NAME";
 *               connectPassPhrase = (WlanConnect_PassPhrase_T)"WPA-PASSWORD";
 *
 *               // Connect to network --> blocking function without callback
 *               retStatus = WlanConnect_EnterpriseWPA(connectSSID, connectUsername, connectPassPhrase, NULL);
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_EnterpriseWPA(WlanConnect_SSID_T connectSSID, WlanConnect_Username_T connectUsername,
        WlanConnect_PassPhrase_T connectPassPhrase, WlanConnect_Callback_T connectCallback);

/******************************************************************************/
/**
 * @brief        WLI Delete All Profiles interface.\n
 *               This function erases from memory all the stored profiles from
 *               WIFI Protected Setup functions.
 *
 * @param  [in]  none
 *
 * @return       RETCODE_OK - successfully erased all profiles.
 * @return       RETCODE_SIMPLELINK_ERROR - error during erase
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * Example usage:
 *               Delete all stored WLI profiles
 * @code
 *               Retcode_T retStatus;
 *
 *               retStatus = WlanConnect_DeleteAllProfiles();
 *
 *               if (RETCODE_OK != retStatus)
 *               {
 *                  printf("Profiles were not deleted correctly");
 *               }
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_DeleteAllProfiles(void);

/******************************************************************************/
/**
 * @brief        WLI disconnect interface.\n
 *               This function disconnects from the WLAN network.\n
 *               Function is configurable in two ways:
 *               - blocking : callback parameter is set to WLI_NULL(maximum blocking in error case will be 15seconds)
 *               - non-blocking : callback parameter is set to the address
 *               of the user function
 *
 * @param  [in]  disconnectCallback
 *               Pointer to user function callback\n
 *               Returns the WLI connection status as a parameter.
 *
 * @return       RETCODE_OK       successfully disconnected from Access point. In case of API success return value after calling a
 *                         blocking function, the WLI_SUCCESS implicitly confirms to the user that WLAN disconnection is done.
 * @return       RETCODE_DISCONNECT_ERROR  Error in disconnecting
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * Example1 usage:
 *               Disconnect with callback (non-blocking)
 * @code
 *               void myDisconnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *               {
 *                   if (WLAN_DISCONNECTED == returnStatus)
 *                   {
 *                       // Network disconnection successfully from WlanConnect_Disconnect function
 *                       printf("Callback Function : Disconnect OK!\n\r");
 *                   }
 *                   else
 *                   {
 *                   //user can retry
 *                   }
 *               }
 *
 *               // local variables
 *               Retcode_T retStatus;
 *               WlanConnect_DisconnectCallback_T disconnectCallback;
 *
 *               // Set disconnect callback function
 *               disconnectCallback = myDisconnectCallbackFunc;
 *
 *               // disconnect from network with callback --> non-blocking
 *               retStatus = WlanConnect_Disconnect(disconnectCallback);
 * @endcode
 *
 * Example2 usage:
 *               Disconnect without callback
 * @code
 *               // local variables
 *               Retcode_T retStatus;
 *
 *               // disconnect from network in blocking mode
 *               retStatus = WlanConnect_Disconnect(NULL);
 *
 *               if (RETCODE_OK == retStatus)
 *               {
 *                   // Disconnect has completed successfully
 *                   printf("Disconnected successfully!\n\r");
 *               }
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_Disconnect(WlanConnect_DisconnectCallback_T disconnectCallback);

/******************************************************************************/
/**
 * @brief        Function used to scan the available networks.\n
 *               Get the available scan information.\n
 *               List of scan results will be available via the call
 *               back function.
 *
 * @param [in]   scanInterval
 *               Time interval between network scan.
 *
 * @param [out]  scanList
 *               Structure pointer hold the WLI scan information data.
 *
 * @return       RETCODE_OK                        - scan done successful.
 * @return       RETCODE_SIMPLELINK_ERROR          - scan was not successful.
 * @return       RETCODE_NO_NW_AVAILABLE           - No network available to scan.
 *
 * @return       Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * Example usage:
 *               Scan networks and print SSIDs
 * @code
 *               // local variables
 *               WLI_scanReturnCode_t retScanStatus;
 *               WlanConnect_ScanInterval_T scanInterval;
 *               WlanConnect_ScanList_T scanList;
 *
 *               // Set scan interval
 *               scanInterval = 5;
 *
 *               // Fill out the scan list by calling the scan network function
 *               retScanStatus = WlanConnect_ScanNetworks(scanInterval, &scanList);
 *
 *               // Set the number of entries
 *               int nbEntries = scanList.numOfScanEntries;
 *
 *               // Print all the non-empty SSIDs
 *               for (int i = 0; i < nbEntries; i++)
 *               {
 *                   if (0 != scanList.scanData[i].ssidLength)
 *                   {
 *                       printf(" - found SSID number %d is : %s\n\r", i, scanList.scanData[i].ssid);
 *
 *                       // Delay each printf with 0,5 seconds
 *                       vTaskDelay(DELAY_500_MSEC);
 *                   }
 *               }
 * @endcode
 *
 ******************************************************************************/
Retcode_T WlanConnect_ScanNetworks(WlanConnect_ScanInterval_T scanInterval,
        WlanConnect_ScanList_T* scanList);
/******************************************************************************/
/**
 * @brief         Function to get the current network status
 *                This function sets a disconnect callback which is used to
 *                notify the user that there has been a disconnection that was
 *                not triggered by the user.ed on connection status.
 *
 * @Warning       This API must be called only after initialization
 *
 * @param [ in ]  allDisconnectCallback
 *                Pointer to user function callback.
 *                Returns the WLI connection status as a parameter.
 *
 * @return        CONNECTED_AND_IPV4_ACQUIRED    - currently WLAN connected and IP acquired.
 * @return        CONNECTED_AND_IP_NOT_ACQUIRED  - currently WLAN connected and IP not acquired.
 * @return        DISCONNECTED_AND_IPV4_ACQUIRED - currently WLAN disconnected and IP acquired
 * @return        DISCONNECTED_IP_NOT_ACQUIRED   - currently WLAN disconnected and IP not acquired.
 *
 * Example usage:
 *                Get current status of a network and set a callback for any
 *                disconnect events
 * @code
 *                void myAllDisconnectCallbackFunc(WlanConnect_ConnectionStatus_T returnStatus)
 *                {
 *                    if (WLI_DISCONNECTED == returnStatus)
 *                    {
 *                        // Network disconnection successfully from WlanConnect_Disconnect function
 *                        printf("Callback Function : A disconnect event has happened!\n\r");
 *                    }
 *                }
 *
 *                // local variables
 *                Retcode_T retStatus;
 *                WlanConnect_DisconnectCallback_T myAllDisconnectCallback;
 *
 *                // Set callback function
 *                myAllDisconnectCallback = myAllDisconnectCallbackFunc;
 *
 *                if (DISCONNECTED_IP_NOT_ACQUIRED
 *                     != WlanConnect_GetCurrentNwStatus()
 *                   && (DISCONNECTED_AND_IPV4_ACQUIRED
 *                     != WlanConnect_GetCurrentNwStatus()))
 *                {
 *                    retStatus = WlanConnect_Disconnect(myAllDisconnectCallback);
 *                    printf("Finished disconnection successfully \n\r");
 *                }
 * @endcode
 ******************************************************************************/
WlanConnect_CurrentNwStatus_T WlanConnect_GetCurrentNwStatus(void);

/******************************************************************************/
/**
 * @brief         Function to get the connection status.\n
 *                This function return the connection status.
 *
 * @param [ in ]  none
 *
 * @return        WLI_CONNECTED                 - WLI connected obtained after calling connect function.
 * @return        WLI_DISCONNECTED              - WLI disconnected obtained after calling disconnect function.
 *
 * Example usage:
 *                Block until IP status is acquired and Wlan is connected
 * @code
 *                while ((WLI_DISCONNECTED == WlanConnect_GetStatus())
 *                 || (NETWORKCONFIG_IP_NOT_ACQUIRED == NetworkConfig_GetIpStatus()))
 *                {
 *                    // Stay here until connected and ip acquired.
 *                    // Timeout logic can be added here.
 *                }
 * @endcode
 * @warning       This API must be called only after initialization
 *
 ******************************************************************************/
WlanConnect_Status_T WlanConnect_GetStatus(void);

/**
 * @brief         Function to get the connection status.\n
 *                This function return the connection status.
 *
 * @param [ in ]  none
 *
 * @return        WLANCONNECT_INITIALZED       - Wlan module is initialized.
 * @return        WLANCONNECT_NOT_INITIALZED   - Wlan module is not initialized.
 ******************************************************************************/
uint8_t WlanConnect_GetInitStatus(void);

/* public global variable declarations */

/* inline function definitions */

#endif /* BCDS_WLANCONNECT_H_ */

/** @} */

/******************************************************************************/

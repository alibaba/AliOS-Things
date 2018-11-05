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
 * @defgroup BCDS_NETWORK_CONFIG Network Configuration Interface
 * @{
 *
 * @brief  Interface header for the Network Configuration Interface module.
 *
 * @details  The interface header exports High-level Network Configuration API's for integrating with the Network Interface which are used to Get & Set IP Settings
 * 		Set DHCP IP address and to get the IPV4 IpAddress and ipv4 byte value.
 *
 * @file
 *
 */

/* header definition ******************************************************* */
#ifndef BCDS_NETWORKCONFIG_H_
#define BCDS_NETWORKCONFIG_H_

/* public interface declaration ********************************************* */
#include "BCDS_Basics.h"
#include "BCDS_Retcode.h"

#ifndef NETWORKCONFIG_DHCP_DISABLED
#define NETWORKCONFIG_DHCP_DISABLED                       (0) /**< Macro for Dynamically Host Configuration Protocol disabled*/
#endif

#ifndef NETWORKCONFIG_DHCP_ENABLED
#define NETWORKCONFIG_DHCP_ENABLED                        (1) /**< Macro for Dynamically Host Configuration Protocol enabled*/
#endif
#define NETWORKCONFIG_DHCP_FLG_NOT_ACQ              (0) /**< Macro for Dynamically Host Configuration Protocol not acquired*/
#define NETWORKCONFIG_DHCP_FLG_ACQ                 (1) /**< Macro for Dynamically Host Configuration Protocol acquired*/

/** Different IP status for the Network Configuration function callback */
enum NetworkConfig_IpStatus_E
{
    NETWORKCONFIG_IPV4_ACQUIRED = 0x00, /**< IPv4 Address Acquired */
    NETWORKCONFIG_IPV6_ACQUIRED = 0x01, /**< IPv6 Address Acquired */
    NETWORKCONFIG_IP_NOT_ACQUIRED = 0x02, /**< IP Address Not Acquired */
};
typedef enum NetworkConfig_IpStatus_E NetworkConfig_IpStatus_T;

/**  Structure to hold Network Configuration IP settings */
struct NetworkConfig_IpSettings_S
{
    uint8_t isDHCP; /**< IP settings static or DHCP */
    uint32_t ipV4; /**< IP Address */
    uint32_t ipV4Mask; /**< IP Address Mask */
    uint32_t ipV4Gateway; /**< IP Address Gateway */
    uint32_t ipV4DnsServer; /**< IP Address DNS Server */
};
typedef struct NetworkConfig_IpSettings_S NetworkConfig_IpSettings_T;

/** Type defining the prototype which Set IP function call back must conform to.
 *  This call back is called to notify the user when the IP is acquired
 *  following the call of NetworkConfig_SetIpDhcp().
 *  Returns the NCI IP status as a parameter*/
typedef void (*NetworkConfig_IpCallback_T)(NetworkConfig_IpStatus_T ipStatus);

/******************************************************************************/
/**
 * @brief       Function to handle WLAN IP settings.
 *              This function gets the IP settings after a Successful connection
 *
 * @param [out] myIpSettings
 *              Network configuration settings. Pointer contains IP address,
 *              Gateway, DNS Server and DHCP information
 *
 * @return      #RETCODE_NULL_POINTER - if the input parameter passes NULL.
 * @return      #RETCODE_OK           - Settings retrieved correctly.
 * @return      #RETCODE_UNINITIALIZED - if WlanConnect_GetInitStatus failed by returning WLANCONNECT_NOT_INITIALZED.
 * @return      #RETCODE_FAILURE - if sl_NetCfgGet failed by returning NETWORKCONFIG_FAILURE .
 *
 * @return       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 *
 * Example usage:
 *              Getting the IP settings
 * @code
 *              // Local variable that stores the IP settings
 *              NetworkConfig_IpSettings_T myIpGet;
 *              // Local variable for storing the return status
 *              Retcode_T retStatus;
 *
 *              // retrieve the IP settings
 *              retStatus = NetworkConfig_GetIpSettings(&myIpGet);
 *
 *              // print a message if the function returned successfully
 *              if (RETCODE_OK == retStatus)
 *              {
 *                  printf("The IP was retrieved successfully");
 *              }
 * @endcode
 *
 ******************************************************************************/
Retcode_T NetworkConfig_GetIpSettings(NetworkConfig_IpSettings_T* myIpSettings);

/******************************************************************************/
/**
 * @brief         Function to handle WLAN IP settings.
 *                This function sets the static IP.
 *
 * @param [out]   myIpSettings
 *                Network configuration settings structure. Contains IP address,
 *                Gateway, DNS Server and DHCP information
 *
 * @return      #RETCODE_OK    -    Settings retrieved correctly.
 * @return      #RETCODE_UNINITIALIZED - if WlanConnect_GetInitStatus failed by returning WLANCONNECT_NOT_INITIALZED.
 * @return      #RETCODE_FAILURE - if sl_NetCfgGet failed by returning NETWORKCONFIG_FAILURE.
 * @return      #RETCODE_FAILURE - if sl_Start failed by returning other than ROLE_STA.
 *
 * @return        Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 * Example usage:
 *                Setting static IP
 * @code
 *                // Local variables
 *                NetworkConfig_IpSettings_T myIpSet;
 *                Retcode_T retStatus;
 *
 *                // IP settings that will be used.
 *                myIpSet.isDHCP        =  (uint8_t)  DHCP_DISABLED;
 *                myIpSet.ipV4          =  NCI_ipv4Value(192,168,2,120);
 *                myIpSet.ipV4DnsServer =  NCI_ipv4Value(192,168,2,1);
 *                myIpSet.ipV4Gateway   =  NCI_ipv4Value(192,168,2,1);
 *                myIpSet.ipV4Mask      =  NCI_ipv4Value(255,255,255,0);
 *
 *                // Set the static IP
 *                retStatus = NetworkConfig_SetIpStatic(myIpSet);
 *
 *                // print a message if the function returned successfully
 *                if (RETCODE_OK == retStatus)
 *                {
 *                    printf("The static IP was set successfully");
 *                }
 * @endcode
 *
 ******************************************************************************/
Retcode_T NetworkConfig_SetIpStatic(NetworkConfig_IpSettings_T myIpSettings);

/******************************************************************************/
/**
 * @brief         Function to handle WLAN IP settings.
 *                This function sets the IP by using  DHCP.
 *
 * @param [in]    myIpCallback
 *                Callback function for notifying user of IP acquisition
 *                using DHCP
 *
 * @return      #RETCODE_UNINITIALIZED - if WlanConnect_GetInitStatus failed by returning WLANCONNECT_NOT_INITIALZED.
 * @return      #RETCODE_FAILURE - if sl_Start failed by returning other than ROLE_STA.
 * @return      #RETCODE_OK   -  sets the IP by using DHCP successfully.
 *
 * @return     Retcode_T       The return value consist of (First 1 MSByte represents Package ID, Next  Byte represents Severity and Last 2 LSBytes represents error code)
 *                              Below shows the error code(last 2 Bytes) meaning
 *
 * Example usage:
 *                Setting DHCP IP with Callback
 * @code
 *                // Callback function
 *                void myDhcpIpCallbackFunc(NetworkConfig_IpStatus_T returnStatus)
 *                {
 *                    // user can do something inside this callback
 *                }
 *
 *                // local variables
 *                NCI_ipCallback_t myIpCallback;
 *                Retcode_T retStatus;
 *
 *                // Set the IP callback
 *                myIpCallback = myDhcpIpCallbackFunc;
 *
 *                // Configure DHCP IP with callback
 *                retStatus = NetworkConfig_SetIpDhcp(myIpCallback);
 * @endcode
 *
 * Example2 usage:
 *                Setting DHCP IP without Callback
 * @code
 *                // Configure DHCP IP without callback.
 *                // Note that this is the default function in case no other
 *                // function is used in order to set the IP
 *                retStatus = NetworkConfig_SetIpDhcp(NULL);
 * @endcode
 *
 ******************************************************************************/
Retcode_T NetworkConfig_SetIpDhcp(NetworkConfig_IpCallback_T myIpCallback);

/******************************************************************************/
/**
 * @brief         Function to convert an IP to hexadecimal.
 *
 * @param [in]    add3
 *                Decimal value of the first byte (MSB) of an IPV4
 * @param [in]    add2
 *                Decimal value of the second byte of an IPV4
 * @param [in]    add1
 *                Decimal value of the third byte of an IPV4
 * @param [in]    add0
 *                Decimal value of the last byte (LSB) of an IPV4
 *
 * @return        On success returns the integer value of IP from given hexadecimal value
 * @return        any error value refer #SL_IPV4_VAL error codes
 *
 * Example usage:
 *                Transform IPV4 to hexadecimal value
 * @code
 *                // Local variables
 *                NetworkConfig_IpSettings_T myIpSet;
 *
 *                // After calling the function the IPV4 values shall be :
 *                // 0xc0a80278, the equivalent for 192.168.2.120
 *                // 0xc0a80201, the equivalent for 192.168.2.1
 *                // 0xffffff00, the equivalent for 255.255.255.0
 *                myIpSet.ipv4          = NetworkConfig_Ipv4Value(192,168,2,120);
 *                myIpSet.ipV4DnsServer = NetworkConfig_Ipv4Value(192,168,2,1);
 *                myIpSet.ipV4Gateway   = NetworkConfig_Ipv4Value(192,168,2,1);
 *                myIpSet.ipV4Mask      = NetworkConfig_Ipv4Value(255,255,255,0);
 * @endcode
 *
 ******************************************************************************/
uint32_t NetworkConfig_Ipv4Value(uint8_t add3, uint8_t add2, uint8_t add1, uint8_t add0);

/******************************************************************************/
/**
 * @brief         Function to convert an hexadecimal to IP.
 *
 * @param [in]    ipValue
 *                Hexadecimal value of the IP V4
 * @param [in]    index
 *                Determines which byte of the IPV4 will be returned.It should be from 0 to 3
 *
 * @return        On success returns the integer value of IP from given hexadecimal value
 * @return        any error value refer #SL_IPV4_BYTE error codes
 *
 * Example usage:
 *                Extract IPV4 byte from the hexadecimal IP value and print it
 * @code
 *                // Get the IP settings
 *                NetworkConfig_GetIpSettings(&myIpGet);
 *
 *                // Print the retrieved IP address. The Gateway, DNS and Mask
 *                // can also be extracted in the same manner
 *                printf(" - The static IP was retrieved : %u.%u.%u.%u \n\r",
 *                      (unsigned int) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 3)),
 *                      (unsigned int) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 2)),
 *                      (unsigned int) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 1)),
 *                      (unsigned int) (NetworkConfig_Ipv4Byte(myIpGet.ipV4, 0)));
 * @endcode
 *
 ******************************************************************************/
uint8_t NetworkConfig_Ipv4Byte(uint32_t ipValue, uint8_t index);

/******************************************************************************/
/**
 * @brief         Function to get the IP status.
 *                This function return the IP status.
 *
 * @param [in]    none
 *
 * @return        NETWORKCONFIG_IPV4_ACQUIRED           - IPv4 was acquired
 * @return        NETWORKCONFIG_IPV6_ACQUIRED           - IPv6 was acquired (currently not supported)
 * @return        NETWORKCONFIG_IP_NOT_ACQUIRED         - IP was not acquired
 *
 * Example usage:
 *                Block until IP status is acquired and Wlan is connected
 * @code
 *                while ((WLI_DISCONNECTED == WlanConnect_GetStatus())
 *                   || (NETWORKCONFIG_IP_NOT_ACQUIRED == NetworkConfig_GetIpStatus()))
 *                {
 *                    // Stay here until connected and ip acquired.
 *                    // Timeout logic can be added here.
 *                }
 * @endcode
 * @warning       This API must be called only after initialization
 *
 ******************************************************************************/
NetworkConfig_IpStatus_T NetworkConfig_GetIpStatus(void);

/**
 * @brief This API returns back the Ip address by the host name
 *
 * @param[in]: url
 *               The string pointer holding the host name
 * @param[out]: destAddr
 *               The variable in which IP address to be stored
 *
 * @return     RETCODE_OK           IP address returned succesffuly
 * @return     RETCODE_FAILURE      Error occurred in fetching the ip address
 * @return     RETCODE_NULL_POINTER if the input parameter passes NULL
 *
 * Example usage:
 *
 * @code
 *                uint8_t url[] = "www.google.com"
 *                uint32_t ipAddress = 0x00;
 *                Retcode_T ret;
 *                ret = NetworkConfig_GetIpaddress(url, &ipAddress);
 *                if(RETCODE_OK != ret)
 *                {
 *                  //handle error
 *                }
 * @endcode
*/
Retcode_T NetworkConfig_GetIpAddress(uint8_t *url, uint32_t *destAddr);

/* inline function definitions */

#endif /* BCDS_NETWORKCONFIG_H_ */

/** @} */

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

/* module includes ********************************************************** */

#include "BCDS_Wlan.h"
/* This is used to define the correct module ID for generation of module
 * error codes
 */
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID (uint32_t)BCDS_WLAN_MODULE_ID_NETWORK_CONFIG

/* own header files */
#include "BCDS_NetworkConfig.h"
#include "NetworkConfig.h"

/* additional interface header files */
#include "simplelink.h"
#include "BCDS_WlanConnect.h"
//#include "BCDS_BSP_Board.h"

/* constant definitions ***************************************************** */

/* local variables ********************************************************** */

/* global variables ********************************************************* */
static volatile NetworkConfig_IpCallback_T IpConfigCallback; /**< Variable for storing the DHCP IP callback*/
static volatile NetworkConfig_IpStatus_T NetworkIpStatus; /**< Flag variable for IP obtained status */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */
void NetworkConfig_SlNetAppEvtHdlr(SlNetAppEvent_t *pNetAppEvent)
{
    SlNetAppEvent_t *pNetApp = (SlNetAppEvent_t *) pNetAppEvent;
    switch (pNetApp->Event)
    {
    case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        
        /* IP v4 was obtained*/
        NetworkIpStatus = NETWORKCONFIG_IPV4_ACQUIRED;
        /* Call function for non-blocking function with callback*/
        if (NULL != IpConfigCallback)
        {
            IpConfigCallback(NETWORKCONFIG_IPV4_ACQUIRED);
        }
        break;
    case SL_NETAPP_IPV6_IPACQUIRED_EVENT:
        /* IP v6 was obtained*/
        /*IPV6 not supported */
        break;
    case SL_NETAPP_IP_LEASED_EVENT:
        /* Ip was leased*/
        /* relevant in AP or P2P GO mode */
        break;
    case SL_NETAPP_IP_RELEASED_EVENT:
        /* Ip was released. Set status for blocking function */
        NetworkIpStatus = NETWORKCONFIG_IP_NOT_ACQUIRED;
        /* Call function for non-blocking function with callback*/
        if (NULL != IpConfigCallback)
        {
            IpConfigCallback(NETWORKCONFIG_IP_NOT_ACQUIRED);
        }
        break;
    default:
        break;
    }
}

/* global functions ********************************************************* */
NetworkConfig_IpStatus_T NetworkConfig_GetIpStatus(void)
{
    return (NetworkIpStatus);
}

void NetworkConfig_SetIpStatus(NetworkConfig_IpStatus_T ipStatus)
{
    NetworkIpStatus = ipStatus;
}

Retcode_T NetworkConfig_GetIpSettings(NetworkConfig_IpSettings_T* myIpSettings)
{
    if (NULL == myIpSettings)
    {
        return (RETCODE(RETCODE_SEVERITY_FATAL, (uint32_t ) RETCODE_NULL_POINTER));
    }
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_GetInitStatus())
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    Retcode_T returnValue = RETCODE_OK;
    int32_t slStatus = NETWORKCONFIG_SUCCESS;
    SlNetCfgIpV4Args_t ipV4;
    uint32_t len = UINT8_C(0);

    memset(&ipV4, 0, sizeof(ipV4));
    len = sizeof(SlNetCfgIpV4Args_t);

    /* Read the IP parameter */
    slStatus = sl_NetCfgGet((uint8_t) SL_IPV4_STA_P2P_CL_GET_INFO,
            (uint8_t*) &myIpSettings->isDHCP, (uint8_t*) &len,
            (uint8_t*) &ipV4);

    /* determine return value*/
    if (NETWORKCONFIG_FAILURE == slStatus)
    {
        returnValue = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_FAILURE);
    }
    else
    {
        myIpSettings->ipV4 = ipV4.ipV4;
        myIpSettings->ipV4DnsServer = ipV4.ipV4DnsServer;
        myIpSettings->ipV4Gateway = ipV4.ipV4Gateway;
        myIpSettings->ipV4Mask = ipV4.ipV4Mask;
    }

    /* return API status*/
    return (returnValue);
}

Retcode_T NetworkConfig_SetIpStatic(NetworkConfig_IpSettings_T myIpSettings)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_GetInitStatus())
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    /* local variables */
    Retcode_T returnValue = RETCODE_OK;
    int32_t slStatus = NETWORKCONFIG_SUCCESS;
    SlNetCfgIpV4Args_t ipV4;
    uint32_t len = UINT8_C(0);

    /* set parameters */
    len = sizeof(SlNetCfgIpV4Args_t);

    ipV4.ipV4 = (unsigned long) (myIpSettings.ipV4);
    ipV4.ipV4DnsServer = (unsigned long) (myIpSettings.ipV4DnsServer);
    ipV4.ipV4Gateway = (unsigned long) (myIpSettings.ipV4Gateway);
    ipV4.ipV4Mask = (unsigned long) (myIpSettings.ipV4Mask);

    /* set static ip*/
    slStatus = sl_NetCfgSet((uint8_t) SL_IPV4_STA_P2P_CL_STATIC_ENABLE, NETWORKCONFIG_ONE, (uint8_t) len, (uint8_t*) &ipV4);
    /* restart the device*/
    if (NETWORKCONFIG_FAILURE != slStatus)
    {
        slStatus = sl_Stop((_u16) NETWORKCONFIG_ZERO);
    }

    if (NETWORKCONFIG_SUCCESS == slStatus)
    {
        BSP_Board_Delay(1000);
        slStatus = sl_Start(NULL, NULL, NULL);

        if (ROLE_STA != (SlWlanMode_e) slStatus)
        {
            returnValue = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_FAILURE);
        }
        else
        {
            returnValue = RETCODE_OK;
        }
    }
    else
    {
        returnValue = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_FAILURE);

    }
    /* return API status*/
    return (returnValue);
}

Retcode_T NetworkConfig_SetIpDhcp(NetworkConfig_IpCallback_T myIpCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_GetInitStatus())
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    Retcode_T returnValue = RETCODE_OK;
    int32_t slStatus = NETWORKCONFIG_SUCCESS;
    uint8_t val = UINT8_C(0);

    val = NETWORKCONFIG_ONE;
    IpConfigCallback = myIpCallback;

    slStatus = sl_NetCfgSet((uint8_t) SL_IPV4_STA_P2P_CL_DHCP_ENABLE, NETWORKCONFIG_ONE, NETWORKCONFIG_ONE, &val);

    /* restart the device*/
    if (NETWORKCONFIG_FAILURE != slStatus)
    {
        slStatus = sl_Stop(NETWORKCONFIG_TIMEOUT);
    }
    if (NETWORKCONFIG_SUCCESS == slStatus)
    {
        BSP_Board_Delay(1000);
        slStatus = sl_Start(NULL, NULL, NULL);
    }
    if (ROLE_STA != (SlWlanMode_e) slStatus)
    {
        returnValue = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_FAILURE);
    }

    /* return API status*/
    return (returnValue);
}

uint32_t NetworkConfig_Ipv4Value(uint8_t add3, uint8_t add2, uint8_t add1, uint8_t add0)
{
    uint32_t returnValue;

    returnValue = SL_IPV4_VAL(add3, add2, add1, add0);

    return (returnValue);
}

uint8_t NetworkConfig_Ipv4Byte(uint32_t ipValue, uint8_t index)
{
    uint8_t returnValue;

    returnValue = SL_IPV4_BYTE(ipValue, index);

    return (returnValue);
}

Retcode_T NetworkConfig_GetIpAddress(uint8_t *url, uint32_t *destAddr)
{
    Retcode_T retValue = RETCODE_OK;

    if ((NULL == url) || (NULL == destAddr))
    {
        return RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
    }
    int16_t retVal = sl_NetAppDnsGetHostByName((int8_t *) url, strlen((char *) url), (long unsigned int *)destAddr, SL_AF_INET);
    if (INT16_C(0) == retVal)
    {
        *destAddr = sl_Htonl(*destAddr);
        retValue = RETCODE_OK;
    }
    else
    {
        retValue = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_FAILURE);

    }
    return retValue;
}

/** ************************************************************************* */

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
//lint -esym(956,*) /* Suppressing "Non const, non volatile static or external variable" lint warning*/
/* module includes ********************************************************** */

/* This is used to define the correct module ID for generation of module
 * error codes
 */
#include "BCDS_Wlan.h"
#undef BCDS_MODULE_ID
#define BCDS_MODULE_ID (uint32_t)BCDS_WLAN_MODULE_ID_WLAN_CONNECT

/* own header files */
#include "BCDS_WlanConnect.h"
#include "WlanConnect.h"
#include "BCDS_WlanDriver.h"
#include "BCDS_NetworkConfig.h"
#include "NetworkConfig.h"

/* additional interface header files */
//#include "BCDS_BSP_Board.h"
#include "simplelink.h"
#include "rhino.h"
//#include "task.h"
//#include "timers.h"
#include <aos/init.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <k_timer.h>
#include <aos/yloop.h>

/* constant definitions ***************************************************** */

/* local variables ********************************************************** */
static volatile WlanConnect_Callback_T WlanConnectCallback = NULL; /**< Variable for storing the connection callback*/
static volatile WlanConnect_DisconnectCallback_T WlanDisconnectCallback = NULL; /**< Variable for storing the disconnection callback from disconnect function*/
static ktimer_t ConnectTimerHandle;
static ktimer_t DisConnectTimerHandle;
static Sl_WlanNetworkEntry_t NetEntries[WLANCONNECT_SCAN_TABLE_SIZE]; /**< Variable for storing the scanned network entries */
static volatile WlanConnect_Status_T WlanConnect_Status; /**< Flag variable for WLI connect status*/
static volatile uint8_t WlanConnect_InitStatus; /**< Flag variable for Wlan init status*/

/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */
void disConnectNonBlockingCall(void *xTimer, void *arg)
{
    BCDS_UNUSED(xTimer);
    BCDS_UNUSED(arg);
    Retcode_T retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_RTOS_QUEUE_ERROR);

    static uint8_t Counter = UINT8_C(0);
    Counter++;
    if (WLAN_DISCONNECTED == WlanConnect_Status)
    {
        WlanDisconnectCallback(WLAN_DISCONNECTED);
        Counter = UINT8_C(0);
        if (krhino_timer_del(&DisConnectTimerHandle) != RHINO_SUCCESS)
        {
            Retcode_RaiseError(retVal);
        }
        return;
    }
    else if ((WLAN_DISCONNECTED != WlanConnect_Status) && (Counter >= WLANCONNECT_MAX_TRIES))
    {
        /* Not connected */
        WlanDisconnectCallback(WLAN_DISCONNECT_ERROR);
    }
    if ((Counter >= WLANCONNECT_MAX_TRIES))
    {
        Counter = UINT8_C(0);
        if (krhino_timer_del(&DisConnectTimerHandle) != RHINO_SUCCESS)
        {
            Retcode_RaiseError(retVal);
        }
    }
}
/** This is a timer callback called from connect API's when connection needs to be non-blocking */
void connectNonBlockingCall(void *xTimer, void *arg)
{
    BCDS_UNUSED(xTimer);
    BCDS_UNUSED(arg);

    Retcode_T retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_RTOS_QUEUE_ERROR);

    static uint8_t Counter = UINT8_C(0);
    Counter++;
    if ((WLAN_CONNECTED == WlanConnect_Status)
            && (NETWORKCONFIG_IPV4_ACQUIRED == NetworkConfig_GetIpStatus()))
    {
        /* connected and IP acquired*/
        WlanConnectCallback(WLAN_CONNECTED);
        Counter = UINT8_C(0);
        if (krhino_timer_del(&ConnectTimerHandle) != RHINO_SUCCESS)
        {
            Retcode_RaiseError(retVal);
        }
        return;
    }
    else if ((WLAN_DISCONNECTED == WlanConnect_Status) && (Counter >= WLANCONNECT_MAX_TRIES))
    {
        /* Not connected */
        WlanConnectCallback(WLAN_CONNECTION_ERROR);
    }
    else if ((WLAN_CONNECTION_PWD_ERROR == WlanConnect_Status) && (Counter >= WLANCONNECT_MAX_TRIES))
    {
        /*Password error*/
        WlanConnectCallback(WLAN_CONNECTION_PWD_ERROR);
    }
    else if ((WLAN_CONNECTED == WlanConnect_Status)
            && (NETWORKCONFIG_IP_NOT_ACQUIRED == NetworkConfig_GetIpStatus()) && (Counter >= WLANCONNECT_MAX_TRIES))
    {
        /* connected but IP not acquired*/
        WlanConnectCallback(WLAN_CONNECTION_ERROR);
    }
    if ((Counter >= WLANCONNECT_MAX_TRIES))
    {
        Counter = UINT8_C(0);
        if (krhino_timer_del(&ConnectTimerHandle) != RHINO_SUCCESS)
        {
            Retcode_RaiseError(retVal);
        }
    }
}

/** This function is called by connect API's when connection needs to be blocking */
static Retcode_T connectBlockingCall(void)
{
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    uint8_t Counter = UINT8_C(0);

    while ((WLAN_DISCONNECTED == WlanConnect_Status)
            || (NETWORKCONFIG_IP_NOT_ACQUIRED == NetworkConfig_GetIpStatus()))

    {
        /* Timeout logic added to avoid blocking */
        aos_msleep(WLANCONNECT_ONE_SEC_DELAY);
        Counter++;
        /* After maximum retry it will come out of this loop*/
        if (WLANCONNECT_MAX_TRIES <= Counter)
        {
            break;
        }
    }
    if ((WLAN_CONNECTED == WlanConnect_Status) && (NETWORKCONFIG_IPV4_ACQUIRED == NetworkConfig_GetIpStatus()))
    {
        /* connected and IP acquired*/
        retVal = RETCODE_OK;
    }
    else if (WLAN_DISCONNECTED == WlanConnect_Status)
    {
        /* not connected */
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_CONNECTION_ERROR);
    }
    else if (WLAN_CONNECTION_PWD_ERROR == WlanConnect_Status)
    {
        /*Password error */
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_ERROR_WRONG_PASSWORD);
    }
    else
    {
        /* connected but IP not acquired*/
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_ERROR_IP_NOT_ACQ);
    }

    return retVal;
}

/**
 * @brief   Function that retrieves time stamp. Currently retrieving
 * @param   time_ptr
 *          Pointer containing time in seconds.
 * @return  returnClockStatus
 *          Return status for the SystemClock_getTime function
 */
static Retcode_T SystemClock_getTime(uint32_t *time_ptr)
{
    /* function should be adapted in order to provide real time stamps */
    Retcode_T returnClockStatus = (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_TIME_NULL));
    *time_ptr = (((xTaskGetTickCount()) / portTICK_RATE_MS) * 1000);
    if (WLANCONNECT_ZERO != *time_ptr)
    {
        returnClockStatus = RETCODE_OK;
    }
    return (returnClockStatus);
}

/* global functions ********************************************************* */
/**
 * @brief
 *      An event handler for general error handling from the simplelink
 *
 *
 * @param[in]  pGeneralEvent - Contains the relevant event information
 *
 * Example Usage:
 * <pre>
 * \code{.c}
 *void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pGeneralEvent)
 *{
 *  assert(NULL != pGeneralEvent);
 *  printf("SL error 0x%x\n", pGeneralEvent->Event);
 *}
 * \endcode
 * </pre>
 * @Warning
 *      This callback is unused in this application
 */
void WlanConnect_SlGeneralEventHdlr(SlDeviceEvent_t *pGeneralEvent)
{
    if (NULL == pGeneralEvent)
    {
        Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_NULL_POINTER));
        return;
    }

    switch (pGeneralEvent->Event)
    {
    case SL_DEVICE_GENERAL_ERROR_EVENT:
        /* This error is returned by WiFI link when Password is wrong */
        if (SL_ERR_SENDER_WILINK == pGeneralEvent->EventData.deviceEvent.sender)
        {
            WlanConnect_Status = WLAN_CONNECTION_PWD_ERROR;
        }
        break;
//        /* @todo - Have a proper implementation for SL_DEVICE_DRIVER_TIMEOUT_ASYNC_EVENT */
//    case SL_DEVICE_DRIVER_TIMEOUT_ASYNC_EVENT:
//            if(SL_ERR_SENDER_HEALTH_MON == pGeneralEvent->EventData.deviceEvent.sender)
//            {
//                WlanConnect_Status = WLAN_CONNECTION_ERROR;
//            }
//        break;
    default:
        {
        int16_t returnValue = WLANCONNECT_ZERO;
        Retcode_T retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
        /* For Other error events we need to reset the device*/
        /* set connection status to disconnected */
        WlanConnect_Status = WLAN_DISCONNECTED;
        /* set IP to not acquired*/
        NetworkConfig_SetIpStatus(NETWORKCONFIG_IP_NOT_ACQUIRED);
        returnValue = sl_Stop((_u16) WLANCONNECT_ZERO);

        if (WLANCONNECT_ZERO == returnValue)
        {
            returnValue = sl_Start(NULL, NULL, NULL);
        }
        if (WLANCONNECT_ZERO != returnValue)
        {
            Retcode_RaiseError(retVal);
        }
    }
        break;
    }
}

void WlanConnect_SLWlanEventHdlr(SlWlanEvent_t *pWlanEvents)
{
    if (NULL == pWlanEvents)
    {
        Retcode_RaiseError(RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_NULL_POINTER));
        return;
    }

    switch (pWlanEvents->Event)
    {
    case SL_WLAN_CONNECT_EVENT:
        /* set status for blocking function */
        WlanConnect_Status = WLAN_CONNECTED;
        //aos_post_event(EV_WIFI, CODE_WIFI_ON_PRE_GOT_IP, 0u);
        break;
    case SL_WLAN_DISCONNECT_EVENT:
        /* set connection status to disconnected */
        WlanConnect_Status = WLAN_DISCONNECTED;
        /* set IP to not acquired*/
        NetworkConfig_SetIpStatus(NETWORKCONFIG_IP_NOT_ACQUIRED);
        break;
    default:
        break;
    }
}

WlanConnect_Status_T WlanConnect_GetStatus(void)
{
    return (WlanConnect_Status);
}

uint8_t WlanConnect_GetInitStatus(void)
{
    return (WlanConnect_InitStatus);
}

Retcode_T WlanConnect_Init(void)
{
    
     /* local variables */
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    int16_t returnValue = WLANCONNECT_SUCCESS;
    if (WLANCONNECT_INITIALZED == WlanConnect_InitStatus)
    {
        return retVal;
    }
    /* initialize WLAN Driver */
    retVal = WlanDriver_Init();
    if (RETCODE_OK != retVal)
    {
        return retVal;
    }
     /* initialize the connection status variable*/
    WlanConnect_Status = WLAN_DISCONNECTED;
    NetworkConfig_SetIpStatus(NETWORKCONFIG_IP_NOT_ACQUIRED);

    /* initialize WLAN Stack */
    
    uint8_t val = UINT8_C(0);
    int32_t slStatus = NETWORKCONFIG_SUCCESS;

    val = NETWORKCONFIG_ONE;

     returnValue = sl_Start(NULL, NULL, NULL);
    printf("sl_Start %d\n",returnValue);
    /* Disable Webserver, that is enabled by default */
    if (ROLE_STA == (SlWlanMode_e) returnValue)
    {
         returnValue = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
        if (WLANCONNECT_SUCCESS <= returnValue)
        {
            WlanConnect_InitStatus = WLANCONNECT_INITIALZED;
            retVal = RETCODE_OK;
        }
        else
        {
            WlanConnect_InitStatus = WLANCONNECT_NOT_INITIALZED;
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SIMPLELINK_ERROR);
        }
    }
    else if ((ROLE_AP == (SlWlanMode_e) returnValue) || (ROLE_P2P == (SlWlanMode_e) returnValue))
    {        
         returnValue = sl_WlanSetMode(ROLE_STA);
        if (0 == returnValue)
        {
            if (0 > sl_Stop(0))
            {
                retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SIMPLELINK_STOP_FAILED);
            }
            if (RETCODE_OK == retVal)
            {
                if (ROLE_STA == sl_Start(NULL, NULL, NULL))
                {
                    /* Disable Webserver, that is enabled by default */
                    returnValue = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
                    if (WLANCONNECT_SUCCESS <= returnValue)
                    {
                        WlanConnect_InitStatus = WLANCONNECT_INITIALZED;
                    }
                    else
                    {
                        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SIMPLELINK_ERROR);
                    }
                }
                else
                {
                    retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SIMPLELINK_STA_MODE_FAILED);
                }
            }
        }
        else
        {
            retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SIMPLELINK_STA_MODE_FAILED);
        }
    }
    else
    {        
         WlanConnect_InitStatus = WLANCONNECT_NOT_INITIALZED;
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SIMPLELINK_ERROR);
    }
    if (RETCODE_OK != retVal)
    {
        Retcode_T retValWlan = WlanDriver_DeviceDisable();
        if (RETCODE_OK == retValWlan)
        {
            retValWlan = WlanDriver_DeInit();
        }
    }

    /* return API status*/
    return (retVal);
}

Retcode_T WlanConnect_DeInit(void)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    /* local variables */
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    int16_t slStatus = WLANCONNECT_SUCCESS;
    WlanConnect_InitStatus = WLANCONNECT_NOT_INITIALZED;
    /* initialize the connection status variable*/
    WlanConnect_Status = WLAN_DISCONNECTED;
    NetworkConfig_SetIpStatus(NETWORKCONFIG_IP_NOT_ACQUIRED);

    /* de-initialize WLAN Stack after 0 miliseconds */
    slStatus = sl_Stop((_u16) WLANCONNECT_TIMEOUT_VAL);

    /* determine return value*/
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        retVal = WlanDriver_DeInit();
    }

    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
    }

    /* return API status*/
    return (retVal);
}

static Retcode_T NonBlockingTimer(void)
{
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    if (RHINO_SUCCESS == krhino_timer_create(&ConnectTimerHandle, (const name_t *) "connect non-blocking call", connectNonBlockingCall,
    WLANCONNECT_TIMER_TICKS, WLANCONNECT_TIMER_TICKS, NULL, TIMER_AUTORELOAD_ON))
    {
//        if (krhino_timer_start(&ConnectTimerHandle) != RHINO_SUCCESS)
//        {
//            retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_TIMERSTART_FAILURE);
//        }
//        else
        {
            retVal = RETCODE_OK;
        }
    }
    else
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_TIMERCREATE_FAILURE);
    }
    return retVal;
}

Retcode_T WlanConnect_Open(WlanConnect_SSID_T connectSSID,
        WlanConnect_Callback_T connectCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    if (NULL == connectSSID)
    {
        return (RETCODE(RETCODE_SEVERITY_FATAL, (uint32_t ) RETCODE_NULL_POINTER));
    }
    /* Local variables */
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    int16_t slStatus = WLANCONNECT_SUCCESS;
    SlSecParams_t secParams;

    /* Set network parameters */
    secParams.KeyLen = WLANCONNECT_ZERO;
    secParams.Type = SL_SEC_TYPE_OPEN;

    /* Store callback function */
    WlanConnectCallback = connectCallback;

    /* Call the connect function */
    slStatus = sl_WlanConnect(connectSSID, strlen(((char*) connectSSID)), NULL, &secParams, NULL);

    /* Determine return value*/
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        if (NULL == connectCallback)
        {
            retVal = connectBlockingCall();
        }
        else
        {
            retVal = NonBlockingTimer();
        }
    }
    else
    {
        /* Simple Link function encountered an error.*/
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
    }

    /* return API status*/
    return (retVal);
}

Retcode_T WlanConnect_WEP_Open(WlanConnect_SSID_T connectSSID,
        WlanConnect_PassPhrase_T connectPass, uint8_t passPhraseLength,
        WlanConnect_Callback_T connectCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    if (UINT8_C(0) == passPhraseLength)
    {
        return (RETCODE(RETCODE_SEVERITY_FATAL, (uint32_t ) RETCODE_INVALID_PARAM));

    }
    if ((NULL == connectSSID) || (NULL == connectPass))
    {
        return (RETCODE(RETCODE_SEVERITY_FATAL, (uint32_t ) RETCODE_NULL_POINTER));
    }
    /* Local variables */
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    int16_t slStatus = WLANCONNECT_SUCCESS;
    SlSecParams_t secParams;

    /* Set network parameters */
    secParams.Key = connectPass;
    secParams.KeyLen = passPhraseLength;
    secParams.Type = SL_SEC_TYPE_WEP;

    /* Store callback function */
    WlanConnectCallback = connectCallback;

    /* Call the connect function */
    slStatus = sl_WlanConnect(connectSSID, strlen(((const char*) connectSSID)), NULL, &secParams, NULL);

    /* Determine return value*/
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        if (NULL == connectCallback)
        {
            retVal = connectBlockingCall();
        }
        else
        {
            retVal = NonBlockingTimer();
        }
    }
    else
    {
        /* Simple Link function encountered an error.*/
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
    }

    return (retVal);
}

Retcode_T WlanConnect_WPA(WlanConnect_SSID_T connectSSID,
        WlanConnect_PassPhrase_T connectPass,
        WlanConnect_Callback_T connectCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    if ((NULL == connectSSID) || (NULL == connectPass))
    {
        return (RETCODE(RETCODE_SEVERITY_FATAL, (uint32_t ) RETCODE_NULL_POINTER));
    }
    /* Local variables */
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    int16_t slStatus = WLANCONNECT_SUCCESS;
    SlSecParams_t secParams;

    /* Set network parameters */
    secParams.Key = connectPass;
    secParams.KeyLen = strlen((const char*) connectPass);
    secParams.Type = SL_SEC_TYPE_WPA;

    /* Store callback function */
    WlanConnectCallback = connectCallback;

    /* Call the connect function */
    slStatus = sl_WlanConnect(connectSSID, strlen(((char*) connectSSID)), NULL, &secParams, NULL);

    /* Determine return value*/
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        if (NULL == connectCallback)
        {
            retVal = connectBlockingCall();
        }
        else
        {
            retVal = NonBlockingTimer();
        }
    }
    else
    {
        /* Simple Link function encountered an error.*/
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
    }

    return (retVal);
}

Retcode_T WlanConnect_WPS_PBC(WlanConnect_Callback_T connectCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    /* Local variables */
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    int16_t slStatus = WLANCONNECT_SUCCESS;
    SlSecParams_t secParams;

    secParams.Key = NULL;
    secParams.KeyLen = WLANCONNECT_ZERO;
    secParams.Type = SL_SEC_TYPE_WPS_PBC;

    /* Store callback function */
    WlanConnectCallback = connectCallback;

    /* Call the connect function */
    slStatus = sl_WlanConnect((signed char*) WLANCONNECT_DUMMY_SSID, strlen(((char*) WLANCONNECT_DUMMY_SSID)), NULL, &secParams, NULL);

    /* Determine return value*/
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        if (NULL == connectCallback)
        {
            retVal = connectBlockingCall();
        }
        else
        {
            retVal = NonBlockingTimer();
        }
    }
    else
    {
        /* Simple Link function encountered an error.*/
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
    }

    /* return API status*/
    return (retVal);
}

Retcode_T WlanConnect_WPS_PIN(WlanConnect_Callback_T connectCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    /* Local variables */
    Retcode_T retVal = (Retcode_T) RETCODE_OK;
    int16_t slStatus = WLANCONNECT_SUCCESS;
    SlSecParams_t secParams;

    secParams.Key = (signed char*) WLANCONNECT_ENROLLEE_PIN;
    secParams.KeyLen = strlen((const char*) WLANCONNECT_ENROLLEE_PIN);
    secParams.Type = SL_SEC_TYPE_WPS_PIN;

    /* Store callback function */
    WlanConnectCallback = connectCallback;

    /* Call the connect function */
    slStatus = sl_WlanConnect((signed char*) WLANCONNECT_DUMMY_SSID, strlen(((char*) WLANCONNECT_DUMMY_SSID)), NULL, &secParams, NULL);

    /* Determine return value*/
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        if (NULL == connectCallback)
        {
            retVal = connectBlockingCall();
        }
        else
        {
            retVal = NonBlockingTimer();
        }
    }
    else
    {
        /* Simple Link function encountered an error.*/
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
    }

    /* return API status*/
    return (retVal);

}

Retcode_T WlanConnect_EnterpriseWPA(WlanConnect_SSID_T connectSSID, WlanConnect_Username_T connectUsername, WlanConnect_PassPhrase_T connectPass, WlanConnect_Callback_T connectCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_UNINITIALIZED));
    }
    if ((NULL == connectSSID) || (NULL == connectUsername) || (NULL == connectPass))
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER));
    }

    /* Local variables */
    Retcode_T retVal = RETCODE_OK;
    int16_t slStatus = WLANCONNECT_SUCCESS;
    SlSecParams_t secParams;
    SlSecParamsExt_t eapParams;

    /* Set network parameters */
    secParams.Key = connectPass;
    secParams.KeyLen = strlen((const char *) connectPass);
    secParams.Type = SL_SEC_TYPE_WPA_ENT;

    /* Set network EAP parameters */
    eapParams.User = (signed char *) connectUsername;
    eapParams.UserLen = strlen((const char *) connectUsername);
    eapParams.AnonUser = (signed char*) connectUsername;
    eapParams.AnonUserLen = strlen((const char *) connectUsername);
    eapParams.CertIndex = UINT8_C(0);
    eapParams.EapMethod = SL_ENT_EAP_METHOD_PEAP0_MSCHAPv2;

    /* Store callback function */
    WlanConnectCallback = connectCallback;

    /* Call the connect function */
    slStatus = sl_WlanConnect((signed char *) connectSSID, strlen(((char *) connectSSID)),
            (unsigned char *) WLANCONNECT_NOT_INITIALZED, &secParams, &eapParams);

    /* Determine return value*/
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        if (NULL == connectCallback)
        {
            retVal = connectBlockingCall();
        }
        else
        {
            retVal = NonBlockingTimer();
        }
    }
    else
    {
        /* Simple Link function encountered an error.*/
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_SIMPLELINK_ERROR);
    }

    return (retVal);
}

Retcode_T WlanConnect_DeleteAllProfiles(void)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    Retcode_T retVal = (Retcode_T) RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNEXPECTED_BEHAVIOR);
    int16_t slStatus = WLANCONNECT_SUCCESS;

    /* Delete all the WLAN saved profiles */
    slStatus = sl_WlanProfileDel(WLANCONNECT_ALL_PROFILES);

    if (WLANCONNECT_SUCCESS == slStatus)
    {
        retVal = RETCODE_OK;
    }
    else if (WLANCONNECT_FAILURE == slStatus)
    {
        retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR);
    }

    return (retVal);
}

Retcode_T WlanConnect_Disconnect(WlanConnect_DisconnectCallback_T disconnectCallback)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    /* Local variables */
    Retcode_T retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNEXPECTED_BEHAVIOR);
    int16_t slStatus = WLANCONNECT_SUCCESS;

    WlanDisconnectCallback = disconnectCallback;
    /* Disconnect from WLAN Network */
    slStatus = sl_WlanDisconnect();
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        if (NULL == disconnectCallback)
        {
            uint8_t Counter = UINT8_C(0);
            while ((WLAN_CONNECTED == WlanConnect_Status) || (WLAN_CONNECTION_PWD_ERROR == WlanConnect_Status))
            {
                /* Timeout logic added to avoid blocking */
                aos_msleep(WLANCONNECT_ONE_SEC_DELAY);
                Counter++;
                /* After maximum retry it will come out of this loop*/
                if (WLANCONNECT_MAX_TRIES <= Counter)
                {
                    break;
                }
            }
            if (WLAN_DISCONNECTED == WlanConnect_Status)
            {
                /* disconnected successfully*/
                retVal = RETCODE_OK;
            }
            if (WLAN_DISCONNECTED != WlanConnect_Status)
            {
                /* Not disconnected */
                retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_DISCONNECT_ERROR);
            }
        }
        else
        {
            if (RHINO_SUCCESS == krhino_timer_create(&DisConnectTimerHandle, (const name_t *) "Disconnect non-blocking call", disConnectNonBlockingCall,
            WLANCONNECT_TIMER_TICKS, WLANCONNECT_TIMER_TICKS, NULL, TIMER_AUTORELOAD_ON))
            {
//                if (krhino_timer_start(&DisConnectTimerHandle) != RHINO_SUCCESS)
//                {
//                    retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_TIMERSTART_FAILURE);
//                }
//                else
                {
                    return (RETCODE_OK);
                }
            }
            else
            {
                retVal = RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_TIMERCREATE_FAILURE);
            }
        }
    }
    else
    {
        if (NULL != disconnectCallback)
        {
            disconnectCallback(WLAN_DISCONNECTED);
        }
        retVal = RETCODE_OK;
    }

    return (retVal);
}

WlanConnect_CurrentNwStatus_T WlanConnect_GetCurrentNwStatus(void)
{
    WlanConnect_CurrentNwStatus_T retVal = DISCONNECTED_IP_NOT_ACQUIRED;

    if ((NETWORKCONFIG_IPV4_ACQUIRED == NetworkConfig_GetIpStatus())
            && (WLAN_CONNECTED == WlanConnect_Status))
    {
        retVal = CONNECTED_AND_IPV4_ACQUIRED;
    }
    else if ((NETWORKCONFIG_IP_NOT_ACQUIRED == NetworkConfig_GetIpStatus())
            && (WLAN_CONNECTED == WlanConnect_Status))
    {
        retVal = CONNECTED_AND_IP_NOT_ACQUIRED;
    }
    else if ((NETWORKCONFIG_IPV4_ACQUIRED == NetworkConfig_GetIpStatus())
            && (WLAN_DISCONNECTED == WlanConnect_Status))
    {
        retVal = DISCONNECTED_AND_IPV4_ACQUIRED;
    }
    else
    {
        retVal = DISCONNECTED_IP_NOT_ACQUIRED;
    }

    return (retVal);
}

Retcode_T WlanConnect_ScanNetworks(WlanConnect_ScanInterval_T scanInterval,
        WlanConnect_ScanList_T* scanList)
{
    if (WLANCONNECT_NOT_INITIALZED == WlanConnect_InitStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNINITIALIZED));
    }
    if (NULL == scanList)
    {
        return (RETCODE(RETCODE_SEVERITY_FATAL, (uint32_t ) RETCODE_NULL_POINTER));
    }
    /* local variables*/
    int16_t slStatus = WLANCONNECT_SUCCESS;
    Retcode_T retVal = (Retcode_T) RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_UNEXPECTED_BEHAVIOR);
    uint16_t runningIdx = WLANCONNECT_ZERO;
    uint32_t numOfEntries = WLANCONNECT_NO_OF_ENTRIES;
    uint8_t copyflag;
    uint8_t listPosition;

    /* initialize scan structures */
    memset(NetEntries, 0, sizeof(NetEntries));
    scanList->NumOfScanEntries = WLANCONNECT_ZERO;
    scanList->TimeStamp = WLANCONNECT_ZERO;
    memset(scanList->ScanData, 0, sizeof(scanList->ScanData));

    /* make sure the connection policy is not set (so no scan is run in the background) */
    slStatus = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(WLANCONNECT_ZERO, WLANCONNECT_ZERO, WLANCONNECT_ZERO, WLANCONNECT_ZERO,
                    WLANCONNECT_ZERO), NULL, WLANCONNECT_ZERO);
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        /* start scanning */
        slStatus = sl_WlanPolicySet(SL_POLICY_SCAN, WLANCONNECT_SCAN_ENABLE,
                (_u8*) &scanInterval, sizeof(scanInterval));
    }
    if (WLANCONNECT_SUCCESS == slStatus)
    {
        /* delay 1 second to make sure that scan is started */
        aos_msleep(portTICK_RATE_MS);

        /* fill 20 network entries 2 by 2*/
        do
        {
            slStatus = sl_WlanGetNetworkList(runningIdx, numOfEntries,
                    &NetEntries[runningIdx]);
            runningIdx += slStatus;
            /* check status */
            if (WLANCONNECT_FAILURE == slStatus)
            {
                return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR));
            }
        } while ((slStatus == (int16_t) numOfEntries)
                && (runningIdx < WLANCONNECT_SCAN_TABLE_SIZE));

        /* disable scan */
        slStatus = sl_WlanPolicySet(SL_POLICY_SCAN, WLANCONNECT_SCAN_DISABLE, NULL,
        WLANCONNECT_ZERO);
        /* check status */
        if (WLANCONNECT_FAILURE == slStatus)
        {
            return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR));
        }

        if (WLANCONNECT_SUCCESS == slStatus)
        {
            /* first element of the list is unique, therefore it must be copied*/
            copyflag = WLANCONNECT_ONE;
        }
        /* initialize scan data list position --> when repeating element subtract from network list*/
        listPosition = WLANCONNECT_ZERO;

        /* fill user network entries*/
        for (int8_t i = WLANCONNECT_ZERO; i < WLANCONNECT_SCAN_TABLE_SIZE; i++)
        {
            /* Search for duplicate SISD */
            for (int8_t sCount = i; sCount > WLANCONNECT_ZERO; sCount--)
            {
                if (WLANCONNECT_ZERO == memcmp(NetEntries[i].ssid,
                        NetEntries[sCount - 1].ssid,
                        sizeof(NetEntries[i].ssid)))
                {
                    /* the compared SSID are equal --> do not copy and exit for*/
                    copyflag = WLANCONNECT_ZERO;
                    sCount = WLANCONNECT_ZERO;
                    listPosition++;
                }
                else
                {
                    /* the compared SSID are different*/
                    copyflag = WLANCONNECT_ONE;
                }
            }
            /* Copy only unique SSID scan data*/
            if ((WLANCONNECT_ONE == copyflag))
            {
                /* Cast and copy the SSID */
                for (int8_t j = WLANCONNECT_ZERO; j < WLANCONNECT_MAX_SSID_LENGTH; j++)
                {
                    scanList->ScanData[i - listPosition].Ssid[j] = NetEntries[i].ssid[j];
                }
                /* Cast and copy the BSSID */
                for (int8_t k = WLANCONNECT_ZERO; k < WLANCONNECT_MAX_MAC_ADDR_LEN; k++)
                {
                    scanList->ScanData[i - listPosition].Bssid[k] = NetEntries[i].bssid[k];
                }
                /* Cast and copy the SSID Lenght, Security Type and RSSI */
                scanList->ScanData[i - listPosition].SsidLength = NetEntries[i].ssid_len;
                scanList->ScanData[i - listPosition].SecurityType = NetEntries[i].sec_type;
                scanList->ScanData[i - listPosition].Rssi = NetEntries[i].rssi;

                /* Copy the number of valid entries */
                scanList->NumOfScanEntries = (i - listPosition + 1);
            }
        }
        /* Add the time stamp of the last scan*/
        slStatus = SystemClock_getTime(&scanList->TimeStamp);
    }

    /* check status */
    if ( WLANCONNECT_ZERO != slStatus)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_SIMPLELINK_ERROR));
    }

    /* Check if Scan function has not found any networks*/
    if (WLANCONNECT_ZERO == scanList->NumOfScanEntries)
    {
        return (RETCODE(RETCODE_SEVERITY_ERROR, (uint32_t ) RETCODE_NO_NW_AVAILABLE));
    }
    else
    {
        retVal = RETCODE_OK;
    }

    return (retVal);
}

/** ************************************************************************* */

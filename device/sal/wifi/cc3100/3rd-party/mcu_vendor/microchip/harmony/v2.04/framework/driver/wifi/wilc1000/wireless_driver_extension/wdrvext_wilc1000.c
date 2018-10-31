/*******************************************************************************
  WILC1000 Wireless Driver Extension

  File Name:
    wdrext_wilc1000.c

  Summary:
    WILC1000 wireless driver extension.

  Description:
    WILC1000 wireless driver extension.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
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

#include "wilc1000_task.h"
#include "nmasic.h"

void ConnectEventCB_bare(bool connected, bool isServer, uint8_t const *const client);
extern sint8 nm_get_firmware_info(tstrM2mRev* M2mRev);

static WDRV_CONFIG s_wdrv_cfg;
WDRV_CONFIG *gp_wdrv_cfg = &s_wdrv_cfg;

WILC1000_PRIV g_wdrvext_priv = {0, };
static WILC1000_INTF s_wilc1000_intf;
static WILC1000_SCAN_CONTROL s_scanControl;
static WILC1000_CONNECT_DATA s_connData;

OSAL_MUTEX_HANDLE_TYPE g_debugConsoleLock;
volatile static bool s_isInitComplete = false;

WDRV_task waiting_task ;

static void WDRV_EXT_InitConnData(WILC1000_CONNECT_DATA *data)
{
    data->isSoftAP = CONN_DATA_UNSPECIFIED;
    data->channel = CONN_DATA_UNSPECIFIED;
    data->secType = CONN_DATA_UNSPECIFIED;
    data->ssidLen = CONN_DATA_UNSPECIFIED;
    data->keyLen = CONN_DATA_UNSPECIFIED;
    data->keyIdx = CONN_DATA_UNSPECIFIED;
}

static bool is_conn_data_valid(WILC1000_CONNECT_DATA *data)
{
    if (data->isSoftAP == CONN_DATA_UNSPECIFIED) {
        WDRV_ASSERT(false, "Network type is not specified yet");
        return false;
    }
    if (data->isSoftAP) {
        if (data->channel == CONN_DATA_UNSPECIFIED) {
            WDRV_ASSERT(false, "Channel is not specified yet");
            return false;
        } else if (data->keyLen == CONN_DATA_UNSPECIFIED) {
            WDRV_ASSERT(false, "Key length is not specified yet");
            return false;
        }
    }
    if (data->secType == CONN_DATA_UNSPECIFIED) {
        WDRV_ASSERT(false, "Security type is not specified yet");
        return false;
    }
    if (data->ssidLen == CONN_DATA_UNSPECIFIED) {
        WDRV_ASSERT(false, "SSID is not specified yet");
        return false;
    }
    return true;
}

static void InitDoneCB(void)
{
	s_isInitComplete = true;
}

static void InitDoneCB_bare(void)
{
	s_isInitComplete = true;
	WDRV_DBG_INFORM_MESSAGE(("WILC1000: Initialization successful!\r\n"));
	WDRV_notifyTask(waiting_task, WDRV_MAC_EVENT_INIT_DONE, eSetBits);
}

static int32_t _WDRV_EXT_Initialize(WDRV_HOOKS const *const hooks)
{
	int32_t res = 0;

	memset(&s_wilc1000_intf, 0, sizeof(WILC1000_INTF));
	memset(&g_wdrvext_priv, 0, sizeof(WILC1000_PRIV));

	WDRV_EXT_InitConnData(&s_connData);
	WDRV_SEM_INIT(&g_wdrvext_priv.scanResultWait);
	WDRV_SEM_INIT(&g_wdrvext_priv.connInfoWait);
	WDRV_SEM_INIT(&g_wdrvext_priv.eventWait);

	s_wilc1000_intf.FrameReceived_CB = hooks->frame_received;
	s_wilc1000_intf.RFReady_CB = hooks->RFReady;
	s_wilc1000_intf.ScanDone_CB = hooks->ScanDone;
	s_wilc1000_intf.InitDone_CB = InitDoneCB;
	s_wilc1000_intf.get_rx_buf = hooks->get_rx_buf;
	s_wilc1000_intf.ConnectEvent_CB = hooks->ConnectEvent;
	s_wilc1000_intf.WPSDone_CB = hooks->WPSDone;

	if (hooks->isEthMode)
		s_wilc1000_intf.ethMode = true;

	if (hooks->isOtaFwUpdateRequested) {
		s_wilc1000_intf.ethMode = false;
		s_wilc1000_intf.fwOtaServerUrl = hooks->fwOtaServerUrl;
	}

	if (hooks->isSerialFwUpdateRequested) {
		res = WDRV_TASK_CREATE((void *const)wilc1000_fw_update, "WILC1000 task", 2048,
			&s_wilc1000_intf, configMAX_PRIORITIES - 1, &g_wdrvext_priv.handle);
	} else
    {
		res = WDRV_TASK_CREATE((void *const)wilc1000_task, "WILC1000 task", WDRV_EXT_RTOS_TASK_SIZE,
			&s_wilc1000_intf, WDRV_EXT_RTOS_TASK_PRIORITY, &g_wdrvext_priv.handle);
	}

	return res ? WDRV_ERROR : WDRV_SUCCESS;
}

static int32_t _WDRV_EXT_Initialize_bare(WDRV_HOOKS const *const hooks)
{
	int32_t res = 0;

	waiting_task =xTaskGetCurrentTaskHandle();

	memset(&s_wilc1000_intf, 0, sizeof(WILC1000_INTF));
	memset(&g_wdrvext_priv, 0, sizeof(WILC1000_PRIV));

	WDRV_EXT_InitConnData(&s_connData);
	WDRV_SEM_INIT(&g_wdrvext_priv.scanResultWait);
	WDRV_SEM_INIT(&g_wdrvext_priv.connInfoWait);
	WDRV_SEM_INIT(&g_wdrvext_priv.eventWait);

	s_wilc1000_intf.FrameReceived_CB = hooks->frame_received;
//	s_wilc1000_intf.RFReady_CB = hooks->RFReady;
//	s_wilc1000_intf.ScanDone_CB = ScanDoneHandler;
	s_wilc1000_intf.InitDone_CB = InitDoneCB_bare;
//	s_wilc1000_intf.get_rx_buf = hooks->get_rx_buf;
	s_wilc1000_intf.ConnectEvent_CB = ConnectEventCB_bare;
//	s_wilc1000_intf.WPSDone_CB = hooks->WPSDone;


	s_wilc1000_intf.ethMode = true;


	res = WDRV_TASK_CREATE((void *const)wilc1000_task_bare, "WILC1000 task", WDRV_EXT_RTOS_TASK_SIZE,
			&s_wilc1000_intf, WDRV_EXT_RTOS_TASK_PRIORITY, &g_wdrvext_priv.handle);

	return res ? WDRV_ERROR : WDRV_SUCCESS;
}


static void _WDRV_EXT_Deinitialize(void)
{
    g_wdrvext_priv.deinit_in_progress = true;
    WDRV_SEM_GIVE(&g_wdrvext_priv.eventWait);

    while (g_wdrvext_priv.deinit_in_progress)
        WDRV_TIME_DELAY(10);

    WDRV_SEM_DEINIT(&g_wdrvext_priv.eventWait);
    WDRV_SEM_DEINIT(&g_wdrvext_priv.scanResultWait);
    WDRV_SEM_DEINIT(&g_wdrvext_priv.connInfoWait);
	WDRV_TASK_DELETE(g_wdrvext_priv.handle);
}

bool isWdrvExtReady(void)
{
	return s_isInitComplete;
}

void WDRV_EXT_CmdSSIDSet(uint8_t *ssid, uint16_t len)
{
    s_connData.ssidLen = len;
    memcpy(s_connData.ssid, ssid, s_connData.ssidLen);
    s_connData.ssid[s_connData.ssidLen] = '\0';
}

void WDRV_EXT_CmdSSIDGet(uint8_t *ssid, uint8_t *ssidLen)
{
    *ssidLen = s_connData.ssidLen;
    memcpy(ssid, s_connData.ssid, *ssidLen);
    ssid[*ssidLen] = '\0';
}

uint32_t WDRV_EXT_CmdConnectContextBssidGet(uint8_t *bssId)
{
    int8_t ret;
    ret = m2m_wifi_get_connection_info();
    WDRV_ASSERT(ret == 0, "WDRV_EXT_CmdConnectContextBssidGet error");

    if (ret == 0) {
    	WDRV_SEM_TAKE(&g_wdrvext_priv.connInfoWait, OSAL_WAIT_FOREVER);    }

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

uint32_t WDRV_EXT_CmdMacAddressGet(uint8_t *MacAddr)
{
    int8_t ret;
	uint8_t mac0[6];


    ret = m2m_wifi_get_mac_address(mac0, MacAddr);
    WDRV_ASSERT(ret == 0, "WDRV_EXT_CmdMacAddressGet error");

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}


void WDRV_EXT_CmdNetModeBSSSet(void)
{
    s_connData.isSoftAP = 0;
}

void WDRV_EXT_CmdNetModeAPSet(void)
{
    s_connData.isSoftAP = 1;
}

int8_t WDRV_EXT_CmdNetModeGet(void)
{
    return s_connData.isSoftAP;
}

uint32_t WDRV_EXT_CmdConnect(void)
{
	int8_t ret;

	if (!is_conn_data_valid(&s_connData)) {
		WDRV_ASSERT(false, "Invalid connection data");
		return WDRV_ERROR;
	}

	waiting_task =xTaskGetCurrentTaskHandle();

	s_connData.channel = s_connData.channel == CONN_DATA_UNSPECIFIED ? M2M_WIFI_CH_ALL : s_connData.channel;

	ret = wilc1000_connect(&s_connData);
	WDRV_ASSERT(ret == 0, "WDRV_EXT_CmdConnect error");

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

uint32_t WDRV_EXT_CmdDisconnect(void)
{
	int8_t ret;

	if (s_connData.isSoftAP) {
		ret = m2m_wifi_disable_ap();
		WDRV_ASSERT(ret == 0, "main: m2m_wifi_disable_ap error");
		wilc1000_connect_event_received(false, true, NULL);
		WDRV_DBG_INFORM_MESSAGE(("Soft AP is disabled\r\n"));
	} else {
		ret = m2m_wifi_disconnect();
		WDRV_ASSERT(ret == 0, "main: m2m_wifi_disconnect error");
	}

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

uint32_t WDRV_EXT_CmdTxPowerSet(uint32_t level)
{
    int8_t ret;

    ret = m2m_wifi_set_tx_power(level);

    WDRV_ASSERT(ret == 0, "WDRV_EXT_CmdTxPowerSet error");

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

uint32_t
WDRV_EXT_CmdScanOptionSet(uint8_t numOfSlots, uint8_t slotTime, uint8_t probesPerSlot, uint8_t rssiThreshold)
{
    int8_t ret;

    ret = m2m_wifi_set_scan_options(numOfSlots, slotTime);
    WDRV_ASSERT(ret == 0, "WDRV_EXT_CmdScanModeSet error");

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

uint32_t WDRV_EXT_CmdPowerSavePut(bool enable, uint8_t mode, uint16_t listenInterval)
{
#if 0
    int32_t ret = 0;
    if (enable) {
        if (mode == 0) { /* ps manual */
            ret = m2m_wifi_set_sleep_mode(M2M_PS_MANUAL, 1);
            if (ret == 0) {
                ret = m2m_wifi_request_sleep(1000);
            }
        } else { /* deep automatic */
            tstrM2mLsnInt strM2mLsnInt;
		    ret = m2m_wifi_set_sleep_mode(M2M_PS_DEEP_AUTOMATIC, 1);
            if (ret == 0) {
		        strM2mLsnInt.u16LsnInt = listenInterval;
                ret = m2m_wifi_set_lsn_int(&strM2mLsnInt);
            }
        }
    } else {
        ret = m2m_wifi_set_sleep_mode(M2M_NO_PS, 0);
    }
	return ret ? WDRV_ERROR : WDRV_SUCCESS;
#else
	return WDRV_SUCCESS;
#endif
}

uint32_t WDRV_EXT_RssiRead(void)
{
	int32_t ret;

	/* Request RSSI for the connected AP. */
	ret = m2m_wifi_req_curr_rssi();

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

void WDRV_EXT_CmdSecNoneSet(void)
{
    s_connData.secType = M2M_WIFI_SEC_OPEN;
}

void WDRV_EXT_CmdSecWEPSet(uint8_t *key, uint16_t len)
{
    s_connData.secType = M2M_WIFI_SEC_WEP;
    s_connData.keyLen = len;
    s_connData.keyIdx = 1; /* 1 is chosen per default. This is configurable. */
    memcpy(s_connData.key, key, len);
    s_connData.key[s_connData.keyLen] = '\0';
}

void WDRV_EXT_CmdSecWPASet(uint8_t *key, uint16_t len)
{
    s_connData.secType = M2M_WIFI_SEC_WPA_PSK;
	s_connData.keyLen = len;
    memcpy(s_connData.key, key, len);
    s_connData.key[s_connData.keyLen] = '\0';
}

uint32_t WDRV_EXT_CmdSecWpsSet(bool pinMode, uint8_t *key, uint16_t keyLen)
{
    uint8_t keyStr[9];
    uint8_t type = pinMode ? WPS_PIN_TRIGGER : WPS_PBC_TRIGGER;
    int8_t ret;

    if (type == WPS_PIN_TRIGGER) {
        WDRV_ASSERT(keyLen == 8, "WDRV_EXT_CmdSecWpsSet error, incorrect WPS PIN length");
        memcpy((void *)keyStr, key, keyLen);
        keyStr[8] = '\0';
    }
    WDRV_DBG_INFORM_MESSAGE(("WPS starts...\r\n"));
    ret = m2m_wifi_wps(type, (const char *)keyStr);

    WDRV_ASSERT(ret == 0, "WDRV_EXT_CmdSecWpsSet error");

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

uint32_t WDRV_EXT_CmdFWVersionGet(uint32_t *major, uint32_t *minor, uint32_t *patch)
{
    tstrM2mRev strtmp;
    int32_t ret;

    ret = nm_get_firmware_info(&strtmp);

	WDRV_DBG_INFORM_PRINT(("Firmware ver:    %u.%u.%u\n", strtmp.u8FirmwareMajor, strtmp.u8FirmwareMinor, strtmp.u8FirmwarePatch));
	WDRV_DBG_INFORM_PRINT(("Min driver ver:  %u.%u.%u\n", strtmp.u8DriverMajor, strtmp.u8DriverMinor, strtmp.u8DriverPatch));
	WDRV_DBG_INFORM_PRINT(("Curr driver ver: %u.%u.%u\n", M2M_FIRMWARE_VERSION_MAJOR_NO, M2M_FIRMWARE_VERSION_MINOR_NO, M2M_FIRMWARE_VERSION_PATCH_NO));
	WDRV_ASSERT(M2M_ERR_FW_VER_MISMATCH != ret, "Firmware version does not match with driver");

    if (ret == 0) {
        *major = strtmp.u8FirmwareMajor;
        *minor = strtmp.u8FirmwareMinor;
        *patch = strtmp.u8FirmwarePatch;
    }

    return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

void WDRV_EXT_CmdChannelSet(uint16_t channel)
{
    s_connData.channel = (int8_t)channel;
}

uint32_t WDRV_EXT_CmdScanStart(void)
{
    int8_t ret;
    uint8_t channel = s_connData.channel == CONN_DATA_UNSPECIFIED ? M2M_WIFI_CH_ALL : s_connData.channel;

    ret = m2m_wifi_request_scan(channel);

    WDRV_ASSERT(ret == 0, "WDRV_EXT_CmdScanStart error");

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

void WDRV_EXT_CmdScanGet(uint16_t *numOfResults)
{
   *numOfResults = m2m_wifi_get_num_ap_found();
}

void WDRV_EXT_WPSResultsRead(WDRV_CONFIG *config, uint32_t *status)
{
    tstrM2MWPSInfo info;

    wilc1000_wps_info_read(&info);
    config->ssidLen = strlen((char *)info.au8SSID);
    memcpy((void *)config->ssid, (void *)info.au8SSID, sizeof(config->ssid));
    config->securityKeyLen = strlen((char *)info.au8PSK);
    memcpy((void *)config->securityKey, (void *)info.au8PSK, sizeof(config->securityKey));
    if (info.u8AuthType == M2M_WIFI_SEC_OPEN) {
        config->securityMode = WDRV_SECURITY_OPEN;
    } else if (info.u8AuthType == M2M_WIFI_SEC_WPA_PSK) {
        config->securityMode = WDRV_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
    } else if (info.u8AuthType == M2M_WIFI_SEC_WEP) {
        if (config->securityKeyLen == 10) /* e.g. strlen("1234567890")  */
            config->securityMode = WDRV_SECURITY_WEP_40;
        else /* e.g. strlen("1234567890abcdef1234567890") */
            config->securityMode = WDRV_SECURITY_WEP_104;
    }

	*status = info.u8AuthType != M2M_WIFI_SEC_INVALID ? WDRV_SUCCESS : WDRV_ERROR;
}

void WDRV_EXT_HWInterruptHandler(void)
{
    wilc1000_isr(&g_wdrvext_priv.eventWait);
}

uint32_t WDRV_EXT_DataSend(uint16_t segSize, uint8_t *p_segData)
{
    int8_t ret;

    if (gp_wdrv_cfg->networkType == WDRV_NETWORK_TYPE_SOFT_AP) 
    {
        ret = wilc1000_eth_data_send(p_segData, segSize, AP_INTERFACE);
    } else {
        ret = wilc1000_eth_data_send(p_segData, segSize, STATION_INTERFACE);
    }
	if (ret) {
		if (ret == M2M_ERR_MEM_ALLOC) {
			WDRV_DBG_TRACE_MESSAGE(("Memory is not available temporarily\r\n"));
			ret = WDRV_OUT_OF_MEMORY;
		} else {
			WDRV_DBG_ERROR_PRINT(("Failed to send data - %d\r\n", ret));
        	ret = WDRV_ERROR;
		}
	}
    return ret;
}

void WDRV_EXT_ModuleUpDown(uint32_t up)
{
    up ? wilc1000_init() : wilc1000_deinit();
}

void WDRV_EXT_ScanResultGet(uint8_t listIndex, WDRV_SCAN_RESULT *p_scanResult)
{
    int8_t ret;
    tstrM2mWifiscanResult result;

    ret = m2m_wifi_req_scan_result(listIndex);
    WDRV_ASSERT(ret == 0, "WDRV_EXT_ScanResultGet error");
    if (ret)
        return;

    WDRV_SEM_TAKE(&g_wdrvext_priv.scanResultWait, OSAL_WAIT_FOREVER);
    wilc1000_scan_result_read(&result);
    memcpy((void *)p_scanResult->ssid, (void *)result.au8SSID, sizeof(p_scanResult->ssid));
    p_scanResult->ssidLen = strlen((char *)p_scanResult->ssid);
    memcpy((void *)p_scanResult->bssid, (void *)result.au8BSSID, sizeof(p_scanResult->bssid));
    p_scanResult->rssi = result.s8rssi;
    p_scanResult->channel = result.u8ch;
    p_scanResult->apConfig = 0;
    if (result.u8AuthType != M2M_WIFI_SEC_OPEN)
        p_scanResult->apConfig |= WDRV_APCONFIG_BIT_PRIVACY;

    if (result.u8AuthType == M2M_WIFI_SEC_WPA_PSK) {
        p_scanResult->apConfig |= WDRV_APCONFIG_BIT_WPA;
        p_scanResult->apConfig |= WDRV_APCONFIG_BIT_WPA2;
    }
}

void WDRV_EXT_ScanDoneSet(void)
{
    WDRV_DBG_INFORM_MESSAGE(("Scan is completed successfully\r\n"));
    s_scanControl.scanListGetOnce = true;
    s_scanControl.scanInProgress = false;
}

bool WDRV_EXT_ScanIsInProgress(void)
{
    return s_scanControl.scanInProgress;
}

uint32_t WDRV_EXT_MulticastFilterSet(uint8_t *addr)
{
    int8_t ret;

    ret = m2m_wifi_enable_mac_mcast(addr, 1);

    WDRV_ASSERT(ret == 0, "WDRV_EXT_MulticastFilterSet error");

	return ret ? WDRV_ERROR : WDRV_SUCCESS;
}

void WDRV_EXT_Initialize(WDRV_HOOKS const *const ehooks, bool initWait)
{
	if (s_isInitComplete)
		return;

	g_debugConsoleLock = NULL;
	WDRV_MUTEX_CREATE(&g_debugConsoleLock);

	WDRV_DBG_INFORM_MESSAGE(("WILC1000: Initializing...\r\n"));

	WDRV_STUB_GPIO_Initialize();
	WDRV_STUB_SPI_Initialize();

	_WDRV_EXT_Initialize(ehooks);

	while (initWait && !s_isInitComplete)
		WDRV_TIME_DELAY(1);
}

int32_t WDRV_EXT_Initialize_bare(WDRV_HOOKS const *const hooks)
{
	int32_t ret = 0; 
	
	if (s_isInitComplete)
		return WDRV_ERROR;

	g_debugConsoleLock = NULL;
	WDRV_MUTEX_CREATE(&g_debugConsoleLock);

	WDRV_DBG_INFORM_MESSAGE(("WILC1000: Initializing...\r\n"));

	WDRV_STUB_GPIO_Initialize();
	WDRV_STUB_SPI_Initialize();

	ret = _WDRV_EXT_Initialize_bare(hooks);

	return ret;
}


void WDRV_EXT_Deinitialize(void)
{
	if (!s_isInitComplete)
		return;

	WDRV_DBG_INFORM_MESSAGE(("WILC1000: De-initializing...\r\n"));
	s_isInitComplete = false;

    _WDRV_EXT_Deinitialize();
    WDRV_STUB_GPIO_DeInitialize();
    WDRV_STUB_SPI_Deinitialize();

    WDRV_MUTEX_DELETE(&g_debugConsoleLock);
}

//DOM-IGNORE-END

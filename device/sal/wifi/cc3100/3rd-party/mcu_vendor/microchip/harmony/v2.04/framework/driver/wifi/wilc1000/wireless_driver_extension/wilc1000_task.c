/*******************************************************************************
  WILC1000 Wireless Driver Main Task

  File Name:
    wilc1000_task.c

  Summary:
    Entry point of WILC1000 core driver.

  Description:
    Entry point of WILC1000 core driver.
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
#include "driver/include/m2m_wifi.h"
#include "driver/source/nmasic.h"

#define PACKET_BUFFER_SIZE 1564

WILC1000_INTF *g_wilc1000_intf;
static bool s_isSoftAP;
static tstrM2MConnInfo s_connInfo;
static tstrM2mWifiscanResult s_scanResult;
static tstrM2MWPSInfo s_wpsInfo;
#ifndef ETH_RX_ZERO_COPY
static uint8_t s_ethRxBuf[PACKET_BUFFER_SIZE];
#endif

uint8_t g_initConn = 0;
extern void WDRV_Connect(void);
extern sint8 m2m_wifi_set_control_ifc(uint8 u8IfcId);

static bool s_isOtaUpdateRun = false;

sint8 wilc1000_init(void)
{
	/* Perform chip reset. */
	nm_reset();
    s_isSoftAP = false;

	return M2M_SUCCESS;
}

sint8 wilc1000_deinit(void)
{
    WDRV_STUB_GPIO_ChipDisable();
    WDRV_STUB_GPIO_ModuleReset();

	return M2M_SUCCESS;
}

void wilc1000_scan_result_read(tstrM2mWifiscanResult *result)
{
    memcpy((void *)result, (void *)&s_scanResult, sizeof(tstrM2mWifiscanResult));
}

void wilc1000_wps_info_read(tstrM2MWPSInfo *info)
{
    memcpy((void *)info, (void *)&s_wpsInfo, sizeof(tstrM2MWPSInfo));
}

void wilc1000_conn_info_read(tstrM2MConnInfo *info)
{
    memcpy((void *)info, (void *)&s_connInfo, sizeof(tstrM2MConnInfo));
}

int8_t wilc1000_connect(WILC1000_CONNECT_DATA *data)
{
    int8_t ret;
    tstrM2MAPConfig strM2MAPConfig;

    if (data->isSoftAP) {
        s_isSoftAP = true;
        memset(&strM2MAPConfig, 0, sizeof(tstrM2MAPConfig));
        strcpy((char *)&strM2MAPConfig.au8SSID, (char *)data->ssid);
        strM2MAPConfig.u8ListenChannel = data->channel;
        strM2MAPConfig.u8SecType = data->secType;
		if (strM2MAPConfig.u8SecType == M2M_WIFI_SEC_WEP) {
			strcpy((char *)&strM2MAPConfig.uniAuth.strWepInfo.au8WepKey, (char *)data->key);
			strM2MAPConfig.uniAuth.strWepInfo.u8KeySz = data->keyLen;
			strM2MAPConfig.uniAuth.strWepInfo.u8KeyIndx = data->keyIdx;
		} else if (strM2MAPConfig.u8SecType == M2M_WIFI_SEC_WPA_PSK) {
			strcpy((char *)&strM2MAPConfig.uniAuth.au8PSK, (char *)data->key); 
		}
        /* Bring up AP mode with parameters structure. */
        ret = m2m_wifi_enable_ap(&strM2MAPConfig);
		WDRV_ASSERT(ret == WDRV_SUCCESS, "m2m_wifi_enable_ap error");
        wilc1000_connect_event_received(true, true, NULL);
        WDRV_DBG_INFORM_PRINT(("Soft AP is enabled, SSID: %s\r\n", (char *)data->ssid));
    } else {
    	s_isSoftAP = false;
        if (data->secType == M2M_WIFI_SEC_WEP) {
            tuniM2MWifiAuth wep;

            wep.strWepInfo.u8KeyIndx = data->keyIdx;
            wep.strWepInfo.u8KeySz = strlen((char *)(data->key));
            memcpy((void *)wep.strWepInfo.au8WepKey, (void *)data->key, wep.strWepInfo.u8KeySz);
            wep.strWepInfo.au8WepKey[wep.strWepInfo.u8KeySz] = '\0';
            wep.strWepInfo.u8KeySz++;
//			m2m_wifi_set_control_ifc(1);
            ret = m2m_wifi_connect((char *)data->ssid, 
            		data->ssidLen, 
            		data->secType, 
            		&wep, 
            		data->channel);
        } else {
//			m2m_wifi_set_control_ifc(1);
            tuniM2MWifiAuth wep;

			memset(&wep, 0, sizeof(wep));
			memcpy((void *)wep.au8PSK, (void *)data->key, strlen((char *)(data->key)));
            ret = m2m_wifi_connect((char *)data->ssid, 
            		data->ssidLen, 
            		data->secType, 
            		&wep, 
            		data->channel);
        }
    }

    return ret;
}


void wilc1000_task_init2(WILC1000_INTF *intf)
{
	g_wilc1000_intf = intf;
	wilc1000_init();
	wilc1000_init_completed();
}

static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
	switch (u8MsgType) {
 	case M2M_WIFI_RESP_SCAN_DONE:
        wilc1000_scan_completed(0);
        break;
	case M2M_WIFI_RESP_SCAN_RESULT:
        memcpy((void *)&s_scanResult, (void *)pvMsg, sizeof(tstrM2mWifiscanResult));
        WDRV_SEM_GIVE(&g_wdrvext_priv.scanResultWait);
        break;
	case M2M_WIFI_RESP_CON_STATE_CHANGED:
	{
		tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;

		if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
			if (g_wilc1000_intf->ethMode) {
				if (s_isSoftAP) {
					int8_t ret;
					ret = m2m_wifi_get_connection_info();
					WDRV_ASSERT(ret == 0, "m2m_wifi_get_connection_info failed");
					WDRV_DBG_INFORM_MESSAGE(("Wi-Fi Connected\r\n"));
				} else {
					wilc1000_connect_event_received(true, false, NULL);
					WDRV_DBG_INFORM_MESSAGE(("Wi-Fi Connected\r\n"));
				}
			} else {
			/* Do nothing in socket mode */
			}
		} else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
			if (g_wilc1000_intf->ethMode) {
				wilc1000_connect_event_received(false, true, NULL);
				if (!s_isSoftAP) {
					WDRV_DBG_INFORM_MESSAGE(("Wi-Fi Disconnected\r\n"));
				}
			} else {
				if (s_isSoftAP) {
					wilc1000_connect_event_received(false, true, NULL);
					WDRV_DBG_INFORM_MESSAGE(("The client is disconnected\r\n"));
				} else {
					wilc1000_connect_event_received(false, false, NULL);
					WDRV_DBG_INFORM_MESSAGE(("Wi-Fi Disconencted\r\n"));
				}
			}
		}
		break;
	}
	case M2M_WIFI_RESP_CONN_INFO:
		memcpy((void *)&s_connInfo, (void *)pvMsg, sizeof(tstrM2MConnInfo));
		if (s_isSoftAP) {
			wilc1000_connect_event_received(true, false, NULL);
		} else {
			WDRV_SEM_GIVE(&g_wdrvext_priv.connInfoWait);
		}
		break;
    case M2M_WIFI_REQ_WPS:
        memcpy((void *)&s_wpsInfo, (void *)pvMsg, sizeof(tstrM2MWPSInfo));
        WDRV_DBG_INFORM_MESSAGE(("Credentials got off of WPS.\r\n"));
		if (s_wpsInfo.u8AuthType != M2M_WIFI_SEC_INVALID) {
        	WDRV_DBG_INFORM_PRINT(("ssid: %s\r\n", s_wpsInfo.au8SSID));
        	WDRV_DBG_INFORM_PRINT(("channel: %d\r\n", s_wpsInfo.u8Ch));
        	WDRV_DBG_INFORM_PRINT(("auth type: %d\r\n", s_wpsInfo.u8AuthType));
			if (s_wpsInfo.u8AuthType != M2M_WIFI_SEC_OPEN)
        		WDRV_DBG_INFORM_PRINT(("psk: %s\r\n", s_wpsInfo.au8PSK));
		}
        wilc1000_wps_completed();
        break;
	case M2M_WIFI_RESP_CURRENT_RSSI:
		WDRV_DBG_INFORM_PRINT(("RSSI for the current connected AP (%d) dBm\r\n", *(int8_t *)pvMsg));
		break;

	case M2M_WIFI_RESP_FIRMWARE_STRTED:
        if (gp_wdrv_cfg->networkType == WDRV_NETWORK_TYPE_SOFT_AP) 
        {
			WDRV_Connect();
		}
		break;

	default:
        WDRV_ASSERT(false, "Invalid Callback ID");
		break;
	}
}

static void eth_cb(uint8 u8MsgType, void * pvMsg,void * pvCtrlBuf)
{
#ifdef ETH_RX_ZERO_COPY
	int32_t wait = 0;
	TCPIP_MAC_PACKET *p_packet;
	tstrM2mIpPktBuf *frame = (tstrM2mIpPktBuf *)pvMsg;

	wilc1000_eth_data_received(frame->u16BufSz, frame->header);
	p_packet = wilc1000_get_rx_bufer();
	while (p_packet == NULL) {
		WDRV_TIME_DELAY(10);
		p_packet = wilc1000_get_rx_bufer();
		if (++wait > 500) {
			/* Something went wrong */
			WDRV_ASSERT(false, "No Rx Buffer Available");
		}
	}
	m2m_wifi_set_receive_buffer(p_packet->pDSeg->segLoad, PACKET_BUFFER_SIZE);
#else /* !ETH_RX_ZERO_COPY */
	tstrM2mIpCtrlBuf *ctrl = (tstrM2mIpCtrlBuf *)pvCtrlBuf;
	wilc1000_eth_data_received(ctrl->u16DataSize, ((uint8 *)pvMsg));
	m2m_wifi_set_receive_buffer(s_ethRxBuf, sizeof(s_ethRxBuf));
#endif /* ETH_RX_ZERO_COPY */
}


void wilc1000_task_init(WILC1000_INTF *intf)
{
    tstrWifiInitParam param;
	uint8_t mac0[6];
	uint8_t mac1[6];
    int8_t ret;

    g_wilc1000_intf = intf;

    wilc1000_init();	

    /* Initialize Wi-Fi parameters structure. */
    memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));
    /* Initialize Wi-Fi driver with data and status callback. */
    param.pfAppWifiCb = wifi_cb;
	param.strEthInitParam.pfAppEthCb = eth_cb;
	param.strEthInitParam.au8ethRcvBuf = s_ethRxBuf;
	param.strEthInitParam.u16ethRcvBufSize = sizeof(s_ethRxBuf);

    ret = m2m_wifi_init(&param);

    if (M2M_SUCCESS != ret) {
        WDRV_ASSERT(false, "m2m_wifi_init call error");
    }

	if (g_wilc1000_intf->fwOtaServerUrl)
		s_isOtaUpdateRun = true;

    if (g_wilc1000_intf->ethMode) {
#ifdef ETH_RX_ZERO_COPY
        TCPIP_MAC_PACKET *p_packet = wilc1000_get_rx_bufer();
        if (p_packet == NULL) {
            WDRV_ASSERT(false, "No Rx Buffer Available");
            return;
        }
        m2m_wifi_set_receive_buffer(p_packet->pDSeg->segLoad, PACKET_BUFFER_SIZE);
#else /* !ETH_RX_ZERO_COPY */
		m2m_wifi_set_receive_buffer(s_ethRxBuf, sizeof(s_ethRxBuf));
#endif /* ETH_RX_ZERO_COPY */
    } 

    m2m_wifi_get_mac_address(mac0, mac1);
    wilc1000_rf_ready(mac1);
    wilc1000_init_completed();
}

void wilc1000_task_deinit(void)
{
    m2m_wifi_deinit(NULL);
    wilc1000_deinit();
}

/**
 * \brief wilc1000 task.
 *
 * wilc1000 driver entry point.
 */
 void wilc1000_task(void *arg)
{
	WILC1000_INTF *intf = (WILC1000_INTF *)arg;

	if (intf->ethMode || intf->fwOtaServerUrl)
		wilc1000_task_init(intf);

	else
		wilc1000_task_init2(intf);

	while (true) {
		int8_t ret;
		ret = m2m_wifi_handle_events(&g_wdrvext_priv.eventWait);
		if (ret != M2M_SUCCESS) {
			WDRV_ASSERT(false, "Failed to handle m2m events");
			break;
		}
		if (g_wdrvext_priv.deinit_in_progress)
			break;
		
	}

	wilc1000_task_deinit();

	g_wdrvext_priv.deinit_in_progress = false;
	while (true) /* wait for task delete */
		WDRV_TIME_DELAY(1000);
}


void wilc1000_task_bare(void *arg)
{
	WILC1000_INTF *intf = (WILC1000_INTF *)arg;

	if (intf->ethMode || intf->fwOtaServerUrl)
		wilc1000_task_init(intf);

	else
		wilc1000_task_init2(intf);

	while (true) {
		int8_t ret;
		ret = m2m_wifi_handle_events(&g_wdrvext_priv.eventWait);
		if (ret != M2M_SUCCESS) {
			WDRV_ASSERT(false, "Failed to handle m2m events");
			break;
		}
		if (g_wdrvext_priv.deinit_in_progress)
			break;
		
		WDRV_TIME_DELAY(10);
	}

	wilc1000_task_deinit();

	g_wdrvext_priv.deinit_in_progress = false;
	while (true) /* wait for task delete */
		WDRV_TIME_DELAY(1000);
}

/**
 *
 * \file
 *
 * \brief This module contains M2M Wi-Fi APIs implementation.
 *
 * Copyright (c) 2016-2018 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "driver/include/m2m_wifi.h"
#include "driver/source/nmdrv.h"
#include "driver/source/m2m_hif.h"
#include "driver/source/nmasic.h"
#ifdef COMPUTE_PMK_IN_HOST
#include "drv_hash/crypto.h" 
#endif

//uint8_t rx_buf[2000];

static volatile uint8 gu8ChNum;
static volatile uint8 gu8scanInProgress = 0;
tpfAppWifiCb gpfAppWifiCb = NULL;

static tpfAppEthCb  gpfAppEthCb  = NULL;
static uint8* 	        gpu8ethRcvBuf=NULL;
static uint16 	        gu16ethRcvBufSize ;

tuCtrlStruct guCtrlStruct __M2M_DMA_BUF_ATT__;

#ifdef CONF_MGMT
static tpfAppMonCb  gpfAppMonCb  = NULL;
#endif
/**
*	@fn			m2m_wifi_cb(uint8 u8OpCode, uint16 u16DataSize, uint32 u32Addr, uint8 grp)
*	@brief		WiFi call back function
*	@param [in]	u8OpCode
*					HIF Opcode type.
*	@param [in]	u16DataSize
*					HIF data length.
*	@param [in]	pu8Buffer
*					HIF data buffer.
*	@param [in]	grp
*					HIF group type.
*	@author
*	@date
*	@version	1.0
*/
static void m2m_wifi_cb(uint8 u8OpCode, uint16 u16DataSize, uint8* pu8Buffer)
{
	
	if (u8OpCode == M2M_WIFI_RESP_CON_STATE_CHANGED)
	{
		tstrM2mWifiStateChanged* pstrState = (tstrM2mWifiStateChanged*)pu8Buffer;
		if (gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RESP_CON_STATE_CHANGED, pstrState);
	}
	else if(u8OpCode == M2M_WIFI_RESP_CONN_INFO)
	{
		tstrM2MConnInfo*		pstrConnInfo = (tstrM2MConnInfo*)pu8Buffer;
		if(gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RESP_CONN_INFO, pstrConnInfo);
	}
	else if(u8OpCode == M2M_WIFI_RESP_AP_ASSOC_INFO)
	{
		tstrM2MAPAssocInfo*	pstrAssocInfo = (tstrM2MAPAssocInfo*)pu8Buffer;
		if(gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RESP_AP_ASSOC_INFO, pstrAssocInfo);
	}
	else if (u8OpCode == M2M_WIFI_RES_WPS)
	{
		tstrM2MWPSInfo* pstrWps = (tstrM2MWPSInfo*)pu8Buffer;
		if (gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RES_WPS, pstrWps);
	}
	else if (u8OpCode == M2M_WIFI_RESP_IP_CONFLICT)
	{
		tstrM2MIPConfig* pstrIpConfig = (tstrM2MIPConfig*)pu8Buffer;
		M2M_DBG("Conflicted IP\"%u.%u.%u.%u\"\n", 
			((uint8 *)&(pstrIpConfig->u32StaticIP))[0], ((uint8 *)&(pstrIpConfig->u32StaticIP))[1],
			((uint8 *)&(pstrIpConfig->u32StaticIP))[2], ((uint8 *)&(pstrIpConfig->u32StaticIP))[3]);
		if (gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RESP_IP_CONFLICT, NULL);
	}
	else if (u8OpCode == M2M_WIFI_RESP_SCAN_DONE)
	{
		tstrM2mScanDone* pstrState = (tstrM2mScanDone*)pu8Buffer;
		gu8scanInProgress = 0;
		gu8ChNum = pstrState->u8NumofCh;
		if (gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RESP_SCAN_DONE, pstrState);
	}
	else if (u8OpCode == M2M_WIFI_RESP_SCAN_RESULT)
	{
		tstrM2mWifiscanResult* pstrScanResult = (tstrM2mWifiscanResult*)pu8Buffer;
		if (gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RESP_SCAN_RESULT, pstrScanResult);
	}
	else if (u8OpCode == M2M_WIFI_RESP_CURRENT_RSSI)
	{
		if (gpfAppWifiCb)
			gpfAppWifiCb(M2M_WIFI_RESP_CURRENT_RSSI, pu8Buffer);
	}
	else if(u8OpCode == M2M_WIFI_RESP_ETHERNET_RX_PACKET)
	{
		tstrM2mIpRsvdPkt * pstrM2MIpRxPkt = (tstrM2mIpRsvdPkt*)pu8Buffer;
		tstrM2MDataBufCtrl  strM2mIpCtrlBuf;
		
		strM2mIpCtrlBuf.u8IfcId = pstrM2MIpRxPkt->u8IfcId;
		strM2mIpCtrlBuf.u16RemainigDataSize = 0;
		if((gpfAppEthCb) &&(gpu8ethRcvBuf)&& (gu16ethRcvBufSize > 0))
		{
					strM2mIpCtrlBuf.u16DataSize = pstrM2MIpRxPkt->u16PktSz;
					strM2mIpCtrlBuf.u8DataOffset = pstrM2MIpRxPkt->u16PktOffset + M2M_HIF_HDR_OFFSET;
					gpfAppEthCb(M2M_WIFI_RESP_ETHERNET_RX_PACKET, pu8Buffer + strM2mIpCtrlBuf.u8DataOffset - M2M_ETH_PAD_SIZE, &(strM2mIpCtrlBuf));
					

		}
	}
	else if(u8OpCode == M2M_WIFI_RESP_PACKET_SENT)
	{
		gpfAppEthCb(M2M_WIFI_RESP_PACKET_SENT, NULL, 0);
	}

#ifdef CONF_MGMT
	else if(u8OpCode == M2M_WIFI_RESP_WIFI_RX_PACKET)
	{
		tstrM2MWifiRxPacketInfo*		pstrRxPacketInfo = (tstrM2MWifiRxPacketInfo*)pu8Buffer;

		u16DataSize -= sizeof(tstrM2MWifiRxPacketInfo);
		pu8Buffer+= sizeof(tstrM2MWifiRxPacketInfo);
		if(gpfAppMonCb)
			gpfAppMonCb(pstrRxPacketInfo, pu8Buffer,u16DataSize);
	}
#endif
	else if(u8OpCode == M2M_WIFI_RESP_FIRMWARE_STRTED)
	{		
		if(gpfAppWifiCb)
				gpfAppWifiCb(M2M_WIFI_RESP_FIRMWARE_STRTED, NULL);
	}
	else
	{
		M2M_ERR("REQ Not defined %d\n",u8OpCode);
	}
}


sint8 m2m_wifi_init(tstrWifiInitParam * param)
{
	sint8 ret = M2M_SUCCESS;
	tstrHifinitParam strHifInitParam;

	gpfAppWifiCb 		= param->pfAppWifiCb;
 	gpfAppEthCb  	    = param->strEthInitParam.pfAppEthCb;
	gpu8ethRcvBuf       = param->strEthInitParam.au8ethRcvBuf;
	gu16ethRcvBufSize	= param->strEthInitParam.u16ethRcvBufSize;

#ifdef CONF_MGMT
	gpfAppMonCb  = param->pfAppMonCb;
#endif

	gu8scanInProgress = 0;
	/* Apply device specific initialization. */
	ret = nm_drv_init(NULL);

	if(ret != M2M_SUCCESS) 	goto _EXIT0;
	/* Initialize host interface module */
	strHifInitParam.pu8RcvBuff = gpu8ethRcvBuf;
	strHifInitParam.u32RcvBuffSize = gu16ethRcvBufSize;
	
	ret = hif_init(&strHifInitParam);
	if(ret != M2M_SUCCESS) 	goto _EXIT1;

	hif_register_cb(M2M_REQ_GRP_WIFI,m2m_wifi_cb);

	return ret;

_EXIT1:
	nm_drv_deinit(NULL);
_EXIT0:

	return ret;
}

sint8  m2m_wifi_deinit(void * arg)
{

	hif_deinit(NULL);

	nm_drv_deinit(NULL);

	return M2M_SUCCESS;
}


sint8 m2m_wifi_handle_events(void * arg)
{
	return hif_handle_isr();
}

sint8 m2m_wifi_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, tuniM2MWifiAuth *puniAuthInfo, uint16 u16Ch)
{
	sint8				ret = M2M_SUCCESS;
	tstrM2mWifiConnect*	pstrConnect = &guCtrlStruct.strM2mWifiConnect;
	tstrM2MWifiSecInfo	*pstrAuthInfo;

	if(u8SecType != M2M_WIFI_SEC_OPEN)
	{
		if((puniAuthInfo == NULL))
		{
			M2M_ERR("Auth parameters INVALID\n");
			ret = M2M_ERR_FAIL;
			goto ERR1;
		}
	}
	if((u8SsidLen<=0)||(u8SsidLen>=M2M_MAX_SSID_LEN))
	{
		M2M_ERR("SSID LEN INVALID\n");
		ret = M2M_ERR_FAIL;
		goto ERR1;
	}

	if(u16Ch>M2M_WIFI_CH_14)
	{
		if(u16Ch!=M2M_WIFI_CH_ALL)
		{
			M2M_ERR("CH INVALID\n");
			ret = M2M_ERR_FAIL;
			goto ERR1;
		}
	}


	m2m_memcpy(pstrConnect->au8SSID, (uint8*)pcSsid, u8SsidLen);
	pstrConnect->au8SSID[u8SsidLen]	= 0;
	pstrConnect->u16Ch				= NM_BSP_B_L_16(u16Ch);

	pstrAuthInfo = &pstrConnect->strSec;
	pstrAuthInfo->u8SecType		= u8SecType;

	if(u8SecType == M2M_WIFI_SEC_WEP)
	{
		tstrM2mWifiWepParams	* pstrWepParams = &(puniAuthInfo->strWepInfo);
		tstrM2mWifiWepParams	*pstrWep = &pstrAuthInfo->uniAuth.strWepInfo;
		pstrWep->u8KeyIndx =pstrWepParams->u8KeyIndx-1;

		if(pstrWep->u8KeyIndx >= WEP_KEY_MAX_INDEX)
		{
			M2M_ERR("Invalid Wep key index %d\n", pstrWep->u8KeyIndx);
			ret = M2M_ERR_FAIL;
			goto ERR1;
		}
		pstrWep->u8KeySz = pstrWepParams->u8KeySz-1;
		if ((pstrWep->u8KeySz != WEP_40_KEY_STRING_SIZE)&& (pstrWep->u8KeySz != WEP_104_KEY_STRING_SIZE))
		{
			M2M_ERR("Invalid Wep key length %d\n", pstrWep->u8KeySz);
			ret = M2M_ERR_FAIL;
			goto ERR1;
		}
		m2m_memcpy((uint8*)pstrWep->au8WepKey,(uint8*)pstrWepParams->au8WepKey, pstrWepParams->u8KeySz);
		pstrWep->au8WepKey[pstrWepParams->u8KeySz] = 0;
		pstrWep->u8WepAuthType = pstrWepParams->u8WepAuthType;

	}
	else if(u8SecType == M2M_WIFI_SEC_WPA_PSK)
	{
		uint8	u8KeyLen;
		if((m2m_strlen(puniAuthInfo->au8PSK)<=0)||(m2m_strlen(puniAuthInfo->au8PSK)>=M2M_MAX_PSK_LEN))
		{
			M2M_ERR("PSK LEN INVALID\n");
			ret = M2M_ERR_FAIL;
			goto ERR1;
		}
		
		u8KeyLen = m2m_strlen((uint8*)puniAuthInfo->au8PSK) + 1;
		
#ifdef COMPUTE_PMK_IN_HOST
		pstrAuthInfo->u8IsPMKUsed = 1;
		pbkdf2_sha1((uint8*)puniAuthInfo->au8PSK,u8KeyLen-1,pstrConnect->au8SSID,m2m_strlen(pstrConnect->au8SSID),
			pstrAuthInfo->uniAuth.au8PMK);		
#else  
		pstrAuthInfo->u8IsPMKUsed = 0;
		m2m_memcpy(pstrAuthInfo->uniAuth.au8PSK, (uint8*)puniAuthInfo->au8PSK, u8KeyLen);
#endif
	}
	else if(u8SecType == M2M_WIFI_SEC_802_1X)
	{
		m2m_memcpy((uint8*)&pstrAuthInfo->uniAuth.strCred1x, (uint8*)(&puniAuthInfo->strCred1x), sizeof(tstr1xAuthCredentials));
	}
	else if(u8SecType == M2M_WIFI_SEC_OPEN)
	{

	}
	else
	{
		M2M_ERR("undefined sec type\n");
		ret = M2M_ERR_FAIL;
		goto ERR1;
	}

	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_CONNECT, (uint8*)pstrConnect, sizeof(tstrM2mWifiConnect),NULL, 0,0);

ERR1:
	return ret;
}

sint8 m2m_wifi_disconnect(void)
{
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_DISCONNECT, NULL, 0, NULL, 0,0);
}

/*!
@fn			NMI_API sint8 m2m_wifi_set_tx_power(uint8 u8TxPwrLevel);
@brief		set the TX power tenuM2mTxPwrLevel
@param [in]	u8TxPwrLevel
			change the TX power tenuM2mTxPwrLevel
@return		The function SHALL return M2M_SUCCESE for success and a negative value otherwise.
@sa			tenuM2mTxPwrLevel
@pre		m2m_wifi_init
@warning	
*/
sint8 m2m_wifi_set_tx_power(uint8 u8TxPwrLevel)
{
	sint8 ret = M2M_SUCCESS;
	tstrM2mTxPwrLevel* pstrM2mTxPwrLevel = &guCtrlStruct.strM2mTxPwrLevel;
	pstrM2mTxPwrLevel->u8TxPwrLevel = u8TxPwrLevel;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_TX_POWER, (uint8*)pstrM2mTxPwrLevel,sizeof(tstrM2mTxPwrLevel), NULL, 0, 0);
	return ret;
}

sint8 m2m_wifi_set_mac_address(uint8 au8MacAddress0[6],uint8 au8MacAddress1[6])
{
	tstrM2mSetMacAddress* pstrTmp = &guCtrlStruct.strM2mSetMacAddress;
	m2m_memcpy((uint8*) pstrTmp->au8Mac0 , (uint8*) au8MacAddress0, 6);
	m2m_memcpy((uint8*) pstrTmp->au8Mac1, (uint8*) au8MacAddress1, 6);
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_MAC_ADDRESS,
		(uint8*) pstrTmp, sizeof(tstrM2mSetMacAddress), NULL, 0,0);
}

/*!
@fn			NMI_API sint8 m2m_wifi_set_lsn_int(tstrM2mLsnInt * pstrM2mLsnInt);
@brief		Set the Wi-Fi listen interval for power save operation. It is represented in units
			of AP Beacon periods.
@param [in]	pstrM2mLsnInt
			Structure holding the listen interval configurations.
@return		The function SHALL return 0 for success and a negative value otherwise.
@sa			tstrM2mLsnInt , m2m_wifi_set_sleep_mode
@pre		m2m_wifi_set_sleep_mode shall be called first
@warning	The Function called once after initialization. 
*/
sint8 m2m_wifi_set_lsn_int(tstrM2mLsnInt* pstrM2mLsnInt)
{
	tstrM2mLsnInt* pstrTmpM2mLsnInt = &guCtrlStruct.strM2mLsnInt;
	m2m_memcpy((uint8*)pstrTmpM2mLsnInt, (uint8*)pstrM2mLsnInt, sizeof(tstrM2mLsnInt));
	pstrTmpM2mLsnInt->u16LsnInt = NM_BSP_B_L_16(pstrTmpM2mLsnInt->u16LsnInt);
	
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_LSN_INT, 
		(uint8*)pstrTmpM2mLsnInt, sizeof(tstrM2mLsnInt), NULL, 0, 0);
}

sint8 m2m_wifi_set_cust_InfoElement(uint8* pau8M2mCustInfoElement)
{
	sint8	s8Ret = M2M_ERR_FAIL;
	
	if((uint8)(pau8M2mCustInfoElement[0]+1) > sizeof(tuCtrlStruct))
	{
		M2M_ERR("Custom Info Element size %d bigger than max ctrl buffer size %d\r\n", 
			pau8M2mCustInfoElement[0]+1, sizeof(tuCtrlStruct));
	}
	else
	{
		m2m_memcpy((uint8*)&guCtrlStruct, pau8M2mCustInfoElement, 
		pau8M2mCustInfoElement[0]+1);
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_CUST_INFO_ELEMENT,
			(uint8*)&guCtrlStruct, pau8M2mCustInfoElement[0]+1, NULL, 0, 0);
	}
	return s8Ret;
}

sint8 m2m_wifi_set_scan_options(uint8 u8NumOfSlot,uint8 u8SlotTime)
{
	sint8	s8Ret = M2M_ERR_FAIL;
	tstrM2MScanOption* pstrM2MScan = &guCtrlStruct.strM2MScanOption;
	pstrM2MScan->u8NumOfSlot = u8NumOfSlot;
	pstrM2MScan->u8SlotTime = u8SlotTime;

	if(pstrM2MScan->u8NumOfSlot < M2M_SCAN_MIN_NUM_SLOTS)
	{
		pstrM2MScan->u8NumOfSlot = M2M_SCAN_MIN_NUM_SLOTS;
	}
	if(pstrM2MScan->u8SlotTime < M2M_SCAN_MIN_SLOT_TIME)
	{
		pstrM2MScan->u8SlotTime = M2M_SCAN_MIN_SLOT_TIME;
	}
	s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_SCAN_OPTION, (uint8*)pstrM2MScan, sizeof(tstrM2MScan),NULL, 0,0);
	if(s8Ret != M2M_SUCCESS)
	{
		M2M_ERR("SCAN Failed Ret = %d\n",s8Ret);
	}
	return s8Ret;
}
sint8 m2m_wifi_set_scan_region(uint8  ScanRegion)
{
	sint8	s8Ret = M2M_ERR_FAIL;
	tstrM2MScanRegion* pstrScanRegion = &guCtrlStruct.strM2MScanRegion;
	pstrScanRegion->u8ScanRegion = ScanRegion;
	s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_SCAN_REGION, (uint8*)pstrScanRegion, sizeof(tstrM2MScanRegion),NULL, 0,0);
	return s8Ret;
}

sint8 m2m_wifi_set_scan_list(tstrM2MScanList* pstrScanList)
{
	sint8	s8Ret = M2M_ERR_FAIL;
	tstrM2MScanList* pstrTmpScanList = &guCtrlStruct.strM2MScanList;
	m2m_memcpy((uint8*)pstrTmpScanList, (uint8*)pstrScanList, sizeof(tstrM2MScanList));
	s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_SCAN_LIST, (uint8*)pstrTmpScanList, sizeof(tstrM2MScanList),NULL, 0,0);
	if(s8Ret != M2M_SUCCESS)
	{
		M2M_ERR("SCAN List Ret = %d\n",s8Ret);
	}
	return s8Ret;
}

sint8 m2m_wifi_request_scan(uint8 ch)
{
	sint8	s8Ret = M2M_ERR_SCAN_IN_PROGRESS;

	if(!gu8scanInProgress)
	{
		tstrM2MScan* pstrtmp = &guCtrlStruct.strM2MScan;
		m2m_memset((uint8*)pstrtmp,0,sizeof(tstrM2MScan));
		pstrtmp->u8ChNum = ch;
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SCAN, (uint8*)pstrtmp, sizeof(tstrM2MScan),NULL, 0,0);

		if(s8Ret == M2M_SUCCESS)
		{
			gu8scanInProgress = 1;
		}
		else
		{
			M2M_ERR("SCAN Failed Ret = %d\n",s8Ret);
		}
	}
	else
	{
		M2M_ERR("SCAN In Progress\n");
	}
	return s8Ret;
}

sint8 m2m_wifi_request_scan_ssid(uint8 ch,char* pcssid)
{
	sint8	s8Ret = M2M_ERR_SCAN_IN_PROGRESS;

	if(!gu8scanInProgress)
	{
		tstrM2MScan* pstrtmp = &guCtrlStruct.strM2MScan;
		m2m_memset((uint8*)pstrtmp,0,sizeof(tstrM2MScan));
		pstrtmp->u8ChNum = ch;
		if(pcssid)
		{
			uint8 len =	m2m_strlen((uint8 *)pcssid);
			m2m_memcpy(pstrtmp->au8SSID,(uint8 *)pcssid,len+1);
		}
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SCAN, (uint8*)pstrtmp, sizeof(tstrM2MScan),NULL, 0,0);

		if(s8Ret == M2M_SUCCESS)
		{
			gu8scanInProgress = 1;
		}
		else
		{
			M2M_ERR("SCAN Failed Ret = %d\n",s8Ret);
		}
	}
	else
	{
		M2M_ERR("SCAN In Progress\n");
	}
	return s8Ret;
}

sint8 m2m_wifi_wps(uint8 u8TriggerType,const char  *pcPinNumber)
{
	tstrM2MWPSConnect* pstrtmp = &guCtrlStruct.strM2MWPSConnect;

	/* Stop Scan if it is ongoing.
	*/
	gu8scanInProgress = 0;
	pstrtmp->u8TriggerType = u8TriggerType;
	/*If WPS is using PIN METHOD*/
	if (u8TriggerType == WPS_PIN_TRIGGER)
		m2m_memcpy ((uint8*)pstrtmp->acPinNumber,(uint8*) pcPinNumber,8);
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_START_WPS, (uint8*)pstrtmp,sizeof(tstrM2MWPSConnect), NULL, 0,0);
}
sint8 m2m_wifi_wps_disable(void)
{
	sint8 ret = M2M_SUCCESS;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_DISABLE_WPS, NULL,0, NULL, 0, 0);
	return ret;
}

sint8 m2m_wifi_p2p(uint8 u8Channel)
{
	sint8 ret = M2M_SUCCESS;
	if((u8Channel == M2M_WIFI_CH_1) || (u8Channel == M2M_WIFI_CH_6) || (u8Channel == M2M_WIFI_CH_11))
	{
		tstrM2MP2PConnect* pstrtmp = &guCtrlStruct.strM2MP2PConnect;
		pstrtmp->u8ListenChannel = u8Channel;
		ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_ENABLE_P2P, (uint8*)pstrtmp, sizeof(tstrM2MP2PConnect), NULL, 0,0);
	}
	else
	{
		M2M_ERR("Listen channel should only be 1, 6 or 11\n");
		ret = M2M_ERR_FAIL;
	}

	return ret;
}
sint8 m2m_wifi_p2p_disconnect(void)
{
	sint8 ret = M2M_SUCCESS;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_DISABLE_P2P, NULL, 0, NULL, 0, 0);
	return ret;
}

sint8 m2m_wifi_enable_ap(CONST tstrM2MAPConfig* pstrM2MAPConfig)
{
	sint8 ret = M2M_SUCCESS;
	tstrM2MAPConfig* pstrTempM2MAPConfig = &guCtrlStruct.strM2MAPConfig;
	
	m2m_memcpy((uint8 *)pstrTempM2MAPConfig, (uint8 *)pstrM2MAPConfig, sizeof(tstrM2MAPConfig));
	
#ifdef COMPUTE_PMK_IN_HOST
	if(pstrM2MAPConfig->u8SecType == M2M_WIFI_SEC_WPA_PSK)
	{		
		pstrTempM2MAPConfig->u8IsPMKUsed = 1;
		pbkdf2_sha1((uint8 *)pstrM2MAPConfig->uniAuth.au8PSK,m2m_strlen((uint8 *)pstrM2MAPConfig->uniAuth.au8PSK),
			(uint8 *)pstrM2MAPConfig->au8SSID,m2m_strlen((uint8 *)pstrM2MAPConfig->au8SSID),pstrTempM2MAPConfig->uniAuth.au8PMK);
	}
#else
	if(pstrM2MAPConfig->u8SecType == M2M_WIFI_SEC_WPA_PSK)
	{
		pstrTempM2MAPConfig->u8IsPMKUsed = 0;
	}
#endif
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_ENABLE_AP, (uint8 *)pstrTempM2MAPConfig, sizeof(tstrM2MAPConfig), NULL, 0, 0);	
	
	return ret;
}
sint8 m2m_wifi_disable_ap(void)
{
	sint8 ret = M2M_SUCCESS;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_DISABLE_AP, NULL, 0, NULL, 0, 0);
	return ret;
}
sint8 m2m_wifi_ap_get_assoc_info(void)
{
	sint8 ret = M2M_SUCCESS;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_AP_ASSOC_INFO, NULL, 0, NULL, 0, 0);
	return ret;
}
sint8 m2m_wifi_ap_add_black_list(uint8 bAddNewEntry,uint8* mac_addr)
{
	sint8 ret = M2M_SUCCESS;
	uint8* pu8BlackListRequest = guCtrlStruct.au8BlackListRequest;

	pu8BlackListRequest[0] = bAddNewEntry;
	m2m_memcpy(&pu8BlackListRequest[1],mac_addr,6);
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_AP_BLACK_LIST,pu8BlackListRequest, 7, NULL, 0, 0);
	return ret;
}


/*!
@fn          NMI_API sint8 m2m_wifi_req_curr_rssi(void);
@brief       Request the current RSSI for the current connected AP, 
			 the response received in wifi_cb M2M_WIFI_RESP_CURRENT_RSSI	
@sa          M2M_WIFI_RESP_CURRENT_RSSI              
@return      The function shall return M2M_SUCCESS for success and a negative value otherwise.
*/
sint8 m2m_wifi_req_curr_rssi(void)
{
	sint8 ret = M2M_SUCCESS;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_CURRENT_RSSI, NULL, 0, NULL,0, 0);
	return ret;
}
/* WILCBARESW-167 - included u8IfcId in param list to select the interface for sending the Ethernet packet.	
					The param u8IfcId can take values(AP_INTERFACE, STATION_INTERFACE OR P2P_INTERFACE)
*/
sint8 m2m_wifi_send_ethernet_pkt(uint8* pu8Packet, uint16 u16PacketSize, uint8	u8IfcId) 
{
	sint8 s8Ret = -1;
	uint16 u16TotalPacketSize;
	if ((pu8Packet != NULL) && (u16PacketSize > 0)) 
	{
		tstrM2MWifiTxPacketInfo* strTxPkt =
				(tstrM2MWifiTxPacketInfo*) (pu8Packet + M2M_HIF_HDR_OFFSET);

		strTxPkt->u16PacketSize = NM_BSP_B_L_16(u16PacketSize);
		strTxPkt->u16HeaderLength = NM_BSP_B_L_16(M2M_ETHERNET_HDR_LEN);
		strTxPkt->u8IfcId = u8IfcId;
		u16TotalPacketSize = M2M_ETHERNET_HDR_OFFSET + M2M_ETH_PAD_SIZE + u16PacketSize;
		s8Ret = hif_send_optimized(M2M_REQ_GRP_WIFI,
				M2M_WIFI_REQ_SEND_ETHERNET_PACKET | M2M_REQ_DATA_PKT, pu8Packet, u16TotalPacketSize);
	}
	return s8Ret;
}
/*!
@fn          NMI_API sint8 m2m_wifi_get_otp_mac_address(uint8 *pu8MacAddr, uint8 * pu8IsValid);
@brief       Request the MAC address stored on the OTP (one time programmable) memory of the device.
			 (the function is Blocking until response received)	
@param [out] pu8MacAddr
			 Output MAC address buffer of 6 bytes size. Valid only if *pu8Valid=1.
@param [out] pu8IsValid
		     A output boolean value to indicate the validity of pu8MacAddr in OTP. 
		     Output zero if the OTP memory is not programmed, non-zero otherwise.	
@return      The function shall return M2M_SUCCESS for success and a negative value otherwise.
@sa          m2m_wifi_get_mac_address             
@pre         m2m_wifi_init required to call any WIFI/socket function
*/
sint8 m2m_wifi_get_otp_mac_address(uint8 *pu8MacAddr, uint8* pu8IsValid)
{
	sint8 ret = M2M_SUCCESS;
	uint8* pu8MacAddrBuf = guCtrlStruct.au8MacAddress;
	ret = hif_chip_wake();
	if(ret == M2M_SUCCESS)
	{
		ret = nmi_get_otp_mac_address(pu8MacAddrBuf, pu8IsValid);
		if(ret == M2M_SUCCESS)
		{
			ret = hif_chip_sleep();
			m2m_memcpy(pu8MacAddr, pu8MacAddrBuf, 6);
		}
	}
	return ret;
}
/*!
@fn          NMI_API sint8 m2m_wifi_get_mac_address(uint8 *pu8MacAddr0, uint8 *pu8MacAddr1)
@brief       Request the current MAC addresses of the device (the working mac addresses).
			 (the function is Blocking until response received)	
@param [out] pu8MacAddr0
			Output MAC address buffer of 6 bytes size for AP interface.
			pu8MacAddr1
			Output MAC address buffer of 6 bytes size for STA interface.	
@return      The function shall return M2M_SUCCESS for success and a negative value otherwise.
@sa          m2m_wifi_get_otp_mac_address             
@pre         m2m_wifi_init required to call any WIFI/socket function
*/
sint8 m2m_wifi_get_mac_address(uint8 *pu8MacAddr0, uint8 *pu8MacAddr1)
{
	sint8 ret = M2M_SUCCESS;
	uint8* pu8MacAddrBuf = guCtrlStruct.au8MacAddressDpl;
	ret = hif_chip_wake();
	if(ret == M2M_SUCCESS)
	{
		ret = nmi_get_mac_address(pu8MacAddrBuf);
		if(ret == M2M_SUCCESS)
		{
			ret = hif_chip_sleep();
			m2m_memcpy(pu8MacAddr0, pu8MacAddrBuf, 6);
			m2m_memcpy(pu8MacAddr1, pu8MacAddrBuf+6, 6);
		}
	}

	return ret;
}
/*!
@fn          NMI_API sint8 m2m_wifi_req_scan_result(uint8 index);
@brief       Reads the AP information from the Scan Result list with the given index, 
			 the response received in wifi_cb M2M_WIFI_RESP_SCAN_RESULT, 
			 the response pointer should be casted with tstrM2mWifiscanResult structure 	
@param [in]  index 
			 Index for the requested result, the index range start from 0 till number of AP's found 
@sa          tstrM2mWifiscanResult,m2m_wifi_get_num_ap_found,m2m_wifi_request_scan             
@return      The function shall return M2M_SUCCESE for success and a negative value otherwise
@pre         m2m_wifi_request_scan need to be called first, then m2m_wifi_get_num_ap_found 
			 to get the number of AP's found
@warning     Function used only in STA mode only. the scan result updated only if scan request called,
			 else it will be cashed in firmware for the host scan request result, 
			 which mean if large delay occur between the scan request and the scan result request, 
			 the result will not be up-to-date
*/

sint8 m2m_wifi_req_scan_result(uint8 index)
{
	sint8 ret = M2M_SUCCESS;
	tstrM2mReqScanResult* pstrReqScanRlt = &guCtrlStruct.strM2mReqScanResult;
	pstrReqScanRlt->u8Index = index;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SCAN_RESULT, (uint8*)pstrReqScanRlt, sizeof(tstrM2mReqScanResult), NULL, 0, 0);
	return ret;
}
/*!
@fn          NMI_API uint8 m2m_wifi_get_num_ap_found(void);
@brief       Reads the number of AP's found in the last Scan Request, 
			 The function read the number of AP's from global variable which updated in the 
			 wifi_cb in M2M_WIFI_RESP_SCAN_DONE.			 
@sa          m2m_wifi_request_scan               
@return      Return the number of AP's found in the last Scan Request.
@pre         m2m_wifi_request_scan need to be called first 
@warning     That function need to be called in the wifi_cb in M2M_WIFI_RESP_SCAN_DONE, 
			 calling that function in any other place will return undefined/undated numbers.
			 Function used only in STA mode only.
*/
uint8 m2m_wifi_get_num_ap_found(void)
{
	return gu8ChNum;
}
/*!
@fn		    NMI_API uint8 m2m_wifi_get_sleep_mode(void);
@brief	    Get the current Power save mode.
@return	    The current operating power saving mode.
@sa		    tenuPowerSaveModes , m2m_wifi_set_sleep_mode
*/
uint8 m2m_wifi_get_sleep_mode(void)
{
	return hif_get_sleep_mode();
}
/*!
@fn			NMI_API sint8 m2m_wifi_set_sleep_mode(uint8 PsTyp, uint8 BcastEn);
@brief      Set the power saving mode for the WILC1000. 
@param [in]	PsTyp
			Desired power saving mode. Supported types are defined in tenuPowerSaveModes.
@param [in]	BcastEn
			Broadcast reception enable flag. 
			If it is 1, the WILC1000 must be awake each DTIM Beacon for receiving Broadcast traffic.
			If it is 0, the WILC1000 will not wakeup at the DTIM Beacon, but its wakeup depends only 
			on the the configured Listen Interval. 
@return     The function SHALL return 0 for success and a negative value otherwise.
@sa			tenuPowerSaveModes
@warning    The function called once after initialization.  
*/
sint8 m2m_wifi_set_sleep_mode(uint8 PsTyp, uint8 BcastEn)
{
	sint8 ret = M2M_SUCCESS;
#ifndef CONF_WILC_USE_3000_REV_A
	tstrM2mPsType* pstrPs = &guCtrlStruct.strM2mPsType;
	pstrPs->u8PsType = PsTyp;
	pstrPs->u8BcastEn = BcastEn;
	ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SLEEP, (uint8*)pstrPs,sizeof(tstrM2mPsType), NULL, 0, 0);
	M2M_INFO("POWER SAVE %d\n",PsTyp);
	hif_set_sleep_mode(PsTyp);
#endif
	return ret;
}
/*!
@fn			NMI_API sint8 m2m_wifi_set_device_name(uint8 *pu8DeviceName, uint8 u8DeviceNameLength);
@brief		Set the WILC1000 device name which is used as P2P device name.
@param [in]	pu8DeviceName
			Buffer holding the device name.
@param [in]	u8DeviceNameLength
			Length of the device name.
@return		The function SHALL return M2M_SUCCESS for success and a negative value otherwise.
@warning	The Function called once after initialization. 
*/
sint8 m2m_wifi_set_device_name(uint8 *pu8DeviceName, uint8 u8DeviceNameLength)
{
	tstrM2MDeviceNameConfig* pstrDeviceName = &guCtrlStruct.strM2MDeviceNameConfig;
	if(u8DeviceNameLength >= M2M_DEVICE_NAME_MAX)
	{
		u8DeviceNameLength = M2M_DEVICE_NAME_MAX;
	}
	//pu8DeviceName[u8DeviceNameLength] = '\0';
	u8DeviceNameLength ++;
	m2m_memcpy(pstrDeviceName->au8DeviceName, pu8DeviceName, u8DeviceNameLength);
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_DEVICE_NAME,
		(uint8*)pstrDeviceName, sizeof(tstrM2MDeviceNameConfig), NULL, 0,0);
}
#ifdef CONF_MGMT
sint8 m2m_wifi_enable_monitoring_mode(tstrM2MWifiMonitorModeCtrl *pstrMtrCtrl)
{
	sint8	s8Ret = -1;
	tstrM2MWifiMonitorModeCtrl* pstrTmpMonitorModeCtrl = &guCtrlStruct.strM2MWifiMonitorModeCtrl;
	m2m_memcpy((uint8*)pstrTmpMonitorModeCtrl, (uint8*)pstrMtrCtrl, sizeof(tstrM2MWifiMonitorModeCtrl));
	if((pstrMtrCtrl->u8ChannelID >= M2M_WIFI_CH_1) && (pstrMtrCtrl->u8ChannelID <= M2M_WIFI_CH_14))
	{
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_ENABLE_MONITORING | M2M_REQ_DATA_PKT,
			(uint8*)pstrTmpMonitorModeCtrl, sizeof(tstrM2MWifiMonitorModeCtrl), NULL, 0,0);
	}
	return s8Ret;
}

sint8 m2m_wifi_disable_monitoring_mode(void)
{
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_DISABLE_MONITORING, NULL, 0, NULL, 0,0);
}

sint8 m2m_wifi_change_monitoring_channel(uint8 u8ChannelID)
{
	sint8	s8Ret = -1;
	guCtrlStruct.u8ChannelID = u8ChannelID;
	if((u8ChannelID >= M2M_WIFI_CH_1) && (u8ChannelID <= M2M_WIFI_CH_14))
	{		
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_CHG_MONITORING_CHNL | M2M_REQ_DATA_PKT,
			(uint8*)(&guCtrlStruct.u8ChannelID), sizeof(u8ChannelID), NULL, 0,0);
	}
	return s8Ret;
}

sint8 m2m_wifi_send_wlan_pkt(uint8 *pu8WlanPacket, uint16 u16WlanHeaderLength, uint16 u16WlanPktSize)
{
	sint8	s8Ret = -1;
	if(pu8WlanPacket != NULL)
	{
		tstrM2MWifiTxPacketInfo*		pstrTxPkt = &guCtrlStruct.strM2MWifiTxPacketInfo;

		pstrTxPkt->u16PacketSize		= NM_BSP_B_L_16(u16WlanPktSize);
		pstrTxPkt->u16HeaderLength	= NM_BSP_B_L_16(u16WlanHeaderLength);
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SEND_WIFI_PACKET | M2M_REQ_DATA_PKT,
		(uint8*)pstrTxPkt, sizeof(tstrM2MWifiTxPacketInfo), pu8WlanPacket, u16WlanPktSize, sizeof(tstrM2MWifiTxPacketInfo));
	}
	return s8Ret;
}
#endif
sint8 m2m_wifi_get_connection_info(void)
{
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_GET_CONN_INFO, NULL, 0, NULL, 0, 0);
}

/*!
@fn	\
	 NMI_API sint8 m2m_wifi_enable_mac_mcast(uint8* pu8MulticastMacAddress, uint8 u8AddRemove)

@brief
	Add MAC filter to receive Multicast packets.

@param [in]	pu8MulticastMacAddress
				Pointer to the MAC address.
@param [in] u8AddRemove
				Flag to Add/Remove MAC address.
@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/

NMI_API sint8 m2m_wifi_enable_mac_mcast(uint8* pu8MulticastMacAddress, uint8 u8AddRemove)
{
	sint8 s8ret = M2M_ERR_FAIL;
	tstrM2MMulticastMac*  pstrMulticastMac = &guCtrlStruct.strM2MMulticastMac;

	if(pu8MulticastMacAddress != NULL )
	{
		pstrMulticastMac->u8AddRemove = u8AddRemove;
		m2m_memcpy(pstrMulticastMac->au8macaddress,pu8MulticastMacAddress,M2M_MAC_ADDRES_LEN);
		M2M_DBG("mac multicast: %x:%x:%x:%x:%x:%x\r\n",
			pstrMulticastMac->au8macaddress[0],pstrMulticastMac->au8macaddress[1],
			pstrMulticastMac->au8macaddress[2],pstrMulticastMac->au8macaddress[3],
			pstrMulticastMac->au8macaddress[4],pstrMulticastMac->au8macaddress[5]);
		s8ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_MAC_MCAST, (uint8 *)pstrMulticastMac,sizeof(tstrM2MMulticastMac),NULL,0,0);
	}

	return s8ret;

}
/*!
@fn	\
	 NMI_API sint8 m2m_wifi_enable_mcast_filter(void)

@brief
	Enable Multicast filter.

@param [in]	None
@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/

NMI_API sint8 m2m_wifi_enable_mcast_filter(void)
{
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_ENABLE_MCAST_FILTER, NULL, 0, NULL,0,0);
}
/*!
@fn	\
	 NMI_API sint8 m2m_wifi_enable_mcast_filter(void)

@brief
	Enable Multicast filter.

@param [in]	None
@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/

NMI_API sint8 m2m_wifi_disable_mcast_filter(void)
{
	return hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_DISABLE_MCAST_FILTER, NULL, 0, NULL,0,0);
}

/*!
@fn	\
	NMI_API sint8  m2m_wifi_set_receive_buffer(void* pvBuffer,uint16 u16BufferLen);

@brief
	set the ethernet receive buffer, should be called in the receive call back.

@param [in]	pvBuffer
				Pointer to the ethernet receive buffer.
@param [in] u16BufferLen
				Length of the buffer.

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8  m2m_wifi_set_receive_buffer(void* pvBuffer,uint16 u16BufferLen)
{
	sint8 s8ret = M2M_SUCCESS;
	if(pvBuffer != NULL)
	{
		gpu8ethRcvBuf = pvBuffer;
		gu16ethRcvBufSize= u16BufferLen;
		hif_set_receive_buffer(pvBuffer, u16BufferLen);
	}
	else
	{
		s8ret = M2M_ERR_FAIL;
		M2M_ERR("Buffer NULL pointer\r\n");
	}
	return s8ret;
}

static uint32 u32CertChunkSize __M2M_DMA_BUF_ATT__;
sint8 m2m_wifi_download_cert(uint8* pCertData,uint32 u32CertSize)
{
	sint8	s8Ret = -1;
	uint8* pChunk = (uint8*)(guCtrlStruct.au8CertChunk);
	uint32 u32ChunkSize = 32,u32OrigCertSize = u32CertSize;
	if(u32CertSize > M2M_ENTERPRISE_CERT_MAX_LENGTH_IN_BYTES)
	{
		M2M_ERR("too large Cert. file, Max is %d\n",M2M_ENTERPRISE_CERT_MAX_LENGTH_IN_BYTES);
		return s8Ret;
	}
	while(u32CertSize > 0)
	{
		if(u32CertSize < u32ChunkSize)
			u32ChunkSize = u32CertSize;

		u32CertChunkSize =  (u32OrigCertSize&0xffff)|(u32ChunkSize<<16);
		m2m_memcpy((uint8 *)pChunk, &pCertData[u32OrigCertSize-u32CertSize], u32ChunkSize);
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_CERT_ADD_CHUNK, (uint8*)u32CertChunkSize, sizeof(u32CertSize),
				(uint8 *)pChunk, u32ChunkSize,sizeof(u32CertSize));
		u32CertSize -= u32ChunkSize;
	}
	s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_CERT_DOWNLOAD_DONE, NULL, 0, NULL, 0, 0);	
	return s8Ret;
}

sint8 m2m_wifi_set_p2p_control_ifc(uint8 u8IfcId)
{
	sint8	s8Ret = -1;
	tstrM2MIfId* pstrIfId = &guCtrlStruct.strM2MIfId;
	pstrIfId->u8IfcId=u8IfcId;
	if(u8IfcId == P2P_STA_CONCURRENCY_INTERFACE || u8IfcId == P2P_AP_CONCURRENCY_INTERFACE)
	{
		s8Ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_P2P_IFC_ID, (uint8*)pstrIfId, sizeof(tstrM2MIfId), NULL, 0, 0);
	}
	return s8Ret;
}

NMI_API sint8 m2m_wifi_set_max_tx_rate(tenuTxDataRate enuMaxTxDataRate)
{
	sint8 s8ret = M2M_ERR_FAIL;
	tstrM2MUserMaxTxRate*  pstrMaxTxRate = &guCtrlStruct.strM2MUserMaxTxRate;

	if(pstrMaxTxRate != NULL )
	{
		pstrMaxTxRate->u32UsrMaxTxRate = (uint32)(enuMaxTxDataRate);
		M2M_DBG("Set Max Tx Rate to : %lu\r\n", pstrMaxTxRate->u32UsrMaxTxRate);
		s8ret = hif_send(M2M_REQ_GRP_WIFI, M2M_WIFI_REQ_SET_MAX_TX_RATE, (uint8 *)pstrMaxTxRate,sizeof(tstrM2MUserMaxTxRate),NULL,0,0);
	}

	return s8ret;
}

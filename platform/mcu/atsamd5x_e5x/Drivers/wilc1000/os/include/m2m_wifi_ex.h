/**
 *
 * \file
 *
 * \brief
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
#ifndef M2M_WIFI_EX_H_INCLUDED
#define M2M_WIFI_EX_H_INCLUDED
#include <driver/include/m2m_wifi.h>

typedef void (*m2m_wifi_callback_t)(void *arg);

sint8 os_m2m_wifi_init(tstrWifiInitParam *param);
sint8 os_m2m_wifi_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, tuniM2MWifiAuth *pvAuthInfo, uint16 u16Ch);
sint8 os_m2m_wifi_enable_ap(tstrM2MAPConfig *ap);
sint8 m2m_wifi_request_callback_ex(m2m_wifi_callback_t callback, void *arg);
sint8 m2m_wifi_request_dhcp_client_ex(void);
sint8 m2m_wifi_enable_ap_ex(CONST tstrM2MAPConfig* pstrM2MAPConfig);
sint8 m2m_wifi_disable_ap_ex(void);

// Macros to alias all other APIs to the _ex versions.
#define m2m_wifi_deinit_ex					m2m_wifi_deinit
#define m2m_wifi_handle_events_ex			m2m_wifi_handle_events
#define m2m_wifi_disconnect_ex				m2m_wifi_disconnect
#define m2m_wifi_get_connection_info_ex		m2m_wifi_get_connection_info
#define m2m_wifi_wps_ex						m2m_wifi_wps
#define m2m_wifi_wps_disable_ex				m2m_wifi_wps_disable 
#define m2m_wifi_p2p_ex						m2m_wifi_p2p
#define m2m_wifi_p2p_disconnect_ex			m2m_wifi_p2p_disconnect
#define m2m_wifi_set_scan_options_ex		m2m_wifi_set_scan_options
#define m2m_wifi_set_scan_region_ex			m2m_wifi_set_scan_region
#define m2m_wifi_request_scan_ex			m2m_wifi_request_scan
#define m2m_wifi_get_num_ap_found_ex		m2m_wifi_get_num_ap_found
#define m2m_wifi_req_scan_result_ex			m2m_wifi_req_scan_result
#define m2m_wifi_req_curr_rssi_ex			m2m_wifi_req_curr_rssi
#define m2m_wifi_get_otp_mac_address_ex		m2m_wifi_get_otp_mac_address
#define m2m_wifi_get_mac_address_ex			m2m_wifi_get_mac_address
#define m2m_wifi_set_sleep_mode_ex			m2m_wifi_set_sleep_mode
#define m2m_wifi_get_sleep_mode_ex			m2m_wifi_get_sleep_mode
#define m2m_wifi_set_device_name_ex			m2m_wifi_set_device_name
#define m2m_wifi_set_lsn_int_ex				m2m_wifi_set_lsn_int
#define m2m_wifi_enable_monitoring_mode_ex	m2m_wifi_enable_monitoring_mode
#define m2m_wifi_disable_monitoring_mode_ex	m2m_wifi_disable_monitoring_mode
#define m2m_wifi_send_wlan_pkt_ex			m2m_wifi_send_wlan_pkt
#define m2m_wifi_send_ethernet_pkt_ex		m2m_wifi_send_ethernet_pkt
#define m2m_wifi_set_cust_InfoElement_ex	m2m_wifi_set_cust_InfoElement
#define m2m_wifi_enable_mac_mcast_ex		m2m_wifi_enable_mac_mcast
#define m2m_wifi_set_receive_buffer_ex		m2m_wifi_set_receive_buffer

sint8 os_m2m_wifi_deinit(void* arg);
sint8 os_m2m_wifi_default_connect(void);
sint8 os_m2m_wifi_disconnect(void);
sint8 os_m2m_wifi_get_connection_info(void);
sint8 os_m2m_wifi_set_mac_address(uint8* au8MacAddress0, uint8* au8MacAddress1);
sint8 os_m2m_wifi_wps(uint8 u8TriggerType, const char* pcPinNumber);
sint8 os_m2m_wifi_wps_disable(void);
sint8 os_m2m_wifi_p2p(uint8 u8Channel);
sint8 os_m2m_wifi_p2p_disconnect(void);
sint8 os_m2m_wifi_disable_ap(void);
sint8 os_m2m_wifi_ap_get_assoc_info(void);
sint8 os_m2m_wifi_set_scan_options(uint8 u8NumOfSlot, uint8 u8SlotTime);
sint8 os_m2m_wifi_set_scan_region(uint8 ScanRegion);
sint8 os_m2m_wifi_set_scan_list(tstrM2MScanList* pstrScanList);
sint8 os_m2m_wifi_request_scan(uint8 ch);
sint8 os_m2m_wifi_request_scan_ssid(uint8 ch, char* pcssid);
uint8 os_m2m_wifi_get_num_ap_found(void);
sint8 os_m2m_wifi_req_scan_result(uint8 index);
sint8 os_m2m_wifi_req_curr_rssi(void);
sint8 os_m2m_wifi_get_otp_mac_address(uint8* pu8MacAddr, uint8* pu8IsValid);
sint8 os_m2m_wifi_get_mac_address(uint8* pu8MacAddr0, uint8* pu8MacAddr1);
sint8 os_m2m_wifi_set_sleep_mode(uint8 PsTyp, uint8 BcastEn);
sint8 os_m2m_wifi_set_device_name(uint8* pu8DeviceName, uint8 u8DeviceNameLength);
sint8 os_m2m_wifi_set_lsn_int(tstrM2mLsnInt* pstrM2mLsnInt);
sint8 os_m2m_wifi_enable_monitoring_mode(tstrM2MWifiMonitorModeCtrl* pstrMtrCtrl);
sint8 os_m2m_wifi_disable_monitoring_mode(void);
sint8 os_m2m_wifi_send_wlan_pkt(uint8* pu8WlanPacket, uint16 u16WlanHeaderLength, uint16 u16WlanPktSize);
sint8 os_m2m_wifi_send_ethernet_pkt(uint8* pu8Packet, uint16 u16PacketSize, uint8 u8IfcId);
sint8 os_m2m_wifi_set_cust_InfoElement(uint8* pau8M2mCustInfoElement);
sint8 os_m2m_wifi_enable_mac_mcast(uint8* pu8MulticastMacAddress, uint8 u8AddRemove);
sint8 os_m2m_wifi_set_receive_buffer(void* pvBuffer, uint16 u16BufferLen);
sint8 os_m2m_wifi_set_p2p_control_ifc(uint8 u8IfcId);
uint8 os_m2m_wifi_get_sleep_mode(void);
sint8 os_m2m_wifi_set_max_tx_rate(tenuTxDataRate enuTxDataRate);
sint8 os_m2m_wifi_enable_mcast_filter(void);
sint8 os_m2m_wifi_disable_mcast_filter(void);


#endif /* M2M_WIFI_EX_H_INCLUDED */

/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BWIFI_STA_H
#define BWIFI_STA_H

#ifdef  __cplusplus
extern "C"
{
#endif

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

struct bwifi_station_config {
	u8 ssid[33];
	u8 passwd[65];//passwd for encrypted ssid, set "all zero" for unencryped ssid
	int8 wep_keyid;//for wep,[0-3] default val:0
	u8 hidden;//1:hidden
	u8 bssid[ETH_ALEN];//bssid or "all zero"
};
struct bwifi_quick_connect_config {
	struct bwifi_station_config config;
	uint32 channel;
	uint32 ip[3];//struct ip_info ip;
};

#ifdef  __cplusplus
}
#endif

#endif /*BWIFI_STA_H*/

## Contents

```sh
.
├── aos.mk
├── app_entry.c
├── app_entry.h
├── Config.in
├── k_app_config.h
├── linkkit_example_solo.c
├── README.md
└── ucube.py

```

## Introduction

The **ulocationapp**  shows ulocation related functions.

### Dependencies

* netmgr
* cjson
* ulocation

### Build

> Notice: Please be sure to use **git bash** when build on Windows! Otherwise may resulting in run problem for linkkit. git bash download url https://git-scm.com/download/win

```sh
aos make clean
aos make menuconfig
Application selects uLocationapp
BSP selects whatever board you want to use
aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload ulocationapp@yourboard
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh
[608200]<I> AT command AT+WSCANOPT=1 succeed, rsp:
OK

buffer full
buffer full
[609720]<I> AT command AT+WSCAN succeed, rsp:
+WSCAN:29
alibaba-guest,141FBA8C7F91,0,1,-30
,307BACF5A620,0,11,-30
h3c_router01,307BACF5A622,5,11,-31
Yuemewifi-3766,948B03033766,0,8,-32
,327BACF5A620,5,11,-33
alibaba-inc,141FBA8C7F90,5,1,-38
NETGEAR63,A00460E99B3E,5,2,-43
,94282EABA0E6,0,1,-44
alibaba-inc,141FBA8C8D00,5,11,-46
alibaba-inc,141FBA8C9100,5,11,-49
Pi3-AP,B827EB271F36,5,1,-50
alibaba-guest,141FBA8C7D61,0,6,-51
alibaba-inc,141FBA8C7D60,5,6,-51
alibaba-guest,141FBA8C8221,0,1,-54
antbang_05E9FE,001CA305E9FE,5,10,-54
alibaba-guest,141FBA8C8251,0,11,-54
alibaba-guest,141FBA8C8D01,0,11,-54
alibaba-inc,141FBA8C8220,5,1,-55
alibaba-guest,141FBA8C9101,0,11,-55
alibaba-guest,141FBA8C8041,0,6,-58
alibaba-guest,141FBA8C6F31,0,6,-58
 免费安全共享WiFi,021CA302E9FE,0,10,-58
alibaba-inc,141FBA8C8040,5,6,-59
alibaba-inc,141FBA8C9290,5,11,-59
alibaba-guest,141FBA8C9291,0,11,-63
alibaba-inc,141FBA8C9020,5,11,-69
alibaba-guest,141FBA8C9021,0,11,-71
alibaba-inc,141FBA8C8250,5,11,-76
alibaba-guest,141FBA8C8E61,0,1,-80

OK

[609860]<I> 0: +WSCAN:29
[609860]<I> 0: alibaba-guest,141FBA8C7F91,0,1,-30
[609870]<I> 1: ,307BACF5A620,0,11,-30
[609880]<I> 2: h3c_router01,307BACF5A622,5,11,-31
[609880]<I> 3: Yuemewifi-3766,948B03033766,0,8,-32
[609890]<I> 4: ,327BACF5A620,5,11,-33
[609890]<I> 5: alibaba-inc,141FBA8C7F90,5,1,-38
[609900]<I> 6: NETGEAR63,A00460E99B3E,5,2,-43
[609910]<I> 7: ,94282EABA0E6,0,1,-44
[609910]<I> 8: alibaba-inc,141FBA8C8D00,5,11,-46
[609920]<I> 9: alibaba-inc,141FBA8C9100,5,11,-49
[609920]<I>
+WSCAN:29
alibaba-guest,141FBA8C7F91,0,1,-30
,307BACF5A620,0,11,-30
h3c_router01,307BACF5A622,5,11,-31
Yuemewifi-3766,948B03033766,0,8,-32
,327BACF5A620,5,11,-33
alibaba-inc,141FBA8C7F90,5,1,-38
NETGEAR63,A00460E99B3E,5,2,-43
,94282EABA0E6,0,1,-44
alibaba-inc,141FBA8C8D00,5,11,-46
alibaba-inc,141FBA8C9100,5,11,-49
Pi3-AP,B827EB271F36,5,1,-50
alibaba-guest,141FBA8C7D61,0,6,-51
alibaba-inc,141FBA8C7D60,5,6,-51
alibaba-guest,141FBA8C8221,0,1,-54
antbang_05E9FE,001CA305E9FE,5,10,-54
alibaba-guest,141FBA8C8251,0,11,-54
alibaba-guest,141FBA8C8D01,0,11,-54
alibaba-inc,141FBA8C8220,5,1,-55
alibaba-guest,141FBA8C9101,0,11,-55
alibaba-guest,141FBA8C8041,0,6,-58
alibaba-guest,141FBA8C6F31,0,6,-58
 免费安全共享WiFi,021CA302E9FE,0,10,-58
alibaba-inc,141FBA8C8040,5,6,-59
alibaba-inc,141FBA8C9290,5,11,-59
alibaba-guest,141FBA8C9291,0,11,-63
alibaba-inc,141FBA8C9020,5,11,-69
alibaba-guest,141FBA8C9021,0,11,-71
alibaba-inc,141FBA8C8250,5,11,-76
alibaba-guest,141FBA8C8E61,0,1,-80

OK

uloc_hal_get_wifi.97: hal_wifi_scan_ap_list returns 10 aps info

uloc_hal_get_wifi.111: test_wifi result for idx=0: wifi_strength: -30, ssid: alibaba-guest, channel 1, bssid: 20 31 186 140 127 145
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 0: 14:1f:ba:8c:7f:91,-30,alibaba-guest
uloc_hal_get_wifi.111: test_wifi result for idx=1: wifi_strength: -30, ssid: , channel 11, bssid: 48 123 172 245 166 32
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 1: 30:7b:ac:f5:a6:20,-30,
uloc_hal_get_wifi.111: test_wifi result for idx=2: wifi_strength: -31, ssid: h3c_router01, channel 11, bssid: 48 123 172 245 166 34
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 2: 30:7b:ac:f5:a6:22,-31,h3c_router01
uloc_hal_get_wifi.111: test_wifi result for idx=3: wifi_strength: -32, ssid: Yuemewifi-3766, channel 8, bssid: 148 139 3 3 55 102
uloc_hal_get_wifi.111: test_wifi result for idx=4: wifi_strength: -33, ssid: , channel 11, bssid: 50 123 172 245 166 32
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 4: 32:7b:ac:f5:a6:20,-33,
uloc_hal_get_wifi.111: test_wifi result for idx=5: wifi_strength: -38, ssid: alibaba-inc, channel 1, bssid: 20 31 186 140 127 144
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 5: 14:1f:ba:8c:7f:90,-38,alibaba-inc
uloc_hal_get_wifi.111: test_wifi result for idx=6: wifi_strength: -43, ssid: NETGEAR63, channel 2, bssid: 160 4 96 233 155 62
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 6: a0:04:60:e9:9b:3e,-43,NETGEAR63
uloc_hal_get_wifi.111: test_wifi result for idx=7: wifi_strength: -44, ssid: , channel 1, bssid: 148 40 46 171 160 230
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 7: 94:28:2e:ab:a0:e6,-44,
uloc_hal_get_wifi.111: test_wifi result for idx=8: wifi_strength: -46, ssid: alibaba-inc, channel 11, bssid: 20 31 186 140 141 0
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 8: 14:1f:ba:8c:8d:00,-46,alibaba-inc
uloc_hal_get_wifi.111: test_wifi result for idx=9: wifi_strength: -49, ssid: alibaba-inc, channel 11, bssid: 20 31 186 140 145 0
conver_ap_info_fmt.40: conver_ap_info_fmt

uloc_hal_get_wifi.124: MAIN AP info for ap 9: 14:1f:ba:8c:91:00,-49,alibaba-inc
user_post_locationinfoo.524: Post event Message : {"mmac":"94:8b:03:03:37:66,-32,Yuemewifi-3766","macs":"14:1f:ba:8c:7f:91,-30,alibaba-guest|30:7b:ac:f5:a6:20,-30,|30:7b:ac:f5:a6:22,-31,h3c_router01|32:7b:ac:f5:a6:20,-33,|14:1f:ba:8c:7f:90,-38,alibaba-inc|a0:04:60:e9:9b:3e,-43,NETGEAR63|94:28:2e:ab:a0:e6,-44,|14:1f:ba:8c:8d:00,-46,alibaba-inc|14:1f:ba:8c:91:00,-49,alibaba-inc"}
[610460]<I> DM Send Message, URI: /sys/a1kxNWAYNmp/gaode_dev_001/thing/event/LocationInfo/post, Payload: {"id":"143","version":"1.0","params":{"mmac":"94:8b:03:03:37:66,-32,Yuemewifi-3766","macs":"14:1f:ba:8c:7f:91,-30,alibaba-guest|30:7b:ac:f5:a6:20,-30,|30:7b:ac:f5:a6:22,-31,h3c_router01|32:7b:ac:f5:a6:20,-33,|14:1f:ba:8c:7f:90,-38,alibaba-inc|a0:04:60:e9:9b:3e,-43,NETGEAR63|94:28:2e:ab:a0:e6,-44,|14:1f:ba:8c:8d:00,-46,alibaba-inc|14:1f:ba:8c:91:00,-49,alibaba-inc"},"method":"thing.event.LocationInfo.post"}
[610530]<I> Upstream Topic: '/sys/a1kxNWAYNmp/gaode_dev_001/thing/event/LocationInfo/post'
[610540]<I> Upstream Payload:

> {
>     "id": "143",
>     "version": "1.0",
>     "params": {
>         "mmac": "94: 8b: 03: 03: 37: 66,
>         -32,
>         Yuemewifi-3766",
>         "macs": "14: 1f: ba: 8c: 7f: 91,
>         -30,
>         alibaba-guest|30: 7b: ac: f5: a6: 20,
>         -30,
>         |30: 7b: ac: f5: a6: 22,
>         -31,
>         h3c_router01|32: 7b: ac: f5: a6: 20,
>         -33,
>         |14: 1f: ba: 8c: 7f: 90,
>         -38,
>         alibaba-inc|a0: 04: 60: e9: 9b: 3e,
>         -43,
>         NETGEAR63|94: 28: 2e: ab: a0: e6,
>         -44,
>         |14: 1f: ba: 8c: 8d: 00,
>         -46,
>         alibaba-inc|14: 1f: ba: 8c: 91: 00,
>         -49,
>         alibaba-inc"
>     },
>     "method": "thing.event.LocationInfo.post"
> }

```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [gateway](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides)

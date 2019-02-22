## Contents

```sh
.
├── aos.mk
├── app_entry.c
├── app_entry.h
├── combo
│   ├── combo_net.c
│   └── combo_net_devinfo.h
├── Config.in
├── data
│   ├── cntdown_tsl.data
│   ├── sched_tsl.data
│   └── solo_tsl.data
├── deprecated
│   ├── cntdown.c
│   ├── sched.c
│   └── solo.c
├── k_app_config.h
├── linkkit_example_cntdown.c
├── linkkit_example_cota.c
├── linkkit_example_sched.c
├── linkkit_example_solo.c
├── README.md
├── readme.txt
└── ucube.py


```

## Introduction

The **linkkitapp**  shows linkkit related functions.

### Dependencies

* yloop
* cli
* linkkit

### Build

> Notice: Please be sure to use **git bash** when build on Windows! Otherwise may resulting in run problem for linkkit. git bash download url https://git-scm.com/download/win

```sh
aos make clean
```

1).linkkit_example_solo.c(default):

```sh
    aos make clean
    aos make linkkitapp@xxxx
```

2).linkkit_example_sched.c:

```sh
    aos make clean
    aos make linkkitapp@xxxx case=sched
```

3).linkkit_example_cntdown.c:

```sh
    aos make clean
    aos make linkkitapp@xxxx case=cntdown
```

4).deprecated/solo.c:

```sh
    aos make clean
    aos make linkkitapp@xxxx deprecated=1
```

5).deprecated/cntdown.c:

```sh
    aos make clean
    aos make linkkitapp@xxxx deprecated=1  case=cntdown
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload linkkitapp@yourboard
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh
             Welcome to AliOS Things
 [   0.000]<I> netmgr [netmgr_ip_got_event#140] : Got ip : 127.0.0.1, gw : 127.0.0.1, mask : 255.255.255.0
 [   0.000]<V> AOS [wifi_service_event#60] : wifi_service_event config.ssid cisco-15A7
[prt] log level set as: [ 5 ]
[dbg] _dm_mgr_search_dev_by_devid(54): Device Not Found, devid: 0
[inf] dm_client_open(37): CM Fd: 0
 [   0.000]<V> AOS [linkkit_event_monitor#186] : IOTX_CONN_CLOUD
[inf] guider_print_dev_guider_info(320): ....................................................
[inf] guider_print_dev_guider_info(321):           ProductKey : a1X2bEnP82z
[inf] guider_print_dev_guider_info(322):           DeviceName : example1
[inf] guider_print_dev_guider_info(323):             DeviceID : a1X2bEnP82z.example1
[inf] guider_print_dev_guider_info(327): ....................................................
[inf] guider_print_dev_guider_info(328):        PartnerID Buf : ,partner_id=example.demo.partner-id
[inf] guider_print_dev_guider_info(329):         ModuleID Buf : ,module_id=example.demo.module-id
[inf] guider_print_dev_guider_info(330):           Guider URL :
[inf] guider_print_dev_guider_info(332):       Guider SecMode : 2 (TLS + Direct)
[inf] guider_print_dev_guider_info(334):     Guider Timestamp : 2524608000000
[inf] guider_print_dev_guider_info(338): ....................................................
[inf] guider_print_conn_info(297): -----------------------------------------
[inf] guider_print_conn_info(298):             Host : a1X2bEnP82z.iot-as-mqtt.cn-shanghai.aliyuncs.com
[inf] guider_print_conn_info(299):             Port : 1883
[inf] guider_print_conn_info(304):         ClientID : a1X2bEnP82z.example1|securemode=2,timestamp=2524608000000,signmethod=hmacsha1,gw=0,ext=0,partner_id=example.demo.partner-id,module_id=example.demo.module-id|
[inf] guider_print_conn_info(306):       TLS PubKey : 0x80b8cec ('-----BEGIN CERTI ...')
[inf] guider_print_conn_info(309): -----------------------------------------
[inf] iotx_mc_init(2334): MQTT init success!
 [   0.000]<I> HAL_TLS [_ssl_client_init#130] : Loading the CA root certificate ...
 [   0.000]<I> HAL_TLS [_ssl_client_init#138] :  ok (0 skipped)
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#327] : Connecting to /a1X2bEnP82z.iot-as-mqtt.cn-shanghai.aliyuncs.com/1883...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#342] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#347] :   . Setting up the SSL/TLS structure...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#360] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#401] : Performing the SSL/TLS handshake...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#411] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#415] :   . Verifying peer X.509 certificate..
 [   0.000]<I> HAL_TLS [_real_confirm#77] : certificate verification result: 0x00
[inf] iotx_mc_connect(2724): mqtt connect success!
wifi_get_mac_addr!!
 [   0.000]<V> AOS [mqtt_connected_event_handler#293] : MQTT Construct  OTA start
 [   0.000]<E> uota [ota_service_init#388] : transport init

 [   0.000]<I> uota [ota_service_init#390] : init mqtt success
 [   0.000]<I> uota [ota_mqtt_publish#66] : mqtt pub name:/ota/device/inform/a1X2bEnP82z/example1 msg:{"id":0,"params":{"version":"app-1.0.0-20181130.1924"}}
 [   0.000]<I> uota [ota_trans_upgrade#139] : upgrade:/ota/device/upgrade/a1X2bEnP82z/example1
[dbg] iotx_mc_subscribe(1994): PERFORM subscribe to '/ota/device/upgrade/a1X2bEnP82z/example1' (msgId=5)
[dbg] MQTTSubscribe(585):         Packet Ident : 00000005
[dbg] MQTTSubscribe(586):                Topic : /ota/device/upgrade/a1X2bEnP82z/example1
[dbg] MQTTSubscribe(587):                  QoS : 0
[dbg] MQTTSubscribe(588):        Packet Length : 47
[inf] iotx_mc_subscribe(2005): mqtt subscribe packet sent,topic = /ota/device/upgrade/a1X2bEnP82z/example1!
[dbg] IOT_MQTT_Subscribe_Sync(3329): packet_id = 5
[dbg] iotx_mc_cycle(1791): PUBACK
[dbg] iotx_mc_cycle(1791): PUBACK
[dbg] iotx_mc_cycle(1791): PUBACK
[dbg] iotx_mc_cycle(1791): PUBACK
[dbg] iotx_mc_cycle(1801): SUBACK
[dbg] iotx_mc_handle_recv_SUBACK(1413):         Return Value : 1

```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [gateway](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides)

## Contents

```sh
.
├── aos.mk
├── app_entry.c
├── app_entry.h
├── Config.in
├── k_app_config.h
├── mqtt_example.c
├── mqtt_example_multithread.c
├── mqtt_example_rrpc.c
├── mqtt_presstest.c
├── README.md
└── ucube.py
```

## Introduction

The **mqttapp**  shows mqtt related functions.

### Dependencies

* yloop
* cli
* linkkit

### Supported Boards

- mk3060
- mk3080
- esp8266

### Build



> Notice: Please be sure to use git bash when build on Windows! Otherwise may resulting in run problem for linkkit. git bash download url https://git-scm.com/download/win

```sh
    aos make clean
```

1).mqtt_example.c:

```sh
    aos make mqttapp@xxxx MQTTAPP_CONFIG_CASE_DEFAULT=y
```

2).mqtt_example_rrpc.c:

```sh
    aos make mqttapp@xxxx MQTTAPP_CONFIG_CASE_RRPC=y
```

3).mqtt_example_multithread.c:

```sh
    aos make mqttapp@xxxx MQTTAPP_CONFIG_CASE_MULTITHREAD=y
```

3).mqtt_presstest.c:
> refs: https://github.com/AITC-LinkCertification/AITC-Manual/wiki/Manual-Channel-MQTT

```sh
    aos make mqttapp@xxxx MQTTAPP_CONFIG_CASE_PRESSTEST=y
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload mqtt_examplexxx@yourboard
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh
 [   0.000]<V> AOS [wifi_service_event#38] : wifi_service_event config.ssid cisco-15A7
[prt] log level set as: [ 5 ]
[inf] guider_print_dev_guider_info(320): ....................................................
[inf] guider_print_dev_guider_info(321):           ProductKey : a1MZxOdcBnO
[inf] guider_print_dev_guider_info(322):           DeviceName : test_01
[inf] guider_print_dev_guider_info(323):             DeviceID : a1MZxOdcBnO.test_01
[inf] guider_print_dev_guider_info(327): ....................................................
[inf] guider_print_dev_guider_info(328):        PartnerID Buf : ,partner_id=example.demo.partner-id
[inf] guider_print_dev_guider_info(329):         ModuleID Buf : ,module_id=example.demo.module-id
[inf] guider_print_dev_guider_info(330):           Guider URL : 
[inf] guider_print_dev_guider_info(332):       Guider SecMode : 2 (TLS + Direct)
[inf] guider_print_dev_guider_info(334):     Guider Timestamp : 2524608000000
[inf] guider_print_dev_guider_info(338): ....................................................
[inf] guider_print_conn_info(297): -----------------------------------------
[inf] guider_print_conn_info(298):             Host : a1MZxOdcBnO.iot-as-mqtt.cn-shanghai.aliyuncs.com
[inf] guider_print_conn_info(299):             Port : 1883
[inf] guider_print_conn_info(304):         ClientID : a1MZxOdcBnO.test_01|securemode=2,timestamp=2524608000000,signmethod=hmacsha1,gw=0,ext=0,partner_id=example.demo.partner-id,module_id=example.demo.module-id|
[inf] guider_print_conn_info(306):       TLS PubKey : 0x8087abc ('-----BEGIN CERTI ...')
[inf] guider_print_conn_info(309): -----------------------------------------
[inf] iotx_mc_init(2334): MQTT init success!
 [   0.000]<I> HAL_TLS [_ssl_client_init#130] : Loading the CA root certificate ...
 [   0.000]<I> HAL_TLS [_ssl_client_init#138] :  ok (0 skipped)
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#327] : Connecting to /a1MZxOdcBnO.iot-as-mqtt.cn-shanghai.aliyuncs.com/1883...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#342] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#347] :   . Setting up the SSL/TLS structure...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#360] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#401] : Performing the SSL/TLS handshake...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#411] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#415] :   . Verifying peer X.509 certificate..
 [   0.000]<I> HAL_TLS [_real_confirm#77] : certificate verification result: 0x00
[inf] iotx_mc_connect(2724): mqtt connect success!
wifi_get_mac_addr!!
mqtt_client|193 :: 
 publish message: 
 topic: /a1MZxOdcBnO/test_01/update
 payload: update: hello! start!
 rc = 4
[dbg] iotx_mc_subscribe(1994): PERFORM subscribe to '/a1MZxOdcBnO/test_01/data' (msgId=5)
[dbg] MQTTSubscribe(585):         Packet Ident : 00000005
[dbg] MQTTSubscribe(586):                Topic : /a1MZxOdcBnO/test_01/data
[dbg] MQTTSubscribe(587):                  QoS : 1
[dbg] MQTTSubscribe(588):        Packet Length : 32
[inf] iotx_mc_subscribe(2005): mqtt subscribe packet sent,topic = /a1MZxOdcBnO/test_01/data!
[dbg] iotx_mc_cycle(1791): PUBACK
event_handle|080 :: publish success, packet-id=1
[dbg] iotx_mc_cycle(1791): PUBACK
```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [gateway](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/MQTT_Connect)

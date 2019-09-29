## Contents

```sh
.
├── aos.mk
├── app_entry.c
├── app_entry.h
├── coap_example.c
├── Config.in
├── k_app_config.h
├── README.md
└── ucube.py

```

## Introduction

The **coapapp**  shows how to connect and send data to the iot platform with coap.

### Dependencies

* yloop
* cli
* coap

### Supported Boards

- mk3060

### Build

```sh
# generate coapapp@mk3060 default config
aos make coapapp@mk3060 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh
[010578]<I> Got ip : 192.168.0.174, gw : 192.168.0.1, mask : 255.255.255.0
[010582]<V> wifi_service_event config.ssid Tenda_ydj
[prt] log level set as: [ 5 ]
[COAP-Client]: Enter Coap Client
*****The Product Key  : a1RP1qZfrEi *****
*****The Device Name  : example1 *****
*****The Device Secret: fPloaszb6saUFJte4EhyuBHQpR9SuKHb *****
*****The Device ID    : a1RP1qZfrEi.example1 *****
[dbg] Cloud_CoAPUri_parse(31): The uri is coap-psk://a1RP1qZfrEi.coap.cn-shanghai.link.aliyuncs.com:5682
[dbg] Cloud_CoAPUri_parse(69): The endpoint type is: 2
[dbg] Cloud_CoAPUri_parse(97): The host name is: a1RP1qZfrEi.coap.cn-shanghai.link.aliyuncs.com
[dbg] Cloud_CoAPUri_parse(120): The port is: 5682
[dbg] iotx_calc_sign_with_seq(91): The source string: clientIda1RP1qZfrEi.example1deviceNameexample1productKeya1RP1qZfrEiseq4912
[dbg] iotx_calc_sign_with_seq(95): The device name sign with seq: d927694052c4edbbe79da64e0314ef16
[dbg] IOT_CoAP_DeviceNameAuth(507): The payload is: {"productKey":"a1RP1qZfrEi","deviceName":"example1","clientId":"a1RP1qZfrEi.example1","sign":"d927694052c4edbbe79da64e0314ef16","seq":"4912"}
[dbg] IOT_CoAP_DeviceNameAuth(508): Send authentication message to server
[dbg] Cloud_CoAPMessage_send(289): ----The message length 159-----
[dbg] Cloud_CoAPNetwork_write(120): [CoAP-NWK]: Network write return 159
[dbg] Cloud_CoAPMessage_send(296): Add message id 1 len 159 to the list
[inf] IOT_CoAP_SendMessage(617): Upstream Topic: '/topic/sys/a1RP1qZfrEi/example1/thing/status/update'
[inf] IOT_CoAP_SendMessage(618): Upstream Payload:

> {
>     "id": "a1RP1qZfrEi_example1_mid",
>     "params": {
>         "_sys_device_mid": "example.demo.module-id",
>         "_sys_device_pid": "example.demo.partner-id"
>     }
> }

```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [Coap_connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/CoAP_Connect)

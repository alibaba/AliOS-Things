## Contents

```sh
.
├── aos.mk
├── qianxun.c
├── Config.in
├── k_app_config.h
├── README.md
└── ucube.py

```

## Introduction

The **ulocation.qianxunapp**  shows ulocation component use qianxun location service.

### Dependencies

* netmgr
* ulocation

* you should have a difference compute GNSS module .
* this sample use the qianxun location service , if you want to use it, you must go to the 
  https://www.qxwz.com/ to get the APPKEY and APPSECRET .

### Build

```sh
aos make clean
aos make menuconfig
Application selects qianxunapp
BSP selects whatever board you want to use
aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload ulocation.qianxunapp@yourboard
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh
1、--QXWZ_TIMESTAMP_RUNNING
[030560]<I>  Server  conn  (0)  disconnected.
[030580]<I>  Server  closed  event.
[030580]<I>  Server  conn  (0)  closed.
timestamp_cb_recv(),  soc=0,  state=TIMESTAMP_STATE_CONNECTED
timestamp_cb_recv():  soc=0,  cur  rcv  len=0    new  rcv  len=324
timestamp  fsm:  move_tostate:  TIMESTAMP_STATE_CONNECTED  next_state  TIMESTAMP_STATE_CONNECTED
timestamp_cb_recv():  soc=0,    rcv  buf=HTTP/1.1  200  OK
Server:  nginx/1.14.1
Date:  Sun,  26  May  2019  12:57:26  GMT
Content-Type:  application/json;  charset=UTF-8
Content-Length:  13
Connection:  close
ETag:  1md8g+YIwfZwqIgeuF5cbHA3
wz-request-id:  11211411111611111010397116101119971214511898106484945511558875446850RbLhxv9AOz7iLtK5
Via:  n25_125

2、--QXWZ_AUTH_START--
{"data":{"expireTime":1558972799999,"services":{},"dsk":"D0000043M2O","dss":"6acafafb5d3735be97d8fb73d26ca9b073b973bed401ab75ee4507154ab49ee9","toExpire":true},"code":0}
qxwz_rtcm_authenticate_user=HTTP/1.1 200 OK
Server: nginx/1.14.1
Date: Sun, 26 May 2019 12:57:37 GMT
Content-Type: application/json; charset=UTF-8
Content-Length: 169
Connection: close
ETag: 1qtw2e8XTVwXcMW2uC9BGQw9
wz-request-id: 11211411111611111010397116101119971214511898106484945501558875457950Wsj5aAxeNoAhkXd1
Via: n4_237

3、--QXWZ_TRACK_START--
--QXWZ_TRACK_RUNNING: next state=2 --
[051890]<I> Server conn (0) disconnected.
[051900]<I> Server closed event.
[051910]<I> Server conn (0) closed.
track_cb_recv(), soc=0, state=TRACK_STATE_CONNECTED
track_cb_recv(): soc=0, cur rcv len=0  new rcv len=321
track fsm: move_tostate: TRACK_STATE_CONNECTED next_state TRACK_STATE_CONNECTED
track_cb_recv(): soc=0,  rcv buf=HTTP/1.1 200 OK
Server: nginx/1.14.1
Date: Sun, 26 May 2019 12:57:48 GMT
Content-Type: application/json; charset=UTF-8
Content-Length: 10
Connection: close
ETag: 1uedUrdddUdiIznWF3J3+QQ0
wz-request-id: 11211411111611111010397116101119971214511898106484945511558875468176Icjc8SYNByfU4mzU
Via: n25_125

4、--QXWZ_RTCM_RUNNING--
qxwz_ntrip_send_gga
qxwz_soc_send_local: send to soc:0, length=82
qxwz_soc_send_local: data: $GNGGA,125816.000,3101.093289,N,12113.464327,E,2,10,1.76,43.328,M,0,M,3,3459*5B


```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

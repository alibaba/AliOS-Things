## Contents

```sh
.
├── aos.mk
├── app_entry.c
├── app_entry.h
├── Config.in
├── http2_example_stream.c
├── http2_example_uploadfile.c
├── k_app_config.h
└── README.md
```

## Introduction

The **http2app**  shows how to connect and send/request data to the iot platform with http2.

### Dependencies

* yloop
* cli
* http2

### Supported Boards

- mk3060

### Build

```sh
# generate http2app@mk3060 default config
aos make http2app@mk3060 -c config

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
cpu num is 1
             Welcome to AliOS Things           
 [   0.000]<I> netmgr [netmgr_ip_got_event#140] : Got ip : 127.0.0.1, gw : 127.0.0.1, mask : 255.255.255.0
 [   0.000]<V> AOS [wifi_service_event#41] : wifi_service_event config.ssid cisco-15A7
[prt] log level set as: [ 5 ]
 [   0.000]<I> HAL_TLS [_ssl_client_init#130] : Loading the CA root certificate ...
 [   0.000]<I> HAL_TLS [_ssl_client_init#138] :  ok (0 skipped)
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#327] : Connecting to /10.101.12.205/9999...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#342] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#347] :   . Setting up the SSL/TLS structure...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#360] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#401] : Performing the SSL/TLS handshake...
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#411] :  ok
 [   0.000]<I> HAL_TLS [_TLSConnectNetwork#415] :   . Verifying peer X.509 certificate..
 [   0.000]<I> HAL_TLS [_real_confirm#77] : certificate verification result: 0x04
[inf] send_callback(58): send_callback data len 24, session->remote_window_size=16777215!
[inf] send_callback(67): send_callback data ends len = 24!
[inf] send_callback(58): send_callback data len 9, session->remote_window_size=16777215!
[inf] send_callback(67): send_callback data ends len = 9!
[dbg] http2_stream_node_search(186): stream node not exist, stream_id = 0
on_stream_frame_send|124 :: ~~~~~stream_id = 0 user_data =(nil), type = 4

http2_stream_test|172 :: ----------usr_data =0x80b6f40

[inf] send_callback(58): send_callback data len 326, session->remote_window_size=16777215!
[inf] send_callback(67): send_callback data ends len = 326!
[inf] on_frame_send_callback(134): [INFO] C --------> S (HEADERS) stream_id [1]
[inf] on_frame_send_callback(136): > :method: POST
[inf] on_frame_send_callback(136): > :path: /stream/open/iotx/vision/voice/intercom/live
[inf] on_frame_send_callback(136): > :scheme: https
[inf] on_frame_send_callback(136): > x-auth-name: devicename
[inf] on_frame_send_callback(136): > x-auth-param-client-id: a1L5EUOh21s.zhangmei_test01
[inf] on_frame_send_callback(136): > x-auth-param-signmethod: hmacsha1
[inf] on_frame_send_callback(136): > x-auth-param-product-key: a1L5EUOh21s
[inf] on_frame_send_callback(136): > x-auth-param-device-name: zhangmei_test01
[inf] on_frame_send_callback(136): > x-auth-param-sign: c1001f64f30b716073a2b57b0b063c0f69f34da7
[inf] on_frame_send_callback(136): > x-sdk-version: 230
[inf] on_frame_send_callback(136): > x-sdk-version-name: 2.3.0
[inf] on_frame_send_callback(136): > x-sdk-platform: c
[inf] on_frame_send_callback(136): > content-length: 0
[inf] on_frame_send_callback(136): > test_name: test_http2_header
[inf] on_frame_send_callback(136): > x-for-test: hello world
[dbg] http2_stream_node_search(186): stream node not exist, stream_id = 1
on_stream_frame_send|124 :: ~~~~~stream_id = 1 user_data =(nil), type = 1

```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [http2_connect](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/H2_Stream)

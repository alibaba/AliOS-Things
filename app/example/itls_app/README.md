# itls sample
  
It is a sample for using itls lib in alios things. 
## Contents

```sh
itls_app
└── itls_client.c        # iTLS test code
```

## Introduction

The **itls** example is an test code for iTLS , **itls** will connect to aliyun, sending and receiving ome data from server.

### Requirements

The **itls** example need to connect network, so device must have the wifi module or ethernet.
The **itls** example need to use id2 do mutual-authentication, so device must make id2 field or factory provisioned.

### Features

* iTLS test

### Dependencies

* cli
* id2
* alicrypto
* netmgr

### Supported Boards

- mk3060
- mk3080
- linuxhost

## Usage

once flash **itls** into device, you need to config device connect to wifi, input this command on board serial console:

```sh
netmgr connect <wifi-name>  <wifi-passwd>
```

### Result

```
# netmgr connect xxw xiaowen6814
[046760]<I> Will connect via at cmd: AT+WJAP=xxw,xiaowen6814

[046790]<I> AT command AT+WJAP=xxw,xiaowen6814 succeed, rsp:

OK
......

[064130]<V> [064130]<I> Hello, WiFi GOT_IP event! at board/developerkit/aos/board_cli.c 200
mac result: e47deb1419df

===========> iTLS Client Sample start.
  . Connecting to tcp/a1WO4Z9qHRw.itls.cn-shanghai.aliyuncs.com/1883... ok
  . Setting up the SSL/TLS structure... ok

  ...

  . Performing the SSL/TLS handshake...mbedtls_parse_auth_code_ext 435:   . Verify iTLS Server authCode OK!
 ok
  > Write to server: 170 bytes written

  < Read from server: 4 bytes read

<=========== iTLS Client Sample End.
```


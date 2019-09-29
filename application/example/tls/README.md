# tls sample
  
It is a sample for using tls lib in alios things. 
## Contents

```sh
tls
├── dtls_client.c       # DTLS test code(abandon)
└── tls_client.c        # TLS test code
```

## Introduction

The **tls** example is an test code for TLS , **tls** will connect `alink.tcp.aliyun.com` with port 443, sending and receiving ome data from server.

### Requirements

The **tls** example need to connect network(`alink.tcp.aliyun.com`), so device must have the wifi module or ethernet.

### Features

* TLS test

### Dependencies

* mbedtls
* cli
* alicrypto
* netmgr

### Supported Boards

- mk3060
- mk3080
- linuxhost

## Usage

once flash **tls** into device, you need to config device connect to wifi, input this command on board serial console:

```sh
netmgr connect <wifi-name>  <wifi-passwd>
```

### Build & Flash

```sh
# generate tls@mk3060 default config
aos make tls@mk3060 -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
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

===========> TLS Client Sample start.
[066280]<I> Server conn (0) successful.
network socket create ok
mbedtls ssl connect ok
mbedtls ssl send ok
mbedtls ssl recv ok
[069530]<I> Server closed event.
[069530]<I> Server conn (0) closed.
<=========== TLS Client Sample End.
```

## Reference

* [mbedtls](https://tls.mbed.org/)
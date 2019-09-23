## Contents

```shell
├── sal_import.h
├── sal.mk
├── include
│     ├── sal_arch.h
│     ├── sal_def.h
│     ├── sal_err.h
│     ├── sal_ipaddr.h
│     ├── sal_sockets.h
│     └── internal
│          ├── sal_arch_internal.h
│          ├── sal_pcb.h
│          ├── sal_sockets_internal.h
│          └── sal_util.h
└── src
     ├── ip4_addr.c
     ├── sal_arch_aos.c
     ├── sal_arch.c
     ├── sal_err.c
     ├── sal_sockets.c
     └── sal_util.c
```

## Introduction

`sal` is short for Socket Adapter Layer. It provides standard tcp/udp client socket interfaces, which can work over communitcation tunnels. For example, a MCU chip uses AT tunnel from another communication chip. In this case, `sal` can be used on MCU side by upper socket-based applications.

### Features

- **Asynchronized transmission**.  `athost` provides synchronized/asynchronized transmission mode. In synchonrized mode, send function will be blocked until lower layer returns, while asynchronized mode send function returns immediately.

- **UDP client**.  `athost` provides both tcp and udp client function. UDP client can be disabled to reduced code size.

### Dependencies
None

## API

### socket

Call this API to create socket.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### connect

Call this API to sep up a TCP connection.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### select

Call this API to poll socket status.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### gethostbyname

Call this API to interpret domain address.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### send

Call this API to send data to remote through a connection.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.


### recv

Call this API to receive data from remote through a connection.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### sendto

Call this API to send data to remote with specified address.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### recvfrom

Call this API to recv data from remote with address information returned.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### close

Call this API to close a socket.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

## HAL

### HAL_SAL_Init

Implement this HAL to initialize communicaiton tunnel.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### HAL_SAL_Deinit

Implement this HAL to deinitialize communicaiton tunnel.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### HAL_SAL_Start

Implement this API to set up a tcp/udp client connection on communication module.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### HAL_SAL_Close

Implement this HAL to close a tcp/udp client connection on communication module.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### HAL_SAL_DomainToIpip

Implement this HAL to interpret domain address.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### HAL_SAL_RegisterNetconnDataInputCb

Implement this HAL to register callback function for data input.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.


## Reference

None.



# README.md: athost

## Contents

```shell
├── src
│   ├── atcmd
│   │     ├── at_coap.c
│   │     ├── at_mqtt.c
│   │     ├── at_tcp_udp.c
│   │     ├── at_wifi_mgmt.c
│   │     └── at_wifi_ywss.c
│   ├── at_util.c
│   ├── at_util.h
│   └── athost.c
├── athost.mk
├── athost_export.h
└── athost_import.h
```

## Introduction

`athost` is an AT component for receiving and processing AT command sets. It provides AT commands such as ICA COAP, ICA MQTT, tcp/udp client, WiFi management, etc.

### Features

- **ICA MQTT**.  `athost` provides ICA MQTT AT commands, including MQTT Parameter setting, Connect/Disconnect, Publish, Subscribe/Unsubsribe, etc.
- **ICA COAP**.  `athost` provides ICA COAP AT commands, including  COAP Parameter setting, Send request with/without ack.

- **TCP/UDP client**. `athost` provides TCP/UDP client AT commands, including start/stop connection, domain to ip, send. 

### Dependencies

- linkkit_sdk_c

## API

### athost_instance_init

Call this API to initialize athost component.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

## Reference

None.

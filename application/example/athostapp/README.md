## Contents

```shell
├── atcmd_config_module.h
├── atcmd_config_platform.h
├── athostapp.c
└── athostapp.mk
```

## Introduction

`athostapp` is an example for receiving and processing AT command sets. It provides AT commands such as ICA MQTT/COAP, tcp/udp client, WiFi management, etc.

Compile command example:

```sh
# generate athostapp@mk3080 default config
aos make athostapp@mk3080 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Features

- **ICA MQTT**.  `athost` provides ICA MQTT AT commands, including MQTT Parameter setting, Connect/Disconnect, Publish, Subscribe/Unsubsribe, etc. This feature is enabled by default.
- **ICA COAP**.  `athost` provides ICA COAP AT commands, including  COAP Parameter setting, Send request with/without ack. To enable COAP, use menuconfig to enable this feature.
- **TCP/UDP client**. `athost` provides TCP/UDP client AT commands, including start/stop connection, domain to ip, send. This feature is enabled by default.
- **WiFi management**. `athost` provides WiFi management AT commands, including set ssid & password, get ip/mac address. This feature is enabled by default.

### Dependencies

- network/sal/athost

### Supported Boards

- mk3060

## Reference

None.

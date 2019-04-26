# README.md: netmgr

## Contents

```shell
├── src
│   ├── netmgr.c
│   ├── netmgr_wifi.c
│   ├── netmgr_net.c
│   └── netmgr_cellular.c
├── hal
│   ├── net.c
│   ├── wifi.c
│   └── cellular.c
├── include
│   ├── netmgr.h
│   ├── netmgr_wifi.h
│   └── netmgr_cellular.h
├── aos.mk
└── ucube.py
```

## Introduction

`netmgr` is a key component for the network provisioning process. It provides a way to start the provisioning process, as well as handling network events, including IP got event, network state (connect and disconnect) event, etc.

### Features

- **Start/Stop of Network Provision Process**.  `netmgr` provides APIs to start and stop network provisioning process. The network can be flexiblely custimized to be WiFi, NB-IoT, etc. 
- **Network State Event Handling**.  `netmgr` also manages the network state change and events handling during the network provision process. The events handled by `netmgr`, in WiFi case, includes: `STATION_UP`, `STATION_DOWN`, `ON_CONNECTED`, `ON_DISCONNECT` and `ON_GOT_IP`.
- **WiFi SSID Persistant Storage**. `netmgr` is also reponsible for saving WiFi SSID/password information in persistant storage, e.g. flash,  upon network connection during the WiFi provisioning process. `netmgr` is also responsible for using the WiFi information stored in persistant storage to connect AP when devices is already provisioned before. 
- **Connect to the specified network with given ssid and password**.  `netmgr` provides APIs to connect the specified WiFi network with given SSID and password.

### Dependencies

- halwifi
- kernel.fs.kv
- yloop

## API

### netmgr_init

Call this API to initialize the netmgr module.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### netmgr_deinit

Call this API to de-initialize the netmgr module.

**Arguments**

None.

**Return**

None.

### netmgr_start

**Arguments**

| name       | type | description                                                  |
| ---------- | ---- | ------------------------------------------------------------ |
| autoconfig | bool | Flag used to indicate if to start the provision process automatically or not. |

**Return**

`int`

| return | description                                                  |
| ------ | ------------------------------------------------------------ |
| 0      | Successfully to start the network provision process.         |
| 1      | Device has already be provisioned and it's successful to start AP connection. |
| -1     | Failure.                                                     |

### netmgr_set_ap_config

This API sets the AP information (SSID, password, bssid) in memory which is to be used by netmgr module.

**Arguments**

| name   | type               | description                   |
| ------ | ------------------ | ----------------------------- |
| config | netmgr_ap_config_t | The AP information to be set. |

**Return**

`int`

| return | description                             |
| ------ | --------------------------------------- |
| 0      | Successfully to set the AP information. |
| other  | Failure.                                |

###  netmgr_get_ap_config

This API fetches and saves the AP information (SSID, password, bssid, etc.) in memory which is to be used by netmgr module.

**Arguments**

| name   | type               | description                                   |
| ------ | ------------------ | --------------------------------------------- |
| config | netmgr_ap_config_t | The address of the AP information to save to. |

**Return**

`int`

| return | description                                      |
| ------ | ------------------------------------------------ |
| 0      | Successfully to set the AP information.          |
| -1     | Failure, i.e. no valid AP information available. |

### netmgr_clear_ap_config

This API sets the AP information (SSID, password, bssid) in memory which is to be used by netmgr module.

**Arguments**

None.

**Return**

None.

### netmgr_set_smart_config

This API is called when there is a instance of netmgr module is to be registered. The instance is who does the provision work in real.

**Arguments**

| name   | type                | description          |
| ------ | ------------------- | -------------------- |
| plugin | autoconfig_plugin_t | The plugin instance. |

**Return**

None.

### netmgr_register_wifi_scan_result_callback

This API registers the callback function which is called when WiFi scan result is available in netmgr module.

**Arguments**

| name | type                         | description            |
| ---- | ---------------------------- | ---------------------- |
| cb   | netmgr_wifi_scan_result_cb_t | The callback funciton. |

**Return**

None.

### netmgr_get_scan_cb_finished

This API returns the result whether scan callback has been successfully called by netmgr module.

**Arguments**

None.

**Return**

`bool`

| return | description                        |
| ------ | ---------------------------------- |
| true   | Scan callback is already finished. |
| false  | Callback not finished yet.         |

### netmgr_get_ip_state

This API returns whether IP is available or not.

**Arguments**

None.

**Return**

`bool`

| return | description              |
| ------ | ------------------------ |
| true   | IP is already available. |
| false  | IP is not ready yet.     |

### netmgr_reconnect_wifi

Call this API to start connecting to AP when WiFi SSID/password information is ready.

**Arguments**

None.

**Return**

None.

### netmgr_wifi_get_ip

This API is used to obtained the IP string from netmgr module.

**Arguments**

| name | type   | description                                                  |
| ---- | ------ | ------------------------------------------------------------ |
| ip   | char * | The memory to hold the IP string result, in dot string format (e.g. 192.168.100.111). |

**Return**

None.

## Reference

None.

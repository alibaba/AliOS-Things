# README.md: uND (Network Diagnosis)

## Contents

```shell
├── hal
│   ├── und_ble.c
│   └── und_wifi.c
├── include
│   ├── und_config.h
│   ├── und.h
│   ├── und_hal.h
│   ├── und_log.h
│   ├── und_packet.h
│   └── und_types.h
├── src
│   ├── und_adapter.c
│   ├── und.c
│   ├── und_manage.c
│   ├── und_packet.c
│   └── und_sched.c
└── ucube.py
```

## Introduction

`uND` is one module to monitor the wireless information and connection exception and report the statistics to cloud.

### Features

- **Collect Statistic of Network Exception and Wireless Information**.  `uND` provides APIs for targets capture to update exception detected.
- **Report Statistic to Cloud for Operation**.  `uND` reports statistic of exception and wireless information to cloud for operation.
- **Persistant Storage for Statistic of Network Exception**. `uND` is also reponsible for saving statistic of network exception in persistant storage, e.g. flash.

### Dependencies

- halwifi
- kernel.fs.kv
- yloop (Timer)

## API

### und_init

Call this API to initialize the uND module.

**Arguments**

None.

**Return**

`0` on success, otherwise failure.

### und_deinit

Call this API to de-initialize the uND module.

**Arguments**

None.

**Return**

None.

### und_update_statis

**Arguments**

| name       | type | description                                                  |
| ---------- | ---- | ------------------------------------------------------------ |
| cap_idx    | int  | the index of capture targets                                 |
| reason     | int  | the reason code about network exception                      |

**Return**

`int`

| return | description                                                  |
| ------ | ------------------------------------------------------------ |
| 0      | Successfully to update statis of network exception.          |
| otherwise      | failure of upate operation.                          |


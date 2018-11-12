## Contents

## Introduction
**kv** is a lightweight persistent stroage module which is based on key-value style. It provides convenient interface for MCU (Micro Control Unit). The developers can achieve more efficient and common application development based on flash memory. The storage module support wear-leveling and power fail safeguard, also have a low footprint.

### Features
- key-value pair set
- key-value pait get
- key-value pair delete
- key-value pair delete for group (same prefix)

### Directories

```sh
kv
├── include
│   ├── kv_adapt.h # kv OS adapt layer declaration
│   ├── kv_api.h   # kv module interface declaration
│   ├── kv_conf.h  # kv configuration
│   └── kv_err.h   # kv errno defination
├── kv_adapt.c     # kv os adapt layer implementation
├── kv.c           # kv core logic
├── kv_cli.c       # kv CLI command
├── kv_types.h     # kv internal data structure defination

```

### Dependencies

## Reference
- [Flash and KV Storage Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/Flash-and-KV-Storage-Porting-Guide)

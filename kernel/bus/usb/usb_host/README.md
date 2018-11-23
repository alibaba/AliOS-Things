## Contents

## Introduction
**usb host** is a simple USB host library module. the USB host library sits on top of the USB host HAl driver. This library offers the APIs used to access USB devices of various classes

### Features
- Mass-storage class
- Remote NDIS

### Directories

```sh
usb_host
├── class
│   ├── msd
│   │   ├── include
│   │   │   └── usb_host_msd.h          # Mass-storage class defination
│   │   └── src
│   │       └── usb_host_msd.c          # Mass-storage class implementation
│   └── rndis
│       ├── include
│       │   ├── usb_host_rndis_common.h # Remote NDIS class defination
│       │   └── usb_host_rndis.h        # Remote NDIS class structure defination
│       └── src
│           └── usb_host_rndis.c        # Remote NDIS class implementation
├── core
│   ├── include
│   │   ├── usb_host_core.h             # USB host core layer defination
│   │   ├── usb_host_debug.h            # USB host debug interface declaration
│   │   ├── usb_host_device.h           # USB host device layer declaration
│   │   ├── usb_host.h                  # USB host common structure defination
│   │   ├── usb_host_os.h               # USB host OS adapt layer declaration
│   │   ├── usb_host_pipe.h             # USB host pipe declaration
│   │   ├── usb_host_request.h          # USB host standard request declaration
│   │   ├── usb_host_spec.h             # USB host spec defination
│   │   ├── usb_host_transfer.h         # USB host transfer declaration
│   │   └── usb_host_types.h            # USB host common type defination
│   └── src
│       ├── usb_host_core.c             # USB host core layer implementation
│       ├── usb_host_device.c           # USB host device layer implementation
│       ├── usb_host_os.c               # USB host OS adapt layer implementation
│       ├── usb_host_pipe.c             # USB host pipe implementation
│       ├── usb_host_request.c          # USB host standard request implementation
│       └── usb_host_transfer.c         # USB host transfer implementation
└──usb_host_conf.h                      # USB host configuration

```

## Reference

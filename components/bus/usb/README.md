## Contents

## Introduction
**usb device** is a simple USB device library module. the USB device library sits on top of the USB host HAl driver. This library offers the APIs required to develop a USB device application.

**usb host** is a simple USB host library module. the USB host library sits on top of the USB host HAl driver. This library offers the APIs used to access USB devices of various classes

### Features
- Mass-storage class
- Remote NDIS
- HID class

### Directories

```sh
usb_host
├── class
│   ├── msd
│   │   └── src
│   │       └── usb_host_msd.c          # Mass-storage class implementation
│   └── rndis
│       ├── include
│       │   ├── usb_host_rndis_common.h # Remote NDIS class defination
│       └── src
│           └── usb_host_rndis.c        # Remote NDIS class implementation
├── core
│   ├── include
│   │   ├── usb_host_core.h             # USB host core layer defination
│   │   ├── usb_host_debug.h            # USB host debug interface declaration
│   │   ├── usb_host_device.h           # USB host device layer declaration
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

usb_device
├── class
│   ├── hid
│   │   ├── include
│   │   │   ├── usb_device_hid.h       # USB HID device interface declaration
│   │   │   ├── usb_device_hid_types.h # USB HID device structure defination
│   │   │   └── usb_device_mouse.h     # USB HID mouse interface declaration
│   │   └── src
│   │       ├── usb_device_hid.c       # USB HID device implementation
│   │       └── usb_device_mouse.c     # USB HID mount implementation
│   └── msd
│       ├── include
│       │   └── usb_device_msd.h       # USB MSD device interface declaration
│       └── src
│           └── usb_device_msd.c       # USB MSD device implementation
├── core
│   ├── include
│   │   ├── usb_device_debug.h         # USB device debug declaration
│   │   ├── usb_device_descriptor.h    # USB device descriptor defination
│   │   ├── usb_device_endpoints.h     # USB device endpoint defination
│   │   ├── usb_device.h               # USB device structure defination
│   │   ├── usb_device_os.h            # USB device OS adapt layer declaration
│   │   └── usb_device_types.h         # USB device types defination
│   └── src
│       └── usb_device.c               # USB device core layer implementation
└── README.md

```

## Reference

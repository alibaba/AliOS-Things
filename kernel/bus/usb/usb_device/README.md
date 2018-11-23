## Contents

## Introduction
**usb device** is a simple USB device library module. the USB device library sits on top of the USB host HAl driver. This library offers the APIs required to develop a USB device application. 

### Features
- Mass-storage class
- HID class

### Directories

```sh
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

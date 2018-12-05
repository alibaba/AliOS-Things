# BLE Mesh Tmall Profile

## Contents

```shell
├── aos.mk
├── CMakeLists.txt
├── Config.in
├── hal
│   ├── ccm.h
│   ├── cntr.h
│   ├── cpu.h
│   ├── debug.h
│   ├── device.c
│   ├── ecb.h
│   ├── nrf5
│   │   ├── cntr.c
│   │   ├── debug.h
│   │   ├── ecb.c
│   │   ├── radio.c
│   │   └── rand.c
│   ├── radio.h
│   └── rand.h
├── hci
│   ├── hci.c
│   ├── hci_driver.c
│   └── hci_internal.h
├── include
│   ├── linker-defs.h
│   └── ll.h
├── Kconfig
├── ll_sw
│   ├── crypto.c
│   ├── ctrl.c
│   ├── ctrl.h
│   ├── ctrl_internal.h
│   ├── ll_adv.c
│   ├── ll_adv.h
│   ├── ll.c
│   ├── ll_filter.c
│   ├── ll_filter.h
│   ├── ll_master.c
│   ├── ll_scan.c
│   ├── ll_test.c
│   ├── ll_test.h
│   └── pdu.h
├── README.md
├── ticker
│   ├── ticker.c
│   └── ticker.h
└── util
    ├── mayfly.c
    ├── mayfly.h
    ├── mem.c
    ├── mem.h
    ├── memq.c
    ├── memq.h
    ├── util.c
    └── util.h
```

## Introduction

This component includes the bluetooth contoller.

## Features

- Bluetooth controller features.

## API

Please refer to APIs in header file `bluetooth/hci.h`.

## Reference

None.

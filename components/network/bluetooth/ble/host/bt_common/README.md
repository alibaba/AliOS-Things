# BLE Common Utilities

## Contents

```shell
├── atomic_c.c
├── bt_common.mk
├── buf.c
├── CMakeLists.txt
├── Config.in
├── dummy.c
├── include
│   ├── arch
│   ├── atomic.h
│   ├── common
│   ├── errno.h
│   ├── irq.h
│   ├── linker
│   ├── misc
│   ├── net
│   ├── soc.h
│   ├── sw_isr_table.h
│   ├── toolchain
│   ├── toolchain.h
│   ├── work.h
│   └── zephyr
├── Kconfig
├── log.c
├── poll.c
├── port
│   ├── aos_port.c
│   └── include
├── rpa.c
├── tinycrypt
│   ├── include
│   ├── Kconfig
│   ├── README
│   └── source
└── work.c
```

## Introduction

This component includes the common used utilities, e.g. k_queue, k_fifo, k_timer, which is used by Bluetooth components (such as BT stack and BT Mesh stack).

## Features

- k_queue
- k_sem
- k_mutex
- k_thread
- k_timer
- net_buf
- tinycrypt (AES encrypt/decrypt, sha256, ecc, etc.)

## API

Please refer to below header files:

```shell
include/atomic.h
include/work.h
include/net/buf.h
tinycrypt/include/tinycrypt/*.h
```

## Reference

None

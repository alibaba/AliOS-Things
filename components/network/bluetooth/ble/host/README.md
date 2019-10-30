# README.md: BT

## Contents

```shell
.
├── bt.mk       //mk for bt component.
├── bt_common   //common component for host stack, including buffer, log, and porting layer with OS.
├── controller  //controller implementation, now specific for nRF52xxx serious controller.
├── hci_driver  //hci driver implementation.
├── host  //host stack.
└── include  //host stack head files.
    ├── bluetooth  //stack head files.
    └── drivers  //hci driver head files.
```

## Introduction

Bt is a highly configurable Bluetooth Low Energy(BLE) stack in compliance with Bluetooth core spec 4.0/4.2/5.0. It offers HCI(H4 or virtual HCI) layer abstraction depending on hardware controller's support and defines clear and common Generic Attribute Profile(GATT) and Generic Access Profile(GAP) APIs for upper application.  

### Features

- **GAP**.  Fully support for BLE Peripheral, Central, Observer and Advertister.
- **GATT**. Fully support for GATT Client and GATT server. 
- **SMP**.  Fully support for BLE SMP feature, including Secure Connection.
- **HCI layer abstraction**. Standard H4 layer for Host or virtual HCI layer for controller(e.g. nRF52832/nRF52832).
- **Configurable parameters and features**. Parameters such as max connection numbers, Tx/Rx buffer counts, features as GAP roles, GATT roles, LOG module can all be reconfigured. 

### Dependencies

- **OS**. Example can refer to `bt_common/port/aos_port.c`  within if AliOS Things is used. Other OSs are also designed to be supported.
- **HCI driver**. Example can refer to `hci_drivers/h4.c`  to support standard HCI host or refer to 'controller/hci/hci_driver.c' for controller virtual HCI implementaion.

## API

Please refer to  [bluetooth](https://yuque.antfin-inc.com/aliosthings/pubdoc/doc.components.network.bluetooth) for API details.

## Reference

None.

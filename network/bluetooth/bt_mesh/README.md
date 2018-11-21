# BLE Mesh

## Contents

```sh
├── bt_mesh.mk
├── Config.in
├── inc
│   ├── access.h
│   ├── adv.h
│   ├── api
│   │   ├── mesh
│   │   │   ├── access.h
│   │   │   ├── cfg_cli.h
│   │   │   ├── cfg_srv.h
│   │   │   ├── health_cli.h
│   │   │   ├── health_srv.h
│   │   │   ├── main.h
│   │   │   └── proxy.h
│   │   └── mesh.h
│   ├── beacon.h
│   ├── foundation.h
│   ├── friend.h
│   ├── lpn.h
│   ├── mesh_crypto.h
│   ├── mesh.h
│   ├── net.h
│   ├── prov.h
│   ├── proxy.h
│   ├── test.h
│   └── transport.h
├── port
│   ├── adv
│   │   ├── bt_mesh_adv.c
│   │   └── bt_mesh_adv.h
│   ├── aos.mk
│   ├── crypto
│   │   ├── bt_mesh_crypto.c
│   │   └── bt_mesh_crypto.h
│   └── gatt
│       ├── bt_mesh_gatt.c
│       └── bt_mesh_gatt.h
├── README.md
├── src
│   ├── access.c
│   ├── adv.c
│   ├── beacon.c
│   ├── cfg_cli.c
│   ├── cfg_srv.c
│   ├── crypto.c
│   ├── friend.c
│   ├── health_cli.c
│   ├── health_srv.c
│   ├── lpn.c
│   ├── main.c
│   ├── net.c
│   ├── prov.c
│   ├── proxy.c
│   ├── shell.c
│   ├── test.c
│   └── transport.c
└── TODO
```

## Introduction

This component is an implementation of BLE Mesh stack.

## Features

- BLE Mesh Stack Implementation.
- BLE Mesh Access Layer APIs.
- BLE Mesh Foundation Models (BLE Mesh Configuration Server/Client Model, BLE Mesh Health Server/Client Model).
- BLE  Mesh Proxy APIs.
- BLE Mesh Profile APIs.

## Dependencies

- BLE Mesh HAL component, e.g. AliOS Things BLE stack based implementation component (`network.bluetooth.bt_mesh.port`).
- OS basis, including queue, semerphor, mutex, thread, timer, etc. Please refer to AliOS Things based implementation (`network.bluetooth.bt.bt_common`).

## API

Please refer to BLE Mesh APIs in `inc/api/mesh/` directory.

## Usage

1. Compile the example code:

   - nrf52832
     $ `aos make bluetooth.blemesh_cli@pca10040`

   - nrf52840
     $ `aos make bluetooth.blemesh_cli@pca10056`

2. Program the dev board with elf images:

   - nrf52832
     $` JLinkGDBServer -if swd -device nRF52832_xxAB`

   - nrf52840
     $` JLinkGDBServer -if swd -device nRF52840_xxAA`

   ```shell
   $ arm-none-eabi-gdb bluetooth.blemesh_cli\@pca10040.elf
    (gdb) target remote 127.0.0.1:2331`
    (gdb) load
    (gdb) monitor reset
    (gdb) c
   ```

3. Connect to the dev board via serial tool (e.g. `minicom`).

4. Verify the mesh provisioning features with `bluez`:

   ```shell
   $ sudo ./meshctl
   [meshctl]# discover-unprovisioned on
   [meshctl]# provision dddd
   <check the output OOB number displayed on the serial terminal of the board and fill it in below line.>
   [meshctl] Enter Numeric key: 4240
   <wait for a few seconds for the device to be connected to Mesh network.>
   ```

## Reference

None


# BLE Mesh Tmall Profile

## Contents

```shell
├── aos.mk
├── bt_mesh_profile.c
├── bt_mesh_profile_config.h
├── bt_mesh_profile.h
├── Config.in
└── README.md
```

## Introduction

This component is an AliOS Things BLE Mesh based implementation of Tmall profile.

## Features

- Tmall Profile Digest and UUID Construct.
- Tmall Profile Provision.

## API

Please refer to APIs in header file `bt_mesh_profile.h`.

## Usage

- **Overview**

This example demonstrates how to leverage the Alibaba ble mesh profile APIs to support provisioning with Tmall Genius (as provisioner) over PB-GATT bearer and join the bluetooth low energy mesh network. After
success provisioning and pairing, usr can control the light on/off status by speaking with Tmall Genius.

Alibaba ble mesh profile APIs are encapsulated based on ported Zephyr bluetooth mesh stack. If vendor has their own ble mesh stack, just need to reimplement the related APIs based on their own mesh stack instead.

The ble mesh profile APIs could be found in the directory: `/network/bluetooth/bt_mesh_profile/bt_mesh_profile.h`.

- **Prerequisites**

Apply for the triple-elements: `product id`, ` device name`,  `device key` , `product key` from Alibaba first. Among of these triple-elemetns, device name will be used to construct the uuid as mac address. And other elements will be used to calcualte the sha256 hash value of static oob information.

- **Steps**

1. Compile the source code and generate the elf image.

   nrf52840
   `$ aos make bluetooth.blemesh_tmall@pca10056`

   nrf52832
   `$ aos make bluetooth.blemesh_tmall@pca10040`

2. Program the dev board with generated elf image.

   nrf52840
   `$ JLinkGDBServer -if swd -device nRF52840_xxAA`

   nrf52832
   `$ JLinkGDBServer -if swd -device nRF52832_xxAB`

3. Open another terminal:

   ```shell
   $ arm-none-eabi-gdb bluetooth.blemesh_tmall\@pca10040.elf
   (gdb) target remote 127.0.0.1:2331
   (gdb) monitor reset
   (gdb) c
   ```

   **Note**: you can also use nrfjprog tool to program the generated hex file.

4. Use minicom or other serial tools to connect to dev board.

5. Verify with Tmall genius (as provisioner):
   1) Say `天猫精灵` to wake up tmall genius smart speaker.
   2) Say `发现设备` to let tmall genius scan on the smart bluetooth devices nearby
   3) After tmall genius responses `嗯，已发现智能设备，是否连接？`, say `连接`, then tmall genius will establish gatt connection with dev board.
   4) After gatt connection has been established, say `天猫精灵，开灯 or 天猫精灵，关灯` to control the LED1's on/off status on the dev board.
   **Note**: Using LED1(p0.17) of nrf52832 pca10040 dev board for demo here.

## Reference

None.
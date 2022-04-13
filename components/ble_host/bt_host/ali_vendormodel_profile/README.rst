Overview
********

This example demonstrates how to leverage the Alibaba ble mesh
profile APIs to support provisioning with Tmall Genius (as provisioner)
over PB-GATT bearer and join the bluetooth low energy mesh network. After
success provisioning and pairing, usr can control the light on/off status
by speaking with Tmall Genius.

Alibaba ble mesh profile APIs are encapsulated based on ported Zephyr
bluetooth mesh stack. If vendor has their own ble mesh stack, just need to
reimplement the related APIs based on their own mesh stack instead.

The ble mesh profile APIs could be found in the directory:
/network/bluetooth/bt_mesh_profile/bt_mesh_profile.h

Prerequisites
*************

Apply for the triple-elements: <product id, device name, device key, product key>
from Alibaba first. Among of these triple-elemetns, device name will be used to
construct the uuid as mac address. And other elements will be used to calcualte
the sha256 hash value of static oob information.

Usage
*****

1. compile the source code and generate the elf image.

   nrf52840
   $ aos make bluetooth.blemesh_tmall@pca10056

   nrf52832
   $ aos make bluetooth.blemesh_tmall@pca10040

2. program the dev board with generated elf image.

   nrf52840
   $ JLinkGDBServer -if swd -device nRF52840_xxAA

   nrf52832
   $ JLinkGDBServer -if swd -device nRF52832_xxAB

   open another terminal
   $ arm-none-eabi-gdb bluetooth.blemesh_tmall\@pca10040.elf
   $ (gdb) target remote 127.0.0.1:2331
   $ (gdb) load
   $ (gdb) monitor reset
   $ (gdb) c
   note: you can also use nrfjprog tool to program the generated hex file.

3. use minicom or other serial tools to connect to dev board.

4. verify with Tmall genius (as provisioner)
   1) say "天猫精灵" to wake up tmall genius smart speaker
   2) say "发现设备" to let tmall genius scan on the smart bluetooth devices nearby
   3) after tmall genius responses "嗯，已发现智能设备，是否连接？", say "连接", then tmall genius will establish gatt connection with dev board.
   4) after gatt connection has been established, say "天猫精灵，开灯 or 天猫精灵，关灯" to control the LED1's on/off status on the dev board.
      note: Using LED1(p0.17) of nrf52832 pca10040 dev board for demo here.


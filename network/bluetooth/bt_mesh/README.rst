Source Code Structure Overview
******************************

bt_mesh/
+-- port
|     +-- adv
|     |     +-- bt_mesh_adv.h
|     |     +-- bt_mesh_adv.c
|     +-- crypto
|     |     +-- bt_mesh_crypto.h
|     |     +-- bt_mesh_crypto.c
|     +-- gatt
|     |     +-- bt_mesh_gatt.h
|     |     +-- bt_mesh_gatt.c
|     +-- os
+-- inc
|     +-- api
|     |     +-- mesh.h
|     |     +-- mesh
|     |     |   +-- access.h
|     |     |   +-- cfg_cli.h
|     |     |   +-- cfg_srv.h
|     |     |   +-- health_cli.h
|     |     |   +-- health_srv.h
|     |     |   +-- main.h
|     |     |   +-- proxy.h
|     +-- access.h
|     +-- adv.h
|     +-- beacon.h
|     +-- crypto.h
|     +-- foundation.h
|     +-- friend.h
|     +-- lpn.h
|     +-- mesh.h
|     +-- net.h
|     +-- prov.h
|     +-- proxy.h
|     +-- transport.h
+-- src
|     +-- access.c
|     +-- adv.c
|     +-- beacon.c
|     +-- cfg_cli.c
|     +-- cfg_srv.c
|     +-- crypto.c
|     +-- friend.c
|     +-- health_cli.c
|     +-- health_srv.c
|     +-- lpn.c
|     +-- main.c
|     +-- net.c
|     +-- prov.c
|     +-- proxy.c
|     +-- transport.c
+-- bt_mesh.mk
+-- README

Detailed description for each directory
***************************************
port: APIs between ble mesh layer and ble core stack are needed to port by the third party or developer. Including 4 sub-directories: adv, crypto, gatt and os.
    +-- adv: ble advertise and scan APIs.
    +-- crypto: aes128 encrypt, aes128-cmac, aes128-ccm, random generator and ECC key managements.
    +-- gatt: ble gatt connection APIs.
    +-- os: os layer APIs, such as timer, sem, mutex, FIFO, thread.

inc: bt mesh header file directory
    +-- api: abstracted from zephyr bluetooth directory and bt mesh directory, /bt/include/bluetooth/ and /bt/host/bt_mesh/

src: bt mesh source code, abstracted from zephyr sub-system bluetooth mesh directory.

Usage
******
1. compile the example code:

   nrf52832
   $ aos make bluetooth.blemesh_cli@pca10040

   nrf52840
   $ aos make bluetooth.blemesh_cli@pca10056

2. program the dev board with elf images

   nrf52832
   $ JLinkGDBServer -if swd -device nRF52832_xxAB

   nrf52840
   $ JLinkGDBServer -if swd -device nRF52840_xxAA

   $ arm-none-eabi-gdb bluetooth.blemesh_cli\@pca10040.elf
   $ (gdb) target remote 127.0.0.1:2331
   $ (gdb) load
   $ (gdb) monitor reset
   $ (gdb) c

3. connect to the dev board via serial tool (minicom)

4. verify the mesh provisioning features with bluez

   ssh to raspberrypi (already enabled bluez setup here)
   $ ssh pi@30.16.18.199 (pwd: raspberry)
   $ cd /home/pi/bluez-5.48/mesh/
   $ sudo ./meshctl
   $ [meshctl]# discover-unprovisioned on
   $ [meshctl]# provision dddd

   check the output OOB number appeared in the serial terminal of dev board and fill it
   $ [meshctl] Enter Numeric key: 4240

   wait a few seconds...

   dev board will obtain the network paramaters from provisioner and finish the provisioning like below:
   # OOB Number: 4240
     Provisioning completed!
     Net ID: 0
     Unicast addr: 0x0114.


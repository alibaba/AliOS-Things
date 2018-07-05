EN | [中文](AliOS-Things-SIG-BLE-mesh-Setup-Guides.zh)

# AliOS Things SIG BLE mesh Setup Guides

# Hardware preparation

One block of Raspberry PI3 

Two blocks of ESP32 Devkitc

Two NRF51822 development boards

# Software environment setup

Currently, Bluez operated in Raspberry PI 3 has been used as provisioner. If you want to operate meshctl, compilation option of Raspberry PI is needed to be modified. Some characteristics concerning safety should be selected to ensure the operation of meshctl. 

## Raspberry PI kernel

Compile Raspberry PI kernel in linux:

Download Raspberry PI source code:

`git clone https://github.com/raspberrypi/linux.git`

Set the path of cross compiler tool chain.

```
`luwang@ubuntu:~/raspberry-pi/linux$ export CCPREFIX=/home/luwang/raspberry-pi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf- 
luwang@ubuntu:~/raspberry-pi/linux$ ${CCPREFIX}gcc -v
Using built-in specs.
COLLECT_GCC=/home/luwang/raspberry-pi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-gcc
COLLECT_LTO_WRAPPER=/home/luwang/raspberry-pi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/../libexec/gcc/arm-linux-gnueabihf/4.8.3/lto-wrapper
Target: arm-linux-gnueabihf
Configured with: /cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/src/gcc-linaro-4.8-2014.01/configure --build=i686-build_pc-linux-gnu --host=i686-build_pc-linux-gnu --target=arm-linux-gnueabihf --prefix=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/install --with-sysroot=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/install/arm-linux-gnueabihf/libc --enable-languages=c,c++,fortran --disable-multilib --enable-multiarch --with-arch=armv6 --with-tune=arm1176jz-s --with-fpu=vfp --with-float=hard --with-pkgversion='crosstool-NG linaro-1.13.1-4.8-2014.01 - Linaro GCC 2013.11' --with-bugurl=https://bugs.launchpad.net/gcc-linaro --enable-__cxa_atexit --enable-libmudflap --enable-libgomp --enable-libssp --with-gmp=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static --with-mpfr=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static --with-mpc=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static --with-isl=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static --with-cloog=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static --with-libelf=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/.build/arm-linux-gnueabihf/build/static --enable-threads=posix --disable-libstdcxx-pch --enable-linker-build-id --enable-plugin --enable-gold --with-local-prefix=/cbuild/slaves/oorts/crosstool-ng/builds/arm-linux-gnueabihf-raspbian-linux/install/arm-linux-gnueabihf/libc --enable-c99 --enable-long-long --with-float=hard
Thread model: posix
gcc version 4.8.3 20140106 (prerelease) (crosstool-NG linaro-1.13.1-4.8-2014.01 - Linaro GCC 2013.11) 
luwang@ubuntu:~/raspberry-pi/linux$`

```

Here we get the .config file compiled by kernel from Raspberry PI. The mirror image installed on Raspberry PI is 2017-11-29-raspbian-stretch.img.

Input the following command on Raspberry PI to generate kernel configuration file

`sudo modprobe configs`

Copy the configuration file generated in linux and unzip it.

`scp pi@raspberrypi:/proc/config.gz PATH_TO_RASPBERRY_PI/linux gunzip -c config.gz > .config`

Modify the following configuration options in configuration file.

```
`CONFIG_CRYPTO_CMAC=y
CONFIG_CRYPTO_USER_API=y
CONFIG_CRYPTO_USER_API_HASH=y
CONFIG_CRYPTO_USER_API_SKCIPHER=y`

```

Generate the kernel

```
`cd PATH_TO_RASPBERRY_PI/linux
ARCH=arm CROSS_COMPILE=${CCPREFIX} make`

```

Generate kernel modules

`ARCH=arm CROSS_COMPILE=${CCPREFIX} INSTALL_MOD_PATH=../modules make modules_install`

Get the following log information

```
`  INSTALL sound/soc/codecs/snd-soc-tas5713.ko
  INSTALL sound/soc/codecs/snd-soc-tpa6130a2.ko
  INSTALL sound/soc/codecs/snd-soc-wm-adsp.ko
  INSTALL sound/soc/codecs/snd-soc-wm5102.ko
  INSTALL sound/soc/codecs/snd-soc-wm8731.ko
  INSTALL sound/soc/codecs/snd-soc-wm8741.ko
  INSTALL sound/soc/codecs/snd-soc-wm8804-i2c.ko
  INSTALL sound/soc/codecs/snd-soc-wm8804.ko
  INSTALL sound/soc/generic/snd-soc-simple-card-utils.ko
  INSTALL sound/soc/generic/snd-soc-simple-card.ko
  INSTALL sound/soc/snd-soc-core.ko
  INSTALL sound/usb/6fire/snd-usb-6fire.ko
  INSTALL sound/usb/caiaq/snd-usb-caiaq.ko
  INSTALL sound/usb/misc/snd-ua101.ko
  INSTALL sound/usb/snd-usb-audio.ko
  INSTALL sound/usb/snd-usbmidi-lib.ko
  INSTALL .modules/lib/firmware/emi26/loader.fw
  INSTALL .modules/lib/firmware/emi26/firmware.fw
  INSTALL .modules/lib/firmware/emi26/bitstream.fw
  INSTALL .modules/lib/firmware/emi62/loader.fw
  INSTALL .modules/lib/firmware/emi62/bitstream.fw
  INSTALL .modules/lib/firmware/emi62/spdif.fw
  INSTALL .modules/lib/firmware/emi62/midi.fw
  INSTALL .modules/lib/firmware/kaweth/new_code.bin
  INSTALL .modules/lib/firmware/kaweth/trigger_code.bin
  INSTALL .modules/lib/firmware/kaweth/new_code_fix.bin
  INSTALL .modules/lib/firmware/kaweth/trigger_code_fix.bin
  INSTALL .modules/lib/firmware/ti_3410.fw
  INSTALL .modules/lib/firmware/ti_5052.fw
  INSTALL .modules/lib/firmware/mts_cdma.fw
  INSTALL .modules/lib/firmware/mts_gsm.fw
  INSTALL .modules/lib/firmware/mts_edge.fw
  INSTALL .modules/lib/firmware/edgeport/boot.fw
  INSTALL .modules/lib/firmware/edgeport/boot2.fw
  INSTALL .modules/lib/firmware/edgeport/down.fw
  INSTALL .modules/lib/firmware/edgeport/down2.fw
  INSTALL .modules/lib/firmware/edgeport/down3.bin
  INSTALL .modules/lib/firmware/whiteheat_loader.fw
  INSTALL .modules/lib/firmware/whiteheat.fw
  INSTALL .modules/lib/firmware/keyspan_pda/keyspan_pda.fw
  INSTALL .modules/lib/firmware/keyspan_pda/xircom_pgs.fw
  INSTALL .modules/lib/firmware/cpia2/stv0672_vp4.bin
  INSTALL .modules/lib/firmware/yam/1200.bin
  INSTALL .modules/lib/firmware/yam/9600.bin
  DEPMOD  4.9.77-v7+
luwang@ubuntu:~/raspberry-pi/linux$ `

```

The upper half of log is omitted because of the length of the space. Remember that the version is 4.9.77-v7+.

In Linux machine, copy the generated kernel from Linux to Raspberry PI.

`cd PATH_TO_RASPBERRY_PI/linux/arch/arm/boot/zImage pi@RASPBARRY:/tmp/kernel_new.img`

In Linux machine, copy the generated modules from Linux to Raspberry PI.

```
`cd PATH_TO_RASPBERRY_PI/modules
tar czf modules.tgz *
scp modules.tgz pi@RASPBERRY:/tmp`

```

Move the copied kernel and modules to boot and lib.

```
`sudo cp /tmp/kernel_new.img /boot
sudo tar zxf /tmp/modules.tgz
sudo cp -rf /tmp/lib/firmware /lib/firmware
sudo cp -rf /tmp/lib/modules /lib/modules`

```

Start the configuration file config.tx in Raspberry PI. Add the following content at the last of the file, and order it to start in the newly generated kernel_new.img.

`kernel=kernel_new.img`

Restart Raspberry PI

`sudo shutdown -r now`

Make sure the kernel information has been updated to the newly generated one.

`uname -r 4.9.77-v7+`

Install the needed dependency.

```
`sudo apt-get update
sudo apt-get install -y libusb-dev libdbus-1-dev libglib2.0-dev libudev-dev libical-dev libreadline-dev autotools-dev automake libtool`

```

## json-c

Download json-c source code.

`git clone https://github.com/json-c/json-c.git`

Compile and install.

```
`cd PATH_TO_JSON_C
sh autogen.sh
./configure
make
sudo make install`

```

## Bluez

Download bluez-5.48.tar.gz from the following link, and unzip it.

```
`https://git.kernel.org/pub/scm/bluetooth/bluez.git
tar xvf bluez-5.48.tar.gz`

```

Modify the logic with which mesh GATT processes data segments. diff as follows:

```
`luwang@ubuntu:~/bluez$ git diff mesh/gatt.c
diff --git a/mesh/gatt.c b/mesh/gatt.c
index 9116a9d..8d564a9 100644
--- a/mesh/gatt.c
+++ b/mesh/gatt.c
@@ -393,7 +393,10 @@ static bool pipe_read(struct io *io, bool prov, void *user_data)
                        break;
 
                res = buf;
   -               mesh_gatt_sar(&res, len);
   +               len = mesh_gatt_sar(&res, len);
   +                if (len == 0) {
   +                    break;
   +                }
 
                if (prov)
                        prov_data_ready(node, res, len);`

```

Compile and install.

```
`cd PATH_TO_BLUEZ
./bootstrap
./configure --enable-mesh --enable-debug
make
sudo make install`

```

## Bluetooth controller

What Bluetooth controller uses is zephyr's bluetooth/hci_uart application, V1.10-branch。

Modify the baud rate to 115200.

```
`luwang@ubuntu:~/zephyr/samples/bluetooth/hci_uart$ git diff nrf5.conf
diff --git a/samples/bluetooth/hci_uart/nrf5.conf b/samples/bluetooth/hci_uart/nrf5.conf
index 4a507be..9aea95f 100644
 --- a/samples/bluetooth/hci_uart/nrf5.conf
 +++ b/samples/bluetooth/hci_uart/nrf5.conf
@@ -4,7 +4,7 @@ CONFIG_UART_CONSOLE=n
 CONFIG_GPIO=y
 CONFIG_SERIAL=y
 CONFIG_UART_INTERRUPT_DRIVEN=y
 -CONFIG_UART_NRF5_BAUD_RATE=1000000
 +CONFIG_UART_NRF5_BAUD_RATE=115200
 CONFIG_UART_NRF5_FLOW_CONTROL=y
 CONFIG_MAIN_STACK_SIZE=512
 CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE=51`

```

## Applications in server side and client side

Download AliOS-Things source code.

`git clone git@github.com:alibaba/AliOS-Things.git`

Compile to generate the applications in server side.

```
`cd PATH_TO_AOS
aos make bluetooth.blemesh_srv@esp32devkitc hci_h4=1`

```

Compile to generate the applications in client side.

`cd PATH_TO_AOS aos make bluetooth.blemesh_cli@esp32devkitc hci_h4=1`

Burn the generated mirror image to ESP32 through esptool.

# Setup of temperature monitoring network

The temperature monitoring network consists of three roles, provisioner, server and client. Server is the publisher of temperature information, and client is the subscriber to temperature information.

## Provisioning and Configuring Server node

In Raspberry PI, run the meshctl command line.

```
`pi@raspberrypi:~/bluez-5.48/mesh $ ./meshctl .
Local config directory not provided.
  netkeys = 1
  appkeys = 2
  provisioners = 1
On/Off client model: new binding 0001
[NEW] Controller B8:27:EB:B2:E7:4A raspberrypi [default]
[NEW] Device EC:60:BA:B5:36:D0 AOS Device
Service added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a
Char added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d:
Char added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000b:
Service added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service0006`

```

Give power to server node, and select the search mode for unprovisioned devices to find out new unprovisioned ones.

```
`[meshctl]# discover-unprovisioned on
SetDiscoveryFilter success
Discovery started
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: yes
		Mesh Provisioning Service (00001827-0000-1000-8000-00805f9b34fb)
			Device UUID: dddd0000000000000000000000000000
			OOB: 0000`

```

Authenticate the device.

```
`[meshctl]# provision dddd
Trying to connect Device EC:60:BA:B5:36:D0 AOS Device
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: no
Connection successful
Services resolved yes
Found matching char: path /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000b, uuid 00002adb-0000-1000-8000-00805f9b34fb
Found matching char: path /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d, uuid 00002adc-0000-1000-8000-00805f9b34fb
Start notification on /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d
Characteristic property changed /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d
AcquireNotify success: fd 7 MTU 65
Notify for Mesh Provisioning Out Data started
Open-Node: 0x1758dc8
Open-Prov: 0x1757598
Open-Prov: proxy 0x175a1c8
Initiated provisioning
Characteristic property changed /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000b
AcquireWrite success: fd 8 MTU 65
GATT-TX:	 03 00 10 
GATT-RX:	 03 01 01 00 01 00 00 04 00 08 00 00 00 
Got provisioning data (12 bytes)
	 01 01 00 01 00 00 04 00 08 00 00 00 
GATT-TX:	 03 02 00 00 02 03 04 
GATT-TX:	 03 03 09 da d3 a0 1e 4f 15 34 3c 1f 88 e2 1f 93 
GATT-TX:	 f6 38 9b 8a 2d 3a a0 49 7a 0b 01 c6 5c 61 d9 cc 
GATT-TX:	 49 be 26 78 5c 5c de c1 05 86 6f 5a 88 0d 93 b8 
GATT-TX:	 60 e2 77 67 3d 72 f4 46 b4 5f d3 b1 87 53 cd 96 
GATT-TX:	 fc ce 
GATT-RX:	 43 03 d2 68 da 6f fe cc f8 97 0e 99 84 e6 eb 93 
GATT-RX:	 15 15 aa 1d 15 5b 2a f2 e7 1f 4d c1 93 51 14 29 
GATT-RX:	 7e b7 75 0c 8d 5a 51 c8 14 29 8d 3a 73 de fa 95 
GATT-RX:	 09 42 7e 7f 0a be e2 ed e5 ff 27 43 26 f7 
GATT-RX:	 c3 ab 8b ef 6f 
Got provisioning data (65 bytes)
	 03 d2 68 da 6f fe cc f8 97 0e 99 84 e6 eb 93 15 
	 15 aa 1d 15 5b 2a f2 e7 1f 4d c1 93 51 14 29 7e 
	 b7 75 0c 8d 5a 51 c8 14 29 8d 3a 73 de fa 95 09 
	 42 7e 7f 0a be e2 ed e5 ff 27 43 26 f7 ab 8b ef 
	 6f 
Request decimal key (0 - 9999)
[AOS 1m[mesh] Enter Numeric key: `

```

See the output of server command line OOB.

```
`Initializing...
Bluetooth initialized
Server
Mesh initialized
attention_on()
OOB Number: 9023
attention_off()`

```

Type 9023 on Raspberry PI, process the following provision, and then print a long string of log.

```
`[AOS 1m[mesh] Enter Numeric key: 9023
GATT-TX:	 03 05 fe 7f b1 04 84 f6 e9 0d 9e c7 62 19 6d 1f 
GATT-TX:	 03 64 
GATT-RX:	 03 05 ca f5 8e 55 ff 1d 92 86 26 fa 8e 22 8c 33 
GATT-RX:	 69 80 
Got provisioning data (17 bytes)
	 05 ca f5 8e 55 ff 1d 92 86 26 fa 8e 22 8c 33 69 
	 80 
GATT-TX:	 03 06 d6 63 cd 7c e6 3b b5 9b 2b a3 f5 3e e6 7c 
GATT-TX:	 15 f1 
GATT-RX:	 03 06 7a 94 aa 7a 3b f3 1c 33 92 cf 8a 3c 86 ec 
GATT-RX:	 9d 0c 
Got provisioning data (17 bytes)
	 06 7a 94 aa 7a 3b f3 1c 33 92 cf 8a 3c 86 ec 9d 
	 0c 
Confirmation Validated
S-Key	 5e ee f5 44 72 cc 88 c7 3a 9a 56 db 11 4a d5 44 
S-Nonce	 82 b4 a8 f2 7b 9b 76 c7 72 8b 3f e2 07 
DevKey	 f8 a9 4d 78 25 b8 47 3d 0d 3d 20 50 91 45 db 20 
Data	 18 ee d9 c2 a5 6a dd 85 04 9f fc 3c 59 ad 0e 12 
Data	 00 00 00 00 00 00 05 01 00 
DataEncrypted + mic	 b7 8a f4 17 c3 e8 05 66 84 4d d6 5e 22 69 73 1c 
DataEncrypted + mic	 90 10 91 8c 5f f7 30 55 bf d1 1c 29 e9 d6 3e 78 
DataEncrypted + mic	 f1 
GATT-TX:	 03 07 b7 8a f4 17 c3 e8 05 66 84 4d d6 5e 22 69 
GATT-TX:	 73 1c 90 10 91 8c 5f f7 30 55 bf d1 1c 29 e9 d6 
GATT-TX:	 3e 78 f1 
GATT-RX:	 03 08 
Got provisioning data (1 bytes)
	 08 
Provision success. Assigned Primary Unicast 0100
Attempting to disconnect from EC:60:BA:B5:36:D0
Characteristic property changed /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d
Write closed
Service added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a
Char added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000b:
Char added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d:
Services resolved no
SetDiscoveryFilter success
Discovery started
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: yes

		Mesh Proxy Service (00001828-0000-1000-8000-00805f9b34fb)
		Identity for node 0100
Trying to connect to mesh
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: no
Connection successful
Services resolved yes
Found matching char: path /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000b, uuid 00002add-0000-1000-8000-00805f9b34fb
Found matching char: path /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d, uuid 00002ade-0000-1000-8000-00805f9b34fb
Start notification on /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d
Characteristic property changed /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d
AcquireNotify success: fd 7 MTU 65
Notify for Mesh Proxy Out Data started
Trying to open mesh session
GATT-RX:	 01 01 00 d4 76 79 43 3f db 10 4a 00 00 00 05 f4 
GATT-RX:	 0a 41 fa b0 af 32 0b 
iv_upd_state = IV_UPD_NORMAL
Mesh session is open
Characteristic property changed /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000b
AcquireWrite success: fd 8 MTU 65
GATT-TX:	 02 f4 9e 64 99 fe 13 cd c8 9b 0e 1c e5 61 82 cc 
GATT-TX:	 ab f5 83 70 
GATT-TX:	 00 f4 b3 a6 53 65 b2 a0 cd 7a 74 9e 95 1d b7 13 
GATT-TX:	 06 e6 48 68 d5 5c 
GATT-RX:	 02 f4 e7 02 fe d4 84 81 a3 32 e7 6e 4b 1b 74 4a 
GATT-RX:	 13 95 f5 61 7f eb 
Proxy Whitelist filter length: 0
GATT-RX:	 00 f4 60 4f af f8 ba 30 1f a6 70 d9 cb 5f 12 dd 
GATT-RX:	 8b 0b 71 6b 68 ad 00 90 07 cf ec e0 28 fe 
GATT-RX:	 00 f4 2a e4 87 bb f8 21 34 0d 45 4c c3 de 5b e3 
GATT-RX:	 21 bb 57 9a 34 94 5a de 4d e8 ca c8 91 cd 
GATT-RX:	 00 f4 b7 a0 67 ff da 92 a3 32 4c c2 c2 4e a5 5c 
GATT-RX:	 7f 12 c7 5f 02 bc 0b 23 9f 13 
	Composition data for node 0100 {
  "cid":"0002",
  "pid":"0000",
  "vid":"0000",
  "crpl":"0005",
  "features":{
    "relay":false,
    "proxy":true,
    "friend":false,
    "lpn":false
  },
  "elements":[
    {
      "elementIndex":0,
      "location":"0000",
      "models":[
        "0000",
        "0001",
        "0002",
        "1100"
      ]
    }
  ]
}
GATT-TX:	 00 f4 2b d6 e6 46 66 5b 65 31 1d 78 08 28 5d 16 
GATT-TX:	 aa e2 97 b6 18 84 1f 29 7f `

```

The above log can be divided as two stages. The first is to use provisioning connection to carry out provision, and the second is to establish mesh proxy connection, which will be used to transmit network configuration information.

When the log is completely printed, the server side will print the information that provision has completed.

```
`Provisioning completed!
Net ID: 0
Unicast addr: 0x0100`

```

Next is to transmit network configuration information based on the mesh proxy connection you just set up.

First, enter the menu.

```
`[AOS Devic-Node-0100]# menu config
Menu config:
Available commands:
-------------------
target <unicast>                                  Set target node to configure
composition-get [page_num]                        Get Composition Data
netkey-add <net_idx>                              Add network key
netkey-del <net_idx>                              Delete network key
appkey-add <app_idx>                              Add application key
appkey-del <app_idx>                              Delete application key
bind <ele_idx> <app_idx> <mod_id> [cid]           Bind app key to a model
ttl-set <ttl>                                     Set default TTL
ttl-get                                           Get default TTL
pub-set <ele_addr> <pub_addr> <app_idx> <per (step|res)> <re-xmt (cnt|per)> <mod id> [cid] 
						  Set publication
pub-get <ele_addr> <model>                        Get publication
proxy-set <proxy>                                 Set proxy state
proxy-get                                         Get proxy state
ident-set <net_idx> <state>                       Set node identity state
ident-get <net_idx>                               Get node identity state
relay-set <relay> <rexmt count> <rexmt steps>     Set relay
relay-get                                         Get relay
hb-pub-set <pub_addr> <count> <period> <features> <net_idx> Set heartbeati publish
sub-add <ele_addr> <sub_addr> <model id>          Subscription add
sub-get <ele_addr> <model id>                     Subscription get
app-get <ele_addr> <model id>                     Get App Keys
back                                              Return to main menu
version                                           Display version
quit                                              Quit program
exit                                              Quit program
help                                              Display help about this program`

```

Select configuring node, where you can see that the node ucast addr is 0x0100.

```
[AOS Devic-Node-0100]# target 0100

Configuring node 0100

[config: Target = 0100]# target 0100[config: Target = 0100]#

```

Configure app key.

```
`[config: Target = 0100]# target 0100[config: Target = 0100]# appkey-add 1
GATT-TX:	 00 f4 e5 ca a3 ba 03 7b db 7f a6 aa 7c 02 25 75 
GATT-TX:	 6b 2a cf d4 93 0a 02 c3 8d 22 7d 30 ee 21 
GATT-TX:	 00 f4 41 5a 17 27 bf 6f 70 59 1e 4e 60 2d be 4f 
GATT-TX:	 1e f9 0d 88 e1 d6 c1 50 f3 51 cc 6d 43 d8 
GATT-RX:	 00 f4 04 00 5a d6 cc af 78 b3 eb 70 e5 78 e9 1b 
GATT-RX:	 87 fc 43 8c b0 ad 90 a2 7a 
GATT-RX:	 00 f4 56 a5 24 11 02 10 62 67 45 42 77 77 3a 7a 
GATT-RX:	 ec 3e 8f 2f 26 56 4f d3 ba 
Node 0100 AppKey Status Success
	NetKey 000, AppKey 001`

将server节点的element 0的sensor server model (0x1100)绑定到app key index =1的app key上

`[config: Target = 0100]# bind 0 1 1100
GATT-TX:	 00 f4 cb 2a f4 45 0f 4d 25 b0 47 d8 15 72 ba 89 
GATT-TX:	 61 fc ec 15 7e fe 8c dc 30 e6 04 
GATT-RX:	 00 f4 af 08 09 a9 12 03 dd fa df 1a 5f e4 7e c1 
GATT-RX:	 4f c8 58 84 82 77 8c 62 d1 cb 59 f9 
Node 0100 Model App Status Success
	Element 0100 AppIdx 001
ModelId 1100`

```

Add the subscription address information 0xc000 of sensor server model 0x1100.

```
`[config: Target = 0100]# sub-add 0100 c000 1100
GATT-TX:	 00 f4 fe fa 07 e1 60 7a 1a ac d1 26 3a 45 8e f7 
GATT-TX:	 5e 8e ff 79 9d aa 90 10 ab b8 ef 
GATT-RX:	 00 f4 a8 c7 92 53 ee 9e 15 c7 fb 47 3e e8 07 fb 
GATT-RX:	 c4 ea 8a f7 2d 71 3a 33 da 12 e9 0f 

Subscription changed for node 0100 status: Success
Element Addr:	0100
Subscr Addr:	c000
Model ID:	1100`

```

Set the app key release information of sensor server model (0x1100). The release address is 0xc000.

```
`[config: Target = 0100]# pub-set 0100 c000 1 0 0 1100
GATT-TX:	 00 f4 d1 a1 54 ee 1b 9f 18 4e 3c 44 ab 57 f9 82 
GATT-TX:	 73 41 9e 9c ab 3d 5c dd ad 27 c3 18 b7 22 
GATT-TX:	 00 f4 d0 95 d8 43 e4 87 12 96 14 bf 52 55 5a 8a 
GATT-TX:	 ad 00 b0 4c b4 46 
GATT-RX:	 00 f4 34 4f 18 3f cf 94 bd 90 49 6a 6d 0e 83 78 
GATT-RX:	 0c b8 7c 14 00 7e 3f b6 fa 
GATT-RX:	 00 f4 66 8c 52 14 1b f9 b8 e2 d9 27 17 0d b0 87 
GATT-RX:	 28 1a 12 65 e6 7f 36 b9 92 c2 c0 79 e8 2d 
GATT-RX:	 00 f4 60 7b 6a a5 b6 9b d4 a2 f0 80 5b e6 9e 0b 
GATT-RX:	 70 0c c9 34 a3 65 11 7f 

Set publication for node 0100 status: Success
Publication address: 0xc000
Period: 0 ms
Retransmit count: 0
Retransmit Interval Steps: 0
GATT-TX:	 00 f4 d2 0f fb e4 49 24 a0 6c 7d 34 a8 bd dc ca 
GATT-TX:	 c6 a8 9f 17 d0 eb 5b 46 31 
[config: Target = 0100]# `

```

## Provisioning and configuring Client node

Run meshctl command line in Raspberry PI.

```
`pi@raspberrypi:~/bluez-5.48/mesh $ ./meshctl .
Local config directory not provided.
  netkeys = 1
  appkeys = 2
  provisioners = 1
  provisioned nodes = 1
On/Off client model: new binding 0001
[NEW] Controller B8:27:EB:B2:E7:4A raspberrypi [default]
[NEW] Device D6:FB:47:4F:7E:A9 AOS Device
Service added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a
Char added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000d:
Char added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service000a/char000b:
Service added /org/bluez/hci0/dev_EC_60_BA_B5_36_D0/service0006`

```

Give power to client node, and select the search mode for unprovisioned devices to find out new unprovisioned ones.

```
`[meshctl]# discover-unprovisioned on
SetDiscoveryFilter success
Discovery started
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: yes
		Mesh Provisioning Service (00001827-0000-1000-8000-00805f9b34fb)
			Device UUID: dddd0000000000000000000000000000
			OOB: 0000`

```

Authenticate the device.

```
`[meshctl]# provision dddd
Trying to connect Device D6:FB:47:4F:7E:A9 AOS Device
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: no
Connection successful
Service added /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service0006
Service added /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a
Char added /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000b:
Char added /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d:
Services resolved yes
Found matching char: path /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000b, uuid 00002adb-0000-1000-8000-00805f9b34fb
Found matching char: path /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d, uuid 00002adc-0000-1000-8000-00805f9b34fb
Start notification on /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d
Characteristic property changed /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d
AcquireNotify success: fd 7 MTU 65
Notify for Mesh Provisioning Out Data started
Open-Node: 0xe51ab0
Open-Prov: 0xe59bf0
Open-Prov: proxy 0xe4c200
Initiated provisioning
Characteristic property changed /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000b
AcquireWrite success: fd 8 MTU 65
GATT-TX:	 03 00 10 
GATT-RX:	 03 01 01 00 01 00 00 04 00 08 00 00 00 
Got provisioning data (12 bytes)
	 01 01 00 01 00 00 04 00 08 00 00 00 
GATT-TX:	 03 02 00 00 02 03 04 
GATT-TX:	 03 03 86 03 e8 b6 50 d7 4d 33 e3 fd aa c0 fa 46 
GATT-TX:	 d5 1c a8 58 91 c1 4d de 96 30 26 8c a0 3c c3 5b 
GATT-TX:	 c6 87 f4 e9 b9 08 a7 e7 2d 67 ae 3a 7c f7 1c 42 
GATT-TX:	 6a da 6d 41 ba 77 17 69 25 a1 75 46 e5 eb 31 ee 
GATT-TX:	 07 5d 
GATT-RX:	 43 03 26 00 86 9a 35 54 40 b8 be 53 a0 88 e1 55 
GATT-RX:	 19 33 ee f4 4d 03 35 12 59 6d a5 63 de a2 0a dc 
GATT-RX:	 24 e4 a7 8a 8d 42 27 76 d8 2c a9 87 69 27 68 61 
GATT-RX:	 2b 56 f8 97 3c a9 bf de b6 64 00 f5 77 91 
GATT-RX:	 c3 73 3f 94 62 
Got provisioning data (65 bytes)
	 03 26 00 86 9a 35 54 40 b8 be 53 a0 88 e1 55 19 
	 33 ee f4 4d 03 35 12 59 6d a5 63 de a2 0a dc 24 
	 e4 a7 8a 8d 42 27 76 d8 2c a9 87 69 27 68 61 2b 
	 56 f8 97 3c a9 bf de b6 64 00 f5 77 91 73 3f 94 
	 62 
Request decimal key (0 - 9999)
[AOS 1m[mesh] Enter Numeric key:`

```

See the output of server command line OOB.

```
`Initializing...
Bluetooth initialized
Mesh initialized
OOB Number: 4159`

```

Type 4159 on Raspberry PI, process the following provision, and then print a long string of log.

```
`[AOS 1m[mesh] Enter Numeric key: 4159
GATT-TX:	 03 05 20 2e 58 e4 b8 06 93 0d a2 b5 2f 7b eb 16 
GATT-TX:	 99 07 
GATT-RX:	 03 05 fd ee fd f5 3d 54 71 42 91 92 64 a4 69 46 
GATT-RX:	 f7 bc 
Got provisioning data (17 bytes)
	 05 fd ee fd f5 3d 54 71 42 91 92 64 a4 69 46 f7 
	 bc 
GATT-TX:	 03 06 e6 4e 15 4c 75 e4 2d 76 d3 0b 13 64 91 f0 
GATT-TX:	 ac e4 
GATT-RX:	 03 06 54 65 35 17 f6 cb 77 bc ae af 19 8e 7c 92 
GATT-RX:	 71 f3 
Got provisioning data (17 bytes)
	 06 54 65 35 17 f6 cb 77 bc ae af 19 8e 7c 92 71 
	 f3 
Confirmation Validated
S-Key	 b4 dc cb 8f 1d 62 4e c4 95 f5 74 6d ff bc 42 d9 
S-Nonce	 54 c1 e5 68 cf a6 ec c0 47 3b 8f f4 25 
DevKey	 0c c5 26 e9 12 e3 d2 89 39 20 3a af 27 a4 f9 d2 
Data	 18 ee d9 c2 a5 6a dd 85 04 9f fc 3c 59 ad 0e 12 
Data	 00 00 00 00 00 00 05 01 01 
DataEncrypted + mic	 80 bb ca b9 3f ca ff 49 5d 22 3c d3 5a 70 45 61 
DataEncrypted + mic	 ae a4 a7 7b b6 15 b6 ed df 7c 47 8f 84 8d d4 dc 
DataEncrypted + mic	 db 
GATT-TX:	 03 07 80 bb ca b9 3f ca ff 49 5d 22 3c d3 5a 70 
GATT-TX:	 45 61 ae a4 a7 7b b6 15 b6 ed df 7c 47 8f 84 8d 
GATT-TX:	 d4 dc db 
GATT-RX:	 03 08 
Got provisioning data (1 bytes)
	 08 
Provision success. Assigned Primary Unicast 0101
Attempting to disconnect from D6:FB:47:4F:7E:A9
Characteristic property changed /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d
Write closed
Service added /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a
Char added /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000b:
Char added /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d:
Services resolved no
SetDiscoveryFilter success
Discovery started
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: yes

		Mesh Proxy Service (00001828-0000-1000-8000-00805f9b34fb)
		Identity for node 0101
Trying to connect to mesh
Adapter property changed 
[CHG] Controller B8:27:EB:B2:E7:4A Discovering: no
Connection successful
Services resolved yes
Found matching char: path /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000b, uuid 00002add-0000-1000-8000-00805f9b34fb
Found matching char: path /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d, uuid 00002ade-0000-1000-8000-00805f9b34fb
Start notification on /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d
Characteristic property changed /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000d
AcquireNotify success: fd 7 MTU 65
Notify for Mesh Proxy Out Data started
Trying to open mesh session
GATT-RX:	 01 01 00 d4 76 79 43 3f db 10 4a 00 00 00 05 f4 
GATT-RX:	 0a 41 fa b0 af 32 0b 
iv_upd_state = IV_UPD_NORMAL
Mesh session is open
Characteristic property changed /org/bluez/hci0/dev_D6_FB_47_4F_7E_A9/service000a/char000b
AcquireWrite success: fd 8 MTU 65
GATT-TX:	 02 f4 8c 6d ef 98 ac 2e 79 c8 e0 2c 83 2b 4a b4 
GATT-TX:	 c9 1f e3 a9 
GATT-TX:	 00 f4 95 a2 60 a7 67 ec a4 40 e9 18 1a 69 a4 00 
GATT-TX:	 c1 54 c7 3d a9 d7 
GATT-RX:	 02 f4 35 d1 c4 66 28 17 1b 6d ed 86 f9 26 19 ca 
GATT-RX:	 d5 6f 3c 9e 7b cd 
Proxy Whitelist filter length: 0
GATT-RX:	 00 f4 98 0a 63 16 58 d9 d6 34 75 81 6d 7e a3 5f 
GATT-RX:	 83 2f 1c 58 17 ad 23 17 7d 7b 50 ee 4e 61 
GATT-RX:	 00 f4 cf 10 98 83 f6 b7 13 22 45 a0 6b 53 1b 51 
GATT-RX:	 c8 35 4b a7 f4 0e f4 f3 d4 c2 0c 25 12 ec 
GATT-RX:	 00 f4 a3 1e 00 1c 07 b8 e8 a6 9f 29 aa 84 7e eb 
GATT-RX:	 ee 7a 7c 6b 13 63 37 ce 
	Composition data for node 0101 {
  "cid":"0002",
  "pid":"0000",
  "vid":"0000",
  "crpl":"0005",
  "features":{
    "relay":false,
    "proxy":true,
    "friend":false,
    "lpn":false
  },
  "elements":[
    {
      "elementIndex":0,
      "location":"0000",
      "models":[
        "0000",
        "0001",
        "1102"
      ]
    }
  ]
}
GATT-TX:	 00 f4 71 66 e6 89 61 44 19 6b c1 75 a4 a9 b8 f2 
GATT-TX:	 ed 9b b9 ee 99 b5 96 bc 05 `

```

The above log can be divided as two stages. The first is to use provisioning connection to carry out provision, and the second is to establish mesh proxy connection, which will be used to transmit network configuration information.

When the log is completely printed, the server side will print the information that provision has completed.

```
`Provisioning completed!
Net ID: 0
Unicast addr: 0x0101`

```

Next is to transmit network configuration information based on the mesh proxy connection you just set up.

First, enter the menu.

```
`[AOS Device-Node-0101]# menu config
Menu config:
Available commands:
-------------------
target <unicast>                                  Set target node to configure
composition-get [page_num]                        Get Composition Data
netkey-add <net_idx>                              Add network key
netkey-del <net_idx>                              Delete network key
appkey-add <app_idx>                              Add application key
appkey-del <app_idx>                              Delete application key
bind <ele_idx> <app_idx> <mod_id> [cid]           Bind app key to a model
ttl-set <ttl>                                     Set default TTL
ttl-get                                           Get default TTL
pub-set <ele_addr> <pub_addr> <app_idx> <per (step|res)> <re-xmt (cnt|per)> <mod id> [cid] 
						  Set publication
pub-get <ele_addr> <model>                        Get publication
proxy-set <proxy>                                 Set proxy state
proxy-get                                         Get proxy state
ident-set <net_idx> <state>                       Set node identity state
ident-get <net_idx>                               Get node identity state
relay-set <relay> <rexmt count> <rexmt steps>     Set relay
relay-get                                         Get relay
hb-pub-set <pub_addr> <count> <period> <features> <net_idx> Set heartbeati publish
sub-add <ele_addr> <sub_addr> <model id>          Subscription add
sub-get <ele_addr> <model id>                     Subscription get
app-get <ele_addr> <model id>                     Get App Keys
back                                              Return to main menu
version                                           Display version
quit                                              Quit program
exit                                              Quit program
help                                              Display help about this program`


```

Select configuring node, where you can see that the node ucast addr is 0x0101.

```
`[AOS Device-Node-0101]# target 0101
Configuring node 0101
[config: Target = 0101]# target 0101[config: Target = 0101]#`


```

Configure app key.

```
`[config: Target = 0101]# target 0101[config: Target = 0101]# appkey-add 1
GATT-TX:	 00 f4 2b 37 95 64 e7 fe ba 41 b3 c8 d9 0c f4 0c 
GATT-TX:	 76 36 a4 f0 f9 c4 f8 93 73 8a 3d 7d 7e c1 
GATT-TX:	 00 f4 43 6e 87 5d 09 0b 4b 05 a0 1b c0 48 da a1 
GATT-TX:	 bf d6 9b 21 91 68 d8 b0 b4 27 57 90 61 06 
GATT-RX:	 00 f4 7f d6 9d 40 6d bc 10 5f 41 67 68 56 9c d8 
GATT-RX:	 b9 77 dc 71 b5 10 ec b1 85 
GATT-RX:	 00 f4 81 91 ee 63 66 b4 5a 42 6f 70 f6 d2 12 93 
GATT-RX:	 2d 9f 21 2d fa a3 89 2f 4b 
Node 0101 AppKey Status Success
	NetKey 000, AppKey 001`


```

Bind the sensor client model (0x1102) of the client node to the app key (index =1).

```
`[config: Target = 0101]# bind 0 1 1102
GATT-TX:	 00 f4 97 c8 22 ee 6f 11 c3 c6 7c 5c 4f 88 e4 3d 
GATT-TX:	 36 9e 19 c7 3f a8 f3 d3 d4 7d 90 
GATT-RX:	 00 f4 e9 e9 33 90 5a f3 cb 88 c1 3e 44 af f6 61 
GATT-RX:	 d0 ef 47 35 35 c3 03 8a c9 43 01 7d 
Node 0101 Model App Status Success
	Element 0101 AppIdx 001
ModelId 1102`


```

Add the subscription address information 0xc000 of sensor server model 0x1102.

```
`[config: Target = 0101]# sub-add 0101 c000 1102
GATT-TX:	 00 f4 c5 5e 47 70 42 84 3b 28 14 26 60 83 01 69 
GATT-TX:	 7c 25 c3 c2 d9 1d d3 fb c0 e5 65 
GATT-RX:	 00 f4 6f 71 02 d9 ec 00 cf c1 96 f0 3a 7f ea a9 
GATT-RX:	 b1 fb 6d 70 31 62 d8 d0 4b 5d 81 a6 

Subscription changed for node 0101 status: Success
Element Addr:	0101
Subscr Addr:	c000
Model ID:	1102`


```

Set the app key release information of sensor server model (0x1100). The release address is 0xc000. 

```
`[config: Target = 0101]# pub-set 0101 c000 1 0 0 1102
GATT-TX:	 00 f4 5a 22 c5 d2 d0 76 94 a9 7f 70 6e 64 c1 f3 
GATT-TX:	 be 58 43 fc f1 61 f0 ac dc ce 4d 0b 9c f1 
GATT-TX:	 00 f4 1a 8a af 58 92 d2 d5 38 b2 0f 62 56 34 96 
GATT-TX:	 5f 08 9e 5a c6 54 
GATT-RX:	 00 f4 eb 12 ea 4f 6f 1f f8 e6 f2 92 c4 d7 82 a0 
GATT-RX:	 ad 9e c9 92 9e 91 d4 a6 3c 
GATT-RX:	 00 f4 2a 11 82 c6 86 27 0a 4a 88 bf a6 02 74 56 
GATT-RX:	 d5 2d 28 72 24 cd ae 91 fa dc 72 35 d7 be 
GATT-RX:	 00 f4 a7 08 14 6c 95 bd 62 07 67 1b c6 c8 95 b8 
GATT-RX:	 f2 a9 3b 03 d2 82 d1 97 

Set publication for node 0101 status: Success
Publication address: 0xc000
Period: 0 ms
Retransmit count: 0
Retransmit Interval Steps: 0
GATT-TX:	 00 f4 f0 60 6a 70 db 3b e2 60 a6 c8 50 d9 03 36 
GATT-TX:	 d5 a1 47 c9 2f 25 49 5e 47`


```

When all the above configuration is completed, you can see the sent request for temperature and reply log at client node.

```
`Sensor status Get request sent with OpCode 0x00008231
Got the sensor status 
Sensor ID: 0x2a1f
Temperature value: 27`


```

You can see the received request and reply log at server side.

```
`Sensor Status Get request received
Sensor status sent with OpCode 0x00000052`
```

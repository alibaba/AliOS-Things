## Introduction

This example demonstrates how to use "atparser" and "at_adapter" components to operate on AT CMD modules, which are connected to host MCU via UART. Fow now, we have supported linuxhost platform as host. Users are free to port the components to other platforms.

This example has been tested working with MxChip MK3060 WiFi module. This AT firmware for MK3060 can be downloaded from http://developer.mxchip.com/fileDownload/457.

### Build

1. Build atapp with basic AT command operations:

```sh
$ aos make atapp@linuxhost osal=posix
```

2. Build atapp with ENETRAWMODE support:

```sh
$ aos make atapp@linuxhost osal=posix at_adapter=1
```

### Result

1. Run atapp and test basic AT command operations:

```sh
$ ./out/atapp@linuxhost/binary/atapp@linuxhost.elf
$ help
$ at simple AT
$ at simple AT+WJAP?
```
2. Run atapp and test ENETRAWMODE operations:
```sh
$ ./out/atapp@linuxhost/binary/atapp@linuxhost.elf --no-tapif
$ help
$ at simle AT
$ at test_at_enet tcp 192.168.0.100 hello
```

Note: The at command may not be available if there is no AT UART device connected. You should make sure the AT UART correctly connected to linuxhost. Check file `board/linuxhost/atcmd_config_platform.h`:`AT_UART_LINUX_DEV` and ensure you have the correct AT UART device.

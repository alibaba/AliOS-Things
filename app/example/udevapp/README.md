## Contents

## Introduction

The **udevapp** is a sample code for **udev**. using command: `aos ota udevapp@xxx` can update device quickly:

![](https://img.alicdn.com/tfs/TB1GINADwTqK1RjSZPhXXXfOFXa-919-571.gif)

> The `productKey` and the `deviceName` will read from kv partition.
> need to update `aos-cube` to 0.3.0 or later

for more **udev** information, check [udev](../../middleware/uagent/udev).

### Features

- developerment over the air.

### Directories

```
udevapp
└─udevapp.c              # udev sample code
```

### Dependencies

- netmgr
- cli
- kv
- udev

### Supported Boards

- developerkit
- esp8266
- esp32devkitc
- mk3060
- mk3080

### Running

> need to update `aos-cube` to 0.3.0 or later
> if using `AliOS Studio`, need to update `AliOS Studio` to 0.10.9 or later.

Before running `aos ota udevapp@xxxx`, you need to set the `productKey` and `deviceName`, typing blow commands in board's console:

```
# kv set productkey a1MZxOdcBn2
# kv set devicename test_02
```

Typing `kv list` to check kv params, looks like:

```
# kv list
wifi = h3c_router01
key_ota_breakpoint =
app_version = app-1.0.0-20190116.1044
productkey = a1MZxOdcBn2
devicename = test_02
```

Once done, reboot board, running `aos ota udevapp@xxxx -d <devicename> -p <productkey>` on PC will update device binary quickly:

`aos ota udevapp@xxx` command:

![](https://img.alicdn.com/tfs/TB1GINADwTqK1RjSZPhXXXfOFXa-919-571.gif)

Or using `AliOS Studio` to update device binary quickly:

![](https://img.alicdn.com/tfs/TB14ROOHbrpK1RjSZTEXXcWAVXa-1140-820.gif)

and device log display like this:

![](https://img.alicdn.com/tfs/TB1ZohLDAzoK1RjSZFlXXai4VXa-926-571.gif)

## Reference


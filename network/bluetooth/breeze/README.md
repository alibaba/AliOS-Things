# README.md: Breeze

## Contents

```shell
├── api
│   ├── breeze_awss_export.c
│   ├── breeze_export.c
│   └── breeze_export.h
├── breeze.mk
├── core
│   ├── auth.c
│   ├── ble_service.c
│   ├── core.c
│   ├── extcmd.c
│   ├── sha256.c
│   ├── transport.c
│   └── utils.c
├── hal
│   ├── ble
│   │   ├── aes.c
│   │   ├── aes_mbed.c
│   │   ├── ali_crypto.h
│   │   ├── ali_crypto_types.h
│   │   ├── ble.mk
│   │   ├── breeze_hal_ble.c
│   │   ├── breeze_hal_os.c
│   │   ├── breeze_hal_sec.c
│   │   ├── include
│   │   └── mbed_crypto.h
│   └── include
│       ├── breeze_hal_ble.h
│       ├── breeze_hal_os.h
│       └── breeze_hal_sec.h
├── include
│   ├── auth.h
│   ├── ble_service.h
│   ├── bzopt.h
│   ├── common.h
│   ├── core.h
│   ├── extcmd.h
│   ├── sha256.h
│   ├── transport.h
│   └── utils.h
```

## Introduction

Breeze is a tiny and flexible SDK which provides secure BLE connection to Alibaba IoT cloud and services. It also offers an efficient way to operate WiFi provision process (i.e. obtaining SSID and password information via BLE connection). Besides, Breeze SDK is also designed to be able to provide add-on features, e.g. OTA.

Breeze SDK defines clear and simple APIs so it can be easily port to different platforms (OS, BLE stack, etc.). 

### Features

- **Simple and Effifent BLE Data Path**. Breeze provides an efficent data path between BLE devices.
- **Secure Connection to Alibaba Cloud**. Breeze provides secure connection to Alibaba IoT cloud services, either in the way of one secret per device, or one secret per product. This is an optional feature, which can be turned on or off according to user's configuration.
- **Easy WiFi Provisioning**. Breeze provides an extremely easy process to do WiFi provision process. This is an optional feature, which can be turned on or off according to user's configuration.

### Dependencies

- **OS and BLE Stack**, i.e. `bluetooth.breeze.hal.ble` if AliOS Things and its BLE stack is used. Other OS and BLE stack from any vendor is also designed to be supported, as long as HAL APIs defined in `hal/include/breeze_hal_ble.h` and `hal/include/breeze_hal_os.h` are correctly implemented on top of vendor's stack.
- **Security API**, i.e. `bluetooth.breeze.hal.ble` if mbedtls-based implementation inside AliOS Things is adopted. User is free to choose other implementation, as long as APIs defined in `hal/include/breeze_hal_sec.h` are correctly implemented.

## API

### breeze_start

Start breeze SDK services. This API is called by user to initialize and start breeze services.

**Arguments**

| name     | type          | description                                                  |
| :------- | :------------ | :----------------------------------------------------------- |
| dev_conf | device_config | The information used to initialize Breeze SDK, including device triple, callbacks, model ID, etc. |

**Return**

`0` on success, `-1` on failure.

### breeze_end

Stop breeze services. This API is called by user to stop the breeze services.

**Arguments**

None.

**Return**

`0` on success, `-1` on failure.

### breeze_awss_init

Initialize breeze awss module. This API is available only when the **WiFi provisioning** feature is enabled.

**Arguments**

| name | type              | description                                                  |
| :--- | :---------------- | :----------------------------------------------------------- |
| cb   | apinfo_ready_cb   | The callback to be called by breeze SDK when AP info ready.  |
| info | breeze_dev_info_t | The device information (e.g. device name, device secret, product id, product key, product secret) required by breeze SDK. |

**Return**

`0` on success, `-1` on failure.

### breeze_awss_start

Start breeze awss process. When this API is called, do not call breeze_start anymore. This API is available only when the **WiFi provisioning** feature is enabled.

**Arguments**

None.

**Return**

None.

### breeze_post

Post device status. This API can be used to update date to BLE server, in non-blocked way. This API uses BLE indicate way to send the data.

**Arguments**

| name   | type     | description                  |
| :----- | :------- | :--------------------------- |
| buffer | uint8_t  | Data to post.                |
| length | uint32_t | Length of the data, in byte. |

**Return**

`0` on success, otherwise error code.

### breeze_post_fast

Post device status in fast way. This API is similiar with `breeze_post`. The difference is that BLE notify way is used to post the data in this API.

**Arguments**

| name   | type     | description                  |
| :----- | :------- | :--------------------------- |
| buffer | uint8_t  | Data to post.                |
| length | uint32_t | Length of the data, in byte. |

**Return**

`0` on success, otherwise error code.

### breeze_post_ext

Post device status with command.

**Arguments**

| name   | type     | description                    |
| :----- | :------- | :----------------------------- |
| buffer | uint8_t  | Data to post.                  |
| length | uint32_t | Length of the data, in byte.   |
| cmd    | uint8_t  | Command to post, 0 by default. |

**Return**

`0` on success, otherwise error code.

### breeze_append_adv_data

Append user specific data to the tail of the breeze advertising data.

**Arguments**

| name | type     | description                  |
| :--- | :------- | :--------------------------- |
| data | uint8_t  | Data to append.              |
| len  | uint32_t | Length of the data, in byte. |

**Return**

None.

### breeze_append_adv_data

Restart BLE advertisement. This API will stop and then start the adv.

**Arguments**

None.

**Return**

None.

## Reference

None.
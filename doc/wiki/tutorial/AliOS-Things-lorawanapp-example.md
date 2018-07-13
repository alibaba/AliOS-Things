EN | [中文](AliOS-Things-lorawanapp-example.zh)

AliOS Things is a light-weight embedded operating system in AliOS Family. This article aims to show a LoRaWANapp example and then complete network construction and data transmit in it. We want to give you a general idea of AliOS Things from it.

## Some related preparation before introduction:

- lora network (the related hardware should be connected)

  - End point
    - MXCHIP MK3047							*1
  - Gateway
    - M1301 Gateway module (GTI)  	*1
      - Raspberry Pi                                                        *1
  - Lora Server
    - [LoRa App Server](https://docs.loraserver.io/lora-app-server/overview/): It's an open-source sever package, and you can download it by yourself.
- 5VDC/2ADC power（power supply by gateway ）   *1
- J-Link                                             *1
- micro-USB line                                *1
- serial port terminal : here we use X-Shell
- J-Flash
- basic knowledge of lora and lorawan
- [AliOS-Things](https://github.com/alibaba/AliOS-Things)

## Start up

### End point

At first, you need to set up the environment. Our IDE is based on VSC, and it's suitable for windows, mac and linux. We now use windows as an example and you can know more details in related links in github.

Open IDE and AliOS Things as follows :

![alios ide](https://img.alicdn.com/tfs/TB1vwKDi2DH8KJjy1XcXXcpdXXa-1926-1052.png)

Select demo and board. Here, select lorawanapp and eml3047. As shown in the following picture, you can click in the bottom highlight part and select in the above part.

![lorawanapp](https://img.alicdn.com/tfs/TB1i1Ghi3nH8KJjSspcXXb3QFXa-1527-818.png)

After selecting, you can click on the `Build` button in the bottom work bar. Compilation information is as followed: 

> [INFO-Build] -------------COMPILE OUTPUT START-------------
>
> [INFO-Build] Making config file for first time
>
> [INFO-Build] 
>
> [INFO-Build] processing components: lorawanapp eml3047 platform/mcu/stm32l0xx/stm32l071kb vcall vfs init auto_component
>
> [INFO-Build] 
>
> [INFO-Build] kernel_version:AOS-R-1.1.1
>
> [INFO-Build] 
>
> [INFO-Build] Build AOS Now
>
> [INFO-Build] 
>
> [INFO-Build] Skipping building bootloader due to \"total\" is not set
>
> [INFO-Build] 
>
> [INFO-Build] 
>
> [INFO-Build] 
>
> [INFO-Build] Compiling lorawanapp
>
> [INFO-Build] 
>
> [INFO-Build] Compiling board_eml3047
>
> [INFO-Build] 
>
> [INFO-Build] Compiling stm32l071kb
>
> [INFO-Build] 
>
> [INFO-Build] Compiling vcall
>
> [INFO-Build] 
>
> [INFO-Build] Compiling vfs
>
> [INFO-Build] 
>
> [INFO-Build] Compiling kernel_init
>
> [INFO-Build] 
>
> [INFO-Build] Compiling auto_component
>
> [INFO-Build] 
>
> [INFO-Build] Compiling lorawan
>
> [INFO-Build] 
>
> [INFO-Build] Compiling armv6m
>
> [INFO-Build] 
>
> [INFO-Build] Compiling rhino
>
> [INFO-Build] 
>
> [INFO-Build] Compiling vfs_device
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/board_eml3047.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/lorawanapp.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/stm32l071kb.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/vcall.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/vfs.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/kernel_init.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/auto_component.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/lorawan.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/armv6m.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/rhino.a
>
> [INFO-Build] 
>
> [INFO-Build] Making out/lorawanapp@eml3047/libraries/vfs_device.a
>
> [INFO-Build] 
>
> [INFO-Build] Making lorawanapp@eml3047.elf
>
> [INFO-Build] 
>
> [INFO-Build] 
>
> [INFO-Build] 
>
> [INFO-Build] Making lorawanapp@eml3047.bin
>
> [INFO-Build] 
>
> [INFO-Build] Making lorawanapp@eml3047.hex
>
> [INFO-Build] 
>
> [INFO-Build] AOS MEMORY MAP
>
> [INFO-Build] |=================================================================|
>
> [INFO-Build] | MODULE                                   | ROM       | RAM      |
>
> [INFO-Build] |=================================================================|
>
> [INFO-Build] | armv6m                                   | 320       | 0        |
>
> [INFO-Build] | board_eml3047                            | 6473      | 1232     |
>
> [INFO-Build] | libc_nano                                | 12422     | 476      |
>
> [INFO-Build] | libgcc                                   | 9932      | 0        |
>
> [INFO-Build] | libm                                     | 772       | 0        |
>
> [INFO-Build] | libnosys                                 | 144       | 4        |
>
> [INFO-Build] | lorawan                                  | 23649     | 2449     |
>
> [INFO-Build] | lorawanapp                               | 417       | 16       |
>
> [INFO-Build] | rhino                                    | 9183      | 3553     |
>
> [INFO-Build] | stm32l071kb                              | 6384      | 156      |
>
> [INFO-Build] | vcall                                    | 16        | 0        |
>
> [INFO-Build] | *fill*                                   | 57        | 11586    |
>
> [INFO-Build] |=================================================================|
>
> [INFO-Build] | TOTAL (bytes)                            | 69769     | 19472    |
>
> [INFO-Build] |=================================================================|
>
> [INFO-Build] Build complete
>
> [INFO-Build] Making .gdbinit
>
> [INFO-Build] 
>
> [INFO-Build] Build Complete!
>
> [INFO-Build] -------------COMPILE OUTPUT FINISHED-------------
>
> [INFO-Build] Compile time: 32.85s

You will see the related output file in "out" folder in AliOS Things after compiling.

![lora_directory](https://img.alicdn.com/tfs/TB1BeKJi_vI8KJjSspjXXcgjXXa-816-572.png)

Them, connect MK3047 to the computer and start programming through J-Flash :

- Create programming project

- Select "Option->Project settings"

  ![jf1](https://img.alicdn.com/tfs/TB1pT08i8HH8KJjy0FbXXcqlpXa-781-224.png)

- Select "File->open data file", and select the *.bin in the binary of folder mentioned in point 4.

- Set the initial location of flash programming as 0x08000000

- Select "Target->Connect" to connect to the target and select "Production Programming" to start programming

  ![jf3](https://img.alicdn.com/tfs/TB1R1dVi0zJ8KJjSspkXXbF7VXa-681-242.png)

​

Open X-Shell and find out related serial port. Connect to the corresponding port. Parameter is shown as below : 

![串口](https://img.alicdn.com/tfs/TB1ldupi3vD8KJjy0FlXXagBFXa-206-154.png)

Serial port terminal will be displayed as followed after connecting. If you see the data of rssi and snr, it means that the end point has been connected to Lorawan network and started to receive gateway data.

![sl2](https://img.alicdn.com/tfs/TB1kuuKi3DD8KJjy0FdXXcjvXXa-848-759.png)

The above is the programming of default code. The lorawan parameters are as follows:

| Parameter         | Value                                    |
| ----------------- | ---------------------------------------- |
| Connecting method | OTAA                                     |
| DevEui            | 31-35-37-31-50-37-7B-18（like the MAC of that board） |
| AppEui            | 01-01-01-01-01-01-01-01                  |
| AppKey            | 2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C |
| Class             | A                                        |

The above four parameters can be modified in aos/example/lorawanapp/commissioning.h :

```
/**
 * When set to 1 the application uses the Over-the-Air activation procedure
 * When set to 0 the application uses the Personalization activation procedure
 */
#define OVER_THE_AIR_ACTIVATION                     1

/**
 * Mote device IEEE EUI (big endian)
 *
 * \remark see STATIC_DEVICE_EUI comments
 */
#define LORAWAN_DEVICE_EUI                          { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 }


/**
 * Application IEEE EUI (big endian)
 */
#define LORAWAN_APPLICATION_EUI                     { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 }

/**
 * AES encryption/decryption cipher application key
 */
#define LORAWAN_APPLICATION_KEY                     { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }


```

Node type can be modified in aos/example/lorawanapp/lorawanapp.c :

```
/**
 * Initialises the Lora Parameters
 */
static LoRaParam_t LoRaParamInit = {
    TX_ON_TIMER,
    APP_TX_DUTYCYCLE,
    CLASS_A,
    LORAWAN_ADR_ON,
    DR_0,
    LORAWAN_PUBLIC_NETWORK,
    JOINREQ_NBTRIALS
}
```

### Gateway

1. Give power to gateway. ( No extra setting is needed. You only need to give it power and enable it to work. Details can refer to lorawan protocols）

### Server

1. configure in server side

   - Create a new project and select related parameters as followed 

     ![s0](https://img.alicdn.com/tfs/TB1NSlOi3vD8KJjSsplXXaIEFXa-1480-271.png)

     ![s2](https://img.alicdn.com/tfs/TB1yehVi0zJ8KJjSspkXXbF7VXa-1221-807.png)

   - Create nodes in the application. You can fill in it according to the print information of serial terminal.

     ![s3](https://img.alicdn.com/tfs/TB1mfRZi0fJ8KJjy0FeXXXKEXXa-1494-312.png)

     ![s4](https://img.alicdn.com/tfs/TB1RTV8i8HH8KJjy0FbXXcqlpXa-1313-828.png)

   - In the process of network connection, the terminal node will be activated and corresponding key will be generated. 

     ![s5](https://img.alicdn.com/tfs/TB1hK4Si_TI8KJjSsphXXcFppXa-1477-666.png)

   - When network connection is completed, data interaction will begin, and you will see it in raw frame logs.

     ![s6](https://img.alicdn.com/tfs/TB1BvRZi0fJ8KJjy0FeXXXKEXXa-1474-665.png)

2. Server side send out data 

   - Open the api page of the server. Find out "post" in "Internal" and fill in the user name and password in "body" frame, which is "admin" and "admin" by default. Click "Try it out!" to generate Token.

     ![api1](https://img.alicdn.com/tfs/TB1Ki9hi8TH8KJjy0FiXXcRsXXa-1221-727.png)

   - Find out jwt field in the below response body, which is the token we need.

     ![api01](https://img.alicdn.com/tfs/TB128F7i22H8KJjy0FcXXaDlFXa-1198-206.png)

   - Copy the token to the blank space on the top right of the page as followed: 

     ![api02](https://img.alicdn.com/tfs/TB1n8dNi4TI8KJjSspiXXbM4FXa-1252-112.png)

   - Find out "DownlinkQueue", and fill in the corresponding devEUI. Fill in the data for test and you should encode it through [base64](https://www.base64encode.org/) before filling in. Set fPort as 100 and click "Try it out!" to send out data.

     ![api2](https://img.alicdn.com/tfs/TB1_1GJi_vI8KJjSspjXXcgjXXa-1221-779.png)

   - Normally, the node will receive data and print it in serial port. Till now on, the example task is completed.

3. If you want to try ABP, Class C（only Class A and C are supported）or other configurations, you can modify the setting of code and node mentioned above.

### Something about Lorawan

1. network architecture : node, gateway, server, application

   There's no direct link with nodes and gateways. (Nodes do not carry gateway identity information.) When a node send out connection request, all gateways can accept it, and after sending requests to server side, gateway will select the one with best signal quality.

2. The effect of ADR is determined by actual test. 

3. Relax frame-counter is a function target to ABP. The frame count of ABP is always cleared because of power cycle, and server side sometimes can't receive data because of frame sequence. As a result, you may need to release the existing frame number to re-receive the new frame count.

   ​


## Summary

Here is an AliOS Things lorawan example. There may be some mistakes and you are welcomed to point out them. This article mainly aims to give you a general idea of AliOS Things and lorawan network.

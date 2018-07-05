EN | [中文](AliOS-Things-lorawanapp-@Keil.zh)

## Some related preparation before introduction:

- Keil v5.25.2.0( the version that is currently used）


- lora network（the related hardware should be connected）
  - End point
    - MXCHIP MK3047							*1
  - Gateway
    - M1301 Gateway module(GTI）   	*1
      - Raspberry Pi                                                        *1
  - Lora Server
    - [LoRa App Server](https://docs.loraserver.io/lora-app-server/overview/): It's an open-source sever package, and you can download it by yourself.
- 5VDC/2ADC power（power supply by gateway ）    *1
- J-Link                                             *1
- micro-USB line                            *1
- serial port terminal : here we use X-Shell
- basic knowledge of lora and LoRaWAN
- [AliOS-Things](https://github.com/alibaba/AliOS-Things)
  ​

## Start up

### End point

1. location of Keil project：aos/projects/Keil/lorawanapp
2. project structure：

![lorawan_keil](https://img.alicdn.com/tfs/TB1X53omRfH8KJjy1XbXXbLdXXa-363-740.png)

 analysis of structure：

- board/eml3047： board-level resource configuration 
- device/eml3047_lwran: lora interface corresponding to the board 
- device/sx127x：lora radio part
- example：example code and aos initiation
- kernel/protocols：LoRaWAN protocol stack
- kernel/rhino：aos kernel
- kernel/vcall：aos api defines the compilation interface the system should provide
- platform/arch：hardware kernel structure, which is now armv6 cortex m0
- platform/mcu：corresponding to MAC driver in the bottom layer 

3.Compile and program it into target board

4.Open X-Shell. Find out the corresponding serial port and connect to it. Port parameters are shown as below : 

![串口](https://img.alicdn.com/tfs/TB1ldupi3vD8KJjy0FlXXagBFXa-206-154.png)

5.Serial port terminal will be displayed as followed after connecting. If you see the data of rssi and snr, it means that the end point has been connected to Lorawan network and started to receive gateway data.

![sl2](https://img.alicdn.com/tfs/TB1kuuKi3DD8KJjy0FdXXcjvXXa-848-759.png)

The above is the programming of default code. The lorawan parameters are as follows:

| Parameter         | Value                                    |
| ----------------- | ---------------------------------------- |
| Connecting method | OTAA                                     |
| DevEui            | 31-35-37-31-50-37-7B-18（like the MAC of that board） |
| AppEui            | 01-01-01-01-01-01-01-01                  |
| AppKey            | 2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C |
| Class             | A                                        |

The above four parameters can be modified in aos/board/eml3047/inc/commissioning.h :

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

     Find out "DownlinkQueue", and fill in the corresponding devEUI. Fill in the data for test and you should encode it through [base64](https://www.base64encode.org/) before filling in. Set fPort as 100 and click "Try it out!" to send out data.

     ![api2](https://img.alicdn.com/tfs/TB1_1GJi_vI8KJjSspjXXcgjXXa-1221-779.png)

   - Normally, the node will receive data and print it in serial port. Till now on, the example task is completed.

     3. If you want to try ABP, Class C（only Class A and C are supported）or other configurations, you can modify the setting of code and node mentioned above.configure in server side

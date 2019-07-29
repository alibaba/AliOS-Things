/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2015 Semtech

Description: End device commissioning parameters

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __LORA_COMMISSIONING_H__
#define __LORA_COMMISSIONING_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/**
 * When set to 1 the application uses the Over-the-Air activation procedure
 * When set to 0 the application uses the Personalization activation procedure
 */
#define OVER_THE_AIR_ACTIVATION                     1

/**
 * Indicates if the end-device is to be connected to a private or public network
 */
#define LORAWAN_PUBLIC_NETWORK                      true

/**
 * When set to 1 DevEui is LORAWAN_DEVICE_EUI
 * When set to 0 DevEui is automatically generated by calling
 *         BoardGetUniqueId function
 */
#define STATIC_DEVICE_EUI                     1

/**
 * Mote device IEEE EUI (big endian)
 *
 * \remark see STATIC_DEVICE_EUI comments
 */
#define LORAWAN_DEVICE_EUI                          { 0xd8, 0x96, 0xe0, 0x00, 0x0a, 0x00, 0x00, 0x04 }

/**
 * Application IEEE EUI (big endian)
 */
#define LORAWAN_APPLICATION_EUI                     { 0xD8, 0x96, 0xE0, 0xE0, 0x00, 0x00, 0xD0, 0xAA }

/**
 * AES encryption/decryption cipher application key
 */
#define LORAWAN_APPLICATION_KEY                     { 0xF6, 0xC0, 0x8C, 0xEB, 0x6E, 0x28, 0xA6, 0x0B, 0x07, 0x3C, 0x0C, 0x68, 0x0A, 0x1B, 0x90, 0xC9 }

/**
 * Current network ID
 */
#define LORAWAN_NETWORK_ID                          ( uint32_t )0

/**
 * When set to 1 DevAdd is LORAWAN_DEVICE_ADDRESS
 * When set to 0 DevAdd is automatically generated using
 *         a pseudo random generator seeded with a value derived from
 *         BoardUniqueId value
 */
#define STATIC_DEVICE_ADDRESS                     0
/**
 * Device address on the network (big endian)
 *
 * \remark see STATIC_DEVICE_ADDRESS comments
 */
#define LORAWAN_DEVICE_ADDRESS                      {0x12, 0x23, 0x34, 0x56}

/**
 * AES encryption/decryption cipher network session key
 */
#define LORAWAN_NWKSKEY                             { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }

/**
 * AES encryption/decryption cipher application session key
 */
#define LORAWAN_APPSKEY                             { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C }

/**
 * Multicast address
 */
#define LORAWAN_MULTICAST_ADDRESS                   0x06344bf2

/**
 * AES encryption/decryption cipher network session key for multicast
 */
#define LORAWAN_MULTICAST_NWKSKEY                   { 0x65, 0xe4, 0x48, 0x3d, 0xe3, 0xa9, 0xce, 0xb1, 0x50, 0xd8, 0x86, 0xf3, 0x61, 0x46, 0x02, 0xb5 }

/**
 * AES encryption/decryption cipher application session key for multicast
 */
#define LORAWAN_MULTICAST_APPSKEY                   { 0xc2, 0x65, 0x67, 0xa3, 0xec, 0xd3, 0x96, 0x67, 0xd6, 0x23, 0xf8, 0x7a, 0xe1, 0x6c, 0x12, 0x23 }


#ifdef __cplusplus
}
#endif

#endif /* __LORA_COMMISSIONING_H__ */

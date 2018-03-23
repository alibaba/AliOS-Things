#ifndef _ALI_COMMON_H_
#define _ALI_COMMON_H_

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#define VERIFY_PARAM_NOT_NULL(p) \
    do {\
        if (p == NULL) printf("Error: VERIFY_PARAM_NOT_NULL failed (p: %p).\r\n", p);\
    } while (0)

#define VERIFY_SUCCESS(e) \
    do {\
        if (e) printf("Error: VERIFY_SUCCESS failed (e: %d).\r\n", e);\
    } while (0)

#define VERIFY_PARAM_NOT_NULL_VOID(p) \
    do {\
        if (p == NULL) printf("Error: VERIFY_PARAM_NOT_NULL_VOID failed (p: %p).\r\n", p);\
    } while (0)

#define VERIFY_MODULE_INITIALIZED_VOID() \
    do {\
        printf("Info: fake VERIFY_MODULE_INITIALIZED_VOID.\r\n");\
    } while (0)

#define VERIFY_MODULE_INITIALIZED() \
    do {\
        printf("Info: fake VERIFY_MODULE_INITIALIZED.\r\n");\
    } while (0)

#define VERIFY_SUCCESS_VOID(e) \
    do {\
        if (e) printf("Error: VERIFY_SUCCESS failed (e: %d).\r\n", e);\
    } while (0)

#define APP_ERROR_CHECK(e) \
    do {\
        if (e) printf("Error: APP_ERROR_CHECK failed (e: %d).\r\n", e);\
    } while (0)

#define GATT_MTU_SIZE_DEFAULT 23

typedef uint16_t ret_code_t;

/* Error codes. */
#define NRF_SUCCESS                          (0)
#define NRF_ERROR_INVALID_PARAM              (1)
#define NRF_ERROR_DATA_SIZE                  (2)
#define NRF_ERROR_INVALID_STATE              (3)
#define NRF_ERROR_GATT_NOTIFY                (4)
#define NRF_ERROR_GATT_INDICATE              (5)
#define NRF_ERROR_BT_ENABLE                  (6)
#define NRF_ERROR_BT_ADV                     (7)
#define NRF_ERROR_TIMEOUT                    (8)
#define NRF_ERROR_BUSY                       (9)
#define NRF_ERROR_INVALID_DATA               (10)
#define NRF_ERROR_INTERNAL                   (12)
#define NRF_ERROR_INVALID_ADDR               (13)
#define NRF_ERROR_NOT_SUPPORTED              (14)
#define NRF_ERROR_NO_MEM                     (15)
#define NRF_ERROR_FORBIDDEN                  (16)
#define NRF_ERROR_NULL                       (17)
#define NRF_ERROR_INVALID_LENGTH             (18)
#define NRF_ERROR_TRANSPORT_TX_FAILURE       (19)
#define NRF_ERROR_FLASH_STORE_FAIL           (20)
#define NRF_ERROR_FLASH_ERASE_FAIL           (21)
#define NRF_ERROR_SETTINGS_FAIL              (22)

#define BLE_CONN_HANDLE_INVALID 0xffff
#define BLE_CONN_HANDLE_MAGIC 0x1234

#if defined (NRF51)
    #define ALI_BLUETOOTH_VER       0x00        /**< Bluetooth version 4.0 (see spec. v1.0.4 ch. 2.2). */
    #define ALI_MAX_SUPPORTED_MTU   23          /**< Maximum supported MTU. */
    #define ALI_CONTEXT_SIZE        394         /**< Context size required, in number of 4-byte words. */
#elif defined (NRF52) || defined(CONFIG_ESP32_WITH_BLE)
    #define ALI_BLUETOOTH_VER       0x01        /**< Bluetooth version 4.2 (see spec. v1.0.4 ch. 2.2). */
    #define ALI_MAX_SUPPORTED_MTU   247         /**< Maximum supported MTU. */
    #define ALI_CONTEXT_SIZE        490         /**< Context size required, in number of 4-byte words. */
#else
    #error No valid target set for ALI_CONTEXT_SIZE.
#endif

#endif

#ifdef __INTERCONNECTION__

#ifndef __APP_INTERCONN_H__
#define __APP_INTERCONN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"
#include "stdint.h"
#include "me_api.h"

#define INTERCONNECTION_MALLOC umm_malloc
#define INTERCONNECTION_FREE umm_free


#define INTERCONNECTION_DEVICE_CHECK(dev)                                \
    do                                                                   \
    {                                                                    \
        if (!dev)                                                        \
            ASSERT(0, "false spp device, %s, %d\n", __func__, __LINE__); \
    } while (0)

#define SPP_MAX_PACKET_SIZE                         1024
#define SPP_MAX_PACKET_NUM 5
#define SPP_TX_BUF_SIZE 1024                         //4096
#define INTERCONNECTION_READ_BUFF_SIZE (4096 + 100)  //(FLASH_SECTOR_SIZE_IN_BYTES + 100)

// connection related definitions
#define DEFAULT_INTERCONN_BLE_DATA_SEC_SIZE (20)
#define DEFAULT_INTERCONN_SPP_DATA_SEC_SIZE (600)

#define APP_INTERCONN_PATH_TYPE_SPP (1 << 0)
#define APP_INTERCONN_PATH_TYPE_BLE (1 << 1)
#define APP_INTERCONN_PATH_TYPE_HFP (1 << 2)
#define APP_INTERCONN_PATH_TYPE (APP_INTERCONN_PATH_TYPE_BLE | APP_INTERCONN_PATH_TYPE_SPP)

#define APP_INTERCONN_SPP_CONNECTED (1 << 0)
#define APP_INTERCONN_BLE_CONNECTED (1 << 1)
#define APP_INTERCONN_SPP_BLE_BOTH_CONNECTED (APP_INTERCONN_SPP_CONNECTED | APP_INTERCONN_BLE_CONNECTED)

#define APP_INTERCONN_SPP_DISCONNECTED (~(1 << 0))
#define APP_INTERCONN_BLE_DISCONNECTED (~(1 << 1))
#define APP_INTERCONN_SPP_BLE_BOTH_DISCONNECTED (0)


// general used definitions
#define CRC16_LENGTH 2

// data structure related definitions
#define EA_MAX_LENGTH 4
#define EA_HEAD_MASK (0x80)
#define EA_DATA_MASK (~EA_HEAD_MASK)
// EA structrue used in CCMP connection negotiation process
#define EA_HEAD_WITH_EXTENSION (0 << 7)
#define EA_HEAD_WITHOUT_EXTENSION (1 << 7)
#define EA_WITH_EXTENSION(a) (a)
#define EA_WITHOUT_EXTENSION(a) (a + EA_HEAD_WITHOUT_EXTENSION)
// EA structure used after CCMP connection established
#define CCMP_EA_HEAD_WITH_EXTENSION (1 << 7)
#define CCMP_EA_HEAD_WITHOUT_EXTENSION (0 << 7)
#define CCMP_EA_WITH_EXTENSION(a) (a + CCMP_EA_HEAD_WITH_EXTENSION)
#define CCMP_EA_WITHOUT_EXTENSION(a) (a)

// sof value used for ccmp communication negotiation process
#define SOF_VALUE 0x7F

// type value used for ccmp communication negotiation process
#define CCMP_NEGO_TYPE_CHANNEL_MANAGER 0x00
#define CCMP_NEGO_TYPE_INSTANT_MESSAGE 0x02

// flag value used for ccmp communication negotiation process
#define CCMP_NEGO_FLAG_WITHOUT_ENCRYPTION 0x00
#define CCMP_NEGO_FLAG_LENGTH 0x00

// init request&response value used for ccmp communication negotiation process
#define CCMP_INIT_REQUEST_VERSION 0x01
#define CCMP_INIT_RESPONSE_VERSION 0x02
#define CCMP_INIT_VERSION_LENGTH 0x01
#define CCMP_INIT_VERSION_VALUE 0x01
#define CCMP_INIT_REQUEST_SUMMARY 0x03
#define CCMP_INIT_RESPONSE_SUMMARY 0x04
#define CCMP_INIT_REQUEST_SECURITY 0x05
#define CCMP_INIT_RESPONSE_SECURITY 0x06
#define CCMP_INIT_SECURITY_LENGTH 0x01

// bussiness request&response value used for ccmp communication negotiation process
#define CCMP_BUSINESS_REQUEST_COMMAND 0x09
#define CCMP_BUSINESS_RESPONSE_COMMAND 0x0A
#define CCMP_BUSINESS_REQUEST_ICONNECT 0x07
#define CCMP_BUSINESS_RESPONSE_ICONNECT_SUPPORT 0x00
#define CCMP_BUSINESS_RESPONSE_ICONNECT_NOT_SUPPORT 0x01
#define CCMP_BUSINESS_REQUEST_SECURITY 0x02
#define CCMP_BUSINESS_RESPONSE_SECURITY_SUPPORT 0x00
#define CCMP_BUSINESS_RESPONSE_SECURITY_NOT_SUPPORT 0x01
#define CCMP_BUSINESS_REQUEST_DATA_LENGTH 0x02

// BR socket creat request&response value used for ccmp communication negotiation process
#define CCMP_SOCKET_REQUEST_COMMAND 0x03
#define CCMP_SOCKET_RESPONSE_COMMAND 0x04
#define CCMP_SOCKET_CREAT_CHANNEL_BT 0x02
#define CCMP_SOCKET_CREAT_BUSINESS_ICONNECT CCMP_BUSINESS_REQUEST_ICONNECT
#define CCMP_SOCKET_CREAT_BUSINESS_TAG "iconnect"
#define CCMP_SOCKET_CREAT_PROTOCOL_BT 0x00
#define CCMP_SOCKET_CREAT_SERVICE_USE_DEFAULT_UUID 0x00
#define CCMP_SOCKET_RESPONSE_RESULT_ACCEPTED 0x00
#define CCMP_SOCKET_RESPONSE_RESULT_REFUSED 0x01
#define CCMP_SOCKET_RESPONSE_PORT_INVALID 0x00

typedef enum {
    INTERCONNECTION_PROCESS_INVALID = 0,
    INTERCONNECTION_PROCESS_READ    = 1,
    INTERCONNECTION_PROCESS_WRITE   = 2,
    INTERCONNECTION_PROCESS_MAX,
} INTERCONNECTION_PROCESS_E;

typedef struct
{
    void *   pDev;
    uint8_t  process;
    uint8_t *data;
    uint32_t dataLen;
} INTERCONNECTION_MAIL_T;

typedef struct
{
    uint8_t type;
    uint8_t length;
    uint8_t value[1];
} __attribute__((__packed__)) TLV_T;

typedef struct
{
    uint8_t flagType;
    uint8_t flagLength;
} __attribute__((__packed__)) CCMP_FLAG_T;

typedef struct
{
    uint8_t     sof;
    uint8_t     type;
    CCMP_FLAG_T flag;
    uint8_t     length;
    uint8_t     payload[1];
} __attribute__((__packed__)) CCMP_NEGO_HEAD_T;
#define CCMP_NEGO_PAYLOAD_OFFSET (OFFSETOF(CCMP_NEGO_HEAD_T, payload))

typedef enum
{
    INTERCONNECTION_NULL = 0,
    INTERCONNECTION_CCMP = 1,
    INTERCONNECTION_SPP_CLIENT = 2,
    INTERCONNECTION_SPP_SERVER = 3,
    INTERCONNECTION_DATA_CHANNEL_NUM,
} INTERCONNECTION_DATA_CHANNEL_E;

#define INTERCONNECTION_DATA_PATH_SPP 0
#define INTERCONNECTION_DATA_PATH_CCMP 1

typedef struct
{
    uint8_t    isSppClientAlive : 1;
    uint8_t    isSppServerAlive : 1;
    uint8_t    isCcmpAlive : 1;
    uint8_t    isCcmpDeniedByMobile : 1;
    uint8_t    reserved : 4;
    uint8_t    ccmpChannel;
    osThreadId rxThread;
    uint8_t    dataPath;
    uint8_t    currentDataChannel; // see @INTERCONNECTION_DATA_CHANNEL_E
    uint8_t    interconnectionDiscoverable;
} __attribute__((__packed__)) INTERCONNECTION_ENV_T;


#define INTERCONNECTION_STATUS_INVALID 0
#define INTERCONNECTION_STATUS_SUCCESS 1
#define INTERCONNECTION_STATUS_ERROR 2

bool app_interconnection_is_read_queue_empty(void);

int interconnect_spp_handle_data_event_func(void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen);

bool app_interconnection_get_spp_client_alive_flag(void);

bool app_interconnection_get_spp_server_alive_flag(void);

void app_interconnection_set_spp_client_alive_flag(bool flag);

void app_interconnection_set_spp_server_alive_flag(bool flag);

bool app_interconnection_get_ccmp_alive_flag(void);

void app_interconnection_set_ccmp_alive_flag(bool flag);

bool app_interconnection_get_ccmp_denied_by_mobile_flag(void);

void app_interconnection_set_ccmp_denied_by_mobile_flag(bool flag);

uint8_t app_interconnection_get_ccmp_channel(void);

void app_interconnection_set_ccmp_channel(uint8_t ccmpChannel);

uint8_t app_interconnection_get_data_path(void);

void app_interconnection_set_data_path(uint8_t dataPath);

uint8_t app_interconnection_get_current_data_channel(void);

void app_interconnection_set_current_data_channel(uint8_t channel);

void app_interconnection_init(void);

void app_interconnection_spp_open(btif_remote_device_t *remote_device);

void app_interconnection_ccmp_client_open(uint32_t param0, uint32_t param1);

void app_interconn_send_done(void);

void app_interconnection_disconnected_callback(void);

void app_set_interconnection_discoverable(uint8_t discoverable);

uint8_t app_get_interconnection_discoverable(void);

#ifdef __cplusplus
}
#endif
#endif  // __APP_INTERCONNECTION_H__
#endif  // #ifdef __INTERCONNECTION__

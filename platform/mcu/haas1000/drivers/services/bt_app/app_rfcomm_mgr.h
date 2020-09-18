#ifndef __APP_RFCOMM_MGR_H__
#define __APP_RFCOMM_MGR_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"

#if defined(__3M_PACK__)
#define L2CAP_MTU                           980
#else
#define L2CAP_MTU                           672
#endif

#define SPP_MAX_PACKET_SIZE     L2CAP_MTU
#define MAXIMUM_RFCOMM_TX_SIZE              ((L2CAP_MTU) - 10)
#define SPP_TX_BUF_DATA_SIZE      4096
#define SPP_MAX_PACKET_NUM \
  ((int)(SPP_TX_BUF_DATA_SIZE / MAXIMUM_RFCOMM_TX_SIZE))

typedef enum
{
    RFCOMM_INCOMING_CONN_REQ = 0,
    RFCOMM_CONNECTED,
    RFCOMM_DISCONNECTED,
    RFCOMM_TX_DONE,
    RFCOMM_UNKNOWN_EVENT
} RFCOMM_EVENT_E;

typedef bool (*rfcomm_callback_func)(RFCOMM_EVENT_E event, uint8_t serviceIndex,
    uint16_t connHandle, uint8_t* pBtAddr, uint8_t* pSentDataBuf, uint16_t sentDataLen);
    
typedef int (*spp_handle_data_event_func_t)(void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen);

typedef struct
{
    uint8_t     rfcomm_ch;
    uint8_t     tx_pkt_cnt;
    uint32_t    app_id;
    const uint8_t*          rfcomm_128bit_uuid;
    rfcomm_callback_func    callback;
    spp_handle_data_event_func_t spp_handle_data_event_func;
    osMutexId   mutexId;
} RFCOMM_CONFIG_T;

int8_t app_rfcomm_open(RFCOMM_CONFIG_T* ptConfig);
void app_rfcomm_close(uint8_t instanceIndex);
int8_t app_rfcomm_write(uint8_t instanceIndex, const uint8_t* pBuf, uint16_t length);
void app_rfcomm_read(uint8_t instanceIndex, uint8_t* pBuf, uint16_t* ptrLen);
void app_rfcomm_mgr_init(void);
uint8_t app_rfcomm_get_instance_index(void* dev);
osThreadId app_rfcomm_get_rx_thread_id(uint8_t serviceIndex);
void app_rfcomm_services_add_sdp(void);
void app_rfcomm_services_remove_sdp(void);

#ifdef __cplusplus
}
#endif

#endif // __APP_RFCOMM_MGR_H__


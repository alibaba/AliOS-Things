#ifndef __APP_INTERACTION_H__
#define __APP_INTERACTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"
#include "stdint.h"


// connection related definitions
#define DEFAULT_INTERACTION_BLE_DATA_SEC_SIZE         (20)
#define DEFAULT_INTERACTION_SPP_DATA_SEC_SIZE         (600)

#define INTERACTION_SPP_DEFAULT_MTU_VALUE             (600)

#define APP_INTERACTION_PATH_TYPE_SPP                 (1 << 0)
#define APP_INTERACTION_PATH_TYPE_BLE                 (1 << 1)
#define APP_INTERACTION_PATH_TYPE_HFP                 (1 << 2)
#define APP_INTERACTION_PATH_TYPE                     (APP_INTERCONN_PATH_TYPE_BLE | APP_INTERCONN_PATH_TYPE_SPP)

#define APP_INTERACTION_SPP_CONNECTED                 (1 << 0)
#define APP_INTERACTION_BLE_CONNECTED                 (1 << 1)
#define APP_INTERACTION_SPP_BLE_BOTH_CONNECTED        (APP_INTERCONN_SPP_CONNECTED | APP_INTERCONN_BLE_CONNECTED)

#define APP_INTERACTION_SPP_DISCONNECTED              (~(1 << 0))
#define APP_INTERACTION_BLE_DISCONNECTED              (~(1 << 1))
#define APP_INTERACTION_SPP_BLE_BOTH_DISCONNECTED     (0)


#define INTERACTION_DEVICE_CHECK(dev)                                   \
    do                                                                   \
    {                                                                    \
        if (!dev)                                                        \
            ASSERT(0, "false spp device, %s, %d\n", __func__, __LINE__); \
    } while (0)


#define SPP_MAX_PACKET_SIZE 600
#define SPP_MAX_PACKET_NUM 5

typedef enum 
{
    INTERACTION_SUCESS = 0x00,
    INTERACTION_FAILURE ,
    INTERACTION_INVALIDATE_PARAMETER,  
    INTERACTION_VERIFICATION_FAILURE,   
    INTERACTION_VALIDATION_FAILURE,  
    INTERACTION_TIME_OUT_WITHOUT_ACK,   
    INTERACTION_TIME_OUT, 
    INTERACTION_WRITTING_FAILURE,    
} INTERACTION_RSP_STATUS_E;

typedef struct {
    uint8_t         connType;
    uint8_t         conidx;
    uint16_t        DataSectorSize;
}  __attribute__((__packed__)) INTERACTION_ENV_T;

typedef struct
{
    void *   pDev;
    uint8_t  process;
    uint8_t *data;
    uint32_t dataLen;
} INTERACTION_MAIL_T;

typedef struct
{
    uint16_t    cmdID;
    uint8_t     transferID;
    uint16_t    paramLen;
    uint8_t     *param;
}__attribute__ ((__packed__)) INTERACTION_PAYLOAD_T;

typedef struct
{
    uint8_t                     cmdSOF;
    uint8_t                     cmdLen;
    uint16_t                    cmdCtrol;
    //uint16_t                  cmdFSN;
    INTERACTION_PAYLOAD_T       cmdPayload;
//  uint16_t                    cmdChecksum;
}__attribute__ ((__packed__)) INTERACTION_CMD_T;

typedef struct
{
    uint8_t                     cmdSOF;
    uint16_t                    cmdLen;
    uint16_t                    cmdCtrol;
    //uint16_t                  cmdFSN;
    INTERACTION_PAYLOAD_T       cmdPayload;
//  uint16_t                    cmdChecksum;
}__attribute__ ((__packed__)) INTERACTION_LONG_CMD_T;



typedef enum {
    INTERACTION_PROCESS_INVALID = 0,
    INTERACTION_PROCESS_READ    = 1,
    INTERACTION_PROCESS_WRITE   = 2,
    INTERACTION_PROCESS_MAX,
} INTERACTION_PROCESS_E;



#define     INTERACTION_SUCCESS_STATUS      0
#define     INTERACTION_FAILURE_STATUS      1
#define     NUMBER_OF_PAIRS                 1   

#define     INTERACTION_CMD_PACKET_SOF                  0xAA


/***************get remote capabilities***************/
#define INTERACTION_GET_REMOTE_CAPABILITIES_REQ    0x0100
#define INTERACTION_GET_REMOTE_CAPABILITIES_RSP    0x8100

typedef struct
{
   uint16_t         cmdID;
   uint8_t         transferID;
   uint16_t        paramLen;
}__attribute__ ((__packed__)) INTERACTION_REMOTE_CAPABILITIES_REQ_T;

typedef struct
{
   uint16_t         cmdID;
   uint8_t         transferID;
   uint16_t        paramLen;
   uint8_t         status;
   uint8_t         featureValue;
}__attribute__ ((__packed__)) INTERACTION_REMOTE_CAPABILITIES_CFM_T;



/*********************MTU***************************/
 #define    INTERACTION_MTU_CMD_REQ             0x0101
 #define    INTERACTION_MTU_CMD_CFM             0x8101

  typedef struct
 {
    uint16_t         cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    uint8_t         mtuValue;
 }__attribute__ ((__packed__)) INTERACTION_MTU_REQ_T;

 typedef struct
 {
    uint16_t         cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    uint8_t         status;
    uint16_t         mtuValue;
 }__attribute__ ((__packed__)) INTERACTION_MTU_CFM_T;
 

 /*********************remote VID***************************/
#define    INTERACTION_REMOTE_VID_CMD_REQ              0x0102
#define    INTERACTION_REMOTE_VID_CMD_CFM              0x8102

typedef struct
{
    uint16_t         cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    uint16_t         remoteVID;
 }__attribute__ ((__packed__)) INTERACTION_VID_REQ_T;
 typedef struct
 {
    uint16_t         cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    uint8_t         status;
    uint16_t        remoteVID;
 }__attribute__ ((__packed__)) INTERACTION_VID_CFM_T;

 /*********************remote PID***************************/
 #define    INTERACTION_REMOTE_PID_CMD_REQ             0x0103
#define     INTERACTION_REMOTE_PID_CMD_CFM             0x8103

typedef struct
{
    uint16_t        cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    //uint8_t         remotePID;
 }__attribute__ ((__packed__)) INTERACTION_PID_REQ_T;
 typedef struct
 {
    uint16_t        cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    uint8_t         status;
    uint8_t         remotePID[3];
 }__attribute__ ((__packed__)) INTERACTION_PID_CFM_T;
 

 /*********************protocol version***************************/
#define INTERACTION_PROTOCOL_VERSION_CMD_REQ        0x0104
#define INTERACTION_PROTOCOL_VERSION_CMD_CFM        0x8104

typedef struct
{
    uint16_t        cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    uint8_t         *version;
 }__attribute__ ((__packed__)) INTERACTION_PROTOCOL_VERSION_REQ_T;
 typedef struct
{
    uint16_t        cmdID;
    uint8_t         transferID;
    uint16_t        paramLen;
    uint8_t         status;
    uint8_t         version[3];
 }__attribute__ ((__packed__)) INTERACTION_PROTOCOL_VERSION_CFM_T;

 /*********************remote version***************************/
 #define    INTERACTION_REMOTE_VERSION_CMD_REQ      0x0105
 #define    INTERACTION_REMOTE_VERSION_CMD_CFM      0x8105
   typedef struct
 {
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
  }__attribute__ ((__packed__)) INTERACTION_REMOTE_VERSION_REQ_T;
 
  typedef struct
 {
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
     uint8_t         numbersOfKey;
     uint8_t         version[3];
  }__attribute__ ((__packed__)) INTERACTION_REMOTE_VERSION_CFM_T;



/************battery infomation start***********/

#define     INTERACTION_BATTERY_CMD_REQ         0x0106
#define     INTERACTION_BATTERY_CMD_CFM         0x8106

typedef struct
{
    uint16_t cmdID;
    uint8_t transferID;
    uint16_t paramLen;  
}__attribute__ ((__packed__)) INTERACTION_BATTERY_REPORT_REQ_T;
    
typedef struct
{
    uint16_t cmdID;
    uint8_t transferID;
    uint16_t paramLen;
    uint8_t status;
    uint8_t nums_of_pairs;
    uint8_t earphone_info;
    uint8_t battery_value;  
}__attribute__ ((__packed__)) INTERACTION_BATTERY_REPORT_CFM_T;


/************upgrade capability info start***************/

#define     INTERACTION_UPGRADE_CAPABITITY_CMD_REQ      0x0107
#define     INTERACTION_UPGRADE_CAPABITITY_CMD_CFM      0x8107

#define     INTERACTION_UPGRADE_SUPPORT             1

typedef struct
{
    uint16_t cmdID;
    uint8_t transferID;
    uint16_t paramLen;
}__attribute__ ((__packed__)) INTERACTION_UPGRADE_REQ_T;


typedef struct
{
    uint16_t cmdID;
    uint8_t transferID;
    uint16_t paramLen;
    uint8_t status;
    uint8_t nums_of_pairs;
    uint16_t  info;
}__attribute__ ((__packed__)) INTERACTION_UPGRADE_CFM_T;

/************key function***************/
#define     INTERACTION_UPGRADE_KEY_FUNCTION_REQ        0x0108
#define     INTERACTION_UPGRADE_KEY_FUNCTION_CFM        0x8108

typedef struct
{
    uint16_t   cmdID;
    uint8_t    transferID;
    uint16_t   paramLen;
}__attribute__ ((__packed__)) INTERACTION_KEY_FUNCTIONS_REQ_T;


typedef struct
{
    uint16_t   cmdID;
    uint8_t    transferID;
    uint16_t   paramLen;
    uint8_t    status;
    uint8_t    nums_of_array;
    uint8_t    earphone_info[8];
}__attribute__ ((__packed__)) INTERACTION_KEY_FUNCTIONS_CFM_T;

/************earbus status***************/
#define     INTERACTION_UPGRADE_EARBUDS_STATUS_REQ      0x0109
#define     INTERACTION_UPGRADE_EARBUDS_STATUS_CFM      0x8109

typedef struct
{
    uint16_t   cmdID;
    uint8_t    transferID;
    uint16_t   paramLen;
}__attribute__ ((__packed__)) INTERACTION_EARBUDS_STATUS_REQ_T;

typedef struct
{
    uint16_t   cmdID;
    uint8_t    transferID;
    uint16_t   paramLen;
    uint8_t    status;
    uint8_t    nums_of_array;
    uint8_t    earbuds_info[4];
}__attribute__ ((__packed__)) INTERACTION_EARBUDS_STATUS_CFM_T;



/************notification command start***************/
#define     INTERACTION_GET_NOTIFICATION_CAPABILITY_CMD_REQ     0x0200
#define     INTERACTION_GET_NOTIFICATION_CAPABILITY_CMD_CFM     0x8200
typedef struct
{
    uint16_t   cmdID;
    uint8_t    transferID;
    uint16_t   paramLen;
}__attribute__ ((__packed__)) INTERACTION_GET_NOTIFICATION_REQ_T;

typedef struct
{
    uint16_t   cmdID;
    uint8_t    transferID;
    uint16_t   paramLen;
    uint8_t    status;
    uint8_t    nums;
    uint16_t    eventType;
}__attribute__ ((__packed__)) INTERACTION_GET_NOTIFICATION_CFM_T;

#define     INTERACTION_REGISTER_NOTIFICATION_CMD_REQ               0x0201
#define     INTERACTION_REGISTER_NOTIFICATION_CMD_CFM               0x8201
typedef struct
{   
    
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         eventType;
     uint8_t         eventCode;
}__attribute__ ((__packed__)) INTERACTION_REGISTER_NOTIFICATION_REQ_T;
typedef struct
{   
    
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION_REGISTER_NOTIFICATION_CFM_T;


#define     INTERACTION_GET_REGISTERED_NOTIFY_CMD_REQ       0x0202
#define     INTERACTION_GET_REGISTERED_NOTIFY_CMD_CFM       0x8202
typedef struct
{   
    
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION_GET_REGISTERED_NOTIFICATION_REQ_T;
typedef struct
{   
    
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
     uint8_t         eventType;
     uint8_t         nums;  
     uint16_t        info;
}__attribute__ ((__packed__)) INTERACTION_GET_REGISTERED_NOTIFICATION_CFM_T;



#define     INTERACTION_CANCEL_REGISTERED_NOTIFY_CMD_REQ    0x0203
#define     INTERACTION_CANCEL_REGISTERED_NOTIFY_CMD_CFM    0x8203
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION_CANCEL_REGISTERED_NOTIFICATION_REQ_T;
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION_CANCEL_REGISTERED_NOTIFICATION_CFM_T;



#define     INTERACTION_NOTIFY_EVENT_CMD_REQ                    0x0204
#define     INTERACTION_NOTIFY_EVENT_CMD_CFM                    0x8204
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         eventType;
     uint8_t         eventData;
}__attribute__ ((__packed__)) INTERACTION__NOTIFICATION_EVENT_REQ_T;
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION_NOTIFICATION_EVENT_CFM_T;



#define     INTERACTION_BATTERY_CHANGED_EVENT_CMD_REQ       0x0001
#define     INTERACTION_BATTERY_CHANGED_EVENT_CMD_CFM       0x8001
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION__BATTERY_CHANGED_REQ_T;

typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION__BATTERY_CHANGED_CFM_T;

#define     INTERACTION_EARBUDS_CHANGED_EVENT_CMD_REQ       0x0002
#define     INTERACTION_EARBUDS_CHANGED_EVENT_CMD_CFM       0x8002
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION__EARBUDS_CHANGED_REQ_T;

typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION__EARBUDS_CHANGED_CFM_T;

/************notification command end***************/


/***********************set command start***************************/
#define   INTERACTION_SET_FIND_MODE_CMD_REQ       0x0400
#define   INTERACTION_SET_FIND_MODE_CMD_CFM       0x8400

typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         eventType;
}__attribute__ ((__packed__)) INTERACTION_SET_MODE_REQ_T;

typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
}__attribute__ ((__packed__)) INTERACTION_SET_MODE_CFM_T;


#define   INTERACTION_SET_KEY_FUNCTIONS_CMD_REQ       0x0401
#define   INTERACTION_SET_KEY_FUNCTIONS_CMD_CFM       0x8401
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         *info;
}__attribute__ ((__packed__)) INTERACTION_SET_kEY_FUNCTIONS_REQ_T;

typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
     uint8_t         status;
}__attribute__ ((__packed__)) INTERACTION_SET_KEY_FUNCTIONS_CFM_T;


#define     INTERACTION_LISTEN_MUSIC_CMD_REQ        0x0402
typedef struct
{   
     uint16_t        cmdID;
     uint8_t         transferID;
     uint16_t        paramLen;
}__attribute__ ((__packed__)) INTERACTION_LISTEN_MUSIC_REQ_T;

/***********************set command end***************************/


/***********ota command start***************/
#define     INTERACTION_OTA_UPGRADE_SYS_REQ                 0x0300
#define     INTERACTION_OTA_UPGRADE_SYS_CFM                 0x8300
#define     INTERACTION_OTA_UPGRADE_START_REQ               0x0301
#define     INTERACTION_OTA_UPGRADE_START_CFM               0x8301
#define     INTERACTION_OTA_UPGRADE_DATA_REQ                0x0302
#define     INTERACTION_OTA_UPGRADE_DATA_CFM                0x8302
#define     INTERACTION_OTA_UPGRADE_BLOCK_VERIFY_REQ        0x0303
#define     INTERACTION_OTA_UPGRADE_BLOCK_VERIFY_CFM        0x8303
#define     INTERACTION_OTA_UPGRADE_IS_VALIDATION_DONE_REQ  0x0304
#define     INTERACTION_OTA_UPGRADE_IS_VALIDATION_DONE_CFM  0x8304
#define     INTERACTION_OTA_UPGRADE_TRANSFER_COMPLETE_IND   0x0305
#define     INTERACTION_OTA_UPGRADE_TRANSFER_COMPLETE_CFM   0x8305

#define     INTERACTION_OTA_UPGRADE_COMMIT_REQ              0x0306
#define     INTERACTION_OTA_UPGRADE_COMMIT_CFM              0x8306
#define     INTERACTION_OTA_UPGRADE_ABORT_REQ               0x0307
#define     INTERACTION_OTA_UPGRADE_ABORT_CFM               0x8307
#define     INTERACTION_OTA_UPGRADE_ERROR_IND               0x0308
/***********ota command end***************/





//add new code begin
void app_interaction_spp_open(void);
int interaction_spp_handle_data_event_func(void *pDev, uint8_t process, uint8_t *pData, uint16_t dataLen);



void app_interaction_report_battery_level(void);
void app_interaction_get_upgrade_capability(void);

void app_interaction_register_notification_hanlder(void);
void app_interaction_send_cmd_handler(uint8_t* ptrData, uint32_t dataLength);

void app_interaction_get_MTU_handler(uint8_t* ptrData, uint32_t dataLength);
void app_interaction_get_remoteVID_handler(uint8_t* ptrData, uint32_t dataLength);

bool app_interaction_check_the_length(uint8_t* ptrData, uint32_t dataLength);

extern void app_interaciton_listen_music_handler(void);

uint8_t app_interaction_get_seq_number(void);




//add new code end

uint8_t app_interaction_get_ota_status();

extern void app_interaction_init();

void app_interaction_connected(uint8_t conidx, uint8_t connType);


void app_interaction_disconnected(uint8_t disconnType);

void app_interaction_send_done(void);

uint8_t app_interaction_get_conn_status();

uint16_t app_interaction_get_datasectorsize(void);
void  app_interaction_spp_receive_data_handler(uint8_t* ptrData, uint32_t dataLength);



#ifdef __cplusplus
}
#endif
#endif  // __APP_TENCENT_OTA_H__

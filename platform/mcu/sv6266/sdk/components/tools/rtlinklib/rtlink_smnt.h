
/*************************************

    Package: RTlink Smnt algorithm
    Version: 1.0
    
    Copyright (C) 2018 RangerTek Inc. All Rights Reserved.
    
*************************************/

#ifndef _RTLINK_SMNT_H_
#define _RTLINK_SMNT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef  char                  	int8;
typedef  unsigned char        	uint8;
typedef  short      	             int16;
typedef  unsigned short      	uint16;

#define MAC_ADDR_LEN 6
#define SSID_MAX_LEN 32
#define PSW_MAX_LEN 64
#define KEY_MAX_LEN 16
#define STR_END_CHR_LEN 1
#define SSID_STR_MAX_LEN SSID_MAX_LEN + STR_END_CHR_LEN
#define PSW_STR_MAX_LEN PSW_MAX_LEN + STR_END_CHR_LEN
#define KEY_STR_MAX_LEN KEY_MAX_LEN + STR_END_CHR_LEN
#define RESEARCH_CH_MAX_CNT 20
#define RESET_CH_CNT 5
#define PAYLOAD_MIN								(3)
#define PAYLOAD_MAX								(140)
#define PAYLOAD_MIN_SEQ							(3)
#define PAYLOAD_MAX_SEQ							(64)

typedef enum{
    RTState_Init = 0x1,             /**< RTState: Init*/
    RTState_Locking = 0x2,     /**< RTState: Locking, search channel and need locked*/
    RTState_Locked = 0x4,      /**< RTState: Locked, begin locked channel and receive raw packet*/
    RTState_Finish = 0x8         /**< RTState: Finish, get ssid & password ready*/
} RTState;

typedef enum {
    RTBand_2_4G,        /**< RTBand: 2.4G channel*/
    RTBand_5G,            /**< RTBand: 5G channel*/
    RTBand_Mixed      /**< RTBand: 2.4G & 5G channel*/
} RTBand;

typedef enum {
    RTProtocol_None = 1,        /**< Protocol: No security type*/
    RTProtocol_Aes,           /**< Protocol: Aes ecb security type*/
    RTProtocol_Unkown    /**<Protocol: Unknown security  type*/
} RTProtocol;

typedef enum {
    RTPkt_Beacon,              /**< Packet type: beacon*/
    RTPkt_Multicast,          /**< Packet type: Multicast*/
    RTPkt_Other                  /**< Packet type: Other*/
} RTPkt;

typedef enum {
    RTStatus_Error,                  /**<Status: normal error*/
    RTStatus_Ok,                      /**<Status: normal pass*/
    RTStatus_Params_Error,  /**<Status: type: parameters error */
    RTStatus_Params_Ok       /**<Status: type: parameters pass */
} RTStatus;

typedef struct {
    uint16 ver:2;                  // Protocol version
    uint16 type:2;               // MSDU type
    uint16 subType:4;        // MSDU subtype
    uint16 toDs:1;              // To DS indication
    uint16 frDs:1;               // From DS indication
    uint16 moreFrag:1;     // More fragment bit
    uint16 retry:1;             // Retry status bit
    uint16 pwrMgmt:1;    // Power management bit
    uint16 moreData:1;    // More data bit
    uint16 wep:1;              // Wep data
    uint16 order:1;            // Strict order expected
} FrameControl;

typedef struct {
    FrameControl fc;
    uint16 duration;
    uint8 addr1[MAC_ADDR_LEN];
    uint8 addr2[MAC_ADDR_LEN];
    uint8 addr3[MAC_ADDR_LEN];
    uint16 frag:4;
    uint16 sequence:12;
    uint8 octet[0];
} *Header80211;

typedef struct {
    uint8 ssidLen;                                     /**< ssid total len*/
    uint8 pwdLen;                                    /**< password total len*/
    uint8 ssid[SSID_STR_MAX_LEN];   /**< ssid data of string*/
    uint8 pwd[PSW_STR_MAX_LEN];  /**< password data of string*/
} RTResult;

typedef struct {
    uint8 secretKey[KEY_STR_MAX_LEN];    /**< security key storage*/
    void (*switchChCallback)(uint8);             /**< switch channel callback*/
    void (*resultCallback)(RTResult);            /**< result callback*/
} RTParams;

typedef struct{
    uint8 encData[PAYLOAD_MAX];          /**< enc data buffer*/
} SmntEncData;

typedef struct{
    uint8 syncAppMac[MAC_ADDR_LEN];
    uint8 syncBssid[MAC_ADDR_LEN];
    uint8 getBeacon;
    uint8 syncSsidLen;
    uint8 syncSsid[SSID_STR_MAX_LEN];
    uint8 chIndex;
    uint8 resetChCnt;
    uint8 payload_multicast[PAYLOAD_MAX];
    RTState state;
    SmntEncData enc;
    RTParams param;
    RTResult result;
}rtlinkSmnt_t;

/**
 * Starts the rtlink.
 *
 * - To start the rtlink, and init parameters
 *
 * @param   param     An initialized rtlink, and registered switch channel & result callback
 */
void RTSmntInit(
    RTParams param);

/**
 * release the rtlink.
 *
 * - To release the rtlink
 */
void RTSmntRelease(
    void);

/**
 * reset the rtlink.
 *
 * - To reset the rtlink
 */
void RTSmntReset(
    void);

/**
 * Receive the raw packet
 *
 * - receive the raw packet, that rtlink algorithm can be work
 *
 * @param   pHeader     raw packet header is 802.11 format
 * @param   length         raw packet len
 */
void RTSmntDatahandler(
    Header80211 pHeader,
    int length); 

/**
 * setting sniffer bandwidth
 *
 * -setting sniddfer bandwidth, that can be receive raw packet
 *
 * @param   type     2.4G or 5G or Mixed  type
 */
void RTSmntSetBand(
    RTBand type);

/**
 * setting cyclecall with change channel
 *
 * -setting cyclecall with change channel, thate can be receive raw packet with correct channel
 *
 * @return RTStatus check this status and user will  make some decisions
 */
RTStatus RTSmntCycleCall(
    void);

#ifdef __cplusplus
}
#endif

#endif

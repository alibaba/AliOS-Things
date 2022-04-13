/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef _HSHF_API_H
#define _HSHF_API_H

#include "btlib_type.h"
#include "rfcomm_api.h"

//#define PRO_PCDEMO
/////////////////////////////////////////////////////////////////
///common
#define HF_RECVDATA_BUF_SIZE  50
#define HS_RECVDATA_BUF_SIZE  48
/* notify upper layer */
enum hshf_event_t {
    /* user command event */
    HSHF_ENTER_PAIRING = 1,
    HSHF_EXIT_PAIRING,
    HF_DIAL_NUM_REQ,
    HF_ANSWER_REQ,
    HF_REJECT_REQ,
    HF_ENDCALL_REQ,
    HF_TRANSFER_REQ,
    HF_DIAL_LASTNUM_REQ,
    HF_TRANSMIT_DTMF,
    HF_VOICE_DIAL_REQ,          //10
    HF_VOICE_DIAL_CANCEL_REQ,
    HSHF_CONNECT_REQ,
    HF_DISCONNECT_REQ,
    HSHF_SPK_VOL_UP,
    HSHF_SPK_VOL_DOWN,
    HSHF_TOGGLE_MIC,
    HSHF_TOGGLE_LED,
    HSHF_TOGGLE_VOLBTN,
    HF_ANSWER_ENDCALL_CONN,     //  answer @ ring, end @ talking, connect@idle
    HF_REJ_REDIAL_TRANS_CONN,   // reject @ ring, redial @ connected, transfer @ talking, connect@idle
    HF_RELEASE_HOLD_REJECT_WAIT_REQ,    //21
    HF_RELEASE_ACTVIE_ACCEPT_OTHER_REQ,
    HF_HOLD_ACTIVE_ACCEPT_OTHER_REQ,
    HF_CONFERENCE_REQ,

    HSHF_SET_PB_STORAGE,
    HSHF_GET_PB_ITEM,
    /* internal event */
    HSHF_EVNT_BEGIN,            //27
    HSHF_CONN_OPENED,           //28
    HSHF_CONN_CLOSED,
    HSHF_CONN_REQ_FAIL,
    HF_AG_SUPPORTED_FEATURE_IND,
    HF_AG_SUPPORTED_INDICATOR_IND,
    HF_AG_CURRENT_INDICATOR_IND,
    HF_INDICATOR_EVENT_IND,
    HF_CIEV_CALL_IND,
    HF_CIEV_SERVICE_IND,
    HF_CIEV_CALLSETUP_IND,
    HF_CALLER_ID_IND,
    HF_VOICE_IND,
    HSHF_RING_IND,              //40
    HSHF_AUDIOCONN_OPENED,
    HSHF_AUDIOCONN_CLOSED,
    HSHF_SPK_VOL_IND,
    HSHF_MIC_VOL_IND,
    HF_IN_BAND_RING_IND,
    HSHF_PAIR_OK,               //46
    HSHF_PAIR_TOUT,
    HSHF_PAIR_FAILED,
    //// NEW for three way call
    HF_CIEW_CALLHELD_IND,
    HF_CCWA_IND
};

enum hshf_callsetup_enum {
    CALL_SETUP_NONE,
    CALL_SETUP_INCOMING,
    CALL_SETUP_OUTGOING,

    CALL_SETUP_REMOTE_ALERT,
    CALL_SETUP_ESTABLISHED
};

enum hshf_call_enum {
    CALL_NONE = 0,
    CALL_ESTABLISHED
};

enum hshf_profile_enum {
    PRO_BOTH = 0,
    PRO_HEADSET,
    PRO_HANDSFREE,
    PRO_EXIT,
    PRO_SHOW
};

enum hshf_conn_state_enum {
    STOP,
    STANDBY = 1,
    PAIRING,
    LISTENING,                  //ready
    CONNECTING,
    CONNECTED,
    SCOCONNECTED
};

/////////////hdzhu added--20090505---
enum hshf_pb_location_enum {
    LOCATION_SM = 0,
    LOCATION_ME,
    LOCATION_MT,
    LOCATION_DC,
    LOCATION_RC,
    LOCATION_MC,
    LOCATION_LD
};

enum hshf_pb_action_enum {
    ACTION_PREV = 0,
    ACTION_NEXT
};

//#define MAX_DIAL_NUM_SIZE     0x10
//#define MAX_SAVED_CALL_NUM    4
struct hshf_control_t {

    struct bdaddr_t remote;

    uint8 rfcomm_handle;

    uint8 listen_channel;

    uint8 disc_reason;

    uint8 profile_set;          /*0~2 *//* 0 - both, if set to both, profile_use depends on AG */

    uint8 profile_use;          /*1~2 *//* 1 - headset only */

    /* 2 - handsfree only */

    uint8 idxCall;              /* Phone status info - index "call"in +CIND - call    */

    uint8 idxService;           /* Phone status info - service index in +CIND - service */

    uint8 idxCallSetup;         /* Phone status info - Bluetooth proprietary call setup status from +CIND - callsetup */

    uint8 idxCallHeld;          /* Phone status info - BLuetooth proprietary call hold status from +CIND - callheld */

    uint8 statusCall;           /* Phone status info - call */

    uint8 statusService;        /* Phone status info - service */

    uint8 statusCallSetup;      /* Phone status info - callsetup */

    uint8 statusCallHeld;       /* Phone status info - callheld */

    int8 hfSupportedFeature;    /*hf supported feature bitmap:            */

    /* bit 0 - EC/NR function                */
    /*     1 - Call waiting and 3-way calling */
    /*     2 - CLI presentation capability   */
    /*     3 - Voice recognition activation   */
    /*     4 - Remote volume control         */
    /*     5 - Enhance call status            */
    /*     6 - Enhanced call control         */
    int16 agSupportedFeature;   /* AG supported feature bitmap  */

    /* bit0 - 3-way calling      */
    /*    1 - EC/NR function    */
    /*    2 - Voice recognition  */
    /*    3 - In-band ring tone */
    /*    4 - Attach a number to a voice tag */
    /*    5 - Ablility to reject a call */
    /*    6 - Enhanced call status     */
    /*    7 - Enhanced call control     */
    /*    8 - extended error result codes */

    enum hshf_conn_state_enum state;    /*check if it is connecting now, if it is connecting,the hf_connect_req will not work */

    uint32 isDialNumPending:1;  /* setup slc connection before dial */

    uint32 isDialLastNumPending:1;  /* setup slc connection before dial */

    uint32 isTransferPending:1; /* setup slc connection before transfer */

    uint32 isBvraPending:1;

    uint32 inbandRingEnabled:1; /*0-disable; 1-enable */

    uint32 bvraEnabled:1;

    uint32 brsf:1;

    uint32 isPBStorage:1;

    uint32 isPBItem:1;

//    uint32 isAutoAnswer:1;
//    uint32 isAutoConn:1;
    uint32 isDialedCallHistory:1;

    uint32 isReceivedCallHistory:1;

    uint32 isMissedCallHistory:1;

    uint32 unused:20;

    uint8 statusRing;           /* ring status info */

    uint8 dialNum[MAX_DIAL_NUM_SIZE];   /* dialed number 0-9, *, #, + */

#if 0
    uint8 dialedCallHistory[MAX_SAVED_CALL_NUM][MAX_DIAL_NUM_SIZE];

    uint8 receivedCallHistory[MAX_SAVED_CALL_NUM][MAX_DIAL_NUM_SIZE];

    uint8 missedCallHistory[MAX_SAVED_CALL_NUM][MAX_DIAL_NUM_SIZE];

#endif                          /* 
                                 */
    char dtmf;                  /* dtmf character, 0-9,*,#,A-D */

    uint8 spkVol;               /* should be implemented in hardware */

    uint8 micVol;               /* should be implemented in hardware */

    uint8 pb_storage;

    uint8 pb_prev_next;

    uint16 idx_pb;

    char callerId[MAX_DIAL_NUM_SIZE];   /* incoming caller id */

//    char recvBuf[HF_RECVDATA_BUF_SIZE];
    char *recvBuf;

    void (*indicate) (uint8 event, void *pData);

};

extern struct hshf_control_t hshf_ctl;

// control list node
struct hshf_control_t_list_node {

    struct hshf_control_t hshf_ctrl;

    struct hshf_control_t_list_node *p_next;

};

// control list head
//extern struct hshf_control_t_list_node* p_hshf_ctrl_list_head = NULL;
/*---- hshf_app.c ----*/
void app_callback(uint8 event, void *pData);

/*----hshf.c----*/
enum hshf_conn_state_enum hshf_getState(void);

void hshf_setState(enum hshf_conn_state_enum state);

int8 hshf_connectReq(void);

int8 hshf_sendCmd(const char *data, uint32 len);

int8 hshf_atCmdParse(void);

int8 hshf_turnOn(void);

int8 hshf_turnOff(void);

int8 hshf_enterPairing(void);

int8 hshf_readPBReq(uint8 direction);

int8 hshf_selectPBReq(uint8 location);

//void hshf_setAutoAnswer(BOOLEAN flag);
//void hshf_setAutoConn(BOOLEAN flag);
uint8 hshf_getAutoAnswer(void);

uint8 hshf_getAutoConn(void);

void hshf_returnCallHistory2Host(uint8 location);

void hshf_cleanCallHistory(void);

void hshf_multi_dev_switch(void);

/*----volume.c----*/
int8 hshf_getSpkVol(void);

int8 hshf_getMicVol(void);

void hshf_setSpkVol(int8 vol);

void hshf_setMicVol(int8 vol);

int8 hshf_spkVolDown(void);

int8 hshf_spkVolUp(void);

void hshf_toggleMic(void);

void hshf_enableMic(void);

void hshf_disableMic(void);

#ifdef PRO_PCDEMO
/* ----profile.c----*/
uint8 hshf_showProfile(void);

#endif /* 
        */
int8 hshf_setProfile(uint8 profile);

uint8 hshf_showProUse(void);

/////////////////////////////////////////////////////////
/////handsfree
typedef struct {

    uint8 length;

    char *caller_id;

} hf_caller_id_ind_t;

/*---- hfp.c ----*/
uint16 hf_getAgSupportedFeature(void);

int8 hf_releaseSCO(struct bdaddr_t *bdaddr, uint8 reason);

/*----connect.c----*/
int8 hf_disconnectReq(void);

int8 hshf_exit_sniff(void);

/*---- call.c ----*/
int8 hf_answer(void);

int8 hf_reject(void);

int8 hf_endCall(void);

int8 hf_transferReq(void);

uint8 hf_isTransferPending(void);

int8 hf_callerIdInd(hf_caller_id_ind_t * ind);

uint8 hf_getCallStatus(void);

uint8 hf_getServiceStatus(void);

uint8 hf_getCallSetupStatus(void);

uint8 hf_acceptSecondCall(uint8 h_type);

/*---- dial.c ----*/
#ifdef PRO_PCDEMO
int8 hf_dialMemory(const uint8 * mem, uint8 length);

void hf_setDialNum(uint8 * num, uint8 len);

void hf_setDialMemory(uint8 * mem, uint8 len);

#endif /* 
        */
int8 hf_dialNum(const uint8 * dialNum, uint8 numLen);

//uint8 hf_isDialNumPending( void );
int8 hf_dialLastNum(void);

uint8 hf_isDialLastNumPending(void);

int8 hf_dialVoice(uint8 enable);

/*---- dtmf.c ----*/
int8 hf_transmitDtmf(char dtmf);

#ifdef PRO_PCDEMO
void hf_setDtmf(char dtmf);

/*---- nrec.c ----*/
//int8 hf_disableNrec( void );
#endif /* 
        */
/*---- ring.c ----*/
uint8 hf_isInbandRing(void);

void hshf_ring(void);

uint8 hshf_getRingStatus(void);

////////////////////////////////////////////////////////////////////////////////
//////headset

int8 hs_button_req(void);

#endif /* */

/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __TWS_ROLE_SWITCH__H__
#define __TWS_ROLE_SWITCH__H__

#include "bluetooth.h"

enum TWS_DATA_STRUCTURE
{
    BT_ME = 0,
    BT_HCI,
    CMGR_CONTEXT,
    
    BT_L2CAP,
    BT_RFC,
    
    RFCOMM_CHANNEL,
    AVRCP_CONTEXT,
    APP_BT_DEVICE,
    AVDEV_CONTEXT,
    SLAVE_SAVE_DATA_OK,
};

enum PSM_CONTEXT_TYPE
{
     PSM_CONTEXT_SDP     = 0x01,  //SDP
     PSM_CONTEXT_RFC     = 0x02,  //RFCOMM MUX,HFP and SPP share
     PSM_CONTEXT_AVDTP   = 0x04,  //A2DP
     PSM_CONTEXT_AVCTP   = 0x08,  //AVRCP
     
     PSM_CONTEXT_INVALID = 0x80,
};

enum PROFILE_CONTEXT_FLAG
{
    BT_COMMON_FLAG = 0x01,
    BT_RFC_MUX_FLAG = 0x02,
    BT_HFP_FLAG = 0x04,
    BT_A2DP_FLAG = 0x08,
    BT_A2DP_CONTINUE_FLAG = 0x10,
    BT_AVRCP_FLAG = 0x20,
    SPP_SERVER_INTERACTION_FLAG = 0x40,
    
    DATA_COMPLETE_FLAG = 0x80,
};

#define BT_ALL_CONTEXT_PSM    (PSM_CONTEXT_SDP | PSM_CONTEXT_RFC | PSM_CONTEXT_AVDTP | PSM_CONTEXT_AVCTP)  

#define BT_ALL_CONTEXT_FLAG   (BT_COMMON_FLAG | BT_RFC_MUX_FLAG | BT_HFP_FLAG | BT_A2DP_FLAG | BT_A2DP_CONTINUE_FLAG | BT_AVRCP_FLAG | SPP_SERVER_INTERACTION_FLAG)

#define BT_ALL_RFC_APP_ID     (BTIF_APP_HFP_PROFILE_ID | BTIF_APP_SPP_MASK) 

#define BT_EARPHONE_BASIC_APP_ID       (BTIF_APP_HFP_PROFILE_ID | BTIF_APP_A2DP_PROFILE_ID | BTIF_APP_AVRCP_PROFILE_ID)

enum TWS_CHANNEL_TYPE
{
     HF_RF_CHANNEL = 0,
     UNKNOWN_CHANNEL,
};

#ifdef __cplusplus
extern "C" {
#endif                          /*  */


#ifdef __cplusplus
}
#endif                          /*  */
#endif                          /* __ME_H */

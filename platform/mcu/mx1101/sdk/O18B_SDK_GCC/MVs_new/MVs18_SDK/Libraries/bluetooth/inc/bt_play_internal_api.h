////////////////////////////////////////////////////////////////////////////////////////////
//					Mountain View Silicon Tech. Inc.
//	Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
//					All rights reserved.
//
//  bt_play_internal_api.h
//
//  Internal used ONLY for play control
//  maintainer: Halley
////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BT_PLAY_INTERNAL_API_H__
#define __BT_PLAY_INTERNAL_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//IMPORTAMT:
// For a phone call procedure, stack will invoke the following status functions in step of:
//  1) BT_DS_CALL_START
//  2) BT_DS_SCO_STREAM_START (if any)
//  3) BT_DS_CALL_SCO_PAUSE
//  4) BT_DS_CALL_END

enum
{
    BT_DS_A2DP_STREAM_START = 1,
    BT_DS_A2DP_STREAM_SUSPEND,

    BT_DS_CALL_START,      //phone call started.
    BT_DS_SCO_STREAM_START,//one MUST update the handleID by invoking function GetScoHciHandle().
    BT_DS_CALL_SCO_STREAM_PAUSE, //pause, this may be invoked
    //  1)just before [BT_DS_CALL_ISO_END]
    //  2)transfer phonecall to phone
    //  from now on, Hci handle is NULL.
    BT_DS_CALL_END,        //phone call ended.
    BT_DS_CALL_AEC_SUPPORTED,

    BT_DS_SNIFF_MODE,
    BT_DS_ACTIVE_MODE,


};

//Call back function used internally for users cfg sco and a2dp playing channel
//
//[HistoryStatus] will be AUTOMATICALLY reset to zero when new BT device is connected.
//[HistoryStatus] is located in internal bt stack memory space.
void BTStackDataStatusCallBackFunc(uint8_t CBParameter, //OUT, can be one of BT_DS_XXXXXX
                                   uint8_t* HistoryStatus //IN/OUT,User used ONLY, size = 1 Byte.
                                  );

#ifdef __cplusplus
}
#endif//__cplusplus

#endif


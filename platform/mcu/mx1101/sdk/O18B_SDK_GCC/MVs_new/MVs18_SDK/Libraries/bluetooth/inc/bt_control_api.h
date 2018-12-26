////////////////////////////////////////////////////////////////////////////////////////////
//					Mountain View Silicon Tech. Inc.
//	Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
//					All rights reserved.
//
//  bt_control_api.h
//
//  Some BT source devices may ONLY support some of functions(or not support any one), It's OK.
//  maintainer: Halley
////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BT_CONTROL_API_H__
#define __BT_CONTROL_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//Start BT playing
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlPlay(void);

//Pause BT playing
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlPause(void);

//Play next song based on the current play list
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlNextSong(void);

//Play pre-song based on the current play list
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlPreSong(void);

//Volume Up for remote BT device
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlVolUp(void);

//Volume Down for remote BT device
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlVolDown(void);

//some BT control targets may NOT support this function
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlStop(void);

//some BT control targets may NOT support this function
//fast forward start,need not re-enter this command
//one may end this function by sending command  BTPlayCtrlEndFastBackForward();
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlFastForward(void);

//some BT control targets may NOT support this function
//fast backward start,need not re-enter this command
//one may end this function by sending command  BTPlayCtrlEndFastBackForward();
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlFastBackward(void);

//some BT control targets may NOT support this function
//ending the pre ff or fb function
//returns zero means error found(some command is on the way), one can try it again later.
uint8_t BTPlayCtrlEndFastBackForward(void);


/////Followings are HF phone call functions//////////////////////////////////////
//get current incoming phone call number string, returns number string pointer
uint8_t* BTGetCurInCallPhoneNum(void);

//accept one incoming phone call
void BTInCallAccept(void);

//reject one incoming phone call
void BTInCallReject(void);

//hand up current connected call
void BTCallHangUp(void);

//disable aec and nr
void BTCallDisableAEC(void);

//calling one phone number, PhoneNumString is NULL ended string and has the length less than 19 bytes.
void BTDialOnePhoneNumber(uint8_t *PhoneNumString);

//redial last number
void BTRedialLastCalledNum(void);

//sync speaker volume between phone and hf(bt music device) when phone call is on the way. 
//One may invoke this function in HF mode.
//Vol = 0 means mute
//Vol = 15 means max volume
//Phones(AG) just display picture, and HF local MUST do the volume change's implementation.
//returns 1 for OK.
uint8_t BTPhoneCallVolSyncSet(uint8_t Vol/*0-15*/);

//Get the sync remote(AG) speaker volume value [0-15] for HF mode
//returns 1 for OK.
uint8_t BtGetRemoteSpeakerVol(void);

//////////////////////////////////////////////////////////


//get current connected remote bt device's address pointer, NULL means NO bt device connected
uint8_t* GetCurConnectedRemDevAdd(void);

//disconnect connected BT device
//returns 1 for OK. 
uint8_t BTDisConnect(void);
void BTConnect(uint8_t* addr);

//disconnect A2dp and HF, SPP may keep connected status.
//returns 1 for OK. 
uint8_t BTDisConnectA2dpHf(void);

uint8_t BTDisConnectA2dp(void);
uint8_t BTDisConnectHf(void);
uint8_t BTDisConnectSpp(void);

//set scan and page enabled when the device is in idle mode.
//returns 1 for OK. If the device is in the connected status, this function returns 0.
//one may invoke GetCurBtConnectedFlag() to get the connected status.
uint8_t BTSetScanEnable(void);

//set scan and page disabled when the device is in idle mode.
//returns 1 for OK. If the device is in the connected status, this function returns 0.
//after invoking this function, the device will NOT be found or connected.
//one may invoke GetCurBtConnectedFlag() to get the connected status.
uint8_t BTSetScanDisable(void);

// Some 2.1 chip doesn't support BLE event, one should disable BLE event
void BTDisableBLEEvent(void);


//get current connected flag. If BT device is connected, then this function returns 1. 
uint8_t GetCurBtConnectedFlag(void);

//set apple battery status
//BatteryLevel:0-9
//DockingState:0-1
void BTSetBatteryState(uint8_t BatteryLevel, uint8_t DockingState);

//disable apple battery status reporting function
void BtDisableBatteryState(void);

//get remote bt device(history) name in format of UTF8 with double-NULL ended.
//zero length means not found, it may refresh at any time of A2DP or HF connected
uint8_t* BTGetRemoteDevName(void);

////device's test mode functions
uint8_t BTDoDeviceInquiry(uint8_t MaxScanTimeInSecond);
uint8_t BTGetDeviceName(const uint8_t* BdAddrLE); //bd addr 6Bytes in LE format(normal format)

uint8_t BTHfDtmf(unsigned char HfDtmfNum); //send one HF DTMF character, return 1 means OK.
//test function
//start one hand-free audio transfer between speaker and phone
void BTStartHfTransfer(void);

//test function
//void BTStartOneHfConnection(void);

//test fucntion
//void BTBqbAvrcpConnect(void);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif



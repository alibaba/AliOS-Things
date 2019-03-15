////////////////////////////////////////////////////////////////////////////////////////////
//					Mountain View Silicon Tech. Inc.
//	Copyright 2012, Mountain View Silicon Tech. Inc., ShangHai, China
//					All rights reserved.
//
//  bt stack related API
//  filename: bt_stack_api.h
//  maintainer: Halley
////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BT_STACK_API_H__
#define __BT_STACK_API_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//supported all features:A2DP+HF+SPP
#define  BT_FEATURE_SUPPORTED_ALL               0x00  /*default*/

//supported all featrues except the SPP
#define  BT_FEATURE_SUPPORTED_NORMAL            0x01

//supported A2DP ONLY
#define  BT_FEATURE_SUPPORTED_HIFI_ONLY         0x02

//supported A2DP + SPP
#define  BT_FEATURE_SUPPORTED_HIFI_SPP          0x04

//support SPP server ONLY
#define  BT_FEATURE_SUPPORTED_SPP               0x08

//support HF + SPP(server)
#define  BT_FEATURE_SUPPORTED_HF_SPP            0x10

//--------------------------------------------------------------------------------------------
// BT_FEATURE_SUPPORTED_XXXX | BT_REATURE_WITHOUT_MIC_SUPPORT means DO NOT support mic communication function!
#define  BT_FEATURE_HAS_MIC_SUPPORT             0x00  /*default*/
#define  BT_FEATURE_WITHOUT_MIC_SUPPORT         0x80  /* disable BT phone-call communication,*/
/* but DO HAS phone call notification */

//////////////////////////////////////////////////////////////////////////////////////////
//supported bt device
//#define  BTUsbDevice            1
//#define  BTUartDeviceBk3211     2
//#define  BTUartDeviceRda5875    3
//#define  BTUartDevice2402       4
//#define  BTUartDeviceHl1100     5
//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
//get bt stack package supported features ID
//
//Return Value
//returns one of {BT_FEATURE_SUPPORTED_ALL,
//                BT_FEATURE_SUPPORTED_NORMAL,
//                BT_FEATURE_SUPPORTED_HIFI_ONLY}.
///////////////////////////////////////////////////////////////////////////////////////////
uint8_t BTStackGetPackageFeatures(void);

///////////////////////////////////////////////////////////////////////////////////////////
//init bt stack before running, (after bt device enum OK)
//
//one can fille [UserDefinedDevLoalName] with NULL for using default name "MV_BT"
//max available user defined device name length is up to 18 Chinese character or 51 ASCII.
//for ASCII, just fill with the ascii itself
//for Chinese character, one SHOULD fill with UTF-8 format.
//Also, it can be NULL and the default name will be used.
//
//[BtFeatureID] can be the return value of function BTStackGetPackageFeatures() or one of
//               {BT_FEATURE_SUPPORTED_ALL,
//                BT_FEATURE_SUPPORTED_NORMAL,
//                BT_FEATURE_SUPPORTED_HIFI_ONLY,
//                BT_FEATURE_SUPPORTED_SPP,
//                BT_FEATURE_SUPPORTED_HF_SPP}.
//
//If speaker system has NOT Mic, [BtFeatureID] can be OR-combined with BT_FEATURE_WITHOUT_MIC_SUPPORT.
//
//Return Value
//none zero means OK(success).
///////////////////////////////////////////////////////////////////////////////////////////
uint32_t BTStackRunInit(uint8_t BTDeviceType, uint8_t *UserDefinedDevLocalName, uint8_t BtFeatureID);

///////////////////////////////////////////////////////////////////////////////////////////
// Invoke once for running one bt stack step.
// One may invoking this function one after another
//
///////////////////////////////////////////////////////////////////////////////////////////
void  BTStackRun(void);

///////////////////////////////////////////////////////////////////////////////////////////
//
// Free BT stack allocated memory resource(allocated by function BTStackRunInit(...)).
//
///////////////////////////////////////////////////////////////////////////////////////////
void  BTStackRunEnd(void);


//
//This function is used to close dongle at any time.
//One may invoke this function at the end of closing bt stack but before releasing the stack memory.
//For this case:
//       Dongle is in plugin status, but app has switched to none-bt mode,
//       then, ONE need to stop the dongle BB working.
//
//For dongle is removed, then this function may NOT invoked.
//
void BTStackCloseDongle(void);


//Following functions are used internal ONLY, pls use them carefully.
//sync function for HW decoding
//--------------------------------------------------------------
//one may invoke this function after cfg the sbc playing cfg(i.e. after invoking function audio_init())
void SetSbcPlayCfgFlag(bool CfgFlag);

//get play cfg flag
//0 means NOT configured, none-zero means has been configured.
uint8_t GetSbcPlayCfgFlag(void);

//After hw decoder returns error [SBC_ERROR_STREAM_EMPTY], one may invoke this function
//to enable refill sbc data by the bt stack.
void SetSbcDecDoneFlag(void);

//get has sbc data flag
//0 means NOT sbc data, none-zero means has sbc data.
uint8_t GetHasSbcDataFlag(void);

//get sbc stream handle, which is used in hw.
void* GetSbcStreamHandle(void);

///////////////////////////////////////////////////////////////////////////////////////////
//
//Get SBC stream data
//same as fread() function
///////////////////////////////////////////////////////////////////////////////////////////
//uint32_t SBC_StreamRead (void* Buffer, uint32_t Size, uint32_t Count, void* StreamPointer);

//get sco hci handle,none zeror means OK
uint16_t GetScoHciHandle(void);

//get list item number for bt receiving buffer list inside the bt stack.
int32_t GetRcvListItemNum(void);

/*
 * OBEX Subsystem
 */

typedef struct _ObexCallbackInfo ObexCallbackInfo;

typedef void (*ObexCallback)(const ObexCallbackInfo *info);

enum
{
    BT_OBEX_NAME         = 1, // File Name
    BT_OBEX_LENGTH       = 3, // File Length
    BT_OBEX_BODY         = 8, // File Data
    BT_OBEX_BODY_END     = 9, // File Data (Last Part)
};

enum
{
    BT_OBEX_CONNECTED,     // OBEX connection is established
    BT_OBEX_DISCONNECTED,  // OBEX connection is destroyed
    BT_OBEX_DATA_IND,      // Received a OBEX data segment
};

struct _ObexCallbackInfo
{
	uint8_t  event;      // OBEX Event

	/*
	 *  Following fields are valid when event is DATA_IND
	 *      A file is splitted into multiple parts
	 *      A part is further splitted into multiple segments
	 *      Each time a segment is received
	 *
	 *      type & total describes the part
	 *      segoff/segdata/seglen describes the current segment
	 */

	uint8_t   type;       // Name, Length, Body ...
	uint16_t  total;      // total length of this part of data
	uint16_t  segoff;     // offset of this segment
	uint8_t*  segdata;    // pointer to data
	uint16_t  seglen;     // Segment Length
};

// Initialize OBEX
//   this function must be called after BTStackRunInit and before BTStackRun
void BTStackInitObex(ObexCallback Callback);

// Lock OBEX
//   call this function if app wants to use OBEX data after callback returns
void BTStackLockObex(void);

// Unlock OBEX
//   call this function when app finishes using OBEX data
void BTStackUnlockObex(void);

// Abort OBEX transfer
void BTStackAbortObex(void);

// Reject OBEX data
void BTStackRejectObexData(void);

////////////////////////////////////////////////
//bt stack data hook for receive or send, one can use this hook to view or do something intresting work on the stack.
//return 1 means one wants to drop the received/send data from the stack(go round the stack data).
//return 0 means do nothing about the stack.
//Important Remarks: Do NOT modify data saved in [DataPointer].
typedef uint8_t (*BTStackHookFunc)(uint8_t* DataPointer, int32_t DataSize);

void BTStackSetSendDataHook(BTStackHookFunc HookFunc);
void BTStackSetReceivedDataHook(BTStackHookFunc HookFunc);


//for user mem alloc purpose, allocated will never be free during the stack's whole life.
typedef void* (*BTStackMemAllocHookFunc)(int32_t Size);

void BTStackSetMemAllocHook(BTStackMemAllocHookFunc HookFunc);

int BtGetRssiValue(void); //get updated RSSI of BB connected, -127dB ~ +127dB, bigger is better
int BtGetLinkqualityValue(void); //get updated linkquality of BB connecetd, 0~255, bigger is better

//hid functions
uint8_t BTHidKeySendByte(uint8_t Key, uint8_t Mode);//HID send one byte of key
uint8_t BTHidMultiKeySend(uint8_t *KeyData/*key array, max length is 6*/, uint8_t Len, uint8_t Mode);//HID send multi-key at one time
uint8_t BTHidDataSend(uint8_t *Data/* max is 36 bytes*/, uint8_t Len);//HID send data through key intrrupt channel

uint8_t BTHidMultiKeySendDown(uint8_t *KeyData/*key array, max is 6 bytes*/, uint8_t Len, uint8_t Mode);//key down and keep
uint8_t BTHidMultiKeySendUp(void); //key up

void BTSetUserDefinedHidDescriptorList(uint8_t * ListData/*less than 256 Bytes*/, uint16_t ListDataLen); //change hid description, details please refer to the related doc.
uint8_t BtHidGetOutData(uint8_t **Data, uint16_t* Len);//received event [BT_UI_CB_STATUS_HID_OUT_DATA_IND]

//////
uint8_t BTHidSendRawData(uint8_t *RawData/*max 46*/, uint16_t Len);
uint8_t BTHidIsReadyUserMode(void); //BTHidDataSend(),BTHidMultiKeySendDown(),BTHidMultiKeySendUp(),BTHidSendRawData().
uint8_t BTHidUserDataModeReset(void);//force to clear related flags, return 0 means error found,such as hid is not connected.

typedef struct _BT_HID_SDP_DATABASE_ITEM
{
    uint16_t AttributID;          // such as 0x0001 for "Service Class List"
    uint16_t LenOfAttributeData;  // length of "AttributeData"
    const uint8_t *AttributeData; // attribute data, such as: "0x35,0x03,0x19,0x11,0x24" for "Service Class List"
    uint32_t WorkBuf;             // will be updated by BT stack    
}BT_HID_SDP_DATABASE_ITEM;

void BTSetUserDefinedHidSdpDatabase(BT_HID_SDP_DATABASE_ITEM *HidSdpDatabase, uint16_t HidSdpDatabaseItemCount);

//BT_HID_SDP_DATABASE_ITEM *GetDefaultHidSdpDatabase(void);

#if 1//bt debug usage, do not remove this
const unsigned char *BTStackGetVersion(void);
#define GetLibVersionBt()  BTStackGetVersion()
void BTDisableSimplePairing(void);
void BTEnableRoleSwitch(void);
void BTDisableQosSetup(void);
void BTDisableAutoSniffMode(void);
void EnableBtDbgInfo(void);
void SetMtEV3Property(void);
void SetMt3EV3Property(void);
void SetEnableRoleSwitchFlag(void);
void SetDisableRoleSwitchFlag(void);
void UnSetRdaEscoProperty(void);
void SetMTK6626RF(void);
void EnableMtkRfAutoDetect(void);//if this function is invoked, then function SetMTK6626RF() is disabled automatically.
void BTStackSetPinCode(uint8_t* pinCode);//Four bytes of Pin code string
//void SetMtkTxPower(unsigned char Power/*0~7, 0 means using default value*/);
void BTStatckSetPageTimeOutValue(uint16_t TimeOutValueInMs);

void BTSetIoCapSyncFlag(void);//IO cap setting for SPP connection ONLY case

void EnableAppleiApFunction(void);//enable Apple BT iAp functions

void BtEnableHidFunc(void);//enable HID functions, details please refer to the related doc.

uint8_t* BTGetCurDeviceNamesAddr(void); //get current connected device's addr or the last connected(inq) device's addr

uint8_t BTDoTestModeDeviceInquiry(uint8_t MaxTimeInSecond);//MV BT testermode
void BTEnableBluetoothTestMode(void);//enable MV BT testermode name="MVBTTester01"
#endif

#ifdef __cplusplus
}
#endif//__cplusplus

#endif


//
//bt_app_ui_callback.h
//  maintainer: Halley
//
//

#ifndef __H_BT_APP_UI_CALLBACK_H__
#define __H_BT_APP_UI_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//void BTStackStatusCallBackFunc(uint8_t CBParameter);
//This function SHOULD be implemented by users.
//CBParameter can be following values:

#define ADV_AVRCP_ATTRIBUTE_MASK_SHUFFLE 0x04
#define ADV_AVRCP_ATTRIBUTE_MASK_REPEAT  0x02
#define ADV_AVRCP_ATTRIBUTE_MASK_EQ      0x01

#define ADV_AVRCP_ATTRIBUTE_ID_EQ        0x01
#define ADV_AVRCP_ATTRIBUTE_ID_REPEAT    0x02
#define ADV_AVRCP_ATTRIBUTE_ID_SHUFFLE   0x03
#define ADV_AVRCP_ATTRIBUTE_ID_SCAN      0x04
enum
{
    BT_UI_CB_STATUS_ERROR = 0,                        //error, one may stop the BT stack.
    BT_UI_CB_STATUS_OUT_OF_MEM_ERROR,                 //out of memory error found, one SHOULD STOP the BT stack!!!
    BT_UI_CB_STATUS_IDLE,                             //idle for waiting user input/control, may paired but un-connected
    BT_UI_CB_STATUS_PAIRED,                           //one BT device paired/unpaired msg
    BT_UI_CB_STATUS_A2DP_CONNECTED,                   //A2DP connected, can enjoy the music now, but no OTHER BT device's connection can be accepted.
    BT_UI_CB_STATUS_PLAY_CONTROL_CONNECTED,           //Play control connected with the BT source device
    BT_UI_CB_STATUS_DISCONNECTED,                     //disconnected of A2DP and AVRCP if any

    BT_UI_CB_STATUS_HID_OUT_DATA_IND = 0x30,          //HID host data out event, one may call function BtHidGetOutData()
    
//followings are advanced avrcp events
    BT_UI_CB_STATUS_ADV_AVRCP_SUPPORTED = 0x40,       //advanced avrcp features supported, such as apple devices(iphone, pad, pod)
    BT_UI_CB_STATUS_ADV_AVRCP_NOT_SUPPORTED,
    BT_UI_CB_STATUS_PLAYING_STATUS,                   //play status reported, call function [AdvAvrcpPlayStatus*]BtGetAdvAvrcpResponse()
    BT_UI_CB_STATUS_TRACK_INFO,                       //track info reported, call function [AdvAvrcpTrackInfo*]BtGetAdvAvrcpResponse()   
    BT_UI_CB_STATUS_AVAILABLE_ATTRIBUTE_TYPES,        //source bt supported attribute types, such as report mode, shuffle mode, ...
                                                      //call function [ADV_AVRCP_ATTRIBUTE_MASK_XXX]BtGetAdvAvrcpResponse()   
    BT_UI_CB_STATUS_AVAILABLE_SETTINGS_FOR_ONE_ATTR,  //supported settings for one attribute,such as repeat all, shuffle on/off
                                                      //call function [void*]BtGetAdvAvrcpResponse(), 
                                                      //for inquiry shuffle, then returns [_AdvAvrcpSupportedSettings*] one may judge based on the pre-send cmd.
    BT_UI_CB_STATUS_CUR_SETTING_FOR_ONE_ATTR,         //current used setting, call function [AdvAvrcpCurSetting*]BtGetAdvAvrcpResponse()
    

    BT_UI_CB_STATUS_HID_CONNECTED = 0x78,             //hid interrupt channel is connected(NO event for reporting control channle connected status)
    BT_UI_CB_STATUS_HID_DISCONNECTED = 0x79,          //hid interrupt or control channle is disconnected

//followings are the un-supported control ID for A2DP
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_PLAY = 0x81,//Play func
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_PAUSE,      //Pause func
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_STOP,       //Stop func
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_VOL_UP,     //Volume Up func
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_VOL_DOWN,   //Volume Down func
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_PRE_SONG,   //Pre-song func
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_NEXT_SONG,  //Next-song func
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_FAST_FORWARD,//FF function
    BT_UI_CB_STATUS_CONTROL_NOT_SUPPORTED_FAST_BACKWARD,//FB function

//folowings three are the msg for Phonebook on handfree channel(AT cmd), so if HF not suported then phonebook also disabled.
    BT_UI_CB_STATUS_PB_SUPPORTED = 0xA0,   //means the connected BT src supports phonebook feature found
    BT_UI_CB_STATUS_PB_ENTRY_SIZE,         //means selected phone book's total entry size info(total record number)
    BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA,     //one selected phone book's record data received
    BT_UI_CB_STATUS_CLCC_RESULT,           //current call list number data received

    BT_UI_CB_STATUS_HF_COMMAND_END = 0xDF, //HF command runs end(OK), including PB commands

//followings are the msg for Handfree(mono speech headphone) ONLY
    BT_UI_CB_STATUS_HF_CONNECTED = 0xE0,   //handfree function connected
    BT_UI_CB_STATUS_HF_DISCONNECTED,       //handfree function disconnected

    BT_UI_CB_STATUS_HF_CALL_SETUP_NONE,     //handfree call setup end, when reject or accept one call(caller or receiver), this event will be released.
    BT_UI_CB_STATUS_HF_CALL_SETUP_INCOMING,      //one call is incoming, waiting for response
    BT_UI_CB_STATUS_HF_CALL_SETUP_OUTCALL_ALERT,      //one out call alert on AG(start ringing to the taget phone)

    BT_UI_CB_STATUS_HF_INCOME_RING,        //one incoming call is ringing and waiting for user's response, once per 2~3seconds.
    BT_UI_CB_STATUS_HF_INCOME_CALL_NUM,    //income phone call number is coming, system will inform this at each ring
    //one may invoke function BTGetCurInCallPhoneNum() to get the caller number string.

    BT_UI_CB_STATUS_HF_CALL_CONNECTED,     //current phone call linking connected
    BT_UI_CB_STATUS_HF_CALL_DISCONNECTED,  //current phone call linking disconnected
    BT_UI_CB_STATUS_HF_CALL_COMMAND_ERROR, //command invoking error at AG. The reason is not given here.

    BT_UI_CB_STATUS_GET_REMOTE_BT_DEV_NAME,//get remote connected bt device's name,one may invoke function BTGetRemoteDevName() to get the name.

    BT_UI_CB_STATUS_CONNECTION_LOST, // Baseband lost connection

    BT_UI_CB_STATUS_CONNECTION_ABORTED, // Stack aborted connection
    
    BT_UI_CB_STATUS_HF_CALL_SETUP_OUT,  // an phone call is outing(start)

    BT_UI_CB_STATUS_HF_PHONE_SPEAKER_VOL, //HF speaker vlume sync,one may invoke function BtGetRemoteSpeakerVol() to get the value[0-15]

    BT_UI_CB_STATUS_SPP_CONNECTED = 0xF0,  //SPP connected if supported
    BT_UI_CB_STATUS_SPP_DISCONNECTED,      //SPP disconnected if any

    BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN = 0xF6, //MFi bt channel is ready, one can do the data exchange
    BT_UI_CB_STATUS_BT_MFi_DATA_IND,            //apple iAp data is coming, one may invoke function BtGetMfiDataInfo(...) to get the data
    BT_UI_CB_STATUS_BT_MFi_DATA_SENT,           //data has been sent through iAp channel

    BT_UI_CB_STATUS_RSSI_LINK_QUALITY_IND,      //indicate RSSI and link quality value are updated, one may invoke function BtGetLinkqualityValue(),BtGetRssiValue()

    BT_UI_CB_STATUS_INQUIRY_RESULT = 0xFA, //test mode event(no bd addr returned.

    BT_UI_CB_STATUS_GET_NAME_PAGE_TIMEOUT = 0xFD,//get remote device name page timeout
    BT_UI_CB_STATUS_BB_NO_RESPONSE = 0xFE, //BT BB has no response, so one may reboot the BB and stack. 

    BT_UI_CB_STATUS_PAGE_TIME_OUT  = 0xFF, //page timeout(HF) or src not ready(ACL waiting for timeout)
};

////////////////////////////////////Phone Book(AT Cmd @ HFP)/////////////////////////////////////////
/*phone book list bitmap IDs*/
#define BT_PB_LIST_ID_DIALLED          0x0001    /*dialled calls list,DC*/
#define BT_PB_LIST_ID_PHONE_MEMORY     0x0010    /*phone book list saved in phone memory,NOT the SIM card, ME*/
#define BT_PB_LIST_ID_SIM_CARD         0x0080    /*phone book list saved in the current selected/active SIM card, SM*/
#define BT_PB_LIST_ID_RECEIVED_CALL    0x0040    /*received calls list, RC*/
#define BT_PB_LIST_ID_MISSED_CALL      0x0008    /*missed calls list,MC*/

typedef struct _OnePhonebookEntry 
{
    unsigned short Idx;             /* Location in current(idx from 1) selected phone book */ 
    unsigned char  Type;            /* Reserved */
    const char     *PhoneNumber;    /* The phone number pointer */ 
    const char     *PhoneText;      /* The text part in the format of UTF8 */ 
} OnePhonebookEntry; /*[BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA]*/

typedef struct _CurPhonebookSize 
{
    unsigned short         IdxFirst;         /* First Entry Idx        */ 
    unsigned short         IdxLast;          /* Last Entry Idx          */ 
    unsigned short         MaxNumberStrLen;  /* Maximum number length */ 
    unsigned short         MaxTextStrLen;    /* Maximum text length   */ 
} CurPhonebookSize;  /*[BT_UI_CB_STATUS_PB_ENTRY_SIZE]*/

void BTQueryPhonebooks(void); //must returns [BT_UI_CB_STATUS_PB_SUPPORTED],call BTGetSupportedPhonebookStorageType
void BTSelectPhonebook(unsigned short PhonebookBitID);//one of BT_PB_LIST_ID_XXXXX
void BTGetPhonebookSize(void); //must returns [BT_UI_CB_STATUS_PB_ENTRY_SIZE],call BtGetCurrentPbResponseData
void BTGetPBListEntry(unsigned short StartIdx/*from 1*/, unsigned short EndIdx/*from 1*/);//must returns [BT_UI_CB_STATUS_PB_ONE_ENTRY_DATA], call BtGetCurrentPbResponseData
unsigned short BTGetSupportedPhonebookStorageType(void); //returns mux of BT_PB_LIST_ID_XXXXX
void* BtGetCurrentPbResponseData(void);
unsigned char BTSetPhonebookCharacterSet2UTF8(void);//API, fix to UTF8 format
/////////////////////////////////////////////////////////////////////////////

unsigned char BtGetCurCallListNumber(void); //returns 1 means OK. Event [BT_UI_CB_STATUS_CLCC_RESULT] may be received after the command is sent.
const char* BtGetCurCallListNumberResponseData(void);//can ONLY be invoked when reveived event [BT_UI_CB_STATUS_CLCC_RESULT].

/////////////////////////Advanced AVRCP Feature(V1.4)///////////////////////////////////////////////////
#define CurStatus_STOPPED  0x00
#define CurStatus_PLAYING  0x01
#define CurStatus_PAUSED   0x02
#define CurStatus_FWD_SEEK 0x03
#define CurStatus_REV_SEEK 0x04
#define CurStatus_ERROR    0xff
typedef struct _AdvAvrcpPlayStatus  /*Event:BT_UI_CB_STATUS_PLAYING_STATUS*/
{
    unsigned int  TrackLengthInMs;
    unsigned int  CurPosInMs;
    unsigned char CurStatus;  /*it can be one of CurStatus_XXXX*/
}AdvAvrcpPlayStatus;

#define TRACK_ATTR_TITLE       0x00000001
#define TRACK_ATTR_ARTIST      0x00000002
#define TRACK_ATTR_ALBUM       0x00000003
#define TRACK_ATTR_TRACK       0x00000004
#define TRACK_ATTR_NUM_TRACKS  0x00000005
#define TRACK_ATTR_GENRE       0x00000006
#define TRACK_ATTR_DURATION    0x00000007

typedef struct _OneAttrInfo
{
    unsigned int   TackAttrID;      //track attribute ID, one of [TRACK_ATTR_XXXX]
    unsigned short CharacterSetID;  //such as UTF8--0x006a
    unsigned short PropertyStringLen;
    const char*    PropertyString;
}OneAttrInfo;

typedef struct _AdvAvrcpTrackInfo /*Event:BT_UI_CB_STATUS_TRACK_INFO*/
{
    unsigned char ItemNum;        /*total property item number*/
    OneAttrInfo   Property[7];  /*max 7 track item info saved*/
    
}AdvAvrcpTrackInfo;

#define ADV_AVRCP_ID_SHUFFLE_OFF          1
#define ADV_AVRCP_ID_SHUFFLE_ALL          2
#define ADV_AVRCP_ID_SHUFFLE_GROUP        3

#define ADV_AVRCP_ID_REPEAT_OFF           1
#define ADV_AVRCP_ID_REPEAT_SINGLE        2
#define ADV_AVRCP_ID_REPEAT_ALL           3
#define ADV_AVRCP_ID_REPEAT_GROUP         4

#define ADV_AVRCP_ID_EQ_OFF               1
#define ADV_AVRCP_ID_EQ_ON                2

typedef struct _AdvAvrcpSupportedSettings /*Event:BT_UI_CB_STATUS_AVAILABLE_SETTINGS_FOR_ONE_ATTR*/
{
    unsigned char SupportedSettingNum;
    unsigned char *SettingIds; // ADV_AVRCP_ID_SHUFFLE_XXX,...,ADV_AVRCP_ID_REPEAT_XXX,...
}AdvAvrcpSupportedSettings;

typedef struct _AdvAvrcpCurSetting /*Event:BT_UI_CB_STATUS_CUR_SETTING_FOR_ONE_ATTR*/
{
    unsigned short AdvAvrcpAttrMask; //it can be one of ADV_AVRCP_ATTRIBUTE_MASK_XXXX
    unsigned char SettingEq;         //it can be one of ADV_AVRCP_ID_EQ_XXX for mask ADV_AVRCP_ATTRIBUTE_MASK_EQ
    unsigned char SettingRepeat;     //it can be one of ADV_AVRCP_ID_REPEAT_XXX for mask ADV_AVRCP_ATTRIBUTE_MASK_REPEAT
    unsigned char SettingShuffle;    //it can be one of ADV_AVRCP_ID_SHUFFLE_XXX for mask ADV_AVRCP_ATTRIBUTE_MASK_SHUFFLE
    unsigned char Reserved;
}AdvAvrcpCurSetting;

void          DisableAdvancedAvrcpFeature(void);   //disable advanced AVRCP feature
void          DisableAdvancedAvrcpPlayStatusAutoReport(void);//disable auto play status report based one advanced avrcp
unsigned char BTPlayCtrlAdvAvrcpGetPlayStauts(void); //returns 1 means cmd is invoked(may not send for avrcp not availabled)./*[Event:BT_UI_CB_STATUS_PLAYING_STATUS]*/
unsigned char BTPlayCtrlAdvAvrcpGetTrackInfo(void);  //returns 1 means cmd is invoked. /*[Event:BT_UI_CB_STATUS_TRACK_INFO*/

unsigned char BTPlayCtrlAdvAvrcpSetPlayerSettingValues(unsigned char IsRepeatModeFlag /*repeat flag(1) or shuffle flag(0)*/, unsigned char ModeParameter);
unsigned char BTPlayCtrlAdvAvrcpGetPlayerSettingValues(unsigned short AttrMask/*ADV_AVRCP_ATTRIBUTE_MASK_XXX*/); /*[Event:BT_UI_CB_STATUS_CUR_SETTING_FOR_ONE_ATTR]*/
unsigned char BTPlayCtrlAdvAvrcpListPlayerSettingValues(unsigned char AttrID/*ADV_AVRCP_ATTRIBUTE_ID_XXXX*/); /*[Event:BT_UI_CB_STATUS_AVAILABLE_SETTINGS_FOR_ONE_ATTR]*/

unsigned char BTPlayCtrlAdvAvrcpListPlayerSettingAttrs(void); //returns 1 means cmd is invoked. [Event:BT_UI_CB_STATUS_AVAILABLE_ATTRIBUTE_TYPES]

void* BtGetAdvAvrcpResponse(void); //returns response data pointer

/*------------------------------------------------------------------------------------------*/

/* Function: send apple device(MFi) compatible data through BT iAp interface.
   Return  : return 1 means the data has been pushed to the sending queue.
   Remarks :
         1)Parameter [Data] can be dropped after invoking this function.
         2)On CAN ONLY send other data after received the event [BT_UI_CB_STATUS_BT_MFi_DATA_SENT].
         3)Event[BT_UI_CB_STATUS_BT_MFi_CHANNEL_OPEN] means apple iAp channel is ready.
*/
int BTiApDataSend(unsigned char* Data /*max size is 640 Bytes*/, unsigned short DataLen);

//This function can ONLY be invoked right after received event [BT_UI_CB_STATUS_BT_MFi_DATA_IND]
int BtGetMfiDataInfo(unsigned char **Data, int *Len);//returns 1 means OK.
//////////////////////////////////////////////////////////////////////////////////////////////
enum
{
    BT_ROM_CB_FUNC_UserProcSppData = 0,
    BT_ROM_CB_FUNC_BTDeviceInit,
    BT_ROM_CB_FUNC_BTStackStatusCallBackFunc,
    BT_ROM_CB_FUNC_BtStackSendDataRom,
    BT_ROM_CB_FUNC_BTStackDataStatusCallBackFunc,
    BT_ROM_CB_FUNC_OS_GetSystemTime,
    BT_ROM_CB_FUNC_BtStackRcvData,
    BT_ROM_CB_FUNC_NVRAM_READ,
    BT_ROM_CB_FUNC_NVRAM_WRITE,
    BT_ROM_CB_FUNC_BT_DEBUG,
    BT_ROM_CB_FUNC_CODE_FIX,
    BT_ROM_CB_FUNC_CODE_FIX_FLAG,
};

//BT Rom product used ONLY
//One MUST invoke all BT_ROM_CB_FUNC_XXXX functions before init the BT stack!!!!
void BTSetRomOtpFunc(uint8_t FuncID/*BT_ROM_CB_FUNC_XXX*/, uint32_t FuncPointer);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif


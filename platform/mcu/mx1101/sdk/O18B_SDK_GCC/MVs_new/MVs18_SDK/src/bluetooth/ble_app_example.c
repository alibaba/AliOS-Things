/////////////////////////////////////////////
//
//

#include "type.h"
#include "app_config.h"

#include "timer_api.h"
#include "sys_app.h"
#include "spp_app_proc.h"
#include "folder.h"
#include "fat_file.h"
#include "player_control.h"
#include "eq_params.h"
#include "ble_api.h"
#include "ble_app_example.h"
#include "bt_app_func.h"

#define BLE_CMD_CHECK_PERIOD_MS 50
extern AUDIO_PLAYER_CONTROL*  gPlayContrl;

static uint8_t isConmandError = 0;
uint16_t prevCmd= 0xffff;

static SW_TIMER BleCommandTimerHandle;
static bool bBleCommandTimer = FALSE;

void PlayCtrlProcess(uint8_t* CmdData, uint8_t datalen);
int16_t playcontrol_att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
int playcontrol_att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);


static uint8_t playcontrol_cmd_data[256];		// 根据spp文档，此处buffer不超过255字节
static uint8_t playcontrol_rsp_data[256];
static uint8_t* playcontrol_rsp_buffer = &playcontrol_rsp_data[5];

static uint8_t playcontrol_cmd_data_len = 0;
static uint8_t playcontrol_rsp_data_len = 0;


uint8_t playcontrol_service_uuid[] = {0x32, 0xa0};//0xa032;
uint8_t playcontrol_characteristic_cmd_declare[] = {0x08, 0x03, 0x01, 0x40,0xa0};//conmand,write only
uint8_t playcontrol_characteristic_result_declare[] = {0x02, 0x05, 0x01, 0x41,0xa0};//conmand excute result, read only
uint8_t playcontrol_characteristic_modechange_result_declare[] = {0x20, 0x07, 0x01, 0x42,0xa0};//conmand excute result, read only

attribute_item_t  playcontrol_attribute[] = {
    {0xf002, 0x0101, 0x2800, NULL, playcontrol_service_uuid, 2},
    {0xf002, 0x0102, 0x2803, NULL, playcontrol_characteristic_cmd_declare, 5},//spp playcontrol conmand, write only
    {0xf108, 0x0103, 0xa040, NULL, playcontrol_cmd_data,256},
    {0xf002, 0x0104, 0x2803, NULL, playcontrol_characteristic_result_declare, 5},//spp conmand excute result, read only
    {0xf102, 0x0105, 0xa041, NULL, playcontrol_rsp_data,256}, 
    {0xf002, 0x0106, 0x2803, NULL, playcontrol_characteristic_modechange_result_declare, 5},//spp conmand excute result, read only
    {0xf020, 0x0107, 0xa042, NULL, playcontrol_rsp_data,7}, 
};

uint16_t playcontrol_cmd_handle = 0x0103;
uint16_t playcontrol_response_handle = 0x0107;

/*
ATTR_PROFILE_T * playcontrol_attr_profile = NULL;
int8_t InitBlePlaycontrolProfile(uint8_t * name)
{
	// 初始化发送、接收buffer
	memset(playcontrol_cmd_data, 0, sizeof(playcontrol_cmd_data));
	memset(playcontrol_rsp_data, 0, sizeof(playcontrol_rsp_data));
	playcontrol_cmd_data_len = 0;
	playcontrol_rsp_data_len = 0;

	// 初始化profile attribute
	playcontrol_attr_profile = mmm_malloc(sizeof(ATTR_PROFILE_T), MMM_ALLOC_FAST);

	if(playcontrol_attr_profile == NULL)
	{
		APP_DBG("xxxxxxxxxx malloc for playcontrol_attr_profile failed xxxxxxxxx\n");
		return -1;
	}

	playcontrol_attr_profile->attr = playcontrol_attribute;
	playcontrol_attr_profile->attr_size = sizeof(playcontrol_attribute);
	playcontrol_attr_profile->att_read = playcontrol_att_read;
	playcontrol_attr_profile->att_write = playcontrol_att_write;
	playcontrol_attr_profile->name = name;

}
*/

uint8_t					g_device_name[] = "PlayControlDemo";
BLE_APP_CONTEXT			g_playcontrol_app_context;
GATT_SERVER_PROFILE		g_playcontrol_profile;

int8_t InitBlePlaycontrolProfile(void)
{
	g_playcontrol_app_context.ble_device_name = g_device_name;
	g_playcontrol_app_context.ble_device_role = PERIPHERAL_DEVICE;

	g_playcontrol_profile.attrs 		= playcontrol_attribute;
	g_playcontrol_profile.attrs_size 	= sizeof(playcontrol_attribute);
	g_playcontrol_profile.attr_read		= playcontrol_att_read;
	g_playcontrol_profile.attr_write	= playcontrol_att_write;

//	g_playcontrol_profile.read_buf		= playcontrol_rsp_data;
//	g_playcontrol_profile.read_buf_len	= sizeof(playcontrol_rsp_data);
//	g_playcontrol_profile.write_buf		= playcontrol_cmd_data;
//	g_playcontrol_profile.write_buf_len	= sizeof(playcontrol_cmd_data);

	return 0;
}

int16_t playcontrol_att_read(uint16_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size)
{
	uint16_t  att_value_len;
	uint16_t uuid16 = att_uuid_for_handle(attribute_handle);
	uint16_t bytes_to_copy;


	if(uuid16 != 0xa041)
		return -1;

	att_value_len = playcontrol_rsp_data_len;

	if(att_value_len == 0)		// data not ready
	{
		uint8_t ErrorAck[1] = {0xFF};
		ResponseCommand(0xffff, ErrorAck, 1);
		att_value_len = 7;
	}

	if (offset > att_value_len)
	{
		return 0;
	}

	bytes_to_copy = att_value_len - offset;

	if (!buffer) 
		return bytes_to_copy;

	if (bytes_to_copy > buffer_size)
	{
		bytes_to_copy = buffer_size;
	}

	memcpy(buffer, &playcontrol_rsp_data[offset], bytes_to_copy);
	return bytes_to_copy;

}

int playcontrol_att_write(uint16_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{
    uint16_t uuid16 ;
    
	if(attribute_handle != playcontrol_cmd_handle)
	{
		return -1;
	}

	uuid16 = att_uuid_for_handle(attribute_handle);
	if(uuid16 != 0xa040)
		return -1;
	
	switch(transaction_mode)
	{
		case TRANSACTION_MODE_NONE:
			memcpy(playcontrol_cmd_data + offset, buffer, buffer_size);
			playcontrol_cmd_data_len = buffer_size;
			ParseCommandData(playcontrol_cmd_data, playcontrol_cmd_data_len);
			playcontrol_cmd_data_len = 0;
		break;

		case TRANSACTION_MODE_ACTIVE:
			memcpy(playcontrol_cmd_data + offset, buffer, buffer_size);
			playcontrol_cmd_data_len += buffer_size;
		break;

		case TRANSACTION_MODE_EXECUTE:
			ParseCommandData(playcontrol_cmd_data, playcontrol_cmd_data_len);
			playcontrol_cmd_data_len = 0;
		break;

		case TRANSACTION_MODE_CANCEL:
			memset(playcontrol_cmd_data, 0, 256);
			playcontrol_cmd_data_len = 0;
		break;
	}
	return 0;
}



uint8_t calc_checksum(uint8_t* Buf, uint8_t Len)
{
    uint8_t CheckSum=0;
    uint8_t i=0;

    while(i < Len)
    {
        CheckSum += Buf[i];
        i++;
    }
    CheckSum = 0 - CheckSum;
    return CheckSum;
}

static void BleCommandTimerCB(void* Param)
{
	if(GetModeSwitchState() == MODE_SWITCH_STATE_DONE)
	{
		uint8_t rsp_buf[1];
		
		bBleCommandTimer = FALSE;
		StopTimer(&BleCommandTimerHandle);
		DeinitTimer(&BleCommandTimerHandle);

		if(prevCmd == SLAVE_SELECT_MODE)
			rsp_buf[0] = (uint8_t)gSys.CurModuleID;
		else if(prevCmd == SLAVE_NEXT_MODE)
			rsp_buf[0] = (uint8_t)gSys.NextModuleID;
		ResponseCommand(prevCmd, rsp_buf, 1);

		// send the response data to client
		GattServerIndicate(playcontrol_response_handle, playcontrol_rsp_data, playcontrol_rsp_data_len);
	}
}

void BleCommandTimerDetect(void)
{
	if(bBleCommandTimer)
	{
		CheckTimer(&BleCommandTimerHandle);
	}
}

void WaitCommandComplete(void)
{
	if(InitTimer(&BleCommandTimerHandle, BLE_CMD_CHECK_PERIOD_MS, BleCommandTimerCB))
	{
		StartTimer(&BleCommandTimerHandle);
		bBleCommandTimer = TRUE;
	}
}

void ParseCommandData(uint8_t* data, uint8_t data_len)
{
	uint8_t ErrorAck[1] = {0xFF};
	uint8_t ErrorExcute[1] = {0x7F};
	uint16_t cmd = (data[3]<<8)+data[4];
	uint8_t checksum = data[5+data[2]] ;
	
	if(data[5 + data[2]] != calc_checksum(&data[2], data[2] + 3))
	{
		ResponseCommand(cmd, ErrorAck, 1);            
		return;
	}

	PlayCtrlProcess(data+3, data[2]);

	if(isConmandError)
		ResponseCommand(cmd, ErrorExcute, 1);    
}

void ResponseCommand(uint16_t Cmd, uint8_t* Buf, uint8_t Len)
{
    uint8_t CheckSum;
    uint8_t LenTemp;
    uint8_t Size_onetimecansend;
    
    int ret = 0;

    CheckSum = Len +( ((Cmd>>8)&0xff) | 0x80) + (Cmd&0xff);
    LenTemp = Len;
    while(LenTemp)
    {
        CheckSum += Buf[LenTemp - 1];
        LenTemp--;
    }
    
    CheckSum = 0 - CheckSum;
    playcontrol_rsp_data[0] = 0x55;
    playcontrol_rsp_data[1] = 0xAA;
    playcontrol_rsp_data[2] = Len;
    playcontrol_rsp_data[3] = (((Cmd>>8)&0xff) | 0x80);
    playcontrol_rsp_data[4] = (Cmd&0xff);

    if(Len)
    {
        memcpy(playcontrol_rsp_data + 5, Buf, Len);
    }

    playcontrol_rsp_data[Len + 5] = CheckSum;
    playcontrol_rsp_data_len = Len + 6;
}


void PlayCtrlProcess(uint8_t* CmdData, uint8_t datalen)
{
    FOLDER  folder;
    FOLDER  ParentFolder;
    FAT_FILE    file;
    uint8_t temp = 0;
    int16_t 	sTempWord;
    int16_t 	sTempWord2;
    uint16_t*  P;
    uint32_t	sTempDWord;
    uint16_t Cmd = (CmdData[0]<<8)+CmdData[1];
//    uint8_t playcontrol_rsp_buffer[90] = {0};
    
    isConmandError = 1;
    switch(Cmd)
    {
    case SLAVE_NEXT_MODE:
        gSys.NextModuleID = GetNextModeId(gSys.CurModuleID);
        MsgSend(MSG_MODE);
        SetModeSwitchState(MODE_SWITCH_STATE_START);
        isConmandError = 0;
        prevCmd = Cmd;
        WaitCommandComplete();
        break;

    case SLAVE_SELECT_MODE:
        if(IsModeEnvOk((uint32_t)CmdData[2]) == FALSE || (CmdData[2] == gSys.CurModuleID) )
        {
            isConmandError = 0;
            playcontrol_rsp_buffer[0] = (uint8_t)gSys.CurModuleID;
            ResponseCommand(SLAVE_SELECT_MODE, playcontrol_rsp_buffer, 1);
            break;
        }
        else
        {
            gSys.NextModuleID = (uint32_t)CmdData[2];
            MsgSend(MSG_MODE);
            SetModeSwitchState(MODE_SWITCH_STATE_START);
            isConmandError = 0;
            prevCmd = Cmd;
            WaitCommandComplete();
        }
        break;

    case SLAVE_GET_MODE:
        playcontrol_rsp_buffer[0] = (uint8_t)gSys.CurModuleID;
        isConmandError = 0;
        ResponseCommand(SLAVE_GET_MODE, playcontrol_rsp_buffer, 1);
        break;

    case SLAVE_GET_DEVICE_LINK:
        temp = IsUDiskLink() ? 1 : 0;
        temp |= IsCardLink() ? 2 : 0;
        temp |= IsLineInLink()? 4 : 0;
        temp |= IsPcLink() ? 8 : 0;
        temp |= IsAvrcpConnected()? 0x10 : 0;
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = temp;
        ResponseCommand(SLAVE_GET_DEVICE_LINK, playcontrol_rsp_buffer, 1);
        break;

    case SLAVE_GET_VALID_FOLDER_SUM:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum);
        playcontrol_rsp_buffer[1] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum);
        playcontrol_rsp_buffer[2] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum);
        playcontrol_rsp_buffer[3] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.ValidFolderSum);
        ResponseCommand(SLAVE_GET_VALID_FOLDER_SUM, playcontrol_rsp_buffer, 4);
        break;
    case SLAVE_GET_FILE_SUM:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = BYTE1(gPlayContrl->FolderHandle.FsContext->gFsInfo.FileSum);
        playcontrol_rsp_buffer[1] = BYTE0(gPlayContrl->FolderHandle.FsContext->gFsInfo.FileSum);
        ResponseCommand(SLAVE_GET_FILE_SUM, playcontrol_rsp_buffer, 2);
        break;
    case SLAVE_GET_FOLDER_INFO:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        BYTE1(sTempWord) = CmdData[2];
        BYTE0(sTempWord) = CmdData[3];
        BYTE1(sTempWord2) = CmdData[4];
        BYTE0(sTempWord2) = CmdData[5];
        if(sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum)
        {
            break;
        }
        //打开相应文件夹
        if(sTempWord == 0)
        {
            if(!FolderOpenByNum(&folder, NULL, sTempWord2))
            {
                break;
            }
        }
        else
        {
            if(!FolderOpenByNum(&ParentFolder, NULL, sTempWord))
            {
                break;
            }
            if(!FolderOpenByNum(&folder, &ParentFolder, sTempWord2))
            {
                break;
            }
        }
        playcontrol_rsp_buffer[0] = (uint8_t)(folder.FolderNum >> 8);
        playcontrol_rsp_buffer[1] = (uint8_t)folder.FolderNum;
        playcontrol_rsp_buffer[2] = (uint8_t)(folder.ValidFolderNum >> 8);
        playcontrol_rsp_buffer[3] = (uint8_t)folder.ValidFolderNum;
        playcontrol_rsp_buffer[4] = (uint8_t)(folder.StartFileNum >> 8);
        playcontrol_rsp_buffer[5] = (uint8_t)folder.StartFileNum;
        playcontrol_rsp_buffer[6] = (uint8_t)(folder.IncFileCnt >> 8);
        playcontrol_rsp_buffer[7] = (uint8_t)folder.IncFileCnt;
        playcontrol_rsp_buffer[8] = (uint8_t)(folder.IncFolderCnt >> 8);
        playcontrol_rsp_buffer[9] = (uint8_t)folder.IncFolderCnt;
        playcontrol_rsp_buffer[10] = (uint8_t)(folder.RecFileCnt >> 8);
        playcontrol_rsp_buffer[11] = (uint8_t)folder.RecFileCnt;
        playcontrol_rsp_buffer[12] = (uint8_t)(folder.RecFolderCnt >> 8);
        playcontrol_rsp_buffer[13] = (uint8_t)folder.RecFolderCnt;
        memcpy(&playcontrol_rsp_buffer[14], &folder.ShortName[0], 8);
        if(FolderGetLongName(&folder, &playcontrol_rsp_buffer[23], 66))
        {
            for(P = (uint16_t*)(&playcontrol_rsp_buffer[23]); *P != 0; P++)//计算长文件名长度
            {
                temp++;
            }
            playcontrol_rsp_buffer[22] = temp * 2;
        }
        else
        {
            playcontrol_rsp_buffer[22] = 0;
        }
        isConmandError = 0;
        ResponseCommand(SLAVE_GET_FOLDER_INFO, playcontrol_rsp_buffer, 23 + playcontrol_rsp_buffer[22]);
        break;
    case SLAVE_GET_FILE_INFO:
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {           
            break;
        }
        BYTE1(sTempWord) = CmdData[2];
        BYTE0(sTempWord) = CmdData[3];
        BYTE1(sTempWord2) = CmdData[4];
        BYTE0(sTempWord2) = CmdData[5];
        if(sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum)
        {
            break;
        }
        //打开相应文件夹
        if(sTempWord != 0)
        {
            if(!FolderOpenByNum(&folder, NULL, sTempWord))
            {
                break;
            }

            if(!FileOpenByNum(&file, &folder, sTempWord2))
            {
                break;
            }
        }
        else
        {
            if(!FileOpenByNum(&file, NULL, sTempWord2))
            {
                break;
            }
        }
        playcontrol_rsp_buffer[0] = (uint8_t)(file.FileNumInDisk >> 8);
        playcontrol_rsp_buffer[1] = (uint8_t)file.FileNumInDisk;
        playcontrol_rsp_buffer[2] = (uint8_t)(file.FileNumInFolder >> 8);
        playcontrol_rsp_buffer[3] = (uint8_t)file.FileNumInFolder;
        playcontrol_rsp_buffer[4] = (uint8_t)(file.FolderNum >> 8);
        playcontrol_rsp_buffer[5] = (uint8_t)file.FolderNum;
        memcpy(&playcontrol_rsp_buffer[6], &file.ShortName[0], 11);
        if(FileGetLongName(&file, &playcontrol_rsp_buffer[18], 66))
        {
            for(P = (uint16_t*)(&playcontrol_rsp_buffer[18]); *P != 0; P++)//计算长文件名长度
            {
                temp++;
            }
            playcontrol_rsp_buffer[17] = temp * 2;
        }
        else
        {
            playcontrol_rsp_buffer[17] = 0;
        }
        isConmandError = 0;
        ResponseCommand(SLAVE_GET_FILE_INFO, playcontrol_rsp_buffer, 18 + playcontrol_rsp_buffer[17]);
        FileClose(&file);
        break;
        
    case SLAVE_PLAY:
    case SLAVE_PAUSE:
        isConmandError = 0;
        MsgSend(MSG_PLAY_PAUSE);
        ResponseCommand(Cmd, NULL, 0);
        break;
    case SLAVE_STOP:
        isConmandError = 0;
        MsgSend(MSG_STOP);
        ResponseCommand(SLAVE_STOP, NULL, 0);
        break;
    case SLAVE_NEXT:
        isConmandError = 0;
        MsgSend(MSG_NEXT);
        ResponseCommand(SLAVE_NEXT, NULL, 0);
        break;

    case SLAVE_PRE:
        isConmandError = 0;
        MsgSend(MSG_PRE);
        ResponseCommand(SLAVE_PRE, NULL, 0);
        break;

    case SLAVE_FF:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        //MsgSend(SYS_KEY_ID_FAST_FORWARD);
        MsgSend(MSG_FF_START);
        ResponseCommand(SLAVE_FF, NULL, 0);
        break;

    case SLAVE_FB:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        //MsgSend(SYS_KEY_ID_FAST_BACKWARD);
        MsgSend(MSG_FB_START);
        ResponseCommand(SLAVE_FB, NULL, 0);
        break;
    case SLAVE_STOP_FF_FB:
        /*
        if(!isUSBorSD())
        break;
        SuspendTick = 0;
        MessageSend(MSG_FIFO_KEY, MSG_FF_FB_END);
        SlaveSendResp(SLAVE_STOP_FF_FB, SlaveRespBuf, 0);
        */
        break;
    case SLAVE_SEEK_PLAY_TIME:
        if(datalen != 4)
            break;
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        isConmandError = 0;
        sTempDWord = (CmdData[2]<<24) +(CmdData[3]<<16) + (CmdData[4]<<8) +CmdData[5];
        if(sTempDWord <= gPlayContrl->CurSongInfo->duration)
        {
            gPlayContrl->CurPlayTime = sTempDWord;
            if(GetPlayState() == PLAYER_STATE_PLAYING)
            {
                DecoderTaskSeek(gPlayContrl->CurPlayTime);
            }
        }
        ResponseCommand(SLAVE_SEEK_PLAY_TIME, NULL, 0);
        break;
    case SLAVE_PLAY_PAUSE:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        MsgSend(MSG_PLAY_PAUSE);
        ResponseCommand(SLAVE_PLAY_PAUSE, NULL, 0);
        break;
    case SLAVE_SELECT_SONG:
        if(datalen != 4)
            return;
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        sTempWord = (CmdData[2]<<8) + CmdData[3];
        sTempWord2 = (CmdData[4]<<8) + CmdData[5];
        if((sTempWord > gPlayContrl->FolderHandle.FsContext->gFsInfo.FolderSum) || (sTempWord == 0))
        {
            break;
        }
        if(gPlayContrl->CurFolderIndex != sTempWord)
        {
            if(!FolderOpenByNum(&gPlayContrl->FolderHandle, NULL, sTempWord))
            {
                break;
            }
        }
        if((sTempWord2 <= 0) || (sTempWord2 > gPlayContrl->FolderHandle.RecFileCnt))
        {
            break;
        }   /**/
        // 停止播放
        AudioPlayerStop();
        gPlayContrl->CurFileIndex = sTempWord2;
        gPlayContrl->CurFolderIndex = sTempWord;

        AudioPlayerPlayPause();

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 1;
        ResponseCommand(SLAVE_SELECT_SONG, playcontrol_rsp_buffer, 1);
        break;

    case SLAVE_GET_PLAY_STATUS:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gSys.CurModuleID;
        playcontrol_rsp_buffer[1] = gPlayContrl->State;
        playcontrol_rsp_buffer[2] = ((gPlayContrl->CurFolderIndex)>>8)&0xff;
        playcontrol_rsp_buffer[3] = (gPlayContrl->CurFolderIndex)&0xff;
        playcontrol_rsp_buffer[4] = ((gPlayContrl->FolderHandle.ValidFolderNum)>>8)&0xff;
        playcontrol_rsp_buffer[5] = (gPlayContrl->FolderHandle.ValidFolderNum)&0xff;
        playcontrol_rsp_buffer[6] = ((gPlayContrl->CurFileIndex)>>8)&0xff;
        playcontrol_rsp_buffer[7] = ((gPlayContrl->CurFileIndex)&0xff);
        playcontrol_rsp_buffer[8] = ((gPlayContrl->PlayerFile.FileNumInFolder)>>8)&0xff;
        playcontrol_rsp_buffer[9] = (gPlayContrl->PlayerFile.FileNumInFolder)&0xff;
        playcontrol_rsp_buffer[10] = ((gPlayContrl->CurPlayTime)>>24)&0xff;
        playcontrol_rsp_buffer[11] = ((gPlayContrl->CurPlayTime)>>16)&0xff;
        playcontrol_rsp_buffer[12] = ((gPlayContrl->CurPlayTime)>>8)&0xff;
        playcontrol_rsp_buffer[13] = ((gPlayContrl->CurPlayTime)&0xff);
        ResponseCommand(SLAVE_GET_PLAY_STATUS, playcontrol_rsp_buffer, 14);
        break;
    case SLAVE_GET_SONG_INFO:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gPlayContrl->CurSongInfo->stream_type;
        playcontrol_rsp_buffer[1] = (uint8_t)gPlayContrl->CurSongInfo->num_channels;
        playcontrol_rsp_buffer[2] = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 24);
        playcontrol_rsp_buffer[3] = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 16);
        playcontrol_rsp_buffer[4]  = (uint8_t)(gPlayContrl->CurSongInfo->sampling_rate >> 8);
        playcontrol_rsp_buffer[5] = (uint8_t)gPlayContrl->CurSongInfo->sampling_rate;

        playcontrol_rsp_buffer[6] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 24);
        playcontrol_rsp_buffer[7] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 16);
        playcontrol_rsp_buffer[8] = (uint8_t)(gPlayContrl->CurSongInfo->bitrate >> 8);
        playcontrol_rsp_buffer[9] = (uint8_t)gPlayContrl->CurSongInfo->bitrate;
        playcontrol_rsp_buffer[10] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 24);
        playcontrol_rsp_buffer[11] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 16);
        playcontrol_rsp_buffer[12] = (uint8_t)(gPlayContrl->CurSongInfo->duration >> 8);
        playcontrol_rsp_buffer[13] = (uint8_t)gPlayContrl->CurSongInfo->duration;
        playcontrol_rsp_buffer[14] = (uint8_t)gPlayContrl->CurSongInfo->vbr_flag;
        ResponseCommand(SLAVE_GET_SONG_INFO, playcontrol_rsp_buffer, 15); //17
        break;
    case SLAVE_SET_REPEAT_MODE:
        if(datalen != 1)
            break;
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        isConmandError = 0;
        if(CmdData[2] < PLAY_MODE_SUM)
        {
            gPlayContrl->CurPlayMode = CmdData[2];
        }

        ResponseCommand(SLAVE_SET_REPEAT_MODE, NULL, 0);
        break;
    case SLAVE_GET_REPEAT_MODE:
        isConmandError = 0;
        playcontrol_rsp_buffer[0] =  gPlayContrl->CurPlayMode;
        ResponseCommand(SLAVE_GET_REPEAT_MODE, playcontrol_rsp_buffer, 1);
        break;
#if 1//def FUNC_TAG_EN	
    case SLAVE_GET_TITLE:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->title[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->title[0], temp);
        ResponseCommand(SLAVE_GET_TITLE, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_ARTIST:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->artist[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->artist[0], temp);
        ResponseCommand(SLAVE_GET_ARTIST, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_ALBUM:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }

        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->album[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->album[0], temp);
        ResponseCommand(SLAVE_GET_ALBUM, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_COMMENT:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->comment[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->comment[0], temp);
        ResponseCommand(SLAVE_GET_COMMENT, playcontrol_rsp_buffer, temp + 3);
        break;
    case SLAVE_GET_YEAR:
        if(!IsPlayerEnvironmentOk())
        {
            break;
        }
        if((gSys.CurModuleID != MODULE_ID_PLAYER_SD) && (gSys.CurModuleID != MODULE_ID_PLAYER_USB))
        {
            break;
        }
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = 0;//gTagInfo.TagType;
        playcontrol_rsp_buffer[1] = gPlayContrl->CurSongInfo->char_set;
        for(P = (uint16_t*)(&gPlayContrl->CurSongInfo->year[0]); *P != 0; P++)//计算Artist长度
        {
            temp++;
        }
        temp = temp * 2;
        if(temp > 30)
        {
            temp = 30;
        }
        playcontrol_rsp_buffer[2] = temp;
        memcpy(&playcontrol_rsp_buffer[3], &gPlayContrl->CurSongInfo->year[0], temp);
        ResponseCommand(SLAVE_GET_YEAR, playcontrol_rsp_buffer, temp + 3);
        break;
#endif
    case SLAVE_VOLADD:
    	isConmandError = 0;
    	MsgSend(MSG_VOL_UP);
    	ResponseCommand(SLAVE_VOLADD, playcontrol_rsp_buffer, 0);
    	break;

    case SLAVE_VOLSUB:
    	isConmandError = 0;
    	MsgSend(MSG_VOL_DW);
    	ResponseCommand(SLAVE_VOLSUB, playcontrol_rsp_buffer, 0);
    	break;
    case SLAVE_SET_VOL:
        if(CmdData[2] <= MAX_VOLUME)
        {
            gSys.Volume = CmdData[2];
        }
#ifdef FUNC_LINEIN_EN
        if(gSys.CurModuleID == MODULE_ID_LINEIN)    //line in
        {
            SetSysVol();
        }
#endif
#ifdef FUNC_RADIO_EN
        else if(gSys.CurModuleID == MODULE_ID_RADIO)
        {
            //RadioSetVolume(gSys.Volume);
        }
        else
#endif
        {
            SetSysVol();
        }
        isConmandError = 0;
        ResponseCommand(SLAVE_SET_VOL, playcontrol_rsp_buffer, 0);
        break;
    case SLAVE_GET_VOL:
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gSys.Volume;
        ResponseCommand(SLAVE_GET_VOL, playcontrol_rsp_buffer, 1);
        break;
    case SLAVE_SET_EQ:
        if(CmdData[2] < EQ_STYLE_SUM)
        {
            if(gSys.Eq == EQ_STYLE_OFF && CmdData[2] != EQ_STYLE_OFF)
            {
                EqEnable();
            }
            else if(gSys.Eq != EQ_STYLE_OFF && CmdData[2] == EQ_STYLE_OFF)
            {
                EqDisable();
            }
            gSys.Eq = CmdData[2];
            if(gPlayContrl->CurSongInfo != NULL)
            {
				uint32_t SampleRateTmp = DacAdcSampleRateGet();
				if(SampleRateTmp == 0)
				{
					SampleRateTmp = 44100;
				}

				EqStyleSelect(SampleRateTmp, gSys.Eq);
			}
            isConmandError = 0;
            ResponseCommand(SLAVE_SET_EQ, playcontrol_rsp_buffer, 0);
        }
        break;
    case SLAVE_GET_EQ:
        isConmandError = 0;
        playcontrol_rsp_buffer[0] = gSys.Eq;
        ResponseCommand(SLAVE_GET_EQ, playcontrol_rsp_buffer, 1);
        break;
    case SLAVE_MUTE:
        isConmandError = 0;
        if(!gSys.MuteFlag)
        {
            gSys.MuteFlag = TRUE;
            DacSoftMuteSet(TRUE, TRUE);
        }
        ResponseCommand(SLAVE_MUTE, playcontrol_rsp_buffer, 0);
        break;
    case SLAVE_UNMUTE:
        isConmandError = 0;
        if(gSys.MuteFlag)
        {
            gSys.MuteFlag = FALSE;
#ifdef FUNC_LINEIN_EN
            if(gSys.CurModuleID == MODULE_ID_LINEIN)    //line in
            {
                SetSysVol();
            }
            else
#endif
#ifdef FUNC_RADIO_EN
                if(gSys.CurModuleID == MODULE_ID_RADIO)
                {
                    //RadioSetVolume(gSys.Volume);
                }
                else
#endif
                {
                    SetSysVol();
                }
            DacSoftMuteSet(FALSE, FALSE);
        }
        ResponseCommand(SLAVE_UNMUTE, playcontrol_rsp_buffer, 0);
        break;
    }
}

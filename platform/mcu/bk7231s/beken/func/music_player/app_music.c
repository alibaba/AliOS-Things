#include "include.h"
#include <string.h>
#include "error.h"
#include "typedef.h"
#include "arm_arch.h"
#include "uart_pub.h"
#include "mem_pub.h"
#include "music_msg_pub.h"
#include "driver_audio_if_pub.h"
#include "playmode_pub.h"
#include "app.h"
#include "app_music.h"
#include "mp3dec.h"
#include "mp3common.h"
#include "coder.h"
#include "layer21.h"

MEDIA_CORE_T g_media_core = {0};


#if (CONFIG_APP_MP3PLAYER == 1)
#define USE_EXTERNAL_DAC

extern void ClearMP3(MP3DecInfo *mp3DecInfo);
UINT8 media_app_msg_handler(UINT32 msg_id);
UINT16 currentDir = 0;
UINT32 file_ptr_save = 0;
static app_player_ctrl app_player;
app_player_ctrl_backup player_config_backup;

UINT8 *readBuf;   
UINT16 *mp3_pcm_ptr; 
UINT16 *mp3_pcm_ptr_tmp; 
INT32  pcm_size = 0;
volatile UINT8 aud_buff_full = 0;
volatile UINT32 buf_free;
         
UINT16  *aulawsmpl;
UINT16  *alawtbl;
UINT16  *ulawtbl;
UINT8  *rbbuf;
HMP3Decoder hMP3Decoder;    //错误信息结构体
MP3FrameInfo mp3FrameInfo;     //mp3文件信息结构体
MP3DecInfo *mp3decinfo;

UINT16 input_number;
UINT8 init=0;
UINT8 mp3_play_flag = 0;  
UINT8 t=0;
static UINT8 media_err_cnt = 0;
UINT8 play_derect = 0;   // 1:next, 0 :previous
UINT8 appPlayerPlayMode=0;
UINT8 media_playing_flag = 0;//current meida playing status
FIL *newFatfs;

static void app_player_mp3_file_end( void );
static void app_player_process_file_end( void );

static void mp3_mem_init(void)
{
	L2DecodeContext *l2i;

	if(NULL == hMP3Decoder)
	{
	    hMP3Decoder = MP3InitDecoder();
	}
	if(NULL == hMP3Decoder)
	{
		APP_MUSIC_PRT("hMP3Decoder_malloc_fail\r\n");
		return;
	}

	mp3decinfo = (MP3DecInfo*)hMP3Decoder;
	readBuf= (unsigned char*)ff_memalloc(READBUF_SIZE);   //FOR READ BUF
	mp3decinfo->mainBuf = readBuf; 
	mp3decinfo->mainBuf_ptr = mp3decinfo->mainBuf + BIT_RESVOR_SIZE;
	mp3decinfo->mainBuf_len = 0;
	mp3decinfo->err_cnt = 0;

	mp3_pcm_ptr = (short*)mp3decinfo->HuffmanInfoPS;

	l2i = (L2DecodeContext *)(mp3decinfo->L2DecInfo);
	l2i->sb_samples = (int32_t*)(mp3decinfo->SubbandInfoPS);
	l2i->synth_buf = (INT16 *)(mp3decinfo->IMDCTInfoPS);


	return;
}


static void mp3_mem_uninit(void)
{
	if(readBuf)
	{
		APP_MUSIC_PRT("mp3 mem free	1\r\n");
		os_free(readBuf);
		readBuf = NULL;
		mp3decinfo->mainBuf = NULL;
	}

	if(hMP3Decoder)
	{
		APP_MUSIC_PRT("mp3 mem free	3\r\n");
		MP3FreeDecoder(hMP3Decoder);
		hMP3Decoder = NULL;
		mp3decinfo = NULL;
	}
}


static void app_player_mp3_init(void)
{
    mp3_mem_init();
    init=0;
	t = 0;
}


static int app_player_to_first_file( void )
{
	FILE_INFO *pFile;

	uint32 player_flag = app_player.player_flag;
	uint32 schedule_cmd = app_player.schedule_cmd;
	memset( (uint8 *)&app_player, 0, sizeof( app_player_ctrl ) );
	app_player.player_flag = player_flag;
	app_player.schedule_cmd = schedule_cmd;
	if( 0 == get_musicfile_count())
	{
		return -1;
	}
	newFatfs = Get_File_From_Number(0);
	pFile = get_file_info();

	app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
	app_player.file_index = 0;
	app_player.file_info.file_blks = newFatfs->obj.objsize;
	app_player.file_info.file_start_cluster = newFatfs->obj.sclust;

	app_player.block_played = 0;
	memcpy(app_player.file_info.ext_name,pFile->extname,3);
	memcpy(app_player.file_info.filename,pFile->fname,8);
	return 0;
}


static int app_player_get_next_stream_file( void )
{
	FILE_INFO *pFile ;
	uint16 musicfilecount = get_musicfile_count();

	if( 0 == musicfilecount)
	{
		return -1;
	}

	app_player.file_index++;
	if(app_player.file_index > (musicfilecount-1))
	app_player.file_index = 0;

	newFatfs = Get_File_From_Number(app_player.file_index);
	pFile = get_file_info();

	app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
	app_player.file_info.file_blks = newFatfs->obj.objsize;
	app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
	app_player.block_played = 0;

	memcpy(app_player.file_info.ext_name,pFile->extname,3);
	memcpy(app_player.file_info.filename,pFile->fname,8);

	return 0;
}


static int app_player_get_prev_stream_file( void )
{
	FILE_INFO *pFile;
	uint16 musicfilecount = get_musicfile_count();

	if( 0 == musicfilecount)
	{
		return -1;
	}

	app_player.file_index--;
	if(app_player.file_index < 0)
	app_player.file_index = musicfilecount-1;

	newFatfs = Get_File_From_Number(app_player.file_index);
	pFile = get_file_info();

	app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
	app_player.file_info.file_blks = newFatfs->obj.objsize;
	app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
	app_player.block_played = 0;

	memcpy(app_player.file_info.ext_name,pFile->extname,3);
	memcpy(app_player.file_info.filename,pFile->fname,8);

	return 0;
}
static int app_player_get_fix_stream_file( uint16  fileIndex )
{
	FILE_INFO *pFile ;
	uint16 musicfilecount = get_musicfile_count();

	if( 0 == musicfilecount)
	{
		return -1;
	}

	app_player.file_index = fileIndex;
	if(app_player.file_index > (musicfilecount-1))
		app_player.file_index = 0;

	newFatfs = Get_File_From_Number(app_player.file_index);
	pFile = get_file_info();

	app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
	app_player.file_info.file_blks = newFatfs->obj.objsize;
	app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
	app_player.block_played = 0;

	memcpy(app_player.file_info.ext_name,pFile->extname,3);
	memcpy(app_player.file_info.filename,pFile->fname,8);

	return 0;
}

static int app_player_play_mp3_file( void )
{	
	int err; 
	static UINT32 error_cnt = 0;
	
	if(!hMP3Decoder && mp3_play_flag)
	{
		return ERR_UNKNOWN;
	}
	err = ERR_MP3_NONE;
	if(aud_buff_full)
		goto fill_aud_buff;

	err = MP3Decode(hMP3Decoder, mp3_pcm_ptr, &pcm_size);
	
	if(err != ERR_MP3_NONE)
	{	
		switch(err)
		{

			case ERR_MP3_INDATA_UNDERFLOW:
			{
				if(f_EOF(newFatfs) == FR_FILE_END)
				{
					APP_MUSIC_PRT("===file end====\r\n");
					app_player_mp3_file_end();
					app_player_process_file_end();
				}
				else
				{
					mp3decinfo->decode_state = MP3_DECODE_FIND_ID3_INFO;
					APP_MUSIC_PRT("not the end of the file\r\n");
				}
				break;
			}	
			case ERR_MP3_READ_DATA_FAILED:
			{
				APP_MUSIC_PRT("**mp3 read error**\r\n");
				break;
			}
			case ERR_MP3_FILE_END:
			{
				app_player_mp3_file_end();
				APP_MUSIC_PRT("mmmm\r\n");
				app_player_process_file_end();
				APP_MUSIC_PRT("nnnnn\r\n");
				break;
			}
			case ERR_MP3_MAINDATA_UNDERFLOW:
			{
				mp3decinfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
				break;
			}
			case ERR_MP3_DECODE_MAX_ERR:
				APP_MUSIC_PRT("---mp3 max error------\r\n");
				app_player_mp3_file_end();
				app_player_process_file_end();
				break;
			default:
				ClearMP3((MP3DecInfo *)hMP3Decoder);
				mp3decinfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
			break;

		}
		APP_MUSIC_PRT("mp3 error retrun:%x,%x\r\n",app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE,app_player.schedule_cmd & 0xFFFF );
		return err;
	}
	
	error_cnt = 0;
	// wait for at least 4 corret frame decode. 
	if(init == 0)
	{
		if(t <= APP_PLAYER_AUDIO_INITION_BEGIN)
		{
			if(mp3decinfo->decode_state == MP3_DECODE_FIND_SYNC_WORD)
				t++;
			return ERR_MP3_NONE;
		}
		
		MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo); //获得解码
		APP_MUSIC_PRT("比特率%dkb/s, 原采样率%dHZ\r\n",
		(mp3FrameInfo.bitrate)/1000,mp3FrameInfo.samprate);
		APP_MUSIC_PRT("原声道数%d, MPAG:%d,层:%d\r\n",
		mp3FrameInfo.nChans,mp3FrameInfo.version,mp3FrameInfo.layer);
		APP_MUSIC_PRT("outputsamps:%d\r\n",mp3FrameInfo.outputSamps);
		APP_MUSIC_PRT("每帧数据:%d\r\n",mp3decinfo->framesize);

		aud_initial(mp3FrameInfo.samprate, 2, 16 );
		init=1;	
	}
	mp3_pcm_ptr_tmp = mp3_pcm_ptr;
	MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
	if(mp3FrameInfo.nChans == 1)
	{
		Convert_Mono(mp3_pcm_ptr,pcm_size);
		pcm_size <<= 1;
	}
	
fill_aud_buff:
	buf_free = aud_get_buffer_size();
	if(buf_free > pcm_size*2)
	{
		aud_fill_buffer((uint8*)mp3_pcm_ptr_tmp, pcm_size*2);
		aud_buff_full = 0;
		pcm_size = 0;
	}
	else 
	{
		/*at least 128 byte space in audbuffer, we can fill it*/
		if(buf_free>=128)
		{
		//we cann't fill audbuffer to full, should be 4bytes less 
		//see func: aud_fill_buffer
			buf_free = (buf_free&0xfffffffe)-4;    
			aud_fill_buffer((uint8*)mp3_pcm_ptr_tmp, buf_free);
			mp3_pcm_ptr_tmp += (buf_free>>1);
			pcm_size -= (buf_free>>1);
		}
		aud_buff_full = 1;
	}

	if(is_aud_opened()==0)
		aud_open();

	return ERR_MP3_NONE;
}


static void app_player_play_pause( UINT8 play_pause  )
{
	UINT32 readbytes = 0;

	if(get_musicfile_count() == 0 )
		return;

	APP_MUSIC_PRT("---play_pause:%x,flag:%x----\r\n",play_pause,app_player.player_flag &  APP_PLAYER_FLAG_PLAY_PAUSE);
	if( (play_pause == 0) &&  (app_player.player_flag &  APP_PLAYER_FLAG_PLAY_PAUSE ))  // pause
	{
		app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_PAUSE;
		aud_close();
	}
	else if( play_pause && !( app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE))
	{
		if( strncmp( app_player.file_info.ext_name, "MP3", 3 ) == 0 )
		{
			if(mp3_play_flag != 1)
			{
			    APP_MUSIC_PRT("start playing start \r\n");

				app_player_mp3_init();
				if(mp3decinfo) 
				{
					mp3decinfo->decode_state = MP3_DECODE_FIND_ID3_INFO;
			        
				}
				else	
				{
					APP_MUSIC_PRT("mp3 memery initial unsucssed\r\n");
				}
				mp3_play_flag = 1;
			}
			else
			{
				mp3decinfo->decode_state = MP3_DECODE_FIND_SYNC_WORD;
				init = 0;
				t = 0;
				aud_buff_full = 0;
			}
		}
		else
		{
			return;
		}
	   	 app_player.player_flag |= APP_PLAYER_FLAG_PLAY_PAUSE;
	}
}


static void app_player_process_schedule_cmd( void )
{
	switch( app_player.schedule_cmd & 0xFFFF )
	{
		case APP_PLAYER_SCHEDULE_CMD_MODE_CHANGE:
			aud_close();
			app_player.schedule_cmd = 0;
			break;
		
		case APP_PLAYER_SCHEDULE_CMD_FIX_SONG:
			if(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH )
			{
				int get_next_return=0;
				
				play_derect = 1;

				app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
				if(mp3_play_flag == 1)
				{
					APP_MUSIC_PRT("END 3\r\n");
					app_player_mp3_file_end();
				}

				get_next_return = app_player_get_fix_stream_file(input_number-1); 	 // get next file
				input_number = 0;

				if( app_player.schedule_cmd  & APP_PLAY_SCHEDULE_CMD_PLAY_FLAG )
					app_player_play_pause(1);
				(void)get_next_return;
			}
			app_player.schedule_cmd = 0;
			break;

		case APP_PLAYER_SCHEDULE_CMD_NEXT_SONG:
			if(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH )
			{
				int get_next_return=0;
				
				play_derect = 1;
				app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;

				if(mp3_play_flag == 1)
				{
					APP_MUSIC_PRT("END 3\r\n");
					app_player_mp3_file_end();
				}
				get_next_return = app_player_get_next_stream_file();         // get next file

				if( app_player.schedule_cmd  & APP_PLAY_SCHEDULE_CMD_PLAY_FLAG )
					app_player_play_pause(1);
				(void)get_next_return;
			}
			app_player.schedule_cmd = 0;
			break;

		case APP_PLAYER_SCHEDULE_CMD_PREV_SONG:
			if(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH )
			{
				int get_pre_return=0;
				play_derect = 0;
				app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
				if(mp3_play_flag == 1)
				{
					APP_MUSIC_PRT("END 4\r\n");
					app_player_mp3_file_end();
				}
				get_pre_return = app_player_get_prev_stream_file();

				if( app_player.schedule_cmd  & APP_PLAY_SCHEDULE_CMD_PLAY_FLAG )
					app_player_play_pause(1);
				(void)get_pre_return;
			}
			app_player.schedule_cmd = 0;
			break;

		default:
			break;
	}
}


static void app_player_file_info_init( void )
{
	int file_index;
	FILE_INFO *pFile;
	if( 0 == get_musicfile_count())
	{
		return;
	}

	memset( (uint8 *)&app_player, 0, sizeof( app_player ) );
	app_player.player_flag |= (APP_PLAYER_FLAG_PLAY_CONTINOUS|APP_PLAYER_FLAG_PLAY_CYCLE|APP_PLAYER_FLAG_HW_ATTACH);

	file_index = player_config_backup.file_index;

	if(file_index > (get_musicfile_count()-1) || file_index < 0)
	{
		app_player_to_first_file();
	}
	else
	{
		newFatfs = Get_File_From_Number(file_index);
		pFile = get_file_info();
		app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_END;
		app_player.file_info.file_blks = newFatfs->obj.objsize;
		app_player.file_info.file_start_cluster = newFatfs->obj.sclust;
		app_player.block_played = 0;
		memcpy(app_player.file_info.ext_name,pFile->extname,3);
		memcpy(app_player.file_info.filename,pFile->fname,8);
		if(app_player.file_info.file_blks == player_config_backup.file_size_blks)
		{
			app_player.file_index = player_config_backup.file_index;
			app_player.block_played = player_config_backup.block_played;
			if( strncmp(app_player.file_info.ext_name, "MP3", 3 ) == 0 )
			{
				f_lseek(newFatfs, app_player.block_played);
			}
		}
		else
		{
			app_player_to_first_file();
		}
	}
}


static uint8 app_player_play_func( void )
{
	if( !(app_player.player_flag & APP_PLAYER_FLAG_HW_ATTACH ))
	{
		app_player_process_schedule_cmd();
		return 1;
	}

	if( app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE )
	{
		if( strncmp( app_player.file_info.ext_name, "MP3", 3 ) == 0 )
		{
			if(app_player_play_mp3_file())
			{
				return 2;   
			}
		}
	}
	app_player_process_schedule_cmd();
	if(aud_buff_full)
		return 1;
	else
		return 0;
}

static void app_player_mp3_file_end( void )
{
	t = 0;
	init=0;
	media_err_cnt = 0;
	mp3_play_flag = 0;

	aud_buff_full = 0;
	aud_close();
	
	mp3_mem_uninit();
	
	rbbuf = NULL;
	f_close(newFatfs);
}
static void app_player_stop( void  )
{
	if( app_player.file_index < 0 )
		return;

	app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_PAUSE;
	if( strncmp( app_player.file_info.ext_name, "MP3", 3 ) == 0 )
	{
		app_player.block_played = 0;
	}
}

static void app_player_process_file_end( void )
{
	if(app_player.media_err_status == FR_DISK_ERR)
	{
		app_player.media_err_status = FR_OK;
		return;
	}
	app_player.player_flag &= ~APP_PLAYER_FLAG_PLAY_PAUSE;
	if( app_player.player_flag & APP_PLAYER_FLAG_PLAY_CONTINOUS )
	{
		int get_file_return = 0;

		if(appPlayerPlayMode==APP_PLAYER_MODE_PLAY_ONE)
			get_file_return = app_player_get_fix_stream_file(app_player.file_index); // get current file
		else
			get_file_return = app_player_get_next_stream_file();  // get next file
		app_player_play_pause(1);                                       // not the file list end, just play next file
		(void)get_file_return;
	}
	else // not continous play
	{
		if( app_player.player_flag & APP_PLAYER_FLAG_PLAY_CYCLE )   // cycle , play last index
		{
			f_lseek(newFatfs,0);
			app_player.block_played = 0;
			app_player_play_pause(1);
		}
		else                                                                                // just close
		{
			app_player_stop();
			aud_close();
		}
	}
}


static void app_player_unload(void)
{
	FRESULT err = FR_OK;
	app_player_play_pause(0);
	aud_close();
	if(mp3_play_flag == 1)
	{
		app_player_mp3_file_end();
	}
	media_uninit();
	memset( &app_player, 0, sizeof(app_player_ctrl) );
}

void media_msg_sender(void const *param_ptr)
{
	OSStatus ret;
	MEDIA_MSG_T msg;

	msg.id = (UINT32)param_ptr;
	ret = rtos_push_to_queue(&g_media_core.io_queue, &msg, BEKEN_NO_WAIT);
	if(kNoErr != ret)
	{
		APP_MUSIC_PRT("media_msg_sender_fail\r\n");
	}
}

static void app_media_thread( void *arg )
{
    OSStatus ret;
	MEDIA_MSG_T msg;
	UINT8 play_flag = 0;
	UINT32 msg_timeout_ms = BEKEN_WAIT_FOREVER;
	UINT8 meida_play=0;

	while(1)
	{	
		ret = rtos_pop_from_queue(&g_media_core.io_queue, &msg, msg_timeout_ms);
		if(kNoErr == ret)
		{
			play_flag = media_app_msg_handler(msg.id);
		}
#ifdef USE_EXTERNAL_DAC
		if(play_flag)
		{
			while(1)
			{
				if(app_player_play_func())
					break;
			}
		}
		msg_timeout_ms = 2;
#endif
	}
}

static void media_thread_uninit(void)
{
	if(g_media_core.handle)
	{
		rtos_delete_thread(&g_media_core.handle);
		g_media_core.handle = 0;
	}
	
	if(g_media_core.io_queue)
	{
		rtos_deinit_queue(&g_media_core.io_queue);
		g_media_core.io_queue = 0;
	}
	
	g_media_core.queue_item_count = 0;
	g_media_core.stack_size = 0;
}

#endif


void media_thread_init(void)
{
#if (CONFIG_APP_MP3PLAYER == 1)
	OSStatus ret;

	g_media_core.queue_item_count = MEDIA_QITEM_COUNT;
	g_media_core.stack_size = MEDIA_STACK_SIZE;
	
	ret = rtos_init_queue(&g_media_core.io_queue, 
							"media queue",
							sizeof(MEDIA_MSG_T),
							g_media_core.queue_item_count);
	if (kNoErr != ret) 
	{
		APP_MUSIC_PRT("Create media queue failed\r\n");
		goto fail;
	}

    ret = rtos_create_thread(&g_media_core.handle, 
				            THD_MEDIA_PRIORITY,
				            "media_thread", 
				            (beken_thread_function_t)app_media_thread, 
				            (unsigned short)g_media_core.stack_size, 
				            (beken_thread_arg_t)0);
	
	if (kNoErr != ret) 
	{
		APP_MUSIC_PRT("Create medai thread failed\r\n");
		goto fail;
	}
#ifdef USE_EXTERNAL_DAC
	if(aud_hw_init()!=kNoErr)
		goto fail;
#endif	
	return;
fail:
	media_thread_uninit();
#endif
}


#if (CONFIG_APP_MP3PLAYER == 1)
UINT8 media_app_msg_handler(UINT32 msg_id)
{
	UINT8 ret = 0;
	static UINT8 media_ok = 0;
	switch(msg_id)
	{
		case MSG_SD_ATTACH:
			APP_MUSIC_PRT("---attach---\r\n");
#if (CONFIG_APP_MP3PLAYER == 1)
			media_init(0);
			if(get_fat_ok_flag())
			{
				media_ok = 1;
				app_player_file_info_init();
				app_player_play_pause( !(app_player.player_flag &  APP_PLAYER_FLAG_PLAY_PAUSE ) );
				ret = 1;
			}
#endif
			break;
		case MSG_SD_DETACH:
			media_ok = 0;
			APP_MUSIC_PRT("---dettach---\r\n");
#if (CONFIG_APP_MP3PLAYER == 1)
			 app_player_unload();
#endif
			break;	
		case MSG_MUSIC_PLAY:
			if(media_ok == 1)
			{
				ret= 1;
			}
			else
				ret = 0;
			break;
			
#ifdef USE_EXTERNAL_DAC		
		case MSG_KEY_PLAY|KEY_LONG:
		case MSG_KEY_PLAY:
			if(app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE)
				ret = 0;
			else
				ret = 1;
			app_player_play_pause(!(app_player.player_flag & APP_PLAYER_FLAG_PLAY_PAUSE));
			break;
#endif
	}
	return ret;
}
#endif

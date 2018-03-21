/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/aos.h"
#include "hal/soc/soc.h"
#include "ff.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pthread.h"
#include "iniparserapi.h"
#include "cdx_log.h"
#include "xplayer.h"
#include "CdxTypes.h"
#include "xplayer_i.h"

static int player_create_mp3_list(void)
{
	FRESULT fs_ret;
	FF_DIR music_dir;
	FIL fp;
	char mp3_list_path[32];
	char file_name[128];
	FILINFO file_info;
	uint32_t bytes;
	uint32_t music_idx = 0;

	LOG("Create music list...");
	fs_ret = f_opendir(&music_dir, MUSIC_DIR);
	if (fs_ret != FR_OK) {
		LOG("open music dir '%s' failed", MUSIC_DIR);
		return -1;
	}
	sprintf(mp3_list_path, "%s/%s", MUSIC_DIR, MUSIC_FILE);
	// check the list exist or not
	if (f_stat(mp3_list_path, &file_info) == FR_OK) {
		LOG("%s exist...", mp3_list_path);
		goto closedir_out;
	}
	fs_ret = f_open(&fp, mp3_list_path, FA_CREATE_NEW | FA_WRITE | FA_READ);
	if(fs_ret != FR_OK) {
		LOG("open file error %d", fs_ret);
		goto dir_fail;
	}
	// read music files and write the name to list
	while (1) {
		fs_ret = f_readdir(&music_dir, &file_info);
		if (fs_ret == FR_OK && file_info.fname[0]) {
			if (file_info.fattrib & AM_DIR) {
				LOG("find a FF_DIR, skip...");
				continue;
			} else {
				if (strstr(file_info.fname, ".mp3") || strstr(file_info.fname, ".MP3")) {
					LOG("find a mp3: %s", file_info.fname);
					strcpy(file_name, file_info.fname);
					strcat(file_name, "\n");
					fs_ret = f_write(&fp, file_name, strlen(file_name), &bytes);
					if (fs_ret != FR_OK) {
						LOG("fail to write file name %s(1)", file_name);
						goto file_fail;
					}
					if (bytes != strlen(file_name)) {
						LOG("fail to write file name %s(2)", file_name);
						goto file_fail;
					}
				}
			}
		} else {
			LOG("scan music end...");
			break;
		}
	}
closefp_out:
	f_close(&fp);
closedir_out:
	f_closedir(&music_dir);
	return 0;
file_fail:
	f_close(&fp);
dir_fail:
	f_closedir(&music_dir);
	return -1;
}

static int player_get_song_name_from_list(uint32_t idx, char* name)
{
	FRESULT fs_ret;
	FF_DIR music_dir;
	FILINFO file_info;
	FIL fp;
	uint32_t offs = 0;
	uint32_t len = 0;
	char tmp_name[128] = {0};
	uint32_t index = 0;
	char c;
	char mp3_list_path[32];
	
	fs_ret = f_opendir(&music_dir, MUSIC_DIR);
	if (fs_ret != FR_OK) {
		LOG("open music dir '%s' failed", MUSIC_DIR);
		return -1;
	}
	sprintf(mp3_list_path, "%s/%s", MUSIC_DIR, MUSIC_FILE);
	// check the list exist or not
	if (f_stat(mp3_list_path, &file_info) != FR_OK) {
		LOG("%s is not exist...", mp3_list_path);
		goto dir_fail;
	}
	fs_ret = f_open(&fp, mp3_list_path, FA_READ);
	if(fs_ret != FR_OK) {
		LOG("open file error %d", fs_ret);
		goto dir_fail;
	}
	while (1) {
		if (f_eof(&fp)) {
			LOG("search to file end");
			break;
		}
		fs_ret = f_read(&fp, &c, 1, &len);
		if (fs_ret != FR_OK) {
			LOG("fail to read file");
			goto file_fail;
		}
		if (len != 1) {
			LOG("fail to read file");
			goto file_fail;
		}
		if (c == '\n') {
			if (index == idx) {
				memcpy(name, tmp_name, offs);
				LOG("get file[%d] name [%s]", idx, tmp_name);
				break;
			} else {
				index++;
				offs = 0;
				memset(tmp_name, 0, 128);
				continue;
			}
		}
		tmp_name[offs] = c;
		offs++;
	}

	f_close(&fp);
	f_closedir(&music_dir);
	return 0;
file_fail:
	f_close(&fp);
dir_fail:
	f_closedir(&music_dir);
	return -1;
}

static int player_print_mp3_list(void)
{
	uint32_t idx = 0;
	char file_name[128];

	while (1) {
		memset(file_name, 0, 128);
		player_get_song_name_from_list(idx++, file_name);
		if (file_name[0] == 0) {
			break;
		}
	}
	return 0;

}

//* a callback for awplayer.
static int CallbackForAwPlayer(void* pUserData, int msg, int ext1, void* param)
{
    PlayerContext* Player = (PlayerContext*)pUserData;

    switch(msg)
    {
        case AWPLAYER_MEDIA_INFO:
        {
            switch(ext1)
            {
                case AW_MEDIA_INFO_NOT_SEEKABLE:
                {
                    Player->mSeekable = 0;
                    LOG("info: media source is unseekable.");
                    break;
                }
                case AW_MEDIA_INFO_RENDERING_START:
                {
                    LOG("info: start to show pictures.");
                    break;
                }
            }
            break;
        }

        case AWPLAYER_MEDIA_ERROR:
        {
            pthread_mutex_lock(&Player->mMutex);
            Player->mStatus = STATUS_STOPPED;
            Player->mPreStatus = STATUS_STOPPED;
            LOG("error: open media source fail.");
            pthread_mutex_unlock(&Player->mMutex);
            Player->mError = 1;

            LOG(" error : how to deal with it");
            break;
        }

        case AWPLAYER_MEDIA_PREPARED:
        {
            LOG("info : preared");
            Player->mPreStatus = Player->mStatus;
            Player->mStatus = STATUS_PREPARED;
			sem_post(&Player->mPrepared);
            LOG("info: prepare ok.");
            break;
        }

        case AWPLAYER_MEDIA_PLAYBACK_COMPLETE:
        {
            LOG("info : mStop");
            sem_post(&Player->mStoped);//* stop the player.
		    Player->mPreStatus = Player->mStatus;
		    Player->mStatus    = STATUS_PLAYEND;
            LOG("info : mStoped");
            //* TODO
            break;
        }

        case AWPLAYER_MEDIA_SEEK_COMPLETE:
        {
            pthread_mutex_lock(&Player->mMutex);
            Player->mStatus = Player->mPreStatus;
            LOG("info: seek ok.\n");
            pthread_mutex_unlock(&Player->mMutex);
            break;
        }

        default:
        {
            //printf("warning: unknown callback from AwPlayer.\n");
            break;
        }
    }

    return 0;
}

static PlayerContext* player_create(void)
{
	SoundCtrl* sound;
	PlayerContext* player;
	int size=sizeof(PlayerContext);
	LOG("xplayer create...size=%d",size);
	player = (PlayerContext *)aos_malloc(size);
	if (player == NULL) {
		return NULL;
	}

	AwParserInit();
	AwStreamInit();
	
	memset(player, 0, size);
	pthread_mutex_init(&player->mMutex, NULL);
    sem_init(&player->mStoped, 0, 0);
    sem_init(&player->mPrepared, 0, 0);
    LOG("start create...");
    player->mAwPlayer = XPlayerCreate();
    if(player->mAwPlayer == NULL)
    {
        LOG("can not create AwPlayer, quit.");
		free(player);
		player = NULL;
        goto fail;
    } else {
        LOG("create AwPlayer success.\n");
    }

    //* set callback to player.
    XPlayerSetNotifyCallback(player->mAwPlayer, CallbackForAwPlayer, (void*)player);
    //* check if the player work.
    if(XPlayerInitCheck(player->mAwPlayer) != 0)
    {
        LOG("initCheck of the player fail, quit.");
        XPlayerDestroy(player->mAwPlayer);
        player->mAwPlayer = NULL;
		free(player);
		player = NULL;
        goto fail;
    }
    LOG("AwPlayer check success.");
    
    sound = SoundDeviceCreate();
    if (sound == NULL) {
    	LOG("SoundDeviceCreate fail, quit.");
        XPlayerDestroy(player->mAwPlayer);
        player->mAwPlayer = NULL;
		free(player);
		player = NULL;
        goto fail;
    }

    XPlayerSetAudioSink(player->mAwPlayer, (void*)sound);

	return player;

fail:
	return NULL;
}

static void player_destroy(PlayerContext* player)
{
    LOG("destroy AwPlayer.");

    if(player->mAwPlayer != NULL)
    {
        XPlayerDestroy(player->mAwPlayer);
        player->mAwPlayer = NULL;
    }

    LOG("destroy AwPlayer 1.");
    pthread_mutex_destroy(&player->mMutex);

    sem_destroy(&player->mPrepared);
    sem_destroy(&player->mStoped);

    free(player);

    return;
}

static void player_set_source(PlayerContext *player, char* pUrl)
{
    player->mSeekable = 1;

	LOG("xplayer set source %s...", pUrl);
    //* set url to the AwPlayer.
    if(XPlayerSetDataSourceUrl(player->mAwPlayer,
                 (const char*)pUrl, NULL, NULL) != 0)
    {
        LOG("error:");
        LOG("    AwPlayer::setDataSource() return fail.");
        return;
    }
	LOG("setDataSource end");

    //* start preparing.
    pthread_mutex_lock(&player->mMutex);

	if (!strncmp(pUrl, "http://", 7)) {
	    if(XPlayerPrepareAsync(player->mAwPlayer) != 0)
	    {
	        LOG("error:");
	        LOG("    AwPlayer::prepareAsync() return fail.");
	        pthread_mutex_unlock(&player->mMutex);
	        return;
	    }
		sem_wait(&player->mPrepared);
	}

    player->mPreStatus = STATUS_STOPPED;
    player->mStatus    = STATUS_PREPARING;
    LOG("preparing...");
    pthread_mutex_unlock(&player->mMutex);
}

static void player_play(PlayerContext *player)
{
    if(XPlayerStart(player->mAwPlayer) != 0)
    {
        LOG("error:");
        LOG("    AwPlayer::start() return fail.");
        return;
    }
    player->mPreStatus = player->mStatus;
    player->mStatus    = STATUS_PLAYING;
    LOG("playing.");
}

static void player_pause(PlayerContext *player)
{
	LOG("xplayer pause...");
    if(XPlayerPause(player->mAwPlayer) != 0)
    {
        LOG("error:");
        LOG("    AwPlayer::pause() return fail.");
        return;
    }
    player->mPreStatus = player->mStatus;
    player->mStatus    = STATUS_PAUSED;
    LOG("paused.");
}

static void player_stop(PlayerContext *player)
{
	LOG("xplayer stop...");
    if(XPlayerReset(player->mAwPlayer) != 0)
    {
        LOG("error:");
        LOG("    AwPlayer::reset() return fail.");
        return;
    }
    player->mPreStatus = player->mStatus;
    player->mStatus    = STATUS_STOPPED;
    LOG("stopped.");
}

static void player_record(PlayerContext *player)
{

    LOG("record.");
}

static PlayerStatus player_get_status(PlayerContext *player)
{
    return player->mStatus;
}

static xplayer_t xPlayer = {
	.create            = player_create,
	.destroy           = player_destroy,
	.set_source        = player_set_source,
	.play              = player_play,
	.pause             = player_pause,
	.stop              = player_stop,
	.record            = player_record,
	.status            = player_get_status,
	.create_music_list = player_create_mp3_list,
	.get_song_name     = player_get_song_name_from_list,
	.show_music_list   = player_print_mp3_list,
};

xplayer_t* xPlayer_create(void)
{
	xPlayer.Player = xPlayer.create();
	if (xPlayer.Player != NULL) {
		return &xPlayer;
	}
	return NULL;
}

void xPlayerDestroy(void)
{
	if (xPlayer.Player) {
		xPlayer.destroy(xPlayer.Player);
	}
}

void xPlayerPlay(char* pUrl)
{
	LOG("xPlayerPlay..");
	if (xPlayer.Player) {
		if (xPlayer.status(xPlayer.Player) == STATUS_PAUSED) {
			LOG("now paused, playing...");
			xPlayer.play(xPlayer.Player);
			return;
		}
		if (xPlayer.status(xPlayer.Player) != STATUS_STOPPED ||
			xPlayerStatus() == STATUS_PLAYEND) {
			LOG("now not stop, stoping...");
			xPlayer.stop(xPlayer.Player);
			aos_msleep(5);
		}
		if (pUrl != NULL) {
			xPlayer.stop(xPlayer.Player);
			aos_msleep(5);
			LOG("now play %s ...", pUrl);
			xPlayer.set_source(xPlayer.Player, pUrl);
			xPlayer.play(xPlayer.Player);
		} else {
			char name[64] = {0};
			char path[64] = {0};
			LOG("NULL url, now play the 1st music");
			xPlayer.get_song_name(0, name);
			sprintf(path, "%s/%s", "file://music", name);
			xPlayer.set_source(xPlayer.Player, path);
			xPlayer.play(xPlayer.Player);
			xPlayer.current_music_idx = 0;
		}
	}
}
void xPlayerPause(void)
{
	LOG("xPlayerPause..");
	if (xPlayer.Player) {
		if (xPlayer.status(xPlayer.Player) == STATUS_PLAYING) {
			xPlayer.pause(xPlayer.Player);
		}
	}
}

void xPlayerResume(void)
{
	LOG("xPlayerResume..");
	if (xPlayer.Player) {
		if (xPlayer.status(xPlayer.Player) == STATUS_PAUSED) {
			xPlayer.play(xPlayer.Player);
		}
	}
}

void xPlayerStop(void)
{
	if (xPlayer.Player) {
		int state=xPlayer.status(xPlayer.Player);
		LOG("xPlayerStop cur state=%d",state);
		if (state != STATUS_STOPPED) {
			xPlayer.stop(xPlayer.Player);
		}
	}
}

void xPlayerPre(void)
{
	LOG("xPlayerPre, playing...");
	if (xPlayer.Player) {
		char name[64] = {0};
		char path[64] = {0};
		if (xPlayer.current_music_idx)
			xPlayer.current_music_idx --;
		xPlayer.get_song_name(xPlayer.current_music_idx, name);
		sprintf(path, "%s/%s", "file://music", name);
		xPlayerPlay(path);
	}
}

void xPlayerNext(void)
{
	LOG("xPlayerNext, playing...");
	if (xPlayer.Player) {
		char name[64] = {0};
		char path[64] = {0};
		int ret = -1;
		xPlayer.current_music_idx++;
		ret = xPlayer.get_song_name(xPlayer.current_music_idx, name);
		if (ret == 0) {
		} else {
			xPlayer.current_music_idx = 0;
			xPlayer.get_song_name(xPlayer.current_music_idx, name);
		}
		
		sprintf(path, "%s/%s", "file://music", name);
		xPlayerPlay(path);
	}
}


PlayerStatus xPlayerStatus(void)
{
	if (xPlayer.Player) {
		return xPlayer.status(xPlayer.Player);
	}
}

int xPlayerShowMusicList(void)
{
	int ret = -1;
	if (xPlayer.Player) {
		ret = xPlayer.create_music_list();
		if (ret) {
			LOG("Create musil list failed");
			return -1;
		}
		xPlayer.show_music_list();
	}
	return 0;
}

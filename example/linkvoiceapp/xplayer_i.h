/*
 * Copyright (C) 2017 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _XPLAYER_I_H
#define _XPLAYER_I_H

#include "audio/manager/audio_manager.h"

#define MMC_DISK_TAG  "0:/"
#define MUSIC_DIR "0:/music"
#define MUSIC_FILE "music_list.txt"

typedef struct DemoPlayerContext
{
    XPlayer*        mAwPlayer;
    int             mPreStatus;
    int             mStatus;
    int             mSeekable;
    int             mError;
    pthread_mutex_t mMutex;
    sem_t           mStoped;
    sem_t           mPrepared;
} PlayerContext;

typedef enum {
	STATUS_STOPPED   = 0,
	STATUS_PREPARING = 1,
	STATUS_PREPARED  = 2,
	STATUS_PLAYING   = 3,
	STATUS_PAUSED    = 4,
	STATUS_SEEKING   = 5,
	STATUS_PLAYEND   = 6,
} PlayerStatus;

typedef struct {
	PlayerContext *Player;
	kbuf_queue_t   *bufque;
	int  current_music_idx;
	int  (*create)(void);
	void (*destroy)(PlayerContext* player);
	void (*set_source)(PlayerContext *player, char* pUrl);
	void (*play)(PlayerContext *player);
	void (*pause)(PlayerContext *player);
	void (*stop)(PlayerContext *player);
	void (*record)(PlayerContext *player);
	PlayerStatus (*status)(PlayerContext *player);
	int  (*create_music_list)(void);
	int  (*get_song_name)(uint32_t idx, char* name);
	int  (*show_music_list)(void);
} xplayer_t;

xplayer_t* xPlayer_create(void);
void xPlayerDestroy(void);
void xPlayerPlay(char* pUrl);
void xPlayerPause(void);
void xPlayerStop(void);
void xPlayerPre(void);
void xPlayerNext(void);
PlayerStatus xPlayerStatus(void);
int xPlayerShowMusicList(void);

int xPlayer_add_to_queue(const char * url);

extern int volume ;
#endif



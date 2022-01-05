/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __ALSA_WAV_PLAYER_H__
#define __ALSA_WAV_PLAYER_H__

typedef int (*alsa_wav_player_file_open)(const char *pathname, int flags);
typedef int (*alsa_wav_player_file_close)(int fd);
typedef int (*alsa_wav_player_file_read)(int fd, void *buf, uint32_t count);

typedef struct {
    alsa_wav_player_file_open open;
    alsa_wav_player_file_close close;
    alsa_wav_player_file_read read;
} alsa_wav_player_file_opt_t;

void alsa_wav_player_file_opt_register(alsa_wav_player_file_opt_t * opt);
int alsa_wav_play_file(const char * path, uint8_t ultra_sound_flag);
int alsa_wav_play_mem(uint8_t *data, uint32_t len);

void alsa_ultra_sound_test(uint8_t en, const char * path_of_20k, const char * path_of_music);

#endif /* __ALSA_WAV_PLAYER_H__ */
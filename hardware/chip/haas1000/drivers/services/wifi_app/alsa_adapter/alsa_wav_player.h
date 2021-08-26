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
#ifndef __GENIE_TEST_H__
#define __GENIE_TEST_H__

#include "tg_audio.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    int channels;
    enum tg_pcm_format format;
    int samplerate;
    int duration_s;
    char *server_ip;
    int port;
    char *path_file;
    char *username;
    char *password;
    int is_tick;
}record_dump_t;

int AG_audio_decoder_test(void);
int AG_audio_decoder_mp3_play_mem(uint8_t * buf, uint32_t size);
int AG_audio_decoder_mp3_play_file(const char * file_path);
int AG_audio_record_test(void);
void agrecord_to_ftp(record_dump_t *record_dump);
void agrecord_to_data(record_dump_t *record_dump);

#ifdef __cplusplus
}
#endif

#endif

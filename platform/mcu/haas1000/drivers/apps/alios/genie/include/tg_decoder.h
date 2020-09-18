#ifndef __TG_DECODER_H__
#define __TG_DECODER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _tg_decoder_ctr tg_decoder_ctr_t;

typedef enum {
    TG_DECODER_FORMAT_INVALID= -1,
    TG_DECODER_FORMAT_MP3 = 0,
    TG_DECODER_FORMAT_AAC,
    TG_DECODER_FORMAT_OPUS
} tg_decoder_format_t;

typedef struct
{
    tg_decoder_format_t  format;
    tg_decoder_ctr_t    *ctr;
    void *audio_dec_handle;
} tg_decoder_handle_t;

tg_decoder_handle_t * tg_decoder_open(tg_decoder_format_t format);
int tg_decode(tg_decoder_handle_t * decoder, unsigned char * data_in, int input_size, unsigned char * data_out, int * output_size, int * output_sample, int * sample_rate);
void tg_decoder_close(tg_decoder_handle_t * decoder);

typedef void (*decoder_audiodump_cb_t)(uint8_t * data, uint32_t len, void * param);
void before_decoder_audiodump_cb_register(decoder_audiodump_cb_t cbf, void *param);
void before_decoder_audiodump_cb_unregister(void);
void after_decoder_audiodump_cb_register(decoder_audiodump_cb_t cbf, void *param);
void after_decoder_audiodump_cb_unregister(void);

#ifdef __cplusplus
}
#endif

#endif /* __TG_DECODER_H__ */
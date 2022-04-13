/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __CP_SERVER_H__
#define __CP_SERVER_H__

#if defined(CHIP_HAS_CP)

#ifdef __cplusplus
extern "C" {
#endif

#define CP_BUF_LEN 0x2000

typedef void (*CP_TASK_T)(void *param);

typedef enum {
    CP_TASK_ID_INVALID,
    CP_TASK_ID_MP3,
    CP_TASK_ID_AAC_ADTS,
    CP_TASK_ID_OPUS_DEC,
    CP_TASK_ID_OPUS_ENC,
    CP_TASK_ID_EQ_DRC,
#ifdef VOIP_ALG_IN_CP
    CP_TASK_ID_VOIP_ALG,
#endif
    CP_TASK_ID_RESAMP,
    CP_TASK_ID_NUM,
}CP_TASK_ID_T;

#ifdef MP3_DECODE_IN_CP
typedef struct {
    uint32_t decoder_hander_addr;
    uint32_t mp3_bytes;
    uint32_t out_pcm_addr;
    uint32_t out_mp3_info_addr;
    uint32_t in_buf_addr;
    int32_t result;
    uint8_t buf_location;
}CP_MSG_MP3_DEC_T;
#endif

#ifdef AAC_DECODE_IN_CP
typedef struct {
    uint32_t decoder_hander_addr;
    uint32_t in_buf_addr;
    uint32_t in_buf_size_addr;
    uint32_t in_buf_valid_addr;
    uint32_t out_buf_addr;
    int32_t out_buf_size;
    int32_t flags;
    int32_t result;
    uint8_t buf_location;
}CP_MSG_AAC_DEC_T;
#endif

#ifdef OPUS_DECODE_IN_CP
typedef struct {
    uint32_t decoder_hander_addr;
    uint32_t in_buf_addr;
    uint32_t in_buf_size;
    uint32_t out_buf_addr;
    uint32_t out_buf_size;
    int32_t decode_fec;
    int32_t result;
    uint8_t buf_location;
}CP_MSG_OPUS_DEC_T;
#endif

#ifdef OPUS_ENCODE_IN_CP
typedef struct {
    uint32_t handler_addr;
    uint32_t in_buf_addr;
    uint32_t in_buf_size;
    uint32_t buf_size;
    uint32_t out_buf_addr;
    int32_t max_data_bytes;
    int32_t result;
    uint8_t buf_location;
}CP_MSG_OPUS_ENC_T;
#endif

#ifdef VOIP_ALG_IN_CP
typedef struct {
    uint32_t decoder_hander_addr;
    uint32_t near_buf_addr;
    uint32_t far_buf_addr;
    uint32_t out_buf_addr;
    uint32_t buf_size;
    int32_t result;
    uint8_t buf_location;
}CP_MSG_VOIP_ALG_T;
#endif

#ifdef EQ_DRC_IN_CP
typedef struct {
    uint32_t buf_addr;
    uint32_t buf_len;
    int32_t result;
}CP_MSG_EQ_DRC_T;
#endif

#ifdef ALSA_RESAMPLE_IN_CP
typedef struct {
    uint32_t handler_addr;
    uint32_t io_addr;
    uint32_t in_size;
    uint32_t out_size;
    int32_t result;
    uint8_t buf_location;
}CP_MSG_RESAMP_T;
#endif

typedef struct {
    CP_TASK_ID_T id;
    uint32_t buf[CP_BUF_LEN/4];
}CP_MSG_T;


int cp_server_init();
void cp_server_register_task(CP_TASK_ID_T id, CP_TASK_T task_handler);
int cp_server_send_msg_to_cp(CP_TASK_ID_T id, void *msg);

void *cp_heap_malloc(uint32_t size);
void *cp_heap_calloc(uint32_t count, uint32_t size);
void cp_heap_free(void *p);
void *cp_heap_realloc(void *p, uint32_t size);
void cp_heap_memory_info(uint32_t *total, uint32_t *used, uint32_t *max_used);

#ifdef __cplusplus
}
#endif

#endif
#endif

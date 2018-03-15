// 测试厂商平台上移植的opus，例子里面调用opus接口进行opus编码；
// 需要关注编码的耗时来评估CPU是否满足opus编码的实时性
/*
 ------ Encode buffer 640 bytes ------
 ------ encode ret 67 ------
 ------ encode success ------
 ------ time consumed 301 microsecond ------ //编码640字节耗时301 microsecond
 */
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opus.h"
#include "opus_defines.h"
#include "ff.h"
#include <unistd.h>
#include <sys/time.h>

#define FRAME_SAMPLES (320)
#define MAX_DATA_BYTES (320 * sizeof(short))
//#define PCM_SAMPLE_FILE "sample.pcm"

#define PCM_SAMPLE_FILE  "0:/sample.wav"
static OpusEncoder* mp_opuEncoder = NULL;
static int m_frameSamples = FRAME_SAMPLES;
static int m_maxDataBytes = MAX_DATA_BYTES;
static int m_vbr = 1;
static int m_bitRate = 27800;
static int m_complexity = 8;
static int m_signal = OPUS_SIGNAL_VOICE;
static int m_sample_rate = 16000;
static int m_channels = 1;

static int initialized = 0;

int opus_codec_init(int sample_rate, int channels, int VBR, int bit_rate,
                    int complexity, int signal) {
    printf("sample_rate %d, channels %d, vbr %d, bit_rate %d, complexity %d, signal %d\n", sample_rate, channels, VBR, bit_rate, complexity, signal);
    if (initialized) {
        printf("Opus codec initialized\n");
        return 0;
    }
    int error = 0;
    m_sample_rate = sample_rate;
    m_channels = channels;
    
    mp_opuEncoder = opus_encoder_create(m_sample_rate, m_channels,
                                        OPUS_APPLICATION_VOIP, &error);
    if (!mp_opuEncoder) {
        //throw exception FIXME
        printf("opus_codec_init create opuEncoder fail!\n");
        error = -1;
    } else {
        printf("opus_codec_init create opuEncoder success!\n");
        //should we configure these seperately ? FIXME
        
        m_vbr = VBR;
        m_bitRate = bit_rate;
        m_complexity = complexity;
        m_signal = signal;
        int ret = OPUS_OK;
        ret += opus_encoder_ctl(mp_opuEncoder, OPUS_SET_VBR(m_vbr));
        ret += opus_encoder_ctl(mp_opuEncoder, OPUS_SET_BITRATE(m_bitRate));
        ret += opus_encoder_ctl(mp_opuEncoder,
                                OPUS_SET_COMPLEXITY(m_complexity));
        ret += opus_encoder_ctl(mp_opuEncoder, OPUS_SET_SIGNAL(m_signal));
        if (OPUS_OK == ret) {
            printf("set param success!\n");
            initialized = 1;
        } else {
            error = -1;
            initialized = 0;
            opus_encoder_destroy(mp_opuEncoder);
            mp_opuEncoder = NULL;
            printf("set param failed. destroy mp_opuEncoder\n");
        }
    }
    return error;
}

int opus_codec_destroy() {
    printf("opus_codec_destroy++\n");
    if (!initialized) {
        printf("Opus codec is not initialized\n");
        return 0;
    }
    if (mp_opuEncoder) {
        opus_encoder_destroy(mp_opuEncoder);
    }
    mp_opuEncoder = NULL;
    initialized = 0;
    printf("opus destroy--\n");
    return 0;
}

static int opus_codec_encode(const char *sample_data, int sample_data_len, unsigned char *buffer,
                      int buffer_capacity, int *buffer_len) {
    printf("------ Encode buffer %d bytes ------\n", sample_data_len);
   // struct timeval begin;
    uint64_t begin = aos_now_ms();
   // gettimeofday(&begin, NULL);
    if (!initialized) {
        return -1;
    }
    if (!sample_data || sample_data_len <= 0 || !buffer || buffer_capacity <= 0) {
        return -1;
    }
    short *samples = (short *)sample_data;
    if (sample_data_len != (FRAME_SAMPLES * sizeof(short))) {
        printf("sample len should be %d, cannot be %d\n", m_frameSamples * sizeof(short), sample_data_len);
        return -1;
    }
    if (buffer_capacity < sample_data_len) {
        return -1;
    }
    if (!initialized) {
        printf("opus Codec not initialized properly");
        return -1;
    }
    *buffer_len = 0;
    //if encoder is NULL FIXME
    if (NULL != mp_opuEncoder) {
        //length is frame_size*channels*sizeof(opus_int16)
            printf("------  opus_encode %d bytes ------\n", sample_data_len);
        int ret = opus_encode(mp_opuEncoder, samples, m_frameSamples, buffer,
                              m_maxDataBytes);
        printf("------ encode ret %d ------\n", ret);
        if (ret < 0) {
            *buffer_len = 0;
            printf("encode err, code = %d\n", ret);
            return -1;
        }
        *buffer_len = ret;
    } else {
        printf("mp_opuEncoder is NULL. should not happen.\n");
    }
    printf("------ encode success ------\n");
    // struct timeval end;
    // gettimeofday(&end, NULL);
    uint64_t after = aos_now_ms();
    int elapse = after-begin;//((end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec));
    printf("------ time consumed %d microsecond ------\n", elapse);
    return 0;
}

    int buffer_capacity = 1024;
    char buffer[1024] = {0};
int opus_test_main()
{
   // aos_msleep(3000);
    int ret = opus_codec_init(m_sample_rate, m_channels, m_vbr, m_bitRate,
                              m_complexity, m_signal);
    printf("------ init ret: %d ------\n", ret);
    
    FIL fp;
    //FILE *fp = NULL;
    int fs_ret = f_open(&fp, PCM_SAMPLE_FILE,FA_READ);
    //fp = fopen(PCM_SAMPLE_FILE, "rb");
    if (fs_ret!=FR_OK) {
        printf("No file opened\n");
        return 0;
    }

    int read_size=12800;
    // f_lseek(&fp,  SEEK_END);
    // file_size = 12800;//f_tell(&fp);
    // f_lseek(&fp,  SEEK_SET);
    int raw_len = 320; 
   // int raw_len = 320;
  // LOG("====read_size=%d===",read_size);
    //short *raw = aos_malloc(raw_len * sizeof(short));
    short *raw = aos_malloc(read_size);
    int read;
    int sum = 0;

    int buffer_len = 0;
    while ((f_read(&fp,raw,read_size,&read)) == FR_OK) {
        
    //while ((read = fread(raw, sizeof(short), raw_len, fp)) != 0) {
         printf("------ f read len= %d ------\n", read);
        sum += read;
        int num=read_size/(raw_len*2);
        for(int i=0;i<num;i++){
            int enc_ret = opus_codec_encode(raw+i*raw_len * sizeof(short), raw_len * sizeof(short), buffer, buffer_capacity, &buffer_len);
            printf("------ enc_ret %d buffer_len=%d------\n", enc_ret,buffer_len);
            buffer_len=0;
           // aos_msleep(10);
        }
        if(read!=read_size){
            break;
        }
    }

    aos_free(raw);
    f_close(&fp);
    ret = opus_codec_destroy();
    printf("------ destroy ret: %d ------\n", ret);
    return 0;
}


// static void douglas_asr_test(int format)
// {
    
//     FRESULT fs_ret;
//     for (int i = 0; i < 3; i++) {
//         int ret = pal_asr_start();
//         FIL fp;
//         //FILE *fp = NULL;
//         if (format == PAL_AUDIO_FORMAT_OPUS) {
//             fs_ret = f_open(&fp, OPUS_SAMPLE_FILE,FA_READ);
//         } else if (format == PAL_AUDIO_FORMAT_PCM){
//             fs_ret = f_open(&fp, PCM_SAMPLE_FILE,FA_READ);
//         } else {
//             LOGE(TAG,"========unsupport format======\n");
//         }
//         if (fs_ret!=FR_OK) {
//             LOGE(TAG,"===========No file opened==fs_ret=%d===\n",fs_ret);
//             return;
//         }
//         int file_size;
//         f_lseek(&fp,  SEEK_END);
//         file_size = f_tell(&fp);
//         f_lseek(&fp,  SEEK_SET);
//         int raw_len = 640; 
// 		printf("====================================================== begin\n");
//         unsigned char *raw = aos_malloc(raw_len * sizeof(unsigned char));
//         int read;
//         int sum = 0;
// 		int count = 0;
//          //f_read(&fp, &c, 1, &len)
//         while ((f_read(&fp,raw, raw_len,&read)) == FR_OK) {
//             sum += read;
//             aos_msleep(100);
		
//             int ret = pal_asr_send_buffer((char *)raw, read * sizeof(unsigned char));
// 			if (ret == PAL_VAD_STATUS_STOP) {
//                 printf("Detect VAD STOP\n");
//                 break;
//             }
//         }
// 		printf("===================================================== end sum: %d\n", sum);
//         aos_free(raw);
//         f_close(&fp);
//         struct pal_rec_result *result = pal_asr_stop();
//         if (result) {
//              log_debug("~~~~~ttsurl=%s",result->tts);
//              xPlayerPlay(result->tts); 
//             // cJSON *params = cJSON_Parse(result->raw);
//             //  cJSON *url = get_service_data(params, 2, "ttsUrl", "tts_url");
//             //  if(!url){
//             //      log_debug("~~~~~url=%s",url->valuestring);
//             //      xPlayerPlay(url->valuestring);
//             //  }
//             pal_rec_result_destroy(result);
//         }
// 		aos_msleep(10*1000);
//     }
// }


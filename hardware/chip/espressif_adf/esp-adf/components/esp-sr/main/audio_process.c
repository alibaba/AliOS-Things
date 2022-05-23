#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "audio_process.h"
#include "esp_ns.h"
#include "esp_aec.h"
#include "esp_agc.h"
#include "esp_vad.h"
#include "esp_mase.h"
#include "audio_test_file.h"

void NSTask(void *arg)
{
    ns_handle_t ns_inst = ns_create(NS_FRAME_LENGTH_MS);
    int chunks = 0;
    int audio_chunksize = NS_FRAME_LENGTH_MS * 16;
    int16_t *ns_in = malloc(audio_chunksize * sizeof(int16_t));
    int16_t *ns_out = malloc(audio_chunksize * sizeof(int16_t));
    while (1) {
        if ((chunks + 1) * audio_chunksize * sizeof(int16_t) <= sizeof(audio_test_file)) {
            memcpy(ns_in, audio_test_file + chunks * audio_chunksize * sizeof(int16_t), audio_chunksize * sizeof(int16_t));
        } else {
            break;
        }
        ns_process(ns_inst, ns_in, ns_out);
        chunks++;
    }
    ns_destroy(ns_inst);
    free(ns_in);
    free(ns_out);
    printf("NS test successfully\n\n");
    vTaskDelete(NULL);    
}

#define AGC_FRAME_BYTES     320
void AGCTask(void *arg)
{
    void *agc_handle = esp_agc_open(3, 16000);
    set_agc_config(agc_handle, 15, 1, -3);
    int chunks = 0;
    int16_t *agc_in  = malloc(AGC_FRAME_BYTES);
    int16_t *agc_out = malloc(AGC_FRAME_BYTES);
    while (1) {
        if ((chunks + 1) * AGC_FRAME_BYTES <= sizeof(audio_test_file)) {
            memcpy(agc_in, audio_test_file + chunks * AGC_FRAME_BYTES, AGC_FRAME_BYTES);
        } else {
            break;
        }
        esp_agc_process(agc_handle, agc_in, agc_out, AGC_FRAME_BYTES / 2, 16000);
        chunks++;
    }
    esp_agc_clse(agc_handle);
    free(agc_in);
    free(agc_out);
    printf("AGC test successfully\n\n");
    vTaskDelete(NULL);    
}

void AECTask(void *arg)
{
    aec_handle_t aec_inst = aec_create(AEC_SAMPLE_RATE, AEC_FRAME_LENGTH_MS, AEC_FILTER_LENGTH);
    int chunks = 0;
    int audio_chunksize = AEC_FRAME_LENGTH_MS * 16;
    int16_t *aec_in = malloc(audio_chunksize * sizeof(int16_t));
    int16_t *aec_ref = malloc(audio_chunksize * sizeof(int16_t));
    int16_t *aec_out = malloc(audio_chunksize * sizeof(int16_t));
    while (1) {
        if ((chunks + 1) * audio_chunksize * sizeof(int16_t) <= sizeof(audio_test_file)) {
            memcpy(aec_in, audio_test_file + chunks * audio_chunksize * sizeof(int16_t), audio_chunksize * sizeof(int16_t));
            memset(aec_ref, 0, audio_chunksize * sizeof(int16_t));
        } else {
            break;
        }
        aec_process(aec_inst, aec_in, aec_ref, aec_out);
        chunks++;
    }
    aec_destroy(aec_inst);
    free(aec_in);
    free(aec_ref);
    free(aec_out);
    printf("AEC test successfully\n\n");
    vTaskDelete(NULL);    
}

void VADTask(void *arg)
{
    vad_handle_t vad_inst = vad_create(VAD_MODE_4, SAMPLE_RATE_HZ, VAD_FRAME_LENGTH_MS);
    int chunks = 0;
    int audio_chunksize = VAD_FRAME_LENGTH_MS * 16;
    int16_t *vad_in = malloc(audio_chunksize * sizeof(int16_t));
    while (1) {
        if ((chunks + 1) * audio_chunksize * sizeof(int16_t) <= sizeof(audio_test_file)) {
            memcpy(vad_in, audio_test_file + chunks * audio_chunksize * sizeof(int16_t), audio_chunksize * sizeof(int16_t));
        } else {
            break;
        }
        vad_state_t vad_state = vad_process(vad_inst, vad_in);
        chunks++;
    }
    vad_destroy(vad_inst);
    free(vad_in);
    printf("VAD test successfully\n\n");
    vTaskDelete(NULL);    
}

void MASETask(void *arg)
{
    mase_handle_t mase_st = mase_create(MASE_SAMPLE_RATE, MASE_FRAME_SIZE, THREE_MIC_CIRCLE, MASE_MIC_DISTANCE, WAKE_UP_ENHANCEMENT_MODE, 6);
    int chunks = 0;
    int nch = 3;
    int audio_chunksize = MASE_FRAME_SIZE * MASE_SAMPLE_RATE * nch / 1000;
    int16_t *mase_in = malloc(audio_chunksize * sizeof(int16_t));
    int16_t *mase_out = malloc(audio_chunksize / nch * sizeof(int16_t));
    while (1) {
        if ((chunks + 1) * audio_chunksize * sizeof(int16_t) <= sizeof(audio_test_file)) {
            memcpy(mase_in, audio_test_file + chunks * audio_chunksize * sizeof(int16_t), audio_chunksize * sizeof(int16_t));
            memset(mase_out, 0, audio_chunksize / nch * sizeof(int16_t));
        } else {
            break;
        }
        mase_process(mase_st, mase_in, mase_out);
        chunks++;
    }
    mase_destory(mase_st);
    free(mase_in);
    free(mase_out);
    printf("MASE test successfully\n\n");
    printf("TEST3 FINISHED\n\n");
    vTaskDelete(NULL);
}

void audio_process_test()
{
    xTaskCreatePinnedToCore(&NSTask, "noise_suppression", 3 * 1024, NULL, 5, NULL, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(&AGCTask, "automatic_gain_control", 3 * 1024, NULL, 5, NULL, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(&AECTask, "acoustic_echo_cancellation", 3 * 1024, NULL, 5, NULL, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(&VADTask, "voice_activity_detection", 3 * 1024, NULL, 5, NULL, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(&MASETask, "mic_array_speech_enhancement", 3 * 1024, NULL, 5, NULL, 1);
}

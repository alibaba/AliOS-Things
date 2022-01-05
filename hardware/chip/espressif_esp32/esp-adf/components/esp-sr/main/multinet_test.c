#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_mn_iface.h"
#include "esp_mn_models.h"
#include "dl_lib_coefgetter_if.h"
#include "multinet_test.h"
#include "dakaidiandeng.h"

static const esp_mn_iface_t *multinet = &MULTINET_MODEL;

void multinetTask(void *arg)
{
    model_iface_data_t *model_data = arg;
    int audio_chunksize = multinet->get_samp_chunksize(model_data);
    int chunk_num = multinet->get_samp_chunknum(model_data);
    int16_t *buffer = malloc(audio_chunksize * sizeof(int16_t));
    assert(buffer);
    int chunks = 0;
    while (1) {
        if ((chunks+1)*audio_chunksize*sizeof(int16_t) <= sizeof(dakaidiandeng)) {
            memcpy(buffer, dakaidiandeng+chunks*audio_chunksize*sizeof(int16_t), audio_chunksize * sizeof(int16_t));   
        } else {
            memset(buffer, 0, audio_chunksize*sizeof(int16_t));
        }
        int command_id = multinet->detect(model_data, buffer);
        chunks++;
        if (chunks == chunk_num || command_id > -1) {
            if (command_id > -1) {
                printf("MN test successfully, Commands ID: %d.\n", command_id);
                break;
            } else {
                printf("can not recognize any speech commands\n");
            }
            break;
        }
    }
    printf("TEST2 FINISHED\n\n");
    multinet->destroy(model_data);
    vTaskDelete(NULL);
}

void multinet_test()
{
    int start_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    printf("Start free RAM size: %d\n", start_size);

    //Initialize multinet model
    model_iface_data_t *model_data = multinet->create(&MULTINET_COEFF, 6000);

    //define_speech_commands(multinet, model_data);
    printf("multinet RAM size: %d\nRAM size after multinet init: %d\n",
           start_size - heap_caps_get_free_size(MALLOC_CAP_8BIT), heap_caps_get_free_size(MALLOC_CAP_8BIT));

    xTaskCreatePinnedToCore(&multinetTask, "multinet", 2 * 1024, (void*)model_data, 5, NULL, 1);
}

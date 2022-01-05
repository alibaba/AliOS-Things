#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "xtensa/core-macros.h"

#include "wakenet_test.h"
#include "multinet_test.h"
#include "audio_process.h"

void app_main()
{
    // test wakenet
    wakenet_test();
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // test multinet
    multinet_test();
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    // test acoustic algorithm
    audio_process_test();
}

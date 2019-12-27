/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifdef AOS_COMP_CLI

#include "cli.h"
#include "aos/kernel.h"

#include "esp_ota_ops.h"
#include "esp_log.h"
#include <string.h>
#include <malloc.h>
#include "esp_spi_flash.h"
#include "rom/spi_flash.h"


static char TAG[]="esp start_addr";

extern void esp_restart();
static  int get_cur_part()
{
    const esp_partition_t *esp_current_partition = esp_ota_get_boot_partition();
    int ret=0;
    if (esp_current_partition == NULL) {
        ESP_LOGE(TAG, "esp_ota_get_boot_partition got null prt");
        return 0;
    }

    if (esp_current_partition->type != ESP_PARTITION_TYPE_APP) {
        ESP_LOGE(TAG, "Error esp_current_partition->type != ESP_PARTITION_TYPE_APP");
        return 0;
    }
    switch(esp_current_partition->subtype){
        case ESP_PARTITION_SUBTYPE_APP_FACTORY:
        return 0;
        case ESP_PARTITION_SUBTYPE_APP_OTA_0:
        return 1;
        case ESP_PARTITION_SUBTYPE_APP_OTA_1:
        return 2;
    }
    return -1;

}

static int set_boot(int which)
{
    int err;
    esp_partition_t find_partition;
    switch(which) {
        case 0:
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_FACTORY;
        break;
        case 1:
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_0;
        break;
        case 2:
            find_partition.subtype = ESP_PARTITION_SUBTYPE_APP_OTA_1;
        break;
    }
    find_partition.type = ESP_PARTITION_TYPE_APP;

    const esp_partition_t *partition = esp_partition_find_first(find_partition.type, find_partition.subtype, NULL);
    assert(partition != NULL);
    err = esp_ota_set_boot_partition(partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed! err=0x%x", err);
        return -1;
    }

    ESP_LOGI(TAG, "Prepare to restart system!");
    esp_restart();
    return 0;
}

 void handle_boot_part_cmd(char *pwbuf, int blen, int argc, char **argv)
{
     if (argc == 1) {
        aos_cli_printf("boot_from app partition %d \r\n",get_cur_part());
        return;
    }
    if (argc == 2) {
        if (!strcmp(argv[1], "--help")) {
            aos_cli_printf("use \"boot_from\" to get boot partition;\r\n use \"boot_from -s which\" to set boot partition,which=0,1,2 \r\n");
            return;
        }
    }
    else if (argc == 3) {
        if (!strcmp(argv[1], "-s")) {
            if (!strcmp(argv[2], "0")) {
                set_boot(0);
            } 
            else if (!strcmp(argv[2], "1")) {
                set_boot(1);
            } 
            else if (!strcmp(argv[2], "2")) {
                set_boot(2);
            } 
        } 
    }
}

 struct cli_command boot_from_cmd = {
    .name = "boot_from",
    .help = "boot_from",
    .function = handle_boot_part_cmd
};

int board_cli_init(void)
{
    return aos_cli_register_command(&boot_from_cmd);
}

#endif


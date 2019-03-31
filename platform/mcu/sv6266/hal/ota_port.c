#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> 
#include <aos/log.h>
#include <errno.h>
#include <hal/ota.h>
#include <hal/soc/soc.h>
#include "sys/flash.h"
#include "osal.h"
#include <CheckSumUtils.h>
#include "wdt/drv_wdt.h"
#include "common.h"

static int sv6266_ota_set_boot(hal_ota_module_t *m, void *something)
{
    ota_finish_param_t *param = (ota_finish_param_t *)something;

    if (param == NULL) {
        return kGeneralErr;
    }
    if (param->result_type == OTA_FINISH) {
        //CRC16_Final( &contex, (uint16_t *)&ota_info.ota_crc );
        printf("switch to new fw\n");
        memset(&ota_info, 0 , sizeof ota_info);
        if (hal_ota_switch_to_new_fw() != 0) {
            return kGeneralErr;
        }
    }
    return kNoErr;
}


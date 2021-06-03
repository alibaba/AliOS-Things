#ifndef __ANT_BAT_H__
#define __ANT_BAT_H__

#include "alipay_iot_api.h"
#include "ant_os.h"
#include "MQTTClient.h"

typedef struct
{
   ant_s32 vol;
   ant_s32 percent;

}ant_bat_capacity_t;


#define  BAT_READ_TIMES_ONCE 5

typedef struct{
   ant_bat_capacity_t  * caps;
   ant_s32 size;
   ant_s32 last_state;
}ant_bat_env_t;

ant_s32 ant_bat_cfg_init(ant_bat_capacity_t * caps,ant_s32 size);
ant_s32 ant_bat_caps_get(ant_s32 * percent, ant_s32 * vol);
ant_s32 ant_iot_pm_init();

#endif


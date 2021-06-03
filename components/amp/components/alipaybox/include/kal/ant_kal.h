//#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ant_typedef.h"
#include "ant_os.h"
#include "ant_event.h"
#include "ant_log.h"
#include "ant_mem.h"
#include "ant_pm.h"
#include "ant_soc.h"
#include "ant_system.h"
#include "ant_fs.h"
#include "ant_task.h"
#include "ant_time.h"
#include "ant_timer.h"
#include "ant_aos/kv.h"
#include "ant_gpio.h"
#include "ant_gprs.h"
#include "ant_flash.h"
#include "ant_audio.h"
#include "ant_error.h"
#include "ant_led.h"
#include "ant_task.h"
#include "ant_task_config.h"
#include "ant_upgrade.h"

#ifdef __SDK_DEBUG_ENABLE__

//内存泄漏压测及定位宏,只有在debug版本才允许被被打开
#define __MEM_LEAK_CHECK_ENABLE__

#endif


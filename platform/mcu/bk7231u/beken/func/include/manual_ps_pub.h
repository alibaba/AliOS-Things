#include "rtos_pub.h"
#include "uart_pub.h"
#include "arm_arch.h"
#include "param_config.h"
#include "rw_pub.h"

#define PS_SUPPORT_MANUAL_SLEEP 0
typedef void (*ps_wakeup_cb)(void);


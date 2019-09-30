#ifndef AUX_CONFIG_H
#define AUX_CONFIG_H

#include <k_default_config.h>

#ifndef RHINO_CONFIG_TASK_KSTACK_OFFSET
#define RHINO_CONFIG_TASK_KSTACK_OFFSET (0)
#endif

#ifndef RHINO_CONFIG_TASK_USTACK_OFFSET
#define RHINO_CONFIG_TASK_USTACK_OFFSET (RHINO_CONFIG_TASK_INFO_NUM * 4 + 8)
#endif

#ifndef RHINO_CONFIG_TASK_MODE_OFFSET
#define RHINO_CONFIG_TASK_MODE_OFFSET (RHINO_CONFIG_TASK_INFO_NUM * 4 + 20)
#endif

#ifndef RHINO_CONFIG_TASK_PID_OFFSET
#define RHINO_CONFIG_TASK_PID_OFFSET (RHINO_CONFIG_TASK_INFO_NUM * 4 + 16)
#endif

#endif // AUX_CONFIG_H


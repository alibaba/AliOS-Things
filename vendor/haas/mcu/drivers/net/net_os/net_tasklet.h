#ifndef __NET_TASKLET_H__
#define __NET_TASKLET_H__
#include "net_defs.h"
#include "cmsis_os.h"
#include "net_debug.h"

extern osMessageQId tasklet_msg_id;
typedef void (*tasklet_handler)(unsigned long data);

struct tasklet_struct{
    tasklet_handler handler;
    unsigned long   para;
    unsigned char   tasklet_pending;
};

#define tasklet_init(tasklet, func, data) {\
	(tasklet)->handler = func;\
	(tasklet)->para = data;\
    (tasklet)->tasklet_pending = N_FALSE;\
    printk(KERN_DEBUG, "tasklet_init: 0x%x %s %d \n", (unsigned int)(tasklet), __func__, __LINE__);\
}

static inline void tasklet_disable(struct tasklet_struct *t)
{

}

static inline void tasklet_enable(struct tasklet_struct *t)
{

}

static inline void tasklet_schedule(struct tasklet_struct *tasklet)
{
    osStatus status;

    if (!tasklet->tasklet_pending) {
        //printk(KERN_DEBUG, "tasklet_schedule: 0x%x %s %d\n", tasklet, __func__, __LINE__);
        tasklet->tasklet_pending = N_TRUE;
        status = osMessagePut(tasklet_msg_id, (uint32_t)tasklet, 0);
        if (status != osOK) {
            printk(KERN_WARN, "tasklet_schedule failed status:%x, %p %s %d \n",
                   status, tasklet, __func__, __LINE__);
        }
    }
}

#define tasklet_kill(tasklet)	{ do {} while (0); }

extern void net_tasklet_init(void);
#endif


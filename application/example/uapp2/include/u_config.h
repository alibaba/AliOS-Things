/**
 * config user space app name, it's also entry task name
 */
#ifndef RHINO_CONFIG_APP_NAME
#define RHINO_CONFIG_APP_NAME "uapp2"
#endif

/**
 * config user space app version
 */
#ifndef RHINO_CONFIG_APP_VERSION
#define RHINO_CONFIG_APP_VERSION 0x00000001
#endif

/**
 * config user space entry task priority
 */
#ifndef RHINO_CONFIG_APP_ENTRY_TASK_PRIO
#define RHINO_CONFIG_APP_ENTRY_TASK_PRIO 20
#endif

/**
 * config user space entry task's kstack size
 * the stack actual size in bytes =
 *   RHINO_CONFIG_APP_ENTRY_TASK_KSTACK_SIZE * sizeof (cpu_stack_t)
 * On default set to 1K bytes.
 */
#ifndef RHINO_CONFIG_APP_ENTRY_TASK_KSTACK_SIZE
#define RHINO_CONFIG_APP_ENTRY_TASK_KSTACK_SIZE 256
#endif

/**
 * config user space entry task's kstack size
 * the stack actual size in bytes =
 *   RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE * sizeof (cpu_stack_t)
 * On default set to 1K bytes.
 */
#ifndef RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE
#define RHINO_CONFIG_APP_ENTRY_TASK_USTACK_SIZE 256
#endif

#if (RHINO_CONFIG_URES > 0)
/**
 * enable user space resoure task
 */
#ifndef RHINO_CONFIG_URES_TASK
#define RHINO_CONFIG_URES_TASK 1
#endif /* RHINO_CONFIG_URES_TASK */

/**
 * config user space resource task priority
 */
#ifndef RHINO_CONFIG_URES_TASK_PRIO
#define RHINO_CONFIG_URES_TASK_PRIO 10
#endif

/**
 * config user space resource task's kstack size
 * the stack actual size in bytes =
 *   RHINO_CONFIG_URES_TASK_KSTACK_SIZE * sizeof (cpu_stack_t)
 */
#ifndef RHINO_CONFIG_URES_TASK_KSTACK_SIZE
#define  RHINO_CONFIG_URES_TASK_KSTACK_SIZE 256
#endif /* RHINO_CONFIG_URES_TASK_KSTACK_SIZE */

/**
 * config user space resource task's ustack size
 * the stack actual size in bytes =
 *   RHINO_CONFIG_URES_TASK_USTACK_SIZE * sizeof (cpu_stack_t)
 */
#ifndef RHINO_CONFIG_URES_TASK_USTACK_SIZE
#define  RHINO_CONFIG_URES_TASK_USTACK_SIZE 256
#endif /* RHINO_CONFIG_URES_TASK_USTACK_SIZE */

#endif /* RHINO_CONFIG_URES */


#if (RHINO_CONFIG_UCLI > 0)
/**
 * enable user space cli
 */
#ifndef RHINO_CONFIG_UCLI_TASK
#define RHINO_CONFIG_UCLI_TASK 1
#endif

/**
 * config user space cli task prioriy
 */
#ifndef RHINO_CONFIG_UCLI_TASK_PRIO
#define RHINO_CONFIG_UCLI_TASK_PRIO 25
#endif

/**
 * config user space cli task's kstack size
 */
#ifndef RHINO_CONFIG_UCLI_TASK_KSTACK_SIZE
#define RHINO_CONFIG_UCLI_TASK_KSTACK_SIZE 256
#endif

/**
 * config user space cli task's ustack size
 */
#ifndef RHINO_CONFIG_UCLI_TASK_USTACK_SIZE
#define RHINO_CONFIG_UCLI_TASK_USTACK_SIZE 256
#endif

#endif /* RHINO_CONFIG_UCLI */


#if (RHINO_CONFIG_UCALLBACK > 0)
/**
 * enable user space callback
 */
#ifndef RHINO_CONFIG_UCALLBACK_TASK
#define RHINO_CONFIG_UCALLBACK_TASK 0
#endif

/**
 * config user space callback task prio
 */
#ifndef RHINO_CONFIG_UCALLBACK_TASK_PRIO
#define RHINO_CONFIG_UCALLBACK_TASK_PRIO 11
#endif

/**
 * config user space callback task kstack size
 */
#ifndef RHINO_CONFIG_UCALLBACK_TASK_KSTACK_SIZE
#define RHINO_CONFIG_UCALLBACK_TASK_KSTACK_SIZE 256
#endif

/**
 * config user space callback task ustack size
 */
#ifndef RHINO_CONFIG_UCALLBACK_TASK_USTACK_SIZE
#define RHINO_CONFIG_UCALLBACK_TASK_USTACK_SIZE 256
#endif

#endif /* RHINO_CONFIG_UCALLBACK */


/**
 * enable user space timer
 */
#ifndef RHINO_CONFIG_UTIMER_TASK
#define RHINO_CONFIG_UTIMER_TASK 1
#endif

/**
 * config user space timer task priority
 */
#ifndef RHINO_CONFIG_UTIMER_TASK_PRIO
#define RHINO_CONFIG_UTIMER_TASK_PRIO 5
#endif

/**
 * config user space timer task's kstack size
 */
#ifndef RHINO_CONFIG_UTIMER_TASK_KSTACK_SIZE
#define RHINO_CONFIG_UTIMER_TASK_KSTACK_SIZE 256
#endif

/**
 * config user space timer task's ustack size
 */
#ifndef RHINO_CONFIG_UTIMER_TASK_USTACK_SIZE
#define RHINO_CONFIG_UTIMER_TASK_USTACK_SIZE 256
#endif

/**
 * config user space timer max message number
 */
#ifndef RHINO_CONFIG_UTIMER_MSG_NUM
#define RHINO_CONFIG_UTIMER_MSG_NUM 6
#endif


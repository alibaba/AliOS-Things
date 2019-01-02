#ifndef U_TASK_H
#define U_TASK_H

kstat_t krhino_utask_create(ktask_t *task,
                            const name_t *name,
                            void *arg,
                            uint8_t pri,
                            tick_t ticks,
                            cpu_stack_t *ustack_buf,
                            size_t ustack,
                            size_t kstack,
                            task_entry_t entry,
                            uint8_t autorun);

#endif // U_TASK_H


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CPU_EVENT_H
#define CPU_EVENT_H

typedef void (*cpu_event_handler)(const void *);

typedef struct {
    cpu_event_handler handler;
    const void       *arg;
} cpu_event_t;

#ifdef VCALL_RHINO
int cpu_notify_event(cpu_event_t *event);
void *cpu_event_malloc(int size);
void cpu_event_free(void *p);

static inline int cpu_call_handler(cpu_event_handler handler, const void *arg)
{
    cpu_event_t event = {
        .handler = handler,
        .arg = arg,
    };
    return cpu_notify_event(&event);
}
#else
#define cpu_event_malloc malloc
#define cpu_event_free free
static inline int cpu_call_handler(cpu_event_handler handler, const void *arg)
{
    handler(arg);
    return 0;
}
#endif

#endif /* CPU_EVENT_H */


//
//  runloop.h
//  pal_sdk
//

#ifndef runloop_h
#define runloop_h

#include "blocking_queue.h"
#include "tic.h"

typedef struct {
    int name;
    void *param;
    int type;
    tic_t timestamp;
    int delay_ms;
} runloop_event_t;

typedef void (*dispatcher_fn)(runloop_event_t *e, void *extra_params);
typedef void (*destroy_handler_fn)(void *entry);

typedef struct _runloop {
    void *thread;
    struct blocking_queue_t *q;
    int loop_running;
    int loop_exit;
    void *mutex;
    dispatcher_fn dispatcher;
    destroy_handler_fn destroy_handler;
} runloop_t;

int runloop_init(runloop_t *run_loop, dispatcher_fn dispatcher, void *dispatcher_param, destroy_handler_fn destroy_handler, const char *name, int stack_size);
void runloop_destroy(runloop_t *run_loop);

#endif /* runloop_h */

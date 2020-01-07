

#ifndef UMESH_LOOP_H_
#define UMESH_LOOP_H_

#include <stdint.h>

typedef void (*umesh_call_t)(void *arg);

int umesh_timer_start(void **timer, int ms,umesh_call_t cb, void *arg);
int umesh_timer_cancel(void **timer, umesh_call_t cb, void *arg);

#endif /* UMESH_LOOP_H_ */

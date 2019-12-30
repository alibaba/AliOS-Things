#ifndef UDATA_QUEUE
#define UDATA_QUEUE
#include "udata_com_desc.h"

int udata_post_msg(sensor_msg_pkg_t msg);
int udata_unregister_msg_handler(int index);
int udata_start(void);
int udata_new_servicetask(const char *name, void (*fn)(void *),void *arg,int stack_size, int prio);
int udata_msg_recv(int task_id, unsigned int ms, void *msg,unsigned int *size);
int udata_observe_servicetask_tag(int taskid,sensor_tag_e tag, uint8_t instance);

#endif

#ifndef UDATA_QUEUE
#define UDATA_QUEUE
#include "uData_com_desc.h"

int uData_post_msg(sensor_msg_pkg_t msg);
int uData_unregister_msg_handler(int index);
int uData_register_msg_handler(void *func);
int uData_start();
int uData_new_servicetask(const char *name, void (*fn)(void *),void *arg,int stack_size, int prio);
int aos_msg_recv(int task_id, unsigned int ms, void *msg,unsigned int *size);
int uData_observe_servicetask_tag(int taskid,sensor_tag_e tag);

#endif

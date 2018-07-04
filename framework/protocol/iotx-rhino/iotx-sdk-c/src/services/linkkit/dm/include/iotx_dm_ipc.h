#ifndef _IOTX_DM_IPC_H_
#define _IOTX_DM_IPC_H_

#include "lite-utils.h"
#include "lite-list.h"
#include "iot_export_dm.h"

typedef struct {
	iotx_dm_event_types_t type;
	char *data;
}iotx_dipc_msg_t;

typedef struct {
	void *data;
	struct list_head linked_list;
}iotx_dipc_msg_node_t;

typedef struct {
	int max_size;
	int size;
	struct list_head message_list;
}iotx_dipc_msg_list_t;

typedef struct {
	void *mutex;
	iotx_dipc_msg_list_t msg_list;
}iotx_dipc_t;

int iotx_dipc_init(int max_size);
void iotx_dipc_deinit(void);
int iotx_dipc_msg_insert(void *data);
int iotx_dipc_msg_next(void **data);

#endif

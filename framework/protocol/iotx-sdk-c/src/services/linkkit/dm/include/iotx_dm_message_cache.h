#ifndef _IOTX_DM_MESSAGE_CACHE_H_
#define _IOTX_DM_MESSAGE_CACHE_H_

#include "lite-list.h"
#include "iot_export_dm.h"

#define IOTX_DCS_MSG_TIMEOUT_MS_DEFAULT (10000)

typedef struct {
	int msgid;
	int devid;
	iotx_dm_event_types_t response_type;
	char *data;
	uint64_t ctime;
	struct list_head linked_list;
}iotx_dmc_node_t;

typedef struct {
	void *mutex;
	struct list_head dmc_list;
}iotx_dmc_ctx_t;

int iotx_dmc_init(void);
int iotx_dmc_deinit(void);
int iotx_dmc_msg_insert(int msg_id, int devid, iotx_dm_event_types_t type, char *data);
int iotx_dmc_msg_search(_IN_ int msg_id, _OU_ iotx_dmc_node_t **node);
int iotx_dmc_msg_remove(int msg_id);
void iotx_dmc_msg_tick(void);

#endif

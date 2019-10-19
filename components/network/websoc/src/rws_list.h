/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __RWS_LIST_H__
#define __RWS_LIST_H__ 1

#include <stdio.h>

typedef union _rws_node_value_union {
	void * object;
	char * string;
	int int_value;
	unsigned int uint_value;
} _rws_node_value;

typedef struct _rws_node_struct {
	_rws_node_value value;
	struct _rws_node_struct * next;
} _rws_node, _rws_list;


_rws_list * rws_list_create(void);

void rws_list_delete(_rws_list * list);

void rws_list_delete_clean(_rws_list ** list);

void rws_list_append(_rws_list * list, _rws_node_value value);

#endif


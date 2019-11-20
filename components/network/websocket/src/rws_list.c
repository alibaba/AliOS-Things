/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "rws_list.h"
#include "rws_memory.h"

_rws_list * rws_list_create(void) {
	return (_rws_list *)rws_malloc_zero(sizeof(_rws_list));
}

void rws_list_delete(_rws_list * list) {
	_rws_list * cur = list;
	while (cur) {
		_rws_list * prev = cur;
		cur = cur->next;
		rws_free(prev);
	}
}

void rws_list_delete_clean(_rws_list ** list) {
	if (list) {
		rws_list_delete(*list);
		*list = NULL;
	}
}

void rws_list_append(_rws_list * list, _rws_node_value value) {
	if (list) {
		_rws_list * cur = list;
		while (cur->next) {
			cur = cur->next;
		}
		cur->next = (_rws_node *)rws_malloc_zero(sizeof(_rws_node));
		cur->next->value = value;
	}
}


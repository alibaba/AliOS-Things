/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

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


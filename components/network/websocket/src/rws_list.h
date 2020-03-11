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


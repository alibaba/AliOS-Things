/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __RWS_FRAME_H__
#define __RWS_FRAME_H__ 1

#include "websoc/librws.h"
#include "rws_common.h"

typedef enum _rws_opcode {
	rws_opcode_continuation = 0x0, // %x0 denotes a continuation frame
	rws_opcode_text_frame = 0x1, // %x1 denotes a text frame
	rws_opcode_binary_frame = 0x2, // %x2 denotes a binary frame
	rws_opcode_connection_close = 0x8, // %x8 denotes a connection close
	rws_opcode_ping = 0x9, // %x9 denotes a ping
	rws_opcode_pong = 0xA // %xA denotes a pong
} rws_opcode;

typedef enum _rws_binary {
	rws_binary_start,
	rws_binary_continue,
	rws_binary_finish
} rws_binary;

typedef struct _rws_frame_struct {
	void * data;
	size_t data_size;
	rws_opcode opcode;
	unsigned char mask[4];
	rws_bool is_masked;
	rws_bool is_finished;
	unsigned char header_size;
} _rws_frame;

size_t rws_check_recv_frame_size(const void * data, const size_t data_size);

_rws_frame * rws_frame_create_with_recv_data(const void * data, const size_t data_size);

// data - should be null, and setted by newly created. 'data' & 'data_size' can be null
void rws_frame_fill_with_send_data(_rws_frame * f, const void * data, const size_t data_size, rws_bool is_finish);

void litews_frame_fill_with_send_bin_data(_rws_frame * f, const void * data, const size_t data_size, rws_binary bin_type);

// combine datas of 2 frames. combined is 'to'
void rws_frame_combine_datas(_rws_frame * to, _rws_frame * from);

_rws_frame * rws_frame_create(void);

void rws_frame_delete(_rws_frame * f);

void rws_frame_delete_clean(_rws_frame ** f);

#endif

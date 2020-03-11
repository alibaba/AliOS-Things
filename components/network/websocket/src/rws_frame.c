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


#include "rws_frame.h"
#include "rws_memory.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define DBG(format,arg...)

_rws_frame * rws_frame_create_with_recv_data(const void * data, const size_t data_size) {
	if (data && data_size >= 2) {
		const unsigned char * udata = (const unsigned char *)data;
		
		const rws_opcode opcode = (rws_opcode)(udata[0] & 0x0f);
		const unsigned int is_finshd = (udata[0] >> 7) & 0x01;
		const unsigned int is_masked = (udata[1] >> 7) & 0x01;
		const unsigned int payload = udata[1] & 0x7f;
		unsigned int header_size = is_masked ? 6 : 2;
		
		unsigned int expected_size = 0, mask_pos = 0;
		size_t index = 0;
		_rws_frame * frame = NULL;
		const unsigned char * actual_udata = NULL;
		unsigned char * unmasked = NULL;
		
		switch (payload) {
			case 126: header_size += 2; break;
			case 127: header_size += 8; break;
			default: break;
		}
		
		if (data_size < header_size) {
			return NULL;
		}
		
		switch (payload) {
			case 126:
				expected_size |= ((unsigned int)udata[2]) << 8;
				expected_size |= (unsigned int)udata[3];
				mask_pos = 4;
				break;
				
			case 127:
				expected_size |= ((unsigned int)udata[6]) << 24;
				expected_size |= ((unsigned int)udata[7]) << 16;
				expected_size |= ((unsigned int)udata[8]) << 8;
				expected_size |= (unsigned int)udata[9];
				mask_pos = 10;
				break;
				
			default:
				if (payload <= 125) {
					mask_pos = 2;
					expected_size = payload;
				}
				break;
		}
		
		frame = rws_frame_create();
		
		frame->opcode = opcode;
		if (is_finshd) frame->is_finished = rws_true;
		frame->header_size = (unsigned char)header_size;
		
		if (is_masked) {
			frame->is_masked = rws_true;
			memcpy(frame->mask, &udata[mask_pos], 4);
		}
		
		if (opcode == rws_opcode_connection_close || opcode == rws_opcode_pong) {
			return frame;
		}
		
		if (!is_finshd) {
			frame->is_finished = rws_false;
		}
		
		if (expected_size > 0) {
			frame->data = rws_malloc(expected_size);
			frame->data_size = expected_size;
			actual_udata = udata + header_size;
			if (is_masked) {
				unmasked = (unsigned char *)frame->data;
				for (index = 0; index < expected_size; index++) {
					*unmasked = *actual_udata ^ frame->mask[index & 0x3];
					unmasked++; actual_udata++;
				}
			} else {
				memcpy(frame->data, actual_udata, expected_size);
			}
		}
		return frame;
	}
	return NULL;
}

void rws_frame_create_header(_rws_frame * f, unsigned char * header, const size_t data_size) {
	const unsigned int size = (unsigned int)data_size;
	unsigned char fin = 0x80;
    if (f->is_finished == rws_false) fin = 0x00;

	*header++ = fin | f->opcode;
	if (size < 126) {
		*header++ = (size & 0xff) | (f->is_masked ? 0x80 : 0);
		f->header_size = 2;
	} else if (size < 65536) {
		*header++ = 126 | (f->is_masked ? 0x80 : 0);
		*header++ = (size >> 8) & 0xff;
		*header++ = size & 0xff;
		f->header_size = 4;
	} else {
		*header++ = 127 | (f->is_masked ? 0x80 : 0);
		
		*(unsigned int *)header = 0;
		header += 4;
		
		*header++ = (size >> 24) & 0xff;
		*header++ = (size >> 16) & 0xff;
		*header++ = (size >> 8) & 0xff;
		*header++ = size & 0xff;
		f->header_size = 10;
	}
	
	if (f->is_masked) {
		memcpy(header, f->mask, 4);
		f->header_size += 4;
	}
}

void litews_frame_create_bin_header(_rws_frame * f, unsigned char * header, const size_t data_size, rws_binary bin_type) 
{
	const unsigned int size = (unsigned int)data_size;

	if(bin_type == rws_binary_start)
	{
		*header++ = 0x00 | f->opcode;
	}
	else if(bin_type == rws_binary_continue)
	{
		*header++ = 0x00 | f->opcode;
	}
	else
	{
		*header++ = 0x80 | f->opcode;
	}

	if (size < 126)
	{
		*header++ = (size & 0xff) | (f->is_masked ? 0x80 : 0);
		f->header_size = 2;
	}
	else if (size < 65536)
	{
		*header++ = 126 | (f->is_masked ? 0x80 : 0);
		*header++ = (size >> 8) & 0xff;
		*header++ = size & 0xff;
		f->header_size = 4;
	}
	else
	{
		*header++ = 127 | (f->is_masked ? 0x80 : 0);

		*(unsigned int *)header = 0;
		header += 4;

		*header++ = (size >> 24) & 0xff;
		*header++ = (size >> 16) & 0xff;
		*header++ = (size >> 8) & 0xff;
		*header++ = size & 0xff;
		f->header_size = 10;
	}

	if (f->is_masked)
	{
		memcpy(header, f->mask, 4);
		f->header_size += 4;
	}
}

void rws_frame_fill_with_send_data(_rws_frame * f, const void * data, const size_t data_size, rws_bool is_finish) {
	unsigned char header[16];
	unsigned char * frame = NULL;
	unsigned char mask[4];
	size_t index = 0;

	f->is_finished = is_finish;
	rws_frame_create_header(f, header, data_size);
	f->data_size = data_size + f->header_size;
	f->data = rws_malloc(f->data_size);
	if (f->data == NULL) {
		DBG("rws_malloc fail\n");
		return;
	}
	frame = (unsigned char *)f->data;
	memcpy(frame, header, f->header_size);
	
	if (data) { // have data to send
		frame += f->header_size;
		memcpy(frame, data, data_size);
		
		if (f->is_masked) {
			memcpy(mask, &f->mask, 4);
			for (index = 0; index < data_size; index++) {
				*frame = *frame ^ mask[index & 0x3];
				frame++;
			}
		}
	}
	//f->is_finished = rws_true;
}

void litews_frame_fill_with_send_bin_data(_rws_frame * f, const void * data, const size_t data_size, rws_binary bin_type) 
{
	unsigned char header[16];
	unsigned char * frame = NULL;
	unsigned char mask[4];
	size_t index = 0;

	litews_frame_create_bin_header(f, header, data_size, bin_type);

	//check bin header data
	//  -----------------------------------------------
	//   F  |   |   |   |   |   |   |   |
	//   I  | 0 | 0 | 0 |    OPCODE     |
	//   N  |   |   |   |   |   |   |   |

	f->data_size = data_size + f->header_size;
	f->data = rws_malloc(f->data_size);
	if (f->data == NULL) {
		DBG("rws_malloc fail\n");
		return;
	}
	frame = (unsigned char *)f->data;
	memcpy(frame, header, f->header_size);

	if (data)
	{ // have data to send
		frame += f->header_size;
		memcpy(frame, data, data_size);

		if (f->is_masked)
		{
			memcpy(mask, &f->mask, 4);
			for (index = 0; index < data_size; index++)
			{
				*frame = *frame ^ mask[index & 0x3];
				frame++;
			}
		}
	}

	if(bin_type == rws_binary_finish)
	{
		f->is_finished = rws_true;
	}
	else
	{
		f->is_finished = rws_false;
	}
}

void rws_frame_combine_datas(_rws_frame * to, _rws_frame * from) {
	unsigned char * comb_data = (unsigned char *)rws_malloc(to->data_size + from->data_size);
	if (comb_data) {
		if (to->data && to->data_size) {
			memcpy(comb_data, to->data, to->data_size);
		}
		comb_data += to->data_size;
		if (from->data && from->data_size) {
			memcpy(comb_data, from->data, from->data_size);
		}
	}
	rws_free(to->data);
	to->data = comb_data;
	to->data_size += from->data_size;
}

_rws_frame * rws_frame_create(void) {
	_rws_frame * f = (_rws_frame *)rws_malloc_zero(sizeof(_rws_frame));
	union {
		unsigned int ui;
		unsigned char b[4];
	} mask_union;
	assert(sizeof(unsigned int) == 4);
	//	mask_union.ui = 2018915346;
	mask_union.ui = (rand() / (RAND_MAX / 2) + 1) * rand();
	memcpy(f->mask, mask_union.b, 4);
	return f;
}

void rws_frame_delete(_rws_frame * f) {
	if (f) {
		rws_free(f->data);
		rws_free(f);
	}
}

void rws_frame_delete_clean(_rws_frame ** f) {
	if (f) {
		rws_frame_delete(*f);
		*f = NULL;
	}
}

size_t rws_check_recv_frame_size(const void * data, const size_t data_size) {
	if (data && data_size >= 2) {
		const unsigned char * udata = (const unsigned char *)data;
		//        const unsigned int is_finshd = (udata[0] >> 7) & 0x01;
		const unsigned int is_masked = (udata[1] >> 7) & 0x01;
		const unsigned int payload = udata[1] & 0x7f;
		unsigned int header_size = is_masked ? 6 : 2;
		
		unsigned int expected_size = 0;
		
		switch (payload) {
			case 126: header_size += 2; break;
			case 127: header_size += 8; break;
			default: break;
		}
		
		if (data_size < header_size) {
			return 0;
		}
		
		switch (payload) {
			case 126:
				expected_size |= ((unsigned int)udata[2]) << 8;
				expected_size |= (unsigned int)udata[3];
				break;
				
			case 127:
				expected_size |= ((unsigned int)udata[6]) << 24;
				expected_size |= ((unsigned int)udata[7]) << 16;
				expected_size |= ((unsigned int)udata[8]) << 8;
				expected_size |= (unsigned int)udata[9];
				break;
				
			default:
				if (payload <= 125) {
					expected_size = payload;
				}
				break;
		}
		
		const unsigned int nPackSize = expected_size + header_size;
		return (nPackSize <= data_size) ? nPackSize : 0;
	}
	return 0;
}



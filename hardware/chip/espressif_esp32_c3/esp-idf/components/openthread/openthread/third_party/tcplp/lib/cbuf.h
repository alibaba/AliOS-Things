/*
 *  Copyright (c) 2018, Sam Kumar <samkumar@cs.berkeley.edu>
 *  Copyright (c) 2018, University of California, Berkeley
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CBUF_H_
#define CBUF_H_

/* CIRCULAR BUFFER */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

struct otLinkedBuffer;

/* Represents a circular buffer. */
struct cbufhead {
    size_t r_index;
    size_t w_index;
    size_t size;
    uint8_t* buf;
};

/* Initializes a circular buffer (chdr is the header storing metadata). */
void cbuf_init(struct cbufhead* chdr, uint8_t* buf, size_t len);

/* Function type for copying data into or out of a circular buffer. */
typedef void(*cbuf_copier_t)(void*, size_t, const void*, size_t, size_t);

/* Instantiations of cbuf_copier_t, for copying data between circular buffers and regular buffers. */
void cbuf_copy_into_buffer(void* buffer, size_t buffer_offset, const void* arr, size_t arr_offset, size_t num_bytes);
void cbuf_copy_from_buffer(void* arr, size_t arr_offset, const void* buffer, size_t buffer_offset, size_t num_bytes);

/* Instantiations of cbuf_copier_t, for copying data between circular buffers and otMessages. */
void cbuf_copy_into_message(void* buffer, size_t buffer_offset, const void* arr, size_t arr_offset, size_t num_bytes);
void cbuf_copy_from_message(void* arr, size_t arr_offset, const void* buffer, size_t buffer_offset, size_t num_bytes);

/* Writes data to the back of the circular buffer using the specified copier. */
size_t cbuf_write(struct cbufhead* chdr, const void* data, size_t data_offset, size_t data_len, cbuf_copier_t copy_from);

/* Reads data from the front ofthe circular buffer using the specified copier. */
size_t cbuf_read(struct cbufhead* chdr, void* data, size_t data_offset, size_t numbytes, int pop, cbuf_copier_t copy_into);

/* Reads data at the specified offset, in bytes, from the front of the circular buffer using the specified copier. */
size_t cbuf_read_offset(struct cbufhead* chdr, void* data, size_t data_offset, size_t numbytes, size_t offset, cbuf_copier_t copy_into);

/* Drops bytes from the front of the circular buffer. */
size_t cbuf_pop(struct cbufhead* chdr, size_t numbytes);

/* Returns the number of bytes available for reading from the circular buffer. */
size_t cbuf_used_space(struct cbufhead* chdr);

/* Returns the number of bytes that can be written to the circular buffer before it is full. */
size_t cbuf_free_space(struct cbufhead* chdr);

/* Returns the total capacity of the circular buffer, in bytes. */
size_t cbuf_size(struct cbufhead* chdr);

/* Returns true if the circular buffer is empty, and false if it is not empty. */
bool cbuf_empty(struct cbufhead* chdr);

/* Populates the provided otLinkedBuffers to reference the data currently in the circular buffer. */
void cbuf_reference(const struct cbufhead* chdr, struct otLinkedBuffer* first, struct otLinkedBuffer* second);

/* Writes DATA at the end of the circular buffer without making it available for
   reading. This data is said to be "out-of-sequence". OFFSET is position at
   which to write these bytes, relative to the positoin where cbuf_write would
   write them. Each bit in the BITMAP corresponds to a byte in the circular
   buffer; the bits corresponding to the bytes containing the newly written
   data are set. The index of the first byte written is stored into FIRSTINDEX,
   if it is not NULL. */
size_t cbuf_reass_write(struct cbufhead* chdr, size_t offset, const void* data, size_t data_offset, size_t numbytes, uint8_t* bitmap, size_t* firstindex, cbuf_copier_t copy_from);

/* Makes NUMBYTES out-of-seqence bytes available for reading in the circular
   buffer. No data is copied; the out-of-sequence bytes made available are the
   bytes currently at the position where cbuf_write would write them. The bytes
   are taken from the unused space of the buffer, and can be set using
   cbuf_reass_write. */
size_t cbuf_reass_merge(struct cbufhead* chdr, size_t numbytes, uint8_t* bitmap);

/* Counts the number of contiguous out-of-sequence bytes at the specified
   OFFSET, until the count reaches the specified LIMIT. */
size_t cbuf_reass_count_set(struct cbufhead* chdr, size_t offset, uint8_t* bitmap, size_t limit);

/* Returns a true value iff INDEX is the index of a byte within OFFSET bytes
   past the end of the buffer. */
int cbuf_reass_within_offset(struct cbufhead* chdr, size_t offset, size_t index);

#endif

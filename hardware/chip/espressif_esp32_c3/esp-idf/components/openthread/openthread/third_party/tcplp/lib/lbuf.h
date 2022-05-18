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

#ifndef LBUF_H_
#define LBUF_H_

#include <stddef.h>
#include <stdint.h>

struct otLinkedBuffer;

/* LINKED BUFFER */

struct lbufhead {
    struct otLinkedBuffer* head;
    struct otLinkedBuffer* tail;
    size_t offset;
    size_t length;
};

/* Initializes a linked buffer. */
void lbuf_init(struct lbufhead* buffer);

/* Returns the contents of the buffer as a linked buffer chain, or NULL if the buffer has
   no head. */
static inline struct otLinkedBuffer* lbuf_head(struct lbufhead* buffer) {
    return buffer->head;
}

/* Adds the contents of NEWENTRY to the buffer by appending it to the end of
   the current chain. */
void lbuf_append(struct lbufhead* buffer, struct otLinkedBuffer* newentry);

/* Extends the last entry of the buffer by the specified number of bytes. */
void lbuf_extend(struct lbufhead* buffer, size_t numbytes);

/* Removes the first NUMBYTES bytes from the buffer, and returns the number of
   bytes removed (which is fewer than NUMBYTES if there were fewer than
   NUMBYTES bytes in the buffer to begin with). *NTRAVERSED is incremented once
   for each entry in the buffer that is no longer referenced and can be
   reclaimed. */
size_t lbuf_pop(struct lbufhead* buffer, size_t numbytes, int* ntraversed);

/* Given a range of indices, specified by an OFFSET from the start and a
   length NUMBYTES, this function locates the chain of linked buffer entries
   that reference the corresponding bytes.
   A pointer to the first entry in the range is stored into FIRST, and the
   number of bytes in the entry before the start of the range is stored into
   FIRSTOFFSET. A pointer to the last entry in the range is stored into LAST,
   and the number of bytes in that entry after the end of the range is stored
   into LASTEXTRA.
   Returns 0 on success and 1 on failure. On failure, FIRST, LAST, FIRSTOFFSET,
   and LASTEXTRA are not set. The only failure condition is when there are not
   enough bytes in the buffer to do the full traversal. */
int lbuf_getrange(struct lbufhead* buffer, size_t offset, size_t numbytes,
                  struct otLinkedBuffer** first, size_t* firstoffset,
                  struct otLinkedBuffer** last, size_t* lastextra);

/* Returns the total number of bytes stored in the buffer. */
size_t lbuf_used_space(struct lbufhead* buffer);

#endif

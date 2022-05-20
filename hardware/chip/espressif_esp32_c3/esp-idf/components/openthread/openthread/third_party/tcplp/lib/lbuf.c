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

/* LINKED BUFFER */

#include "lbuf.h"
#include <string.h>
#include <openthread/tcp.h>

void lbuf_init(struct lbufhead* buffer) {
    memset(buffer, 0x00, sizeof(struct lbufhead));
}

void lbuf_append(struct lbufhead* buffer, otLinkedBuffer* newentry) {
    otLinkedBuffer* tail = buffer->tail;
    if (tail == NULL) {
        buffer->head = newentry;
        buffer->tail = newentry;
        buffer->offset = 0;
        buffer->length = newentry->mLength;
        newentry->mNext = NULL;
    } else {
        tail->mNext = newentry;
        buffer->tail = newentry;
        buffer->length += newentry->mLength;
        newentry->mNext = NULL;
    }
}

void lbuf_extend(struct lbufhead* buffer, size_t numbytes) {
    buffer->tail->mLength += numbytes;
}

size_t lbuf_pop(struct lbufhead* buffer, size_t numbytes, int* ntraversed) {
    otLinkedBuffer* curr = buffer->head;
    size_t bytesleft = numbytes;
    size_t curroffset = buffer->offset;
    if (curr == NULL) {
        return 0;
    }
    while (bytesleft >= curr->mLength - curroffset) {
        ++*ntraversed;
        bytesleft -= (curr->mLength - curroffset);
        buffer->length -= (curr->mLength - curroffset);
        if (buffer->tail == curr) {
            buffer->head = NULL;
            buffer->tail = NULL;
            buffer->offset = 0;
            return numbytes - bytesleft;
        }
        curr = curr->mNext;
        curroffset = 0;
    }
    /* Handle the last entry. */
    buffer->head = curr;
    buffer->offset = curroffset + bytesleft;
    buffer->length -= bytesleft;
    return numbytes;
}

int lbuf_getrange(struct lbufhead* buffer, size_t offset, size_t numbytes,
                  otLinkedBuffer** first, size_t* firstoffset,
                  otLinkedBuffer** last, size_t* lastextra) {
    otLinkedBuffer* curr = buffer->head;
    size_t offsetleft = offset + buffer->offset;
    size_t bytesleft = numbytes;
    if (buffer->length < offset + numbytes) {
        return 1; // out of range
    }
    while (offsetleft > 0 && offsetleft >= curr->mLength) {
        offsetleft -= curr->mLength;
        curr = curr->mNext;
    }
    *first = curr;
    *firstoffset = offsetleft;
    bytesleft += offsetleft;
    while (bytesleft > 0 && bytesleft > curr->mLength) {
        bytesleft -= curr->mLength;
        curr = curr->mNext;
    }
    *last = curr;
    *lastextra = curr->mLength - bytesleft;
    return 0;
}

size_t lbuf_used_space(struct lbufhead* buffer) {
    return buffer->length;
}

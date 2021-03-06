/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2012 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "nghttp2_queue.h"

#include <string.h>
#include <assert.h>
#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#endif

extern void *HAL_Malloc(uint32_t size);
extern void HAL_Free(void *ptr);

#if INFRA_MEM_STATS
#define NGHTTP2_QUEUE_MALLOC(size)         LITE_malloc(size, MEM_MAGIC, "nghttp2.queue")
#define NGHTTP2_QUEUE_FREE(ptr)            LITE_free(ptr)
#else
#define NGHTTP2_QUEUE_MALLOC(size)         HAL_Malloc(size)
#define NGHTTP2_QUEUE_FREE(ptr)            {HAL_Free((void *)ptr);ptr = NULL;}
#endif

void nghttp2_queue_init(nghttp2_queue *queue) {
  queue->front = queue->back = NULL;
}

void nghttp2_queue_free(nghttp2_queue *queue) {
  if (!queue) {
    return;
  } else {
    nghttp2_queue_cell *p = queue->front;
    while (p) {
      nghttp2_queue_cell *next = p->next;
      NGHTTP2_QUEUE_FREE(p);
      p = next;
    }
  }
}

int nghttp2_queue_push(nghttp2_queue *queue, void *data) {
  nghttp2_queue_cell *new_cell =
      (nghttp2_queue_cell *)NGHTTP2_QUEUE_MALLOC(sizeof(nghttp2_queue_cell));
  if (!new_cell) {
    return NGHTTP2_ERR_NOMEM;
  }
  new_cell->data = data;
  new_cell->next = NULL;
  if (queue->back) {
    queue->back->next = new_cell;
    queue->back = new_cell;

  } else {
    queue->front = queue->back = new_cell;
  }
  return 0;
}

void nghttp2_queue_pop(nghttp2_queue *queue) {
  nghttp2_queue_cell *front = queue->front;
  assert(front);
  queue->front = front->next;
  if (front == queue->back) {
    queue->back = NULL;
  }
  NGHTTP2_QUEUE_FREE(front);
}

void *nghttp2_queue_front(nghttp2_queue *queue) {
  assert(queue->front);
  return queue->front->data;
}

void *nghttp2_queue_back(nghttp2_queue *queue) {
  assert(queue->back);
  return queue->back->data;
}

int nghttp2_queue_empty(nghttp2_queue *queue) { return queue->front == NULL; }

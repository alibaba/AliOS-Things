/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include "nghttp2_queue.h"

#include <string.h>
#include <assert.h>

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
      free(p);
      p = next;
    }
  }
}

int nghttp2_queue_push(nghttp2_queue *queue, void *data) {
  nghttp2_queue_cell *new_cell =
      (nghttp2_queue_cell *)malloc(sizeof(nghttp2_queue_cell));
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
  free(front);
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

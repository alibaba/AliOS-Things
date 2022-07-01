/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Common data structures.
 */
#ifndef __COLLECTIONS_H__
#define __COLLECTIONS_H__
#include <stdbool.h>
#include <stddef.h>

////////////
// bitmap //
////////////

typedef struct bitmap
{
    size_t size;
    char *data;
}
bitmap_t;

void bitmap_alloc(bitmap_t *ptr, size_t size);
void bitmap_free(bitmap_t *ptr);
void bitmap_clear(bitmap_t *ptr);
void bitmap_bit_set(bitmap_t *ptr, size_t index);
bool bitmap_bit_get(bitmap_t *ptr, size_t index);
#define BITMAP_COMPUTE_ROW_INDEX(image, y) (((image)->w)*(y))
#define BITMAP_COMPUTE_INDEX(row_index, x) ((row_index)+(x))

//////////
// lifo //
//////////

typedef struct lifo
{
    size_t len, size, data_len;
    char *data;
}
lifo_t;

void lifo_alloc(lifo_t *ptr, size_t size, size_t data_len);
void lifo_alloc_all(lifo_t *ptr, size_t *size, size_t data_len);
void lifo_free(lifo_t *ptr);
void lifo_clear(lifo_t *ptr);
size_t lifo_size(lifo_t *ptr);
bool lifo_is_not_empty(lifo_t *ptr);
bool lifo_is_not_full(lifo_t *ptr);
void lifo_enqueue(lifo_t *ptr, void *data);
void lifo_dequeue(lifo_t *ptr, void *data);
void lifo_poke(lifo_t *ptr, void *data);
void lifo_peek(lifo_t *ptr, void *data);

//////////
// fifo //
//////////

typedef struct fifo
{
    size_t head_ptr, tail_ptr, len, size, data_len;
    char *data;
}
fifo_t;

void fifo_alloc(fifo_t *ptr, size_t size, size_t data_len);
void fifo_alloc_all(fifo_t *ptr, size_t *size, size_t data_len);
void fifo_free(fifo_t *ptr);
void fifo_clear(fifo_t *ptr);
size_t fifo_size(fifo_t *ptr);
bool fifo_is_not_empty(fifo_t *ptr);
bool fifo_is_not_full(fifo_t *ptr);
void fifo_enqueue(fifo_t *ptr, void *data);
void fifo_dequeue(fifo_t *ptr, void *data);
void fifo_poke(fifo_t *ptr, void *data);
void fifo_peek(fifo_t *ptr, void *data);

//////////
// list //
//////////

typedef struct list_lnk
{
    struct list_lnk *next_ptr, *prev_ptr;
    char data[];
}
list_lnk_t;

typedef struct list
{
    list_lnk_t *head_ptr, *tail_ptr;
    size_t size, data_len;
}
list_t;

void list_init(list_t *ptr, size_t data_len);
void list_copy(list_t *dst, list_t *src);
void list_free(list_t *ptr);
void list_clear(list_t *ptr);
size_t list_size(list_t *ptr);
void list_push_front(list_t *ptr, void *data);
void list_push_back(list_t *ptr, void *data);
void list_pop_front(list_t *ptr, void *data);
void list_pop_back(list_t *ptr, void *data);
void list_get_front(list_t *ptr, void *data);
void list_get_back(list_t *ptr, void *data);
void list_set_front(list_t *ptr, void *data);
void list_set_back(list_t *ptr, void *data);
void list_insert(list_t *ptr, void *data, size_t index);
void list_remove(list_t *ptr, void *data, size_t index);
void list_get(list_t *ptr, void *data, size_t index);
void list_set(list_t *ptr, void *data, size_t index);

//////////////
// iterator //
//////////////

list_lnk_t *iterator_start_from_head(list_t *ptr);
list_lnk_t *iterator_start_from_tail(list_t *ptr);
list_lnk_t *iterator_next(list_lnk_t *lnk);
list_lnk_t *iterator_prev(list_lnk_t *lnk);
void iterator_get(list_t *ptr, list_lnk_t *lnk, void *data);
void iterator_set(list_t *ptr, list_lnk_t *lnk, void *data);

#endif /* __COLLECTIONS_H__ */

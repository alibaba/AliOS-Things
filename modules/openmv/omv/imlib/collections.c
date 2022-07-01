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
#include "imlib.h"
#define CHAR_BITS (sizeof(char) * 8)
#define CHAR_MASK (CHAR_BITS - 1)
#define CHAR_SHIFT IM_LOG2(CHAR_MASK)

////////////
// bitmap //
////////////

void bitmap_alloc(bitmap_t *ptr, size_t size)
{
    ptr->size = size;
    ptr->data = (char *) fb_alloc0(((size + CHAR_MASK) >> CHAR_SHIFT) * sizeof(char), FB_ALLOC_NO_HINT);
}

void bitmap_free(bitmap_t *ptr)
{
    if (ptr->data) {
        fb_free();
    }
}

void bitmap_clear(bitmap_t *ptr)
{
    memset(ptr->data, 0, ((ptr->size + CHAR_MASK) >> CHAR_SHIFT) * sizeof(char));
}

void bitmap_bit_set(bitmap_t *ptr, size_t index)
{
    ptr->data[index >> CHAR_SHIFT] |= 1 << (index & CHAR_MASK);
}

bool bitmap_bit_get(bitmap_t *ptr, size_t index)
{
    return (ptr->data[index >> CHAR_SHIFT] >> (index & CHAR_MASK)) & 1;
}

//////////
// lifo //
//////////

void lifo_alloc(lifo_t *ptr, size_t size, size_t data_len)
{
    ptr->len = 0;
    ptr->size = size;
    ptr->data_len = data_len;
    ptr->data = (char *) fb_alloc(size * data_len, FB_ALLOC_NO_HINT);
}

void lifo_alloc_all(lifo_t *ptr, size_t *size, size_t data_len)
{
    uint32_t tmp_size;
    ptr->data = (char *) fb_alloc_all(&tmp_size, FB_ALLOC_NO_HINT);
    ptr->data_len = data_len;
    ptr->size = tmp_size / data_len;
    ptr->len = 0;
    *size = ptr->size;
}

void lifo_free(lifo_t *ptr)
{
    if (ptr->data) {
        fb_free();
    }
}

void lifo_clear(lifo_t *ptr)
{
    ptr->len = 0;
}

size_t lifo_size(lifo_t *ptr)
{
    return ptr->len;
}

bool lifo_is_not_empty(lifo_t *ptr)
{
    return ptr->len;
}

bool lifo_is_not_full(lifo_t *ptr)
{
    return ptr->len != ptr->size;
}

void lifo_enqueue(lifo_t *ptr, void *data)
{
    memcpy(ptr->data + (ptr->len * ptr->data_len), data, ptr->data_len);

    ptr->len += 1;
}

void lifo_dequeue(lifo_t *ptr, void *data)
{
    if (data) {
        memcpy(data, ptr->data + ((ptr->len - 1) * ptr->data_len), ptr->data_len);
    }

    ptr->len -= 1;
}

void lifo_poke(lifo_t *ptr, void *data)
{
    memcpy(ptr->data + (ptr->len * ptr->data_len), data, ptr->data_len);
}

void lifo_peek(lifo_t *ptr, void *data)
{
    memcpy(data, ptr->data + ((ptr->len - 1) * ptr->data_len), ptr->data_len);
}

//////////
// fifo //
//////////

void fifo_alloc(fifo_t *ptr, size_t size, size_t data_len)
{
    ptr->head_ptr = 0;
    ptr->tail_ptr = 0;
    ptr->len = 0;
    ptr->size = size;
    ptr->data_len = data_len;
    ptr->data = (char *) fb_alloc(size * data_len, FB_ALLOC_NO_HINT);
}

void fifo_alloc_all(fifo_t *ptr, size_t *size, size_t data_len)
{
    uint32_t tmp_size;
    ptr->data = (char *) fb_alloc_all(&tmp_size, FB_ALLOC_NO_HINT);
    ptr->data_len = data_len;
    ptr->size = tmp_size / data_len;
    ptr->len = 0;
    ptr->tail_ptr = 0;
    ptr->head_ptr = 0;
    *size = ptr->size;
}

void fifo_free(fifo_t *ptr)
{
    if (ptr->data) {
        fb_free();
    }
}

void fifo_clear(fifo_t *ptr)
{
    ptr->head_ptr = 0;
    ptr->tail_ptr = 0;
    ptr->len = 0;
}

size_t fifo_size(fifo_t *ptr)
{
    return ptr->len;
}

bool fifo_is_not_empty(fifo_t *ptr)
{
    return ptr->len;
}

bool fifo_is_not_full(fifo_t *ptr)
{
    return ptr->len != ptr->size;
}

void fifo_enqueue(fifo_t *ptr, void *data)
{
    memcpy(ptr->data + (ptr->head_ptr * ptr->data_len), data, ptr->data_len);

    size_t temp = ptr->head_ptr + 1;

    if (temp == ptr->size) {
        temp = 0;
    }

    ptr->head_ptr = temp;
    ptr->len += 1;
}

void fifo_dequeue(fifo_t *ptr, void *data)
{
    if (data) {
        memcpy(data, ptr->data + (ptr->tail_ptr * ptr->data_len), ptr->data_len);
    }

    size_t temp = ptr->tail_ptr + 1;

    if (temp == ptr->size) {
        temp = 0;
    }

    ptr->tail_ptr = temp;
    ptr->len -= 1;
}

void fifo_poke(fifo_t *ptr, void *data)
{
    memcpy(ptr->data + (ptr->head_ptr * ptr->data_len), data, ptr->data_len);
}

void fifo_peek(fifo_t *ptr, void *data)
{
    memcpy(data, ptr->data + (ptr->tail_ptr * ptr->data_len), ptr->data_len);
}

//////////
// list //
//////////

void list_init(list_t *ptr, size_t data_len)
{
    ptr->head_ptr = NULL;
    ptr->tail_ptr = NULL;
    ptr->size = 0;
    ptr->data_len = data_len;
}

void list_copy(list_t *dst, list_t *src)
{
    memcpy(dst, src, sizeof(list_t));
}

void list_free(list_t *ptr)
{
    for (list_lnk_t *i = ptr->head_ptr; i; ) {
        list_lnk_t *j = i->next_ptr;
        xfree(i);
        i = j;
    }
}

void list_clear(list_t *ptr)
{
    list_free(ptr);

    ptr->head_ptr = NULL;
    ptr->tail_ptr = NULL;
    ptr->size = 0;
}

size_t list_size(list_t *ptr)
{
    return ptr->size;
}

void list_push_front(list_t *ptr, void *data)
{
    list_lnk_t *tmp = (list_lnk_t *) xalloc(sizeof(list_lnk_t) + ptr->data_len);
    memcpy(tmp->data, data, ptr->data_len);

    if (ptr->size++) {
        tmp->next_ptr = ptr->head_ptr;
        tmp->prev_ptr = NULL;
        ptr->head_ptr->prev_ptr = tmp;
        ptr->head_ptr = tmp;
    } else {
        tmp->next_ptr = NULL;
        tmp->prev_ptr = NULL;
        ptr->head_ptr = tmp;
        ptr->tail_ptr = tmp;
    }
}

void list_push_back(list_t *ptr, void *data)
{
    list_lnk_t *tmp = (list_lnk_t *) xalloc(sizeof(list_lnk_t) + ptr->data_len);
    memcpy(tmp->data, data, ptr->data_len);

    if (ptr->size++) {
        tmp->next_ptr = NULL;
        tmp->prev_ptr = ptr->tail_ptr;
        ptr->tail_ptr->next_ptr = tmp;
        ptr->tail_ptr = tmp;
    } else {
        tmp->next_ptr = NULL;
        tmp->prev_ptr = NULL;
        ptr->head_ptr = tmp;
        ptr->tail_ptr = tmp;
    }
}

void list_pop_front(list_t *ptr, void *data)
{
    list_lnk_t *tmp = ptr->head_ptr;

    if (data) {
        memcpy(data, tmp->data, ptr->data_len);
    }

    if (tmp->next_ptr) {
        tmp->next_ptr->prev_ptr = NULL;
    }
    ptr->head_ptr = tmp->next_ptr;
    ptr->size -= 1;
    xfree(tmp);
}

void list_pop_back(list_t *ptr, void *data)
{
    list_lnk_t *tmp = ptr->tail_ptr;

    if (data) {
        memcpy(data, tmp->data, ptr->data_len);
    }

    tmp->prev_ptr->next_ptr = NULL;
    ptr->tail_ptr = tmp->prev_ptr;
    ptr->size -= 1;
    xfree(tmp);
}

void list_get_front(list_t *ptr, void *data)
{
    memcpy(data, ptr->head_ptr->data, ptr->data_len);
}

void list_get_back(list_t *ptr, void *data)
{
    memcpy(data, ptr->tail_ptr->data, ptr->data_len);
}

void list_set_front(list_t *ptr, void *data)
{
    memcpy(ptr->head_ptr->data, data, ptr->data_len);
}

void list_set_back(list_t *ptr, void *data)
{
    memcpy(ptr->tail_ptr->data, data, ptr->data_len);
}

void list_insert(list_t *ptr, void *data, size_t index)
{
    if (index == 0) {
        list_push_front(ptr, data);
    } else if (index >= ptr->size) {
        list_push_back(ptr, data);
    } else if (index < (ptr->size >> 1)) {

        list_lnk_t *i = ptr->head_ptr;

        while (index) {
            i = i->next_ptr;
            index -= 1;
        }

        list_lnk_t *tmp = (list_lnk_t *) xalloc(sizeof(list_lnk_t) + ptr->data_len);
        memcpy(tmp->data, data, ptr->data_len);

        tmp->next_ptr = i;
        tmp->prev_ptr = i->prev_ptr;
        i->prev_ptr->next_ptr = tmp;
        i->prev_ptr = tmp;
        ptr->size += 1;

    } else {

        list_lnk_t *i = ptr->tail_ptr;
        index = ptr->size - index - 1;

        while (index) {
            i = i->prev_ptr;
            index -= 1;
        }

        list_lnk_t *tmp = (list_lnk_t *) xalloc(sizeof(list_lnk_t) + ptr->data_len);
        memcpy(tmp->data, data, ptr->data_len);

        tmp->next_ptr = i;
        tmp->prev_ptr = i->prev_ptr;
        i->prev_ptr->next_ptr = tmp;
        i->prev_ptr = tmp;
        ptr->size += 1;
    }
}

void list_remove(list_t *ptr, void *data, size_t index)
{
    if (index == 0) {
        list_pop_front(ptr, data);
    } else if (index >= (ptr->size - 1)) {
        list_pop_back(ptr, data);
    } else if (index < (ptr->size >> 1)) {

        list_lnk_t *i = ptr->head_ptr;

        while (index) {
            i = i->next_ptr;
            index -= 1;
        }

        if (data) {
            memcpy(data, i->data, ptr->data_len);
        }

        i->prev_ptr->next_ptr = i->next_ptr;
        i->next_ptr->prev_ptr = i->prev_ptr;
        ptr->size -= 1;
        xfree(i);

    } else {

        list_lnk_t *i = ptr->tail_ptr;
        index = ptr->size - index - 1;

        while (index) {
            i = i->prev_ptr;
            index -= 1;
        }

        if (data) {
            memcpy(data, i->data, ptr->data_len);
        }

        i->prev_ptr->next_ptr = i->next_ptr;
        i->next_ptr->prev_ptr = i->prev_ptr;
        ptr->size -= 1;
        xfree(i);
    }
}

void list_get(list_t *ptr, void *data, size_t index)
{
    if (index == 0) {
        list_get_front(ptr, data);
    } else if (index >= (ptr->size - 1)) {
        list_get_back(ptr, data);
    } else if (index < (ptr->size >> 1)) {

        list_lnk_t *i = ptr->head_ptr;

        while (index) {
            i = i->next_ptr;
            index -= 1;
        }

        memcpy(data, i->data, ptr->data_len);

    } else {

        list_lnk_t *i = ptr->tail_ptr;
        index = ptr->size - index - 1;

        while (index) {
            i = i->prev_ptr;
            index -= 1;
        }

        memcpy(data, i->data, ptr->data_len);
    }
}

void list_set(list_t *ptr, void *data, size_t index)
{
    if (index == 0) {
        list_set_front(ptr, data);
    } else if (index >= (ptr->size - 1)) {
        list_set_back(ptr, data);
    } else if (index < (ptr->size >> 1)) {

        list_lnk_t *i = ptr->head_ptr;

        while (index) {
            i = i->next_ptr;
            index -= 1;
        }

        memcpy(i->data, data, ptr->data_len);

    } else {

        list_lnk_t *i = ptr->tail_ptr;
        index = ptr->size - index - 1;

        while (index) {
            i = i->prev_ptr;
            index -= 1;
        }

        memcpy(i->data, data, ptr->data_len);
    }
}

//////////////
// iterator //
//////////////

list_lnk_t *iterator_start_from_head(list_t *ptr)
{
    return ptr->head_ptr;
}

list_lnk_t *iterator_start_from_tail(list_t *ptr)
{
    return ptr->tail_ptr;
}

list_lnk_t *iterator_next(list_lnk_t *lnk)
{
    return lnk->next_ptr;
}

list_lnk_t *iterator_prev(list_lnk_t *lnk)
{
    return lnk->prev_ptr;
}

void iterator_get(list_t *ptr, list_lnk_t *lnk, void *data)
{
    memcpy(data, lnk->data, ptr->data_len);
}

void iterator_set(list_t *ptr, list_lnk_t *lnk, void *data)
{
    memcpy(lnk->data, data, ptr->data_len);
}

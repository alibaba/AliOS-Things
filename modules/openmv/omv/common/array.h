/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Dynamic array.
 */
#ifndef __ARRAY_H__
#define __ARRAY_H__
typedef void (*array_dtor_t)(void*);
typedef int  (*array_comp_t)(const void*, const void*);
// (left < right) == negative
// (left == right) == zero
// (left > right) == positive
typedef struct {
    int index;
    int length;
    void **data;
    array_dtor_t dtor;
} array_t;
void array_alloc(array_t **a, array_dtor_t dtor);
void array_alloc_init(array_t **a, array_dtor_t dtor, int size);
void array_clear(array_t *array);
void array_free(array_t *array);
int array_length(array_t *array);
void *array_at(array_t *array, int idx);
void array_push_back(array_t *array, void *element);
void *array_pop_back(array_t *array);
void *array_take(array_t *array, int idx);
void array_erase(array_t *array, int idx);
void array_resize(array_t *array, int num);
void array_sort(array_t *array, array_comp_t comp);
void array_isort(array_t *array, array_comp_t comp);
#endif //__ARRAY_H__

#ifndef _WIDGET_BASE_H
#define _WIDGET_BASE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include <memory.h>
#include "amp_defines.h"
//#include "amp_system.h"
#ifdef __cplusplus
extern "C" {
#endif

#if 0
typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned int        uint32_t;
typedef signed int          int32_t;
// typedef unsigned long long  uint64_t;
// typedef long long           int64_t;
#endif

typedef union {
    uint32_t value;
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    };
} widget_color;

typedef struct _widget_default_type_map_t {
    int   type;
    char  *name;
} widget_default_type_map_t;

typedef struct _list_node_t {
    struct _list_node_t *prev;
    struct _list_node_t *next;
    void              *data;
}list_node_t;

typedef struct _list {
    list_node_t head;
    list_node_t tail;
    uint32_t  len;
}list;

char *widget_str_duplicate(char *src);

int widget_style_type_get(char *str);

bool widget_str_equal(char *str1, char *str2);

int widget_str2type(char *name, widget_default_type_map_t* map, int max);

int widget_str2uint(char *str, uint32_t *data);

int widget_str2int(char *str, int32_t *data);

int widget_color_parse(char *str, widget_color *color);

void amp_list_init(list* list);

void amp_node_insert(list_node_t *list_node, list_node_t *node);

void amp_node_delete(list_node_t *node);

void amp_list_node_insert(list* list, int index, list_node_t *node);

void amp_list_node_append(list* list, list_node_t *node);

int amp_list_node_delete(list *list, list_node_t *node);

int amp_strtok_num_get(char *str, const char *delim);

#if 0
void *amp_debug_malloc(uint32_t size, char* func, int line);


void *amp_debug_calloc(uint32_t nitems, uint32_t size, char* func, int line);


void *amp_debug_realloc(void* addr, uint32_t size, char* func, int line);

void amp_debug_free(void* addr, char* func, int line);



#define render_alloc(size)              amp_debug_malloc(size, __func__, __LINE__)
#define render_calloc(nitems, size)     amp_debug_calloc(nitems, size, __func__, __LINE__)
#define render_realloc(addr, size)      amp_debug_realloc(addr, size, __func__, __LINE__)
#define render_free(addr)               amp_debug_free(addr, __func__, __LINE__)
#else
#define render_alloc   aos_malloc
#define render_calloc  aos_calloc
#define render_realloc aos_realloc
#define render_free    aos_free
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_WIDGET_BASE_H*/


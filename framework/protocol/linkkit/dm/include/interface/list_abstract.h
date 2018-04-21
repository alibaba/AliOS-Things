#ifndef LIST_ABSTRACT_H
#define LIST_ABSTRACT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdarg.h>
#include <stdlib.h>

typedef void (*print_fp_t)(void* data);
typedef void (*handle_fp_t)(void* node, va_list* params);

typedef struct {
    size_t size;
    const char*  _class_name;
    void* (*ctor)(void* _self, va_list* params);
    void* (*dtor)(void* _self);
    void  (*insert)(void* _self, void* data);
    void  (*remove)(void* _self, void* data);
    void  (*clear)(void* _self);
    int   (*empty)(const void* _self);
    int   (*get_size)(const void* _self);
    void  (*iterator)(const void* _self, handle_fp_t handle_fp, va_list* params);
    void  (*print)(const void* _self, print_fp_t print_fp);
} list_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIST_ABSTRACT_H */

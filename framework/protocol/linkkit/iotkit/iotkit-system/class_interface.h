#ifndef __CLASS_INTERFACE_H__
#define __CLASS_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdarg.h>

/* base-class define */
/* abstract class */
typedef struct {
    size_t       _size;
    const char*  _class_name;
    void*        (*ctor)(void* _self, va_list* params);
    void*        (*dtor)(void* _self);
} abstract_class_t;


/* new & delete */
extern void* new_object(const void* _class, ...);
extern void  delete_object(void* _class);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CLASS_INTERFACE_H__ */
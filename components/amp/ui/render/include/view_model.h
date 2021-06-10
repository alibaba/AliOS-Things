#ifndef _VIEW_MODEL_H
#define _VIEW_MODEL_H

//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include <memory.h>
#include "aos_system.h"
#include "render.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef struct _vm_desc_t{
    vm_data_state_e  state;
    widget_desc_t   *widget;
    char            *name;

    char            *props;
    //vm_data_type_e   type;
    vm_data_type_t   data;
    int              num;
}vm_desc_t;

typedef struct _vm_msg_t{
    vm_desc_t       payload;
    vm_msg_type_e   type;
}vm_msg_t;


int vm_msg_put(vm_msg_t* msg, int size);

int vm_data_register(widget_desc_t * widget, char *props, char *name);

int vm_data_send_to_view(vm_msg_type_e msg_type, vm_desc_t *payload, int num);

bool vm_data_is_registered(widget_desc_t * widget, char *name);

int vm_data_duplicate(widget_desc_t *src, widget_desc_t *dest, char *name, int index);

int vm_props_set(widget_desc_t *widget, char *name, vm_data_type_t *value, int index);

int amp_view_model_init();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_VIEW_MODEL_H*/


#ifndef _VIEW_BIND_H
#define _VIEW_BIND_H

#include "view_model.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef int (*VIEW_SEND_TO_VM_F)(vm_msg_t *, int);
typedef int (*VIEW_DATA_BIND_VM_F)(widget_desc_t *, char *, char *);
typedef int (*VIEW_DATA_UNBIND_VM_F)(widget_desc_t *, char *, char *);

typedef struct _view_bind_proc_t {
    bool                  flag;
    VIEW_SEND_TO_VM_F     send;
    VIEW_DATA_BIND_VM_F   bind;
    VIEW_DATA_UNBIND_VM_F unbind;
}view_bind_proc_t;

int view_data_bind_init(VIEW_DATA_BIND_VM_F bind, VIEW_DATA_UNBIND_VM_F unbind, VIEW_SEND_TO_VM_F send);

int view_data_bind_deinit(void);

int view_data_send_to_vm(vm_msg_t *msg, int len);

int view_data_bind(widget_desc_t *widget, char *props, char* name);

int view_data_unbind(widget_desc_t *widget, char *props, char *name);

int view_msg_create(void);

int view_msg_proc(void);

int view_msg_put(vm_msg_t* msg, int size);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_VIEW_BIND_H*/


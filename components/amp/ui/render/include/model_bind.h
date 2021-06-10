#ifndef _MODEL_BIND_H
#define _MODEL_BIND_H

#include "view_model.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*MODEL_DATA_SEND_TO_VM_F)(vm_msg_t *, int);

typedef struct _model_bind_proc_t {
    bool                    flag;
    MODEL_DATA_SEND_TO_VM_F send;
}model_bind_proc_t;

int model_data_bind_init(MODEL_DATA_SEND_TO_VM_F send);

int model_data_bind_deinit(void);

int model_data_send_to_vm(vm_msg_t *msg, int len);

int model_eval_str(char *name, vm_data_type_t* data);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_VIEW_BIND_H*/


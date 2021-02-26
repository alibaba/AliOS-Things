/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef _UAI_ODLA_ADAPT_H_
#define _UAI_ODLA_ADAPT_H_

#include "uai_function.h"
#include "uai_load.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UAI_TENSOR_STATIC_MEM
#define UAI_TENSOR_PREFIX static
#else
#define UAI_TENSOR_PREFIX
#endif

#define UAI_ALLOC_TENSOR(tensor)                       \
    do {                                               \
        if (tensor == NULL) {                          \
            tensor = uai_malloc(sizeof(uai_tensor_s)); \
            if(tensor == NULL) {                       \
                UAI_LOGE("alloc tensor failed\n");     \
                return;                                \
            }                                          \
        }                                              \
    } while (0);

typedef int8_t _odla_Value;
typedef void (*odla_helper_func)(void *comp);

#ifndef UAI_ODLA_SUPPORT_FREE_MEM
typedef struct uai_mem_list{
    struct uai_mem_list *next;
    struct uai_mem_list *tail;
    void *mem_ptr;
}uai_mem_list_t;
#endif

struct _odla_Computation {
    int32_t layer_id;
    uai_tensor_s *input;
    uai_tensor_s *output;
    uai_model_quant_scale_data_t *model_quant_scale;
    odla_helper_func helper_func;
#ifndef UAI_ODLA_SUPPORT_FREE_MEM
    uai_mem_list_t *mem_list;
#endif
};

struct _odla_Session {
    struct _odla_Computation *comp;
};

#define KERNEL_SCALE(LAYER_ID) ((const uint32_t *)&((comp->model_quant_scale->scale)[comp->model_quant_scale->head.scale_offset[UAI_KERNRL_SCALE][LAYER_ID]]))
#define BIAS_SCALE(LAYER_ID)   ((const uint32_t *)&((comp->model_quant_scale->scale)[comp->model_quant_scale->head.scale_offset[UAI_BIAS_SCALE][LAYER_ID]]))
#define ACT_SCALE(LAYER_ID)    (((const uint32_t)((comp->model_quant_scale->scale)[comp->model_quant_scale->head.scale_offset[UAI_ACT_SCALE][LAYER_ID]])))
#define SCALE_SHIFT(LAYER_ID)  ((const uint8_t)(comp->model_quant_scale->head.scale_shift[LAYER_ID]))


uai_mem_list_t *uai_odla_init_memlist();
int uai_odla_add_memlist(uai_mem_list_t *memlist, void *ptr);
void uai_odla_free_memlist(uai_mem_list_t *memlist);

#ifdef __cplusplus
} /* C extern */
#endif


#endif
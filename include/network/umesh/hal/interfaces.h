/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_INTERFACES_H
#define UR_INTERFACES_H

#include "hal/interface_context.h"

void interface_init(void);
void interface_start(void);
void interface_stop(void);
void reset_network_context(void);
slist_t *get_network_contexts(void);
network_context_t *get_default_network_context(void);
network_context_t *get_sub_network_context(hal_context_t *hal);
network_context_t *get_hal_default_network_context(hal_context_t *hal);
network_context_t *get_network_context_by_meshnetid(uint16_t meshnetid, bool def);
slist_t *get_hal_contexts(void);
uint8_t get_hal_contexts_num(void);
hal_context_t *get_default_hal_context(void);
hal_context_t *get_hal_context(media_type_t type);

#endif  /* UR_INTERFACES_H */

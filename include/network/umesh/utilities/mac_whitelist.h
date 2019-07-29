/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UR_MAC_WHITELIST_H
#define UR_MAC_WHITELIST_H

#include "umesh_hal.h"

bool whitelist_is_enabled(void);
void whitelist_enable(void);
void whitelist_disable(void);
bool whitelist_is_enabled(void);
const whitelist_entry_t *whitelist_get_entries(void);
whitelist_entry_t *whitelist_add(const mac_address_t *address);
void whitelist_remove(const mac_address_t *address);
whitelist_entry_t *whitelist_find(const mac_address_t *address);
void whitelist_clear(void);
void whitelist_clear_constant_rssi(whitelist_entry_t *entry);
ur_error_t whitelist_get_constant_rssi(const whitelist_entry_t *entry,
                                       int8_t *rssi);
void whitelist_set_constant_rssi(whitelist_entry_t *entry, int8_t rssi);
bool is_whitelist_enabled(void);

#endif  /* UR_MAC_WHITELIST_H */

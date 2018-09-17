/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_80211_H
#define UMESH_80211_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <umesh_hal.h>

#define MESH_DATA_OFF 32

#define OFF_DST 4
#define OFF_SRC 10
#define OFF_BSS 16

typedef struct {
    uint8_t version: 2;
    uint8_t type: 2;
    uint8_t subtype: 4;
    uint8_t tods: 1;
    uint8_t fromds: 1;
    uint8_t morefrag: 1;
    uint8_t retry: 1;
    uint8_t pwrmgt: 1;
    uint8_t moredata: 1;
    uint8_t wep: 1;
    uint8_t order: 1;
} __attribute__((packed)) mac80211_fctl_t;

int umesh_80211_make_frame(umesh_hal_module_t *module, frame_t *frame, mac_address_t *dest, void *fpkt);
bool umesh_80211_filter_frame(umesh_hal_module_t *module, uint8_t *pkt, int count);

#ifdef __cplusplus
}
#endif

#endif /* UMESH_80211_H */

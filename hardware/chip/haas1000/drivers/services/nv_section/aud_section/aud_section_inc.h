/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __aud_section_inc_h__
#define __aud_section_inc_h__
#ifdef __cplusplus
extern "C" {
#endif

#include "aud_section.h"

int anccfg_loadfrom_audsec(const struct_anc_cfg *list[], const struct_anc_cfg *list_44p1k[], uint32_t count);

#ifdef __cplusplus
}
#endif
#endif


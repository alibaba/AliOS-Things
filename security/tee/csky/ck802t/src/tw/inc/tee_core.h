/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#ifndef _TEE_CORE_H_
#define _TEE_CORE_H_

#include "tee_types.h"

tee_stat_t tee_core_init(void);
void tee_core_cleanup(void);
tee_stat_t tee_core_proc(void *msg);

tee_stat_t tee_core_get_cur_uuid(TEE_UUID *uuid);

#endif /* _TEE_CORE_H_ */

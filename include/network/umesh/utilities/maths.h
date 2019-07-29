/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UMESH_MATHS_H
#define UMESH_MATHS_H

uint32_t umesh_get_random(void);
uint32_t umesh_get_hashword(const uint32_t *key, uint16_t length, uint32_t init);

#endif  /* UMESH_MATHS_H */

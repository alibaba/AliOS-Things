/*
 * Copyright (c) 2016-2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

int prng_init(void);
int prng_process(struct net_buf *buf);

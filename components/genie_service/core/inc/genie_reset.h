/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __GENIE_RESET_H__
#define __GENIE_RESET_H__

#define GENIE_RESET_BY_REPEAT_COUNTER 4
#define GENIE_RESET_BY_REPEAT_TIMEOUT (3 * 1000)

#define GENIE_RESET_WAIT_TIMEOUT (3 * 1000)

uint8_t genie_reset_get_hw_reset_flag(void);

genie_event_e genie_reset_do_sw_reset(void);

genie_event_e genie_reset_do_hw_reset(bool is_only_report);

int genie_reset_provision(void);

int genie_reset_init(void);

#endif

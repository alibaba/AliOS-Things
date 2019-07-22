/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef ULOCATION_LINKKIT_H
#define ULOCATION_LINKKIT_H

#ifdef __cplusplus
extern "C" {
#endif

char *compose_msg_to_report(void);

void parse_positioning_info(char *msg_from_cloud);

#ifdef __cplusplus
}
#endif

#endif /* ULOC_LINKKIT_H */


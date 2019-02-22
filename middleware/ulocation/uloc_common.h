/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _ULOC_LINKKIT_H_
#define _ULOC_LINKKIT_H_

#ifdef __cplusplus
extern "C" {
#endif

char * compose_msg_to_report();
void parse_positioning_info(char * msg_from_cloud);

#ifdef __cplusplus
}
#endif  /* extern C */

#endif /* _ULOC_LINKKIT_H_ */


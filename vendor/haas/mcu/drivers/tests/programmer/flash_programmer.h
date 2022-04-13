/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __FLASH_PROGRAMMER_H__
#define __FLASH_PROGRAMMER_H__

#ifdef __cplusplus
extern "C" {
#endif

int get_burn_data_buf(unsigned int *index);

int handle_data_buf(unsigned int index);

int free_data_buf(void);

int data_buf_in_use(void);

void programmer_loop(void);

int send_notif_msg(const unsigned char *buf, unsigned char len);

int send_reply(const unsigned char *payload, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif


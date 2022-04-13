/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_COMB_H__
#define __UVOICE_COMB_H__


int comb_add_amount(const char *amount, int index);
int comb_add_number(const char *number, int index);
int comb_add_file_source(const char *source, int index);
int comb_add_http_source(const char *source, int index);

int comb_alipay_number(double amount);
int comb_receipt_play(const char *req);
int comb_content_play(const char *req);
int comb_init(void);


#endif /* __UVOICE_COMB_H__ */

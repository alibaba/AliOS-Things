/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_COMB_H__
#define __UVOICE_COMB_H__


#define COMB_SOURCE_LIST_MAX	100
#define COMB_SOURCE_PATH_LEN	64

typedef struct {
	const char sources[COMB_SOURCE_LIST_MAX][COMB_SOURCE_PATH_LEN];
	int count;
	void (*callback)(void *args, int event);
	void *userdata;
} comb_source_info_t;

int comb_add_amount(const char *amount, int index);
int comb_add_number(const char *number, int index);
int comb_add_file_source_list(comb_source_info_t *info);
int comb_add_http_source(const char *source, int index);
int comb_clr_http_source(void);

int comb_alipay_number(double amount);
int comb_receipt_play(const char *req);
int comb_content_play(const char *req);
int comb_play_stop(void);
int comb_init(void);
int comb_deinit(void);


#endif /* __UVOICE_COMB_H__ */

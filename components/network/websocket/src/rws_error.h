/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __RWS_ERROR_H__
#define __RWS_ERROR_H__ 1

#include "rws_string.h"

struct rws_error_struct {
	int code;
	int http_error;
	char * description;
};

rws_error rws_error_create(void);

rws_error rws_error_new_code_descr(const int code, const char * description);

void rws_error_delete(rws_error error);

void rws_error_delete_clean(rws_error * error);

#endif

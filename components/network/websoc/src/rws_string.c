/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "rws_string.h"
#include "rws_memory.h"

char * rws_string_copy(const char * str) {
	return str ? rws_string_copy_len(str, strlen(str)) : NULL;
}

char * rws_string_copy_len(const char * str, const size_t len) {
	char * s = (str && len > 0) ? (char *)rws_malloc(len + 1) : NULL;
	if (s) {
		memcpy(s, str, len);
		s[len] = 0;
		return s;
	}
	return NULL;
}

void rws_string_delete(char * str) {
	rws_free(str);
}

void rws_string_delete_clean(char ** str) {
	if (str) {
		rws_free(*str);
		*str = NULL;
	}
}


/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "rws_memory.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void * rws_malloc(const size_t size) {
	if (size > 0) {
		void * mem = malloc(size);
		assert(mem);
		return mem;
	}
	return NULL;
}

void * rws_malloc_zero(const size_t size) {
	void * mem = rws_malloc(size);
	if (mem) {
		memset(mem, 0, size);
	}
	return mem;
}

void rws_free(void * mem) {
	if (mem) {
		free(mem);
	}
}

void rws_free_clean(void ** mem) {
	if (mem) {
		rws_free(*mem);
		*mem = NULL;
	}
}


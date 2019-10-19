/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */


#include "websoc/librws.h"
#include "rws_error.h"
#include "rws_string.h"
#include "rws_memory.h"


// private
rws_error rws_error_create(void) {
	return (rws_error)rws_malloc_zero(sizeof(struct rws_error_struct));
}

rws_error rws_error_new_code_descr(const int code, const char * description) {
	rws_error e = (rws_error)rws_malloc_zero(sizeof(struct rws_error_struct));
	e->code = code;
	e->description = rws_string_copy(description);
	return e;
}

void rws_error_delete(rws_error error) {
	if (error) {
		rws_string_delete(error->description);
		rws_free(error);
	}
}

void rws_error_delete_clean(rws_error * error) {
	if (error) {
		rws_error_delete(*error);
		*error = NULL;
	}
}

// public
int rws_error_get_code(rws_error error) {
	return error ? error->code : 0;
}

int rws_error_get_http_error(rws_error error) {
	return error ? error->http_error : 0;
}

const char * rws_error_get_description(rws_error error) {
	return error ? error->description : NULL;
}


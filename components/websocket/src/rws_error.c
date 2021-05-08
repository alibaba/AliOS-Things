/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "librws.h"
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


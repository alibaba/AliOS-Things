/*
 * Copyright (c) 2014, The protobuf-c authors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protobuf-c.h"

int
main(int argc, char **argv)
{
	printf("PACKAGE_VERSION = %s\n",
	       PACKAGE_VERSION);
	printf("PROTOBUF_C_VERSION = %s\n",
	       PROTOBUF_C_VERSION);
	printf("PROTOBUF_C_VERSION_NUMBER = %d\n",
	       PROTOBUF_C_VERSION_NUMBER);
	printf("protobuf_c_version() = %s\n",
	       protobuf_c_version());
	printf("protobuf_c_version_number() = %d\n",
	       protobuf_c_version_number());

	assert(strcmp(PACKAGE_VERSION, PROTOBUF_C_VERSION) == 0);
	assert(strcmp(PROTOBUF_C_VERSION, protobuf_c_version()) == 0);
	assert(PROTOBUF_C_VERSION_NUMBER == protobuf_c_version_number());

	return EXIT_SUCCESS;
}

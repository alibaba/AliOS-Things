/*
 * Copyright (c) Cameron Rich
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AXHTTP_H__
#define __AXHTTP_H__

// HTTPD extern struct and api
#include "web_proc.h"

#define BACKLOG 15
#define VERSION "1.0.0"
#define AXTLS_VERSION    "1.5.3"
#define HTTP_VERSION     "HTTP/1.1"

#define MAXPOSTDATASIZE                     30000 /* adjust for file uploadsize*/
#define MAXREADLENGTH                       4096  /* FF3=4096, IE7=8760 */
#define BLOCKSIZE                           4096

#define INITIAL_CONNECTION_SLOTS            6

#define STATE_WANT_TO_READ_HEAD             1
#define STATE_WANT_TO_SEND_HEAD             2
#define STATE_WANT_TO_READ_FILE             3
#define STATE_WANT_TO_SEND_FILE             4
#define STATE_DOING_DIR                     5

/*  1XX Informational responses
    2XX Successful responses
    3XX Redirection messages
    4XX Client error responses
    5XX Server error responses */
#define HTTPD_REQUEST_OK	200
#define HTTPD_NO_MODIFIED	304
#define HTTPD_BAD_REQUEST	400
#define HTTPD_UNAUTHORIZED	401
#define HTTPD_FORBIDDEN		403
#define HTTPD_NOT_FOUND		404
#define HTTPD_METHOD_NA		405
#define HTTPD_ERROR			500
#define HTTPD_NOT_IMPLEMENT	501

enum
{
    TYPE_GET,
    TYPE_HEAD,
    TYPE_POST
};

struct serverstruct
{
    struct serverstruct *next;
    int sd;
};

/* global prototypes */
extern struct serverstruct *servers;
extern struct connstruct *usedconns;
extern struct connstruct *freeconns;
extern const char * const server_version;

/* conn.c prototypes */
void removeconnection(struct connstruct *cn);

/* proc.c prototypes */
void procdodir(struct connstruct *cn);
void procreadhead(struct connstruct *cn);
void procsendhead(struct connstruct *cn);
void procreadfile(struct connstruct *cn);
void procsendfile(struct connstruct *cn);
void read_post_data(struct connstruct *cn);

/* tdate prototypes */
void tdate_init(void);
time_t tdate_parse(const char* str);

int httpd_main(void *argv);
void sigint_cleanup(int sig);

int special_read(int sd, void *buf, size_t count);
int special_write(int sd, const char *buf, size_t count);

#endif /* __AXHTTP_H__ */


/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef _AOS_HTTPC_H_
#define _AOS_HTTPC_H_

struct hostent *aos_httpc_get_host_by_name(const char *name);

int aos_httpc_socket_connect(uintptr_t fd, const struct sockaddr *name, socklen_t namelen);

#endif /* _AOS_HTTPC_H_ */


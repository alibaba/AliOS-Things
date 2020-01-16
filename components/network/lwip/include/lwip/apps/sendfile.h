/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
#ifndef LWIP_SENDFILE_H
#define LWIP_SENDFILE_H
ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count);
#endif /* LWIP_PING_H */

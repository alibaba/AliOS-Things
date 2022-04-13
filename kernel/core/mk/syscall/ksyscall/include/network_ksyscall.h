/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETWORK_KSYSCALL_H
#define NETWORK_KSYSCALL_H

int sys_network_accept_stub(void *arg);

int sys_network_bind_stub(void *arg);

int sys_network_shutdown_stub(void *arg);

int sys_network_getpeername_stub(void *arg);

int sys_network_getsockname_stub(void *arg);

int sys_network_getsockopt_stub(void *arg);

int sys_network_setsockopt_stub(void *arg);

int sys_network_close_stub(void *arg);

int sys_network_connect_stub(void *arg);

int sys_network_listen_stub(void *arg);

int sys_network_recv_stub(void *arg);

int sys_network_read_stub(void *arg);

int sys_network_recvfrom_stub(void *arg);

int sys_network_send_stub(void *arg);

int sys_network_sendmsg_stub(void *arg);

int sys_network_sendto_stub(void *arg);

int sys_network_socket(void *arg);

int sys_network_write_stub(void *arg);

int sys_network_writev_stub(void *arg);

int sys_network_select_stub(void *arg);

int sys_network_ioctl_stub(void *arg);

int sys_network_fcntl_stub(void *arg);

int sys_network_eventfd_stub(void *arg);

int sys_network_try_wakeup_stub(void *arg);

int sys_network_gethostbyname_stub(void *arg);

int sys_network_gethostbyname_r_stub(void *arg);

void sys_network_freeaddrinfo_stub(void *arg);

int sys_network_getaddrinfo_stub(void *arg);

#endif /* NETWORK_KSYSCALL_H */


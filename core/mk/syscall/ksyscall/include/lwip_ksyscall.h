/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef LWIP_KSYSCALL_H
#define LWIP_KSYSCALL_H

int sys_lwip_accept_stub(void *arg);

int sys_lwip_bind_stub(void *arg);

int sys_lwip_shutdown_stub(void *arg);

int sys_lwip_getpeername_stub (void *arg);

int sys_lwip_getsockname_stub (void *arg);

int sys_lwip_getsockopt_stub (void *arg);

int sys_lwip_setsockopt_stub (void *arg);

int sys_lwip_close_stub(void *arg);

int sys_lwip_connect_stub(void *arg);

int sys_lwip_listen_stub(void *arg);

int sys_lwip_recv_stub(void *arg);

int sys_lwip_read_stub(void *arg);

int sys_lwip_recvfrom_stub(void *arg);

int sys_lwip_send_stub(void *arg);

int sys_lwip_sendmsg_stub(void *arg);

int sys_lwip_sendto_stub(void *arg);

int sys_lwip_socket(void *arg);

int sys_lwip_write_stub(void *arg);

int sys_lwip_writev_stub(void *arg);

int sys_lwip_select_stub(void *arg);

int sys_lwip_ioctl_stub(void *arg);

int sys_lwip_fcntl_stub(void *arg);

int sys_lwip_eventfd_stub(void *arg);

#if LWIP_PACKET
int sys_lwip_try_wakeup_stub(void *arg);
#endif

int sys_lwip_gethostbyname_stub(void *arg);

int sys_lwip_gethostbyname_r_stub(void *arg);

void sys_lwip_freeaddrinfo_stub(void *arg);

int sys_lwip_getaddrinfo_stub(void *arg);

#endif /* LWIP_KSYSCALL_H */


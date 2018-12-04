/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
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

int sys_lwip_eventfd_stub(unsigned int initval, int flags);

#if LWIP_PACKET
int sys_lwip_try_wakeup_stub(int s, int rcvevent, int sendevent, int errevent);
#endif

#endif /* LWIP_KSYSCALL_H */


/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef DYN_SYSCALL_H
#define DYN_SYSCALL_H

/**
 * register syscall
 *  return value:
 *      0 : success
 *      -1: invalid syscall number
 *      -2: syscall is already registered
 */
int register_syscall(int syscall_no, void *func);

/**
 * unregister syscall
 *  return value:
 *      0 : success
 *      -1: invalid syscall number
 */
int unregister_syscall(int syscall_no);

#endif /* DYN_SYSCALL_H */


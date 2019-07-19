/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef SYSCALL_OPS_H
#define SYSCALL_OPS_H

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

#endif /* SYSCALL_OPS_H */


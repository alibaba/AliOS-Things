/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef THREAD_CALL_H
#define THREAD_CALL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>

#define OS_THREAD_MASTER_PATH "/tmp/master.socket"
#define OS_THREAD_SLAVE_PATH "/tmp/slave.socket"
#define OS_THREAD_PROC_ARGS_NUM    20
#define OS_THREAD_CALL_RESULT      1234
#define OS_THREAD_CALL_SIGNAL       (SIGRTMIN + 1)


typedef int (*thread_call_proc_fun)(int a,int b,int c,int d,int e,int f,int g,int h,int i,int j,int k,int l,int m);

typedef struct {
	thread_call_proc_fun thread_call_proc;
	int in_args[OS_THREAD_PROC_ARGS_NUM];   /*only support sizeof(arg) <= sizeof(int) */
	bool thread_fun_return;                 /*set the fun should have return value or not*/
	int thread_fun_result;                  /*if thread_fun_return==true, thread_fun_result is the return value*/
} os_thread_proc_args;


#endif /* THREAD_CALL_H */


/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#define _GNU_SOURCE

#include <sched.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <aos/aos.h>

#include <cpu_event.h>
#include <k_api.h>
#include <time.h>
#include <ucontext.h>
#include <sys/syscall.h>
#include "thread_call.h"


static sigset_t thread_call_sig_set;
int g_thread_master_fd = 0;

extern os_thread_proc_args g_os_threadcall_proc;
extern void thread_signal_proc(int signo, siginfo_t *si, void *ucontext);
extern void enter_signal(int signo);
extern void leave_signal(int signo);


/*called in alios main thread ,initialize master socket and signal proc*/
int thread_call_master_init(void)
{
    int ret;
    int fd;
    int reuse=0;
    struct sockaddr_un un;

    struct sigaction thread_call_assert_action = {
        .sa_flags = SA_SIGINFO | SA_RESTART,
        .sa_sigaction = thread_signal_proc,
    };

    sigemptyset(&thread_call_sig_set);
    sigaddset(&thread_call_sig_set, OS_THREAD_CALL_SIGNAL);
    thread_call_assert_action.sa_mask   = thread_call_sig_set;

    ret = sigaction(OS_THREAD_CALL_SIGNAL, &thread_call_assert_action, NULL);
    if(ret != 0)
    {
	    printf("sigaction fail\r\n");
		return -1;   
    }

    bzero(&un, sizeof(un));
	
	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)  
	{  
	    printf("cpu_init_hook socket fail\r\n");
		return -1;   
	}  

    unlink(OS_THREAD_MASTER_PATH);
    
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
        return -1;
    }

    un.sun_family = AF_UNIX;
    strcpy (&(un.sun_path[0]), OS_THREAD_MASTER_PATH);

    ret = bind(fd , (struct sockaddr *)&un, sizeof(struct sockaddr_un)); 
    if(ret < 0)
    {
        perror("bind");
        return -1;
    }
  
    g_thread_master_fd = fd;

    return 0;
  
}

/*master signal OS_THREAD_CALL_SIGNAL proc function*/
void thread_signal_proc(int signo, siginfo_t *si, void *ucontext)
{
    int ret;
	int proc_data = OS_THREAD_CALL_RESULT;
	struct sockaddr_un server_address;
	bzero(&server_address, sizeof(server_address));
	server_address.sun_family = AF_UNIX;

	strcpy (&(server_address.sun_path[0]), OS_THREAD_SLAVE_PATH);
    enter_signal(signo);

    krhino_intrpt_enter();

    if(g_os_threadcall_proc.thread_call_proc != NULL)
	{
		if(g_os_threadcall_proc.thread_fun_return)
		{
			g_os_threadcall_proc.thread_fun_result = g_os_threadcall_proc.thread_call_proc(g_os_threadcall_proc.in_args[0],
                g_os_threadcall_proc.in_args[1],g_os_threadcall_proc.in_args[2],g_os_threadcall_proc.in_args[3],g_os_threadcall_proc.in_args[4],
                g_os_threadcall_proc.in_args[5],g_os_threadcall_proc.in_args[6],g_os_threadcall_proc.in_args[7],g_os_threadcall_proc.in_args[8],
                g_os_threadcall_proc.in_args[9],g_os_threadcall_proc.in_args[10],g_os_threadcall_proc.in_args[11],g_os_threadcall_proc.in_args[12]);
		}
		else
		{
			g_os_threadcall_proc.thread_call_proc(g_os_threadcall_proc.in_args[0],
                g_os_threadcall_proc.in_args[1],g_os_threadcall_proc.in_args[2],g_os_threadcall_proc.in_args[3],g_os_threadcall_proc.in_args[4],
                g_os_threadcall_proc.in_args[5],g_os_threadcall_proc.in_args[6],g_os_threadcall_proc.in_args[7],g_os_threadcall_proc.in_args[8],
                g_os_threadcall_proc.in_args[9],g_os_threadcall_proc.in_args[10],g_os_threadcall_proc.in_args[11],g_os_threadcall_proc.in_args[12]);
		}

	}
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(server_address.sun_path) + 1;
	ret = sendto(g_thread_master_fd, &proc_data, sizeof(int), 0, &server_address, len);
    if(ret < 0 )
    {
	    printf("thread_signal_proc sendto fail,g_client_linkkit_fd:%d\r\n",g_thread_master_fd);
        perror("sendto error");
    }

    krhino_intrpt_exit();

    leave_signal(signo);
}



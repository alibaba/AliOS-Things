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


os_thread_proc_args g_os_threadcall_proc = {0};
int g_thread_slave_fd = 0;
extern int g_thread_master_fd;
extern void cpu_call_signal(uint8_t cpu_num,int sig);

/*shoule be called in another user thread ,and to run function in main thread;
*/
int thread_call_slave_init(void)
{
	int fd;
	int flags;
	int server_len;
    int ret;
    int reuse = 0;
	struct sockaddr_un server_address;
	bzero(&server_address, sizeof(server_address));

	if ((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
	    printf("thread_call_slave_init socket fail\r\n");
		return -1;
	}
    unlink(OS_THREAD_SLAVE_PATH);

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        perror("setsockopet error\n");
        return -1;
    }

    // get file flags
	flags  = fcntl(fd, F_GETFL, 0);
    // set to non-block mode
	fcntl(fd, F_SETFL, flags & (~O_NONBLOCK));

	server_address.sun_family = AF_UNIX;
	strcpy (&(server_address.sun_path[0]), OS_THREAD_SLAVE_PATH);

	server_len = sizeof (server_address);
	ret = bind(fd, (struct sockaddr *)&server_address, server_len);
    if(ret < 0)
    {
        extern int errno;
	    printf("thread_call_slave_init bind fail,errno:%d\r\n",errno);
        perror("bind error");
        return -1;
    }

	g_thread_slave_fd = fd;

    return 0;
}



int thread_call_send_proc(os_thread_proc_args * proc_args)
{
	int len;
	int buffer[100] = {0};
	struct sockaddr_un addr;
	int addr_len = sizeof(struct sockaddr_un);

	bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
	strcpy (&(addr.sun_path[0]), OS_THREAD_SLAVE_PATH);

	memcpy(&g_os_threadcall_proc,proc_args,sizeof(os_thread_proc_args));
	g_os_threadcall_proc.thread_fun_result = 0;

    cpu_call_signal(0,OS_THREAD_CALL_SIGNAL);

	len = recvfrom(g_thread_slave_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr ,&addr_len);
	if(len <= 0 || buffer[0] != OS_THREAD_CALL_RESULT)
	{
	    extern int errno;
	    printf("thread_call_send_proc recvfrom fail,buffer:%d,errno:%d,g_server_linkkit_fd:%d\r\n",buffer[0],errno,g_thread_slave_fd);
        perror("recvfrom error");
		return -1;
	}

	return g_os_threadcall_proc.thread_fun_result;

}




/*call example*/

/*thread_call_master_init should be called in alios main thread ,initialize master socket and signal proc
and to thread_call_slave_init should be called in slave thread.
for example:
-----------------begin-------------------
pthread_t pthreadtmp;
extern int thread_call_master_init(void);
extern void *os_thread_init_example(void *arg);
thread_call_master_init();
pthread_create(&pthreadtmp, NULL, (void *)os_thread_init_example, 0);
-----------------end---------------------

*/


int thread_call_example(int i)
{
	printf("thread_call_example arg should be 10; trully in :%d\r\n",i);

	return 0xff;
}

int _thread_call_example(int i)
{
	os_thread_proc_args  args;
	args.thread_call_proc = (thread_call_proc_fun)thread_call_example;
	args.in_args[0] = i;
	args.thread_fun_return = true;

	return thread_call_send_proc(&args);
}

void *os_thread_init_example(void *arg)
{
    int ret;
	thread_call_slave_init();
    
    ret = _thread_call_example(10);
	printf("_thread_call_example should return 0xff; trully out :0x%x\r\n",ret);
	
	return 0;
}


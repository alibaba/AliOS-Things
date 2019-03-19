/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <u_api.h>
#include <task.h>
#include <errnox.h>

#define KSTACK_SIZE  	0x100

int task_create(task_t *task, size_t stack_size, task_func_t entry, void *arg)
{
	int *stack;
	kstat_t stat;

	if (NULL == task) {
		return -EINVAL;
	}

	if (entry == NULL) {
		return -EINVAL;
	}

	stack = (int*)malloc(sizeof(stack_size));
	if (NULL == stack) {
		return -ENOMEM;
	}

	stat = krhino_utask_create((ktask_t**)task, "app_task", (void*)arg, AOS_DEFAULT_APP_PRI,
		                       (tick_t)0, (cpu_stack_t*)stack, KSTACK_SIZE, stack_size/sizeof(int),
		                       entry, 1);

	if (stat != RHINO_SUCCESS) {
		free(stack);
		return -TOERRNO(stat);
	}

	return 0;
}

int task_exit(void)
{
	int ret;

	ret = krhino_utask_del(NULL);
	if (ret != RHINO_SUCCESS) {
		return -TOERRNO(ret);
	}

	return 0;
}


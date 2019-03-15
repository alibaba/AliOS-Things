/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kernel/os/os.h"
#include "sys/list.h"
#include "sys/param.h"
#include "sys/defs.h"
#include "container.h"


#define CONTAINER_DEBUG(fmt, arg...)	//printf("[Container debug] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define CONTAINER_ALERT(fmt, arg...)	printf("[Container alert] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define CONTAINER_ERROR(fmt, arg...)	printf("[Container error] <%s : %d> " fmt "\n", __func__, __LINE__, ##arg)
#define CONTAINER_NOWAY()				printf("[Container should not be here] <%s : %d> \n", __func__, __LINE__)
#define CONTAINER_NOTSUPPORT() 			CONTAINER_ALERT("not support command")


typedef struct list_node
{
	struct list_head node;
	uint32_t arg;
} list_node;

typedef struct sorted_list
{
	container_base base;
	OS_Semaphore_t sem;
	OS_Mutex_t lock;
	struct list_head head;
	struct list_head pool;
	list_node *poolBuf;
	int (*compare)(uint32_t newArg, uint32_t oldArg);
} sorted_list;

typedef struct sorted_list_config
{
	uint32_t size;
	int (*compare)(uint32_t oldArg, uint32_t newArg);
} sorted_list_config;

static int sorted_list_init(struct container_base *base, uint32_t config)
{
	OS_Status ret1 = OS_FAIL;
	OS_Status ret2 = OS_FAIL;
	list_node *vec = NULL;
	sorted_list_config *cfg = (sorted_list_config *)config;
	sorted_list *impl = __containerof(base, sorted_list, base);

	INIT_LIST_HEAD(&impl->head);
	INIT_LIST_HEAD(&impl->pool);
	impl->base.size = cfg->size;
	impl->compare = cfg->compare;

	ret1 = OS_SemaphoreCreateBinary(&impl->sem);
	if (ret1 != OS_OK)
		goto failed;

	ret2 = OS_MutexCreate(&impl->lock);
	if (ret2 != OS_OK)
		goto failed;

	vec = malloc(sizeof(list_node) * impl->base.size);
	if (vec == NULL)
		goto failed;
	memset(vec, 0, sizeof(list_node) * impl->base.size);
	impl->poolBuf = vec;

	for(int i = impl->base.size; i-- > 0; )
	{
		INIT_LIST_HEAD(&vec[i].node);
		list_add_tail(&vec[i].node, &impl->pool);
	}

	return 0;

failed:
	if (vec != NULL)
		free(vec);
	if (ret1 == OS_OK)
		OS_SemaphoreDelete(&impl->sem);
	if (ret2 == OS_OK)
		OS_MutexDelete(&impl->lock);
	if (impl != NULL)
		free(impl);

	return -1;
}

static int sorted_list_deinit(struct container_base *base)
{
	sorted_list *impl = __containerof(base, sorted_list, base);

	/* TODO: flush the node left */

	OS_SemaphoreDelete(&impl->sem);
	OS_MutexDelete(&impl->lock);
	if (impl->poolBuf != NULL)
		free(impl->poolBuf);
	if (impl != NULL)
		free(impl);

	return 0;
}

static int sorted_list_control(struct container_base *base, uint32_t cmd, uint32_t arg)
{
	sorted_list *impl = __containerof(base, sorted_list, base);
	/* TODO: tbc... */
	(void)impl;
	return -1;
}

static int sorted_list_push(struct container_base *base, uint32_t arg, uint32_t timeout)
{
#define BLOCK_WAIT_EACHTIME (3)
	sorted_list *impl = __containerof(base, sorted_list, base);
	list_node *node = NULL;
	list_node *tmp = NULL;

	OS_MutexLock(&impl->lock, -1);	/* TODO: OS should move to event_queue */

	/* 1. get 1 node from pool */
	while ((node = list_first_entry_or_null(&impl->pool, list_node, node)) == NULL)
	{
		OS_MutexUnlock(&impl->lock);
		OS_MSleep(BLOCK_WAIT_EACHTIME);
		if (timeout <= BLOCK_WAIT_EACHTIME) {
			CONTAINER_ALERT("list full and timeout");

			return -1;
		}
		OS_MutexLock(&impl->lock, -1);
		timeout -= BLOCK_WAIT_EACHTIME;
	}
	node->arg = arg;

	CONTAINER_DEBUG("get a node from pool");

	/* 2. find a proper place and insert it */
	list_for_each_entry(tmp, &impl->head, node)
	{
		if (impl->compare(arg, tmp->arg) >= 0) {
			tmp = list_prev_entry(tmp, node);
			break;
		}
	}
	list_add(&node->node, &tmp->node);

	CONTAINER_DEBUG("insert node to list");

	/* 3. release sem to pop */
	OS_SemaphoreRelease(&impl->sem);

	OS_MutexUnlock(&impl->lock);
	return 0;
}

static int sorted_list_pop(struct container_base *base, uint32_t *arg, uint32_t timeout)
{
	sorted_list *impl = __containerof(base, sorted_list, base);
	list_node *node = NULL;
	OS_Status ret;

	ret = OS_SemaphoreWait(&impl->sem, timeout);
	if (ret != OS_OK)
		return -1;

	OS_MutexLock(&impl->lock, -1);

	/* 1. get node from list */
	node = list_first_entry_or_null(&impl->head, list_node, node);
	if (node == NULL)
	{
		CONTAINER_ERROR("list empty but sem released!");
		OS_MutexUnlock(&impl->lock);
		return -2;
	}
	*arg = node->arg;

	CONTAINER_DEBUG("get a node from list");

	/* 2. remove the node from this and push it to pool */
	list_del(&node->node);
	list_add_tail(&node->node, &impl->head);

	CONTAINER_DEBUG("remove a node from list and push it to pool");

	OS_MutexUnlock(&impl->lock);
	return 0;
}

container_base *sorted_list_create(uint32_t size, int (*compare)(uint32_t newArg, uint32_t oldArg))
{
	sorted_list *impl = malloc(sizeof(*impl));
	if (impl == NULL)
		return NULL;
	memset(impl, 0, sizeof(*impl));

	sorted_list_config cfg;
	cfg.compare = compare;
	cfg.size = size;

	impl->base.control = sorted_list_control;
	impl->base.deinit = sorted_list_deinit;
	impl->base.pop = sorted_list_pop;
	impl->base.push = sorted_list_push;

	if (sorted_list_init(&impl->base, (uint32_t)&cfg) != 0)
	{
		CONTAINER_ERROR("init failed");
		free(impl);
		return NULL;
	}

	return &impl->base;
}

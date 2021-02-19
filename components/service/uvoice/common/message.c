/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_os.h"

#include "uvoice_common.h"
#include "uvoice_message.h"


int uvoice_msgqueue_recv(uvoice_msgqueue_t *msgqueue, int type, void *msg, int size, int timeout)
{
	int flag = timeout == OS_WAIT_FOREVER ? 0 : IPC_NOWAIT;
	return msgrcv(msgqueue->qid, msg, size, type, flag);
}

int uvoice_msgqueue_send(uvoice_msgqueue_t *msgqueue, void *msg, int size, int block)
{
	int flag = block ? 0 : IPC_NOWAIT;
	return msgsnd(msgqueue->qid, msg, size, flag);
}

uvoice_msgqueue_t *uvoice_msgqueue_create(char *filename, int id)
{
	uvoice_msgqueue_t *msgqueue;
	key_t key;

	if (!filename) {
		M_LOGE("filename null !\n");
		return -1;
	}

	if (id < 0 || id > 255) {
		M_LOGE("id %d invalid !\n", id);
		return -1;
	}

	msgqueue = snd_zalloc(sizeof(uvoice_msgqueue_t), AFM_EXTN);
	if (!msgqueue) {
		M_LOGE("alloc msgqueue failed !\n");
		return NULL;
	}
	msgqueue->id = id;

	key = ftok(filename, id);
	if (key == -1) {
		M_LOGE("generate key failed !\n");
		snd_free(msgqueue);
		return NULL;
	}

	msgqueue->id = msgget(key, IPC_CREAT|0666);
	if (msgqueue->id == -1) {
		M_LOGE("create msgqueue failed !\n");
		snd_free(msgqueue);
		return NULL;
	}

	msgqueue->lock = os_mutex_new();
	msgqueue->recv_sem = os_sem_new(0);

	return msgqueue;
}


int uvoice_msgqueue_release(uvoice_msgqueue_t *msgqueue)
{
	if (!msgqueue) {
		M_LOGE("msgqueue null !\n");
		return NULL;
	}

	os_sem_free(msgqueue->recv_sem);
	os_mutex_free(msgqueue->lock);

	snd_free(msgqueue);
	return 0;
}


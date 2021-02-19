/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_MESSAGE_H__
#define __UVOICE_MESSAGE_H__


typedef struct {
	char *filename;
	int id;
	int qid;
	int recv_waiting;
	os_sem_t recv_sem;
	os_mutex_t lock;
} uvoice_msgqueue_t;

int uvoice_msgqueue_recv(uvoice_msgqueue_t *msgqueue, int type, void *msg, int size, int timeout);
int uvoice_msgqueue_send(uvoice_msgqueue_t *msgqueue, void *msg, int size, int block);
uvoice_msgqueue_t *uvoice_msgqueue_create(char *filename, int id);
int uvoice_msgqueue_release(uvoice_msgqueue_t *msgqueue);


#endif /* __UVOICE_MESSAGE_H__ */

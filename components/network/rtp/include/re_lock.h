/**
 * @file re_lock.h  Interface to locking functions
 *
 * Copyright (C) 2010 Creytiv.com
 */


struct lock;

int  lock_alloc(struct lock **lp);

void lock_read_get(struct lock *l);
void lock_write_get(struct lock *l);

int  lock_read_try(struct lock *l);
int  lock_write_try(struct lock *l);

void lock_rel(struct lock *l);

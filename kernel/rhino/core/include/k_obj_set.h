/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef K_OBJ_SET_H
#define K_OBJ_SET_H

#if (RHINO_CONFIG_KOBJ_SET > 0)
typedef void (*kobj_set_notify)(struct blk_obj *obj, struct kobj_set *handle);

typedef struct kobj_set {
    void           *docker;
    kqueue_t        queue;
    kobj_set_notify notify;
} kobj_set_t;

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * This function will create a kobj_set
 * @param[out] handle   pointer to the pointer of kobj_set_t(the space is allocated and freed by kernel)
 * @param[in]  name     name of the kobj_set
 * @param[in]  msg_num  num of the kobj_set's contained notify msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_kobj_set_dyn_create(kobj_set_t **handle, const name_t *name,
                                   size_t msg_num);

/**
 * This function will del a kobj_set
 * @param[in]  handle   pointer to the pointer of kobj_set_t(the space is allocated and freed by kernel)
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_kobj_set_dyn_del(kobj_set_t *handle);
#endif

/**
 * This function will create a kobj_set
 * @param[in]  handle   pointer to the kobj_set_t(the space is provided by user)
 * @param[in]  name     name of the queue
 * @param[in]  start    start address of the kobj_set internal space
 * @param[in]  msg_num  num of the kobj_set's contained notify msg
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_kobj_set_create(kobj_set_t *handle, const name_t *name,
                               void **start, size_t msg_num);

/**
 * This function will del a kobj_set
 * @param[in]  handle    pointer to the kobj_set_t(the space is provided by user)
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_kobj_set_del(kobj_set_t *handle);

/**
 * This function will insert a kobj to the set
 * @param[in]  obj      pointer to the kobj,such as semphore,queue,buf queue,etc.
                        for example,you can cast a semphore pointer to blk_obj_t *.
 * @param[in]  handle   pointer to the kobj_set_t
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_kobj_set_insert(blk_obj_t *obj, kobj_set_t *handle);

/**
 * This function will rm a kobj from the set
 * @param[in]  obj      pointer to the kobj,such as semphore,queue,buf queue,etc.
                        for example,you can cast a semphore pointer to blk_obj_t *.
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_kobj_set_rm(blk_obj_t *obj);

/**
 * This function will block until any kobj in the set unlock.
 * @param[in]  handle   pointer to the kobj_set_t
 * @param[out] obj      pointer to the pointer of the kobj,such as semphore,queue,buf queue,etc.
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_kobj_select(kobj_set_t *handle, blk_obj_t **obj, tick_t ticks);
#endif

#endif /*K_OBJ_SET_H*/


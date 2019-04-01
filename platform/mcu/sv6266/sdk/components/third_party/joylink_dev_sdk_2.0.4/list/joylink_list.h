#ifndef __JD_LIST_H_
#define __JD_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "joylink.h"
#include "yc_list.h"

typedef int LIST_BOOL_t;

#define LIST_BOOL_FALSE     (0)
#define LIST_BOOL_TRUE      (1)

#define mem_get(type, n) calloc(sizeof(type),n)
#define mem_put(ptr)  \
    do{               \
        free(ptr);    \
        (ptr) = NULL; \
    }while(0)

typedef enum __list_ret{
    E_LIST_RET_OK = 0,
    E_LIST_ERROR = -1,
    E_LIST_ERROR_PARAM_INVAILD = -2
}E_LIST_RET;

typedef struct _id_node	
{
	LHead_t	list;
	int32_t	id;
    int32_t len;
    void *data;
}IDNode_t;

//===========================

/**
 * brief: 
 *
 * @Param: id
 * @Param: lhead
 *
 * @Returns: 
 */
int32_t is_joylink_idnode_exist(int32_t id, LHead_t *lhead);

/**
 *1 create the IDNode 
 *2 data must be the malloc point 
 */
int32_t joylink_idnode_add(int32_t id, void *data, int32_t data_len, LHead_t *lhead);

/**
 *1 free node->data
 *2 free node 
 */
int32_t joylink_idnode_del(int32_t id, LHead_t *lhead);

/**
 * brief: 
 *
 * @Param: size
 * @Param: pids
 * @Param: lhead
 *
 * @Returns: 
 */
int joylink_ids_get(int32_t size, int32_t *pids, LHead_t *lhead);

/**
 * brief: 
 *
 * @Param: id
 * @Param: lhead
 *
 * @Returns: 
 */
IDNode_t * joylink_idnode_get_by_id(int32_t id, LHead_t *lhead);

/**
 * brief: 
 *
 * @Param: lhead
 *
 * @Returns: 
 */
int32_t joylink_idnode_list_clear(LHead_t *lhead);

/**
 * brief: 
 *
 * @Param: lhead
 */
void joylink_idnode_list_debugp(LHead_t *lhead);
#endif

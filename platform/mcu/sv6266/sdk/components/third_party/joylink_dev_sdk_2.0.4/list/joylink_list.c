/* --------------------------------------------------
 * @file: jd_list.c
 * @brief: 
 * @version: 1.0
 * --------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "joylink_list.h"

/**
 * brief: 
 *
 * @Param: id
 * @Param: lhead
 *
 * @Returns: 
 */
int32_t
is_joylink_idnode_exist(int32_t id, LHead_t *lhead)
{
    if(NULL == lhead){
        return LIST_BOOL_FALSE;
    }
	IDNode_t *pos, *idNode;
    LIST_BOOL_t exist_flag = LIST_BOOL_FALSE;

    list_for_each_entry_safe(idNode, pos, lhead, list){
        if(idNode != NULL && idNode->id == id){
            exist_flag = LIST_BOOL_TRUE;
			break;
        }
    }

    return exist_flag;
}

/**
 *1 create the IDNode 
 *2 data must be the malloc point 
 */
int32_t
joylink_idnode_add(int32_t id, void *data, int32_t data_len, LHead_t *lhead)
{
    if(NULL == lhead || NULL == data){
        return E_LIST_ERROR_PARAM_INVAILD;
    }
	IDNode_t *idNode;

    idNode = (IDNode_t *)mem_get(IDNode_t, 1);
    if(NULL == idNode){
        return E_LIST_ERROR;
    }

    idNode->data = (char*)mem_get(char, data_len);
    if(NULL == idNode->data){
        mem_put(idNode);
        return E_LIST_ERROR;
    }

    INIT_LIST_HEAD(&idNode->list);

    idNode->id = id;
    memcpy(idNode->data, data, data_len);
    idNode->len = data_len;

    yc_list_add_tail(&idNode->list, lhead);

    return E_LIST_RET_OK;
}

/**
 *1 free node->data
 *2 free node 
 */
int32_t 
joylink_idnode_del(int32_t id, LHead_t *lhead)
{
    if(NULL == lhead){
        return E_LIST_ERROR_PARAM_INVAILD;
    }
    int32_t ret =E_LIST_ERROR;
	IDNode_t *pos, *idNode;
    
    list_for_each_entry_safe(idNode, pos, lhead, list){
        if(idNode != NULL && idNode->id == id && NULL == idNode->data){
            yc_list_del(&idNode->list);
            if(NULL != idNode->data){
                mem_put(idNode->data);
            }
            mem_put(idNode);
            ret = E_LIST_RET_OK;
            break;
        }
    }

    return ret;
}

/**
 * brief: 
 *
 * @Param: size
 * @Param: pids
 * @Param: lhead
 *
 * @Returns: 
 */
int 
joylink_ids_get(int32_t size, int32_t *pids, LHead_t *lhead)
{
    if(NULL == pids || NULL == lhead){
        return E_LIST_ERROR_PARAM_INVAILD;
    }
	IDNode_t *pos, *idNode;
    int32_t node_size, count = 0;

    node_size = yc_list_size(lhead);
    if(size < node_size){
        return E_LIST_ERROR_PARAM_INVAILD;
    }

    list_for_each_entry_safe(idNode, pos, lhead, list){
        pids[count++] = idNode->id;
    }

    return count;
}

/**
 * brief: 
 *
 * @Param: id
 * @Param: lhead
 *
 * @Returns: 
 */
IDNode_t *
joylink_idnode_get_by_id(int32_t id, LHead_t *lhead)
{
    if(NULL == lhead){
        return NULL;
    }
	IDNode_t *pos, *idNode;
	IDNode_t *ret = NULL;
    
    list_for_each_entry_safe(idNode, pos, lhead, list){
        if(idNode->id == id){
            ret = idNode;
            break;
        }
    }

    return ret;
}

/**
 *1 create the IDNode 
 *2 data must be the malloc point 
 */
int32_t 
joylink_idnode_list_clear(LHead_t *lhead)
{
    if(NULL == lhead){
        return E_LIST_ERROR_PARAM_INVAILD;
    }
	IDNode_t *n;
    IDNode_t *idNode;
    int32_t count = 0;

    list_for_each_entry_safe(idNode, n, lhead, list){
        yc_list_del(&idNode->list);
        if(NULL != idNode->data){
            mem_put(idNode->data);
        }
        mem_put(idNode);
        count ++;
    }

    return count; 
}

/**
 * brief: 
 *
 * @Param: lhead
 */
void
joylink_idnode_list_debugp(LHead_t *lhead)
{
    printf("--------------------\n");
	LHead_t	*pos;
	IDNode_t *idNode; 
    list_for_each(pos,lhead){
        idNode = list_entry(pos, IDNode_t , list);
        printf("----%d\n", idNode->id);
    }
    printf("--------------------\n");

}

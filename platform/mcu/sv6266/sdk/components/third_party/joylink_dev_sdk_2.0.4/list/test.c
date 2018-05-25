#include <stdio.h>

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include <unistd.h>

#include "joylink.h"
#include "joylink_list.h"

typedef struct  __join_packet{
    char name[256];
    int current_count;
    int total_len;
    int id;
    int is_run;
    LHead_t lhead;
    JLPacketHead_t phead;
}JLJoinPacket_t;

#define JL_MAX_JOIN_PKG_ARRARY              (20)
#define JL_MAX_JOIN_PKG_TOTAL_LEN           (20 * 1024)

#define JL_JOIN_PKG_RUN                     (1)
#define JL_JOIN_PKG_STOP                    (0)

JLJoinPacket_t _g_jps[JL_MAX_JOIN_PKG_ARRARY];

/**
 * brief: get _g_jps index by name id. 
 *
 * @Param: name
 * @Param: id
 *
 * @Returns: 1 index of _g_jps
 *           2 E_RET_ERROR_NO_EXIST
 *           3 E_RET_ERROR_PARAM_INVAID
 */
static int
joylink_join_pkg_get_index_by_name_id(char *name, int id)
{
    if(NULL == name){
        return E_RET_ERROR_PARAM_INVAID;
    }
    int ret = E_RET_ERROR_NO_EXIST;
    int i; 
    for(i = 0; i < JL_MAX_JOIN_PKG_ARRARY; i++){
        if(_g_jps[i].is_run == JL_JOIN_PKG_RUN){
            if(!strcmp(name, _g_jps[i].name) 
                    && id == _g_jps[i].id){
                ret = i;
                break;
            }
        }
    }

    return ret;
}

/**
 * brief: 
 * @Returns: 1 index of _g_jps
 *           2 E_RET_ERROR_PKG_BREAK_ARRAY
 */
static int
joylink_join_pkg_get_idle_index()
{
    int ret = E_RET_ERROR_PKG_BREAK_ARRAY;
    int i; 
    for(i = 0; i < JL_MAX_JOIN_PKG_ARRARY; i++){
        if(_g_jps[i].is_run != JL_JOIN_PKG_RUN){
            ret = i;
            break;
        }
    }
    return ret;
}

/**
 * brief: 1 find a idle index 
 *        2 save JLPacketHead_t 
 *        3 init JLJoinPacket_t
 *          must care name, id, is_run, total_len, lhead, lphead. 
 *
 * @Returns: 1 E_RET_ERROR_PARAM_INVAID
             2 E_RET_ERROR_PKG_NUM_BREAK_MAX
             3 E_RET_ERROR_PKG_BREAK_ARRAY
             4 index of _g_jps
 */
static int
joylink_join_pkg_cache_new_node(char *name, int id, JLPacketHead_t *phead, char *data, int len)
{
    if(NULL == name || NULL == phead
            || NULL == data){
        return E_RET_ERROR_PARAM_INVAID;
    }
    int ret;

    JLJoinPacket_t *jp;
    if(0 <= (ret = joylink_join_pkg_get_idle_index())){
        jp = &_g_jps[ret];
        INIT_LIST_HEAD(&jp->lhead);
        if(E_LIST_RET_OK == joylink_idnode_add(phead->index, data, len, &jp->lhead)){
            strcpy(jp->name, name);
            jp->id = id;
            jp->is_run = JL_JOIN_PKG_RUN;
            memcpy(&jp->phead, phead, sizeof(JLPacketHead_t));
            jp->total_len  = len;
        }else{
            ret = E_RET_ERROR;
        }
    }

    return ret;
}

/**
 * brief: 1 find index by name and id
 *        2 if no exist ,cache idle node, add data
 *        3 if exist add data
 *
 * @Returns: 1 E_RET_ERROR_PARAM_INVAID
             2 E_RET_ERROR_PKG_NUM_BREAK_MAX
             3 E_RET_ERROR_PKG_BREAK_ARRAY
             4 E_RET_OK 
             5 E_RET_ALL_DATA_HIT
 */
int
joylink_join_pkg_add_data(char *name, int id, JLPacketHead_t *phead, char *data, int len)
{
    if(NULL == name || NULL == phead
            || NULL == data){
        return E_RET_ERROR_PARAM_INVAID;
    }
    int ret = joylink_join_pkg_get_index_by_name_id(name, id);
    JLJoinPacket_t *jp = NULL;
    if(ret >= 0){
        jp = &_g_jps[ret];
        if(E_LIST_RET_OK == joylink_idnode_add(phead->index, data, len, &jp->lhead)){
            jp->total_len += len;
        }else{
            ret = E_RET_ERROR;
        }
    }else if(ret == E_RET_ERROR_NO_EXIST){
        ret = joylink_join_pkg_cache_new_node(name, id, phead, data, len);
        jp = &_g_jps[ret];
    }
    
    if(ret >= 0){
        if(jp->current_count == jp->phead.total){
            ret = E_RET_ALL_DATA_HIT;
        }else{
            ret = E_RET_OK;
        }
    }

    return ret;
}

/**
 * brief: 1 malloc a buffer 
 *        2 find all data by 1 to phead->total 
 *        3 copy all data to buffer
 *
 * @Returns: 1 NULL 
 *           2 A malloc buffer point, rember to free it.
 */
char *
joylink_join_pkg_join_data(char *name, int id, int *out_len)
{
    if(NULL == name){ 
        return NULL;
    }
    char *p_data = NULL;
    int offset = 0;
    JLJoinPacket_t *jp;
    IDNode_t *pkg;
    int i;
    int ret = joylink_join_pkg_get_index_by_name_id(name, id);

    if(ret >= 0){
        jp = &_g_jps[ret];
        p_data = (char *)malloc(jp->total_len);
        *out_len = jp->total_len;
        if(NULL !=  p_data){
            for(i = 1; i <= jp->phead.total; i ++){
                if(NULL == (pkg = joylink_idnode_get_by_id(i, &jp->lhead))){
                    free(p_data);
                    p_data = NULL;
                    goto RET;
                }else{
                    memcpy(p_data + offset, pkg->data, pkg->len); 
                    offset += pkg->len;
                }
            }
        }
    }

RET:
    return p_data;
}

/**
 * brief: 1 clean all pkg data
 *        2 set JLJoinPacket_t as 0 
 *
 * @Returns: 1 E_RET_ERROR_PARAM_INVAID 
 *           2 E_RET_ERROR 
 *           3 E_RET_OK
 */
int
joylink_join_pkg_clean_node_by_name_id(char *name, int id)
{
    if(NULL == name){ 
        return E_RET_ERROR_PARAM_INVAID;
    }
    JLJoinPacket_t *jp;
    int ret = joylink_join_pkg_get_index_by_name_id(name, id);
    if(ret >= 0){
        jp = &_g_jps[ret];
        joylink_idnode_list_clear(&jp->lhead);
        bzero(jp, sizeof(JLJoinPacket_t));
        ret = E_RET_OK;
    }

    return ret;
}

void
test_join_data()
{
    char buffer[1024 * 20] = {0};
    FILE *fp = fopen("./tt.txt", "r");
    int red = fread(buffer, 1, sizeof(buffer), fp);
    /*printf("-->len:%d buffer:%s\n", red, buffer);*/

    char *name = "test 1";
    char *name_2 = "test 2";
    int id = 1;
    JLPacketHead_t phead;
    bzero(&phead, sizeof(JLPacketHead_t));

    phead.total = (int)(red/1000) + (red%1000 ? 1:0);

    int i; 
    int len;
    int offset = 0;
    for(i = 1; i <= phead.total; i ++){
        phead.index = i;
        if(i == phead.total){
            len = red%1000;
        }else{
            len = 1000;
        }

        joylink_join_pkg_add_data(name, id, &phead, buffer + offset, len);
        offset += len;
    }
    fclose(fp);

    char * p_data = joylink_join_pkg_join_data(name, id, &len);
    printf("---:org len:%d: len:%d after buff:%s\n", red, len, p_data);

    bzero(buffer, sizeof(buffer));
    fp = fopen("./t2.txt", "r");
    red = fread(buffer, 1, sizeof(buffer), fp);
   
    offset = 0;
    len = 0;
    for(i = 1; i <= phead.total; i ++){
        phead.index = i;
        if(i == phead.total){
            len = red%1000;
        }else{
            len = 1000;
        }

        offset += len;
        if(E_RET_ALL_DATA_HIT ==  joylink_join_pkg_add_data(name_2, id, &phead, buffer + offset, len)){
            p_data = joylink_join_pkg_join_data(name_2, id, &len);
            printf("---:org len:%d: len:%d after buff:%s\n", red, len, p_data);
            printf("VVVVVVVVVVV\n");
            if(NULL != p_data){
                free(p_data);
            }
        }
    }

    joylink_join_pkg_clean_node_by_name_id(name_2, id);

    p_data = joylink_join_pkg_join_data(name_2, id, &len);
    printf("---:org len:%d: len:%d after buff:%s\n", red, len, p_data);
    if(NULL != p_data){
        free(p_data);
    }

    joylink_join_pkg_clean_node_by_name_id(name, id);
    joylink_join_pkg_clean_node_by_name_id(name_2, id);
}

int 
joylink_util_cut_ip_port(const char *ipport, char *out_ip, int *out_port)
{
    if(NULL == ipport || 
            NULL == out_ip ||
            NULL == out_port){
        return E_RET_ERROR;
    }
    int ret = E_RET_OK;
    int offset = 0;
    int len = strlen(ipport);

    while(ipport[offset] != ':' && offset < len){
        offset++;
    }
    
    if(offset < len){
        memcpy(out_ip, ipport, offset);
        out_ip[offset] = '\0';
        *out_port = atoi(ipport + offset + 1);
    }else{
        ret = E_RET_ERROR;
    }

	return ret;
}

#if 0
int main()
{
    /*test_join_data();*/
    char ip[128];
    int port =  0;

    joylink_util_cut_ip_port("192.168.111.190:9001", ip, &port);

    printf("ip:%s:port:%d\n", ip, port);
    return 0;
}
#endif

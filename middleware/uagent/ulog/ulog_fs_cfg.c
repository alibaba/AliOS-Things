/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulog_session_file.h"
#include <string.h>
#include <fcntl.h>
#include "aos/kernel.h"
#include "cJSON.h"
#include "errno.h"
#include "ulog_api.h"

#define ULOG_CFG_PARA_KEY_SIZE 12

typedef struct {
    ulog_cfg_type_t  type;
    char*            desription;
    char*            cfg_key[ulog_cfg_para_cnt];
} ulog_cfg_key_list_t;

typedef struct {
    char ulog_cfg_val[ULOG_CFG_PARA_VAL_SIZE];
}ulog_cfg_t;

typedef struct _ulog_cfg_node_t {
    ulog_cfg_type_t           type;
    unsigned short            file_idx;
    ulog_cfg_t               *cfg[ulog_cfg_para_cnt];
    struct _ulog_cfg_node_t  *next;
} ulog_cfg_node_t;

static aos_mutex_t mutex_entry_cfg_mm;

static ulog_cfg_node_t *ulog_cfg_header = NULL;

ulog_cfg_key_list_t cfg_key_list[ulog_cfg_type_cnt] =
{
    {ulog_cfg_type_working,  "working_idx", {NULL, NULL}},
    {ulog_cfg_type_list,     "file_idx",    {"start", "end"}}
};

static void parser_cfg_file(const char* cfg);

static ulog_cfg_key_list_t *get_ulog_cfg_key(const ulog_cfg_type_t type)
{
    ulog_cfg_key_list_t *rc = NULL;
    uint8_t i = 0;
    for (; i < ulog_cfg_type_cnt; i++) {
        if (type == cfg_key_list[i].type) {
            rc = &cfg_key_list[i];
            break;
        }
    }
    return rc;
}

void cfg_init_mutex(void)
{
    aos_mutex_new(&mutex_entry_cfg_mm);
}

bool cfg_get_mutex(void)
{
    return 0 == aos_mutex_lock(&mutex_entry_cfg_mm, AOS_WAIT_FOREVER);
}

void cfg_release_mutex(void)
{
    aos_mutex_unlock(&mutex_entry_cfg_mm);
}

int aos_get_ulog_list(char* buf, const unsigned short len)
{
    int rc = -EINVAL;
    if(NULL!=buf && (len>=strlen(LOG_LIST_PREFIX)+strlen(LOG_LIST_SUFFIX)+1) ){
        rc = -EIO;
        if(aos_mutex_is_valid(&mutex_entry_cfg_mm)){
            if(cfg_get_mutex()){
                rc = 0;
                ulog_cfg_node_t *p = ulog_cfg_header;
                if(NULL!=p){
                    char time_buf[24];
                    snprintf(buf, len-strlen(LOG_LIST_SUFFIX)-1, LOG_LIST_PREFIX, ulog_format_time(time_buf, sizeof(time_buf)));
                    unsigned short off = strlen(buf);

                    while (p != NULL && rc == 0) {
                        if (p->type == ulog_cfg_type_list) {
                            const int empty_room = len-strlen(LOG_LIST_SUFFIX)-1-off;
                            int result = 0;
                            if(empty_room>(result=snprintf(&buf[off], empty_room, LOG_LIST_NODE,p->file_idx,
                                p->cfg[0]==NULL?"":p->cfg[0]->ulog_cfg_val,p->cfg[1]==NULL?"":p->cfg[1]->ulog_cfg_val))){
                                off += result;
                            }else{
                                rc = -ENOMEM;
                            }
                        }
                        p = p->next;
                    }
                    if(rc==0){
                        if(buf[off-1]==','){
                            off--;
                        }
                        snprintf(&buf[off], len-strlen(LOG_LIST_SUFFIX)-1-off, LOG_LIST_SUFFIX);
                    }
                }else{
                    rc = -EIO;
                }
                cfg_release_mutex();
            }
        }
    }
    return rc;
}

int update_mm_cfg(const ulog_cfg_type_t type, const unsigned short idx, const ulog_cfg_para_t para_type, char val[ULOG_CFG_PARA_VAL_SIZE])
{
    int rc = -1;
    if(cfg_get_mutex()){
        if (NULL == ulog_cfg_header) {
            ulog_cfg_header = (ulog_cfg_node_t *)aos_malloc(sizeof(ulog_cfg_node_t));
            if (NULL != ulog_cfg_header) {
                ulog_cfg_header->next = NULL;
                ulog_cfg_header->type = type;
                ulog_cfg_header->file_idx = idx;
                uint8_t i = 0;
                for(; i<ulog_cfg_para_cnt; i++){
                    if(NULL!=get_ulog_cfg_key(type)->cfg_key[i]){
                        ulog_cfg_header->cfg[i] = (ulog_cfg_t *)aos_malloc(sizeof(ulog_cfg_t));
                        memset(ulog_cfg_header->cfg[para_type], 0, sizeof(ulog_cfg_t));
                    }
                }
                if (ulog_cfg_para_none != para_type) {
                    strncpy(ulog_cfg_header->cfg[para_type]->ulog_cfg_val, val, ULOG_CFG_PARA_VAL_SIZE - 1);
                }
                rc = 0;
            } else {
                SESSION_FS_INFO("alloc memory fail for type %d idx %d\n", type, idx);
            }
        } else {
            ulog_cfg_node_t *p = ulog_cfg_header;
            while (p != NULL && rc != 0) {
                if (p->type == type && (ulog_cfg_para_none == para_type || p->file_idx == idx)) { /* found it alredy in list, just update */
                    if (ulog_cfg_para_none == para_type) {
                        /* no cfg, just update file list, apply to type working */
                        p->file_idx = idx;
                        rc = 0;
                        break;
                    } else {
                        uint8_t i = 0;
                        for(; i<ulog_cfg_para_cnt; i++){
                            if(NULL!=get_ulog_cfg_key(type)->cfg_key[i] && NULL==p->cfg[i]){
                                p->cfg[i] = (ulog_cfg_t *)aos_malloc(sizeof(ulog_cfg_t));
                                memset(p->cfg[i], 0, sizeof(ulog_cfg_t));
                            }
                        }
                        strncpy(p->cfg[para_type]->ulog_cfg_val, val, ULOG_CFG_PARA_VAL_SIZE - 1);
                        rc = 0;
                    }
                    break;
                }
                if (p->next == NULL) { /* last node */
                    p->next = (ulog_cfg_node_t*)aos_malloc(sizeof(ulog_cfg_node_t));
                    if (NULL != p->next) {
                        p->next->next = NULL;
                        p->next->type = type;
                        p->next->file_idx = idx;

                        uint8_t i = 0;
                        for(; i<ulog_cfg_para_cnt; i++){
                            if(NULL!=get_ulog_cfg_key(type)->cfg_key[i]){
                                p->next->cfg[i] = (ulog_cfg_t *)aos_malloc(sizeof(ulog_cfg_t));
                                memset(p->next->cfg[i], 0, sizeof(ulog_cfg_t));
                            }
                        }
                        if (ulog_cfg_para_none != para_type) {
                            strncpy(p->next->cfg[para_type]->ulog_cfg_val, val, ULOG_CFG_PARA_VAL_SIZE - 1);
                        }
                        rc = 0;
                    } else {
                        SESSION_FS_INFO("alloc memory fail for type %d idx %d\n", type, idx);
                    }
                    break;
                } else {
                    p = p->next;
                }
            }
        }
        cfg_release_mutex();
    }
    return rc;
}

/**
*
* Sync cfg from cfg file to memory
*
* return 0 if this step pass, else indicates this step fail
*
*/
int load_cfg_2_mm()
{
    int rc = -1;
    const int fd = open_log_file(ULOG_FILE_CFG_IDX, O_RDONLY, 0);
    if (fd >= 0) {
        char one_cfg_item[ULOG_CFG_LINE_MAX_SIZE];
        int off = 0;
        int read_len = 0;
        /* log cofig file exist, read it to archive config item */
        while ((read_len = get_log_line(fd, one_cfg_item, sizeof(one_cfg_item))) > 1) {
            off += (read_len + ((ULOG_CFG_LINE_MAX_SIZE == read_len) ? 0 : 1));
            parser_cfg_file(one_cfg_item);

        }
        aos_close(fd);
    }

    ulog_cfg_node_t *p = ulog_cfg_header;
    while (p != NULL) {
        SESSION_FS_DEBUG("check cfg item in mm type %d idx %d\n", p->type, p->file_idx);
        uint8_t i = 0;
        for(; i<ulog_cfg_para_cnt; i++){
            if(p->cfg[i]!=NULL){
                SESSION_FS_DEBUG("check cfg item in mm cfg value %s @ %d\n", p->cfg[i]->ulog_cfg_val, i);
            }
        }
        p = p->next;

    }
    return rc;
}

/**
*
* Sync cfg from memory to file
*
* return 0 if this step pass, else indicates this step fail
*
*/
int cfg_mm_2_file(const int fd)
{
    int rc = -EINVAL;
    if (fd >= 0 && NULL != ulog_cfg_header) {
        ulog_cfg_node_t *p = ulog_cfg_header;
        while (NULL != p) {
            switch (p->type)
            {
            case ulog_cfg_type_working: {
                cJSON *ulog_cfg_obj = cJSON_CreateObject();
                if (NULL != ulog_cfg_obj) {

                    cJSON_AddItemToObject(ulog_cfg_obj, get_ulog_cfg_key(p->type)->desription, cJSON_CreateNumber(p->file_idx));
                    char *cfg_line = cJSON_PrintUnformatted(ulog_cfg_obj);
                    cJSON_Delete(ulog_cfg_obj);
                    if (NULL != cfg_line) {
                        write_log_line(fd, cfg_line, true);
                        SESSION_FS_INFO("sync cfg to file %s\n", cfg_line);
                        cJSON_free(cfg_line);
                        cfg_line = NULL;
                        rc = 0;
                    } else {
                        rc = -EIO;
                    }
                } else {
                    rc = -ENOMEM;
                    SESSION_FS_INFO("[%s#%d]create cjson obj fail\n", __FILE__, __LINE__);
                }

            }
                break;

            case ulog_cfg_type_list: {
                cJSON *ulog_cfg_obj = cJSON_CreateObject();
                if (NULL != ulog_cfg_obj) {
                    uint8_t i = 0;
                    cJSON_AddItemToObject(ulog_cfg_obj, get_ulog_cfg_key(p->type)->desription, cJSON_CreateNumber(p->file_idx));
                    for (; i < ulog_cfg_para_cnt; i++) {
                        if (p->cfg[i] != NULL) {
                            cJSON_AddItemToObject(ulog_cfg_obj, get_ulog_cfg_key(p->type)->cfg_key[i], cJSON_CreateString(p->cfg[i]->ulog_cfg_val));
                        }
                    }

                    char *cfg_line = cJSON_PrintUnformatted(ulog_cfg_obj);
                    cJSON_Delete(ulog_cfg_obj);
                    if (NULL != cfg_line) {
                        write_log_line(fd, cfg_line, true);
                        SESSION_FS_INFO("sync cfg to file %s\n", cfg_line);
                        cJSON_free(cfg_line);
                        cfg_line = NULL;
                        rc = 0;
                    } else {
                        rc = -EIO;
                    }
                } else {
                    rc = -ENOMEM;
                    SESSION_FS_INFO("[%s#%d]create cjson obj fail\n", __FILE__, __LINE__);
                }

            }
                break;

            default:
                break;
            }
            p = p->next;
        }

    } else {
        SESSION_FS_INFO("sync fail as mm illegal\n");
    }
    return rc;
}

static void parser_cfg_file(const char* cfg)
{
    if (NULL != cfg) {
        cJSON *root = NULL;
        root = cJSON_Parse(cfg);

        if (NULL != root) {
            uint8_t i = 0;
            cJSON *config_node = NULL;
            SESSION_FS_DEBUG("parser_cfg_file input %s\n",cfg);
            for (; i < ulog_cfg_type_cnt; i++) {
                SESSION_FS_DEBUG("key list description %s @ %d\n",cfg_key_list[i].desription, i);
                config_node = cJSON_GetObjectItem(root, cfg_key_list[i].desription);
                if (NULL != config_node) {
                    if (cJSON_IsNumber(config_node)) {
                        if(ulog_cfg_type_working==cfg_key_list[i].type){
                            update_mm_cfg(cfg_key_list[i].type, (ulog_idx_type)config_node->valueint, ulog_cfg_para_none, NULL);
                        }
                        uint8_t j = 0;
                        cJSON *config_para = NULL;
                        for (; j < ulog_cfg_para_cnt; j++) {
                            if (cfg_key_list[i].cfg_key[j] != NULL) {
                                SESSION_FS_DEBUG("key list description %s cfg key %s @ %d %d\n",cfg_key_list[i].desription,cfg_key_list[i].cfg_key[j], i,j);
                                config_para = cJSON_GetObjectItem(root, cfg_key_list[i].cfg_key[j]);
                                if (NULL != config_para && cJSON_IsString(config_para)) {
                                    update_mm_cfg(cfg_key_list[i].type,
                                        (ulog_idx_type)config_node->valueint,
                                        j,
                                        config_para->valuestring);
                                }
                            }
                        }
                    }

                    break;
                }
            }

            cJSON_Delete(root);
            root = NULL;
        }
    }
}

ulog_idx_type get_working_from_cfg_mm()
{
    ulog_idx_type rc = ULOG_FILE_IDX_INVALID;
    ulog_cfg_node_t *p = ulog_cfg_header;
    while (p != NULL) {
        if (p->type == ulog_cfg_type_working) { /* found it alredy in list, just update */
            rc = p->file_idx;
            break;
        } else {
            p = p->next;
        }
    }
    return rc;
}


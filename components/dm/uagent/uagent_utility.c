/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include "uagent/uagent.h"
#include "uagent_inner_api.h"
#include "aos/kernel.h"
#include "aos/errno.h"
#include "k_config.h"
#include "cJSON.h"

static aos_mutex_t uagent_mutex;

void uagent_init_mutex(void)
{
    aos_mutex_new(&uagent_mutex);
}

bool uagent_get_mutex(void)
{
    return 0 == aos_mutex_lock(&uagent_mutex, AOS_WAIT_FOREVER);
}

void uagent_release_mutex(void)
{
    aos_mutex_unlock(&uagent_mutex);
}

int add_list(mod_func_t *mod_func_tree, const ua_func_t func, char *func_name)
{
    int rc = -1;
    if (NULL != mod_func_tree && NULL != func_name) {
        UAGENT_DEBUG("will add list mod %s func %d func name %s\n", mod_func_tree->mod_info.name, func, func_name);
        uagent_func_node_t *header = mod_func_tree->header;
        if (NULL == mod_func_tree->header) {
            mod_func_tree->header = (uagent_func_node_t*)aos_malloc(sizeof(uagent_func_node_t));
            if (NULL != mod_func_tree->header) {
                mod_func_tree->header->next = NULL;
                mod_func_tree->header->func = func;
                memset(mod_func_tree->header->func_name, 0, sizeof(mod_func_tree->header->func_name));
                strncpy(mod_func_tree->header->func_name, func_name, sizeof(mod_func_tree->header->func_name) - 1);
                mod_func_tree->mod_info.func_count = 1;
                rc = 0;
            } else {
                UAGENT_ERR("[uA]alloc memory of func %s for mod %s fail\n", func_name, mod_func_tree->mod_info.name);
            }
        } else {
            uagent_func_node_t *p = header;
            while (p != NULL) {
                if (p->func == func) { /* found it alredy in list, just update */
                    memset(header->func_name, 0, sizeof(header->func_name));
                    strncpy(header->func_name, func_name, sizeof(header->func_name) - 1);
                    /* keep the count */
                    rc = 0;
                    break;
                }
                if (p->next == NULL) {
                    p->next = (uagent_func_node_t*)aos_malloc(sizeof(uagent_func_node_t));
                    if (NULL != p->next) {
                        p->next->next = NULL;
                        p->next->func = func;
                        memset(p->next->func_name, 0, sizeof(p->next->func_name));
                        strncpy(p->next->func_name, func_name, sizeof(p->next->func_name) - 1);
                        mod_func_tree->mod_info.func_count++;
                        rc = 0;
                    } else {
                        UAGENT_ERR("[uA]alloc memory of func %s for mod %s fail\n", func_name, mod_func_tree->mod_info.name);
                    }
                    break;
                } else {
                    p = p->next;
                }
            }
        }
    }
    return rc;
}

int del_list(mod_func_t *mod_func_tree, const ua_func_t func)
{
    int rc = -1;
    if (NULL != mod_func_tree) {
        uagent_func_node_t *header = mod_func_tree->header;
        if (NULL != header) {
            uagent_func_node_t *p = header;
            if (p->func == func) {/* header matches, remove it */
                mod_func_tree->header = p->next;
                aos_free(p);
                p = NULL;
                rc = 0;
            } else {
                uagent_func_node_t *before = header;
                p = before->next;
                while (p != NULL) {
                    if (p->func == func) {
                        before->next = p->next;/* rear of list, just remove this node */
                        aos_free(p);
                        p = NULL;
                        rc = 0;
                        break;
                    } else {
                        before = p;
                        p = before->next;
                    }
                }
            }
            if (0 == rc) {
                if (mod_func_tree->mod_info.func_count > 0) {
                    mod_func_tree->mod_info.func_count--;
                }
            }
        }
    }
    return rc;
}

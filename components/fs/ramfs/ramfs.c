/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ramfs_types.h"
#include "ramfs_api.h"
#include "ramfs_adapt.h"

#define RAMFS_LL_NODE_META_SIZE (sizeof(ramfs_ll_node_t *) + \
                                 sizeof(ramfs_ll_node_t *))

#define RAMFS_LL_PREV_OFFSET(ll) (ll->size)
#define RAMFS_LL_NEXT_OFFSET(ll) (ll->size + sizeof(ramfs_ll_node_t *))

#define RAMFS_LL_READ(ll, i) \
    for (i = ramfs_ll_get_head(&ll); i != NULL; i = ramfs_ll_get_next(&ll, i))
#define RAMFS_LL_READ_BACK(ll, i) \
    for (i = ramfs_ll_get_tail(&ll); i != NULL, u = ramfs_ll_get_prev(&ll, i))

static ramfs_ll_t g_file_ll;

static uint8_t g_inited = 0;

/**
 * @brief Set the previous node pointer of a node
 *
 * @param[in]  ll   pointer to linked list
 * @param[out] act  pointer to a node which prev node pointer should be set
 * @param[in]  prev pointer to the previous node before act
 *
 * @return None
 */
static void ramfs_node_set_prev(ramfs_ll_t *ll, ramfs_ll_node_t *act,
                                ramfs_ll_node_t *prev)
{
    memcpy(act + RAMFS_LL_PREV_OFFSET(ll), &prev, sizeof(ramfs_ll_node_t *));
}

/**
 * @brief Set the next node pointer of a node
 *
 * @param[in]  ll   pointer to linked list
 * @param[out] act  poiner to a node which next node pointer should be set
 * @param[in]  next poiner to the next node after act
 *
 * @return None
 */
static void ramfs_node_set_next(ramfs_ll_t *ll, ramfs_ll_node_t *act,
                                ramfs_ll_node_t *next)
{
    memcpy(act + RAMFS_LL_NEXT_OFFSET(ll), &next, sizeof(ramfs_ll_node_t *));
}

/**
 * @brief Initialize ramfs linked list
 *
 * @param[out] ll   pointer to the ramfs linked list
 * @param[in]  size the size of 1 node in bytes
 *
 * @return None
 */
static void ramfs_ll_init(ramfs_ll_t *ll, uint32_t size)
{
    ll->head = NULL;
    ll->tail = NULL;

    if (size & 0x3) {
        size &= ~0x3;
        size += 4;
    }

    ll->size = size;
}

/**
 * @brief Add a new head to the linked list
 *
 * @param[in] ll pointer to the linked list
 *
 * @return pointer to the new head, NULL if no memory
 */
static void *ramfs_ll_ins_head(ramfs_ll_t *ll)
{
    ramfs_ll_node_t *new = NULL;

    new = ramfs_mm_alloc(ll->size + RAMFS_LL_NODE_META_SIZE);

    if (new != NULL) {
        ramfs_node_set_prev(ll, new, NULL);     /* No prev before the new head */
        ramfs_node_set_next(ll, new, ll->head); /* After new comes the old head */

        if (ll->head != NULL) { /* If there is old head then before it goes the new */
            ramfs_node_set_prev(ll, ll->head, new);
        }

        ll->head = new; /* Set the new head in the linked list */
        if (ll->tail == NULL) { /* If there is no tail, set the tail too */
            ll->tail = new;
        }
    }

    return new;
}

/**
 * @brief Return with head node of the linked list
 *
 * @param[in] ll pointer to the linked list
 *
 * @return pointer to the head of linked list
 */
void *ramfs_ll_get_head(ramfs_ll_t *ll)
{
    void *head = NULL;

    if (ll->head != NULL) {
        head = ll->head;
    }

    return head;
}

/**
 * @brief Return with tail node of the linked list
 *
 * @param[in] ll pointer to the linked list
 *
 * @return pointer to the tail of linked list
 */
void *ramfs_ll_get_tail(ramfs_ll_t *ll)
{
    void *tail = NULL;

    if (ll->tail != NULL) {
        tail = ll->tail;
    }

    return tail;
}

/**
 * @brief Return with the pointer of the next node after act
 *
 * @param[in] ll  pointer to the linked list
 * @param[in] act pointer to a node
 *
 * @return pointer to the next node
 */
void *ramfs_ll_get_next(ramfs_ll_t *ll, void *act)
{
    void *next = NULL;

    ramfs_ll_node_t *node = act;

    if (ll != NULL) {
        memcpy(&next, node + RAMFS_LL_NEXT_OFFSET(ll), sizeof(void *));
    }

    return next;
}

/**
 * @brief Return with the pointer of the previous node after act
 *
 * @param[in] ll  pointer to the linked list
 * @param[in] act pointer to a node
 *
 * @return pointer to the previous node
 */
void *ramfs_ll_get_prev(ramfs_ll_t *ll, void *act)
{
    void *prev = NULL;

    ramfs_ll_node_t *node = act;

    if (ll != NULL) {
        memcpy(&prev, node + RAMFS_LL_PREV_OFFSET(ll), sizeof(void *));
    }

    return prev;
}

/**
 * @brief Remove the node from linked list
 *
 * @param[in] ll   pointer to the linked list of node
 * @param[in] node pointer to the node in linked list
 *
 * @return None
 */
static void ramfs_ll_remove(ramfs_ll_t *ll, void *node)
{
    ramfs_ll_node_t *prev;
    ramfs_ll_node_t *next;

    if (ramfs_ll_get_head(ll) == node) {
        ll->head = ramfs_ll_get_next(ll, node);

        if (ll->head == NULL) {
            ll->tail = NULL;
        } else {
            ramfs_node_set_prev(ll, ll->head, NULL);
        }

    } else if (ramfs_ll_get_tail(ll) == node) {
        ll->tail = ramfs_ll_get_prev(ll, node);

        if (ll->tail == NULL) {
            ll->head = NULL;
        } else {
            ramfs_node_set_next(ll, ll->tail, NULL);
        }

    } else {
        prev = ramfs_ll_get_prev(ll, node);
        next = ramfs_ll_get_next(ll, node);

        ramfs_node_set_next(ll, prev, next);
        ramfs_node_set_prev(ll, next, prev);
    }
}

/**
 * @brief Search path in link
 *
 * @param[in] fn file name
 *
 * @return 0 if get path in link, RAMFS_ERR_FS if failed
 */
static int ramfs_search_link(ramfs_entry_t *entry, char *path)
{
    link_name_t *link_name = entry->link;

    while (link_name != NULL) {
        if (strcmp(link_name->name, path) == 0) {
            return RAMFS_OK;
        } else {
            link_name = link_name->next;
        }
    }

    return RAMFS_ERR_FS;
}

/**
 * @brief Search path in link
 *
 * @param[in] fn file name
 *
 * @return 0 if get path in link, RAMFS_ERR_FS if failed
 */
static int ramfs_search_link_part(ramfs_entry_t *entry, char *path)
{
    link_name_t *link_name = entry->link;

    while (link_name != NULL) {
        if (strncmp(link_name->name, path, strlen(path)) == 0) {
            return RAMFS_OK;
        } else {
            link_name = link_name->next;
        }
    }

    return RAMFS_ERR_FS;
}

/**
 * @brief Give the ramfs entry from a filename
 *
 * @param[in] fn file name
 *
 * @return pointer to the dynamically allocated entry with 'fn'.
 *         NULL if no entry found with that name
 */
static ramfs_entry_t *ramfs_entry_get(const char *fn)
{
    ramfs_entry_t *entry;
    int            ret = -1;

    RAMFS_LL_READ(g_file_ll, entry) {
        if (strcmp(entry->fn, fn) == 0) {
            return entry;
        } else {
            ret = ramfs_search_link(entry, (char *)fn);
            if (ret == RAMFS_OK) {
                return entry;
            }
        }
    }

    return NULL;
}

/**
 * @brief Create dir involved in path
 *
 * @param[in] fn file name
 *
 * @return poiner to the dynamically allocated new entry
 *         NULL if no space for the entry
 */
static int ramfs_entry_dir_new(const char *path)
{
    int    i    = 0;
    int    flag = 0;

    char dir_buf[RAMFS_PATH_MAX];

    ramfs_entry_t *new_entry = NULL;

    /* create dir involved in path, if path is "/ramfs/f1/f2/file1", then
       create dir "/ramfs/f1" and "/ramfs/f1/f2" */
    for(i = 1; i < strlen(path); i++) {
        if (path[i] == '/') {
            memset(dir_buf, 0, sizeof(dir_buf));
            memcpy(dir_buf, path, i);

            flag = 0;
            new_entry = NULL;

            /* if the same dir is found, ignore this dir */
            RAMFS_LL_READ(g_file_ll, new_entry) {
                if ((strcmp(new_entry->fn, dir_buf) == 0) && (new_entry->is_dir == 1)) {
                    flag = 1;
                    break;
                }
            }

            if (flag == 0) {
                new_entry = ramfs_ll_ins_head(&g_file_ll); /* Create a new file */
                if (new_entry == NULL) {
                    return RAMFS_ERR_MALLOC;
                }

                new_entry->fn = ramfs_mm_alloc(i + 1);
                memset(new_entry->fn, 0, (i + 1));
                strncpy(new_entry->fn, path, i);

                new_entry->data       = NULL;
                new_entry->size       = 0;
                new_entry->refs       = 0;
                new_entry->const_data = 0;
                new_entry->is_dir     = 1;
                new_entry->link       = NULL;
                new_entry->link_count = 0;
            }
        }
    }

    return RAMFS_OK;
}

/**
 * @brief Create a new entry with 'fn' file name
 *
 * @param[in] fn file name
 *
 * @return poiner to the dynamically allocated new entry
 *         NULL if no space for the entry
 */
static ramfs_entry_t *ramfs_entry_new(const char *fn)
{
    ramfs_entry_t *new_entry = NULL;

    int ret = -1;

    /* create dir involved in path, if path is "/ramfs/f1/f2/file1", then
       create dir "/ramfs/f1" and "/ramfs/f1/f2" */
    ret = ramfs_entry_dir_new(fn);
    if (ret != RAMFS_OK) {
        return NULL;
    }

    new_entry = ramfs_ll_ins_head(&g_file_ll); /* Create a new file */
    if (new_entry == NULL) {
        return NULL;
    }

    new_entry->fn = ramfs_mm_alloc(strlen(fn) + 1);
    strcpy(new_entry->fn, fn);

    new_entry->data       = NULL;
    new_entry->size       = 0;
    new_entry->refs       = 0;
    new_entry->const_data = 0;
    new_entry->is_dir     = 0;
    new_entry->link       = NULL;
    new_entry->link_count = 0;

    return new_entry;
}

void ramfs_init(void)
{
    ramfs_ll_init(&g_file_ll, sizeof(ramfs_entry_t));

    g_inited = 1;
}

void ramfs_deinit(void)
{
    if (g_inited) {
        memset(&g_file_ll, 0 , sizeof(g_file_ll));
        g_inited = 0;
    }
}

int32_t ramfs_ready(void)
{
    return g_inited;
}

int32_t ramfs_open(void *fp, const char* fn, uint32_t mode)
{
    ramfs_file_t *file   = (ramfs_file_t *)fp;
    ramfs_entry_t *entry = ramfs_entry_get(fn);

    file->entry = NULL;

    if ((entry != NULL) && (entry->is_dir == 1)) {
        return RAMFS_ERR_NOT_EXIST;
    }

    if (entry == NULL) {
        if ((mode & RAMFS_MODE_WR) != 0) {
            entry = ramfs_entry_new(fn);
            if (entry == NULL) {
                return RAMFS_ERR_FULL;
            }
        } else {
            return RAMFS_ERR_NOT_EXIST;
        }

    }

    file->entry     = entry;
    file->entry->ar = mode & RAMFS_MODE_RD ? 1 : 0;
    file->entry->aw = mode & RAMFS_MODE_WR ? 1 : 0;
    file->rwp       = 0;

    entry->refs++;

    return RAMFS_OK;
}

int32_t ramfs_create_const(const char *fn, const void *data, uint32_t len)
{
    int32_t res;

    ramfs_file_t   file;
    ramfs_entry_t *entry;

    res = ramfs_open(&file, fn, RAMFS_MODE_RD);
    if (res == RAMFS_OK) {
        ramfs_close(&file);
        return RAMFS_ERR_DENIED;
    }

    res = ramfs_open(&file, fn, RAMFS_MODE_WR);
    if (res != RAMFS_OK) {
        return res;
    }

    entry = file.entry;

    if (entry->data != NULL) {
        return RAMFS_ERR_DENIED;
    }

    entry->data       = (void *)data;
    entry->size       = len;
    entry->const_data = 1;
    entry->ar         = 1;
    entry->aw         = 0;

    res = ramfs_close(&file);

    return res;
}

int32_t ramfs_close(void *fp)
{
    ramfs_file_t *file = (ramfs_file_t *)fp;

    if (file->entry == NULL) {
        return RAMFS_OK;
    }

    if (file->entry->refs > 0) {
        file->entry->refs--;
    }

    return RAMFS_OK;
}

int32_t ramfs_remove(const char *fn)
{
    ramfs_entry_t *entry = ramfs_entry_get(fn);

    link_name_t *link_name      = NULL;
    link_name_t *link_name_next = NULL;

    if (entry == NULL) {
        return RAMFS_ERR_NOT_EXIST;
    }

    if (entry->is_dir == 1) {
        return ramfs_rmdir(fn);
    }

    if (entry->refs != 0) {
        return RAMFS_ERR_DENIED;
    }

    ramfs_ll_remove(&g_file_ll, entry);
    ramfs_mm_free(entry->fn);
    entry->fn = NULL;

    link_name = entry->link;
    while(link_name != NULL) {
        link_name_next = link_name->next;
        ramfs_mm_free(link_name->name);
        ramfs_mm_free(link_name);
        link_name = link_name_next;
    }

    if (entry->const_data == 0) {
        ramfs_mm_free(entry->data);
        entry->data = NULL;
    }

    ramfs_mm_free(entry);

    return RAMFS_OK;
}

int32_t ramfs_rename(const char *old, const char *new)
{
    ramfs_entry_t *entry = NULL;

    entry = ramfs_entry_get(new);
    if (entry != NULL) {
        return RAMFS_ERR_PATH;
    }

    entry = ramfs_entry_get(old);
    if (entry == NULL) {
        return RAMFS_ERR_NOT_EXIST;
    }

    if ((entry != NULL) && (entry->is_dir == 1)) {
        return RAMFS_ERR_PATH;
    }

    /* rename linked name is not supported */
    if (strcmp(entry->fn, old) != 0) {
        return RAMFS_ERR_NOT_IMP;
    }

    entry->fn = ramfs_mm_realloc(entry->fn, strlen(new));
    strcpy(entry->fn, new);
    if (entry->fn == NULL) {
        return RAMFS_ERR_MALLOC;
    }

    return RAMFS_OK;
}

int32_t ramfs_read(void *fp, void *buf, uint32_t btr, uint32_t *br)
{
    uint8_t       *data;
    ramfs_file_t  *file;
    ramfs_entry_t *entry;

    file  = (ramfs_file_t *)fp;
    entry = file->entry;
    *br   = 0;

    if (entry->data == NULL || entry->size == 0) {
        return RAMFS_OK;
    } else if (entry->ar == 0) {
        return RAMFS_ERR_DENIED;
    }

    if (file->rwp + btr > entry->size) {
        *br = entry->size - file->rwp;
    } else {
        *br = btr;
    }

    if (entry->const_data == 0) {
        data = (uint8_t *)entry->data;
    } else {
        data = entry->data;
    }

    data += file->rwp;
    memcpy(buf, data, *br);

    file->rwp += *br;

    return RAMFS_OK;
}

int32_t ramfs_write(void *fp, const void *buf, uint32_t btw, uint32_t *bw)
{
    uint32_t  new_size;
    uint8_t  *new_data;
    uint8_t  *rwp;

    ramfs_file_t  *file;
    ramfs_entry_t *entry;

    file  = (ramfs_file_t *)fp;
    entry = file->entry;
    *bw   = 0;

    if (entry->aw == 0) {
        return RAMFS_ERR_DENIED;
    }

    new_size = file->rwp + btw;
    if (new_size > entry->size) {
        new_data = ramfs_mm_realloc(entry->data, new_size);
        if (new_data == NULL) {
            return RAMFS_ERR_FULL;
        }

        entry->data = new_data;
        entry->size = new_size;
    }

    rwp = (uint8_t *)entry->data;
    rwp += file->rwp;

    memcpy(rwp, buf, btw);
    *bw = btw;
    file->rwp += *bw;

    return RAMFS_OK;
}

int32_t ramfs_seek(void *fp, uint32_t pos)
{
    uint8_t *new_data;

    ramfs_file_t  *file;
    ramfs_entry_t *entry;

    file  = (ramfs_file_t *)fp;
    entry = file->entry;

    if (pos < entry->size) {
        file->rwp = pos;
    } else {
        if (entry->aw == 0) {
            return RAMFS_ERR_DENIED;
        }

        new_data = ramfs_mm_realloc(entry->data, pos);
        if (new_data == NULL) {
            return RAMFS_ERR_FULL;
        }

        entry->data = new_data;
        entry->size = pos;
        file->rwp   = pos;
    }

    return RAMFS_OK;
}

int32_t ramfs_tell(void *fp, uint32_t *pos)
{
    *pos = ((ramfs_file_t *)fp)->rwp;

    return RAMFS_OK;
}

int32_t ramfs_trunc(void *fp)
{
    void *new_data;

    ramfs_file_t  *file;
    ramfs_entry_t *entry;

    file  = (ramfs_file_t *)fp;
    entry = file->entry;

    if (entry->aw == 0) {
        return RAMFS_ERR_DENIED;
    }

    new_data = ramfs_mm_realloc(entry->data, file->rwp);
    if (new_data == NULL) {
        return RAMFS_ERR_FULL;
    }

    entry->data = new_data;
    entry->size = file->rwp;

    return RAMFS_OK;
}

int32_t ramfs_size(void *fp, uint32_t *size)
{
    *size = ((ramfs_file_t *)fp)->entry->size;

    return RAMFS_OK;
}

int32_t ramfs_access(const char *path, int32_t mode)
{
    ramfs_entry_t *entry = ramfs_entry_get(path);

    if (mode == F_OK) {
        if (entry == NULL) {
            return RAMFS_ERR_DENIED;
        } else {
            return RAMFS_OK;
        }
    }

    if (mode == R_OK) {
        if (entry == NULL) {
            return RAMFS_OK;
        } else {
            if (entry->ar == 1) {
                return RAMFS_OK;
            } else {
                return RAMFS_ERR_DENIED;
            }
        }
    }

    if (mode == W_OK) {
        if (entry == NULL) {
            return RAMFS_OK;
        } else {
            if (entry->aw == 1) {
                return RAMFS_OK;
            } else {
                return RAMFS_ERR_DENIED;
            }
        }
    }

    if (mode == X_OK) {
        return RAMFS_OK;
    }

    return RAMFS_ERR_DENIED;
}

int32_t ramfs_mkdir(const char *path)
{
    ramfs_entry_t *entry = ramfs_entry_get(path);

    if ((entry != NULL) && (entry->is_dir == 1)) {
        return RAMFS_ERR_EXIST;
    }

    entry = ramfs_entry_new(path);
    if (entry == NULL) {
        return RAMFS_ERR_FULL;
    }

    entry->is_dir = 1;

    return RAMFS_OK;
}

int32_t ramfs_opendir(void *dp, const char *path)
{
    ramfs_dir_t   *ramfs_dp;
    ramfs_entry_t *entry;

    ramfs_dp = (ramfs_dir_t *)dp;
    entry = ramfs_entry_get(path);

    if (entry == NULL) {
        return RAMFS_ERR_NOT_EXIST;
    }

    if (entry->is_dir != 1) {
        entry = ramfs_entry_get(path);
        if (entry == NULL) {
            return RAMFS_ERR_NOT_EXIST;
        }

        if (entry->is_dir != 1) {
            return RAMFS_ERR_NOT_EXIST;
        }
    }

    ramfs_dp->dir_name = ramfs_mm_alloc(strlen(path));

    if (ramfs_dp->dir_name == NULL) {
        return RAMFS_ERR_FULL;
    }

    strcpy(ramfs_dp->dir_name, path);
    ramfs_dp->last_entry = NULL;

    return RAMFS_OK;
}

int32_t ramfs_readdir(void *dp, char *fn)
{
    int i      = 0;
    int len    = 0;
    int search = 1;

    char *name = NULL;
    char *data = NULL;

    ramfs_dir_t *ramfs_dp = (ramfs_dir_t *)dp;

    if (ramfs_dp->last_entry == NULL) {
        ramfs_dp->last_entry = ramfs_ll_get_head(&g_file_ll);
    }

    while ((search == 1) && (ramfs_dp->last_entry != NULL)) {
        if ((strncmp(ramfs_dp->dir_name, ramfs_dp->last_entry->fn, strlen(ramfs_dp->dir_name)) == 0)
          &&(*(ramfs_dp->last_entry->fn + strlen(ramfs_dp->dir_name)) != '\0')) {
            name = ramfs_dp->last_entry->fn + strlen(ramfs_dp->dir_name) + 1;
            data = name;
            len  = strlen(ramfs_dp->last_entry->fn) - strlen(ramfs_dp->dir_name);

            search = 0;

            for (i = 0; i < len; i++) {
                if (*name == '/') {
                    search = 1;
                    break;
                }
                name++;
            }
        }

        ramfs_dp->last_entry = ramfs_ll_get_next(&g_file_ll, ramfs_dp->last_entry);
    }

    if (ramfs_dp->last_entry != NULL) {
        strcpy(fn, data);
    } else {
        return RAMFS_ERR_NOT_EXIST;
    }

    return RAMFS_OK;
}

int32_t ramfs_closedir(void *dp)
{
    ramfs_dir_t *ramfs_dp = (ramfs_dir_t *)dp;

    if (ramfs_dp->dir_name != NULL) {
        ramfs_mm_free(ramfs_dp->dir_name);
    }

    return RAMFS_OK;
}

int32_t ramfs_rmdir(const char *path)
{
    ramfs_entry_t *entry;
    int            ret = -1;
    int flag = 0;

    /* if file existed in the dir return error ! */
    RAMFS_LL_READ(g_file_ll, entry) {
        if ((strncmp(entry->fn, path, strlen(path)) == 0)
            && (*(char*)(entry->fn + strlen(path)) == '/')) {
            flag = 1;
            break;
        } else {
            ret = ramfs_search_link_part(entry, (char *)path);
            if (ret == RAMFS_OK) {
                flag = 1;
                break;
            }
        }
    }

    if (flag == 1) {
        return RAMFS_ERR_DENIED;
    }

    /* if no file existed in the dir remove the dir ! */
    RAMFS_LL_READ(g_file_ll, entry) {
        if ((strncmp(entry->fn, path, strlen(path)) == 0) && (entry->is_dir == 1)) {
            ramfs_ll_remove(&g_file_ll, entry);
            ramfs_mm_free(entry->fn);
            entry->fn = NULL;
            ramfs_mm_free(entry);
        }
    }

    return RAMFS_OK;
}

int32_t ramfs_stat(const char *path, ramfs_stat_t *st)
{
    ramfs_entry_t *entry = ramfs_entry_get(path);

    if (st == NULL) {
        return RAMFS_ERR_INV_PARAM;
    }

    if (entry != NULL) {
        st->st_size = entry->size;

        if (entry->ar == 1) {
            st->st_mode |= RAMFS_MODE_RD;
        }

        if (entry->aw == 1) {
            st->st_mode |= RAMFS_MODE_WR;
        }

        if (entry->is_dir == 1) {
            st->is_dir = 1;
        }

    } else {
        return RAMFS_ERR_NOT_EXIST;
    }

    return RAMFS_OK;
}

int ramfs_add_link(ramfs_entry_t *entry, char *path)
{
    link_name_t *link_name   = entry->link;
    link_name_t *link_name_c = NULL;

    link_name_c = ramfs_mm_alloc(sizeof(link_name_t));

    if (link_name_c != NULL) {
        link_name_c->name = ramfs_mm_alloc(strlen(path));
        if (link_name_c->name != NULL) {
            strcpy(link_name_c->name, path);
            link_name_c->next = NULL;
            entry->link_count += 1;
        } else {
            ramfs_mm_free(link_name_c);
            return RAMFS_ERR_MALLOC;
        }
    } else {
        return RAMFS_ERR_MALLOC;
    }

    if (link_name == NULL) {
        entry->link = link_name_c;
    } else {
        while (link_name->next != NULL) {
            link_name = link_name->next;
        }

        link_name->next = link_name_c;
    }

    return 0;
}

int ramfs_remove_link(ramfs_entry_t *entry, char *path)
{
    link_name_t *link_name   = entry->link;
    link_name_t *link_name_c = NULL;
    link_name_t *link_name_l = NULL;

    if (link_name == NULL) {
         return RAMFS_ERR_INV_PARAM;
    }

    if (strncmp(link_name->name, path, strlen(path)) == 0) {
        ramfs_mm_free(link_name->name);
        link_name->name = ramfs_mm_alloc(strlen(link_name->next->name));
        if (link_name->name == NULL) {
            return RAMFS_ERR_MALLOC;
        }

        strcpy(link_name->name, link_name->next->name);
        link_name->next = link_name->next->next;
        entry->link_count -= 1;
    } else {
        link_name_l = link_name;
        link_name_c = link_name->next;

        while (link_name_c != NULL) {
            if (strncmp(link_name_c->name, path, strlen(path)) == 0) {
                link_name_l->next = link_name_c->next;
                ramfs_mm_free(link_name_c->name);
                ramfs_mm_free(link_name_c);
                entry->link_count -= 1;
            }

            link_name_l = link_name_c;
            link_name_c = link_name_c->next;
        }
    }

    return 0;
}

int ramfs_link(const char *path1, const char *path2)
{
    ramfs_entry_t *entry = NULL;
    int            ret = -1;

    entry = ramfs_entry_get(path2);
    if (entry != NULL) {
        return RAMFS_ERR_PATH;
    }

    entry = ramfs_entry_get(path1);
    if (entry == NULL) {
        return RAMFS_ERR_NOT_EXIST;
    }

    if ((entry != NULL) && (entry->is_dir == 1)) {
        return RAMFS_ERR_PATH;
    }

    if (entry->link_count >= RAMFS_LINK_MAX) {
        return RAMFS_ERR_LINK_MAX;
    }

    ret = ramfs_add_link(entry, (char *)path2);
    if (ret != 0) {
        return RAMFS_ERR_MALLOC;
    }

    return 0;
}

int ramfs_unlink(const char *path)
{
    ramfs_entry_t *entry = NULL;
    int            ret = -1;

    entry = ramfs_entry_get(path);
    if (entry == NULL) {
        return RAMFS_ERR_PATH;
    }

    if (strncmp(entry->fn, path, strlen(path)) == 0) {
        ret = ramfs_remove(path);
    } else {
        ret = ramfs_remove_link(entry, (char *)path);
    }

    return ret;
}

int32_t ramfs_pathconf(int32_t name)
{
    int32_t val = 0;

    switch (name) {
        case _PC_FILESIZEBITS :
            val = -1;
            break;
        case _PC_LINK_MAX :
            val = RAMFS_LINK_MAX;
            break;
        case _PC_MAX_CANON :
            val = -1;
            break;
        case _PC_MAX_INPUT :
            val = -1;
            break;
        case _PC_NAME_MAX :
            val = RAMFS_NAME_MAX;
            break;
        case _PC_PATH_MAX :
            val = RAMFS_PATH_MAX;
            break;
        case _PC_PIPE_BUF :
            val = -1;
            break;
        case _PC_2_SYMLINKS :
            val = -1;
            break;
        case _PC_ALLOC_SIZE_MIN :
            val = RAMFS_ALLOC_SIZE_MIN;
            break;
        case _PC_REC_INCR_XFER_SIZE :
            val = -1;
            break;
        case _PC_REC_MAX_XFER_SIZE :
            val = -1;
            break;
        case _PC_REC_MIN_XFER_SIZE :
            val = -1;
            break;
        case _PC_REC_XFER_ALIGN :
            val = -1;
            break;
        case _PC_SYMLINK_MAX :
            val = -1;
            break;
        case _PC_CHOWN_RESTRICTED :
            val = -1;
            break;
        case _PC_NO_TRUNC :
            val = -1;
            break;
        case _PC_VDISABLE :
            val = -1;
            break;
        case _PC_ASYNC_IO :
            val = -1;
            break;
        case _PC_PRIO_IO :
            val = -1;
            break;
        case _PC_SYNC_IO :
            val = -1;
            break;
        default:
            val = -1;
            break;
    }

    return val;
}

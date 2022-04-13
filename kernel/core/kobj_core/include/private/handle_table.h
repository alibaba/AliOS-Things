/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef K_HANDLE_TABLE_H
#define K_HANDLE_TABLE_H

#include <stdint.h>
#include "object_api.h"
#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define PROCESS_ID_ERROR ((task_group_t *)(-1))

/* second-level table */
#define OBJ_INDEX_GET(obj_hdl)          ((obj_hdl) & (OBJ_HANDLE_INDEX_CNT - 1))
#define OBJ_ENTRY_INDEX_GET(obj_hdl)    ((obj_hdl) & (OBJ_HANDLE_TBL_SIZE - 1))
#define OBJ_TABLE_INDEX_GET(obj_hdl)    (OBJ_INDEX_GET(obj_hdl) >> OBJ_HANDLE_ENTRY_BITS)

/* increment for generation number */

#define OBJ_HANDLE_GEN_INCR              OBJ_HANDLE_INDEX_CNT        /* 00010000 */
#define OBJ_HANDLE_GEN_MAX               (((1 << OBJ_HANDLE_GEN_BITS) - 1) <<          \
        OBJ_HANDLE_INDEX_BITS)

typedef struct handle_table_entry {
    klist_t     handle_node;             /* node for obj_core_t.obj_handle_list */
    union {
        void            *entry_obj_id;   /* if allocated: object ID */
        int             next_free;       /* if free: link to next free entry */
    } u;
    obj_handle_t obj_handle;             /* object handle */
} handle_table_entry_t;

typedef struct handle_table {
    struct handle_table_entry   handle_table [OBJ_HANDLE_TBL_SIZE];
    struct master_handle_table *master_tbl;
} handle_table_t;

#ifdef __cplusplus
}
#endif

#endif /* K_HANDLE_TABLE_H */

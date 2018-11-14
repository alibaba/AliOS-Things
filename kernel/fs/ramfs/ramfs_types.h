/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RAMFS_TYPES_H
#define RAMFS_TYPES_H

#define RAMFS_MAGIC  0x890910
#define RAMFS_LETTER 'U'

/* Description of a file entry */
typedef struct {
    char     *fn;
    void     *data;
    uint32_t  size;       /* Data length in bytes */
    uint16_t  refs;       /* Open count */
    uint8_t   const_data : 1;
    uint8_t   is_dir : 1;
    uint8_t   ar : 1;     /* 1: Access for read is enabled */
    uint8_t   aw : 1;     /* 1: Access for write is enabled */
} ramfs_entry_t;

/* Description of a file */
typedef struct {
    ramfs_entry_t *entry; /* Pointer to the file entry */
    uint32_t       rwp; /* Read write pointer */
} ramfs_file_t;

/* Description of a directory */
typedef struct {
    char          *dir_name;
    ramfs_entry_t *last_entry;
} ramfs_dir_t;

typedef uint8_t ramfs_ll_node_t;

/* Description of a linked list */
typedef struct {
    uint32_t         size;
    ramfs_ll_node_t *head;
    ramfs_ll_node_t *tail;
} ramfs_ll_t;

#endif /* RAMFS_TYPES_H */


/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef RAMFS_H
#define RAMFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define RAMFS_MAGIC 0x890910

#define RAMFS_LETTER 'U'

#define LL_READ(list, i) \
    for (i = ll_get_head(&list); i != NULL; i = ll_get_next(&list, i))
#define LL_READ_BACK(list, i) \
    for (i = ll_get_tail(&list); i != NULL; i = ll_get_prev(&list, i))

    /*Description of a file entry */
    typedef struct
    {
        char    *fn_d;
        void    *data_d;
        uint32_t size; /*Data length in bytes*/
        uint16_t oc;   /*Open Count*/
        uint8_t  const_data : 1;
        uint8_t  is_dir : 1;
        uint8_t  ar : 1; /*1: Access for read is enabled */
        uint8_t  aw : 1; /*1: Access for write is enabled */
    } ramfs_ent_t;

    /*File descriptor, used to handle opening an entry more times simultaneously
     Contains unique informations about the specific opening*/
    typedef struct
    {
        ramfs_ent_t *ent; /*Pointer to the entry*/
        uint32_t     rwp; /*Read Write Pointer*/

    } ramfs_file_t;

    /* Read directory descriptor.
     * It is used to to iterate through the entries in a directory*/
    typedef struct
    {
        char        *dir_name;
        ramfs_ent_t *last_ent;
    } ramfs_dir_t;

    typedef struct
    {
        mode_t  st_mode;
        off_t   st_size;
        uint8_t is_dir;
    } ramfs_stat_t;

    typedef enum
    {
        RAMFS_RES_OK = 0,
        RAMFS_RES_HW_ERR,     /*Low level hardware error*/
        RAMFS_RES_FS_ERR,     /*Error in the file system structure */
        RAMFS_RES_NOT_EX,     /*Driver, file or directory is not exists*/
        RAMFS_RES_FULL,       /*Disk full*/
        RAMFS_RES_LOCKED,     /*The file is already opened*/
        RAMFS_RES_DENIED,     /*Access denied. Check 'fs_open' modes and write
                                 protect*/
        RAMFS_RES_BUSY,       /*The file system now can't handle it, try later*/
        RAMFS_RES_TOUT,       /*Process time outed*/
        RAMFS_RES_NOT_IMP,    /*Requested function is not implemented*/
        RAMFS_RES_OUT_OF_MEM, /*Not enough memory for an internal operation*/
        RAMFS_RES_INV_PARAM,  /*Invalid parameter among arguments*/
        RAMFS_RES_UNKNOWN,    /*Other unknown error*/
    } ramfs_res_t;

    typedef enum
    {
        RAMFS_MODE_WR = 0x01,
        RAMFS_MODE_RD = 0x02,
    } ramfs_mode_t;

    typedef struct
    {
        uint32_t total_size;
        uint32_t free_cnt;
        uint32_t free_size;
        uint32_t free_biggest_size;
        uint32_t used_cnt;
        uint8_t  used_pct;
        uint8_t  frag_pct;
    } lv_mem_monitor_t;

    /*Dummy type to make handling easier*/
    typedef uint8_t ll_node_t;

    /*Description of a linked list*/
    typedef struct
    {
        uint32_t   n_size;
        ll_node_t *head;
        ll_node_t *tail;
    } ll_t;

    /**
     * Create a driver for ufs and initialize it.
     */
    void ramfs_init(void);

    /**
     * Give the state of the ufs
     * @return true if ufs is initialized and can be used else false
     */
    bool ramfs_ready(void);

    /**
     * Open a file in ufs
     * @param file_p pointer to a ramfs_file_t variable
     * @param fn name of the file. There are no directories so e.g. "myfile.txt"
     * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g.
     * FS_MODE_WR | FS_MODE_RD)
     * @return RAMFS_RES_OK: no error, the file is opened
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_open(void *file_p, const char *fn, ramfs_mode_t mode);

    /**
     * Create a file with a constant data
     * @param fn name of the file (directories are not supported)
     * @param const_p pointer to a constant data
     * @param len length of the data pointed by 'const_p' in bytes
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_create_const(const char *fn, const void *const_p,
                                   uint32_t len);

    /**
     * Close an opened file
     * @param file_p pointer to an 'ramfs_file_t' variable. (opened with
     * ramfs_open)
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_close(void *file_p);

    /**
     * Remove a file. The file can not be opened.
     * @param fn '\0' terminated string
     * @return RAMFS_RES_OK: no error, the file is removed
     *         RAMFS_RES_DENIED: the file was opened, remove failed
     */
    ramfs_res_t ramfs_remove(const char *fn);

    /**
     * Read data from an opened file
     * @param file_p pointer to an 'ramfs_file_t' variable. (opened with
     * ramfs_open )
     * @param buf pointer to a memory block where to store the read data
     * @param btr number of Bytes To Read
     * @param br the real number of read bytes (Byte Read)
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_read(void *file_p, void *buf, uint32_t btr, uint32_t *br);

    /**
     * Write data to an opened file
     * @param file_p pointer to an 'ramfs_file_t' variable. (opened with
     * ramfs_open)
     * @param buf pointer to a memory block which content will be written
     * @param btw the number Bytes To Write
     * @param bw The real number of written bytes (Byte Written)
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_write(void *file_p, const void *buf, uint32_t btw,
                            uint32_t *bw);

    /**
     * Set the read write pointer. Also expand the file size if necessary.
     * @param file_p pointer to an 'ramfs_file_t' variable. (opened with
     * ramfs_open )
     * @param pos the new position of read write pointer
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_seek(void *file_p, uint32_t pos);

    /**
     * Give the position of the read write pointer
     * @param file_p pointer to an 'ramfs_file_t' variable. (opened with
     * ramfs_open )
     * @param pos_p pointer to to store the result
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_tell(void *file_p, uint32_t *pos_p);

    /**
     * Truncate the file size to the current position of the read write pointer
     * @param file_p pointer to an 'ramfs_file_t' variable. (opened with
     * ramfs_open )
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_trunc(void *file_p);

    /**
     * Give the size of the file in bytes
     * @param file_p file_p pointer to an 'ramfs_file_t' variable. (opened with
     * ramfs_open )
     * @param size_p pointer to store the size
     * @return RAMFS_RES_OK: no error, the file is read
     *         any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_size(void *file_p, uint32_t *size_p);

    /**
     * get access info
     * @param path The path of the file
     * @param mode the info to get
     * @return RAMFS_RES_OK or any error from lv__fs_res_t enum
     */
    ramfs_res_t ramfs_access(const char *path, int mode);

    /**
     * Create a directory
     * @param path The path of the file
     * @return RAMFS_RES_OK or any error from lv__fs_res_t enum
     */
    ramfs_res_t ramfs_mkdir(const char *path);

    /**
     * Initialize a ramfs_read_dir_t variable to directory reading
     * @param rddir_p pointer to a 'ufs_read_dir_t' variable
     * @param path The path of the file
     * @return RAMFS_RES_OK or any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_dir_open(void *rddir_p, const char *path);

    /**
     * Read the next file name
     * @param dir_p pointer to an initialized 'ufs_read_dir_t' variable
     * @param fn pointer to buffer to sore the file name
     * @return RAMFS_RES_OK or any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_dir_read(void *dir_p, char *fn);

    /**
     * Close the directory reading
     * @param rddir_p pointer to an initialized 'ufs_read_dir_t' variable
     * @return RAMFS_RES_OK or any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_dir_close(void *rddir_p);

    /**
     * Get file info
     * @param[in]   path  The path of the file to find information about
     * @param[out]  st    The stat buffer to write to.
     * @return RAMFS_RES_OK or any error from ramfs_res_t enum
     */
    ramfs_res_t ramfs_stat(const char *path, ramfs_stat_t *st);

    /**
     * Give the size of a drive
     * @param total_p pointer to store the total size [kB]
     * @param free_p pointer to store the free site [kB]
     * @return RAMFS_RES_OK or any error from 'fs_res_t'
     */
    ramfs_res_t ramfs_free(uint32_t *total_p, uint32_t *free_p);

    /**********************
     *      MACROS
     **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifndef _PLAT_GEN_H_
#define _PLAT_GEN_H_


#include <stdint.h>

typedef enum {
    RO_READ = 1,
    RO_WRITE = 2
} rsvd_part_perm_t;

/*
 * get device unique id
 *
 * param: out:    dev_id: device uinque id
 *        in_out: id_len: device uinque id length
 *
 * return: 0: success
 *         -1: fail
 *
*/
int get_dev_id(uint8_t *dev_id, uint32_t *id_len);

/*
 * open the reserved partition, if no file system return 0 directly
 *
 * parametr: in: flag: RO_READ: user only has read permission
 *                     RO_WRITE: user only has write permission
 *                     RO_READ | RO_WRITE: user has read and write permission
 *
 * return: the new file descriptor: success
 *         -1: fail
*/
int open_rsvd_part(int flag);

/*
 * write reserved  partition
 *
 * parametr: in: fd: file handle, can be ignored if no file system
 *               offset: the offset of the reserved partition
 *               data: the data need to write
 *               data_len: the length of the data need to write
 *
 * return: 0: success
 *         -1: fail
*/
int write_rsvd_part(int fd, uint32_t offset, void *data, uint32_t data_len);

/*
 * read reserved partition
 *
 * parametr: in:  fd: file handle, can be ignored if no file system
 *                offset: the offset of the reserved partition
 *                read_len: the length of the data need to read
 *           out: buffer: the data read from the reserved part

 * return: 0: success
           -1: fail
 */
int read_rsvd_part(int fd, uint32_t offset, void *buffer, uint32_t read_len);

/*
 * close the file descriptor of reserved partition, if no file system, return 0 directly
 *
 * parametr: in: fd: file descriptor of the reserved part

 * return: 0: success
           -1: fail
 */

int close_rsvd_part(int fd);
#endif //_PLAT_GEN_H_


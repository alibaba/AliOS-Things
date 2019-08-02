/*
 * Copyright (C) 2016-2019 Alibaba Group Holding Limited
 */

#ifndef _LS_HAL_KM_H_
#define _LS_HAL_KM_H_

#include <stdint.h>

#define LS_HAL_READ     1
#define LS_HAL_WRITE    2

/*
 * get device unique id
 *
 * note: if dev_id is NULL and *id_len is 0, return -1
 *        and set *id_len to the real length of the id
 *
 * param: out:  dev_id: device uinque id should be smaller than 100 byte
 *        in_out: id_len: device uinque id length
 *
 * return: 0: success
 *         -1: fail
 *
*/
int ls_hal_get_dev_id(uint8_t *dev_id, uint32_t *id_len);

/*
 * open the reserved partition that user has read and write permission
 *
 * note: if not support file system, return 0 directly
 *
 * parametr: in: flag: LS_HAL_READ: user only has read permission
 *                     LS_HAL_WRITE: user only has write permission
 *                     LS_HAL_READ | LS_HAL_WRITE: user has read and write permission
 *
 * return: the new file descriptor: success
 *         -1: fail
*/
int ls_hal_open_rsvd_part(int flag);

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
int ls_hal_write_rsvd_part(int fd, uint32_t offset, void *data, uint32_t data_len);

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
int ls_hal_read_rsvd_part(int fd, uint32_t offset, void *buffer, uint32_t read_len);

/*
 * close the file descriptor of reserved partition, if no file system support, return 0 directly
 *
 * parametr: in: fd: file descriptor of the reserved part

 * return: 0: success
           -1: fail
 */

int ls_hal_close_rsvd_part(int fd);

#endif /* _LS_HAL_KM_H_ */


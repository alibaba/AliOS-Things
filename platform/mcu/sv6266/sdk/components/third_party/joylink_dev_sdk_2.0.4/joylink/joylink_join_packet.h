#ifndef _JOIN_PKG_H
#define _JOIN_PKG_H

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include "joylink.h"

/**
 * brief: 
 *
 * @Param: name
 * @Param: id
 * @Param: phead
 * @Param: data
 * @Param: len
 *
 * @Returns: 
 */
int
joylink_join_pkg_add_data(char *name, int id, JLPacketHead_t *phead, char *data, int len);

/**
 * brief: 
 *
 * @Param: name
 * @Param: id
 * @Param: out_len
 *
 * @Returns: 
 */
char *
joylink_join_pkg_join_data(char *name, int id, int *out_len);

/**
 * brief: 
 *
 * @Param: name
 * @Param: id
 *
 * @Returns: 
 */
int
joylink_join_pkg_clean_node_by_name_id(char *name, int id);
#endif

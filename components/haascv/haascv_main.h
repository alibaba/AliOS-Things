/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This file provides haascv entry.
*/

#ifndef HAASCV_MAIN_H
#define HAASCV_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

extern int haascv_run(int argc, char **argv);
extern int haascv_init(void);
extern void haascv_add_path(char * path,int8_t length);


#ifdef __cplusplus
}
#endif

#endif /* HAASCV_MAIN_H */

/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef TSL_FILE_H
#define TSL_FILE_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

long tsl_file_get_size(FILE *fp);

int tsl_file_read(char *buf, FILE *fp, long size);

int tsl_file_write(const char *buf, FILE *fp, long size);

char *tsl_read_from_file(const char *filename, int *buf_size);

/*
int tsl_write_to_file(const char *filename, const char *buf, int buf_size);
*/

#ifdef __cplusplus
}
#endif

#endif

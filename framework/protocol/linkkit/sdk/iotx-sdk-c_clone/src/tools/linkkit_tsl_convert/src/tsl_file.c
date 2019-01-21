/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include <stdlib.h>
#include "tsl_file.h"
#include "iot_import.h"

long tsl_file_get_size(FILE *fp)
{
    long filesize = 0;
    if (!fp) {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return filesize;
}
int tsl_file_read(char *buf, FILE *fp, long size)
{
    size_t count = 0;
    size_t readlen = 0;
    long read_size = size;

    while ((readlen = fread(buf, 1, read_size - count, fp)) > 0) {
        count += readlen;
    }
    if (read_size != count) {
        return -1;
    }
    return 0;
}

int tsl_file_write(const char *buf, FILE *fp, long size)
{
    size_t count = 0;
    size_t writelen = 0;
    long write_size = size;

    while ((writelen = fwrite(buf, 1, write_size - count, fp)) > 0) {
        count += writelen;
    }
    if (write_size != count) {
        return -1;
    }
    return 0;
}


char *tsl_read_from_file(const char *filename, int *buf_size)
{
    char *buf = NULL;
    int file_size = 0;
    FILE *fp = NULL;

    if (!filename || !buf_size) {
        goto do_exit;
    }

    fp = fopen(filename, "rb");
    if (!fp) {
        goto do_exit;
    }

    file_size = tsl_file_get_size(fp);
    if (file_size < 0) {
        goto do_exit;
    }

    buf = (char *)HAL_Calloc(file_size + 1, 1);
    if (!buf) {
        goto do_exit;
    }

    if (tsl_file_read(buf, fp, file_size)) {
        HAL_Free(buf);
        goto do_exit;
    }
    buf[file_size] = '\0';
    *buf_size = file_size;
    fclose(fp);
    return buf;
do_exit:
    if (fp) {
        fclose(fp);
    }
    return NULL;
}

/*
int tsl_write_to_file(const char *filename, const char *buf, int buf_size)
{

}
*/

/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

#include <string.h>
//#include <dirent.h>

//#include <sys/types.h>
//#include <sys/stat.h>

#include "core/fs.h"
#include "aos_fs.h"

lxb_char_t *
lexbor_fs_file_easy_read(const lxb_char_t *full_path, size_t *len)
{
#if 1
    void *fh;
    long size;
    size_t nread;
    lxb_char_t *data;
    struct aos_stat st = {0};



    fh = aos_open((const char *)full_path, O_RDONLY);
    if (fh == NULL) {
        aos_printf("%s ============= %d\n\r",__func__, __LINE__);
        goto error;
    }
#if 0
    if (fseek(fh, 0L, SEEK_END) != 0) {
        goto error_close;
    }
#endif
    aos_stat(full_path, &st);

    //size = HAL_Ftell(fh);
    size = st.st_size;
    aos_printf("%s ============= %d  size = %d   %d\n\r",__func__, __LINE__, size, st.st_size);
    if (size < 0) {
        aos_printf("%s ============= %d  size = %d   %d\n\r",__func__, __LINE__, size, st.st_size);
        goto error_close;
    }

    data = aos_malloc(size + 1);
    if (data == NULL) {
        aos_printf("%s ============= %d  size = %d\n\r",__func__, __LINE__, size);
        goto error_close;
    }

    nread = aos_read(fh, data, size);
    if (nread != (size_t) size) {
        aos_free(data);
        aos_printf("%s ============= %d  size = %d  nread = %d\n\r",__func__, __LINE__, size, nread);

        goto error_close;
    }

    aos_close(fh);

    data[size] = '\0';

    if (len != NULL) {
        *len = nread;
    }

    return data;

error_close:

    aos_close(fh);

error:

    if (len != NULL) {
        *len = 0;
    }
#endif
    return NULL;
}

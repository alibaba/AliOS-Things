/**
 * Copyright (2018) Bestech Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEM_FILE_H
#define MEM_FILE_H

#include <stdio.h>
#include "mem_list.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif



/*
   mem file path: /mem://xxxxxx.xxx

*/

typedef struct {
    osSemaphoreId _osSemaphoreId;
    osSemaphoreDef_t _osSemaphoreDef;
#if defined(CMSIS_OS_RTX) && (osCMSIS < 0x20000U)
    uint32_t _semaphore_data[2];
#endif
} rw_sem_t;

typedef struct{
    char *buff;
    rw_sem_t* rw_lock;
    int  length;
    int  r_offset;
    int  w_offset;
    int  users;
}mem_file_cb_t;

typedef struct{
    char *name;
    char *addr;
    int  length;
}mem_file_t;

typedef struct{
    bool (*is_valid_mem_file)(const char *path);
    bool(* is_valid_mem_file_fd)(mem_file_cb_t *stream);	
    mem_file_cb_t* (*fopen)(const char *path, char *rw_mode);
    int (*fseek)(mem_file_cb_t *stream, long offset, int whence);
    long (*ftell)(mem_file_cb_t *stream);
    void (*rewind)(mem_file_cb_t *stream);    
    int  (*fgetpos)(mem_file_cb_t *stream, fpos_t *pos);
    int (*fsetpos)(mem_file_cb_t *stream, const fpos_t *pos);
    size_t (*fread)(void *ptr, size_t size, size_t nmemb, mem_file_cb_t *stream);
    size_t (*fwrite)(const void *ptr, size_t size, size_t nmemb,mem_file_cb_t *stream);
    int (*fclose)(mem_file_cb_t *stream);
    int (*flen)(mem_file_cb_t *stream);
    int (*fregister)(const char *path,  char *addr, int size);
    
}mem_file_ops_t;

 mem_file_ops_t* get_mem_file_system(void);

#ifdef __cplusplus
}
#endif


#endif // MEM_FILE_H

/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __FS_ADAPT_H__
#define __FS_ADAPT_H__

#ifdef __cplusplus
extern "C"
{
#endif

    void fs_adapt_init(void);
    int flush(int fd);
    int filesize(int fd);
    int relax(int fd, int size);
    int open(const char *pathname, int flags, ...);
    uint32_t read(int fd, void *buf, size_t count);
    uint32_t write(int fd, const void *buf, size_t nbyte);
    int lseek(int fildes, int offset, int whence);
    //int _is_virtual_path(const char *path);
    int close(int fd);
#ifdef __cplusplus
}
#endif

#endif /* __FS_ADAPT_H__ */
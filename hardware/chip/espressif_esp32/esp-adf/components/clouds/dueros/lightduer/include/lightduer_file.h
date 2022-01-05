/**
 * Copyright (2018) Baidu Inc. All rights reserved.
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
// Author: Chen Yun (chenyun08@baidu.com)
//
// Description: Wrapper for file operation.

#ifndef BAIDU_DUER_LIBDUER_DEVICE_FRAMEWORK_CORE_LIGHTDUER_FILE_H
#define BAIDU_DUER_LIBDUER_DEVICE_FRAMEWORK_CORE_LIGHTDUER_FILE_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *duer_file_t;

/*
 * The file system operation callbacks
 */
typedef duer_file_t (*duer_fs_open)(const char *filename, const char *mode);
typedef size_t (*duer_fs_read)(void *buffer, size_t size, size_t count, duer_file_t stream);
typedef size_t (*duer_fs_write)(const void *buffer, size_t size, size_t count, duer_file_t stream);
typedef long(*duer_fs_size)(duer_file_t stream);
typedef int (*duer_fs_seek)(duer_file_t stream, long offset, int origin);
typedef long(*duer_fs_tell)(duer_file_t stream);
typedef int (*duer_fs_close)(duer_file_t stream);

/*
 * Initial the file system callbacks for file opeation
 *
 * @Param f_open, open file
 * @Param f_read, read block of data from stream
 * @Param f_write, write block of data to stream
 * @Param f_seek, reposition stream position indicator
 * @Param f_tell, get current position in stream
 * @Param f_close, close file
 */
DUER_EXT void duer_file_init(duer_fs_open f_open,
                             duer_fs_read f_read,
                             duer_fs_write f_write,
                             duer_fs_size f_size,
                             duer_fs_seek f_seek,
                             duer_fs_tell f_tell,
                             duer_fs_close f_close);

/*
 * @describe Opens the file whose name is specified in the parameter filename and associates
 *  it with a stream that can be identified in future operations by the FILE pointer returned.
 * @Param filename, C string containing the name of the file to be opened.Its value shall
 *  follow the file name specifications of the running environment and can include a path
 *  (if supported by the system).
 * @Param mode,C string containing a file access mode. It can be:"r" read: Open file for
 *  input operations. The file must exist."w" write: Create an empty file for output operations.
 *  If a file with the same name already exists, its contents are discarded and the file is
 *  treated as a new empty file."a" append: Open file for output at the end of a file. Output
 *  operations always write data at the end of the file, expanding it. Repositioning operations
 *  (fseek, fsetpos, rewind) are ignored. The file is created if it does not exist."r+" read/update:
 *  Open a file for update (both for input and output). The file must exist.
 *  "w+"    write/update: Create an empty file and open it for update (both for input and output). If a file with the same
 *  name already exists its contents are discarded and the file is treated as a new empty file.
 *  "a+"    append/update: Open a file for update (both for input and output) with all output operations writing data at the
 *  end of the file. Repositioning operations (fseek, fsetpos, rewind) affects the next input operations, but output operations
 *  move the position back to the end of file. The file is created if it does not exist.
 * @Return If the file is successfully opened, the function returns a pointer to a FILE object that can be used to identify
 *  the stream on future operations.Otherwise, a null pointer is returned.
 */
DUER_EXT duer_file_t duer_file_open(const char *filename, const char *mode);

/*
 * @describe Read block of data from stream
 * @Param buffer, Pointer to a block of memory with a size of at least (size*count) bytes, converted to a void*.
 * @Param size, in bytes, of each element to be read.size_t is an unsigned integral type.
 * @Param count,Number of elements, each one with a size of size bytes.size_t is an unsigned integral type.
 * @Param, Pointer to a FILE object that specifies an input stream.
 * @Return The total number of elements successfully read is returned.
 */
DUER_EXT size_t duer_file_read(void *buffer,
                               size_t size,
                               size_t count,
                               duer_file_t stream);

/*
 * @describe Write block of data to stream
 * @Param buffer, Pointer to the array of elements to be written, converted to a const void*.
 * @Param size, Size in bytes of each element to be written.size_t is an unsigned integral type.
 * @Param count,Number of elements, each one with a size of size bytes.size_t is an unsigned integral type.
 * @Param, Pointer to a FILE object that specifies an output stream.
 * @Return The total number of elements successfully written is returned.
 */
DUER_EXT size_t duer_file_write(const void *buffer,
                                size_t size,
                                size_t count,
                                duer_file_t stream);

/*
 * @describe gets the size of a file.
 * @Param stream, Pointer to the open file object structure.
 * @Return Returns the size of the file in unit of byte.
 */
DUER_EXT long duer_file_size(duer_file_t stream);

/*
 * @describe Sets the position indicator associated with the stream to a new position.
 * @Param stream,pointer to a FILE object that identifies the stream.
 * @Param offset,Binary files: Number of bytes to offset from origin.Text files: Either zero, or a value returned by ftell.
 * @Param origin,Position used as reference for the offset. It is specified by one of the following constants defined
 *  in <cstdio> exclusively to be used as arguments for this function:
 *          Constant    Reference position
 *          SEEK_SET    Beginning of file
 *          SEEK_CUR    Current position of the file pointer
 *          SEEK_END    End of file *
 * @Return if successful, the function returns zero.Otherwise, it returns non-zero value.
 */
DUER_EXT int duer_file_seek(duer_file_t stream, long offset, int origin);

/*
 * @describe returns the current value of the position indicator of the stream.
 * @Param stream, pointer to a FILE object that identifies the stream.
 * @Return On success, the current value of the position indicator is returned.
 *         On failure, -1L is returned, and errno is set to a system-specific positive value.
 */
DUER_EXT long duer_file_tell(duer_file_t stream);

/*
 * @describe closes the file associated with the stream and disassociates it.
 * @Param stream, pointer to a FILE object that specifies the stream to be closed.
 * @Return if the stream is successfully closed, a zero value is returned.
 *         On failure, EOF is returned.
 */
DUER_EXT int duer_file_close(duer_file_t stream);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIBDUER_DEVICE_FRAMEWORK_CORE_LIGHTDUER_FILE_H

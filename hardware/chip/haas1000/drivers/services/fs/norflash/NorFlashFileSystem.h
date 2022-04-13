/* mbed Microcontroller Library
 * Copyright (c) 2006-2012 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef MBED_NORFLASHFILESYSTEM_H
#define MBED_NORFLASHFILESYSTEM_H

#include "mbed.h"
#include "FATFileSystem.h"
#include <stdint.h>

/** Access the filesystem on an NorFlash Card
 *
 * @code
 * #include "mbed.h"
 * #include "NorFlashFileSystem.h"
 *
 * NorFlashFileSystem nf("nf"); 
 *
 * int main() {
 *     FILE *fp = fopen("/nf/myfile.txt", "w");
 *     fprintf(fp, "Hello World!\n");
 *     fclose(fp);
 * }
 */
class NorFlashFileSystem : public FATFileSystem {
public:

    /** Create the File System for accessing an NorFlash Card
     *
     * @param name The name used to access the virtual filesystem
     */
    NorFlashFileSystem(const char* name);
	virtual ~NorFlashFileSystem();
    virtual int disk_initialize();
    virtual int disk_status();
    virtual int disk_read(uint8_t* buffer, uint64_t block_number, uint8_t count);
    virtual int disk_write(const uint8_t* buffer, uint64_t block_number, uint8_t count);
    virtual int disk_sync();
    virtual uint64_t disk_sectors();

protected:

    uint64_t _nf_sectors();
    uint64_t _sectors;
    uint32_t _erase_size;

    int _is_initialized;
};

#endif

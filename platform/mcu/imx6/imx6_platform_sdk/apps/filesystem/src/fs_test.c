/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*----------------------------------------------------------------------------
		File Includes
----------------------------------------------------------------------------*/
#include "sdk.h"
#include "types.h"
#include "platform_init.h"
#include "print_version.h"
#include "filesystem/fsapi.h"
#include "filesystem/fat/fstypes.h"
#include "filesystem/fat/fat_internal.h"
#include "timer/epit.h"
#include "usdhc/usdhc_ifc.h"
#include "core/cortex_a9.h"
#include "core/mmu.h"
#include "registers/regsepit.h"

//#define FS_DEBUG
#ifdef FS_DEBUG
#define dbprintf(fmt, arg...) printf(fmt, ##arg)
#else
#define dbprintf(fmt, ...)
#endif

#define FS_CHUNK_SIZE_512B      512
#define FS_CHUNK_SIZE_1K        1024
#define FS_CHUNK_SIZE_4K        (1024*4)
#define FS_CHUNK_SIZE_32K       (1024*32)
#define FS_CHUNK_SIZE_256K      (1024*256)
#define FS_CHUNK_SIZE_1M        (1024*1024)
#define FS_CHUNK_SIZE_4M        (1024*1024*4)
#define FS_CHUNK_SIZE_16M       (1024*1024*16)

#define TEST_BUFFER_ADDR    0x28000000
#define TEST_FILE_SIZE  FS_CHUNK_SIZE_4M
#define DeviceNum 0
#define SD_TEST_OFFSET  0x1000000   //offset from 16MB
typedef struct fs_rw_speed_data_s {
    int WorkMode;               // PIO or ADMA
    int ChunkSize;
    float ReadSpeed;
    float WriteSpeed;
    float RawReadSpeed;
    float RawWriteSpeed;
} fs_rw_speed_data_t;

char test_mode[2][8] = { "PIO", "ADMA" };
char status_tag[4][2] = { "-", "\\", "|", "/" };

fs_rw_speed_data_t rw_test_data[] = {
    {.WorkMode = 0,.ChunkSize = FS_CHUNK_SIZE_512B},
    {.WorkMode = 0,.ChunkSize = FS_CHUNK_SIZE_1K},
    {.WorkMode = 0,.ChunkSize = FS_CHUNK_SIZE_4K},
    {.WorkMode = 0,.ChunkSize = FS_CHUNK_SIZE_32K},
    {.WorkMode = 0,.ChunkSize = FS_CHUNK_SIZE_1M},
    {.WorkMode = 1,.ChunkSize = FS_CHUNK_SIZE_512B},
    {.WorkMode = 1,.ChunkSize = FS_CHUNK_SIZE_1K},
    {.WorkMode = 1,.ChunkSize = FS_CHUNK_SIZE_4K},
    {.WorkMode = 1,.ChunkSize = FS_CHUNK_SIZE_32K},
    {.WorkMode = 1,.ChunkSize = FS_CHUNK_SIZE_1M}
};

uint8_t readfile[] = "testin.dat";
uint8_t writefile[] = "testoutx.dat";
char dataSize[16];
uint32_t epit_instance = HW_EPIT2, epit_freq = 0;

extern uint32_t g_usdhc_instance;
extern void print_media_fat_info(uint32_t);

static void get_data_size(uint32_t size, char *str)
{
    if (!(size % FS_CHUNK_SIZE_1M))
        sprintf(str, "%dMB", size / FS_CHUNK_SIZE_1M);
    else if (!(size % FS_CHUNK_SIZE_1K))
        sprintf(str, "%dKB", size / FS_CHUNK_SIZE_1K);
    else
        sprintf(str, "%dB", size);
}

int fat_write_speed_test(fs_rw_speed_data_t * test_data, int i)
{
    uint8_t *WriteBuffer = NULL;
    uint32_t BytesWritten = 0;
    uint32_t TimeCount = 0;
    uint32_t ChunkSize = test_data->ChunkSize;
    int32_t count = 0, nMB = 0;
    int32_t fout;

    set_card_access_mode(test_data->WorkMode, 0);

    char *buf = (char *)memchr(writefile, '.', strlen((char *)writefile));
    buf[-1] = (char)('0' + i);

    if ((fout = Fopen(writefile, (uint8_t *) "w")) < 0) {
        printf("Can't open the file: %s !\n", writefile);
        return ERROR_GENERIC;
    }

    WriteBuffer = (uint8_t *) TEST_BUFFER_ADDR;

    epit_freq = get_main_clock(IPG_CLK);
    epit_init(epit_instance, CLKSRC_IPG_CLK, epit_freq / 1000000,
              SET_AND_FORGET, 10000, WAIT_MODE_EN | STOP_MODE_EN);
    epit_setup_interrupt(epit_instance, NULL, FALSE);
    epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode

    _raw_puts("\tWriting file ");
    _raw_puts((char *)writefile);
    _raw_puts("   ");
    while (BytesWritten < TEST_FILE_SIZE) {
        if ((count =
             Fwrite(fout, (uint8_t *) (WriteBuffer + BytesWritten), ChunkSize)) != ChunkSize) {
            printf("Fwrite failed. Exit.\n");
            Fclose(fout);
            return ERROR_GENERIC;
        }
        BytesWritten += count;
        if (BytesWritten / FS_CHUNK_SIZE_1M > nMB) {
            _raw_puts("\b");
            _raw_puts(status_tag[nMB % 4]);
            nMB++;
        }
    }

    TimeCount = epit_get_counter_value(epit_instance);
    epit_counter_disable(epit_instance); //polling mode

    TimeCount = (0xFFFFFFFF - TimeCount) / 1000;    //ms
    get_data_size(BytesWritten, dataSize);
    printf(" Done! write %s in %d ms\n", dataSize, TimeCount);

    test_data->WriteSpeed = ((float)BytesWritten / FS_CHUNK_SIZE_1M * 1000) / (float)TimeCount;

    Fclose(fout);

    return 0;
}

int sd_raw_write_speed_test(fs_rw_speed_data_t * test_data, int i)
{
    uint8_t *WriteBuffer = NULL;
    uint32_t BytesWritten = 0;
    uint32_t TimeCount = 0;
    uint32_t ChunkSize = test_data->ChunkSize;
    int32_t nMB = 0;
    uint32_t sdcard_offset = SD_TEST_OFFSET;

    set_card_access_mode(test_data->WorkMode, 0);

    WriteBuffer = (uint8_t *) TEST_BUFFER_ADDR;

    epit_freq = get_main_clock(IPG_CLK);
    epit_init(epit_instance, CLKSRC_IPG_CLK, epit_freq / 1000000,
              SET_AND_FORGET, 10000, WAIT_MODE_EN | STOP_MODE_EN);
    epit_setup_interrupt(epit_instance, NULL, FALSE);
    epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode

    _raw_puts("\tWriting sdcard directly     ");
    while (BytesWritten < TEST_FILE_SIZE) {
        card_data_write(g_usdhc_instance, (int *)(WriteBuffer + BytesWritten), ChunkSize,
                        sdcard_offset + BytesWritten);
        BytesWritten += ChunkSize;
        if (BytesWritten / FS_CHUNK_SIZE_1M > nMB) {
            _raw_puts("\b");
            _raw_puts(status_tag[nMB % 4]);
            nMB++;
        }
    }

    TimeCount = epit_get_counter_value(epit_instance);
    epit_counter_disable(epit_instance); //polling mode

    TimeCount = (0xFFFFFFFF - TimeCount) / 1000;    //ms
    get_data_size(BytesWritten, dataSize);
    printf(" Done! write %s in %d ms\n", dataSize, TimeCount);

    test_data->RawWriteSpeed = ((float)BytesWritten / FS_CHUNK_SIZE_1M * 1000) / (float)TimeCount;

    return 0;
}

void create_input_file(void)
{
    uint8_t *WriteBuffer = NULL;
    uint32_t BytesWritten = 0;
    uint32_t ChunkSize = FS_CHUNK_SIZE_1M;
    int32_t count = 0, nMB = 0;
    int32_t fout;

    set_card_access_mode(1, 0); // ADMA mode

    if ((fout = Fopen(readfile, (uint8_t *) "w")) < 0) {
        printf("Can't open the file: %s !\n", readfile);
    }

    WriteBuffer = (uint8_t *) TEST_BUFFER_ADDR;

    _raw_puts("\tCreating file ");
    _raw_puts((char *)readfile);
    _raw_puts("    ");
    while (BytesWritten < TEST_FILE_SIZE) {
        if ((count =
             Fwrite(fout, (uint8_t *) (WriteBuffer + BytesWritten), ChunkSize)) != ChunkSize) {
            printf("Fwrite failed. Exit.\n");
            Fclose(fout);
        }
        BytesWritten += count;
        if (BytesWritten / FS_CHUNK_SIZE_1M > nMB) {
            _raw_puts("\b");
            _raw_puts(status_tag[nMB % 4]);
            nMB++;
        }
    }

    get_data_size(BytesWritten, dataSize);
    printf(" Done! write %s\n", dataSize);

    Fclose(fout);
}

int fat_read_speed_test(fs_rw_speed_data_t * test_data, int i)
{
    int32_t fin;
    uint8_t *ReadBuffer;
    uint32_t FileSize = 0;
    int ChunkSize = test_data->ChunkSize;
    int BytesRead = 0;
    int32_t count = 0, nMB = 0;
    uint32_t TimeCount = 0;

    set_card_access_mode(test_data->WorkMode, 0);

    if ((fin = Fopen(readfile, (uint8_t *) "r")) < 0) {
        // Create readfile if it does not exist
        create_input_file();
        if ((fin = Fopen(readfile, (uint8_t *) "r")) < 0) {
            printf("Can't open the file: %s !\n", readfile);
            return ERROR_GENERIC;
        }
    }

    FileSize = GetFileSize(fin);
    dbprintf("File %s of size %d opened!\n", readfile, FileSize);

    ReadBuffer = (uint8_t *) TEST_BUFFER_ADDR;

    epit_freq = get_main_clock(IPG_CLK);
    epit_init(epit_instance, CLKSRC_IPG_CLK, epit_freq / 1000000,
              SET_AND_FORGET, 10000, WAIT_MODE_EN | STOP_MODE_EN);
    epit_setup_interrupt(epit_instance, NULL, FALSE);
    epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode

    _raw_puts("\tReading file ");
    _raw_puts((char *)readfile);
    _raw_puts("     ");
    while (BytesRead < TEST_FILE_SIZE) {
        if ((count = Fread(fin, (uint8_t *) (ReadBuffer + BytesRead), ChunkSize)) < 0) {
            Fclose(fin);
            return ERROR_GENERIC;
        }

        BytesRead += count;
        if (BytesRead / FS_CHUNK_SIZE_1M > nMB) {
            _raw_puts("\b");
            _raw_puts(status_tag[nMB % 4]);
            nMB++;
        }
        if (count < ChunkSize) {
            break;
        }
    }
    TimeCount = epit_get_counter_value(epit_instance);
    epit_counter_disable(epit_instance); //polling mode

    TimeCount = (0xFFFFFFFF - TimeCount) / 1000;    //ms
    get_data_size(BytesRead, dataSize);
    printf(" Done! read %s in %d ms\n", dataSize, TimeCount);
    test_data->ReadSpeed = ((float)BytesRead / FS_CHUNK_SIZE_1M * 1000) / (float)TimeCount;

    Fclose(fin);
    return 0;
}

int sd_raw_read_speed_test(fs_rw_speed_data_t * test_data, int i)
{
    uint8_t *ReadBuffer;
    int ChunkSize = test_data->ChunkSize;
    int BytesRead = 0;
    int32_t nMB = 0;
    uint32_t TimeCount = 0;
    uint32_t sdcard_offset = SD_TEST_OFFSET;

    set_card_access_mode(test_data->WorkMode, 0);

    ReadBuffer = (uint8_t *) TEST_BUFFER_ADDR;

    epit_freq = get_main_clock(IPG_CLK);
    epit_init(epit_instance, CLKSRC_IPG_CLK, epit_freq / 1000000,
              SET_AND_FORGET, 10000, WAIT_MODE_EN | STOP_MODE_EN);
    epit_setup_interrupt(epit_instance, NULL, FALSE);
    epit_counter_enable(epit_instance, 0xFFFFFFFF, 0);   //polling mode

    _raw_puts("\tReading sdcard directly     ");
    while (BytesRead < TEST_FILE_SIZE) {
        card_data_read(g_usdhc_instance, (int *)(ReadBuffer + BytesRead), ChunkSize,
                       sdcard_offset + BytesRead);

        BytesRead += ChunkSize;
        if (BytesRead / FS_CHUNK_SIZE_1M > nMB) {
            _raw_puts("\b");
            _raw_puts(status_tag[nMB % 4]);
            nMB++;
        }
    }
    TimeCount = epit_get_counter_value(epit_instance);
    epit_counter_disable(epit_instance); //polling mode

    TimeCount = (0xFFFFFFFF - TimeCount) / 1000;    //ms
    get_data_size(BytesRead, dataSize);
    printf(" Done! read %s in %d ms\n", dataSize, TimeCount);
    test_data->RawReadSpeed = ((float)BytesRead / FS_CHUNK_SIZE_1M * 1000) / (float)TimeCount;

    return 0;
}

void dump_test_results(void)
{
    int i = 0;
    printf
        ("_______________________________________________________________________________________________\n");
    printf
        ("R/W Size        Test Mode       Per Access      Read Speed      Write Speed    Raw RD Speed    Raw WR Speed\n");
    while (i < sizeof(rw_test_data) / sizeof(rw_test_data[0])) {
        printf("%4dMB          ", TEST_FILE_SIZE / FS_CHUNK_SIZE_1M);

        printf("%-12s    ", test_mode[rw_test_data[i].WorkMode]);

        get_data_size(rw_test_data[i].ChunkSize, dataSize);
        printf("%8s        ", dataSize);

        printf("%2d.%04dMB       ", (int)rw_test_data[i].ReadSpeed,
               (int)(rw_test_data[i].ReadSpeed * 10000) % 10000);
        printf("%2d.%04dMB       ", (int)rw_test_data[i].WriteSpeed,
               (int)(rw_test_data[i].WriteSpeed * 10000) % 10000);

        printf("%2d.%04dMB       ", (int)rw_test_data[i].RawReadSpeed,
               (int)(rw_test_data[i].RawReadSpeed * 10000) % 10000);
        printf("%2d.%04dMB       \n", (int)rw_test_data[i].RawWriteSpeed,
               (int)(rw_test_data[i].RawWriteSpeed * 10000) % 10000);

        i++;
    }
    printf
        ("_______________________________________________________________________________________________\n");
}

int fat_test(void)
{
    int TestResult = 0;
    int i = 0;

    /* configure cache */
    arm_icache_enable();
    arm_dcache_invalidate();
    mmu_enable();
    arm_dcache_enable();

    set_card_access_mode(0, 0);

    dbprintf("FSInit				");
    if (FSInit(NULL, bufy, maxdevices, maxhandles, maxcaches) != SUCCESS) {
        TestResult = -1;
        dbprintf("FAIL\n");
    } else {
        dbprintf("PASS\n");
    }

    /*init the drive */
    FSDriveInit(DeviceNum);
    SetCWDHandle(DeviceNum);

#ifdef FS_DEBUG
    print_media_fat_info(DeviceNum);
#endif

    while (i < sizeof(rw_test_data) / sizeof(rw_test_data[0])) {
        get_data_size(rw_test_data[i].ChunkSize, dataSize);
        printf("\n** Test loop #%d, read/write in %s per access in %s mode\n", i, dataSize,
               test_mode[rw_test_data[i].WorkMode]);

        /*read input file to TEST BUFFER */
        fat_read_speed_test(&rw_test_data[i], i);

        /*Dump the test buffer to SD card */
        fat_write_speed_test(&rw_test_data[i], i);

        /*Read from raw SD card */
        sd_raw_read_speed_test(&rw_test_data[i], i);

        /*Write to raw SD card */
        sd_raw_write_speed_test(&rw_test_data[i], i);

        i++;
    }

    dump_test_results();

    return 0;
}

int main(void)
{
    platform_init();

    print_version();

    fat_test();

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <endian.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inc/nftl_conf.h>

#define PAGE_SIZE NFTL_PAGE_SIZE
#define PAGES_PER_BLOCK NFTL_PAGE_PER_BLOCK_NUMS
#if PAGES_PER_BLOCK < 2
#error Less than 2 pages on one block is not supported.
#endif
#define BLOCK_SIZE NFTL_BLOCK_SIZE
#define SPARE_LEN_PER_PAGE 16

#define LOGICAL_PARTITION_NUMS NFTL_LOGICAL_PARTITION_NUM
#define LOGICAL_PARTITION0_BLOCK_NUMS NFTL_PART0_BLOCK_NUMS
#define LOGICAL_PARTITION1_BLOCK_NUMS NFTL_PART1_BLOCK_NUMS
#define LOGICAL_PARTITION2_BLOCK_NUMS NFTL_PART2_BLOCK_NUMS
#define PART0_BUFFER_BLOCK_NUMS NFTL_PART0_BUFFER_BLOCK_NUMS
#define PART1_BUFFER_BLOCK_NUMS NFTL_PART1_BUFFER_BLOCK_NUMS
#define PART2_BUFFER_BLOCK_NUMS NFTL_PART2_BUFFER_BLOCK_NUMS

#define LOGICAL_PARTITION_TOTAL_BLOCK_NUMS NFTL_LOGICAL_TOTAL_BLOCK_NUMS
#define BUFFER_TOTAL_BLOCK_NUMS NFTL_BUFFER_TOTAL_BLOCK_NUMS

#define SIMULATOR_TOTAL_BLOCKS NFTL_BLOCK_NUMS

#define MAIN_AREA_SIZE (BLOCK_SIZE * SIMULATOR_TOTAL_BLOCKS)
#define BLOCK_SPARE_AREA_SIZE (SPARE_LEN_PER_PAGE * PAGES_PER_BLOCK)
#define TOTAL_SPARE_AREA_SIZE (SPARE_LEN_PER_PAGE * PAGES_PER_BLOCK * SIMULATOR_TOTAL_BLOCKS)
#define SIMULATOR_TOTAL_SIZE (MAIN_AREA_SIZE + TOTAL_SPARE_AREA_SIZE)

uint8_t file_buffer[SIMULATOR_TOTAL_SIZE] = {0xff};
uint32_t partition_page_write_offset[LOGICAL_PARTITION_NUMS] = {
    0,
#if LOGICAL_PARTITION_NUMS >= 2
    PAGE_SIZE * PAGES_PER_BLOCK * (LOGICAL_PARTITION0_BLOCK_NUMS + PART0_BUFFER_BLOCK_NUMS),
#endif
#if LOGICAL_PARTITION_NUMS == 3
    PAGE_SIZE * PAGES_PER_BLOCK * (LOGICAL_PARTITION0_BLOCK_NUMS + PART0_BUFFER_BLOCK_NUMS + LOGICAL_PARTITION1_BLOCK_NUMS + PART1_BUFFER_BLOCK_NUMS),
#endif
};
uint32_t partition_spare_write_offset[LOGICAL_PARTITION_NUMS] = {
    0,
#if LOGICAL_PARTITION_NUMS >= 2
    BLOCK_SPARE_AREA_SIZE * (LOGICAL_PARTITION0_BLOCK_NUMS + PART0_BUFFER_BLOCK_NUMS),
#endif
#if LOGICAL_PARTITION_NUMS == 3
    BLOCK_SPARE_AREA_SIZE * (LOGICAL_PARTITION0_BLOCK_NUMS + PART0_BUFFER_BLOCK_NUMS + LOGICAL_PARTITION1_BLOCK_NUMS + PART1_BUFFER_BLOCK_NUMS),
#endif
};

static void print_usage(const char *name)
{
    printf("Usage:\n  %s <out_img> <[systemA_img] [systemB_img] [data_img]>\n\n"
           "Note: At least one (and at most 3) input image should be provided.\n", name);
}

int main(int argc, char* argv[])
{
    int ret;
    FILE *fin, *fout;
    char *infile, *outfile;

    if ((argc < 3) || (argc > (LOGICAL_PARTITION_NUMS + 2))) {
        print_usage(argv[0]);
        return -1;
    }

    int infile_idx = 0;
    unsigned long logical_page_offset;
    uint32_t last_idx = 0;
    uint8_t last_page[PAGE_SIZE + SPARE_LEN_PER_PAGE];
    memset(last_page, 0xff, sizeof(last_page));
    memset(file_buffer, 0xff, sizeof(file_buffer));
    while (infile_idx < (argc - 2)) {
        int pageidx = 0, write_pageidx = 0;

        infile = argv[2 + infile_idx];
        logical_page_offset = 0;

        printf("Processing input image file %s ...\n", infile);

        fin = fopen(infile, "rb");
        if (!fin) {
            printf("Failed to open input image, errno: %d.\n", errno);
            return -1;
        }

        struct stat statbuf;
        ret = stat(infile, &statbuf);
        if (ret == -1) {
            printf("Failed to stat file %s\n", infile);
            return -1;
        }

        size_t size=statbuf.st_size;

        assert(size % BLOCK_SIZE == 0);
        printf("Totally %ld bytes (%ld blocks).\n", size, size / BLOCK_SIZE);

        ret = fseek(fin, 0, SEEK_SET);
        if (ret == -1) {
            printf("%s %d failed to fseek, errno: %d.\n", __func__, __LINE__, errno);
            return -1;
        }

        while (pageidx < (size / BLOCK_SIZE * PAGES_PER_BLOCK)) {
            uint8_t *page = file_buffer + partition_page_write_offset[infile_idx] +
                            write_pageidx * PAGE_SIZE;
            uint8_t *spare = file_buffer + MAIN_AREA_SIZE +
                             partition_spare_write_offset[infile_idx] +
                             write_pageidx * SPARE_LEN_PER_PAGE;
            size_t fret;
            uint32_t lno, ver;

            fret = fread(page, 1, PAGE_SIZE, fin);
            if (fret != PAGE_SIZE) {
                printf("%s %d fread failed, errno: %d\n", __func__, __LINE__, errno);
                return -1;
            }

            ver = htole32(0);
            lno = htole32(logical_page_offset + pageidx);
            memcpy(spare, &lno, sizeof(lno));
            memcpy(spare + sizeof(lno), &ver, sizeof(ver));

            printf("%d-th page proccessed.\n", pageidx);

            pageidx++;
            write_pageidx++;

            memcpy(last_page + last_idx, &lno, sizeof(lno));
            last_idx += sizeof(lno);
            memcpy(last_page + last_idx, &ver, sizeof(ver));
            last_idx += sizeof(ver);

            assert(last_idx <= PAGE_SIZE);

            if ((pageidx % (PAGES_PER_BLOCK - 1)) == 0) {
                memcpy(page + PAGE_SIZE, last_page, PAGE_SIZE);
                memcpy(spare + SPARE_LEN_PER_PAGE, last_page +
                       sizeof(last_page) - SPARE_LEN_PER_PAGE,
                       SPARE_LEN_PER_PAGE);
                write_pageidx++;
                memset(last_page, 0xff, sizeof(last_page));
                last_idx = 0;

                printf("%d blocks processed.\n", pageidx / PAGES_PER_BLOCK + 1);
            }
        }

        if (pageidx % (PAGES_PER_BLOCK - 1) != 0) {
            printf("%d pages left on last block (%d).\n",
                   pageidx % (PAGES_PER_BLOCK - 1),
                   pageidx / PAGES_PER_BLOCK + 1);
        }

        printf("Complete processing input image file %s.\n", infile);

        memset(last_page, 0xff, sizeof(last_page));
        last_idx = 0;
        fclose(fin);
        infile_idx++;
    }

    outfile = argv[1];
    fout = fopen(outfile, "wb");
    if (!fout) {
        printf("Failed to open output image file %s.\n", outfile);
        return -1;
    }

    ret = fseek(fout, 0, SEEK_SET);
    if (ret == -1) {
        printf("%s %d failed to fseek, errno: %d.\n", __func__, __LINE__, errno);
        return -1;
    }

    size_t fret = fwrite(file_buffer, 1, sizeof(file_buffer), fout);
    if (fret != sizeof(file_buffer)) {
        printf("%s %d fwrite failed, errno: %d\n", __func__, __LINE__, errno);
        return -1;
    }

    fclose(fout);

    return 0;
}

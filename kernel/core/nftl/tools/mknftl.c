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

#define PAGE_SIZE 4096
#define PAGES_PER_BLOCK 64
#define BLOCK_SIZE (PAGES_PER_BLOCK * PAGE_SIZE)
#define SPARE_LEN_PER_PAGE 16
#if PAGES_PER_BLOCK < 2
    #error Less than 2 pages on one block is not supported.
#endif

static void print_usage(const char *name)
{
    printf("Usage:\n  %s <out_img> <in_img>\n", name);
}

int main(int argc, char *argv[])
{
    int ret;
    FILE *fin, *fout;
    char *infile, *outfile;

    if (argc != 3) {
        print_usage(argv[0]);
        return -1;
    }

    outfile = argv[1];
    fout = fopen(outfile, "wb");
    if (!fout) {
        printf("Failed to open output image file %s.\n", outfile);
        return -1;
    }

    uint8_t page[PAGE_SIZE + SPARE_LEN_PER_PAGE] = {0xff};
    uint8_t last_page[PAGE_SIZE + SPARE_LEN_PER_PAGE] = {0xff};
    uint32_t last_idx = 0;
    int pageidx = 0;

    infile = argv[2];

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

    size_t size = statbuf.st_size;
    assert(size % PAGE_SIZE == 0);

    int total_pages = size / PAGE_SIZE;

    printf("Totally %ld bytes (raw: %d pages, nftl: %d).\n",
           size, total_pages, total_pages / (PAGES_PER_BLOCK - 1) + total_pages);

    ret = fseek(fin, 0, SEEK_SET);
    if (ret == -1) {
        printf("%s %d failed to fseek, errno: %d.\n", __func__, __LINE__, errno);
        return -1;
    }

    memset(last_page, 0xff, sizeof(last_page));
    while (pageidx < total_pages) {
        int pageid = 0;
        size_t fret;
        uint32_t lno, ver;

        memset(page, 0xff, sizeof(page));

        fret = fread(page, 1, PAGE_SIZE, fin);
        if (fret != PAGE_SIZE) {
            printf("%s %d fread failed, errno: %d\n", __func__, __LINE__, errno);
            return -1;
        }

        ver = htole32(0);
        lno = htole32(pageidx);
        memcpy(page + PAGE_SIZE, &lno, sizeof(lno));
        memcpy(page + PAGE_SIZE + sizeof(lno), &ver, sizeof(ver));

        fret = fwrite(page, 1, sizeof(page), fout);
        if (fret != sizeof(page)) {
            printf("%s %d fwrite failed, errno: %d\n", __func__, __LINE__, errno);
            return -1;
        }

        //printf("%d-th page proccessed.\n", pageidx);

        pageidx++;

        memcpy(last_page + last_idx, &lno, sizeof(lno));
        last_idx += sizeof(lno);
        memcpy(last_page + last_idx, &ver, sizeof(ver));
        last_idx += sizeof(ver);

        assert(last_idx <= PAGE_SIZE);

        if ((pageidx % (PAGES_PER_BLOCK - 1)) == 0) {
            fret = fwrite(last_page, 1, sizeof(last_page), fout);
            if (fret != sizeof(page)) {
                printf("%s %d fwrite failed, errno: %d\n", __func__, __LINE__, errno);
                return -1;
            }

            memset(last_page, 0xff, sizeof(last_page));
            last_idx = 0;

            printf("%d blocks proccessed.\n", pageidx / (PAGES_PER_BLOCK - 1));
        }
    }

    if (pageidx % (PAGES_PER_BLOCK - 1) != 0) {
        printf("%d pages left on last block (%d).\n",
               pageidx % (PAGES_PER_BLOCK - 1),
               pageidx / (PAGES_PER_BLOCK - 1) + 1);
    }

    printf("Complete processing input image file %s.\n", infile);

    fclose(fin);
    fclose(fout);

    return 0;
}

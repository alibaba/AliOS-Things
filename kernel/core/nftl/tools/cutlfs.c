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

#define EMPTY_BYTE 0x00

static void print_usage(const char *name)
{
    printf("Usage: %s -b <block_size> -i <input_img_file> -o <output_img_file>\n", name);
}

static bool is_empty(uint8_t *buf, int siz)
{
    for (int i = 0; i < siz; i++) {
        if (buf[i] != EMPTY_BYTE) {
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    int ret;
    FILE *fin, *fout;
    int opt = 0;
    int block_size = 0;
    char *infile = NULL, *outfile = NULL;
    uint8_t *block_buffer;

    while ((opt = getopt(argc, argv, "i:o:b:h?")) != -1) {
        switch (opt) {
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'b':
                block_size = atoi(optarg);
                break;
            case 'h':
            /* FALLTHROUGH */
            case '?':
            /* FALLTHROUGH */
            default:
                print_usage(argv[0]);
                break;
        }
    }

    if (!infile || !outfile || (block_size <= 0)) {
        print_usage(argv[0]);
        return -1;
    }

    block_buffer = (uint8_t *)malloc(block_size);
    if (!block_buffer) {
        printf("Failed to malloc.\n");
        return -1;
    }

    fin = fopen(infile, "rb");
    if (!fin) {
        printf("Failed to open input image file %s.\n", outfile);
        free(block_buffer);
        return -1;
    }

    fout = fopen(outfile, "wb");
    if (!fout) {
        printf("Failed to open output image file %s.\n", outfile);
        free(block_buffer);
        fclose(fin);
        return -1;
    }

    struct stat statbuf;
    ret = stat(infile, &statbuf);
    if (ret == -1) {
        printf("Failed to stat file %s\n", infile);
        goto err;
    }

    size_t size = statbuf.st_size;
    assert(size % block_size == 0);

    printf("Origin image totally %ld bytes (%ld blocks).\n", size, size / block_size);

    ret = fseek(fin, 0, SEEK_SET);
    if (ret == -1) {
        printf("%s %d failed to fseek, errno: %d.\n", __func__, __LINE__, errno);
        goto err;
    }

    size_t fret;
    int blkidx = 0, last_non_empty = 0;
    while (blkidx < (size / block_size)) {
        memset(block_buffer, 0, block_size);
        fret = fread(block_buffer, 1, block_size, fin);
        if (fret != block_size) {
            printf("%s %d fread failed, errno: %d\n", __func__, __LINE__, errno);
            goto err;
        }

        if (!is_empty(block_buffer, block_size)) {
            printf("Non-empty block %d\n", blkidx);
            if (last_non_empty < (blkidx - 1)) {
                ret = fseek(fin, (last_non_empty + 1) * block_size, SEEK_SET);
                if (ret == -1) {
                    printf("%s %d failed to fseek, errno: %d.\n", __func__, __LINE__, errno);
                    goto err;
                }

                while (last_non_empty < (blkidx - 1)) {
                    memset(block_buffer, 0, block_size);
                    fret = fread(block_buffer, 1, block_size, fin);
                    if (fret != block_size) {
                        printf("%s %d fread failed, errno: %d\n", __func__, __LINE__, errno);
                        goto err;
                    }

                    last_non_empty++;
                    fret = fwrite(block_buffer, 1, block_size, fout);
                    if (fret != block_size) {
                        printf("%s %d fwrite failed, errno: %d\n", __func__, __LINE__, errno);
                        goto err;
                    }
                }

                assert(last_non_empty == ((blkidx - 1)));

                ret = fseek(fin, (blkidx + 1) * block_size, SEEK_SET);
                if (ret == -1) {
                    printf("%s %d failed to fseek, errno: %d.\n", __func__, __LINE__, errno);
                    goto err;
                }
            }

            last_non_empty = blkidx;
            fret = fwrite(block_buffer, 1, block_size, fout);
            if (fret != block_size) {
                printf("%s %d fwrite failed, errno: %d\n", __func__, __LINE__, errno);
                goto err;
            }
        } else {
            printf("Empty block %d\n", blkidx);
        }

        blkidx++;
    }

    printf("Output image totally %d bytes (%d blocks).\n", block_size * (last_non_empty + 1), last_non_empty + 1);
    printf("Completed!\n");
    return 0;

err:
    free(block_buffer);
    fclose(fin);
    fclose(fout);
    return -1;
}

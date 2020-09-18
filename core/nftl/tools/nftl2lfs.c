#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <endian.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define TAGPAGE_MAPPING_RECORD_LEN 8

typedef struct {
    uint32_t phypageidx;
    uint32_t ver;
} map_record_t;

#define MAX_LFS_PAGES (2048*64)
static map_record_t g_nftl_map[MAX_LFS_PAGES];

static void print_usage(const char *name)
{
    printf("\nUsage:\n");
    printf("  %s -i <nftl_img> -o <out_img> -p <nftl_page_size> -k <nftl_page_per_block> "
           "-l <lfs_total_pages> -s <spare_len_per_page> [-n <nftl_img_blocks>]\n", name);
}

static void to_map_record(uint8_t *buf, map_record_t *r, uint32_t phyidx, uint32_t *lidx)
{
    r->phypageidx = phyidx;
    r->ver = le32toh(*(uint32_t *)(buf+4));
    *lidx = le32toh(*(uint32_t *)buf);
}

static bool is_valid_page(uint8_t *buf, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        if (buf[i] != 0xff) return true;
    }

    return false;
}

static void dump_map(map_record_t *map, uint32_t size)
{
    printf("logical\t\tphysical\tversion\n");
    for (int i = 0; i < size; i++) {
        printf("%d\t\t%d\t\t%d\n", i, map[i].phypageidx, map[i].ver);
    }
}

int main(int argc, char* argv[])
{
    int opt, ret, errcode = -1, i;
    char *infile = NULL, *outfile = NULL;
    FILE *fin = NULL, *fout = NULL;
    size_t fret;
    uint32_t page_size = 0, page_per_block = 0, spare_len = 0, block_size;
    uint32_t nftl_blocks = 0, lfs_pages = 0, pages_on_last_blk = 0;
    //map_record_t *g_nftl_map = NULL;
    uint8_t *page_buf = NULL;
    map_record_t r = {0};
    uint8_t *g_in_img = NULL;
    uint32_t blkoff, tagpage_off;
    uint32_t logical_page_idx;
    uint32_t blkidx = 0, pagesiz_with_spare, blksiz_with_spare;

    while ((opt = getopt(argc, argv, "i:o:p:n:l:k:s:h?")) != -1) {
        switch (opt) {
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'p':
                page_size = atoi(optarg);
                break;
            case 'n':
                nftl_blocks = atoi(optarg);
                break;
            case 'l':
                lfs_pages = atoi(optarg);
                break;
            case 'k':
                page_per_block = atoi(optarg);
                break;
            case 's':
                spare_len = atoi(optarg);
                break;
            case 'h':
            /* FALLTHROUGH */
            case '?':
            /* FALLTHROUGH */
            default:
                printf("Unrecognized option %c\n", opt);
                print_usage(argv[0]);
                return -1;
        }
    }

    if (!infile || !outfile || !lfs_pages || !page_per_block || !spare_len || !page_size) {
        printf("Invalid argument provided.\n");
        print_usage(argv[0]);
        return -1;
    }

    block_size = page_size * page_per_block;
    pagesiz_with_spare = page_size + spare_len;
    blksiz_with_spare = pagesiz_with_spare * page_per_block;

    struct stat statbuf;
    ret = stat(infile, &statbuf);
    if (ret == -1) {
        printf("Failed to stat file %s\n", infile);
        return -1;
    }

    size_t size = statbuf.st_size;
    assert(size % pagesiz_with_spare == 0);

    if (nftl_blocks == 0) {
        nftl_blocks = size / blksiz_with_spare;
    }

    if (size % blksiz_with_spare != 0) {
        pages_on_last_blk = (size % blksiz_with_spare) / pagesiz_with_spare;
    }

    /* nftl image may be a partial mapped one, not fully restored!!! */
    //assert(lfs_pages <= (nftl_blocks * (page_per_block - 1)));

    /* hmmm, I hate reading the whole file too, but I don't
     * know why fread page by page do not work.
     */
    g_in_img = (uint8_t *)calloc(size, 1);
    if (!g_in_img) {
        printf("malloc failed.\n");
        return -1;
    }

#if 0
    g_nftl_map = (map_record_t *)malloc(lfs_pages);
    if (!g_nftl_map) {
        printf("malloc failed.\n");
        free(g_in_img);
        return -1;
    }
#endif

    page_buf = (uint8_t *)malloc(page_size);
    if (!page_buf) {
        printf("page buffer malloc failed.\n");
        //free(g_nftl_map);
        free(g_in_img);
        return -1;
    }

    printf("Dump settings:\n");
    printf("  block size: %d\n", block_size);
    printf("  page size: %d\n", page_size);
    printf("  pages per block: %d\n", page_per_block);
    printf("  nftl blocks: %d\n", nftl_blocks);
    printf("  logical blocks/pages: %d\n", lfs_pages);
    printf("  nftl image file: %s\n", infile);
    printf("  restored image file: %s\n", outfile);
    printf("  \n\n");

    for (int i = 0; i < lfs_pages; i++) {
        g_nftl_map[i].phypageidx = -1;
        g_nftl_map[i].ver = 0;
    }

    fin = fopen(infile, "rb");
    if (!fin) {
        printf("Failed to open input image, errno: %d.\n", errno);
        return -1;
    }

    memset(g_in_img, 0, size);
    fret = fread(g_in_img, 1, size, fin);
    if (fret != size) {
        printf("%s %d fread failed, errno: %d, eof: %s\n", __func__,
               __LINE__, errno, feof(fin) > 0 ? "true" : "false");
        return -1;
    }

    //fclose(fin);

    while (blkidx < nftl_blocks) {
        blkoff = blkidx * blksiz_with_spare,
        tagpage_off = blkoff + pagesiz_with_spare * (page_per_block - 1);

        memset(page_buf, 0, pagesiz_with_spare);
        memcpy(page_buf, g_in_img + tagpage_off, pagesiz_with_spare);

        /* check tag page */
        for (i = 0; i < spare_len; i++) {
            if (*(page_buf + page_size + i) != 0xff) break;
        }

        /* if tag page not valid, go check first page, and traverse each
         * page on the block for mapping if applicable.
         */
        if (i >= spare_len) {
            /* read each page to get mapping */
            memset(page_buf, 0, pagesiz_with_spare);

            for (int j = 0; j < (page_per_block - 1); j++) {
                if (!is_valid_page(g_in_img + blkoff + pagesiz_with_spare * j, pagesiz_with_spare)) break;
                memcpy(page_buf + j * TAGPAGE_MAPPING_RECORD_LEN,
                       g_in_img + blkoff + pagesiz_with_spare * j + page_size,
                       TAGPAGE_MAPPING_RECORD_LEN);
            }
        }

        /* restore mapping */
        for (int j = 0; j < (page_per_block - 1); j++) {
            if (*(uint32_t *)(page_buf + j * TAGPAGE_MAPPING_RECORD_LEN) == 0xffffffff) {
                /* invalid page means record terminated here */
                break;
            }

            to_map_record(page_buf + j * TAGPAGE_MAPPING_RECORD_LEN, &r,
                          blkidx * page_per_block + j, &logical_page_idx);
            assert(logical_page_idx < lfs_pages);

            if ((g_nftl_map[logical_page_idx].phypageidx == -1) ||
                (g_nftl_map[logical_page_idx].ver < r.ver)) {
                g_nftl_map[logical_page_idx].phypageidx = r.phypageidx;
                g_nftl_map[logical_page_idx].ver = r.ver;
            }
        }

        blkidx++;
    }

    /* handle last partial block, may happend if it is a tool-generated nftl image */
    uint8_t *page_off = NULL;
    if (pages_on_last_blk > 0) {
        blkoff = blkidx * blksiz_with_spare;
        for (int j = 0; j < pages_on_last_blk; j++) {
            page_off = g_in_img + blkoff + pagesiz_with_spare * j;
            if (!is_valid_page(page_off, pagesiz_with_spare)) {
                break;
            }

            to_map_record(page_off + page_size, &r,
                          blkidx * page_per_block + j,
                          &logical_page_idx);
            assert(logical_page_idx < lfs_pages);

            if (g_nftl_map[logical_page_idx].phypageidx == -1) {
                g_nftl_map[logical_page_idx].phypageidx = r.phypageidx;
                g_nftl_map[logical_page_idx].ver = r.ver;
            } else if (g_nftl_map[logical_page_idx].ver < r.ver) {
                g_nftl_map[logical_page_idx].ver = r.ver;
            }
        }
    }

#if 0
    /* examine restored mapping again */
    uint32_t lastmapping1 = 0, lastmapping2 = 0;
    for (uint32_t i = 0; i < lfs_pages; i++) {
        if (g_nftl_map[i].phypageidx != -1) {
            printf("Invalid restored map!\n");
            dump_map(g_nftl_map, lfs_pages);
            goto err;
        }
    }
#endif

    dump_map(g_nftl_map, lfs_pages);

    /* now all mapping restored correctly, rebuild the image */
    fout = fopen(outfile, "wb");
    if (!fout) {
        printf("Failed to open output image, errno: %d.\n", errno);
        fclose(fin);
        goto err;
    }

    for (i = 0; i < lfs_pages; i++) {
        if (g_nftl_map[i].phypageidx == -1) {
            /* not mapped page */
            memset(page_buf, 0xff, page_size);
        } else {
            memset(page_buf, 0, pagesiz_with_spare);
            memcpy(page_buf, g_in_img + pagesiz_with_spare *\
                   g_nftl_map[i].phypageidx, page_size);
        }

        fret = fwrite(page_buf, 1, page_size, fout);
        if (fret != page_size) {
            printf("%s %d fwrite failed, errno: %d\n", __func__, __LINE__, errno);
            break;
        }
    }

    fclose(fout);
    errcode = 0;

err:
    //if (g_nftl_map) free(g_nftl_map);
    //if (page_buf) free(page_buf);
    //if (g_in_img) free(g_in_img);

    return errcode;
}

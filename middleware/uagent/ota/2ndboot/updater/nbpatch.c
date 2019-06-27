/*-
 * Copyright 2003-2005 Colin Percival
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "xzdec.h"
#include "nbpatch.h"
#include "libc.h"
#include "updater.h"

#define HEADER_SIZE 48

void * nbpatch_alloc(size_t size)
{
#if (!defined BOARD_ESP8266)
    return  malloc(size);
#else
    #define IRAM_HEAP_BASE (0x40108000)
    return  (void *)IRAM_HEAP_BASE;
#endif
}

void nbpatch_free(void *buf)
{
    if(buf != NULL) {
#if (!defined BOARD_ESP8266)
        free(buf);
#endif
    }
}
long offtin(unsigned char *buf) {
    long y;

    y = buf[7] & 0x7F;
    y = y * 256;
    y += buf[6];
    y = y * 256;
    y += buf[5];
    y = y * 256;
    y += buf[4];
    y = y * 256;
    y += buf[3];
    y = y * 256;
    y += buf[2];
    y = y * 256;
    y += buf[1];
    y = y * 256;
    y += buf[0];

    if (buf[7] & 0x80)
        y = -y;

    return y;
}

static long ota_nbpatch(const unsigned long old_t, long old_size, unsigned long patch, long *seek_pos, int num)
{
    int ret = 0;
    long newsize = 0;
    long bzctrllen = 0;
    long bzdatalen = 0;
    long bzextralen = 0;
    unsigned char header[HEADER_SIZE], buf[8];
    unsigned char *old = NULL;
    unsigned char *newbuf = NULL;
    long oldpos = 0;
    long newpos = 0;
    long ctrl[3] = {0};
    long lenread = 0;
    long i = 0;
    unsigned short crc = 0, cal_crc = 0;
    struct xz_dec *ctrl_dec = NULL;
    struct xz_buf cb;
    struct xz_dec *diff_dec = NULL;
    struct xz_buf db;
    struct xz_dec *extra_dec = NULL;
    struct xz_buf eb;
    int success = 0;

    OTA_LOG_I("nbpatch num:%d begin.", num);
    if(!old_t || !patch || !seek_pos ) {
        ret = OTA_NB_PARAMS_ERR;
        goto patch_error;
    }
    long seekpos = *seek_pos;
    old = (unsigned char *)(malloc(SECTOR_SIZE));
    if (old == NULL){
        ret = OTA_NB_MEM_OP_FAIL;
        goto patch_error;
    }
    /*
     File format:
     0	8	"BSDIFF40"
     8	8	X
     16	8	Y
     24	8	sizeof(newfile)
     32	X	bzip2(control block)
     32+X	Y	bzip2(diff block)
     32+X+Y	???	bzip2(extra block)
     with control block a set of triples (x,y,z) meaning "add x bytes
     from oldfile to x bytes from the diff block; copy y bytes from the
     extra block; seek forwards in oldfile by z bytes".
     */
    ret = ota_patch_read(patch, header, seekpos, HEADER_SIZE);
    if(ret < 0) {
        ret = OTA_NB_FILE_OP_FAIL;
        goto patch_error;
    }

    /* Check for appropriate magic */
    if (memcmp(header, "BSDIFF40", 8) != 0) {
        ret = OTA_NB_LZMA_HEAD_FAIL;
        goto patch_error;
    }
    /* Read lengths from header */
    bzctrllen = offtin(header + 8);
    bzdatalen = offtin(header + 16);
    bzextralen = offtin(header + 24);
    newsize = offtin(header + 32);
    crc = offtin(header + 40);
    if ((bzctrllen < 0) || (bzdatalen < 0) || (newsize < 0)) {
        ret = OTA_NB_PATCH_OP_FAIL;
        goto patch_error;
    }
    if ((newbuf = (unsigned char *)nbpatch_alloc(newsize + 1)) == 0) {
        ret = OTA_NB_MEM_OP_FAIL;
        goto patch_error;
    }

    xz_crc32_init();
    success = xz_init(&ctrl_dec, &cb);
    if (!success) {
        ret = OTA_NB_MEM_OP_FAIL;
        goto patch_error;
    }

    success = xz_init(&diff_dec, &db);
    if (!success) {
        ret = OTA_NB_MEM_OP_FAIL;
        goto patch_error;
    }

    success = xz_init(&extra_dec,&eb);
    if (!success) {
        ret = OTA_NB_MEM_OP_FAIL;
        goto patch_error;
    }

    xzReadHandler cbhandler;
    memset(&cbhandler, 0, sizeof(xzReadHandler));
    cbhandler.avail_size = bzctrllen;
    cbhandler.read_pos = seekpos + HEADER_SIZE;
    xzReadHandler diffhandler;
    memset(&diffhandler, 0, sizeof(xzReadHandler));
    diffhandler.avail_size = bzdatalen;
    diffhandler.read_pos = seekpos + HEADER_SIZE + bzctrllen;
    xzReadHandler extrahandler;
    memset(&extrahandler, 0, sizeof(xzReadHandler));
    extrahandler.avail_size = bzextralen;
    extrahandler.read_pos = seekpos + HEADER_SIZE + bzctrllen + bzdatalen;

    while (newpos < newsize) {
        /* Read control data */
        for (i = 0; i <= 2; i++) {
            lenread = xz_read(&cbhandler, &cb, ctrl_dec, patch, buf, 8);
            if (lenread < 8) {
                ret = OTA_NB_PATCH_OP_FAIL;
                goto patch_error;
            }
            ctrl[i] = offtin(buf);
            /* OTA_LOG_I("ctrlp[i] %ld", ctrl[i]); */
        };
        /* Sanity-check */
        if (newpos + ctrl[0] > newsize){
            ret = OTA_NB_PATCH_OP_FAIL;
            goto patch_error;
        }
        /* Read diff string */
        lenread = xz_read(&diffhandler, &db, diff_dec, patch, newbuf + newpos, ctrl[0]);
        if ((lenread < ctrl[0])){
            ret = OTA_NB_PATCH_OP_FAIL;
            goto patch_error;
        }
        if(oldpos > old_size || oldpos + ctrl[0] > old_size) {
            ret = OTA_NB_PATCH_OP_FAIL;
            goto patch_error;
        }
        long cp_size  = ctrl[0];
        long base_pos = 0;
        int idx = 0;
        long i;
        for (i = 0; i < ctrl[0]; i++) {
            if (!(i % SECTOR_SIZE)) {
                base_pos = (idx++) * SECTOR_SIZE;
                long osize = cp_size > SECTOR_SIZE ? SECTOR_SIZE : cp_size;
                memset(old, 0, osize);
                ret = ota_patch_read(old_t, old, oldpos + base_pos, osize);
                if(ret < 0) {
                     ret = OTA_NB_FILE_OP_FAIL;
                     goto patch_error;
                }
                cp_size -= osize;
                /* OTA_LOG_I("start cp:%ld bp:%ld",cp_size, base_pos); */
            }
            if (i >= base_pos) {
                /* OTA_LOG_I("oldpos:%d", old[i - base_pos]); */
                newbuf[newpos + i] += old[i - base_pos];
            }
        }
        /* Adjust pointers */
        newpos += ctrl[0];
        oldpos += ctrl[0];
        /* Sanity-check */
        if (newpos + ctrl[1] > newsize) {
            ret = OTA_NB_LZAM_OP_FAIL;
            goto patch_error;
        }
        /* Read extra string */
        lenread = xz_read(&extrahandler, &eb, extra_dec, patch, newbuf + newpos, ctrl[1]);
        if (lenread < ctrl[1]) {
            ret = OTA_NB_LZAM_OP_FAIL;
            goto patch_error;
        }
        /* Adjust pointers */
        newpos += ctrl[1];
        oldpos += ctrl[2];
    };
    cal_crc = crc16_computer(newbuf, newsize);
    if(cal_crc != crc) {
        OTA_LOG_I("cal %0x != crc %0x", cal_crc, crc);
        ret = OTA_NB_CRC_OP_FAIL;
        goto patch_error;
    }
    ret = ota_patch_new_data(newbuf, newsize);
    if(ret < 0) {
        ret = OTA_NB_FILE_OP_FAIL;
        goto patch_error;
    }
    seekpos += HEADER_SIZE;
    seekpos += bzctrllen;
    seekpos += bzdatalen;
    seekpos += bzextralen;
    *seek_pos = seekpos;

patch_error:
    OTA_LOG_I("nbpatch num:%d clen:%ld dlen:%ld elen:%ld crc:0x%02x ret:%d\n", num, bzctrllen, bzdatalen, bzextralen, cal_crc, ret);
    if(ctrl_dec){
        xz_end(ctrl_dec);
        ctrl_dec = NULL;
    }
    if(diff_dec) {
        xz_end(diff_dec);
        diff_dec = NULL;
    }
    if(extra_dec){
        xz_end(extra_dec);
        extra_dec = NULL;
    }
    if(old){
        free(old);
        old = NULL;
    }
    if(newbuf){
        free(newbuf);
        newbuf = NULL;
    }
    return (ret < 0) ? ret: newsize;
}

int ota_nbpatch_main(void) {
    int ret = -1;
    unsigned long old_t = 0;
    unsigned long patch_t = 0;
    int patch_num = 0;
    long patch_off = 0;
    long patch_size = 0;
    long old_size = 0;
    long new_off = 0;
    long new_size = 0;
    ota_boot_param_t ota_param = {0};

    OTA_LOG_I("nbpatch start\n");
    memset(&ota_param, 0, sizeof(ota_boot_param_t));
    ret = ota_patch_read_param(&ota_param);
    if(ret < 0){
        OTA_LOG_I("r param err.\n");
        return OTA_NB_PARAMS_ERR;
    }
    old_t = ota_param.dst_adr;
    patch_t = ota_param.src_adr;
    patch_off = ota_param.patch_off;
    patch_size = ota_param.len;
    old_size = ota_param.old_size;
    new_off = ota_param.new_off;
    new_size = ota_param.new_size;
    if (!old_t || !patch_t || old_size <= 0 || patch_size <= 0 || (ota_param.upg_flag != OTA_UPGRADE_DIFF)) {
        OTA_LOG_I("param check err.\n");
        return OTA_NB_PARAMS_ERR;
    }

    OTA_LOG_I("oaddr:0x%lx paddr:0x%lx num:%d noff:%lx nsize:%lx poff:%lx psize:%ld status:%d \n", old_t, patch_t, patch_num, new_off, new_size, patch_off, patch_size, ota_param.patch_status);
    while (patch_off < patch_size) {
        if(!ota_param.patch_status) {
            new_size = ota_nbpatch(old_t, old_size, patch_t, &patch_off, patch_num);
            ota_param.patch_num = ++patch_num;
            ota_param.patch_off = patch_off;
            ota_param.new_off = new_off;
            ota_param.new_size = new_size;
            ota_param.patch_status = 1;
            if (new_size < 0) {
                ret = new_size;
                goto EXIT;
            }
        }
        ret = ota_load_new_data(new_off, new_size);
        if(ret < 0) {
            ret = OTA_NB_FILE_OP_FAIL;
            OTA_LOG_I("err new off:%ld \n", new_size);
            goto EXIT;
        }
        new_off += new_size;
        ota_param.patch_status = 0;
        ota_param.new_off = new_off;
    }
EXIT:
    if(patch_off > patch_size) {
        ota_param.patch_num = 0;
        ota_param.patch_off = 0;
        ota_param.patch_status = 0;
        ota_param.new_off = 0;
        ota_param.new_size = 0;
    }
    ota_patch_write_param(&ota_param);
    OTA_LOG_I("nbpatch end ret: %d new off:%lx size:%lx patch_off:0x%lx \n",ret, new_off, new_size, patch_off);
    return (ret < 0) ? ret: new_off;
}

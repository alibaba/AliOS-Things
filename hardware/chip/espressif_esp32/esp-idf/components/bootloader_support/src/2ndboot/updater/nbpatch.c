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
#include "nbpatch.h"
#include "libc.h"
#include "updater.h"

#define HEADER_SIZE 48

long offtin(unsigned char *buf) {
    long y;

    y = buf[7] & 0x7F;
    y <<= 8; y += buf[6];
    y <<= 8; y += buf[5];
    y <<= 8; y += buf[4];
    y <<= 8; y += buf[3];
    y <<= 8; y += buf[2];
    y <<= 8; y += buf[1];
    y <<= 8; y += buf[0];
    if (buf[7] & 0x80)
        y = -y;
    return y;
}

long xz_read(xzReadHandler *handler, struct xz_buf *b,struct xz_dec *s, unsigned long patch, unsigned char *buf, int len) 
{
    long ret = 0;
    if (handler == NULL || b == NULL || s == NULL || patch == 0 || buf == NULL || len <= 0) {  
         return 0; 
    };

    b->out = buf;
    b->out_pos = 0;
    b->out_size = len;
    while(true) {
        if (b->in_pos == b->in_size && handler->avail_size > 0) {
            size_t readsize = IN_BUF_MAX >  handler->avail_size ? handler->avail_size:IN_BUF_MAX;
            b->in_size = ota_patch_read(patch, handler->in_buf, handler->read_pos, readsize);
            handler->avail_size -= readsize;
            handler->read_pos += readsize;
            b->in_pos = 0;
            b->in = handler->in_buf;
        }

        ret = xz_dec_run(s, b);
        if (ret != XZ_OK && ret != XZ_STREAM_END) {
            OTA_LOG_I("xz dec err:%ld \n", ret);
            return 0;
        }
        if (handler->avail_size <= 0 && b->in_pos == b->in_size
                && b->out_pos < len) {
            return b->out_pos;
        }

        if (b->out_pos == len) {
            return len;
        }
    }
    return ret;
}

static long ota_nbpatch(const unsigned long old_addr, long old_size, unsigned long patch, long *seek_pos, int num)
{
    int ret = 0;
    long new_size = 0;
    long ctrl_len = 0;
    long diff_len = 0;
    long extra_len = 0;
    unsigned char header[HEADER_SIZE], buf[8];
    unsigned char *old_buf = NULL;
    unsigned char *new_buf = NULL;
    long old_pos = 0;
    long new_pos = 0;
    long ctrl[3] = {0};
    long read_len = 0;
    long i = 0;
    CRC16_CTX crc_ctx;
    unsigned short patch_crc = 0;
    unsigned short crc = 0, cal_crc = 0;
    struct xz_dec *ctrl_dec = NULL, *diff_dec = NULL, *extra_dec = NULL;
    struct xz_buf cb = {0}, db = {0}, eb = {0};

    if(!old_addr || !patch || !seek_pos ) {
        ret = OTA_NB_INVALID_PARAM;
        goto EXIT;
    }
    long patch_pos = *seek_pos;
    old_buf = malloc(SECTOR_SIZE);
    if (old_buf == NULL){
        ret = OTA_NB_MEMORY_FAIL;
        goto EXIT;
    }
    /*
     File format:
     0	8	"BSDIFF40"
     8	8	X
     16	8	Y
     24	8	sizeof(newfile)
     32	X	bzip2(control block)
     32+X       bzip2(diff block)
     32+X+Y     bzip2(extra block)
     with control block a set of triples (x,y,z) meaning "add x bytes
     from oldfile to x bytes from the diff block; copy y bytes from the
     extra block; seek forwards in oldfile by z bytes".
     */
    ret = ota_patch_read(patch, header, patch_pos, HEADER_SIZE);
    if(ret < 0) {
        ret = OTA_NB_HEADER_FAIL;
        goto EXIT;
    }

    /* Check for appropriate magic */
    if (memcmp(header, "BSDIFF40", 8) != 0) {
        ret = OTA_NB_HEADER_FAIL;
        goto EXIT;
    }
    /* Read lengths from header */
    ctrl_len = offtin(header + 8);
    diff_len = offtin(header + 16);
    extra_len = offtin(header + 24);
    new_size = offtin(header + 32);
    crc = offtin(header + 40);
    if ((ctrl_len < 0) || (diff_len < 0) || (new_size < 0)) {
        ret = OTA_NB_HEADER_FAIL;
        goto EXIT;
    }

    OTA_LOG_I("nbpatch %d header clen:%ld dlen:%ld elen:%ld crc:0x%02x... \n", num, ctrl_len, diff_len, extra_len, crc);
    new_buf = malloc(new_size);
    if (new_buf == NULL) {
        ret = OTA_NB_MEMORY_FAIL;
        goto EXIT;
    }
    crc16_init(&crc_ctx);
    xz_crc32_init();
    ctrl_dec = xz_dec_init(XZ_DYNALLOC, DIFF_DICT_SIZE);
    if (ctrl_dec == NULL) {
        ret = OTA_NB_MEMORY_FAIL;
        goto EXIT;
    }
    diff_dec = xz_dec_init(XZ_DYNALLOC, DIFF_DICT_SIZE);
    if (diff_dec == NULL) {
        ret = OTA_NB_MEMORY_FAIL;
        goto EXIT;
    }
    extra_dec = xz_dec_init(XZ_DYNALLOC, DIFF_DICT_SIZE);
    if (extra_dec == NULL) {
        ret = OTA_NB_MEMORY_FAIL;
        goto EXIT;
    }

    xzReadHandler cbhandler;
    memset(&cbhandler, 0, sizeof(xzReadHandler));
    cbhandler.avail_size = ctrl_len;
    cbhandler.read_pos = patch_pos + HEADER_SIZE;
    xzReadHandler diffhandler;
    memset(&diffhandler, 0, sizeof(xzReadHandler));
    diffhandler.avail_size = diff_len;
    diffhandler.read_pos = patch_pos + HEADER_SIZE + ctrl_len;
    xzReadHandler extrahandler;
    memset(&extrahandler, 0, sizeof(xzReadHandler));
    extrahandler.avail_size = extra_len;
    extrahandler.read_pos = patch_pos + HEADER_SIZE + ctrl_len + diff_len;

    while (new_pos < new_size) {
        /* Read control data */
        for (i = 0; i <= 2; i++) {
            read_len = xz_read(&cbhandler, &cb, ctrl_dec, patch, buf, 8);
            if (read_len < 8) {
                ret = OTA_NB_READ_CTRL_FAIL;
                goto EXIT;
            }
            ctrl[i] = offtin(buf);
            /* OTA_LOG_I("ctrlp[i] %ld", ctrl[i]); */
        };

        /* Sanity-check */
        if (new_pos + ctrl[0] > new_size){
            ret = OTA_NB_READ_DIFF_FAIL;
            goto EXIT;
        }
        /* Read diff string */
        read_len = xz_read(&diffhandler, &db, diff_dec, patch, new_buf + new_pos, ctrl[0]);
        if ((read_len < ctrl[0])){
            ret = OTA_NB_READ_DIFF_FAIL;
            goto EXIT;
        }
        if(old_pos > old_size || old_pos + ctrl[0] > old_size) {
            ret = OTA_NB_READ_OLD_FAIL;
            goto EXIT;
        }
        long cp_size  = ctrl[0];
        long base_pos = 0;
        int idx = 0;
        long i;
        for (i = 0; i < ctrl[0]; i++) {
            if (!(i % SECTOR_SIZE)) {
                base_pos = (idx++) * SECTOR_SIZE;
                long osize = cp_size > SECTOR_SIZE ? SECTOR_SIZE : cp_size;
                memset(old_buf, 0, osize);
                ret = ota_patch_read(old_addr, old_buf, old_pos + base_pos, osize);
                if(ret < 0) {
                     ret = OTA_NB_READ_OLD_FAIL;
                     goto EXIT;
                }
                cp_size -= osize;
                /* OTA_LOG_I("start cp:%ld bp:%ld",cp_size, base_pos); */
            }
            if (i >= base_pos) {
                new_buf[new_pos + i] += old_buf[i - base_pos];
            }
        }
        crc16_update(&crc_ctx, new_buf + new_pos, ctrl[0]);
        /* Adjust pointers */
        new_pos += ctrl[0];
        old_pos += ctrl[0];
        /* Sanity-check */
        if (new_pos + ctrl[1] > new_size) {
            ret = OTA_NB_READ_EXTRA_FAIL;
            goto EXIT;
        }
        /* Read extra string */
        read_len = xz_read(&extrahandler, &eb, extra_dec, patch, new_buf + new_pos, ctrl[1]);
        if (read_len < ctrl[1]) {
            ret = OTA_NB_READ_EXTRA_FAIL;
            goto EXIT;
        }
        crc16_update(&crc_ctx, new_buf + new_pos, ctrl[1]);
        /* Adjust pointers */
        new_pos += ctrl[1];
        old_pos += ctrl[2];
    };
    crc16_final(&crc_ctx, &patch_crc);
    cal_crc = crc16_computer(new_buf, new_size);
    if(cal_crc != crc) {
        OTA_LOG_I("cal %02x != crc %02x", cal_crc, crc);
        ret = OTA_NB_CRC_COMP_FAIL;
        goto EXIT;
    }
    ret = ota_patch_new_data(new_buf, new_size);
    if(ret < 0) {
        ret = OTA_NB_WRITE_DATA_FAIL;
        goto EXIT;
    }
    patch_pos += HEADER_SIZE;
    patch_pos += ctrl_len;
    patch_pos += diff_len;
    patch_pos += extra_len;
    *seek_pos = patch_pos;

EXIT:
    OTA_LOG_I("nbpatch %d end cal_crc:0x%02x patch_crc:0x%02x ret:%d\n", num, cal_crc, patch_crc, ret);
    if(ctrl_dec){
        xz_dec_end(ctrl_dec);
        ctrl_dec = NULL;
    }
    if(diff_dec) {
        xz_dec_end(diff_dec);
        diff_dec = NULL;
    }
    if(extra_dec){
        xz_dec_end(extra_dec);
        extra_dec = NULL;
    }
    if(old_buf){
        free(old_buf);
        old_buf = NULL;
    }
    if(new_buf){
        free(new_buf);
        new_buf = NULL;
    }
    return (ret < 0) ? ret: new_size;
}

int ota_nbpatch_main(void) {
    int ret = -1;
    unsigned long old_addr = 0;
    unsigned long patch_addr = 0;
    int patch_num = 0;
    long patch_off = 0;
    long patch_size = 0;
    long old_size = 0;
    long new_off = 0;
    long new_size = 0;
    ota_boot_param_t ota_param = {0};

    memset(&ota_param, 0, sizeof(ota_boot_param_t));
    ret = ota_patch_read_param(&ota_param);
    if(ret < 0){
        OTA_LOG_I("r param err.\n");
        return OTA_NB_INVALID_PARAM;
    }
    old_addr = ota_param.dst_adr;
    patch_addr = ota_param.src_adr;
    patch_off = ota_param.patch_off;
    patch_size = ota_param.len;
    old_size = ota_param.old_size;
    new_off = ota_param.new_off;
    new_size = ota_param.new_size;
    if (!old_addr || !patch_addr || old_size <= 0 || patch_size <= 0 || (ota_param.upg_flag != OTA_UPGRADE_DIFF)) {
        OTA_LOG_I("param check err.\n");
        return OTA_NB_INVALID_PARAM;
    }

    OTA_LOG_I("oaddr:0x%lx paddr:0x%lx num:%d noff:%lx nsize:%lx poff:%lx psize:%ld status:%d \n", old_addr, patch_addr, patch_num, new_off, new_size, patch_off, patch_size, ota_param.patch_status);
    while (patch_off < patch_size) {
        if(!ota_param.patch_status) {
            new_size = ota_nbpatch(old_addr, old_size, patch_addr, &patch_off, patch_num);
            ota_param.patch_num = ++patch_num;
            ota_param.patch_off = patch_off;
            ota_param.new_off = new_off;
            ota_param.new_size = new_size;
            ota_param.patch_status = 1;
            if (new_size < 0) {
                ret = new_size;
                goto EXIT;
            }
            ota_patch_write_param(&ota_param);
        }
        ret = ota_load_new_data(new_off, new_size);
        if(ret < 0) {
            ret = OTA_NB_WRITE_DATA_FAIL;
            OTA_LOG_I("err new off:%ld \n", new_size);
            goto EXIT;
        }
        new_off += new_size;
        ota_param.patch_status = 0;
        ota_param.new_off = new_off;
        ota_patch_write_param(&ota_param);
    }
EXIT:
    if(patch_off > patch_size) {
        ota_param.patch_num = 0;
        ota_param.patch_off = 0;
        ota_param.patch_status = 0;
        ota_param.new_off = 0;
        ota_param.new_size = 0;
        ota_param.upg_flag = 0;
    }
    ota_patch_write_param(&ota_param);
    OTA_LOG_I("nbpatch complete ret: %d new off:%lx size:0x%lx patch_off:0x%lx \n",ret, new_off, new_size, patch_off);
    return (ret < 0) ? ret: new_off;
}

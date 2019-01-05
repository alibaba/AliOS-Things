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

int err_code;
static void *nbpatch_buffer;

void nbpatch_buffer_init()
{
    nbpatch_buffer = NULL;
}

void * nbpatch_buffer_alloc(size_t size)
{
    if(nbpatch_buffer != NULL) {
#if (!defined BOARD_ESP8266)
        free(nbpatch_buffer);
#endif
    }
#if (!defined BOARD_ESP8266)
    nbpatch_buffer = malloc(size);
#else
    nbpatch_buffer = (void *)IRAM_HEAP_BASE;
#endif
    return nbpatch_buffer;
}

void * nbpatch_buffer_get()
{
    return nbpatch_buffer;
}

void nbpatch_buffer_free()
{
    if(nbpatch_buffer != NULL) {
#if (!defined BOARD_ESP8266)
        free(nbpatch_buffer);
#endif
    }
    nbpatch_buffer = NULL;
}

off_t offtin(u_char *buf) {
    off_t y;

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

static off_t nbpatch_section(const unsigned long src, off_t old_end, unsigned long dst, off_t *seek_pos, off_t splict_size, int num) ;

off_t nbpatch(unsigned long old_t, off_t old_size, const unsigned long new_t, off_t new_size, off_t splict_size) {
    int ret = -1;
    PatchStatus * pstatus = NULL;

    if (!old_t || !new_t || old_size <= 0 || new_size <= 0 || splict_size <= 0) {
        err_code = NBDIFF_PARAMS_INPUT_ERROR;
        return err_code;
    }

    off_t seekpos = 0;
    off_t patchsize = 0;
    off_t pendingsize = 0;
    int num = 0;
    pstatus = nbpatch_get_pstatus();
    if(pstatus == NULL) {
        return -1;
    }

    memset(pstatus, 0, sizeof(PatchStatus));
    read_patch_status(pstatus);

    num = pstatus->num;
    seekpos = pstatus->seekpos;
    patchsize = pstatus->patched_size;
    pendingsize = pstatus->pending_size;

    LOG("pendsize %ld, num %d, pos %d, psize %d pstatus %d", pendingsize, num, seekpos, patchsize, pstatus->status);
    while (seekpos < new_size || pstatus->status != 0) {
        if(!pstatus->status) {
            pendingsize = nbpatch_section(old_t, old_size, new_t, &seekpos, splict_size, num);
            if (err_code) {
                LOG("sec err:%d\n", err_code);
                nbpatch_buffer_free();
                goto nbpatch_error;
            }
            #if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_DIRECT)
            pstatus->num = ++num;
            pstatus->seekpos = seekpos;
            pstatus->patched_size = patchsize;
            pstatus->pending_size = pendingsize;
            pstatus->status = 1;
            save_patch_status(pstatus);
            #endif
        }

        off_t offset = 0;
        off_t copy_size = load_bakeup_data(patchsize, pendingsize, offset);
        if((copy_size == -1)||(copy_size != (pendingsize - offset))) {
            LOG("err %ld, %ld, err %d", copy_size, pendingsize, err_code = NBDIFF_FILE_OP_FAIL);
            nbpatch_buffer_free();
            goto nbpatch_error;
        }

        patchsize += pendingsize;
        pstatus->status       = 0;
#if (AOS_OTA_RECOVERY_TYPE != OTA_RECOVERY_TYPE_DIRECT)
        pstatus->pending_size = pendingsize;
        pstatus->seekpos = seekpos;
        nbpatch_ota_addr_free(seekpos);  //already unpacked, can free now
        pstatus->num = ++num;
#endif
        pstatus->patched_size = patchsize;
        nbpatch_buffer_free();
        save_patch_status(pstatus);
    }

    ret = patchsize;
nbpatch_error:
    LOG("nbpatch suc:%d\n",ret);
    pstatus->num = 0;
    pstatus->patched_size = 0;
    pstatus->seekpos = 0;
    pstatus->status = 0;
    pstatus->crc = 0;
    pstatus->pending_size = 0;
    save_patch_status(pstatus);
    return ret;
}

static off_t nbpatch_section(const unsigned long src, off_t old_size, unsigned long dst, off_t *seek_pos, off_t splict_size, int num)
{
    off_t newsize = 0;
    off_t bzctrllen, bzdatalen ,bzextralen;
    u_char header[HEADER_SIZE], buf[8];
    u_char *old = NULL;
    u_char *newbuf;
    off_t oldpos=0;
    off_t newpos=0;
    off_t ctrl[3];
    off_t lenread;
    off_t i;
    if(!src || !dst || !seek_pos ) {
        err_code = NBDIFF_PARAMS_INPUT_ERROR;
        goto patch_error;
    }
    off_t seekpos = *seek_pos;
    
    rec_wdt_feed();

    old = (u_char *)(malloc(SECTOR_SIZE));
    if (old == NULL){
        err_code = NBDIFF_MEM_OP_FAIL;
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
    int ret = nbpatch_read(dst, header, seekpos, HEADER_SIZE, 0);
    if(ret < 0) {
        err_code = NBDIFF_FILE_OP_FAIL;
        goto patch_error;
    }

    /* Check for appropriate magic */
    if (memcmp(header, "BSDIFF40", 8) != 0) {
        err_code = NBDIFF_LZAM_OP_FAIL;
        goto patch_error;
    }

    /* Read lengths from header */
    bzctrllen = offtin(header + 8);
    bzdatalen = offtin(header + 16);
    bzextralen = offtin(header + 24);
    newsize = offtin(header + 32);
    uint16_t crc = offtin(header + 40);
    if ((bzctrllen < 0) || (bzdatalen < 0) || (newsize < 0)) {
        err_code = NBDIFF_LZAM_OP_FAIL;
        goto patch_error;
    }

    if ((newbuf = (u_char *)nbpatch_buffer_alloc(newsize + 1)) == 0) {
        err_code = NBDIFF_MEM_OP_FAIL;
        goto patch_error;
    }

    struct xz_dec *ctrl_dec = NULL;
    struct xz_buf cb;
    struct xz_dec *diff_dec = NULL;
    struct xz_buf db;
    struct xz_dec *extra_dec = NULL;
    struct xz_buf eb;
    int success = 0;
    xz_crc32_init();
    success = xz_init(&ctrl_dec, &cb);
    if (!success) {
        err_code = NBDIFF_LZAM_OP_FAIL;
        goto patch_error;
    }

    success = xz_init(&diff_dec, &db);
    if (!success) {
        err_code = NBDIFF_LZAM_OP_FAIL;
        goto patch_error;
    }

    success = xz_init(&extra_dec,&eb);
    if (!success) {
        err_code = NBDIFF_LZAM_OP_FAIL;
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
            lenread = xz_read(&cbhandler, &cb, ctrl_dec, dst, buf, 8);
            if (lenread < 8)
            {
                err_code = NBDIFF_LZAM_OP_FAIL;
                goto patch_error;
            }
            ctrl[i] = offtin(buf);
            //LOG("ctrlp[i] %ld", ctrl[i]);
        };

        /* Sanity-check */
        if (newpos + ctrl[0] > newsize){
            err_code = NBDIFF_LZAM_OP_FAIL;
            goto patch_error;
        }

        /* Read diff string */
        lenread = xz_read(&diffhandler, &db, diff_dec, dst, newbuf + newpos, ctrl[0]);
        if ((lenread < ctrl[0])){
            err_code = NBDIFF_LZAM_OP_FAIL;
            goto patch_error;
        }

        if(oldpos > old_size || oldpos + ctrl[0] > old_size) {
            LOG("pos over: %ld, %ld, %ld",oldpos,ctrl[0] ,old_size);
            err_code = NBDIFF_LZAM_OP_FAIL;
            goto patch_error;
        }

        off_t cp_size = ctrl[0];
        off_t base_pos = 0;
        int num = 0;
        off_t i;

        for (i = 0; i < ctrl[0]; i++) {
            if (!(i % SECTOR_SIZE)) {
                base_pos = (num++) * SECTOR_SIZE;
                off_t siz = cp_size > SECTOR_SIZE ? SECTOR_SIZE : cp_size;
                memset(old, 0, siz);
                nbpatch_read(src, old, oldpos + base_pos, siz, 1);
                cp_size -= siz;
                //LOG("start cp %ld, %ld",cp_size, base_pos);
            }
            if (i >= base_pos) {
                //LOG("oldpos %d", old[i - base_pos]);
                newbuf[newpos + i] += old[i - base_pos];
            }
        }
        rec_wdt_feed();
        //LOG("oldpos %ld, newpos %ld",oldpos, newpos);
        /* Adjust pointers */
        newpos += ctrl[0];
        oldpos += ctrl[0];

        /* Sanity-check */
        if (newpos + ctrl[1] > newsize) {
            err_code = NBDIFF_LZAM_OP_FAIL;
            goto patch_error;
        }

        /* Read extra string */
        lenread = xz_read(&extrahandler, &eb, extra_dec, dst, newbuf + newpos, ctrl[1]);
        if (lenread < ctrl[1]) {
            err_code = NBDIFF_LZAM_OP_FAIL;
            goto patch_error;
        }

        /* Adjust pointers */
        newpos += ctrl[1];
        oldpos += ctrl[2];
        rec_wdt_feed();
    };

    if(newsize > splict_size) {
        err_code = NBDIFF_PATCH_OP_FAIL;
        goto patch_error;
    }
    uint16_t cal_crc = 0;
    CRC16_Context crc_context;
    CRC16_Init(&crc_context);
    CRC16_Update(&crc_context, newbuf, newsize);
    CRC16_Final(&crc_context, &cal_crc);
    if(cal_crc != crc) {
        LOG("cal %0x != crc %0x", cal_crc, crc);
        err_code = NBDIFF_CRC_OP_FAIL;
        goto patch_error;
    }

#if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_DIRECT)
        ret = save_bakeup_data((unsigned long )newbuf, newsize);
        if(ret < 0) {
            err_code = NBDIFF_FILE_OP_FAIL;
            goto patch_error;
        }
#endif

    seekpos += HEADER_SIZE;
    seekpos += bzctrllen;
    seekpos += bzdatalen;
    seekpos += bzextralen;
    *seek_pos = seekpos;
    err_code = 0;
patch_error:
    LOG("nb section:%d num:%d", err_code,num);
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
    return newsize;
}

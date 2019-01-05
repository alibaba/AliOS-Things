/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "nbpatch.h"
#include "rec_verify_fw.h"

static PatchStatus *g_pstatus = NULL;
PatchStatus *nbpatch_get_pstatus(void)
{
    if(g_pstatus == NULL)
    {
        g_pstatus = (PatchStatus *)malloc(sizeof(PatchStatus)); // this memory will not free
        if(NULL != g_pstatus) {
            memset(g_pstatus, 0, sizeof(PatchStatus));
        }
    }
    return g_pstatus;
}
#if (AOS_OTA_RECOVERY_TYPE != OTA_RECOVERY_TYPE_DIRECT)
void nbpatch_flash_status_init(uint32_t ota_len)
{
    uint32_t i   = 0;
    uint32_t num = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();
    uint32_t     par_len = patch_flash_get_partion_length(HAL_PARTITION_OTA_TEMP);

    memset(pstatus->REC_FLASH_STAT_E, OTA_FLASH_STATUS_NULL, SPLISE_NUM);
    memset(pstatus->ota_addr, 0xFF, OTA_ADDR_NUM * sizeof(uint16_t));

    num = ota_len / SPLICT_SIZE + (((ota_len % SPLICT_SIZE) == 0)?0:1);
    if(num > OTA_ADDR_NUM){
        num = OTA_ADDR_NUM;
    }
    for(i = 0; i < num; i++) {
        pstatus->REC_FLASH_STAT_E[i] = OTA_FLASH_STATUS_USED;
        pstatus->ota_addr[i] = i; // addr is the number of SPLICT_SIZE
    }

    num = par_len / SPLICT_SIZE ;
    for(; i < num; i++) {
        pstatus->REC_FLASH_STAT_E[i] = OTA_FLASH_STATUS_FREE;
    }

    pstatus->recovery_phase = REC_PHASE_NBPATCH;

    save_patch_status(pstatus);
}

int nbpatch_flash_status_check(uint32_t addr)
{
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if(addr / SPLICT_SIZE >= SPLISE_NUM) {
        return -1;
    }

    return pstatus->REC_FLASH_STAT_E[addr / SPLICT_SIZE];
}

void nbpatch_ota_status_set(off_t offset, uint32_t status)
{
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if(offset / SPLICT_SIZE >= SPLISE_NUM) {
        return;
    }

    pstatus->REC_FLASH_STAT_E[offset / SPLICT_SIZE] = status;
    save_patch_status(pstatus);
}

void nbpatch_ota_addr_set(off_t old_offset, off_t new_offset)
{
    uint32_t i = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if ((old_offset / SPLICT_SIZE >= SPLISE_NUM) || (new_offset / SPLICT_SIZE >= SPLISE_NUM)){
        return;
    }

    for(i = 0; i < OTA_ADDR_NUM; i++)
    {
        if(((pstatus->ota_addr[i] * SPLICT_SIZE) == old_offset) && (pstatus->ota_addr[i] != 0xFFFF))
        {
            pstatus->ota_addr[i] = new_offset / SPLICT_SIZE;
        }
    }
    save_patch_status(pstatus);
}

off_t nbpatch_ota_addr_get(off_t old_offset)
{
    off_t new_offset;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if (old_offset / SPLICT_SIZE >= OTA_ADDR_NUM) {
        return old_offset;
    }

    new_offset = pstatus->ota_addr[old_offset / SPLICT_SIZE] * SPLICT_SIZE + old_offset % SPLICT_SIZE;

    return new_offset;
}

// find free space from end to front
off_t nbpatch_find_free_space(void)
{
    int i   = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    for(i = SPLISE_NUM - 1; i >= 0; i--) {
        if(pstatus->REC_FLASH_STAT_E[i] == 0) {
            return i * SPLICT_SIZE;
        }
    }
    return 0xFFFFFFFF;
}

void nbpatch_ota_addr_free(off_t range)
{
    int i = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if ( range / SPLICT_SIZE > OTA_ADDR_NUM) {
        return;
    }

    for(i = 0; i < (range / SPLICT_SIZE); i++)
    {
        if(pstatus->ota_addr[i] != 0)
        {
            pstatus->REC_FLASH_STAT_E[pstatus->ota_addr[i]] = OTA_FLASH_STATUS_FREE;
            pstatus->ota_addr[i] = 0;
        }
    }
    save_patch_status(pstatus);
}

void nbpatch_copy_app2ota() {
    int i   = 0;
    int num = 0;
    int ret = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();
    unsigned int par_len = patch_flash_get_partion_length(HAL_PARTITION_OTA_TEMP);

    pstatus->recovery_phase = REC_PHASE_COPY;
    save_patch_status(pstatus);

    num = par_len  / SPLICT_SIZE;
    for(i = 0; i < num; i++) {
        if((pstatus->REC_FLASH_STAT_E[i] != OTA_FLASH_STATUS_REVY) && (pstatus->REC_FLASH_STAT_E[i] != OTA_FLASH_STATUS_SYNC)) {
            LOG("copy to addr 0x%x, status %d\n", i * SPLICT_SIZE, pstatus->REC_FLASH_STAT_E[i]);
            ret = patch_flash_copy_par(HAL_PARTITION_OTA_TEMP, HAL_PARTITION_APPLICATION, i * SPLICT_SIZE, SPLICT_SIZE);
            LOG("copy to addr 0x%x, st:%d ret:%d\n", i * SPLICT_SIZE, pstatus->REC_FLASH_STAT_E[i], ret);
            pstatus->REC_FLASH_STAT_E[i] = OTA_FLASH_STATUS_SYNC;
            save_patch_status(pstatus);
        }
    }
    save_patch_status(pstatus);
}

#endif

#if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_ABBACK)
int nbpatch_swap_section(int par1, int par2, int offset, size_t swap_size, unsigned int sec)
{
    int i;
    int num;
    void *buf = NULL;
    void *buf2 = NULL;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    LOG("swap offset 0x%x\n", offset);

    buf = malloc(SECTOR_SIZE);
    if(NULL == buf) {
        return -1;
    }

    buf2 = malloc(SECTOR_SIZE);
    if(NULL == buf2) {
        free(buf);
        return -1;
    }

    rec_wdt_feed();

    num = swap_size  / SECTOR_SIZE;

    for(i = sec; i < num; i ++ ) {
        if((sec != 0) && (sec == i) && (0 != pstatus->swaped_state)) {
            if (1 == pstatus->swaped_state) {
                patch_flash_read(PARTITION_BACKUP_PARAM, (const unsigned char *)buf, offset + i*SECTOR_SIZE, SECTOR_SIZE);
                patch_flash_read(par2, (const unsigned char *)buf2, offset + i*SECTOR_SIZE, SECTOR_SIZE);
                goto state1_label;
            } else if(2 == pstatus->swaped_state) {
                patch_flash_read(PARTITION_BACKUP_PARAM, (const unsigned char *)buf, offset + i*SECTOR_SIZE, SECTOR_SIZE);
                goto state2_label;
            } else if(3 == pstatus->swaped_state) {
                continue;
            }
        } else {
            patch_flash_read(par1, (const unsigned char *)buf, offset + i*SECTOR_SIZE, SECTOR_SIZE);
            patch_flash_read(par2, (const unsigned char *)buf2, offset + i*SECTOR_SIZE, SECTOR_SIZE);
        }
        pstatus->swaped_idx   = i;
        pstatus->swaped_state = 0;
        save_patch_status(pstatus);

        if(0 == memcmp(buf, buf2, SECTOR_SIZE)) {
            continue;
        }

        patch_flash_erase(PARTITION_BACKUP_PARAM, SECTOR_SIZE, SECTOR_SIZE);
        patch_flash_write(PARTITION_BACKUP_PARAM, (const unsigned char *)buf, SECTOR_SIZE, SECTOR_SIZE);

        pstatus->swaped_state = 1;
        save_patch_status(pstatus);

        state1_label:
        patch_flash_erase(par1, offset + i*SECTOR_SIZE, SECTOR_SIZE);
        patch_flash_write(par1, (const unsigned char *)buf2, offset + i*SECTOR_SIZE, SECTOR_SIZE);

        pstatus->swaped_state = 2;
        save_patch_status(pstatus);

        state2_label:
        patch_flash_erase(par2, offset + i*SECTOR_SIZE, SECTOR_SIZE);
        patch_flash_write(par2, (const unsigned char *)buf, offset + i*SECTOR_SIZE, SECTOR_SIZE);

        pstatus->swaped_state = 3;
        save_patch_status(pstatus);
    }
    free(buf);
    free(buf2);
    return 0;
}

//swap the difference between the app and ota partion
int nbpatch_swap_app2ota(unsigned char all_flag)
{
    unsigned int i   = 0;
    unsigned int sec = 0;
    char version[OTA_MAX_VER_LEN];
	unsigned int par_len = patch_flash_get_partion_length(HAL_PARTITION_APPLICATION);
    PatchStatus* pstatus = nbpatch_get_pstatus();

    read_patch_status(pstatus);
    rec_wdt_feed();

    if(REC_PHASE_SWAP == pstatus->recovery_phase) {
	    i = pstatus->swap_addr;
    }
    if(0 != i) {
        sec = pstatus->swaped_idx;
    }
    LOG("swap offset 0x%x, index %d\r\n", i*SPLICT_SIZE, sec);
    pstatus->recovery_phase = REC_PHASE_SWAP;
    save_patch_status(pstatus);

    for(; i < par_len / SPLICT_SIZE; i++) {
        if((TRUE == all_flag) || (OTA_FLASH_STATUS_REVY == pstatus->REC_FLASH_STAT_E[i])) {
            pstatus->swap_addr  = i;
            nbpatch_swap_section(HAL_PARTITION_APPLICATION, HAL_PARTITION_OTA_TEMP, i*SPLICT_SIZE, SPLICT_SIZE, sec);
            sec = 0;
        }
    }

    pstatus->recovery_phase = REC_PHASE_DONE;
    //switch the version string
    memset(version, 0, OTA_MAX_VER_LEN);
    memcpy(version, pstatus->app_version, OTA_MAX_VER_LEN);
    memcpy(pstatus->app_version, pstatus->ota_version, OTA_MAX_VER_LEN);
    memcpy(pstatus->ota_version, version, OTA_MAX_VER_LEN);
    save_patch_status(pstatus);
    return 0;
}
#endif

int nbpatch_main(void)
{
    int ret = 0;
    int nbpatch_size = 0;
    PatchStatus * pstatus = NULL;
    pstatus = nbpatch_get_pstatus();
    if(pstatus == NULL) {
        ret = NBPATCH_ST_FAIL;
        goto END;
    }

    read_patch_status(pstatus);
    if (pstatus->dst_adr > HAL_PARTITION_OTA_TEMP || pstatus->src_adr > HAL_PARTITION_OTA_TEMP) {
        ret = NBPATCH_PARAM_FAIL;
        goto END;
    }

    if (!pstatus->diff) {
        ret = NBPATCH_DIFF_FAIL;
        goto END;
    }

    uint32_t old_size = patch_flash_get_partion_length(pstatus->dst_adr);
    if(pstatus->len > old_size || pstatus->len == 0) {
        ret = NBPATCH_DIFF_FAIL;
        goto END;
    }

#if (AOS_OTA_RECOVERY_TYPE != OTA_RECOVERY_TYPE_DIRECT)
    if(pstatus->patch_index == 0) {
        nbpatch_flash_status_init(pstatus->len);
    }
    pstatus->patch_index += 1;
    save_patch_status(pstatus);

    if(pstatus->patch_index > 1) {
        switch (pstatus->recovery_phase) {
            case REC_PHASE_COPY:
                goto nbpatch_copy_lable;
                break;
            #if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_ABBACK)
            case REC_PHASE_SWAP:
                goto nbpatch_swap_lable;
                break;
            #endif
            default:
                break;
        };
    }
#endif

    if(pstatus->status > 1) {//first recovery, init patch status
        pstatus->seekpos = 0;
        pstatus->patched_size = 0;
        pstatus->pending_size = 0;
        pstatus->status = 0;
        pstatus->num = 0;
        save_patch_status(pstatus);
    }

    nbpatch_buffer_init();
    nbpatch_size = nbpatch(pstatus->dst_adr, old_size, pstatus->src_adr,  pstatus->len, SPLICT_SIZE);
    if(nbpatch_size <= 0) {
        ret = NBPATCH_DIFF_FAIL;
        goto END;
    }
#if (AOS_OTA_RECOVERY_TYPE != OTA_RECOVERY_TYPE_DIRECT)
    else{
        nbpatch_copy_lable:
        nbpatch_copy_app2ota();
        ret = rec_verify_firmware(pstatus->src_adr, nbpatch_size);
        LOG("verify fw adr:0x%x len:0x%x ret:0x%x\r\n", pstatus->src_adr, nbpatch_size,ret);

        #if (AOS_OTA_RECOVERY_TYPE == OTA_RECOVERY_TYPE_ABBACK)
        nbpatch_swap_lable:
        ret = nbpatch_swap_app2ota(FALSE);
        #endif
    }
#endif
#if (AOS_OTA_RECOVERY_TYPE != OTA_RECOVERY_TYPE_ABBOOT)
    pstatus->recovery_phase = REC_PHASE_DONE;
#else
    pstatus->recovery_phase = REC_PHASE_SWITCH;
#endif
    save_patch_status(pstatus);
END:
    LOG("nbpatch end:%d",ret);
    return ret;
}

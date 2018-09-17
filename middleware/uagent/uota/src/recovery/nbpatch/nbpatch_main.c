/**
* @author E-mail: ting.guit@alibaba-inc.com
* @version time：2018年4月24日 下午6:23:57
* nbpatch_main.c
*/
#include "nbpatch.h"
#include "rec_verify_fw.h"

static PatchStatus *g_pstatus = NULL;
PatchStatus *nbpatch_get_pstatus(void)
{
    if(g_pstatus == NULL)
    {
        g_pstatus = (PatchStatus *)malloc(sizeof(PatchStatus)); // 使用动态内存，在recovery不运行时节省内存。该内存不需要释放。
    }
    return g_pstatus;
}
#if (defined RECOVERY_FLASH_COPY)
void nbpatch_flash_status_init(uint32_t ota_len) {
    uint32_t i   = 0;
    uint32_t num = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();
    uint32_t par_len = get_partion_length(HAL_PARTITION_OTA_TEMP);

    //将空间状态初始化为3
    memset(pstatus->ota_flash_status, 3, SPLISE_NUM);
	memset(pstatus->ota_addr, 0xFF, OTA_ADDR_NUM * sizeof(uint16_t));

    num = ota_len / SPLICT_SIZE + (((ota_len % SPLICT_SIZE) == 0)?0:1);
    if(num > OTA_ADDR_NUM){
        num = OTA_ADDR_NUM;   // 超过4个，说明ota差分包大小超过了256k。实际不可能发生
    }
    for(i = 0; i < num; i++) {
        pstatus->ota_flash_status[i] = 1;
        pstatus->ota_addr[i] = i;  // 此处地址为相对于分区头的偏移，为64k的个数
    }

    num = (par_len - SECTOR_SIZE) / SPLICT_SIZE ;
    for(; i < num; i++) {
        pstatus->ota_flash_status[i] = 0;
    }

    save_patch_status(pstatus);
}

int nbpatch_flash_status_check(uint32_t addr) {
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if(addr / SPLICT_SIZE >= SPLISE_NUM) {
        return -1;
    }

    return pstatus->ota_flash_status[addr / SPLICT_SIZE];
}

void nbpatch_ota_status_set(off_t offset, uint32_t status) {
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if(offset / SPLICT_SIZE >= SPLISE_NUM) {
        return;
    }

    pstatus->ota_flash_status[offset / SPLICT_SIZE] = status;
    save_patch_status(pstatus);
}

void nbpatch_ota_addr_set(off_t old_offset, off_t new_offset) {
	uint32_t i = 0;
	uint32_t num;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if ((old_offset / SPLICT_SIZE >= SPLISE_NUM) || (new_offset / SPLICT_SIZE >= SPLISE_NUM)){
        return;
    }

    for(i = 0; i < OTA_ADDR_NUM; i++)
    {
        if(((pstatus->ota_addr[i] * SPLICT_SIZE) == old_offset) && (pstatus->ota_addr[i] != 0xFFFFFFFF))
        {
            pstatus->ota_addr[i] = new_offset / SPLICT_SIZE;
        }
    }
    save_patch_status(pstatus);
}

off_t nbpatch_ota_addr_get(off_t old_offset) {
	off_t new_offset;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if (old_offset / SPLICT_SIZE >= OTA_ADDR_NUM) {    // TODO: 此处4需要改为宏定义
        return old_offset;
    }

	new_offset = pstatus->ota_addr[old_offset / SPLICT_SIZE] * SPLICT_SIZE + old_offset % SPLICT_SIZE;

    return new_offset;
}

// 从后往前找
off_t nbpatch_find_free_space(void){
    int i   = 0;
    uint32_t num = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    for(i = SPLISE_NUM - 1; i >= 0; i--) {
        if(pstatus->ota_flash_status[i] == 0) {
            return i * SPLICT_SIZE;
        }
    }
    return 0xFFFFFFFF;
}

void nbpatch_ota_addr_free(off_t range) {
    int i = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();

    if ( range / SPLICT_SIZE > OTA_ADDR_NUM) {
        return;
    }

    for(i = 0; i < (range / SPLICT_SIZE); i++)
    {
        if(pstatus->ota_addr[i] != 0)
        {
            pstatus->ota_flash_status[pstatus->ota_addr[i]] = 0;
            pstatus->ota_addr[i] = 0;
        }
    }
    save_patch_status(pstatus);
}

void nbpatch_copy_app2ota() {
    int i   = 0;
    int num = 0;
    PatchStatus* pstatus = nbpatch_get_pstatus();
    uint32_t par_len = get_partion_length(HAL_PARTITION_OTA_TEMP);

    num = (par_len - SECTOR_SIZE)  / SPLICT_SIZE ;
    for(i = 0; i < num; i++) {
	if(pstatus->ota_flash_status[i] != 2) {
	    patch_flash_copy_par(HAL_PARTITION_OTA_TEMP, HAL_PARTITION_APPLICATION, i * SPLICT_SIZE, SPLICT_SIZE);
	    pstatus->ota_flash_status[i] = 2;
        }
    }
    save_patch_status(pstatus);
}
#endif
int nbpatch_main(void)
{
	int ret = 0;
	uint32_t addr = 0;
	uint32_t ota_len;

	LOG("nbpatch enter\r\n");
	PatchStatus * pstatus = NULL;
	pstatus = nbpatch_get_pstatus();
	if(pstatus == NULL) {
		LOG("error:pstatus is NULL\n");
		return -1;
	}

	memset(pstatus, 0, sizeof(PatchStatus));
	read_patch_status(pstatus);
	if (pstatus->dst_adr > HAL_PARTITION_OTA_TEMP || pstatus->src_adr > HAL_PARTITION_OTA_TEMP) {
		LOG("pstatus adr overflow\r\n");
		return -1;
    }

    if (!pstatus->diff_version) {
        LOG("pstatus diff_version error\r\n");
        return -1;
    }

    uint32_t old_size = get_partion_length(pstatus->dst_adr);
    if(pstatus->patch_size > old_size || pstatus->patch_size == 0) {
        LOG("p overflow \r\n");
        return -1;
    }
#if (defined RECOVERY_FLASH_COPY)
    if(pstatus->patch_index == 0) {
        //初始化空闲flash位置
        nbpatch_flash_status_init(pstatus->patch_size);
    }
    pstatus->patch_index += 1;
    save_patch_status(pstatus);

    if(pstatus->status > 1) {//first recovery, init patch status
        pstatus->seekpos = 0;
        pstatus->patched_size = 0;
        pstatus->pending_size = 0;
        pstatus->status = 0;
        pstatus->num = 0;
        save_patch_status(pstatus);
    }
#endif
    nbpatch_buffer_init();
    int nbpatch_size = nbpatch(pstatus->dst_adr, old_size, pstatus->src_adr,  pstatus->patch_size, SPLICT_SIZE);
    if(nbpatch_size <= 0) {
        LOG("nbpatch error \r\n");
        ret = -1;
    }
#if (defined RECOVERY_FLASH_COPY)
    else{
        if(rec_get_ota_mode() == 2)
        {
            nbpatch_copy_app2ota();
            LOG("copy app text to ota end\n");
        }
        ret = rec_verify_firmware(pstatus->src_adr, nbpatch_size);
        LOG("nbpatch success adr:0x%x len:0x%x verify:0x%x\n", pstatus->src_adr, nbpatch_size,ret);
    }
#endif
    return ret;
}

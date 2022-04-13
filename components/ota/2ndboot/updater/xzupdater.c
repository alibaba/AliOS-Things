/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "updater.h"
#include "xz.h"

int ota_image_check(unsigned int addr, unsigned int size, unsigned int crc)
{
    int ret = 0;
    unsigned char data_buf[SECTOR_SIZE] = {0};
    unsigned int read_size = 0;
    uint16_t cal_crc = 0;
    CRC16_CTX contex;
    crc16_init(&contex );

    for( ; size > 0; size -= read_size, addr += read_size ) {
        read_size = size >= SECTOR_SIZE ? SECTOR_SIZE : size;
        memset(data_buf, 0, SECTOR_SIZE);
	ret = flash_read_data(addr, data_buf, read_size);
	if (ret != 0){
	    OTA_LOG_I("r d ret:%d.\n",ret);
	}
        crc16_update(&contex, data_buf, read_size);
    }
    crc16_final(&contex, &cal_crc);
    OTA_LOG_I("cal crc:0x%04x, crc:0x%x \n", cal_crc, crc);
    if(cal_crc != crc)
        return -1;

    return 0;
}

int ota_image_copy(unsigned int dst, unsigned int src, unsigned int size)
{
    int ret = 0;
    unsigned char data_buf[SECTOR_SIZE] = {0};
    unsigned int addr = 0, read_size = 0;
    OTA_LOG_I("ota copy data src:0x%08x dst:0x%08x size;0x%08x \n", src, dst, size);
    ret = flash_erase(dst, size);
    if (ret != 0){
        OTA_LOG_I("copy e ret:%d.\n",ret);
    }
    for(addr = dst; size > 0; size -= read_size, src += read_size, addr += read_size) {
        read_size = size >= SECTOR_SIZE ? SECTOR_SIZE : size;
        memset(data_buf, 0, SECTOR_SIZE);
        ret = flash_read_data(src, data_buf, read_size);
        if (ret != 0){
            OTA_LOG_I("copy r ret:%d.\n",ret);
        }
        ret = flash_write_data(addr, data_buf, read_size);
        if (ret != 0){
            OTA_LOG_I("copy w ret:%d.\n",ret);
        }
    }
    return ret;
}

static bool ota_is_xz_header(unsigned int addr)
{
    unsigned char data_buf[16] = {0};
    const unsigned char xz_header[6] = { 0xFD, '7', 'z', 'X', 'Z', 0x00 };
    int ret = flash_read_data(addr, data_buf, 16);
    if (0 != ret){
        OTA_LOG_I("r xz ret:%d.\n",ret);
    }
    if (!memcmp(data_buf, xz_header, 6)) {
        OTA_LOG_I("is xz file.\n");
        return true;
    }
    return false;
}

static int32_t ota_copy_xz(unsigned int dst_addr, unsigned int src_addr, unsigned int size)
{
    struct xz_buf b;
    struct xz_dec *s;
    enum xz_ret ret = XZ_OK;
    unsigned char* in_buf = NULL;
    unsigned char* out_buf = NULL;
    unsigned int read_size;
    unsigned int rema_size = size;
    unsigned int read_addr = src_addr;
    unsigned int write_addr = dst_addr;
    unsigned int erase_addr = write_addr & ~0xFFF;
    int err = 0;

    err = flash_erase(erase_addr, SECTOR_SIZE);
    if(0 != err) {
        OTA_LOG_I("e ret:%d \n",err);
    }
    xz_crc32_init();
    s = xz_dec_init(XZ_DYNALLOC, 1 << 15);
    if (s == NULL) {
        OTA_LOG_I("mem fail\n");
        goto exit;
    }
    in_buf = malloc(XZ_BUF_SIZE);
    if(in_buf == NULL) {
        return OTA_XZ_MEM_FAIL;
    }
    out_buf = malloc(XZ_BUF_SIZE);
    if(out_buf == NULL) {
        return OTA_XZ_MEM_FAIL;
    }

    b.in = in_buf;
    b.in_pos = 0;
    b.in_size = 0;
    b.out = out_buf;
    b.out_pos = 0;
    b.out_size = XZ_BUF_SIZE;

    while (1) {
        if (b.in_pos == b.in_size) {
	     read_size = rema_size > XZ_BUF_SIZE ? XZ_BUF_SIZE : rema_size;
             err = flash_read_data(read_addr, in_buf, read_size);
             if (0 != err) {
		OTA_LOG_I("r err:%d.\n",err);
             }
             if(read_size > 0) {
		b.in_size = read_size;
		b.in_pos = 0;
             }
             rema_size -= read_size;
             read_addr += read_size;
	}
	ret = xz_dec_run(s, &b);
	//OTA_LOG_I("xz dec ret:%d p:0x%0x s:0x%x out p:0x%0x s:0x%x \n", ret, read_addr, b.in_size, write_addr, b.out_size);
	if (b.out_pos == b.out_size) {
             if(write_addr + b.out_pos >= erase_addr + SECTOR_SIZE) {
                 erase_addr += SECTOR_SIZE;
		 err = flash_erase(erase_addr, SECTOR_SIZE);
		 if(0 != err) {
                      OTA_LOG_I("e f err:%d\n",err);
		 }
             }
             printf(".");
             err = flash_write_data(write_addr, out_buf, b.out_pos);
             if (err != 0) {
	         OTA_LOG_I("w f err:%d.\n",err);
             }
             write_addr += b.out_pos;
             b.out_pos = 0;
        }
        if (ret == XZ_OK)
            continue;
        if(write_addr + b.out_pos >= erase_addr + SECTOR_SIZE) {
            erase_addr += SECTOR_SIZE;
	    err = flash_erase(erase_addr, SECTOR_SIZE);
	    if(0 != err) {
		OTA_LOG_I("er #err:%d\n",err);
	    }
        }
	err = flash_write_data(write_addr, out_buf, b.out_pos);
	if (err != 0) {
	    OTA_LOG_I("w f #err:%d.\n",err);
	}
	switch (ret) {
		case XZ_STREAM_END:
                     xz_dec_end(s);
                     printf(".\r\n");
                     return 0;
		case XZ_MEM_ERROR:
                     goto exit;
		case XZ_MEMLIMIT_ERROR:
                     goto exit;
		case XZ_FORMAT_ERROR:
                     goto exit;
		case XZ_OPTIONS_ERROR:
                     goto exit;
		case XZ_DATA_ERROR:
		case XZ_BUF_ERROR:
                     goto exit;
		default:
		     goto exit;
        }
     }
exit:
    xz_dec_end(s);
    if(in_buf) {
        free(in_buf);
    }
    if(out_buf) {
        free(out_buf);
    }
    return ret;
}

static int ota_xz_uncompress(unsigned int dst_addr, unsigned int src_addr, unsigned int size)
{
    if(ota_is_xz_header(src_addr) == true) {
        return ota_copy_xz(dst_addr, src_addr, size);
    }
    return -1;
}

static int32_t ota_verify_xz(unsigned int dst_addr, unsigned int src_addr, unsigned int size)
{
    struct xz_buf b;
    struct xz_dec *s;
    enum xz_ret ret = XZ_OK;
    unsigned char data_buf[SECTOR_SIZE] = {0};
    unsigned char* in_buf = NULL;
    unsigned char* out_buf = NULL;
    unsigned int read_size;
    unsigned int rema_size = size;
    unsigned int read_addr = src_addr;
    unsigned int write_addr = dst_addr;
    int err = 0;
    xz_crc32_init();
    s = xz_dec_init(XZ_DYNALLOC, 1 << 15);
    if (s == NULL) {
	OTA_LOG_I("xz fail\n");
	goto exit;
    }
    in_buf = malloc(XZ_BUF_SIZE);
    if(in_buf == NULL) {
        return OTA_XZ_MEM_FAIL;
    }
    out_buf = malloc(XZ_BUF_SIZE);
    if(out_buf == NULL) {
        return OTA_XZ_MEM_FAIL;
    }

    b.in = in_buf;
    b.in_pos = 0;
    b.in_size = 0;
    b.out = out_buf;
    b.out_pos = 0;
    b.out_size = XZ_BUF_SIZE;

    while (1)  {
	if (b.in_pos == b.in_size) {
             read_size = rema_size > XZ_BUF_SIZE ? XZ_BUF_SIZE : rema_size;
             err = flash_read_data(read_addr, in_buf, read_size);
             if (0 != err) {
		  OTA_LOG_I("r v err:%d.\n",err);
             }
             if(read_size > 0) {
	          b.in_size = read_size;
		  b.in_pos = 0;
             }
             rema_size -= read_size;
             read_addr += read_size;
	}
	ret = xz_dec_run(s, &b);
        //OTA_LOG_I("v xz dec ret:%d p:0x%0x s:0x%x out p:0x%0x s:0x%x \n", ret, b.in_pos, b.in_size, b.out_pos, b.out_size);
	if (b.out_pos == b.out_size) {
             memset(data_buf, 0, SECTOR_SIZE);
             err = flash_read_data(write_addr, data_buf, b.out_pos);
             if (0 != err){
		  OTA_LOG_I("v r err:%d.\n",err);
             }
             if(memcmp(data_buf, out_buf, b.out_pos) != 0)
                  goto exit;
                  write_addr += b.out_pos;
		  b.out_pos = 0;
             }
             if (ret == XZ_OK)
		  continue;
             err = flash_read_data(write_addr, data_buf, b.out_pos);
	     if (0 != err){
		  OTA_LOG_I("r w err:%d.\n",err);
             }
             if(memcmp(data_buf, out_buf, b.out_pos) != 0)
                  goto exit;
             switch (ret) {
	     case XZ_STREAM_END:
                  xz_dec_end(s);
                  OTA_LOG_I("verify xz OK.\n");
		  return 0;
	     case XZ_MEM_ERROR:
		  goto exit;
	     case XZ_MEMLIMIT_ERROR:
		  goto exit;
	     case XZ_FORMAT_ERROR:
		  goto exit;
	     case XZ_OPTIONS_ERROR:
		  goto exit;
	     case XZ_DATA_ERROR:
	     case XZ_BUF_ERROR:
                  goto exit;
	     default:
		  goto exit;
	     }
        }
exit:
    if(in_buf) {
        free(in_buf);
    }
    if(out_buf) {
        free(out_buf);
    }
    xz_dec_end(s);
    return ret;
}

static int ota_xz_verify(unsigned int dst_addr, unsigned int src_addr, unsigned int size)
{
    if(ota_is_xz_header(src_addr) == true) {
        return ota_verify_xz(dst_addr, src_addr, size);
    }
    return -1;
}

int ota_xz_main(void)
{
    int ret = 0;
    ota_boot_param_t ota_hdl = {0};
    hal_logic_partition_t *par_info = flash_get_info(HAL_PARTITION_PARAMETER_1);
    hal_logic_partition_t *ota_info = flash_get_info(HAL_PARTITION_OTA_TEMP);
    hal_logic_partition_t *app_info = flash_get_info(HAL_PARTITION_APPLICATION);
    if(par_info == NULL || ota_info == NULL || app_info == NULL) {
        printf("flash info err.\r\n");
        return -1;
    }
    ota_patch_read_param(&ota_hdl);
    OTA_LOG_I("ota d:0x%08x,s:0x%08x,n:0x%08x,crc:0x%04x upg_flag:0x%04x pd:0x%08x \n", ota_hdl.dst_adr, ota_hdl.src_adr, ota_hdl.len, ota_hdl.crc, ota_hdl.upg_flag, par_info->partition_start_addr);
    if( ota_hdl.dst_adr > app_info->partition_start_addr + app_info->partition_length || 
        ota_hdl.src_adr > ota_info->partition_start_addr + ota_info->partition_length || 
        ota_hdl.len > ota_info->partition_length || 
        ota_hdl.len <= 0 || 
        ota_hdl.upg_flag != OTA_UPGRADE_XZ) {
        return OTA_XZ_PARAM_FAIL;
    }
    /* check image crc16 */    
    if(ota_image_check(ota_hdl.src_adr, ota_hdl.len, ota_hdl.crc) != 0) {
        return OTA_XZ_CRC_FAIL;
    }

    OTA_LOG_I("xz uncompress ...\n");
    /* XZ uncompress */ 
    if(ota_xz_uncompress(ota_hdl.dst_adr, ota_hdl.src_adr, ota_hdl.len) != 0) {
        return OTA_XZ_CRC_FAIL;
    }

    /* XZ verify */
    if(ota_xz_verify(ota_hdl.dst_adr, ota_hdl.src_adr, ota_hdl.len) != 0) {
        return OTA_XZ_VERIFY_FAIL;
    }
    return ret;
}


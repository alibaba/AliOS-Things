#include "hal/soc/soc.h"
#include <ti/drivers/net/wifi/simplelink.h>

//#define DEBUG_FD csp_printf
#define DEBUG_FD

unsigned long max_size = 8*1024;

static int open_flash(int pno, bool w)
{
    char fn[64];
    int flash_fd;
    int fs_open_flag = 0;
    int ret;
    
    start_nwp();
    
    snprintf(fn, sizeof(fn), "aos_partition_5%d.bin", pno);

    if(w)
	fs_open_flag = SL_FS_OVERWRITE;
    else
        fs_open_flag = SL_FS_READ;

    flash_fd = sl_FsOpen((unsigned char *)fn, fs_open_flag, NULL);

    if (flash_fd >= 0) {
        goto out;
    }
    else
      DEBUG_FD("File ID is:0x%08x\r\n", flash_fd);

    flash_fd = sl_FsOpen((unsigned char *)fn, SL_FS_CREATE | SL_FS_OVERWRITE | SL_FS_CREATE_MAX_SIZE(max_size), NULL);
    if (flash_fd < 0)
        goto out;

    char *buf = (char *)malloc(max_size);
    if (!buf) {
        goto out;
    }
    memset(buf, 0xFF, max_size);

    ret = sl_FsWrite(flash_fd, 0, (unsigned char *)buf, max_size);
    
    DEBUG_FD("initialized writing %d bytes\r\n", ret);
    free(buf);
    sl_FsClose(flash_fd, NULL, NULL, 0);
    flash_fd = sl_FsOpen((unsigned char *)fn, fs_open_flag, NULL);;
out:
    return flash_fd;
}

int32_t hal_flash_write(hal_partition_t pno, uint32_t* poff, const void* buf ,uint32_t buf_size)
{
    int ret;
    char *tmp, *new_val;
    int i;

    int flash_fd;
      
    flash_fd = open_flash(pno, false);
    if (flash_fd < 0)
        return -1;

    char *origin = (char *)malloc(max_size);
    if (!origin) {
        ret = -1;
        goto exit;
    }
    memset(origin, 0xFF, max_size);

    ret = sl_FsRead(flash_fd, 0, (unsigned char *)origin, max_size);
    if (ret < 0) {
        DEBUG_FD("error reading flash before writing:%d\r\n", ret);
        goto exit;
    }

    memcpy(origin + (*poff), buf, buf_size);
    
    sl_FsClose(flash_fd, NULL, NULL, 0);

    flash_fd = open_flash(pno, true);
    if (flash_fd < 0) {
	ret = flash_fd;
        goto exit;
    }

    char *tmp_buf = (char *)buf;
    if(buf_size < 1024) {
      DEBUG_FD("Write to flash with offset: %d\r\n", *poff);
      for(i = 0; i < buf_size; i ++) {
        DEBUG_FD(" %2x", tmp_buf[i]);
        if(i > 0 && i%64 == 0)
            DEBUG_FD("\r\n");
      }
      DEBUG_FD("\r\n====================================\r\n");
    }
    
    ret = sl_FsWrite(flash_fd, 0, (unsigned char *)origin, max_size);
    
    if (ret < 0)
        DEBUG_FD("error writing flash:%d\r\n", ret);
    else
        *poff += ret;

exit:
    sl_FsClose(flash_fd, NULL, NULL, 0);     
    free(origin);      
    return ret < 0 ? ret : 0;
}

int32_t hal_flash_read(hal_partition_t pno, uint32_t* poff, void* buf, uint32_t buf_size)
{
    int flash_fd;
    int i;
    int offset;

    if (poff == NULL)
        return -1;

    flash_fd = open_flash(pno, false);
    if (flash_fd < 0)
        return -1;

    DEBUG_FD("Read from flash with offset :%d \r\n", *poff);
    
    offset = *poff;
    
    int ret = sl_FsRead(flash_fd, offset, (unsigned char *)buf, buf_size);
    if (ret < 0) {
        DEBUG_FD("error reading flash: %d\r\n", ret);
    }
    else
        *poff += ret;
    
    char *tmp_buf = (char *)buf;
    
    for(i = 0; i < buf_size; i ++){
      DEBUG_FD(" %2x", tmp_buf[i]);
      if(i > 0 && i %64 == 0)
            DEBUG_FD("\r\n");
    }
    
    DEBUG_FD("\r\n====================================\r\n");
    
    sl_FsClose(flash_fd, NULL, NULL, 0);

    return ret < 0 ? ret : 0;
}

int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set,
                        uint32_t size)
{
    int ret;
    int flash_fd = open_flash(in_partition, false);
    if (flash_fd < 0)
        return -1;
    
    char *origin = (char *)malloc(max_size);
    if (!origin) {
        ret = -1;
        goto exit;
    }
    memset(origin, 0xFF, max_size);
    
    ret = sl_FsRead(flash_fd, 0, (unsigned char *)origin, max_size);
    if (ret < 0) {
        DEBUG_FD("error reading flash before writing:%d\r\n", ret);
        goto exit;
    }
    sl_FsClose(flash_fd, NULL, NULL, 0);
    
    memset(origin + off_set, 0xFF, size);
    
    flash_fd = open_flash(in_partition, true);
    if (flash_fd < 0) {
	ret = flash_fd;
        goto exit;
    }

    ret = sl_FsWrite(flash_fd, 0, (unsigned char *)origin, max_size);
    if (ret < 0)
        DEBUG_FD("error erase flash:%d\r\n", ret);
    
exit:
    sl_FsClose(flash_fd, NULL, NULL, 0);
    free(origin);
    return ret < 0 ? ret : 0;
}

int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}




#include <string.h>
#include <assert.h>
#include "flash.h"
#include "nand_flash.h"
#include "nftl_bd.h"
#include "inc/nftl_conf.h"

#define PART0_TOTAL_BLOCK_NUMS (NFTL_PART0_BLOCK_NUMS + NFTL_PART0_BUFFER_BLOCK_NUMS)
#define PART1_TOTAL_BLOCK_NUMS (NFTL_PART1_BLOCK_NUMS + NFTL_PART1_BUFFER_BLOCK_NUMS)
#define PART2_TOTAL_BLOCK_NUMS (NFTL_PART2_BLOCK_NUMS + NFTL_PART2_BUFFER_BLOCK_NUMS)

#define PAGE_SIZE NFTL_PAGE_SIZE
#define BLOCK_SIZE NFTL_BLOCK_SIZE
static const char *_dev_path = "/tmp/nftl_bd";
static struct nftl_config _cfg;

typedef struct {
    hal_partition_t p;
    uint8_t tbl_idx;
} part_info_t;

typedef struct {
    uint32_t offset;
    uint32_t nums;
} offset_info_t;

static offset_info_t _blk_offset_tbl[] = {
    {0, PART0_TOTAL_BLOCK_NUMS},
    {PART0_TOTAL_BLOCK_NUMS, PART1_TOTAL_BLOCK_NUMS},
    {PART0_TOTAL_BLOCK_NUMS + PART1_TOTAL_BLOCK_NUMS, PART2_TOTAL_BLOCK_NUMS},
};

part_info_t _pinfo[] = {
    {NFTL_PHYSICAL_PARTITION0, 0},
    {NFTL_PHYSICAL_PARTITION1, 1},
    {NFTL_PHYSICAL_PARTITION2, 2},
};

static offset_info_t* get_offset_info(hal_partition_t p)
{
    for (int i = 0; i < (sizeof(_pinfo) / sizeof(_pinfo[0])); i++) {
        if (_pinfo[i].p == p) {
            return &(_blk_offset_tbl[_pinfo[i].tbl_idx]);
        }
    }

    return NULL;
}

int32_t hal_flash_init()
{
    memset(&_cfg, 0, sizeof(_cfg));
    _cfg.prog_size = PAGE_SIZE;
    _cfg.block_size = BLOCK_SIZE;
    _cfg.block_count = NFTL_BLOCK_NUMS;
    return nftl_bd_create(&_cfg, _dev_path);
}

int32_t hal_flash_destroy()
{
    nftl_bd_destroy(&_cfg);
    return 0;
}

/**
 * Get the information of the specified flash area
 *
 * @param[in]  in_partition     The target flash logical partition
 * @param[in]  partition        The buffer to store partition info
 *
 * @return  0: On success， otherwise is error
 */
int32_t hal_flash_info_get(hal_partition_t in_partition, hal_logic_partition_t *partition)
{
    memset(partition, 0, sizeof(hal_logic_partition_t));
    return 0;
}

/**
 * Erase an area on a Flash logical partition
 *
 * @note  Erase on an address will erase all data on a sector that the
 *        address is belonged to, this function does not save data that
 *        beyond the address area but in the affected sector, the data
 *        will be lost.
 *
 * @param[in]  in_partition  The target flash logical partition which should be erased
 * @param[in]  off_set       Start address of the erased flash area
 * @param[in]  size          Size of the erased flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase(hal_partition_t in_partition, uint32_t off_set, uint32_t size)
{
    offset_info_t *info =  get_offset_info(in_partition);
    assert(info != NULL);
    assert((off_set + size) <= (info->nums * _cfg.block_size));
    return nftl_bd_erase(&_cfg, info->offset + off_set / _cfg.block_size);
}

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write(hal_partition_t in_partition, uint32_t *off_set,
                        const void *in_buf, uint32_t in_buf_len)
{
    int ret;

    offset_info_t *info =  get_offset_info(in_partition);
    assert(info != NULL);
    assert((*off_set + in_buf_len) <= (info->nums * _cfg.block_size));

    ret = nftl_bd_prog(&_cfg, info->offset + *off_set / _cfg.block_size,
                       *off_set % _cfg.block_size,
                       in_buf, in_buf_len);

    *off_set += in_buf_len;

    return ret;
}

/**
 * Write data to an area on a flash logical partition without erase
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  data_buf        point to the data buffer that will be written to flash
 * @param[in]  data_buf_len    The length of the data buffer
 * @param[in]  spare_buf       point to the spare data buffer that will be written to flash
 * @param[in]  spare_buf_len   The length of the spare buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_write_with_spare(hal_partition_t in_partition, uint32_t *off_set,
                                   const void *data_buf, uint32_t data_buf_len,
                                   const void *spare_buf, uint32_t spare_buf_len)
{
    int ret;

    offset_info_t *info =  get_offset_info(in_partition);
    assert(info != NULL);
    assert((*off_set + data_buf_len) <= (info->nums * _cfg.block_size));

    ret = nftl_bd_prog_with_spare(&_cfg, info->offset + *off_set / _cfg.block_size,
                                  *off_set % _cfg.block_size,
                                  data_buf, data_buf_len,
                                  spare_buf, spare_buf_len);

    *off_set += data_buf_len;

    return ret;
}

/**
 * Write data to an area on a flash logical partition with erase first
 *
 * @param[in]  in_partition    The target flash logical partition which should be read which should be written
 * @param[in]  off_set         Point to the start address that the data is written to, and
 *                             point to the last unwritten address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  inBuffer        point to the data buffer that will be written to flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_erase_write(hal_partition_t in_partition, uint32_t *off_set,
                              const void *in_buf, uint32_t in_buf_len)
{
    int ret;

    offset_info_t *info =  get_offset_info(in_partition);
    assert(info != NULL);
    assert((*off_set + in_buf_len) <= (info->nums * _cfg.block_size));

    nftl_bd_erase(&_cfg, info->offset + *off_set / _cfg.block_size);
    ret = nftl_bd_prog(&_cfg, info->offset + *off_set / _cfg.block_size,
                       *off_set % _cfg.block_size,
                       in_buf, in_buf_len);

    *off_set += in_buf_len;

    return ret;
}

/**
 * Read data from an area on a Flash to data buffer in RAM
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  outBuffer       Point to the data buffer that stores the data read from flash
 * @param[in]  inBufferLength  The length of the buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read(hal_partition_t in_partition, uint32_t *off_set,
                       void *out_buf, uint32_t in_buf_len)
{
    int32_t ret;

    offset_info_t *info =  get_offset_info(in_partition);
    assert(info != NULL);
    assert((*off_set + in_buf_len) <= (info->nums * _cfg.block_size));

    ret = nftl_bd_read(&_cfg, info->offset + *off_set / _cfg.block_size,
                       *off_set % _cfg.block_size,
                       out_buf, in_buf_len);

    *off_set += in_buf_len;

    return ret;
}

/**
 * Read data (with spare) from an area on a Flash to data buffer in RAM,
 *
 * @param[in]  in_partition    The target flash logical partition which should be read
 * @param[in]  off_set         Point to the start address that the data is read, and
 *                             point to the last unread address after this function is
 *                             returned, so you can call this function serval times without
 *                             update this start address.
 * @param[in]  data_buf        Point to the data buffer that stores the data read from flash
 * @param[in]  data_buf_len    The length of the data buffer
 * @param[in]  spare_buf       Point to the spare data buffer that stores the data read from flash
 * @param[in]  spare_buf_len   The length of the spare buffer
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_read_with_spare(hal_partition_t in_partition, uint32_t *off_set,
                                  void *data_buf, uint32_t data_buf_len,
                                  void *spare_buf, uint32_t spare_buf_len)
{
    int ret;

    offset_info_t *info =  get_offset_info(in_partition);
    assert(info != NULL);
    assert((*off_set + data_buf_len) <= (info->nums * _cfg.block_size));

    ret = nftl_bd_read_with_spare(&_cfg, info->offset + *off_set / _cfg.block_size,
                                  *off_set % _cfg.block_size,
                                  data_buf, data_buf_len,
                                  spare_buf, spare_buf_len);

    *off_set += data_buf_len;

    return ret;
}

/**
 * Set security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of enabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_enable_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

/**
 * Disable security options on a logical partition
 *
 * @param[in]  partition  The target flash logical partition
 * @param[in]  offset     Point to the start address that the data is read, and
 *                        point to the last unread address after this function is
 *                        returned, so you can call this function serval times without
 *                        update this start address.
 * @param[in]  size       Size of disabled flash area
 *
 * @return  0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_dis_secure(hal_partition_t partition, uint32_t off_set, uint32_t size)
{
    return 0;
}

/**
 * Convert physical address to logic partition id and offset in partition
 *
 * @param[out]  in_partition Point to the logic partition id
 * @param[out]  off_set      Point to the offset in logic partition
 * @param[in]   addr         The physical address
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_addr2offset(hal_partition_t *in_partition, uint32_t *off_set, uint32_t addr)
{
    return 0;
}

int hal_flash_isbad(hal_partition_t in_partition, uint32_t off_set)
{
    return nftl_bd_isbad(&_cfg, off_set / _cfg.block_size);
}

int hal_flash_markbad(hal_partition_t in_partition, uint32_t off_set)
{
    return nftl_bd_markbad(&_cfg, off_set / _cfg.block_size);
}

uint32_t hal_flash_get_pgsize()
{
    return PAGE_SIZE;
}

uint32_t hal_flash_get_blksize()
{
    return BLOCK_SIZE;
}

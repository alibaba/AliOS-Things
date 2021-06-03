/**
 * ant_fs.h
 *
 * 文件系统接口
 */
#ifndef __ANT_FS_H__
#define __ANT_FS_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROOTDIR                   "C:\\"
#define CUSTOM_AUDIO_DIR          "C:\\"

//文件最大长度
#define ANT_MAX_FILE_PATH_LEN     256
#define ANT_FS_TRIGGER_FORMAT     1000

// 文件打开和创建方式
#define ANT_FS_READ_WRITE         0x00000000L
#define ANT_FS_READ_ONLY          0x00000100L
#define ANT_FS_CREATE             0x00010000L
#define ANT_FS_CREATE_ALWAYS      0x00020000L
#define ANT_FS_OPEN_SHARED        0x00000200L

// 文件移动方式
#define ANT_FS_MOVE_COPY          0x00000001     // Move file|folder by copy
#define ANT_FS_MOVE_KILL          0x00000002     // Delete the moved file|folder after moving

/****************************************************************************
 * 文件系统错误码
 ***************************************************************************/
#define ANT_FS_PARAM_ERROR                    -2    /* User */
#define ANT_FS_INVALID_FILENAME               -3    /* User */
#define ANT_FS_DRIVE_NOT_FOUND                -4    /* User or Fatal */
#define ANT_FS_TOO_MANY_FILES                 -5    /* User or Normal: use over max file handle number or more than 256 files in sort */
#define ANT_FS_NO_MORE_FILES                  -6    /* Normal */
#define ANT_FS_WRONG_MEDIA                    -7    /* Fatal */
#define ANT_FS_INVALID_FILE_SYSTEM            -8    /* Fatal */
#define ANT_FS_FILE_NOT_FOUND                 -9    /* User or Normal */
#define ANT_FS_INVALID_FILE_HANDLE           -10    /* User or Normal */
#define ANT_FS_UNSUPPORTED_DEVICE            -11    /* User */
#define ANT_FS_UNSUPPORTED_DRIVER_FUNCTION   -12    /* User or Fatal */
#define ANT_FS_CORRUPTED_PARTITION_TABLE     -13    /* fatal */
#define ANT_FS_TOO_MANY_DRIVES               -14    /* not use so far */
#define ANT_FS_INVALID_FILE_POS              -15    /* User */
#define ANT_FS_ACCESS_DENIED                 -16    /* User or Normal */
#define ANT_FS_STRING_BUFFER_TOO_SMALL       -17    /* User */
#define ANT_FS_GENERAL_FAILURE               -18    /* Normal */
#define ANT_FS_PATH_NOT_FOUND                -19    /* User */
#define ANT_FS_FAT_ALLOC_ERROR               -20    /* Fatal: disk crash */
#define ANT_FS_ROOT_DIR_FULL                 -21    /* Normal */
#define ANT_FS_DISK_FULL                     -22    /* Normal */
#define ANT_FS_TIMEOUT                       -23    /* Normal: FS_CloseMSDC with nonblock */
#define ANT_FS_BAD_SECTOR                    -24    /* Normal: NAND flash bad block */
#define ANT_FS_DATA_ERROR                    -25    /* Normal: NAND flash bad block */
#define ANT_FS_MEDIA_CHANGED                 -26    /* Normal */
#define ANT_FS_SECTOR_NOT_FOUND              -27    /* Fatal */
#define ANT_FS_ADDRESS_MARK_NOT_FOUND        -28    /* not use so far */
#define ANT_FS_DRIVE_NOT_READY               -29    /* Normal */
#define ANT_FS_WRITE_PROTECTION              -30    /* Normal: only for MSDC */
#define ANT_FS_DMA_OVERRUN                   -31    /* not use so far */
#define ANT_FS_CRC_ERROR                     -32    /* not use so far */
#define ANT_FS_DEVICE_RESOURCE_ERROR         -33    /* Fatal: Device crash */
#define ANT_FS_INVALID_SECTOR_SIZE           -34    /* Fatal */
#define ANT_FS_OUT_OF_BUFFERS                -35    /* Fatal */
#define ANT_FS_FILE_EXISTS                   -36    /* User or Normal */
#define ANT_FS_LONG_FILE_POS                 -37    /* User : FS_Seek new pos over sizeof int */
#define ANT_FS_FILE_TOO_LARGE                -38    /* User: filesize + pos over sizeof int */
#define ANT_FS_BAD_DIR_ENTRY                 -39    /* Fatal */
#define ANT_FS_ATTR_CONFLICT                 -40    /* User: Can't specify FS_PROTECTION_MODE and FS_NONBLOCK_MOD */
#define ANT_FS_CHECKDISK_RETRY               -41    /* System: don't care */
#define ANT_FS_LACK_OF_PROTECTION_SPACE      -42    /* Fatal: Device crash */
#define ANT_FS_SYSTEM_CRASH                  -43    /* Normal */
#define ANT_FS_FAIL_GET_MEM                  -44    /* Normal */
#define ANT_FS_READ_ONLY_ERROR               -45    /* User or Normal */
#define ANT_FS_DEVICE_BUSY                   -46    /* Normal */
#define ANT_FS_ABORTED_ERROR                 -47    /* Normal */
#define ANT_FS_QUOTA_OVER_DISK_SPACE         -48    /* Normal: Configuration Mistake */
#define ANT_FS_PATH_OVER_LEN_ERROR           -49    /* Normal */
#define ANT_FS_APP_QUOTA_FULL                -50    /* Normal */
#define ANT_FS_VF_MAP_ERROR                  -51    /* User or Normal */
#define ANT_FS_DEVICE_EXPORTED_ERROR         -52    /* User or Normal */
#define ANT_FS_DISK_FRAGMENT                 -53    /* Normal */
#define ANT_FS_DIRCACHE_EXPIRED              -54    /* Normal */
#define ANT_FS_QUOTA_USAGE_WARNING           -55    /* Normal or Fatal: System Drive Free Space Not Enought */
#define ANT_FS_ERR_DIRDATA_LOCKED            -56    /* Normal */
#define ANT_FS_INVALID_OPERATION             -57    /* Normal */
#define ANT_FS_ERR_VF_PARENT_CLOSED          -58    /* Virtual file's parent is closed */
#define ANT_FS_ERR_UNSUPPORTED_SERVICE       -59    /* The specified FS service is closed in this project. */
#define ANT_FS_ERR_INVALID_JOB_ID            -81
#define ANT_FS_ERR_ASYNC_JOB_NOT_FOUND       -82
#define ANT_FS_MSDC_MOUNT_ERROR             -100    /* Normal */
#define ANT_FS_MSDC_READ_SECTOR_ERROR       -101    /* Normal */
#define ANT_FS_MSDC_WRITE_SECTOR_ERROR      -102    /* Normal */
#define ANT_FS_MSDC_DISCARD_SECTOR_ERROR    -103    /* Normal */
#define ANT_FS_MSDC_PRESNET_NOT_READY       -104    /* System */
#define ANT_FS_MSDC_NOT_PRESENT             -105    /* Normal */
#define ANT_FS_EXTERNAL_DEVICE_NOT_PRESENT  -106    /* Normal */
#define ANT_FS_HIGH_LEVEL_FORMAT_ERROR      -107    /* Normal */
#define ANT_FS_CARD_BATCHCOUNT_NOT_PRESENT  -110    /* Normal */
#define ANT_FS_FLASH_MOUNT_ERROR            -120    /* Normal */
#define ANT_FS_FLASH_ERASE_BUSY             -121    /* Normal: only for nonblock mode */
#define ANT_FS_NAND_DEVICE_NOT_SUPPORTED    -122    /* Normal: Configuration Mistake */
#define ANT_FS_FLASH_OTP_UNKNOWERR          -123    /* User or Normal */
#define ANT_FS_FLASH_OTP_OVERSCOPE          -124    /* User or Normal */
#define ANT_FS_FLASH_OTP_WRITEFAIL          -125    /* User or Normal */
#define ANT_FS_FDM_VERSION_MISMATCH         -126    /* System */
#define ANT_FS_FLASH_OTP_LOCK_ALREADY       -127    /* User or Normal */
#define ANT_FS_FDM_FORMAT_ERROR             -128    /* The format of the disk content is not correct */
#define ANT_FS_FDM_USER_DRIVE_BROKEN        -129    /* User drive unrecoverable broken*/
#define ANT_FS_FDM_SYS_DRIVE_BROKEN         -130    /* System drive unrecoverable broken*/
#define ANT_FS_FDM_MULTIPLE_BROKEN          -131    /* multiple unrecoverable broken*/
#define ANT_FS_LOCK_MUTEX_FAIL              -141    /* System: don't care */
#define ANT_FS_NO_NONBLOCKMODE              -142    /* User: try to call nonblock mode other than NOR flash */
#define ANT_FS_NO_PROTECTIONMODE            -143    /* User: try to call protection mode other than NOR flash */
/*
 * If disk size exceeds FS_MAX_DISK_SIZE (unit is KB, defined in makefile),
 * FS_TestMSDC(), FS_GetDevStatus(FS_MOUNT_STATE_ENUM) and all access behaviors will
 * get this error code.
 */
#define ANT_FS_DISK_SIZE_TOO_LARGE          (ANT_FS_MSDC_MOUNT_ERROR),     /*Normal*/
#define ANT_FS_MINIMUM_ERROR_CODE           -65536 /* 0xFFFF0000 */

/****************************************************************************
 * 文件删除扩展方式
 ***************************************************************************/
#define ANT_FS_FILE_TYPE              0x00000004     
#define ANT_FS_DIR_TYPE               0x00000008     
#define ANT_FS_RECURSIVE_TYPE         0x00000010

/****************************************************************************
 * Constants for File Seek
 ***************************************************************************/
typedef enum
{
   ANT_FS_FILE_BEGIN,    // Beginning of file
   ANT_FS_FILE_CURRENT,  // Current position of file pointer
   ANT_FS_FILE_END       // End of file
} ant_fs_seekpos_e;

typedef enum 
{
    ANT_FS_UFS = 1,       // U盘存储
    ANT_FS_SD  = 2,       // SD卡存储
    ANT_FS_RAM = 3,       // RAM存储
    ANT_FS_FAT = 0xFF     // norflash FAT存储
} ant_fs_storage_e;

typedef enum {
    ANT_FS_FMT_DONE = -1,
    ANT_FS_FMT_FAIL,
    ANT_FS_FMT_LOW_FMT,
    ANT_FS_FMT_HIGH_FMT 
} ant_fs_format_action_e;

typedef void (* ant_fs_format_cb)(const char *device_name, ant_fs_format_action_e action, ant_u32 total, ant_u32 completed);


typedef struct
{
    ant_s16 fs_index;  ///< internal fs index
    ant_s16 _reserved; ///< reserved
} ant_fs_dir_t;

/**
 * dirent data structure
 */
typedef struct
{
    ant_s32 d_ino;            ///< inode number, file system implementation can use it for any purpose
    ant_u8 d_type; ///< type of file
    ant_char d_name[256];     ///< file name
}ant_fs_dirent_t;

/**
 * 获取文件系统根路径
 *
 * @return 非空字符串：获取成功，返回文件系统根路径
 *               NULL：获取失败
 */
ant_char* ant_fs_get_root_path();


/**
 * 打开或创建一个文件
 *
 * @param filename 文件路径
 * @param flag 文件打开方式，参考文件打开和创建标志
 *
 * @return 大于等于0： 打开或创建成功，返回文件句柄
 *           其他值： 打开或创建失败，取值参见文件系统错误码
 */
ant_s32 ant_fs_open(char* filename, ant_u32 flag);

ant_s32 ant_ext_fs_open(char *filename, ant_u32 flag);

/**
 * 创建内存文件
 *
 * @param filename 文件名称
 * @param flag 创建标志，参考文件打开和创建标志
 * @param ramfile_size 内存文件大小
 * @return
 */
ant_s32 ant_fs_open_ramfile(char *filename, ant_u32 flag, ant_u32 ramfile_size);

/**
 * 从文件中当前位置读取数据
 *
 * @param handle 文件句柄
 * @param data 数据缓存
 * @param len 读取数据的长度
 *
 * @return  大于0： 读取成功，返回读取的数据长度
 *         其他值： 读取失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_read(ant_s32 handle, void *data, ant_u32 len);

/**
 * 向文件当前位置写入数据
 *
 * @param handle 文件句柄
 * @param data 写入数据缓存
 * @param len 写入数据长度
 *
 * @return 大于0： 写入成功，返回写入的数据长度
 *        其他值： 写入失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_write(ant_s32 handle, const void *data, ant_u32 len);

/**
 * 定位文件当前访问位置
 *
 * @param handle 文件句柄
 * @param offset 位置偏移
 * @param whence 起始位置，参见ant_fs_seekpos_e
 *
 * @return     0： 定位成功
 *         其他值： 定位失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_seek(ant_s32 handle, ant_s32 offset, ant_fs_seekpos_e whence);

/**
 * 获取文件当前访问位置
 *
 * @param handle 文件句柄
 *
 * @return 大于等于0： 返回文件当前访问位置
 *            其他值： 获取失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_tell(ant_s32 handle);

/**
 * 截断文件长度为0
 *
 * @param handle 文件句柄
 *
 * @return     0： 成功
 *         其他值： 失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_truncate(ant_s32 handle);

/**
 * 将文件缓冲区的数据写入磁盘（flash）
 *
 * @param handle 文件句柄
 */
void ant_fs_flush(ant_s32 handle);

/**
 * 关闭文件
 *
 * @param handle 文件句柄
 */
void ant_fs_close(ant_s32 handle);

/**
 * 获取文件长度
 *
 * @param filename 文件路径名
 *
 * @return 大于等于0： 返回文件的长度
 *            其他值： 失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_getsize(char *filename);

ant_s32 ant_ext_fs_getsize(char *filename);

/**
 * 删除文件
 *
 * @param filename 件路径名
 *
 * @return     0： 成功
 *         其他值： 失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_delete(char *filename);

ant_s32 ant_ext_fs_delete(char *filename);


/**
 * 返回文件是否存在
 *
 * @param filename 件路径名
 *
 * @return 1： 文件存在
 *         0： 文件不存在
 */
ant_s32 ant_fs_exist(char *filename);

/**
 * 判断文件当前位置是否在末尾
 *
 * @param filename 件路径名
 *
 * @return 1： 当前位置在末尾
 *         0： 当前位置不在末尾
 */
ant_bool ant_fs_eof(ant_s32 h);

/**
 * 文件重命名
 *
 * @param filename 重命名文件路径
 * @param filename_new 新命名文件路径
 *
 * @return     0： 文件重命名成功
 *         其他值： 文件重命名失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_rename(char *filename, char *filename_new);

ant_s32 ant_ext_fs_rename(char *filename, char *filename_new);


/**
 * 创建目录
 *
 * @param dirname 目录名称
 *
 * @return     0： 创建目录成功
 *         其他值： 创建失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_create_dir(char *dirname);

/**
 * 删除目录
 *
 * @param dirname 目录名称
 *
 * @return     0： 删除目录成功
 *         其他值： 删除目录失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_delete_dir(char *dirname);

/**
 * 开始遍历目录中的文件
 *
 * @param path 目录路径
 * @param filename 存储文件名的缓存
 * @param filename_len 存储文件名的缓存长度
 * @param file_size 返回遍历到的文件大小
 * @param is_dir  返回遍历的文件是否为目录，1：表示目录，0表示文件
 *
 * @return 大于等于0： 开始遍历成功，返回遍历句柄
 *           其他值： 遍历失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_find_first(char *path, char *filename, ant_u32 filename_len, ant_u32 *file_size, ant_bool *is_dir);

/**
 * 继续遍历目录中的文件
 *
 * @param path 目录路径
 * @param filename 存储文件名的缓存
 * @param filename_len 存储文件名的缓存长度
 * @param file_size 返回遍历到的文件大小
 * @param is_dir  返回遍历的文件是否为目录，1：表示目录，0表示文件
 *
 * @return     0： 遍历成功
 *         其他值： 遍历失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_find_next(ant_s32 handle, char *filename, ant_u32 filename_len, ant_u32 *file_size, ant_bool *is_dir);

/**
 * 结束目录遍历
 *
 * @param handle 遍历句柄
 */
   void ant_fs_find_close(ant_s32 handle);

/**
 * 删除文件扩展
 *
 * @param path 目录路径
 * @param flag 删除方式，参见文件删除方式
 *
 * @return     0： 删除成功
 *         其他值： 删除失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_delete_ext(char* path, ant_u32 flag);

/**
 * 获取用户文件系统剩余大小
 *
 * @return 文件系统剩余大小，单位为字节
 */
ant_s32 ant_fs_get_free_space(void);

/**
 * 获取用户文件系统总大小
 *
 * @return 文件系统总大小，单位为字节
 */
ant_s32 ant_fs_get_total_space(void);

/**
 * 文件系统格式化
 *
 * @param cb 格式化过程回调函数，参见 ant_fs_format_cb
 *
 * @return 0：格式化调用成功
 *         其他值： 格式化调用失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_format(ant_fs_format_cb cb);

/**
 * 创建临时文件
 *
 * @return 临时文件句柄
 */
ant_s32 ant_fs_tmpfile(void);

/**
 * 关闭临时文件
 *
 * @param handle
 *
 * @return     0： 关闭成功
 *         其他值： 关闭失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_tmpfile_close(ant_s32 handle);

/**
 * 向文件以打印格式输出
 *
 * @param handle 文件句柄
 * @param format 打印格式
 * @param ... 打印参数
 *
 * @return     0： 输出成功
 *         其他值： 输出失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_printf(ant_s32 handle, const char *format, ...);

/**
 * 从文件以打印格式输入
 *
 * @param handle 文件句柄
 * @param format 打印格式
 * @param ... 打印参数
 *
 * @return     0： 输入成功
 *         其他值： 输入失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_scanf(ant_s32 handle, const char *format, ...);

/**
 * 向文件写入一行
 *
 * @param str 写入的字符串
 * @param h 文件句柄
 *
 * @return 0： 写入成功
 *         其他值： 写入失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_puts(char *str, ant_s32 h);

/**
 * 从文件中获取一行
 *
 * @param str 写入的字符串
 * @param h 文件句柄
 *
 * @return 0： 获取成功
 *         其他值： 获取失败，返回错误码，参见文件系统错误码
 */
char *  ant_fs_gets(char *buf, ant_s32 len, ant_s32 h);

/**
 * 向文件写入一个字符
 * 
 * @param c 写入的字符
 * @param h 文件句柄
 * 
 * @return 0： 写入成功
 *         其他值： 写入失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_putc(char c, ant_s32 h);

/**
 * 从文件获取一个字符
 * 
 * @param c 获取的字符
 * @param h 文件句柄
 * 
 * @return 0： 获取成功
 *         其他值： 获取失败，返回错误码，参见文件系统错误码
 */
ant_s32 ant_fs_getc(char c, ant_s32 h);


char *ant_fs_get_real_path(char *filename);


ant_fs_dir_t *     ant_fs_open_dir(char * dirname);

ant_fs_dir_t *     ant_fs_open_dir_ext(char * dirname);

ant_fs_dirent_t *  ant_fs_read_dir(ant_fs_dir_t * p_dir );

ant_s32            ant_fs_close_dir(ant_fs_dir_t * p_dir);

ant_s32            ant_ext_fs_exist(char *filename);

ant_s32            ant_ext_fs_get_free_space(void);

ant_s32            ant_ext_fs_get_total_space(void);




#ifdef __cplusplus
} // extern "C"
#endif

#endif


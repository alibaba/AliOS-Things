#ifndef __ANT_EXT_FS_H__
#define __ANT_EXT_FS_H__

#define FILE_SYSTEM_FLASH_START_ADDR      (0)
#define FILE_SYSTEM_FLASH_SIZE            (8*1024*1024)

#define FILE_DIR   "/EXT\0"

ant_bool ant_ext_flash_init(void);
void     ant_ext_fs_filesystem_init(void);
ant_bool ant_ext_fs_creat_dir(char* dir);	

#endif


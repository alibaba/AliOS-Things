#ifndef _FSAL_H_
#define _FSAL_H_

#include "soc_defs.h"

#ifdef CONFIG_OS_ALIOS
#else
#include "sys/flash.h"
#include "spiffs_interface.h"
#include "spiffs_config.h"
#include "spiffs_nucleus.h"

typedef spiffs* 		SSV_FS;
typedef spiffs_file		SSV_FILE;
typedef spiffs_stat		SSV_FILE_STAT;
typedef spiffs_DIR		SSV_DIR;
typedef struct spiffs_dirent	SSV_DIRENT;

#define FS_errno(fs_handle)	prvSPIFFS_errno((fs_handle))
#define FS_info(fs_handle, ptrTOTAL_SIZE, ptrUSED_SIZE) prvSPIFFS_info((fs_handle),(ptrTOTAL_SIZE),(ptrUSED_SIZE))
#endif

SSV_FS		FS_init(void);
SSV_FS		FS_reset(void);
void		FS_list(SSV_FS fs);
void		FS_remove_prevota(SSV_FS fs);
int32_t		FS_create(SSV_FS fs, const char* path, uint32_t mode);
SSV_FILE	FS_open(SSV_FS fs, const char* path, uint32_t flags, uint32_t mode);
int32_t 	FS_read(SSV_FS fs, SSV_FILE file, void* buf, uint32_t len);
int32_t 	FS_write(SSV_FS fs, SSV_FILE file, void* buf, uint32_t len);
int32_t		FS_lseek(SSV_FS fs, SSV_FILE file, int32_t offs, uint32_t whence);
int32_t		FS_remove(SSV_FS fs, const char* path);
int32_t 	FS_fremove(SSV_FS fs, SSV_FILE file);
int32_t		FS_stat(SSV_FS fs, const char* path, SSV_FILE_STAT* stat);
int32_t		FS_fstat(SSV_FS fs, SSV_FILE file, SSV_FILE_STAT* stat);
int32_t		FS_flush(SSV_FS fs, SSV_FILE file);
int32_t		FS_close(SSV_FS fs, SSV_FILE file);
int32_t		FS_rename(SSV_FS fs, const char* old, const char* new);
SSV_DIR*	FS_opendir(SSV_FS fs, const char* name, SSV_DIR* d);
int32_t		FS_closedir(SSV_DIR* d);
SSV_DIRENT*	FS_readdir(SSV_DIR* d, SSV_DIRENT* e);

#endif

#ifndef _PLAY_MODE_H_
#define  _PLAY_MODE_H_

#include "ff.h"

extern FIL *Get_File_From_Number(WORD number);
extern FILE_INFO *get_file_info(void);
extern WORD get_musicfile_count(void);
extern BYTE media_init(BYTE disk_type);
extern BYTE get_fat_ok_flag(void);
extern void media_uninit(void);
#endif

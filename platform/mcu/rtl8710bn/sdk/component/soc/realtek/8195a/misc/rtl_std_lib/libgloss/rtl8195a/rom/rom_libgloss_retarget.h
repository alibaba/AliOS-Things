#ifndef ROM_LIBGLOSS_RETARGET_H
#define ROM_LIBGLOSS_RETARGET_H

#include <sys/stat.h>
#include <basic_types.h>

#define __rtl_close		__rtl_close_v1_00
#define __rtl_fstat		__rtl_fstat_v1_00
#define __rtl_isatty	__rtl_isatty_v1_00
#define __rtl_lseek		__rtl_lseek_v1_00
#define __rtl_open		__rtl_open_v1_00
#define __rtl_read		__rtl_read_v1_00
#define __rtl_write		__rtl_write_v1_00
#define __rtl_sbrk		__rtl_sbrk_v1_00

extern _LONG_CALL_ int __rtl_close_v1_00(int fildes);
extern _LONG_CALL_ int __rtl_fstat_v1_00(int fildes , struct stat *st);
extern _LONG_CALL_ int __rtl_isatty_v1_00(int file);
extern _LONG_CALL_ int __rtl_lseek_v1_00(int file , int ptr , int dir);
extern _LONG_CALL_ int __rtl_open_v1_00(char *file , int flags , int mode);
extern _LONG_CALL_ int __rtl_read_v1_00(int file , char *ptr , int len);
extern _LONG_CALL_ int __rtl_write_v1_00(int file , const char *ptr , int len);
extern _LONG_CALL_ void* __rtl_sbrk_v1_00(int incr);


struct _rom_libgloss_ram_map {
	int   (*libgloss_close)(int fildes);
	int   (*libgloss_fstat)(int fildes , struct stat *st);
	int   (*libgloss_isatty)(int file);
	int   (*libgloss_lseek)(int file , int ptr , int dir);
	int   (*libgloss_open)(char *file , int flags , int mode);
	int	  (*libgloss_read)(int file , char *ptr , int len);
	int	  (*libgloss_write)(int file , const char *ptr , int len);
	void* (*libgloss_sbrk)(int incr);
};

#endif	/* ROM_LIBGLOSS_RETARGET_H */

#ifndef _FS_
#define _FS_
//#include <typedef.h>
//#include <emod.h>
//#include <elibs.h>

#include "cdx_log.h"
#include "fs/fatfs/ff.h"

//#define O_RDONLY        00000000
//#define O_WRONLY        00000001
//#define O_RDWR          00000002
//#define O_CREAT         00000100  /* not fcntl */
#ifdef __cplusplus
extern "C"
{
#endif

#define FILE FIL

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

FILE * cedarx_fopen(const char * filename, const char * mode);
int cedarx_fseek(FILE * stream, long long offset, int whence);
long long cedarx_ftell(FILE * stream);
int cedarx_fclose(FILE * stream);
int cedarx_fwrite(const void * ptr, int size, int nmemb, FILE * stream);
int cedarx_fread(void * ptr, int size, int nmemb, FILE * stream);

#define fopen cedarx_fopen
#define fseek cedarx_fseek
#define ftell cedarx_ftell
#define fclose cedarx_fclose
#define fwrite cedarx_fwrite
#define fread cedarx_fread


#if 0
int fputc(int c, ES_FILE *fp);
#endif

#if FD_SUPPORT
int open(const char *name, int flag, int prems);
int  close(int fd);
int read(int fd, void *buf, unsigned  count);
int sscanf( const char *, const char *, ...);
signed long long  lseek(int fd, signed long long  offset, int whence);
#endif

#ifdef __cplusplus
}
#endif

#endif

/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.08b    (C)ChaN, 2011
/----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following trems.
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/

#ifndef _FATFS
#define _FATFS	8237	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif
#include "types.h"
#include "ffconf.h"		/* FatFs configuration options */

#if _FATFS != _FFCONF
#error Wrong configuration file (ffconf.h).
#endif


/* DBCS code ranges and SBCS extend char conversion table */

/* Definitions of volume management */

#if _MULTI_PARTITION		/* Multiple partition configuration */
#define LD2PD(vol) (VolToPart[vol].pd)	/* Get physical drive# */
#define LD2PT(vol) (VolToPart[vol].pt)	/* Get partition# */
typedef struct {
	uint_8 pd;	/* Physical drive# */
	uint_8 pt;	/* Partition # (0-3) */
} PARTITION;
extern const PARTITION VolToPart[];	/* Volume - Physical location resolution table */

#else						/* Single partition configuration */
#define LD2PD(vol) (vol)	/* Logical drive# is bound to the same physical drive# */
#define LD2PT(vol) 0		/* Always mounts the 1st partition */

#endif



/* Type of path name strings on FatFs API */

#if _LFN_UNICODE			/* Unicode string */
#if !_USE_LFN
#error _LFN_UNICODE must be 0 in non-LFN cfg.
#endif
#ifndef _INC_TCHAR
typedef WCHAR TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#endif

#else						/* ANSI/OEM string */
#ifndef _INC_TCHAR
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif

#endif



/* File system object structure (FATFS) */

typedef struct {
	uint_8	fs_type;		/* FAT sub-type (0:Not mounted) */
	uint_8	drv;			/* Physical drive number */
	uint_8	csize;			/* Sectors per cluster (1,2,4...128) */
	uint_8	n_fats;			/* Number of FAT copies (1,2) */
	uint_8	wflag;			/* win[] dirty flag (1:must be written back) */
	uint_8	fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
	uint_16	id;				/* File system mount ID */
	uint_16	n_rootdir;		/* Number of root directory entries (FAT12/16) */
#if _MAX_SS != 512
	uint_16	ssize;			/* Bytes per sector (512,1024,2048,4096) */
#endif
#if _FS_REENTRANT
	_SYNC_t	sobj;			/* Identifier of sync object */
#endif
#if !_FS_READONLY
	uint_32	last_clust;		/* Last allocated cluster */
	uint_32	free_clust;		/* Number of free clusters */
	uint_32	fsi_sector;		/* fsinfo sector (FAT32) */
#endif
#if _FS_RPATH
	uint_32	cdir;			/* Current directory start cluster (0:root) */
#endif
	uint_32	n_fatent;		/* Number of FAT entries (= number of clusters + 2) */
	uint_32	fsize;			/* Sectors per FAT */
	uint_32	fatbase;		/* FAT start sector */
	uint_32	dirbase;		/* Root directory start sector (FAT32:Cluster#) */
	uint_32	database;		/* Data start sector */
	uint_32	winsect;		/* Current sector appearing in the win[] */
	uint_8	win[_MAX_SS];	/* Disk access window for Directory, FAT (and Data on tiny cfg) */
} FATFS;



/* File object structure (FIL) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	uint_16	id;				/* Owner file system mount ID */
	uint_8	flag;			/* File status flags */
	uint_8	pad1;
	uint_32	fptr;			/* File read/write pointer (0 on file open) */
	uint_32	fsize;			/* File size */
	uint_32	sclust;			/* File start cluster (0 when fsize==0) */
	uint_32	clust;			/* Current cluster */
	uint_32	dsect;			/* Current data sector */
#if !_FS_READONLY
	uint_32	dir_sect;		/* Sector containing the directory entry */
	uint_8*	dir_ptr;		/* Ponter to the directory entry in the window */
#endif
#if _USE_FASTSEEK
	uint_32*	cltbl;			/* Pointer to the cluster link map table (null on file open) */
#endif
#if _FS_SHARE
	uint_32	lockid;			/* File lock ID (index of file semaphore table) */
#endif
#if !_FS_TINY
	uint_8	buf[_MAX_SS];	/* File data read/write buffer */
#endif
} FIL;



/* Directory object structure (DIR) */

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	uint_16	id;				/* Owner file system mount ID */
	uint_16	index;			/* Current read/write index number */
	uint_32	sclust;			/* Table start cluster (0:Root dir) */
	uint_32	clust;			/* Current cluster */
	uint_32	sect;			/* Current sector */
	uint_8*	dir;			/* Pointer to the current SFN entry in the win[] */
	uint_8*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _USE_LFN
	uint_8*	lfn;			/* Pointer to the LFN working buffer */
	uint_16	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */
#endif
} DIR;


/* Date structure (DATE)*/
typedef union{
  uint_16 Word;       
  struct{
    uint_16 day:5;    /* Day (1..31) */
    uint_16 month:4;  /* Month (1..12) */
    uint_16 year:7;   /* Year origin from 1980 (0..127) */ 
  }Bits;
} DATE;


/* Time structure (TIME) */
typedef union{
  uint_16 Word;
  struct{
    uint_16 second:5;  /* Second / 2 (0..29) */
    uint_16 minute:6;  /* Minute (0..59) */
    uint_16 hour:5;    /* Hour (0..23) */
  }Bits;
}TIME;

/* year original macros */
#define YEAR_ORIGIN        1980

/* File status structure (FILINFO) */
typedef struct {
	uint_32	fsize;			/* File size */
	DATE	fdate;	  		/* Last modified date */
	TIME	ftime;		  	/* Last modified time */
	uint_8	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
#if _USE_LFN
	TCHAR*	lfname;			/* Pointer to the LFN buffer */
	uint_32 	lfsize;		/* Size of LFN buffer in TCHAR */
#endif
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occured in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Acces denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Acces denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume on the physical drive */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file shareing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES	/* (18) Number of open files > _FS_SHARE */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_mount (uint_8, FATFS*);						/* Mount/Unmount a logical drive */
FRESULT f_open (FIL*, const TCHAR*, uint_8);			/* Open or create a file */
FRESULT f_read (FIL*, void*, uint_32, uint_32*);		/* Read data from a file */
FRESULT f_lseek (FIL*, uint_32);						/* Move file pointer of a file object */
FRESULT f_close (FIL*);									/* Close an open file object */
FRESULT f_opendir (DIR*, const TCHAR*);					/* Open an existing directory */
FRESULT f_readdir (DIR*, FILINFO*);						/* Read a directory item */
FRESULT f_stat (const TCHAR*, FILINFO*);				/* Get file status */

FRESULT f_write (FIL*, void*, uint_32, uint_32*);		/* Write data to a file */
FRESULT f_getfree (const TCHAR*, uint_32*, FATFS**);	/* Get number of free clusters on the drive */
FRESULT f_truncate (FIL*);								/* Truncate file */
FRESULT f_sync (FIL*);									/* Flush cached data of a writing file */
FRESULT f_unlink (const TCHAR*);						/* Delete an existing file or directory */
FRESULT	f_mkdir (const TCHAR*);							/* Create a new directory */
FRESULT f_chmod (const TCHAR*, uint_8, uint_8);			/* Change attriburte of the file/dir */
FRESULT f_utime (const TCHAR*, const FILINFO*);			/* Change timestamp of the file/dir */
FRESULT f_rename (const TCHAR*, const TCHAR*);			/* Rename/Move a file or directory */
FRESULT f_forward (FIL*, uint_32(*)(const uint_8*,uint_32), uint_32, uint_32*);	/* Forward data to the stream */

FRESULT f_mkfs (uint_8, uint_8, uint_32);					/* Create a file system on the drive */

FRESULT f_chdrive (uint_8);							/* Change current drive */
FRESULT f_chdir (const TCHAR*);						/* Change current directory */
FRESULT f_getcwd (TCHAR*, uint_32);					/* Get current directory */

int f_putc (TCHAR, FIL*);							/* Put a character to the file */
int f_puts (const TCHAR*, FIL*);					/* Put a string to the file */
int f_printf (FIL*, const TCHAR*, ...);				/* Put a formatted string to the file */
TCHAR* f_gets (TCHAR*, int, FIL*);					/* Get a string from the file */
#ifndef EOF
#define EOF (-1)
#endif

#define f_eof(fp) (((fp)->fptr == (fp)->fsize) ? 1 : 0)
#define f_error(fp) (((fp)->flag & FA__ERROR) ? 1 : 0)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->fsize)



/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !_FS_READONLY
extern uint_32 get_fattime (void);
#endif

/* Unicode support functions */
#if _USE_LFN							/* Unicode - OEM code conversion */
uint_16 ff_convert (uint_16, uint_32);	/* OEM-Unicode bidirectional conversion */
uint_16 ff_wtoupper (uint_16);			/* Unicode upper-case conversion */
#if _USE_LFN == 3						/* Memory functions */
void* ff_memalloc (uint_32);			/* Allocate memory block */
void ff_memfree (void*);				/* Free memory block */
#endif
#endif

/* Sync functions */
#if _FS_REENTRANT
int ff_cre_syncobj (uint8_t, _SYNC_t*);/* Create a sync object */
int ff_req_grant (_SYNC_t);			/* Lock sync object */
void ff_rel_grant (_SYNC_t);		/* Unlock sync object */
int ff_del_syncobj (_SYNC_t);		/* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01
#define	FA_OPEN_EXISTING	0x00
#define FA__ERROR			0x80

#if !_FS_READONLY
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40
#endif


/* FAT sub type (FATFS.fs_type) */

#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3


/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */


/* Fast seek function */
#define CREATE_LINKMAP	0xFFFFFFFF


/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1	/* Enable word access to the FAT structure */
#define	LD_WORD(ptr)		(uint_16)(*(uint_16*)(uint_8*)(ptr))
#define	LD_DWORD(ptr)		(uint_32)(*(uint_32*)(uint_8*)(ptr))
#define	ST_WORD(ptr,val)	*(uint_16*)(uint_8*)(ptr)=(uint_16)(val)
#define	ST_DWORD(ptr,val)	*(uint_32*)(uint_8*)(ptr)=(uint_32)(val)
#else					/* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(uint_16)(((uint_16)*(uint_8*)((ptr)+1)<<8)|(uint_16)*(uint_8*)(ptr))
#define	LD_DWORD(ptr)		(uint_32)(((uint_32)*(uint_8*)((ptr)+3)<<24)|((uint_32)*(uint_8*)((ptr)+2)<<16)|((uint_16)*(uint_8*)((ptr)+1)<<8)|*(uint_8*)(ptr))
#define	ST_WORD(ptr,val)	*(uint_8*)(ptr)=(uint_8)(val); *(uint_8*)((ptr)+1)=(uint_8)((uint_16)(val)>>8)
#define	ST_DWORD(ptr,val)	*(uint_8*)(ptr)=(uint_8)(val); *(uint_8*)((ptr)+1)=(uint_8)((uint_16)(val)>>8); *(uint_8*)((ptr)+2)=(uint_8)((uint_32)(val)>>16); *(uint_8*)((ptr)+3)=(uint_8)((uint_32)(val)>>24)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FATFS */

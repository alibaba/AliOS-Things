#ifndef _FF_HEADER_FILE_
#define _FF_HEADER_FILE_

#include "include.h"
#include "ffconf.h"

#if CFG_USE_SDCARD_HOST
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FATFS
#define _FATFS	7231	/* Revision ID */

    /* Definitions of volume management */
    /* Each logical drive is bound to the same physical drive number */
#define LD2PD(vol)      (uint8)(vol)
    /* Find first valid partition or in SFD */
#define LD2PT(vol)      0
    typedef unsigned short	WCHAR;

    /* Type of path name strings on FatFs API */
#if _LFN_UNICODE			/* Unicode string */
#if !_USE_LFN
#error _LFN_UNICODE must be 0 at non-LFN cfg.
#endif
#ifndef _INC_TCHAR
    typedef WCHAR TCHAR;
#define _T(x)       L ## x
#define _TEXT(x)    L ## x
#endif

#else						/* ANSI/OEM string */
#ifndef _INC_TCHAR
    typedef char TCHAR;
#define _T(x)       x
#define _TEXT(x)    x
#endif

#endif

#define SD_DATA_CRC_ERR     (-255)

typedef struct _fat_file_info_s
{
    char    filename[13];
    char    ext_name[3];
    uint32  file_start_cluster;
    uint32  file_blks;
} fat_file_info_t;

typedef struct _FATFS_
{
    uint8	fs_type;	/* FAT sub type */
    uint8	drive;		/* Physical drive number */
    uint8	csize;		/* Number of sectors per cluster */
    uint8	n_fats;		/* Number of FAT copies */
    uint8	wflag;		/* win[] dirty flag (1:must be written back) */
    uint8	fsi_flag;	/* fsinfo dirty flag (1:must be written back) */
    uint16	id;			/* File system mount ID */
    uint16	n_rootdir;	/* Number of root directory entries (0 on FAT32) */
    uint32	sects_fat;	/* Sectors per fat */
    uint32	max_clust;	/* Maximum cluster# + 1. Number of clusters is max_clust - 2 */
    uint32	fatbase;	/* FAT start sector */
    uint32	dirbase;	/* Root directory start sector (Cluster# on FAT32) */
    uint8	win[_MAX_SS];/* Disk access window for Directory/FAT */	//make sure win is allign(4)
    uint32	database;	/* Data start sector */
    uint32	winsect;	/* Current sector appearing in the win[] */
} FATFS;

/* File object structure */
typedef struct _FIL_
{
    FATFS	*fs;			/* Pointer to the owner file system object */
    uint16	id;			/* Owner file system mount ID */
    uint8	flag;		/* File status flags */
    uint8	csect;		/* Sector address in the cluster */
    uint32	fptr;		/* File R/W pointer */
    uint32	fsize;		/* File size */
    uint32	org_clust;	/* File start cluster */
    uint8	buf[_MAX_SS];/* File R/W buffer *///make sure buf is allign(4)
    uint32	curr_clust;	/* Current cluster */
    uint32	dsect;		/* Current data sector */
    uint32	ExNoChainFlag;
} FIL;


/* Directory object structure (DIR) */
typedef struct
{
    FATFS	*fs;			/* Pointer to the owner file system object (**do not change order**) */
    uint16	id;			/* Owner file system mount ID (**do not change order**) */
    uint16	index;		/* Current read/write index number */
    uint32	sclust;		/* Table start cluster (0:Root dir) */
    uint32	clust;		/* Current cluster */
    uint32	sect;		/* Current sector */
    uint8	*dir;		/* Pointer to the current SFN entry in the win[] */
    uint8	*fn;			/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
    uint32	ExNoChainFlag;  /* if the value equals to 0x03,then all cluster are contiguous*/
#if _USE_LFN
    WCHAR	*lfn;		/* Pointer to the LFN working buffer */
    uint16	lfn_idx;	/* Last matched LFN index number (0xFFFF:No LFN) */
#endif
} DIR;

/* File status structure (FILINFO) */
typedef struct
{
    uint32	fsize;		/* File size */
    uint16	fdate;		/* Last modified date */
    uint16	ftime;		/* Last modified time */
    uint8	fattrib;	/* Attribute */
    TCHAR	fname[13];	/* Short file name (8.3 format) */

    uint8	ExNoChainFlag;	//
    uint8	reserved;
    uint32	fcluster;
#if _USE_LFN
    TCHAR	*lfname;		/* Pointer to the LFN buffer */
    uint32 	lfsize;		/* Size of LFN buffer in TCHAR */
#endif
} FILINFO, FILINFOADD, *PFILINFOADD;;

typedef struct
{
    uint32	fcluster;	/* ÎÄ¼þµÄ´ØºÅ */
    char	fname[13];	/* Short file name (8.3 format) */
    char    extname[3];
    //	uint8	ExNoChainFlag;//exfat flag
    uint8   fat_ok_flag;
    uint16	Reserved1;
} FILE_INFO;

/*app struct*/
//add by wangxuejun
typedef struct
{
    uint32		first_cluster;
    uint16 		music_total;
    uint8		broot_dir;
    uint8		ExNoChainFlag;
} FAT_DIR_INFO;
//add by wangxuejun
typedef struct
{
    uint32		cluster_number;
    uint16 		dirlevel;
    uint8 		broot_dir;
    uint8		ExNoChainFlag;
} DIR_QUEUE;


/* File function return code (FRESULT) */
typedef enum
{
    FR_OK = 0,				/* (0) Succeeded */
    FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
    FR_INT_ERR,				/* (2) Assertion failed */
    FR_NOT_READY,			/* (3) The physical drive cannot work */
    FR_NO_FILE,				/* (4) Could not find the file */
    FR_NO_PATH,				/* (5) Could not find the path */
    FR_INVALID_NAME,		/* (6) The path name format is invalid */
    FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
    FR_EXIST,				/* (8) Access denied due to prohibited access */
    FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
    FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
    FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
    FR_NOT_ENABLED,			/* (12) The volume has no work area */
    FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
    FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
    FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
    FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
    FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
    FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
    FR_INVALID_PARAMETER,	/* (19) Given parameter is invalid */
    FR_FILE_END				/* (20) File end */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */
FRESULT dir_seek (
    DIR *dj,		/* Pointer to directory object */
    uint16 idx		/* Directory index number */
);

FRESULT move_window (
    FATFS *fs,		/* File system object */
    uint32 sector	/* Sector number to make apperance in the fs->win[] */
);					/* Move to zero only writes back dirty window */

FRESULT f_lseek (
    FIL *fp,		/* Pointer to the file object */
    uint32 ofs		/* File pointer from top of file */
);

/* Close an open file object */
FRESULT f_close (FIL *fp);
/* Read data from a file */
FRESULT f_read (FIL *fp, void *buff, uint32 btr, uint32 *br);
/* Read data from a file */
FRESULT f_wav_read(FIL *fp, void *buff, uint32 btr, uint32 *br);
/* Mount/Unmount a logical drive */
FRESULT f_mount (uint8 vol, FATFS *fs);
/* Unmount a logical drive */
FRESULT f_unmount (FATFS *fs);
/*decide the end of an file*/
FRESULT f_EOF(FIL *fp);

/* Get current directory */
FRESULT f_getcwd (TCHAR *buff, uint32 len);
/* Open or create a file */
FRESULT f_open (FIL *fp, const TCHAR *path, uint8 mode);
/* Open a directory */
FRESULT f_opendir (DIR *dp, const TCHAR *path);
/* Read a directory item */
FRESULT f_readdir (DIR *dp, FILINFO *fno);
/* Get file status */
FRESULT f_stat (const TCHAR *path, FILINFO *fno);
/* Write data to the file */
FRESULT f_write (FIL *fp, const void *buff, uint32 btw, uint32 *bw);
/* Change current directory */
FRESULT f_chdir (const TCHAR *path);
/* Create a sub directory */
FRESULT f_mkdir (const TCHAR *path);
/* Rename/Move a file or directory */
FRESULT f_rename (const TCHAR *path_old, const TCHAR *path_new);
/* Delete an existing file or directory */
FRESULT f_unlink (const TCHAR *path);
/* check whether get to the end of file */
FRESULT f_eof(FIL *fp );

//uint32 SD_Fs_Init(void);
uint32 Media_Fs_Init(uint8 type);
uint16 get_musicfile_count(void);
FILE_INFO *get_file_info(void);
FIL *Get_File_From_Number(uint16 number);

#ifndef EOF
#define EOF (-1)
#endif

    /*--------------------------------------------------------------*/
    /* Additional user defined functions                            */
    void mem_cpy (void *dst, const void *src, uint32 cnt);

    /* Unicode support functions */
#if _USE_LFN
    /* Unicode - OEM code conversion */
    /* OEM-Unicode bidirectional conversion */
    WCHAR ff_convert (WCHAR chr, uint32 dir);
    /* Unicode upper-case conversion */
    WCHAR ff_wtoupper (WCHAR chr);
#endif


    /* Definitions of sector size */
#if (_MAX_SS < _MIN_SS)
    || (_MAX_SS != 512 && _MAX_SS != 1024 && _MAX_SS != 2048 && _MAX_SS != 4096)
    || (_MIN_SS != 512 && _MIN_SS != 1024 && _MIN_SS != 2048 && _MIN_SS != 4096)
#error Wrong sector size configuration.
#endif
#if _MAX_SS == _MIN_SS
#define	SS(fs)	((uint32)_MAX_SS)	/* Fixed sector size */
#else
#define	SS(fs)	((fs)->ssize)	/* Variable sector size */
#endif

    /*--------------------------------------------------------------*/
    /* Flags and offset address                                     */
    /* File access control and file status flags (FIL.flag) */
#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_OPEN_EXISTING	0x00
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define	FA_OPEN_APPEND		0x30

    /* FAT sub type (FATFS.fs_type) */
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3
#define	FS_ExFAT	4

    /* File attribute bits for directory entry */
#define	AM_RDO	    0x01	/* Read only */
#define	AM_HID	    0x02	/* Hidden */
#define	AM_SYS	    0x04	/* System */
#define	AM_VOL	    0x08	/* Volume label */
#define AM_LFN	    0x0F	/* LFN entry */
#define AM_DIR	    0x10	/* Directory */
#define AM_ARC	    0x20	/* Archive */
#define AM_MASK	    0x3F	/* Mask of defined bits */

    /* Fast seek feature */
#define CREATE_LINKMAP	0xFFFFFFFF

    /* Name status flags */
#define NS			11		/* Index of name status byte in fn[] */
#define NS_LOSS		0x01	/* Out of 8.3 format */
#define NS_LFN		0x02	/* Force to create LFN entry */
#define NS_LAST		0x04	/* Last segment */
#define NS_BODY		0x08	/* Lower case flag (body) */
#define NS_EXT		0x10	/* Lower case flag (ext) */
#define NS_DOT		0x20	/* Dot entry */


    /*FAT32/16 */
#define BS_jmpBoot			0		/* Jump instruction (3) */
#define BS_OEMName			3		/* OEM name (8) */
#define BPB_BytsPerSec		11		/* Sector size [byte] (2) */
#define BPB_SecPerClus		13		/* Cluster size [sector] (1) */
#define BPB_RsvdSecCnt		14		/* Size of reserved area [sector] (2) */
#define BPB_NumFATs			16		/* Number of FAT copies (1) */
#define BPB_RootEntCnt		17		/* Number of root directory entries for FAT12/16 (2) */
#define BPB_TotSec16		19		/* Volume size [sector] (2) */
#define BPB_Media			21		/* Media descriptor (1) */
#define BPB_FATSz16			22		/* FAT size [sector] (2) */
#define BPB_SecPerTrk		24		/* Track size [sector] (2) */
#define BPB_NumHeads		26		/* Number of heads (2) */
#define BPB_HiddSec			28		/* Number of special hidden sectors (4) */
#define BPB_TotSec32		32		/* Volume size [sector] (4) */
#define BS_DrvNum			36		/* Physical drive number (2) */
#define BS_BootSig			38		/* Extended boot signature (1) */
#define BS_VolID			39		/* Volume serial number (4) */
#define BS_VolLab			43		/* Volume label (8) */
#define BS_FilSysType		54		/* File system type (1) */
#define BPB_FATSz32			36		/* FAT size [sector] (4) */
#define BPB_ExtFlags		40		/* Extended flags (2) */
#define BPB_FSVer			42		/* File system version (2) */
#define BPB_RootClus		44		/* Root directory first cluster (4) */
#define BPB_FSInfo			48		/* Offset of FSINFO sector (2) */
#define BPB_BkBootSec		50		/* Offset of backup boot sector (2) */
#define BS_DrvNum32			64		/* Physical drive number (2) */
#define BS_BootSig32		66		/* Extended boot signature (1) */
#define BS_VolID32			67		/* Volume serial number (4) */
#define BS_VolLab32			71		/* Volume label (8) */
#define BS_FilSysType32		82		/* File system type (1) */
#define	FSI_LeadSig			0		/* FSI: Leading signature (4) */
#define	FSI_StrucSig		484		/* FSI: Structure signature (4) */
#define	FSI_Free_Count		488		/* FSI: Number of free clusters (4) */
#define	FSI_Nxt_Free		492		/* FSI: Last allocated cluster (4) */
#define MBR_Table			446		/* MBR: Partition table offset (2) */
#define	SZ_PTE				16		/* MBR: Size of a partition table entry */
#define BS_55AA				510		/* Signature word (2) */

#define	DIR_Name			0		/* Short file name (11) */
#define	DIR_Attr			11		/* Attribute (1) */
#define	DIR_NTres			12		/* NT flag (1) */
#define DIR_CrtTimeTenth	13		/* Created time sub-second (1) */
#define	DIR_CrtTime			14		/* Created time (2) */
#define	DIR_CrtDate			16		/* Created date (2) */
#define DIR_LstAccDate		18		/* Last accessed date (2) */
#define	DIR_FstClusHI		20		/* Higher 16-bit of first cluster (2) */
#define	DIR_WrtTime			22		/* Modified time (2) */
#define	DIR_WrtDate			24		/* Modified date (2) */
#define	DIR_FstClusLO		26		/* Lower 16-bit of first cluster (2) */
#define	DIR_FileSize		28		/* File size (4) */
#define	LDIR_Ord			0		/* LFN entry order and LLE flag (1) */
#define	LDIR_Attr			11		/* LFN attribute (1) */
#define	LDIR_Type			12		/* LFN type (1) */
#define	LDIR_Chksum			13		/* Sum of corresponding SFN entry */
#define	LDIR_FstClusLO		26		/* Filled by zero (0) */
#define	SZ_DIR				32		/* Size of a directory entry */
#define	LLE					0x40	/* Last long entry flag in LDIR_Ord */
#define	DDE					0xE5	/* Deleted directory entry mark in DIR_Name[0] */
#define	NDDE				0x05	/* Replacement of the character collides with DDE */


    /*Exfat*/
#define	BPB_ExFatName		0x04	/*xfat string*/
#define	BPB_ExFatOffset		0x50	/*fat offset*/
#define	BPB_ExFatLength		0x54	/*fat length*/
#define	BPB_ExClusHPOffset  0x58	/*cluster Heap Offset*/
#define	BPB_ExClusterCnt	0x5c	/*cluster count*/
#define	BPB_ExRootDirBase	0x60	/*root dir cluster*/
#define	BPB_ExBytePerSecSft 0x6c  	/*bytes per sector shift */
#define	BPB_ExSecPerClusSft	0x6d 	/*sectors per cluster shift */
#define	BPB_ExNumofFATs		0x6e	/*number of fat*/

#define	FILE_DIR_ENTRY      0x85
#define	STREAM_EXT_DIR_ENT  0xc0
#define	FILE_NAME_DIR_ENTRY 0xc1

#define	ExFat_Dir_Attr		0x04   // byte

#define	ExFat_NoChainFlag	0x01
#define	ExFat_DataLength	0x08
#define	ExFat_FirstCluster	0x14
#define	ExFat_NameLength	0x03   // 1byte, unicode uint

    /*--------------------------------*/
    /* Multi-byte word access macros  */
    /* Use byte-by-byte access to the FAT structure */
#define	LD_WORD(ptr)		(uint16)  \
    (((uint16)*((uint8*)(ptr)+1)<<8)  \
    |(uint16)*(uint8*)(ptr))

#define	LD_DWORD(ptr)		(uint32)  \
    (((uint32)*((uint8*)(ptr)+3)<<24) \
    |((uint32)*((uint8*)(ptr)+2)<<16) \
    |((uint16)*((uint8*)(ptr)+1)<<8)  \
    |*(uint8*)(ptr))

#define	ST_WORD(ptr,val)	        \
    *(uint8*)(ptr)=(uint8)(val);    \
    *((uint8*)(ptr)+1)=(uint8)((uint16)(val)>>8)

#define	ST_DWORD(ptr,val)	        \
    *(uint8*)(ptr)=(uint8)(val);    \
    *((uint8*)(ptr)+1)=(uint8)((uint16)(val)>>8);  \
    *((uint8*)(ptr)+2)=(uint8)((uint32)(val)>>16); \
    *((uint8*)(ptr)+3)=(uint8)((uint32)(val)>>24)

#endif

#ifdef __cplusplus
}
#endif

#endif /* _FATFS */


#if CFG_USE_USB_HOST
/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT file system module  R0.12c                             /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2017, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/----------------------------------------------------------------------------*/


#ifndef _FATFS
#define _FATFS	68300	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "integer.h"	/* Basic integer types */
    //#include "types.h"
#include "ffconf.h"		/* FatFs configuration options */
    //#include"syscall.h"
    //#include"ccsbcs.h"

#if _FATFS != _FFCONF
#error Wrong configuration file (ffconf.h).
#endif



    /* Definitions of volume management */

#if _MULTI_PARTITION		/* Multiple partition configuration */
    typedef struct
    {
        BYTE pd;	/* Physical drive number */
        BYTE pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
    } PARTITION;
    extern PARTITION VolToPart[];	/* Volume - Partition resolution table */
#endif



    /* Type of path name strings on FatFs API */

#if _LFN_UNICODE			/* Unicode (UTF-16) string */
#if _USE_LFN == 0
#error _LFN_UNICODE must be 0 at non-LFN cfg.
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



    /* Type of file size variables */

#if _FS_EXFAT
#if _USE_LFN == 0
#error LFN must be enabled when enable exFAT
#endif
    typedef QWORD FSIZE_t;
#else
    typedef DWORD FSIZE_t;
#endif

    typedef struct _fat_file_info_s
    {
        char filename[13];
        char  ext_name[3];
        uint32   file_start_cluster;
        uint32  file_blks;
    } fat_file_info_t;



    typedef struct
    {
        uint32	fcluster;
        char	fname[13];	/* Short file name (8.3 format) */
        char    extname[3];
        //	uint8	ExNoChainFlag;//exfat flag
        uint8   fat_ok_flag;
        uint16	Reserved1;
    } FILE_INFO;

    //*app struct*/
    //add by wangxuejun

    typedef struct
    {
        uint32		first_cluster;
        uint16 		music_total;
        uint8		broot_dir;
        uint8		ExNoChainFlag;
    } FAT_DIR_INFO;


    //add by wangxuejun
    typedef struct
    {
        uint32		cluster_number;
        uint16 		dirlevel;
        uint8 		broot_dir;
        uint8		ExNoChainFlag;
    } DIR_QUEUE;
    //add by wangxuejun






    /* File system object structure (FATFS) */

    typedef struct
    {
        BYTE	fs_type;		/* File system type (0:N/A) */
        BYTE	drv;			/* Physical drive number */
        BYTE	n_fats;			/* Number of FATs (1 or 2) */
        BYTE	wflag;			/* win[] flag (b0:dirty) */
        BYTE	fsi_flag;		/* FSINFO flags (b7:disabled, b0:dirty) */
        WORD	id;				/* File system mount ID */
        WORD	n_rootdir;		/* Number of root directory entries (FAT12/16) */
        WORD	csize;			/* Cluster size [sectors] */
#if _MAX_SS != _MIN_SS
        WORD	ssize;			/* Sector size (512, 1024, 2048 or 4096) */
#endif
#if _USE_LFN != 0
        WCHAR	*lfnbuf;			/* LFN working buffer */
#endif
#if _FS_EXFAT
        BYTE	*dirbuf;			/* Directory entry block scratchpad buffer */
#endif
#if _FS_REENTRANT
        _SYNC_t	sobj;			/* Identifier of sync object */
#endif
#if !_FS_READONLY
        DWORD	last_clst;		/* Last allocated cluster */
        DWORD	free_clst;		/* Number of free clusters */
#endif
#if _FS_RPATH != 0
        DWORD	cdir;			/* Current directory start cluster (0:root) */
#if _FS_EXFAT
        DWORD	cdc_scl;		/* Containing directory start cluster (invalid when cdir is 0) */
        DWORD	cdc_size;		/* b31-b8:Size of containing directory, b7-b0: Chain status */
        DWORD	cdc_ofs;		/* Offset in the containing directory (invalid when cdir is 0) */
#endif
#endif
        DWORD	n_fatent;		/* Number of FAT entries (number of clusters + 2) */
        DWORD	fsize;			/* Size of an FAT [sectors] */
        DWORD	volbase;		/* Volume base sector */
        DWORD	fatbase;		/* FAT base sector */
        DWORD	dirbase;		/* Root directory base sector/cluster */
        DWORD	database;		/* Data base sector */
        DWORD	winsect;		/* Current sector appearing in the win[] */
        BYTE	win[_MAX_SS];	/* Disk access window for Directory, FAT (and file data at tiny cfg) */
    } FATFS;



    /* Object ID and allocation information (_FDID) */

    typedef struct
    {
        FATFS	*fs;			/* Pointer to the owner file system object */
        WORD	id;			/* Owner file system mount ID */
        BYTE	attr;		/* Object attribute */
        BYTE	stat;		/* Object chain status (b1-0: =0:not contiguous, =2:contiguous (no data on FAT), =3:flagmented in this session, b2:sub-directory stretched) */
        DWORD	sclust;		/* Object start cluster (0:no cluster or root directory) */
        FSIZE_t	objsize;	/* Object size (valid when sclust != 0) */
#if _FS_EXFAT
        DWORD	n_cont;		/* Size of first fragment, clusters - 1 (valid when stat == 3) */
        DWORD	n_frag;		/* Size of last fragment needs to be written (valid when not zero) */
        DWORD	c_scl;		/* Containing directory start cluster (valid when sclust != 0) */
        DWORD	c_size;		/* b31-b8:Size of containing directory, b7-b0: Chain status (valid when c_scl != 0) */
        DWORD	c_ofs;		/* Offset in the containing directory (valid when sclust != 0 and non-directory object) */
#endif
#if _FS_LOCK != 0
        UINT	lockid;		/* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
    } _FDID;



    /* File object structure (FIL) */

    typedef struct
    {
        _FDID	obj;			/* Object identifier (must be the 1st member to detect invalid object pointer) */
        BYTE	flag;			/* File status flags */
        BYTE	err;			/* Abort flag (error code) */
        FSIZE_t	fptr;			/* File read/write pointer (Zeroed on file open) */
        DWORD	clust;			/* Current cluster of fpter (invalid when fptr is 0) */
        DWORD	sect;			/* Sector number appearing in buf[] (0:invalid) */
#if !_FS_READONLY
        DWORD	dir_sect;		/* Sector number containing the directory entry */
        BYTE	*dir_ptr;		/* Pointer to the directory entry in the win[] */
#endif
#if _USE_FASTSEEK
        DWORD	*cltbl;			/* Pointer to the cluster link map table (nulled on open, set by application) */
#endif
#if !_FS_TINY
        BYTE	buf[_MAX_SS];	/* File private data read/write window */
#endif
    } FIL;



    /* Directory object structure (DIR) */

    typedef struct
    {
        _FDID	obj;			/* Object identifier */
        DWORD	dptr;			/* Current read/write offset */
        DWORD	clust;			/* Current cluster */
        DWORD	sect;			/* Current sector (0:Read operation has terminated) */
        BYTE	*dir;			/* Pointer to the directory item in the win[] */
        BYTE	fn[12];			/* SFN (in/out) {body[8],ext[3],status[1]} */
#if _USE_LFN != 0
        DWORD	blk_ofs;		/* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
#endif
#if _USE_FIND
        const TCHAR *pat;		/* Pointer to the name matching pattern */
#endif
    } DIR;



    /* File information structure (FILINFO) */

    typedef struct
    {
        FSIZE_t	fsize;			/* File size */
        WORD	fdate;			/* Modified date */
        WORD	ftime;			/* Modified time */
        BYTE	fattrib;		/* File attribute */
#if _USE_LFN != 0
        TCHAR	altname[13];			/* Altenative file name */
        TCHAR	fname[_MAX_LFN + 1];	/* Primary file name */
#else
        TCHAR	fname[13];		/* File name */
#endif
    } FILINFO;


    /* File status structure (FILINFO) */
    typedef struct
    {
        uint32	fsize;			/* File size */
        uint16	fdate;			/* Last modified date */
        uint16	ftime;			/* Last modified time */
        uint8	fattrib;		/* Attribute */
        TCHAR	fname[13];		/* Short file name (8.3 format) */

        uint8	ExNoChainFlag;	//
        uint8	reserved;
        uint32	fcluster;
#if _USE_LFN
        TCHAR	*lfname;			/* Pointer to the LFN buffer */
        uint32 	lfsize;			/* Size of LFN buffer in TCHAR */
#endif
    } FILINFOADD, *PFILINFOADD;



    /* File function return code (FRESULT) */

    typedef enum
    {
        FR_OK = 0,				/* (0) Succeeded */
        FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
        FR_INT_ERR,				/* (2) Assertion failed */
        FR_NOT_READY,			/* (3) The physical drive cannot work */
        FR_NO_FILE,				/* (4) Could not find the file */
        FR_NO_PATH,				/* (5) Could not find the path */
        FR_INVALID_NAME,		/* (6) The path name format is invalid */
        FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
        FR_EXIST,				/* (8) Access denied due to prohibited access */
        FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
        FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
        FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
        FR_NOT_ENABLED,			/* (12) The volume has no work area */
        FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
        FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any problem */
        FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
        FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
        FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
        FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_LOCK */
        FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
    } FRESULT;



    /*--------------------------------------------------------------*/
    /* FatFs module application interface                           */

    FRESULT f_open (FIL *fp, const TCHAR *path, BYTE mode);				/* Open or create a file */
    FRESULT f_close (FIL *fp);											/* Close an open file object */
    FRESULT f_read (FIL *fp, void *buff, UINT btr, UINT *br);			/* Read data from the file */
    FRESULT f_write (FIL *fp, const void *buff, UINT btw, UINT *bw);	/* Write data to the file */
    FRESULT f_lseek (FIL *fp, FSIZE_t ofs);								/* Move file pointer of the file object */
    FRESULT f_truncate (FIL *fp);										/* Truncate the file */
    FRESULT f_sync (FIL *fp);											/* Flush cached data of the writing file */
    FRESULT f_opendir (DIR *dp, const TCHAR *path);						/* Open a directory */
    FRESULT f_closedir (DIR *dp);										/* Close an open directory */
    FRESULT f_readdir (DIR *dp, FILINFO *fno);							/* Read a directory item */
    FRESULT f_findfirst (DIR *dp, FILINFO *fno, const TCHAR *path, const TCHAR *pattern);	/* Find first file */
    FRESULT f_findnext (DIR *dp, FILINFO *fno);							/* Find next file */
    FRESULT f_mkdir (const TCHAR *path);								/* Create a sub directory */
    FRESULT f_unlink (const TCHAR *path);								/* Delete an existing file or directory */
    FRESULT f_rename (const TCHAR *path_old, const TCHAR *path_new);	/* Rename/Move a file or directory */
    FRESULT f_stat (const TCHAR *path, FILINFO *fno);					/* Get file status */
    FRESULT f_chmod (const TCHAR *path, BYTE attr, BYTE mask);			/* Change attribute of a file/dir */
    FRESULT f_utime (const TCHAR *path, const FILINFO *fno);			/* Change timestamp of a file/dir */
    FRESULT f_chdir (const TCHAR *path);								/* Change current directory */
    FRESULT f_chdrive (const TCHAR *path);								/* Change current drive */
    FRESULT f_getcwd (TCHAR *buff, UINT len);							/* Get current directory */
    FRESULT f_getfree (const TCHAR *path, DWORD *nclst, FATFS **fatfs);	/* Get number of free clusters on the drive */
    FRESULT f_getlabel (const TCHAR *path, TCHAR *label, DWORD *vsn);	/* Get volume label */
    FRESULT f_setlabel (const TCHAR *label);							/* Set volume label */
    FRESULT f_forward (FIL *fp, UINT(*func)(const BYTE *, UINT), UINT btf, UINT *bf);	/* Forward data to the stream */
    FRESULT f_expand (FIL *fp, FSIZE_t szf, BYTE opt);					/* Allocate a contiguous block to the file */
    FRESULT f_mount (FATFS *fs, const TCHAR *path, BYTE opt);			/* Mount/Unmount a logical drive */
    FRESULT f_mkfs (const TCHAR *path, BYTE opt, DWORD au, void *work, UINT len);	/* Create a FAT volume */
    FRESULT f_fdisk (BYTE pdrv, const DWORD *szt, void *work);			/* Divide a physical drive into some partitions */
    int f_putc (TCHAR c, FIL *fp);										/* Put a character to the file */
    int f_puts (const TCHAR *str, FIL *cp);								/* Put a string to the file */
    int f_printf (FIL *fp, const TCHAR *str, ...);						/* Put a formatted string to the file */
    TCHAR *f_gets (TCHAR *buff, int len, FIL *fp);						/* Get a string from the file */





#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->obj.objsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#define f_rmdir(path) f_unlink(path)

#ifndef EOF
#define EOF (-1)
#endif




    /*--------------------------------------------------------------*/
    /* Additional user defined functions                            */

    /* RTC function */
#if !_FS_READONLY && !_FS_NORTC
    DWORD get_fattime (void);
#endif

    /* Unicode support functions */
#if _USE_LFN != 0						/* Unicode - OEM code conversion */
    //WCHAR ff_convert (WCHAR chr, UINT dir);	/* OEM-Unicode bidirectional conversion */
    //WCHAR ff_wtoupper (WCHAR chr);			/* Unicode upper-case conversion */
#if _CODE_PAGE < 900
#include"ccsbcs.h"
#elif  (_CODE_PAGE == 936)
#include"cc936.h"
#endif
    //#include"ccsbcs.c"
#if _USE_LFN == 3						/* Memory functions */
    //void* ff_memalloc (UINT msize);			/* Allocate memory block */
    //void ff_memfree (void* mblock);			/* Free memory block */
#include"syscall.h"
#endif

#endif




    /* Sync functions */
#if _FS_REENTRANT
    int ff_cre_syncobj (BYTE vol, _SYNC_t *sobj);	/* Create a sync object */
    int ff_req_grant (_SYNC_t sobj);				/* Lock sync object */
    void ff_rel_grant (_SYNC_t sobj);				/* Unlock sync object */
    int ff_del_syncobj (_SYNC_t sobj);				/* Delete a sync object */
#endif




    /*--------------------------------------------------------------*/
    /* Flags and offset address                                     */


    /* File access mode and open method flags (3rd argument of f_open) */
#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_OPEN_EXISTING	0x00
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define	FA_OPEN_APPEND		0x30

    /* Fast seek controls (2nd argument of f_lseek) */
#define CREATE_LINKMAP	((FSIZE_t)0 - 1)

    /* Format options (2nd argument of f_mkfs) */
#define FM_FAT		0x01
#define FM_FAT32	0x02
#define FM_EXFAT	0x04
#define FM_ANY		0x07
#define FM_SFD		0x08

    /* Filesystem type (FATFS.fs_type) */
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3
#define FS_EXFAT	4

    /* File attribute bits for directory entry (FILINFO.fattrib) */
#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */


#if 1

    // void mem_set (void* dst, int val, UINT cnt);

    //void mem_cpy (void* dst, const void* src, UINT cnt);

    /* Move to zero only writes back dirty window */


    FRESULT f_lseek (
        FIL *fp,		/* Pointer to the file object */
        FSIZE_t ofs		/* File pointer from top of file */
    );
    //*/

    FRESULT move_window (
        FATFS *fs,		/* File system object */
        DWORD sector	/* Sector number to make apperance in the fs->win[] */
    );					/* Move to zero only writes back dirty window */

    FRESULT f_EOF(FIL *fp);  /*decide the end of an file*/

    FRESULT dir_seek (
        DIR *dj,		/* Pointer to directory object */
        uint16 idx		/* Directory index number */
    );

    FRESULT chk_mounted_con (FATFS *rfs, uint8 type);

#endif


#ifdef __cplusplus
}
#endif

#endif /* _FATFS */

#endif
#endif // _FF_HEADER_FILE_
// eof
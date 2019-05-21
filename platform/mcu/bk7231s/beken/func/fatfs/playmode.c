#include "diskio.h"		/* Declarations of low level disk I/O functions */
#if CFG_USE_SDCARD_HOST
#include "ff.h"

#define MAX_DIR_SUPPORT     1024
#define	MAX_SONG_DIR	    64       // 128
#define	MAX_DIR_LEVEL	    6

FILE_INFO FileInfo;
char *MP3_Ext = ".MP3";
char *WAV_Ext = ".WAV";
FAT_DIR_INFO file_queue[MAX_SONG_DIR];
uint16 mp3queuecount = 0; //有MP3文件的文件夹数目
uint16 mp3filecount = 0;//	MP3歌曲总的数目
FATFS Fatfs_buf;
FIL fsFD;
DIR_QUEUE *point_front;//队头指针
DIR_QUEUE *point_rear;//队尾指针
static uint16 rear_cnt = 0;

DIR_QUEUE g_dirbuf[MAX_DIR_SUPPORT];

extern void mem_set (void *dst, int val, uint32 cnt);
extern FRESULT chk_mounted_con (FATFS *rfs, uint8 chk_wp);

int strcmpi (const void *dst, const void *src, int cnt)
{
    const char *d = (const char *)dst, *s = (const char *)src;
    int r = 0;
    while (cnt-- && (r = *d++ - *s++) == 0) ;
    return r;
}

char *strrchr_con(char *str, char ch)
{
    char *p = (char *)str;
    while (*str) str++;
    while ((str-- != p) && (*str != ch));
    if (*str == ch)
        return( (char *)str );
    return NULL;
}
static void Convert2UpperCase(char *str, uint32 len)
{
    uint32 i;
    for(i = 0; i < len; i++)
        ((str[i] <= 'z') && (str[i] >= 'a')) ? (str[i] -= 0x20) : str[i];
}


FRESULT f_open_con (
    FIL *fp,			/* Pointer to the blank file object */
    PFILINFOADD finfo,
    uint8 mode			/* Access mode and file open mode flags */
)
{
    fp->flag = mode;				/* File access mode */
    fp->org_clust = finfo->fcluster;/* File start cluster */
    fp->fsize = finfo->fsize;	/* File size */
    fp->fptr = 0;
    fp->csect = 255;		/* File pointer */
    fp->dsect = 0;
    fp->fs = &Fatfs_buf;
    fp->id = Fatfs_buf.id;	/* Owner file system object of the file */
    fp->ExNoChainFlag = finfo->ExNoChainFlag;
    return FR_OK;
}

////
FRESULT fl_listatdir(DIR *dj_con, PFILINFOADD fi_con, uint16 idx)
{
    int i;
    uint8 c, *dir;
    char *p;
    FRESULT ret = FR_NO_FILE;
    /*for exfat use*/
    static	uint8	DirEntryAttr = 0;
    static	uint8	no_fat_chain_Flag = 0;
    static  uint8	ExtNameStartFlag = 0;
    static	uint8	exfat_name_length = 0;
    static	uint8   extname_idx = 0;
    static	char   	ext_name[3];
    static	uint32	exfat_first_cluster = 0;
    static	uint32	exfat_file_size = 0;
    static	uint16  exfat_entry_phase = 0;
    uint32	tmp = 0;

    dj_con->dir = dj_con->fs->win + (idx % (SS(dj_con->fs) / 32)) * 32;
    dir = dj_con->dir;

    if(dj_con->fs->fs_type == FS_ExFAT)
    {
        switch (dir[0])
        {
        case FILE_DIR_ENTRY:
            if(exfat_entry_phase == 0)
            {
                DirEntryAttr = dir[ExFat_Dir_Attr] & 0x3f; //get file attribute
                exfat_entry_phase++;
            }
            ret = FR_NO_FILE;
            break;

        case STREAM_EXT_DIR_ENT:
            if(exfat_entry_phase == 1)
            {
                exfat_first_cluster = LD_DWORD(dir + ExFat_FirstCluster);
                exfat_file_size = LD_DWORD(dir + ExFat_DataLength); //Byte unit
                exfat_name_length = dir[ExFat_NameLength];
                no_fat_chain_Flag = dir[ExFat_NoChainFlag];
                extname_idx = 0;
                ExtNameStartFlag = 0;
                exfat_entry_phase++;
            }
            ret = FR_NO_FILE;
            break;

        case FILE_NAME_DIR_ENTRY:
            if(exfat_entry_phase == 2 )
            {
                if(exfat_name_length < 19)
                {
                    tmp = (exfat_name_length > 15) ? 15 : exfat_name_length;
                    for(i = 0; i < tmp; i++)
                    {
                        if(ExtNameStartFlag)
                        {
                            ext_name[extname_idx] = (char)dir[(i + 1) << 1];
                            extname_idx++;

                        }
                        else
                        {

                            if((dir[(i + 1) << 1] == '.') && (dir[((i + 1) << 1) + 1] == 0))
                                ExtNameStartFlag = 1;
                        }
                    }
                    exfat_name_length -= tmp;
                }
                else
                    exfat_name_length -= 15;

                if(exfat_name_length == 0)
                {
                    /*all file name directory entries have been handled*/
                    mem_set(fi_con->fname, ' ', 13);
                    Convert2UpperCase(ext_name, 3);
                    mem_cpy(&fi_con->fname[9], ext_name, 3);
                    fi_con->fname[0] = '*';
                    fi_con->fname[8] = '.';//for our usage , ignore the name
                    fi_con->fname[12] = '\0';

                    fi_con->fattrib = DirEntryAttr; 	/* Attribute */
                    fi_con->fsize = exfat_file_size;	/* Size */
                    fi_con->fcluster = exfat_first_cluster;/*file start cluster*/
                    fi_con->ExNoChainFlag = no_fat_chain_Flag;
                    exfat_entry_phase = 0;
                    ret = FR_OK;
                }
                else
                    ret = FR_NO_FILE;
            }
            break;
        case 0:
            ret = FR_EXIST;
            break;
        default:
            exfat_entry_phase = 0;
            ret = FR_NO_FILE;
            break;
        }
    }
    else
    {
        p = fi_con->fname;

        if ((dir[0] == 0xE5) || (dir[0] == 0x05) || (dir[0] == 0x20) || (dir[0] == '.')
                || (dir[DIR_Attr] == 0x0f))
            return FR_NO_FILE;

        if (dir[0] == 0x00)
            return FR_EXIST;

        for (i = 0; i < 8; i++)
        {
            /* Copy name body */
            c = dir[i];
            *p++ = c;
        }
        fi_con->fname[8] = ' ';
        fi_con->fname[9] = ' ';
        fi_con->fname[10] = ' ';
        fi_con->fname[11] = ' ';
        if (dir[8] != ' ')
        {
            /* Copy name extension */
            *p++ = '.';
            for (i = 8; i < 11; i++)
            {
                c = dir[i];
                if (c == ' ')
                    break;
                *p++ = c;
            }
        }
        fi_con->fname[12] = '\0';
        fi_con->fattrib = dir[DIR_Attr];				/* Attribute */
        fi_con->fsize = LD_DWORD(dir + DIR_FileSize);		/* Size */
        fi_con->fdate = LD_WORD(dir + DIR_WrtDate);		/* Date */
        fi_con->ftime = LD_WORD(dir + DIR_WrtTime);		/* Time */
        fi_con->fcluster = ((uint32)LD_WORD(dir + DIR_FstClusHI) << 16) | LD_WORD(dir + DIR_FstClusLO);
        ret = FR_OK;
    }
    return ret;
}

static FRESULT  get_curdir_info(DIR_QUEUE *p_front)
{
    uint16 idx = 0;
    uint16 cur_musicfile = 0;
    uint16 ic, dc;
    FRESULT ret;
    DIR dj;
    FILINFOADD fl_curnode;
    char *Ext = NULL;
    char *Ext1 = NULL;

    dj.fs = &Fatfs_buf;
    dj.ExNoChainFlag = p_front->ExNoChainFlag;

    dc = SS(dj.fs) / 32;//entry count per sector
    ic = SS(dj.fs) / 32 * dj.fs->csize;//entry count per cluster
    if(p_front->broot_dir)
        dj.sclust = 0;//for Fat16
    else
        dj.sclust = p_front ->cluster_number;

    os_printf("get_curdir_info\r\n");
    while(1)
    {
        if((idx % ic) == 0) //new cluster
        {
            ret = dir_seek(&dj, idx);
            if (ret != FR_OK)
                break;
        }

        if((idx % dc) == 0) //new sector
        {
            if((idx % ic) != 0) //not cluster boundary
                dj.sect++;
            ret = move_window(dj.fs, dj.sect);
            if (ret != FR_OK)
                break;

        }

        ret = fl_listatdir(&dj, &fl_curnode, idx);
        idx++;

        if(ret == FR_OK)
        {
            if((fl_curnode.fattrib & 0x10) != 0)
            {
                point_rear->cluster_number = fl_curnode.fcluster;
                point_rear->dirlevel = p_front->dirlevel + 1;
                point_rear->broot_dir = 0;
                point_rear->ExNoChainFlag = fl_curnode.ExNoChainFlag;
                rear_cnt++;
                if(rear_cnt > MAX_DIR_SUPPORT - 1)
                    break;
                point_rear++;

            }
            if((fl_curnode.fattrib & 0x20) != 0) /*对只读、隐藏、系统等文件的属性支持*/
            {
                Ext = fl_curnode.fname;
                os_printf("get_curdir_info: fl_curnode.fname = \"%s\"\r\n", Ext);
                Ext1 = strrchr_con(Ext, '.');
                if (!strcmpi(Ext1, MP3_Ext, 4))
                    cur_musicfile++;
                else
                {
                    if(!strcmpi(Ext1, WAV_Ext, 4))
                        cur_musicfile++;
                }

            }
        }

        if(ret == FR_NO_FILE)
            continue;

        if(ret == FR_EXIST)
            break;
    }//while(1)
    if (cur_musicfile)
    {
        file_queue[mp3queuecount].first_cluster = p_front->cluster_number;
        file_queue[mp3queuecount].music_total = cur_musicfile;
        file_queue[mp3queuecount].broot_dir = p_front->broot_dir;
        file_queue[mp3queuecount].ExNoChainFlag = p_front->ExNoChainFlag;
        mp3queuecount++;
        mp3filecount += cur_musicfile;
    }
    return FR_OK;
}

/*input:
i -- directory index
number -- music file idx in  directory i */
static FIL	*GetFile_From_NumInDir(uint16 i, uint16 number)
{
    uint16 idx = 0;
    uint16 ic, dc, brootdir, music_count;
    FRESULT ret;
    FILINFOADD fl_curnode;
    char *Ext = NULL;
    char *Ext1 = NULL;
    DIR dj_con;

    FIL *fio = &fsFD;

    dj_con.fs = &Fatfs_buf;
    brootdir = file_queue[i].broot_dir;
    if(brootdir)
        dj_con.sclust = 0;
    else
        dj_con.sclust = file_queue[i].first_cluster;

    dc = SS(dj_con.fs) / 32;
    ic = SS(dj_con.fs) / 32 * dj_con.fs->csize;
    music_count = 0;
    number += 1;

    while(1)
    {
        if((idx % ic) == 0)
        {
            ret = dir_seek(&dj_con, idx);
            if (ret != FR_OK)
                return NULL;
        }
        if((idx % dc) == 0)
        {
            if((idx % ic) != 0)
                dj_con.sect++;
            ret = move_window(dj_con.fs, dj_con.sect);
            if (ret != FR_OK)
                return NULL;
        }

        ret = fl_listatdir(&dj_con, &fl_curnode, idx);
        idx++;

        if(ret == FR_OK)
        {
            if((fl_curnode.fattrib & 0x20) != 0) /*对只读、隐藏、系统等文件的属性支持*/
            {
                Ext = fl_curnode.fname;
                Ext1 = strrchr_con(Ext, '.');
                if (!strcmpi(Ext1, MP3_Ext, 4))
                    music_count++;
                else
                {
                    if (!strcmpi(Ext1, WAV_Ext, 4))
                        music_count++;
                }
            }
        }

        if (music_count == number)
            break;

        if(ret == FR_NO_FILE)
            continue;

        if(ret == FR_EXIST)
            return NULL;
    }
    FileInfo.fcluster = fl_curnode.fcluster;
    mem_cpy(FileInfo.fname, fl_curnode.fname, 12);
    mem_cpy(FileInfo.extname, FileInfo.fname + 9, 3);
    f_open_con(fio, &fl_curnode, FA_READ);

    return fio;
}


/*input:number -- 0~mp3filecount-1 */
//Playlist_GetSongFileInfo
FIL *Get_File_From_Number(uint16 number)
{
    uint16 i;
    FIL *fhFile;

    for(i = 0; i < MAX_SONG_DIR; i++)
    {
        if(number >= file_queue[i].music_total)
        {
            number = number - file_queue[i].music_total;
            continue;
        }
        else
            break;
    }
    if(i == MAX_SONG_DIR)
        return NULL;
    fhFile = GetFile_From_NumInDir(i, number);
    return fhFile;
}

/*scan whole disk, get total music file number and store the DIRs info
which have music files
*/
static uint32 initfatsystem(uint8 type)
{
    int count = 0, ret;
    DIR_QUEUE *dir_buf;
    rear_cnt = 0;
    if((ret = chk_mounted_con(&Fatfs_buf, type)) != FR_OK)
    {
        FAT_PRT("Fat Init Err:%d!!!\r\n", ret);
        return 1;
    }
#if 1
    //if(driver_sdcard_get_init_status())
    {
        //dir_buf=(DIR_QUEUE*)jmalloc(MAX_DIR_SUPPORT*sizeof(DIR_QUEUE),0);//最大支持的文件夹
        //if(!dir_buf)
        //	return -1;
        dir_buf = &g_dirbuf[0];

        mem_set(file_queue, 0, MAX_SONG_DIR * sizeof(FAT_DIR_INFO));
        mp3filecount = 0;
        mp3queuecount = 0;
        point_front = point_rear = dir_buf;

        point_front ->cluster_number = Fatfs_buf.dirbase;
        point_front ->dirlevel = 0;
        point_front ->ExNoChainFlag = 0;

        if (Fatfs_buf.fs_type == FS_FAT16)
            point_front ->broot_dir = 1;
        else
            point_front ->broot_dir = 0;

        point_rear++;
        rear_cnt++;
        while(1)
        {
            if ((mp3queuecount < MAX_SONG_DIR)
                    && (point_front ->dirlevel < MAX_DIR_LEVEL)
                    && (point_front != point_rear)
                    && (count < MAX_DIR_SUPPORT))
            {
                if(rear_cnt > MAX_DIR_SUPPORT - 1)
                    break;

                get_curdir_info(point_front);
                count++;
                point_front++;
            }
            else
                break;
        }

        FAT_PRT("count=%d  mp3queuecount=%d  mp3filecount=%d,rear_cnt =%d\r\n", count, mp3queuecount, mp3filecount, rear_cnt);
        //driver_sdcard_set_init_status(0);
        //jfree(dir_buf);
        // dir_buf = NULL;
    }
#endif
    return 0;
}

/*Sd card & filesystem initialization*/
//uint32 SD_Fs_Init(void)
uint32 Media_Fs_Init(uint8 type)
{
    uint32 ret = 1;
    FileInfo.fat_ok_flag = 0;
    if(f_mount(1, &Fatfs_buf) == FR_OK)
    {
        if(initfatsystem(type) == FR_OK)
        {
            FileInfo.fat_ok_flag = 1;
            ret = 0;
        }
    }
    return ret;
}

uint8 get_fat_ok_flag(void)
{
    return FileInfo.fat_ok_flag;
}

uint16 get_musicfile_count(void)
{
    return mp3filecount;
}
FILE_INFO *get_file_info(void)
{
    return (&FileInfo);
}

uint8 get_disk_type(void)
{
    return Fatfs_buf.drive;
}

#endif


#if CFG_USE_USB_HOST
#endif


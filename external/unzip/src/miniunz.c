/*
   miniunz.c
   Version 1.01e, February 12th, 2005

   Copyright (C) 1998-2005 Gilles Vollant
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>

#define unix

#ifdef unix
#include <unistd.h>
#include <utime.h>
#else
#include <dirent.h>
#include <io.h>
#endif

#include "unzip.h"
#include "miniunz.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

/*
  mini unzip, demo of unzip package

  usage :
  Usage : miniunz [-exvlo] file.zip [file_to_extract] [-d extractdir]

  list the file in the zipfile, and print the content of FILE_ID.ZIP or README.TXT
    if it exists
*/

/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    tmu_date : the SAME new date at the tm_unz format */
void change_file_date(const char*  filename, tm_unz tmu_date)
{

#ifdef unix
    struct utimbuf ut;
    struct tm newdate;
    newdate.tm_sec = tmu_date.tm_sec;
    newdate.tm_min = tmu_date.tm_min;
    newdate.tm_hour = tmu_date.tm_hour;
    newdate.tm_mday = tmu_date.tm_mday;
    newdate.tm_mon = tmu_date.tm_mon;
    if (tmu_date.tm_year > 1900)
        newdate.tm_year = tmu_date.tm_year - 1900;
    else
        newdate.tm_year = tmu_date.tm_year;
    newdate.tm_isdst = -1;

    ut.actime = ut.modtime = mktime(&newdate);
    //utime(filename, &ut);
#endif
}

/* mymkdir and change_file_date are not 100 % portable
   As I don't know well Unix, I wait feedback for the unix portion */

int mymkdir(const char* dirname)
{
    int ret = 0;
#ifdef unix
    ret = mkdir(dirname, 0775);
#endif
    return ret;
}

int makedir(newdir) char* newdir;
{
    char* buffer;
    char* p;
    int len = (int)strlen(newdir);

    if (len <= 0)
        return 0;

    buffer = (char*)malloc(len + 1);
    strcpy(buffer, newdir);

    if (buffer[len - 1] == '/') {
        buffer[len - 1] = '\0';
    }
    if (mymkdir(buffer) == 0) {
        free(buffer);
        return 1;
    }

    p = buffer + 1;
    while (1) {
        char hold;

        while (*p && *p != '\\' && *p != '/')
            p++;
        hold = *p;
        *p = 0;
        if ((mymkdir(buffer) == -1) && (errno == ENOENT)) {
            printf("couldn't create directory %s\n", buffer);
            free(buffer);
            return 0;
        }
        if (hold == 0)
            break;
        *p++ = hold;
    }
    free(buffer);
    return 1;
}

void do_banner()
{
    printf("MiniUnz 1.01b, demo of zLib + Unz package written by Gilles Vollant\n");
    printf("more info at http://www.winimage.com/zLibDll/unzip.html\n\n");
}

// static void do_help()
// {
//     printf("Usage : miniunz [-e] [-x] [-v] [-l] [-o] [-p password] file.zip [file_to_extr.] [-d extractdir]\n\n"
//            "  -e  Extract without pathname (junk paths)\n"
//            "  -x  Extract with pathname\n"
//            "  -v  list files\n"
//            "  -l  list files\n"
//            "  -d  directory to extract into\n"
//            "  -o  overwrite files without prompting\n"
//            "  -p  extract crypted file using password\n\n");
// }

int do_list(unzFile uf)
{
    uLong i;
    unz_global_info gi;
    int err;

    err = unzGetGlobalInfo(uf, &gi);
    if (err != UNZ_OK) {
        printf("error %d with zipfile in unzGetGlobalInfo \n", err);
    }

    printf(" Length  Method   Size  Ratio   Date    Time   CRC-32     Name\n");
    printf(" ------  ------   ----  -----   ----    ----   ------     ----\n");
    for (i = 0; i < gi.number_entry; i++) {
        char filename_inzip[256];
        unz_file_info file_info;
        uLong ratio = 0;
        const char* string_method = NULL;
        char charCrypt = ' ';
        err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
        if (err != UNZ_OK) {
            printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
            break;
        }
        if (file_info.uncompressed_size > 0)
            ratio = (file_info.compressed_size * 100) / file_info.uncompressed_size;

        /* display a '*' if the file is crypted */
        if ((file_info.flag & 1) != 0)
            charCrypt = '*';

        if (file_info.compression_method == 0)
            string_method = "Stored";
        else if (file_info.compression_method == Z_DEFLATED) {
            uInt iLevel = (uInt)((file_info.flag & 0x6) / 2);
            if (iLevel == 0)
                string_method = "Defl:N";
            else if (iLevel == 1)
                string_method = "Defl:X";
            else if ((iLevel == 2) || (iLevel == 3))
                string_method = "Defl:F"; /* 2:fast , 3 : extra fast*/
        } else
            string_method = "Unkn. ";

        printf("%7lu  %6s%c%7lu %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
               file_info.uncompressed_size, string_method,
               charCrypt,
               file_info.compressed_size,
               ratio,
               (uLong)file_info.tmu_date.tm_mon + 1,
               (uLong)file_info.tmu_date.tm_mday,
               (uLong)file_info.tmu_date.tm_year % 100,
               (uLong)file_info.tmu_date.tm_hour, (uLong)file_info.tmu_date.tm_min,
               (uLong)file_info.crc, filename_inzip);
        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK) {
                printf("error %d with zipfile in unzGoToNextFile\n", err);
                break;
            }
        }
    }

    unzClose(uf);

    return 0;
}

int direct_is_exist(const char* path)
{
    struct stat stat_buf;
    if (stat(path, &stat_buf) == 0) {
        return (stat_buf.st_mode & S_IFMT) == S_IFDIR;
    }

    return 0;
}

int do_extract_currentfile(unzFile uf,
        const int* popt_extract_without_path,
        int* popt_overwrite,
        const char* password,
        const char* direct_name)
{
    char filename_inzip[256];
    int err = UNZ_OK;
    FILE* fout = NULL;
    void* buf;
    uInt size_buf;

    unz_file_info file_info;
    err = unzGetCurrentFileInfo(uf, &file_info, filename_inzip, sizeof(filename_inzip),
                                NULL, 0, NULL, 0);
    if (err != UNZ_OK) {
        printf("error %d with zipfile in unzGetCurrentFileInfo\n", err);
        return err;
    }

    char path_name[256];
    int path_length = 0;
    char absolute_path_name[256];
    char* p;
    for (p = filename_inzip; *p != '\0'; p++) {
        path_name[path_length] = *p;
        if ((*p) == '/') {
            path_name[path_length] = '\0';
            strcpy(absolute_path_name, direct_name);
            strcat(absolute_path_name, path_name);
            if (!direct_is_exist(absolute_path_name)) {
                mymkdir(absolute_path_name);
            }
            path_name[path_length] = '/';
        }

        path_length++;
    }

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf == NULL) {
        printf("Error allocating memory\n");
        return UNZ_INTERNALERROR;
    }

    char write_filename[256];
    int skip = 0;

    strcpy(write_filename, direct_name);
    strcat(write_filename, filename_inzip);

    err = unzOpenCurrentFilePassword(uf, password);
    if (err != UNZ_OK) {
        printf("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
    }

    if (((*popt_overwrite) == 0) && (err == UNZ_OK)) {
        char rep = 0;
        FILE* ftestexist;
        ftestexist = fopen(write_filename, "rb");
        if (ftestexist != NULL) {
            fclose(ftestexist);
            do {
                char answer[128];
                int ret;

                printf("The file %s exists. Overwrite ? [y]es, [n]o, [A]ll: ", write_filename);
                ret = scanf("%1s", answer);
                if (ret != 1) {
                    exit(EXIT_FAILURE);
                }
                rep = answer[0];
                if ((rep >= 'a') && (rep <= 'z'))
                    rep -= 0x20;
            } while ((rep != 'Y') && (rep != 'N') && (rep != 'A'));
        }

        if (rep == 'N')
            skip = 1;

        if (rep == 'A')
            *popt_overwrite = 1;
    }

    if ((skip == 0) && (err == UNZ_OK)) {
        fout = fopen(write_filename, "wb");
        if (fout == NULL) {
            //printf("error opening %s\n", write_filename);
        }
    }

    if (fout != NULL) {
        //printf(" extracting: %s\n", write_filename);

        do {
            err = unzReadCurrentFile(uf, buf, size_buf);
            if (err < 0) {
                printf("error %d with zipfile in unzReadCurrentFile\n", err);
                break;
            }
            if (err > 0)
                if (fwrite(buf, err, 1, fout) != 1) {
                    printf("error in writing extracted file\n");
                    err = UNZ_ERRNO;
                    break;
                }
        } while (err > 0);
        if (fout)
            fclose(fout);

        if (err == 0)
            change_file_date(write_filename, file_info.tmu_date);
    }

    if (err == UNZ_OK) {
        err = unzCloseCurrentFile(uf);
        if (err != UNZ_OK) {
            printf("error %d with zipfile in unzCloseCurrentFile\n", err);
        }
    } else
        unzCloseCurrentFile(uf); /* don't lose the error */

    free(buf);
    return err;
}

int do_extract(unzFile uf,
        int opt_extract_without_path,
        int opt_overwrite,
        const char* password,
        const char* direct_name)
{
    uLong i;
    unz_global_info gi;
    int err;

    err = unzGetGlobalInfo(uf, &gi);
    if (err != UNZ_OK)
        printf("error %d with zipfile in unzGetGlobalInfo \n", err);

    for (i = 0; i < gi.number_entry; i++) {
        if (do_extract_currentfile(uf, &opt_extract_without_path,
                                   &opt_overwrite,
                                   password,
                                   direct_name) != UNZ_OK)
            break;

        if ((i + 1) < gi.number_entry) {
            err = unzGoToNextFile(uf);
            if (err != UNZ_OK) {
                printf("error %d with zipfile in unzGoToNextFile\n", err);
                break;
            }
        }
    }

    return 0;
}

int do_extract_onefile(unzFile uf,
        const char* filename,
        int opt_extract_without_path,
        int opt_overwrite,
        const char* password,
        const char* direct_name)
{
    if (unzLocateFile(uf, filename, CASESENSITIVITY) != UNZ_OK) {
        printf("file %s not found in the zipfile\n", filename);
        return 2;
    }

    if (do_extract_currentfile(uf, &opt_extract_without_path,
                               &opt_overwrite,
                               password,
                               direct_name) == UNZ_OK)
        return 0;
    else
        return 1;
}

int miniUnzip(const char* filePath, const char* outPath)
{
    const char* zipfilename = filePath;
    const char* filename_to_extract = NULL;
    const char* password = NULL;
    char filename_try[MAXFILENAME + 16] = "";
    int opt_do_list = 0;
    int opt_do_extract = 1;
    int opt_do_extract_withoutpath = 0;
    int opt_overwrite = 1;
    int opt_extractdir = 0;
    const char* dirname = NULL;
    unzFile uf = NULL;
    int ret_value = 0;

    if (zipfilename != NULL) {
#ifdef USEWIN32IOAPI
        zlib_filefunc_def ffunc;
#endif

        strncpy(filename_try, zipfilename, MAXFILENAME - 1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
        filename_try[MAXFILENAME] = '\0';

#ifdef USEWIN32IOAPI
        fill_win32_filefunc(&ffunc);
        uf = unzOpen2(zipfilename, &ffunc);
#else
        uf = unzOpen(zipfilename);
#endif
    }

    if (uf == NULL) {
        printf("Cannot open %s\n", zipfilename);
        return 1;
    }
    //printf("%s opened\n", filename_try);

    if (opt_do_list == 1)
        return do_list(uf);
    else if (opt_do_extract == 1) {
        if (opt_extractdir && chdir(dirname)) {
            printf("Error changing into %s, aborting\n", dirname);
            exit(-1);
        }

        if (filename_to_extract == NULL)
            ret_value = do_extract(uf, opt_do_extract_withoutpath, opt_overwrite, password, outPath);
        else
            ret_value = do_extract_onefile(uf, filename_to_extract,
                                           opt_do_extract_withoutpath, opt_overwrite, password, outPath);
    }

    unzClose(uf);

    return ret_value;
}

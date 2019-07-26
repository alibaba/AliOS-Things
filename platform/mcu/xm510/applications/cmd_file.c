/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "aos/kernel.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "fatfs/aos_fatfs.h"

#define XM_PATH_MAX 256
//#define XM_LINE_LEN 256

static char working_directory[XM_PATH_MAX] = {"/"};

/**
 * this function will normalize a path according to specified parent directory
 * and file name.
 *
 * @param directory the parent path
 * @param filename the file name
 *
 * @return the built full file path (absolute path)
 */
static char *_normalize_path(const char *path)
{
    char *fullpath;
    char *dst0, *dst, *src;

    /* check parameters */
    if(path == NULL)
	{
		return NULL;
	}

    if (path[0] != '/') /* it's a absolute path, use it directly */
    {
        fullpath = aos_malloc(strlen(working_directory) + strlen(path) + 2);

        if (fullpath == NULL)
            return NULL;

        /* join path and file name */
        snprintf(fullpath, strlen(working_directory) + strlen(path) + 2,
            "%s/%s", working_directory, path);
    }
    else
    {
        fullpath = strdup(path); /* copy string */

        if (fullpath == NULL)
            return NULL;
    }

    src = fullpath;
    dst = fullpath;

    dst0 = dst;
    while (1)
    {
        char c = *src;

        if (c == '.')
        {
            if (!src[1]) src ++; /* '.' and ends */
            else if (src[1] == '/')
            {
                /* './' case */
                src += 2;

                while ((*src == '/') && (*src != '\0'))
                    src ++;
                continue;
            }
            else if (src[1] == '.')
            {
                if (!src[2])
                {
                    /* '..' and ends case */
                    src += 2;
                    goto up_one;
                }
                else if (src[2] == '/')
                {
                    /* '../' case */
                    src += 3;

                    while ((*src == '/') && (*src != '\0'))
                        src ++;
                    goto up_one;
                }
            }
        }

        /* copy up the next '/' and erase all '/' */
        while ((c = *src++) != '\0' && c != '/')
            *dst ++ = c;

        if (c == '/')
        {
            *dst ++ = '/';
            while (c == '/')
                c = *src++;

            src --;
        }
        else if (!c)
            break;

        continue;

up_one:
        dst --;
        if (dst < dst0)
        {
            aos_free(fullpath);
            return NULL;
        }
        while (dst0 < dst && dst[-1] != '/')
            dst --;
    }

    *dst = '\0';

    /* remove '/' in the end of path if exist */
    dst --;
    if ((dst != fullpath) && (*dst == '/'))
        *dst = '\0';

    /* final check fullpath is not empty, for the special path of lwext "/.." */
    if ('\0' == fullpath[0])
    {
        fullpath[0] = '/';
        fullpath[1] = '\0';
    }

    return fullpath;
}

int chdir(const char *path)
{
    char *fullpath;
    aos_dir_t *d;

    if (path == NULL)
    {
        return 0;
    }

    if (strlen(path) > XM_PATH_MAX)
    {
        return -1;
    }

    fullpath = _normalize_path(path);
    if (fullpath == NULL)
    {
        return -1; /* build path failed */
    }
	if(strcmp(fullpath, "/"))
	{
		d = aos_opendir(fullpath);
		if (d == NULL)
		{
			aos_free(fullpath);
			/* this is a not exist directory */

			return -1;
		}

		/* close directory stream */
		aos_closedir(d);
	}

    /* copy full path to working directory */
    strncpy(working_directory, fullpath, XM_PATH_MAX);
    /* release normalize directory path name */
    aos_free(fullpath);

    return 0;
}

void cmd_cd(char *pwbuf, int blen, int argc, char **argv)
{
	if(argc == 1)
	{
		chdir("/");
	}
	else
	{
		chdir(argv[1]);
	}

	printf("%s\n", working_directory);
}

void cmd_pwd(char *pwbuf, int blen, int argc, char **argv)
{
	printf("%s\n", working_directory);
}

void cmd_ls(char *pwbuf, int blen, int argc, char **argv)
{
	char *path = NULL;
    struct aos_stat stat;
	const char *file_system[] = {"/sdcard", "/jffs2", "/ramfs", "/cramfs"};
	int i = 0;
	if(argc == 1) {
		path = strdup(working_directory);
	} else {
		path = _normalize_path(argv[1]);
	}
	if(path == NULL) {
		printf("path error!\n");
		return;
	}else if(!strcmp(path, "/")){
		for(i = 0; i < (sizeof(file_system) / sizeof(file_system[0])); i++)
		{
			if((aos_stat(file_system[i], &stat) == 0) && ((S_ISDIR(stat.st_mode)))){
				printf("%-25s", file_system[i]);
				printf("%-25s\n", "<DIR>");
			}
		}
		goto free_path;
	}else if (aos_stat(path, &stat) < 0) {
        printf("BAD file: %s\n", path);
		goto free_path;
	} else {
		printf("%-25s", path);
		if (S_ISDIR(stat.st_mode)) {
			printf("%-25s\n", "<DIR>");
		} else {
			printf("%-25lu\n", stat.st_size);
			goto free_path;
		}
	}         
	
    aos_dir_t *dp = (aos_dir_t *)aos_opendir(path);

    if (dp) {
        aos_dirent_t *out_dirent;
        char *full_path = aos_malloc(XM_PATH_MAX);
        while(1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent == NULL)
                break;
            if(snprintf(full_path, strlen(path) + strlen(out_dirent->d_name) + 2, "%s/%s", path, out_dirent->d_name) < 0) {
				printf("full path %s/%s out of memory\n", path, out_dirent->d_name);
				break ;
			}
            memset(&stat, 0, sizeof(struct aos_stat));
            if (aos_stat(full_path, &stat) < 0) {
                printf("BAD file: %s\n", full_path);
            } else {
                printf("%-25s", out_dirent->d_name);
                if (S_ISDIR(stat.st_mode)) {
                    printf("%-25s\n", "<DIR>");
                } else {
                    printf("%-25lu\n", stat.st_size);
                }
            }         
        }
        aos_free(full_path);
    	aos_closedir(dp);
    }

free_path:
	aos_free(path);
	return; 
}

void cmd_rm(char *pwbuf, int blen, int argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: rm [FILE]\n");
		return ;
	}

	char *path = _normalize_path(argv[1]);
	if(path == NULL) {
		printf("path error!\n");
		return;
	}
	
	if (aos_unlink(path) < 0)
    {
        printf("Delete %s failed\n", path);
    }

	aos_free(path);
}

void cmd_cat(char *pwbuf, int blen, int argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: cat [FILE]\n");
		return ;
	}

	int fd;
	int length;
    char buffer[81];
	char *path = _normalize_path(argv[1]);

    if ((fd = aos_open(path, O_RDONLY)) < 0)
    {
        printf("Open %s failed %d\n", path, fd);

        return;
    }

    do
    {
        memset(buffer, 0, sizeof(buffer));
        length = aos_read(fd, buffer, sizeof(buffer)-1 );
        if (length > 0)
        {
            printf("%s", buffer);
        }
    }while (length > 0);

	//printf("\n");
	fflush(stdout);

    aos_close(fd);
}

void cmd_touch(char *pwbuf, int blen, int argc, char **argv)
{
	int fd;
	char *path = NULL;
	struct aos_stat stat;
	if(argc != 2) {
		printf("Usage: touch [FILE]\n");
		return ;
	}

	path = _normalize_path(argv[1]);

    memset(&stat, 0, sizeof(struct aos_stat));
	if (aos_stat(path, &stat) >= 0)
    {
        printf("file %s exist\n", argv[1]);
		goto free_path;
    }

	if((fd = aos_open(path, O_RDWR | O_CREAT | O_TRUNC)) < 0) 
	{
		printf("create file %s failed %d\n", path, fd);
		goto free_path;
	}
	aos_close(fd);

free_path:
	aos_free(path);
	return;
}

#define BUF_SZ  4096
void copyfile(const char *src, const char *dst)
{
    int src_fd, dst_fd;
    unsigned char *block_ptr;
    int read_bytes;

    block_ptr = aos_malloc(BUF_SZ);
    if (block_ptr == NULL)
    {
        printf("out of memory\n");

        return;
    }

    if ((src_fd = aos_open(src, O_RDONLY)) < 0)
    {
        aos_free(block_ptr);
        printf("Read %s failed\n", src);

        return;
    }
    if ((dst_fd = aos_open(dst, O_WRONLY | O_CREAT | O_TRUNC)) < 0)
    {
        aos_free(block_ptr);
        aos_close(src_fd);

        printf("Write %s failed %d\n", dst, dst_fd);

        return;
    }

    do
    {
        read_bytes = aos_read(src_fd, block_ptr, BUF_SZ);
        if (read_bytes > 0)
        {
            int length;

            length = aos_write(dst_fd, block_ptr, read_bytes);
            if (length != read_bytes)
            {
                /* write failed. */
                printf("Write file data failed, errno=%d\n", length);
                break;
            }
        }
    } while (read_bytes > 0);

    aos_close(src_fd);
    aos_close(dst_fd);
    aos_free(block_ptr);
}

static const char *_get_path_lastname(const char *path)
{
    char * ptr;
    if ((ptr = strrchr(path, '/')) == NULL)
        return path;

    /* skip the '/' then return */
    return ++ptr;
}

void cmd_cp(char *pwbuf, int blen, int argc, char **argv)
{
	char *src = NULL;
	char dst[XM_PATH_MAX] = {0};
	char *path = NULL;
	struct aos_stat stat;
	if(argc != 3) {
		printf("Usage: cp SOURCE DEST\n");
		return;
	}

	src = _normalize_path(argv[1]);
	if(src == NULL)
	{
		printf("path error!\n");
		return;
	}

    /* check the staus of src and dst */
    if (aos_stat(src, &stat) < 0)
    {
        printf("copy failed, bad %s\n", src);
		goto out;
    }
    if (S_ISDIR(stat.st_mode)) {
		printf("copy failed, %s is DIR\n", src);
		goto out;
	}

	path = _normalize_path(argv[2]);
    if (aos_stat(path, &stat) < 0)
    {
        strcpy(dst, path);
    }
    else
    {
        if (S_ISDIR(stat.st_mode)) 
		{
			const char *filename = _get_path_lastname(src);
			if(snprintf(dst, strlen(path) + strlen(filename) + 2, "%s/%s", path, filename) < 0) {
				printf("dst name out of memory\n");
				goto out;
			}
		}
        else
			strcpy(dst, path);
    }

    // do copy
    copyfile(src, dst);

out:
	if(src)
		aos_free(src);
	if(path)
		aos_free(path);
}

void cmd_mkdir(char *pwbuf, int blen, int argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: mkdir [DIR]\n");
		return ;
	}

	char *path = _normalize_path(argv[1]);
	if(path == NULL) {
		printf("path error!\n");
		return;
	}
	
	if (aos_mkdir(path) < 0)
    {
        printf("mkdir %s failed\n", path);
    }

	aos_free(path);
}

void cmd_rmdir(char *pwbuf, int blen, int argc, char **argv)
{
	if(argc != 2) {
		printf("Usage: rmdir [DIR]\n");
		return ;
	}

	char *path = _normalize_path(argv[1]);
	if(path == NULL) {
		printf("path error!\n");
		return;
	}

	if (aos_rmdir(path) < 0)
    {
        printf("rmdir %s failed\n", path);
    }

	aos_free(path);
}

static int parsex(char c)
{
	int r;
	if((c >= '0' && c <= '9')) 
		r = c - '0';
	else if(c >= 'a' && c <= 'f') 
		r = c - 'a';
	else if(c >= 'A' && c <= 'F') 
		r = c - 'A';
	else 
		r = -1;
	return r;
}

static int atoi2(const char **ptr)
{
	int c;
	const char *p = *ptr;
	if((c = parsex(*p++)) >= 0) {
		int c2 = parsex(*p);
		if(c2 >= 0){
			c = c*0x10+c2;
		} else {
			p--;
		}						
	} else {
		c = '\\';
		p -= 3;
	}
	*ptr = p;
	return c;
}

void cmd_echo(char *pwbuf, int blen, int argc, char **argv)
{
	int c;
	const char *arg;
	char *out;
	char *buffer;
	unsigned int buflen;

	if(argc < 2) {
		return ;
	} else if(argc > 3) {
		printf("Usage: echo \"string\" [filename]\n");
		return ;
	}

	arg = argv[1];
	buflen = strlen(arg);
	out = buffer = aos_malloc(buflen + 1); /* +1 is needed for "no args" case */

	while ((c = *arg++) != '\0') {
		if (c == '\\') {			
			switch(*arg) {
				case 'c': goto do_write;
				case 'a': c = '\a'; break;
				case 'b': c = '\b'; break;
				case 'n': c = '\n'; break;
				case 'r': c = '\r'; break;
				case 't': c = '\t'; break;
				case 'v': c = '\v'; break;
				case 'f': c = '\f'; break;
				case '\\': c = '\\'; break;
				case 'x': arg++; c = atoi2(&arg); break;
				default: c = '\\'; arg--; break;
			}
			arg ++;
		}
		*out++ = c;
	}
    *out = '\0';

do_write:
	if(argc == 2) {
		printf("%s\n", buffer);
	} else if (argc == 3) {
		char *path = _normalize_path(argv[2]);
		int fd = aos_open(path, O_RDWR | O_APPEND | O_CREAT);
		if(fd < 0) {
			printf("open file %s error!\n", path);
		} else {
			aos_write(fd, buffer, out - buffer);
			aos_close(fd);
		}
		aos_free(path);
	}

	aos_free(buffer);
	return ;
}

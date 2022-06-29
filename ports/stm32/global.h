#ifndef __GLOBAL_H
#define __GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "systick.h"
#include "pendsv.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

extern mp_obj_t get_path(const char *src_path, uint8_t *res);
extern mp_obj_t file_type(const char *fileName);
extern FRESULT f_open_helper(FIL *fp, const TCHAR *path, BYTE mode);
extern int sprintf(char *str, const char *fmt, ...);
extern bool check_sys_file(const char *check_file);

#define my_isdigit(c) ((c) >= '0' && (c) <= '9')

#ifdef __cplusplus
}
#endif
#endif

#ifdef __GLOBAL_H
// -----------------------------------------------------------------------
__attribute__((weak)) bool check_sys_file(const char *check_file)
{
    mp_obj_t iter = mp_vfs_ilistdir(0, NULL);
    mp_obj_t next;
    while ((next = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        const char *flie = mp_obj_str_get_str(mp_obj_subscr(next, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL));
        if (0 == strcmp(check_file, flie)) {
            return true;
        }
    }
    return false;
}

__weak char *itoa(int num, char *str, int radix)
{
    char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum;
    int i = 0, j, k;

    if (radix == 10 && num < 0) {
        unum = (unsigned)-num;
        str[i++] = '-';
    } else {
        unum = (unsigned)num;
    }
    do {
        str[i++] = index[unum % (unsigned)radix];
        unum /= radix;

    } while (unum);
    str[i] = '\0';
    if (str[0] == '-')
        k = 1;
    else
        k = 0;

    char temp;
    for (j = k; j <= (i - 1) / 2; j++) {
        temp = str[j];
        str[j] = str[i - 1 + k - j];
        str[i - 1 + k - j] = temp;
    }
    return str;
}

__weak int atoi(const char *pstr)
{
    int Ret_Integer = 0;
    int Integer_sign = 1;
    /*
     * 判断指针是否为空
     */
    if (pstr == NULL) {
        return 0;
    }

    /*
     * 跳过前面的空格字符
     */
    while (*pstr == '\0') {
        pstr++;
    }

    /*
     * 判断正负号
     * 如果是正号，指针指向下一个字符
     * 如果是符号，把符号标记为Integer_sign置-1，然后再把指针指向下一个字符
     */
    if (*pstr == '-') {
        Integer_sign = -1;
    }
    if (*pstr == '-' || *pstr == '+') {
        pstr++;
    }

    /*
     * 把数字字符串逐个转换成整数，并把最后转换好的整数赋给Ret_Integer
     */
    while (*pstr >= '0' && *pstr <= '9') {
        Ret_Integer = Ret_Integer * 10 + *pstr - '0';
        pstr++;
    }
    Ret_Integer = Integer_sign * Ret_Integer;

    return Ret_Integer;
}
__weak char *strrchr(const char *str, int ch)
{
    char *p = (char *)str;
    while (*str)
        str++;
    while (str-- != p && *str != (char)ch)
        ;
    if (*str == (char)ch)
        return (char *)str;
    return NULL;
}

// -----------------------------------------------------------------------
// 路径解析出来
// 返回0 为flash 1为sd。否则解析失败。
// src_path:输入路径 ret_path：返回的相对路径
__weak mp_obj_t get_path(const char *src_path, uint8_t *res)
{
    uint8_t date_len = 0, cp_len = 0;

    char upda_str[50];

    char cp_str[7];
    char *ret_path;

    memset(upda_str, '\0', sizeof(upda_str));
    strncpy(upda_str, &src_path[1], strlen(src_path) - 1);
    date_len = strlen(upda_str);

    ret_path = strchr(upda_str, '/');
    if (ret_path == 0)
        mp_raise_ValueError(MP_ERROR_TEXT("file path error"));

    cp_len = date_len - strlen(ret_path);

    memset(cp_str, '\0', 7);
    strncpy(cp_str, upda_str, cp_len);

    *res = 2;
    if (strncmp(cp_str, "flash", 5) == 0)
        *res = 0;
    else if (strncmp(cp_str, "sd", 2) == 0)
        *res = 1;
    else {
        mp_raise_ValueError(MP_ERROR_TEXT("no find sd or flash path"));
    }

    date_len = (date_len - cp_len - 1);
    memset(upda_str, '\0', sizeof(upda_str));
    strncpy(upda_str, &src_path[cp_len + 2], date_len);

    return mp_obj_new_str(upda_str, date_len);
}

__weak mp_obj_t file_type(const char *fileName)
{
    char dest[10];
    memset(dest, '\0', sizeof(dest));
    char *ret = strrchr(fileName, '.');
    if (ret == NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("no find file type"));
    }
    strncpy(dest, &ret[1], strlen(ret) - 1);
    return mp_obj_new_str(dest, strlen(ret) - 1);
}
__weak int sprintf(char *str, const char *fmt, ...)
{
    int count = 0;
    char c;
    char *s;
    int n;

    int index = 0;
    int ret = 2;

    char buf[65];
    char digit[16];
    int num = 0;
    int len = 0;

    memset(buf, 0, sizeof(buf));
    memset(digit, 0, sizeof(digit));

    va_list ap;

    va_start(ap, fmt);

    while (*fmt != '\0') {
        // printf("*fmt=[%c]\n", *fmt);
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
            case 'd': /* 整型 */
            {
                n = va_arg(ap, int);
                if (n < 0) {
                    *str = '-';
                    str++;
                    n = -n;
                }
                // printf("case d n=[%d]\n", n);
                // itoa(n, buf);
                itoa(n, buf, 10);
                // printf("case d buf=[%s]\n", buf);
                memcpy(str, buf, strlen(buf));
                str += strlen(buf);
                break;
            }
            case 'c': /* 字符型 */
            {
                c = va_arg(ap, int);
                *str = c;
                str++;

                break;
            }
            case 'x': /* 16进制 */
            {
                n = va_arg(ap, int);
                // xtoa(n, buf);
                itoa(n, buf, 16);
                memcpy(str, buf, strlen(buf));
                str += strlen(buf);
                break;
            }
            case 's': /* 字符串 */
            {
                s = va_arg(ap, char *);
                memcpy(str, s, strlen(s));
                str += strlen(s);
                break;
            }
            case '%': /* 输出% */
            {
                *str = '%';
                str++;

                break;
            }
            case '0': /* 位不足的左补0 */
            {
                index = 0;
                num = 0;
                memset(digit, 0, sizeof(digit));

                while (1) {
                    fmt++;
                    ret = my_isdigit(*fmt);
                    if (ret == 1) {
                        // 是数字
                        digit[index] = *fmt;
                        index++;
                    } else {
                        num = atoi(digit);
                        break;
                    }
                }
                switch (*fmt) {
                case 'd': /* 整型 */
                {
                    n = va_arg(ap, int);
                    if (n < 0) {
                        *str = '-';
                        str++;
                        n = -n;
                    }
                    // itoa(n, buf);
                    itoa(n, buf, 10);
                    len = strlen(buf);
                    if (len >= num) {
                        memcpy(str, buf, strlen(buf));
                        str += strlen(buf);
                    } else {
                        memset(str, '0', num - len);
                        str += num - len;
                        memcpy(str, buf, strlen(buf));
                        str += strlen(buf);
                    }
                    break;
                }
                case 'x': /* 16进制 */
                {
                    n = va_arg(ap, int);
                    // xtoa(n, buf);
                    itoa(n, buf, 16);
                    len = strlen(buf);
                    if (len >= num) {
                        memcpy(str, buf, len);
                        str += len;
                    } else {
                        memset(str, '0', num - len);
                        str += num - len;
                        memcpy(str, buf, len);
                        str += len;
                    }
                    break;
                }
                case 's': /* 字符串 */
                {
                    s = va_arg(ap, char *);
                    len = strlen(s);
                    if (len >= num) {
                        memcpy(str, s, strlen(s));
                        str += strlen(s);
                    } else {
                        memset(str, '0', num - len);
                        str += num - len;
                        memcpy(str, s, strlen(s));
                        str += strlen(s);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            default:
                break;
            }
        } else {
            *str = *fmt;
            str++;

            if (*fmt == '\n') {
            }
        }
        fmt++;
    }

    va_end(ap);

    return count;
}
// ==================================================================

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
STATIC FATFS *lookup_path(const TCHAR **path)
{
    mp_vfs_mount_t *fs = mp_vfs_lookup_path(*path, path);
    if (fs == MP_VFS_NONE || fs == MP_VFS_ROOT) {
        return NULL;
    }
    // here we assume that the mounted device is FATFS
    return &((fs_user_mount_t *)MP_OBJ_TO_PTR(fs->obj))->fatfs;
}

__weak FRESULT f_open_helper(FIL *fp, const TCHAR *path, BYTE mode)
{
    FATFS *fs = lookup_path(&path);
    if (fs == NULL) {
        return FR_NO_PATH;
    }
    return f_open(fs, fp, path, mode);
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif

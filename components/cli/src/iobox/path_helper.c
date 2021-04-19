#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <path_helper.h>

static int up_one_level(char *s)
{
    char *tail;

    if (!s)
        return -1;

    tail = s + strlen(s) - 1;
    if (*tail == '/')
        tail--;

    while (*tail != '\0' && *tail != '/')
        tail--;

    if (*tail == '\0') {
        return -1;
    } else {
        *(tail + 1) = '\0';
        return 0;
    }
}

char *get_realpath(const char *path, char *resolved_path, unsigned int len)
{
    char *ret, *p = (char *)path, *r = resolved_path;

    if (!path || !r || len < 1)
        return NULL;

    memset(r, 0, len);

    // deal with heading char
    if (p[0] != '/') {
        // relative path
        ret = getcwd(r, len);
        if (!ret)
            return NULL;

        // add tailing '/' if no
        if (r[strlen(r) - 1] != '/') {
            r[strlen(r)] = '/';
        }

        r += strlen(r);
    } else {
        // absolute path
        r[0] = '/';
        r++;
    }

    // iterate to exclude '.', '..'. '/'
    while (*p != '\0') {
        while (*p == '/')
            p++;
        if (*p == '\0')
            break;

        if (*p == '.') {
            p++;
            // end with '.'
            if (*p == '\0')
                break;

            if (*p == '.') {
                // '..' or '../'
                if ((*(p + 1) != '/') && (*(p + 1) != '\0')) {
                    printf("Invalid path %s\r\n", path);
                    return NULL;
                } else {
                    // '..' case
                    p++;
                    // if (*p == '/') {
                    if (up_one_level(resolved_path) != 0) {
                        printf("Failed to go up now. Invalid path %s\r\n", path);
                        return NULL;
                    }

                    r = resolved_path + strlen(resolved_path);
                    // }

                    // end with '.'
                    if (*p == '\0') {
                        break;
                    }
                }
            } else {
                if (*p == '/' || *p == '\0') {
                    p++;
                } else {
                    // '.xxx' might be hidden file or dir
                    p--;
                    goto copy_valid;
                }
            }
        }

        while (*p == '/')
            p++;
        if (*p == '\0')
            break;

        // if another round of ./.., just continue
        if (*p == '.')
            continue;

copy_valid:
        // path string may be found now, save to r
        while ((*p != '/') && (*p != '\0'))
            *r++ = *p++;

        // add taling '/' if necessary
        if (*(r - 1) != '/') {
            *r++ = '/';
        }
    }

    /**
     * considering "cd ../config" for tab key case,
     * we need set string EOF avoid out of control.
     */
    *r = '\0';

    // exclude the tailing '/', just in case it is a file
    if ((resolved_path[strlen(resolved_path) - 1] == '/') &&
        (strlen(resolved_path) != 1)) {
        resolved_path[strlen(resolved_path) - 1] = '\0';
    }

    return resolved_path;
}

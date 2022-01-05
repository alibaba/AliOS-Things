#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "ota_log.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota_hal.h"
#include "upack_data_file.h"

OTA_WEAK int ota_fopen(const char *filename, int mode)
{
    int fd = -1;
    if (filename != NULL) {
        fd = open(filename, mode);
    }
    return fd;
}

OTA_WEAK int ota_fwrite(int fd, const void *buf, unsigned int size)
{
    int ret = -1;
    if ((fd >= 0) && (buf != NULL)) {
        ret = write(fd, buf, size);
    }
    return ret;
}

OTA_WEAK int ota_fread(int fd, void *buf, unsigned int size)
{
    int ret = -1;
    if ((fd >= 0) && (buf != NULL)) {
        ret = read(fd, buf, size);
    }
    return ret;
}

OTA_WEAK int ota_fclose(int fd)
{
    int ret = -1;
    if (fd >= 0) {
        ret = close(fd);
    }
    return ret;
}


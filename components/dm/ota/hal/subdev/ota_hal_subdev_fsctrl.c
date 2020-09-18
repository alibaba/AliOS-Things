#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "ota_log.h"
#include "aos/kernel.h"
#include "aos/hal/flash.h"
#include "ota_import.h"
#include "ota_hal_os.h"
#include "ota/ota_hal.h"

#define SUBDEV_FILE_NAME   "/data/subdev_image.bin"

OTA_WEAK int ota_get_storefile_name(char *filename)
{
    int ret = -1;
    if(filename != NULL) {
        ret = 0;
        strcpy(filename, SUBDEV_FILE_NAME);
    }
    return ret;
}

OTA_WEAK void *ota_fopen(const char *filename, const char *mode)
{
    FILE *fptr = NULL;
    if(filename != NULL && mode != NULL) {
        fptr = fopen(filename, mode);
    }
    return fptr;
}

OTA_WEAK int ota_fwrite(const void *ptr, unsigned int size, unsigned int nmemb, void *stream)
{
    int ret = -1;
    if((ptr != NULL) && (stream != NULL)) {
        ret = fwrite(ptr, size, nmemb, (FILE*)stream);
    }
    return ret;
    
}

OTA_WEAK int ota_fread(const void *ptr, unsigned int size, unsigned int nmemb, void *stream)
{
    int ret = -1;
    if((ptr != NULL) && (stream != NULL)) {
        ret = fread(ptr, size, nmemb, (FILE*)stream);
    }
    return ret;
}

OTA_WEAK int ota_fclose(void *stream)
{
    int ret = -1;
    if(stream != NULL) {
        ret = fclose((FILE*)stream);
    }
    return ret;
}
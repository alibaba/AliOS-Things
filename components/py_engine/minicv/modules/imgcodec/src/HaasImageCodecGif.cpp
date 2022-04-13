
#include "HaasImageCodecGif.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasImageCodecGif::HaasImageCodecGif()
{
    LOG_D("entern\n");
}

HaasImageCodecGif::~HaasImageCodecGif()
{
    LOG_D("entern\n");
}

int HaasImageCodecGif::ImgRead(ImageBuffer_t **image, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecGif::ImgReadMulti(ImageBuffer_t **images, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecGif::ImgWrite(ImageBuffer_t *image, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecGif::ImgWriteMulti(ImageBuffer_t **images, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecGif::ImgDecode(void *addr, ImageBuffer_t **image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

ImageBuffer_t * HaasImageCodecGif::ImgDecode(const char *filename)
{
    LOG_D("entern\n");
    return NULL;
}

int HaasImageCodecGif::ImgEncode(void *addr, ImageBuffer_t **image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecGif::haveImageReader(char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecGif::haveImageWriter(char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

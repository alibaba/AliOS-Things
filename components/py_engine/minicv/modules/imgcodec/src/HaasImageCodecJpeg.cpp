
#include "HaasImageCodecJpeg.h"
#include "HaasLog.h"
#include "HaasErrno.h"

HaasImageCodecJpeg::HaasImageCodecJpeg()
{
    LOG_D("entern\n");
}

HaasImageCodecJpeg::~HaasImageCodecJpeg()
{
    LOG_D("entern\n");
}

int HaasImageCodecJpeg::ImgRead(ImageBuffer_t **image, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecJpeg::ImgReadMulti(ImageBuffer_t **images, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecJpeg::ImgWrite(ImageBuffer_t *image, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecJpeg::ImgWriteMulti(ImageBuffer_t **images, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecJpeg::ImgDecode(void *addr, ImageBuffer_t **image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

ImageBuffer_t * HaasImageCodecJpeg::ImgDecode(const char *filename)
{
    LOG_D("entern\n");
    return NULL;
}

int HaasImageCodecJpeg::ImgEncode(void *addr, ImageBuffer_t **image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecJpeg::haveImageReader(char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecJpeg::haveImageWriter(char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

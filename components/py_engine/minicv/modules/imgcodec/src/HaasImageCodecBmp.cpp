
#include "HaasImageCodecBmp.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "SkImageDecoder.h"
#include "SkStream.h"

extern void sk_init_imagedecoder_bmp();

HaasImageCodecBmp::HaasImageCodecBmp()
{
    LOG_D("entern\n");
}

HaasImageCodecBmp::~HaasImageCodecBmp()
{
    LOG_D("entern\n");
}

int HaasImageCodecBmp::ImgRead(ImageBuffer_t **image, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecBmp::ImgReadMulti(ImageBuffer_t **images, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecBmp::ImgWrite(ImageBuffer_t *image, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecBmp::ImgWriteMulti(ImageBuffer_t **images, char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecBmp::ImgDecode(void *addr, ImageBuffer_t **image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

ImageBuffer_t * HaasImageCodecBmp::ImgDecode(const char *filename)
{
    LOG_D("entern filename = %s;\n", filename);
	SkFILEStream stream(filename);
	if (stream.isValid())
	{
        SkStream* streamBase = &stream;
		sk_init_imagedecoder_bmp();
		SkImageDecoder* codec = SkImageDecoder::Factory(streamBase);
		if (NULL != codec)
		{
			codec->setPreferQualityOverSpeed(false);
			SkBitmap::Config pref = SkBitmap::kRGB_565_Config;
            //LOG_D("call pref = %d;SkBitmap::kRGB_565_Config = %d;\n", pref, SkBitmap::kRGB_565_Config);
            SkBitmap* _skBitmap = new SkBitmap;
			if (!codec->decode(streamBase, _skBitmap, pref, SkImageDecoder::kDecodePixels_Mode))
			{
                LOG_D("call delete _skBitmap\n");
                delete _skBitmap;
                _skBitmap = NULL;
                return NULL;
			}

            void *pixels = _skBitmap->getPixels();
            int32_t width = _skBitmap->width();
            int32_t height = _skBitmap->height();
            LOG_D("bitmap pixels = %p;width = %d; height = %d;\n", pixels, width, height);
			ImageBuffer_t *image = (ImageBuffer_t*)malloc(sizeof(ImageBuffer_t));
			image->format = PIXEL_FMT_RGB565;
			image->width = width;
			image->height = height;
			image->size = width * height *4;
			image->address[0] = (uint8_t*)malloc(image->size);
			memcpy(image->address[0], (uint8_t *)pixels, image->size);

            delete _skBitmap;
            _skBitmap = NULL;
            //LOG_D("call delete codec\n");
			delete codec;
            LOG_D("image = %p;\n", image);
            return image;
		}
	}
    LOG_D("error end filename = %s;\n", filename);
    return NULL;
}

int HaasImageCodecBmp::ImgEncode(void *addr, ImageBuffer_t **image)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecBmp::haveImageReader(char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageCodecBmp::haveImageWriter(char * filename)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

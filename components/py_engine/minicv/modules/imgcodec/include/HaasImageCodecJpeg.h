#ifndef HAAS_IMAGE_CODEC_JPEG_H
#define HAAS_IMAGE_CODEC_JPEG_H

#include "IHaasImageCodec.h"

class HaasImageCodecJpeg : public IHaasImageCodec
{
public:
	HaasImageCodecJpeg();
	virtual ~HaasImageCodecJpeg();

	virtual int ImgRead(ImageBuffer_t **image, char * filename);
	virtual int ImgReadMulti(ImageBuffer_t **images, char * filename);
	virtual int ImgWrite(ImageBuffer_t *image, char * filename);
	virtual int ImgWriteMulti(ImageBuffer_t **images, char * filename);
	virtual int ImgDecode(void *addr, ImageBuffer_t **image);
    virtual ImageBuffer_t * ImgDecode(const char * filename);
	virtual int ImgEncode(void *addr, ImageBuffer_t **image);
	virtual int haveImageReader(char * filename);
	virtual int haveImageWriter(char * filename);
};
#endif

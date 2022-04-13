#ifndef HAAS_IMAGE_CODEC_PNG_H
#define HAAS_IMAGE_CODEC_PNG_H

#include "IHaasImageCodec.h"

class HaasImageCodecPng : public IHaasImageCodec
{
public:
	HaasImageCodecPng();
	virtual ~HaasImageCodecPng();

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

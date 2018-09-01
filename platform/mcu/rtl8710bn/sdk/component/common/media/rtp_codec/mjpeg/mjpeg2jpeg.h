#ifndef MJPEG_2_JPEG_H
#define MJPEG_2_JPEG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef AV_RB16
#   define AV_RB16(x)                           \
    ((((const char*)(x))[0] << 8) |          \
      ((const char*)(x))[1])
#endif

typedef struct AVFrames
{
	char *FrameData;
	int FrameLength;
}AVFrame;

static char *append(char *buf, const char *src, int size);
static char *append_dht_segment(char *buf);
void mjpeg2jpeg(AVFrame *in_frame);



#endif
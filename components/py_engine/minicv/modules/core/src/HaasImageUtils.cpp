#include "HaasImageUtils.h"
#include "HaasLog.h"
#include "HaasErrno.h"

uint32_t ImageUtils::GetSize(PixelFmt_t format, uint32_t width, uint32_t height)
{
	uint32_t baseSize = width * height;
	switch (format) {
	case PIXEL_FMT_YUV420P:
	case PIXEL_FMT_NV12:
	case PIXEL_FMT_NV21:
		return baseSize * 3 / 2;
	case PIXEL_FMT_YUYV422:
		return baseSize * 2;
	case PIXEL_FMT_RGB24:
	case PIXEL_FMT_BGR24:
		return baseSize * 3;
	case PIXEL_FMT_ARGB:
	case PIXEL_FMT_RGBA:
	case PIXEL_FMT_ABGR:
	case PIXEL_FMT_BGRA:
		return baseSize * 4;
	default:
		return STATUS_ERROR;
	}
}

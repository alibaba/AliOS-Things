
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageUtils.h"
#include "ImageCvtColorCpu.h"

ImageCvtColorCpu::ImageCvtColorCpu() {}

ImageCvtColorCpu::~ImageCvtColorCpu() {}

void ImageCvtColorCpu::YuvPixel(
        uint8_t y,
		uint8_t u,
		uint8_t v,
		uint8_t* b,
		uint8_t* g,
		uint8_t* r,
		const struct YuvConstants* yuvconstants) {
   // LOG_I("%s:entern cpu\n", __func__);
	int ub = -yuvconstants->kUVToRB[0];
	int ug = yuvconstants->kUVToG[0];
	int vg = yuvconstants->kUVToG[4];
	int vr = -yuvconstants->kUVToRB[4];
	int bb = yuvconstants->kUVBiasBGR[0];
	int bg = yuvconstants->kUVBiasBGR[1];
	int br = yuvconstants->kUVBiasBGR[2];
	int yg = yuvconstants->kYToRgb[1];

	uint32_t y1 = (uint32_t)(y * 0x0101 * yg) >> 16;
	*b = Clamp((int32_t)(-(u * ub) + y1 + bb) >> 6);
	*g = Clamp((int32_t)(-(u * ug + v * vg) + y1 + bg) >> 6);
	*r = Clamp((int32_t)(-(v * vr) + y1 + br) >> 6);
    //LOG_I("%s:out cpu\n", __func__);
}

void ImageCvtColorCpu::YUY2ToARGBRow_C(
        const uint8_t* src_yuy2,
		uint8_t* rgb_buf,
		const struct YuvConstants* yuvconstants,
		int width) {
    //LOG_I("%s:entern cpu\n", __func__);
	int x;
	for (x = 0; x < width - 1; x += 2) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 0, rgb_buf + 1,
				rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
		YuvPixel(src_yuy2[2], src_yuy2[1], src_yuy2[3], rgb_buf + 4, rgb_buf + 5,
				rgb_buf + 6, yuvconstants);
		rgb_buf[7] = 255;
		src_yuy2 += 4;
		rgb_buf += 8;  // Advance 2 pixels.
	}
	if (width & 1) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 0, rgb_buf + 1,
				rgb_buf + 2, yuvconstants);
		rgb_buf[3] = 255;
	}
    //LOG_I("%s:out cpu\n", __func__);
}

void ImageCvtColorCpu::YUY2ToRGBARow_C(
        const uint8_t* src_yuy2,
		uint8_t* rgb_buf,
		const struct YuvConstants* yuvconstants,
		int width) {
    //LOG_I("%s:entern cpu\n", __func__);
	int x;
	for (x = 0; x < width - 1; x += 2) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 1, rgb_buf + 2,
				rgb_buf + 3, yuvconstants);
		rgb_buf[0] = 255;
		YuvPixel(src_yuy2[2], src_yuy2[1], src_yuy2[3], rgb_buf + 5, rgb_buf + 6,
				rgb_buf + 7, yuvconstants);
		rgb_buf[4] = 255;
		src_yuy2 += 4;
		rgb_buf += 8;  // Advance 2 pixels.
	}
	if (width & 1) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 1, rgb_buf + 2,
				rgb_buf + 3, yuvconstants);
		rgb_buf[0] = 255;
	}
    //LOG_I("%s:out cpu\n", __func__);
}

void ImageCvtColorCpu::YUY2ToBGRRow_C(
        const uint8_t* src_yuy2,
		uint8_t* rgb_buf,
		const struct YuvConstants* yuvconstants,
		int width) {
    //LOG_I("%s:entern cpu\n", __func__);
	int x;
	for (x = 0; x < width - 1; x += 2) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 2, rgb_buf + 1,
				rgb_buf + 0, yuvconstants);
		YuvPixel(src_yuy2[2], src_yuy2[1], src_yuy2[3], rgb_buf + 6, rgb_buf + 5,
				rgb_buf + 4, yuvconstants);
		src_yuy2 += 4;
		rgb_buf += 6;  // Advance 2 pixels.
	}
	if (width & 1) {
		YuvPixel(src_yuy2[0], src_yuy2[1], src_yuy2[3], rgb_buf + 2, rgb_buf + 1,
				rgb_buf + 0, yuvconstants);
	}
    //LOG_I("%s:out cpu\n", __func__);
}

int ImageCvtColorCpu::YUY2ToARGB(
        const uint8_t* src_yuy2,
		int src_stride_yuy2,
		uint8_t* dst_argb,
		int dst_stride_argb,
		int width,
		int height) {
    LOG_I("%s:xxxentern cpu\n", __func__);
	int y;
	if (!src_yuy2 || !dst_argb || width <= 0 || height == 0) {
		return -1;
	}
	// Negative height means invert the image.
	if (height < 0) {
		height = -height;
		src_yuy2 = src_yuy2 + (height - 1) * src_stride_yuy2;
		src_stride_yuy2 = -src_stride_yuy2;
	}
	// Coalesce rows.
	if (src_stride_yuy2 == width * 2 && dst_stride_argb == width * 4) {
		width *= height;
		height = 1;
		src_stride_yuy2 = dst_stride_argb = 0;
	}
	for (y = 0; y < height; ++y) {
		YUY2ToARGBRow_C(src_yuy2, dst_argb, &kYuvI601Constants, width);
		src_yuy2 += src_stride_yuy2;
		dst_argb += dst_stride_argb;
	}
    LOG_I("%s:out cpu\n", __func__);
	return 0;
}

int ImageCvtColorCpu::YUY2ToRGBA(
        const uint8_t* src_yuy2,
		int src_stride_yuy2,
		uint8_t* dst_rgba,
		int dst_stride_rgba,
		int width,
		int height) {
    LOG_I("%s:xxxentern cpu\n", __func__);
	int y;
	if (!src_yuy2 || !dst_rgba || width <= 0 || height == 0) {
		return -1;
	}
	// Negative height means invert the image.
	if (height < 0) {
		height = -height;
		src_yuy2 = src_yuy2 + (height - 1) * src_stride_yuy2;
		src_stride_yuy2 = -src_stride_yuy2;
	}
	// Coalesce rows.
	if (src_stride_yuy2 == width * 2 && dst_stride_rgba == width * 4) {
		width *= height;
		height = 1;
		src_stride_yuy2 = dst_stride_rgba = 0;
	}
	for (y = 0; y < height; ++y) {
		YUY2ToARGBRow_C(src_yuy2, dst_rgba, &kYuvI601Constants, width);
		src_yuy2 += src_stride_yuy2;
		dst_rgba += dst_stride_rgba;
	}
    LOG_I("%s:out cpu\n", __func__);
	return 0;
}

int ImageCvtColorCpu::YUY2ToBGR(
        const uint8_t* src_yuy2,
		int src_stride_yuy2,
		uint8_t* dst_bgr,
		int dst_stride_bgr,
		int width,
		int height) {
    LOG_I("%s:xxxentern cpu\n", __func__);
	int y;
	if (!src_yuy2 || !dst_bgr || width <= 0 || height == 0) {
		return -1;
	}
	// Negative height means invert the image.
	if (height < 0) {
		height = -height;
		src_yuy2 = src_yuy2 + (height - 1) * src_stride_yuy2;
		src_stride_yuy2 = -src_stride_yuy2;
        LOG_I("%s:xxxcpu  1\n", __func__);
	}
	// Coalesce rows.
	if (src_stride_yuy2 == width * 2 && dst_stride_bgr == width * 3) {
		width *= height;
		height = 1;
		src_stride_yuy2 = dst_stride_bgr = 0;
        LOG_I("%s:xxxcpu  2\n", __func__);
	}
	for (y = 0; y < height; ++y) {
		YUY2ToBGRRow_C(src_yuy2, dst_bgr, &kYuvI601Constants, width);
		src_yuy2 += src_stride_yuy2;
		dst_bgr += dst_stride_bgr;
	}
    LOG_I("%s:out cpu\n", __func__);
	return 0;
}

int ImageCvtColorCpu::RGBToBGR(
        const uint8_t* src,
        uint8_t* dst,
        int width,
        int height)
{
    LOG_I("%s:entern cpu\n", __func__);
	int y;
	if (!src || !dst || width <= 0 || height == 0) {
        LOG_I("%s:par is illegal\n", __func__);
		return STATUS_EINVAL;
	}

	int size = width * height *3;
	int i = 0, j = 0;

	for(i=0; i<size; i+=3, j+=3)
	{
		dst[i+2] = src[j];
		dst[i+1] = src[j+1];
		dst[i]   = src[j+2];
	}
    LOG_I("%s:out cpu\n", __func__);
    return STATUS_OK;
}

int ImageCvtColorCpu::RGBAToBGR(
        const uint8_t* src,
        uint8_t* dst,
        int width,
        int height)
{
    LOG_I("%s:entern cpu\n", __func__);
	int y;
	if (!src || !dst || width <= 0 || height == 0) {
        LOG_I("%s:par is illegal\n", __func__);
		return STATUS_EINVAL;
	}

	int size = width * height *3;
	int i = 0, j = 0;

	for(i=0; i<size; i+=3, j+=4)
	{
		dst[i+2] = src[j];
		dst[i+1] = src[j+1];
		dst[i]   = src[j+2];
	}
    LOG_I("%s:out cpu\n", __func__);
    return STATUS_OK;
}

int ImageCvtColorCpu::ARGBToBGR(
        const uint8_t* src,
        uint8_t* dst,
        int width,
        int height)
{
    LOG_I("%s:entern cpu\n", __func__);
	int y;
	if (!src || !dst || width <= 0 || height == 0) {
        LOG_I("%s:par is illegal\n", __func__);
		return STATUS_EINVAL;
	}

	int size = width * height *3;
	int i = 0, j = 0;

	for(i=0; i<size; i+=3, j+=4)
	{
		dst[i+2] = src[j+1];
		dst[i+1] = src[j+2];
		dst[i]   = src[j+3];
	}
    LOG_I("%s:out cpu\n", __func__);
    return STATUS_OK;
}

// Also used for 420
void ImageCvtColorCpu::I422ToARGBRow_C(const uint8_t* src_y,
                     const uint8_t* src_u,
                     const uint8_t* src_v,
                     uint8_t* rgb_buf,
                     const struct YuvConstants* yuvconstants,
                     int width) {
  int x;
  for (x = 0; x < width - 1; x += 2) {
    YuvPixel(src_y[0], src_u[0], src_v[0], rgb_buf + 0, rgb_buf + 1,
             rgb_buf + 2, yuvconstants);
    rgb_buf[3] = 255;
    YuvPixel(src_y[1], src_u[0], src_v[0], rgb_buf + 4, rgb_buf + 5,
             rgb_buf + 6, yuvconstants);
    rgb_buf[7] = 255;
    src_y += 2;
    src_u += 1;
    src_v += 1;
    rgb_buf += 8;  // Advance 2 pixels.
  }
  if (width & 1) {
    YuvPixel(src_y[0], src_u[0], src_v[0], rgb_buf + 0, rgb_buf + 1,
             rgb_buf + 2, yuvconstants);
    rgb_buf[3] = 255;
  }
}

int ImageCvtColorCpu::I420ToARGBMatrix(const uint8_t* src_y,
                     int src_stride_y,
                     const uint8_t* src_u,
                     int src_stride_u,
                     const uint8_t* src_v,
                     int src_stride_v,
                     uint8_t* dst_argb,
                     int dst_stride_argb,
                     const struct YuvConstants* yuvconstants,
                     int width,
                     int height) {
  int y;
  if (!src_y || !src_u || !src_v || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_argb = dst_argb + (height - 1) * dst_stride_argb;
    dst_stride_argb = -dst_stride_argb;
  }

  for (y = 0; y < height; ++y) {
    I422ToARGBRow_C(src_y, src_u, src_v, dst_argb, yuvconstants, width);
    dst_argb += dst_stride_argb;
    src_y += src_stride_y;
    if (y & 1) {
      src_u += src_stride_u;
      src_v += src_stride_v;
    }
  }
  return 0;
}


// Convert I420 to ARGB.
int ImageCvtColorCpu::I420ToARGB(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height) {
  return I420ToARGBMatrix(src_y, src_stride_y, src_u, src_stride_u, src_v,
                          src_stride_v, dst_argb, dst_stride_argb,
                          &kYuvI601Constants, width, height);
}

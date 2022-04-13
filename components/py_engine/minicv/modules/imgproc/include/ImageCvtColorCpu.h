#ifndef HAAS_IMAGE_CVT_COLOR_CPU_H
#define HAAS_IMAGE_CVT_COLOR_CPU_H

#include "HaasCommonImage.h"
#include "ImageColorCommon.h"

class ImageCvtColorCpu
{
    public:
        ImageCvtColorCpu();
        ~ImageCvtColorCpu();
        int YUY2ToARGB(
                const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        int YUY2ToRGBA(
                const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_rgba,
                int dst_stride_rgba,
                int width,
                int height);

        int YUY2ToBGR(
                const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_bgr,
                int dst_stride_bgr,
                int width,
                int height);

        int RGBToBGR(
                const uint8_t* src,
                uint8_t* dst,
                int width,
                int height);

        int RGBAToBGR(
                const uint8_t* src,
                uint8_t* dst,
                int width,
                int height);

        int ARGBToBGR(
                const uint8_t* src,
                uint8_t* dst,
                int width,
                int height);

        int I420ToARGB(
                const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);
    private:
        void YuvPixel(
                uint8_t y,
                uint8_t u,
                uint8_t v,
                uint8_t* b,
                uint8_t* g,
                uint8_t* r,
                const struct YuvConstants* yuvconstants);

        void YUY2ToARGBRow_C(
                const uint8_t* src_yuy2,
                uint8_t* rgb_buf,
                const struct YuvConstants* yuvconstants,
                int width);

        void YUY2ToRGBARow_C(
                const uint8_t* src_yuy2,
                uint8_t* rgb_buf,
                const struct YuvConstants* yuvconstants,
                int width);

        void YUY2ToBGRRow_C(
                const uint8_t* src_yuy2,
                uint8_t* rgb_buf,
                const struct YuvConstants* yuvconstants,
                int width);

        int I420ToARGBMatrix(
                const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        void I422ToARGBRow_C(const uint8_t* src_y,
                const uint8_t* src_u,
                const uint8_t* src_v,
                uint8_t* rgb_buf,
                const struct YuvConstants* yuvconstants,
                int width);

        int32_t clamp0(int32_t v) {
            return -(v >= 0) & v;
        }

        int32_t clamp255(int32_t v) {
            return (-(v >= 255) | v) & 255;
        }

        uint32_t Clamp(int32_t val) {
            int v = clamp0(val);
            return (uint32_t)(clamp255(v));
        }
};
#endif

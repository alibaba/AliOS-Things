
#include "HaasImageProcOpenCV.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageUtils.h"
#include <string>

HaasImageProcOpenCV::HaasImageProcOpenCV()
{
    LOG_D("entern\n");
}

HaasImageProcOpenCV::~HaasImageProcOpenCV()
{
    LOG_D("entern\n");
}

int HaasImageProcOpenCV::Open()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcOpenCV::Close()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcOpenCV::CvtColor(const ImageBuffer_t* src, PixelFmt_t dst_format,
        ImageBuffer_t** dst)
{
    LOG_D("entern\n");
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

    if (dst == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

	int width = src->width;
	int height = src->height;
	PixelFmt_t in_format = src->format;
	PixelFmt_t out_format = dst_format;

	cv::Mat srcImage;
	if (in_format == PIXEL_FMT_YUV420P) {
		srcImage.create(height * 3 / 2, width, CV_8UC1);
		memcpy(srcImage.data, src->address[0], width * height * 3 / 2);
	} else if (in_format == PIXEL_FMT_YUYV422) {
		srcImage.create(height, width, CV_8UC2);
		memcpy(srcImage.data, src->address[0], width * height * 2);
	} else if (in_format == PIXEL_FMT_RGBA) {
		srcImage.create(height, width, CV_8UC4);
		memcpy(srcImage.data, src->address[0], width * height * 4);
	} else if (in_format == PIXEL_FMT_RGB24) {
		srcImage.create(height, width, CV_8UC3);
		memcpy(srcImage.data, src->address[0], width * height * 3);
	} else if (in_format == PIXEL_FMT_BGRA) {
		srcImage.create(height, width, CV_8UC4);
		memcpy(srcImage.data, src->address[0], width * height * 4);
	} else if (in_format == PIXEL_FMT_RGBA) {
		srcImage.create(height, width, CV_8UC4);
		memcpy(srcImage.data, src->address[0], width * height * 4);
	} else {
		return STATUS_EPERM;
	}

	cv::Mat dstImage;
	if (in_format != out_format) {
		switch (out_format) {
		case PIXEL_FMT_YUV420P:
			if (in_format == PIXEL_FMT_YUV422P) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2RGB_Y422);
				cv::cvtColor(dstImage, srcImage, CV_RGB2YUV_I420);
			} else if (in_format == PIXEL_FMT_RGB24) {
				cv::cvtColor(srcImage, dstImage, CV_RGB2YUV_I420);
				dstImage.copyTo(srcImage);
			} else {
				return STATUS_EPERM;
			}
			break;
		case PIXEL_FMT_YUV422P:
			if (in_format == PIXEL_FMT_YUV420P) {
				return STATUS_ENOSUPPORT;
			} else if (in_format == PIXEL_FMT_RGB24) {
				cv::cvtColor(srcImage, dstImage, CV_RGB2YUV_I420);
				dstImage.copyTo(srcImage);
			} else {
				return STATUS_EPERM;
			}
			break;
		case PIXEL_FMT_RGB24:
			if (in_format == PIXEL_FMT_YUV420P) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2RGB_I420);
				dstImage.copyTo(srcImage);
			} else if (in_format == PIXEL_FMT_YUV422P) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2RGB_Y422);
				dstImage.copyTo(srcImage);
			} else if (in_format == PIXEL_FMT_YUYV422) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2RGB_YUYV);
				dstImage.copyTo(srcImage);
            } else if (in_format == PIXEL_FMT_RGBA) {
				cv::cvtColor(srcImage, dstImage, CV_RGBA2RGB);
				dstImage.copyTo(srcImage);
			} else {
				return STATUS_EPERM;
			}
			break;
		case PIXEL_FMT_RGBA:
			if (in_format == PIXEL_FMT_YUV420P) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2RGBA_I420);
				dstImage.copyTo(srcImage);
			} else if (in_format == PIXEL_FMT_YUV422P) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2RGBA_Y422);
				dstImage.copyTo(srcImage);
			} else if (in_format == PIXEL_FMT_YUYV422) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2RGBA_YUYV);
				dstImage.copyTo(srcImage);
			} else if (in_format == PIXEL_FMT_RGB24) {
				cv::cvtColor(srcImage, dstImage, CV_RGB2RGBA);
				dstImage.copyTo(srcImage);
			} else {
				return STATUS_EPERM;
			}
			break;
		case PIXEL_FMT_BGR24:
			if (in_format == PIXEL_FMT_RGBA) {
				cv::cvtColor(srcImage, dstImage, CV_RGBA2BGR);
				dstImage.copyTo(srcImage);
            } else if (in_format == PIXEL_FMT_BGRA) {
				cv::cvtColor(srcImage, dstImage, CV_BGRA2BGR);
				dstImage.copyTo(srcImage);
			} else if (in_format == PIXEL_FMT_YUYV422) {
				cv::cvtColor(srcImage, dstImage, CV_YUV2BGR_YUYV);
				dstImage.copyTo(srcImage);
			} else {
				return STATUS_EPERM;
			}
			break;
		default:
			return STATUS_EPERM;
		}
	} else {
		return STATUS_EPERM;
	}

	*dst = (ImageBuffer_t*)malloc(sizeof(ImageBuffer_t));
	(*dst)->width = width;
	(*dst)->height = height;
	(*dst)->size = ImageUtils::GetSize(out_format, width, height);
	(*dst)->format = out_format;
	(*dst)->address[0] = (uint8_t*)malloc((*dst)->size);
	memcpy((*dst)->address[0], srcImage.data, (*dst)->size);
    return STATUS_OK;
}

int HaasImageProcOpenCV::Resize(const ImageBuffer_t* src, const ImageSize_t dst_size,
        ImageBuffer_t** dst)
{
    LOG_D("entern\n");
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

	int width = src->width;
	int height = src->height;

	cv::Mat srcImage;
	cv::Mat dstImage;
	if (src->format == PIXEL_FMT_YUV420P) {
		srcImage.create(height * 3 / 2, width, CV_8UC1);
		memcpy(srcImage.data, src->address[0], width * height * 3 / 2);
		cv::cvtColor(srcImage, dstImage, CV_YUV2RGB_I420);
		cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
		cv::cvtColor(dstImage, srcImage, CV_RGB2YUV_I420);
	} else if (src->format == PIXEL_FMT_RGB24) {
		dstImage.create(height, width, CV_8UC3);
		memcpy(dstImage.data, src->address[0], width * height * 3);
		srcImage.create(dst_size.width, dst_size.height, CV_8UC3);
		cv::resize(dstImage, srcImage, cv::Size(dst_size.width, dst_size.height));
	} else {
		return STATUS_EPERM;
	}

	*dst = (ImageBuffer_t*)malloc(sizeof(ImageBuffer_t));
	(*dst)->width = dst_size.width;
	(*dst)->height = dst_size.height;
	(*dst)->size = ImageUtils::GetSize(src->format, dst_size.width, dst_size.height);
	(*dst)->format = src->format;
	(*dst)->address[0] = (uint8_t*)malloc((*dst)->size);
	memcpy((*dst)->address[0], srcImage.data, (*dst)->size);
    return STATUS_OK;
}

int HaasImageProcOpenCV::ImgCopy(const ImageBuffer_t* src, ImageBuffer_t** dst)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcOpenCV::Rectangle(const ImageBuffer_t* src, int32_t left, int32_t top,
        int32_t right, int32_t bottom)
{
    LOG_I("%s:entern\n", __func__);
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

	int width = src->width;
	int height = src->height;
    int height_stride = height;
	int src_format = src->format;
    int type = CV_8UC1;
    LOG_I("%s: width = %d;height = %d; format = %d;type = %d;\n", __func__, width, height, src_format, type);

	if (src_format == PIXEL_FMT_YUV420P) {
        height_stride = height * 3 / 2;
        type = CV_8UC1;
	} else if (src_format == PIXEL_FMT_YUYV422) {
        height_stride = height;
        type = CV_8UC2;
	} else if (src_format == PIXEL_FMT_RGBA) {
        height_stride = height;
        type = CV_8UC4;
	} else if (src_format == PIXEL_FMT_RGB24) {
        height_stride = height;
        type = CV_8UC3;
	} else if (src_format == PIXEL_FMT_BGRA) {
        height_stride = height;
        type = CV_8UC4;
	} else {
		return STATUS_EPERM;
	}

    LOG_I("%s:left = %d;top = %d;right = %d;bottom = %d;\n", __func__,left, top, right, bottom);
	cv::Mat srcImage(height_stride, width, type, src->address[0]);
    rectangle(srcImage, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(128), 2, 8, 0);
    LOG_I("%s:out\n", __func__);
    return STATUS_OK;
}

int HaasImageProcOpenCV::Circle(const ImageBuffer_t* src, int32_t center_x, int32_t center_y,
        int32_t radius)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcOpenCV::DrawMarker(const ImageBuffer_t* src, Point_t *pt, int32_t type)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcOpenCV::FillPoly(const ImageBuffer_t* src, Point_t** pts, int32_t color)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasImageProcOpenCV::PutText(const ImageBuffer_t* src, char* text, int32_t left,
        int32_t top)
{
    LOG_I("%s:entern\n", __func__);
    if (src == NULL || src->address[0] == NULL)
    {
        LOG_W("invalid parameter\n");
        return STATUS_EINVAL;
    }

    std::string text_string(text);
	int width = src->width;
	int height = src->height;
    int height_stride = height;
	int src_format = src->format;
    int type = CV_8UC1;
    LOG_I("%s: width = %d;height = %d; format = %d;type = %d;\n", __func__, width, height, src_format, type);
    LOG_I("%s: text = %s;\n", __func__, text);

	if (src_format == PIXEL_FMT_YUV420P) {
        height_stride = height * 3 / 2;
        type = CV_8UC1;
	} else if (src_format == PIXEL_FMT_YUYV422) {
        height_stride = height;
        type = CV_8UC2;
	} else if (src_format == PIXEL_FMT_RGBA) {
        height_stride = height;
        type = CV_8UC4;
	} else if (src_format == PIXEL_FMT_RGB24) {
        height_stride = height;
        type = CV_8UC3;
	} else if (src_format == PIXEL_FMT_BGRA) {
        height_stride = height;
        type = CV_8UC4;
	} else {
		return STATUS_EPERM;
	}

    LOG_I("%s:left = %d;top = %d;\n", __func__,left, top);
	cv::Mat srcImage(height_stride, width, type, src->address[0]);
    putText(srcImage, text_string, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(128), 2, true);
    LOG_I("%s:out\n", __func__);
    return STATUS_OK;
}

int HaasImageProcOpenCV::DilateErode(const ImageBuffer_t* src, ImageBuffer_t* dst,
        int32_t iMode, int32_t iThresh)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

void HaasImageProcOpenCV::ImageAdd(const ImageBuffer_t* src, ImageBuffer_t* added,
        ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcOpenCV::ImageMinus(const ImageBuffer_t* src, ImageBuffer_t* minused,
        ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcOpenCV::ImageElementMultiply(const ImageBuffer_t* src,
        ImageBuffer_t* multiplied, ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

void HaasImageProcOpenCV::ImageElementDivide(const ImageBuffer_t* src,
        ImageBuffer_t* divied, ImageBuffer_t* dst)
{
    LOG_D("entern\n");
}

int HaasImageProcOpenCV::WriteImageToFile(const ImageBuffer_t* src, char* file_name)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

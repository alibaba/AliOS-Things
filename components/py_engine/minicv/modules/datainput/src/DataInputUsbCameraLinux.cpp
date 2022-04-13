#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include "DataInputUsbCameraLinux.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageUtils.h"

DataInputUsbCameraLinux::DataInputUsbCameraLinux()
	: devIndex(0),
	  isRunning(false),
	  devFd(-1),
	  bufferNum(4),
	  bufferList(NULL),
	  imageInfoList(NULL)
{
}

DataInputUsbCameraLinux::~DataInputUsbCameraLinux()
{

}

int DataInputUsbCameraLinux::Open(int cameraNum)
{
	char path[256];
	int ret;

	sprintf(path, "/dev/video%d", cameraNum);
	ret = open(path, O_RDWR|O_NONBLOCK, 0);
	if (-1 == ret) {
		LOG_E("Failed to open %s\n", path);
		return STATUS_ENODEV;
	}

	devIndex = cameraNum;
	devName = path;
	devFd = ret;
	return InitDevice();
}

int DataInputUsbCameraLinux::Close()
{
    LOG_I("entern\n");
	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EBADF;
	}

	UninitDevice();
	close(devFd);
	devFd = -1;

    LOG_I("out\n");
	return STATUS_OK;
}

int DataInputUsbCameraLinux::Start()
{
	uint32_t i;
	enum v4l2_buf_type type;
	int ret;

	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EBADF;
	}

	if (NULL == bufferList) {
		ret = InitBuffer();
		if (ret != 0)
			return ret;
	}

	for (i = 0; i < bufferNum; i++) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == ioctl(devFd, VIDIOC_QBUF, &buf)) {
			LOG_E("VIDIOC_QBUF (%d) Failed\n", i);
			return STATUS_EBUSY;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(devFd, VIDIOC_STREAMON, &type)) {
		LOG_E("VIDIOC_STREAMON Failed\n");
		return STATUS_EAGAIN;
	}
	isRunning = true;

	return STATUS_OK;
}

int DataInputUsbCameraLinux::Stop()
{
	enum v4l2_buf_type type;

	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EBADF;
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(devFd, VIDIOC_STREAMOFF, &type)) {
		LOG_E("VIDIOC_STREAMOFF Failed\n");
		return STATUS_EBUSY;
	}

	isRunning = false;

	return STATUS_OK;
}

int DataInputUsbCameraLinux::GetCapability(UsbCameraCap_t *cap)
{
	//cap->resolution = VGA;
	cap->format = PIXEL_FMT_YUYV422;
	return 0;
}

int DataInputUsbCameraLinux::SetResolution(ImageSize_t size)
{
	if (3840 <= size.width || size.width < 0 || 2160 <= size.height || size.height <= 0)//limit:4K
		return STATUS_EPERM;

	struct v4l2_frmsizeenum frmsize;
	struct v4l2_format v4lfmt;
	bool isSizeSupported = false;

	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EBADF;
	}

	// verify resolution
	memset(&frmsize, 0, sizeof(frmsize));
	frmsize.index = 0;
	frmsize.pixel_format = GetV4l2PixFmt(devPixFmt);

	while (ioctl(devFd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0) {
		if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {

			if ((frmsize.discrete.width == size.width) &&
				(frmsize.discrete.height == size.height)) {
				isSizeSupported = true;
				break;
			}
		}
		frmsize.index++;
	}

	if (!isSizeSupported) {
		return STATUS_EAGAIN;
	}

	// set new resolution
	memset(&v4lfmt, 0, sizeof(v4lfmt));
	v4lfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(devFd, VIDIOC_G_FMT, &v4lfmt)) {
		LOG_E("VIDIOC_G_FMT Failed\n");
		return STATUS_EAGAIN;
	}

	v4lfmt.fmt.pix.width = size.width;
	v4lfmt.fmt.pix.height = size.height;

	if (-1 == ioctl(devFd, VIDIOC_S_FMT, &v4lfmt)) {
		LOG_E("VIDIOC_S_FMT Failed\n");
		return STATUS_EAGAIN;
	}

	devWidth = size.width;
	devHeight = size.height;
	return STATUS_OK;
}

int DataInputUsbCameraLinux::SetFrameRate(uint32_t rate)
{
	if (rate <= 0)
		return STATUS_EPERM;

	struct v4l2_frmivalenum fival;
	struct v4l2_streamparm fpsparam;
	bool isSupported = false;

	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EBADF;
	}

	// verify frame rate
	memset(&fival, 0, sizeof(fival));
	fival.index = 0;
	fival.pixel_format = GetV4l2PixFmt(devPixFmt);
	fival.width = devWidth;
	fival.height = devHeight;

	while (ioctl(devFd, VIDIOC_ENUM_FRAMEINTERVALS, &fival) == 0) {
		if (fival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {

			LOG_D("discrete frame rate: %d / %d\n",
					fival.discrete.numerator,
					fival.discrete.denominator);
			if (rate == (fival.discrete.denominator / fival.discrete.numerator)) {
				isSupported = true;
				break;
			}
		}
		fival.index++;
	}

	if (!isSupported) {
		LOG_W("fps %d not supported\n", rate);
		return STATUS_EAGAIN;
	}

	// set new frame rate
	memset(&fpsparam, 0, sizeof(fpsparam));
	fpsparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fpsparam.parm.capture.timeperframe.numerator = 1;
	fpsparam.parm.capture.timeperframe.denominator = rate;

	if (-1 == ioctl(devFd, VIDIOC_S_PARM, &fpsparam)) {
		LOG_E("VIDIOC_S_PARM Failed\n");
		return STATUS_EAGAIN;
	}

	devFrameRate = rate;
	return STATUS_OK;
}

int DataInputUsbCameraLinux::SetPixelFormat(PixelFmt_t fmt)
{
	struct v4l2_format v4lfmt;
	struct v4l2_fmtdesc fmtdesc;
	uint32_t pixelformat;
	bool isFmtSupported = false;

	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EAGAIN;
	}

	pixelformat = GetV4l2PixFmt(fmt);

	// verify pixel format
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index = 0;
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while (ioctl(devFd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
		if (pixelformat == fmtdesc.pixelformat) {
			isFmtSupported = true;
			break;
		}

		fmtdesc.index++;
	}

	if (!isFmtSupported) {
		return STATUS_EAGAIN;
	}

	// set pixel format
	memset(&v4lfmt, 0, sizeof(v4lfmt));
	v4lfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(devFd, VIDIOC_G_FMT, &v4lfmt) != 0) {
		LOG_E("VIDIOC_G_FMT Failed\n");
		return STATUS_EAGAIN;
	}

	v4lfmt.fmt.pix.pixelformat = pixelformat;

	if (ioctl(devFd, VIDIOC_S_FMT, &v4lfmt) != 0) {
		LOG_E("VIDIOC_S_FMT Failed\n");
		return STATUS_EAGAIN;
	}

	devPixFmt = fmt;
	return STATUS_OK;
}

int DataInputUsbCameraLinux::RequestImage(ImageBuffer_t** image, int32_t timeout)
{
	fd_set fds;
	struct timeval tv;
	int ret;
	struct v4l2_buffer buf;
	uint32_t index;

    LOG_D("entern\n");
	if (image == NULL) {
		LOG_W("invalid parameter\n");
		return STATUS_EINVAL;
	}
	*image = NULL;

	if (!isRunning) {
		LOG_W("device has been stopped\n");
		return STATUS_EPERM;
	}

	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EBADF;
	}

	FD_ZERO(&fds);
	FD_SET(devFd, &fds);

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	ret = select(devFd + 1, &fds, NULL, NULL, &tv);
	if (-1 == ret) {
		LOG_E("select failed\n");
		return STATUS_EAGAIN;
	}

	if (0 == ret) {
		LOG_W("select timeout\n");
		return STATUS_ETIMEDOUT;
	}

	// read frame
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(devFd, VIDIOC_DQBUF, &buf)) {
		LOG_E("VIDIOC_DQBUF Failed\n");
		return STATUS_EAGAIN;
	}

	if (image != NULL) {
		index = buf.index;

		imageInfoList[index].size = ImageUtils::GetSize(devPixFmt, devWidth, devHeight);
		imageInfoList[index].width = devWidth;
		imageInfoList[index].height = devHeight;
		imageInfoList[index].format = devPixFmt;
		imageInfoList[index].address[0] = reinterpret_cast<uint8_t *>(bufferList[index].start);

		*image = &imageInfoList[index];
        LOG_D("&image = %p;\n", &image);
	}

	return STATUS_OK;
}

int DataInputUsbCameraLinux::ReleaseImage(ImageBuffer_t* image)
{
	struct v4l2_buffer buf;
	uint32_t i;
	uint32_t index;

    LOG_I("entern\n");
	if (image == NULL) {
		LOG_W("invalid parameter\n");
		return STATUS_EINVAL;
	}

	if (bufferList == NULL) {
		LOG_W("bufferList has been freed\n");
		return STATUS_EINVAL;
	}

	if (devFd == -1) {
		LOG_W("device not open\n");
		return STATUS_EBADF;
	}

	for (i = 0; i < bufferNum; i++) {
		if (bufferList[i].start == image->address[0]) {
			index = i;
			break;
		}
	}

	if (i >= bufferNum) {
		LOG_E("no buffer found\n");
		return STATUS_EAGAIN;
	}

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = index;

	if (-1 == ioctl(devFd, VIDIOC_QBUF, &buf)) {
		LOG_E("VIDIOC_QBUF (%d) Failed\n", buf.index);
		return STATUS_EAGAIN;
	}

    LOG_I("out\n");
	return STATUS_OK;
}

int DataInputUsbCameraLinux::InitDevice()
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format v4lfmt;
	unsigned int min;

	// verify capability

	if (-1 == ioctl(devFd, VIDIOC_QUERYCAP, &cap)) {
		LOG_E("%s not V4L2 device\n", devName.c_str());
		return STATUS_EPERM;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		LOG_E("%s not video capture device\n", devName.c_str());
		return STATUS_EPERM;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		LOG_E("%s not support streaming IO\n", devName.c_str());
		return STATUS_EPERM;
	}

	// set crop area
	memset(&cropcap, 0, sizeof(cropcap));
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == ioctl(devFd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; // default crop rect

		if (-1 == ioctl(devFd, VIDIOC_S_CROP, &crop)) {
			LOG_I("VIDIOC_S_CROP not supported\n");
			// error ignored
		}
	}

	// trying to configure pixel format as YUV420P
	SetPixelFormat(PIXEL_FMT_YUV420P);

	// verify pixel format
	memset(&v4lfmt, 0, sizeof(v4lfmt));
	v4lfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(devFd, VIDIOC_G_FMT, &v4lfmt)) {
		LOG_E("VIDIOC_G_FMT Failed\n");
		return STATUS_EPERM;
	}

	switch (v4lfmt.fmt.pix.pixelformat)
	{
	case V4L2_PIX_FMT_YUYV:
		devPixFmt = PIXEL_FMT_YUYV422;
		break;
	case V4L2_PIX_FMT_YUV422P:
		devPixFmt = PIXEL_FMT_YUV422P;
		break;
	case V4L2_PIX_FMT_YUV420:
		devPixFmt = PIXEL_FMT_YUV420P;
		break;
	default:
		LOG_W("Unsupported pixel format '%c%c%c%c'\n",
			v4lfmt.fmt.pix.pixelformat & 0xFF,
			(v4lfmt.fmt.pix.pixelformat >> 8) & 0xFF,
			(v4lfmt.fmt.pix.pixelformat >> 16) & 0xFF,
			(v4lfmt.fmt.pix.pixelformat >> 24) & 0xFF);

		LOG_W("trying YUYV422...\n");
		if (SetPixelFormat(PIXEL_FMT_YUYV422) != 0) {
			LOG_E("Unsupported pixel format\n");
			return STATUS_EINVAL;
		}
		LOG_W("OK\n");
	}

	min = v4lfmt.fmt.pix.width * 2;
	if (v4lfmt.fmt.pix.bytesperline < min)
		v4lfmt.fmt.pix.bytesperline = min;

	min = v4lfmt.fmt.pix.bytesperline * v4lfmt.fmt.pix.height;
	if (v4lfmt.fmt.pix.sizeimage < min)
		v4lfmt.fmt.pix.sizeimage = min;

	devWidth = v4lfmt.fmt.pix.width;
	devHeight = v4lfmt.fmt.pix.height;

	// get current frame rate
	struct v4l2_streamparm fpsparam;
	memset(&fpsparam, 0, sizeof(fpsparam));
	fpsparam.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(devFd, VIDIOC_G_PARM, &fpsparam) == 0) {
		int numerator = fpsparam.parm.capture.timeperframe.numerator;
		int denominator = fpsparam.parm.capture.timeperframe.denominator;
		LOG_D("frame interval: %d / %d\n", numerator, denominator);
		devFrameRate = denominator / numerator;
	} else {
		LOG_W("Failed to get fps\n");
		devFrameRate = 30;
	}

	return STATUS_OK;
}

int DataInputUsbCameraLinux::InitBuffer()
{
	struct v4l2_requestbuffers req;
	uint32_t index;
	uint32_t i;
	int ret = 0;

	memset(&req, 0, sizeof(req));
	req.count = bufferNum;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if ((-1 == ioctl(devFd, VIDIOC_REQBUFS, &req)) || (req.count < 2)) {
		LOG_E("VIDIOC_REQBUFS Failed\n");
		return STATUS_ENOSPC;
	}

	bufferList = (UsbCameraBuffer *)calloc(req.count, sizeof(*bufferList));
	if (!bufferList) {
		LOG_E("Out of memory\n");
		return STATUS_ENOMEM;
	}
	memset(bufferList, 0, req.count * sizeof(*bufferList));

	for (index = 0; index < req.count; index++) {
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(buf));
		buf.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory	= V4L2_MEMORY_MMAP;
		buf.index	= index;

		if (-1 == ioctl(devFd, VIDIOC_QUERYBUF, &buf)) {
			LOG_E("VIDIOC_QUERYBUF (%d) Failed\n", index);
			ret = STATUS_EBUSY;
			goto free_bufferlist;
		}

		bufferList[index].length = buf.length;
		bufferList[index].offset = buf.m.offset;
		bufferList[index].start =
				mmap(NULL, buf.length,
					PROT_READ | PROT_WRITE,
					MAP_SHARED,
					devFd, buf.m.offset);

		if (MAP_FAILED == bufferList[index].start) {
			LOG_E("Failed to map memory for buffer %d\n", index);
			ret = STATUS_ENOSPC;
			goto unmap_bufferlist;
		}
	}

	imageInfoList = (ImageBuffer_t *)calloc(req.count, sizeof(*imageInfoList));
	if (!imageInfoList) {
		LOG_E("Out of memory\n");
		ret = STATUS_ENOMEM;
		goto unmap_bufferlist;
	}
	memset(imageInfoList, 0, req.count * sizeof(*imageInfoList));

	return STATUS_OK;

unmap_bufferlist:
	for (i = 0; i < bufferNum; i++) {
		if ((bufferList[i].start != MAP_FAILED) &&
			(bufferList[i].start != NULL)) {
			munmap(bufferList[i].start, bufferList[i].length);
			bufferList[i].start = NULL;
		}
	}

free_bufferlist:
	if (bufferList != NULL) {
		free(bufferList);
		bufferList = NULL;
	}

	return ret;
}

int DataInputUsbCameraLinux::UninitDevice()
{
	UninitBuffer();
	return STATUS_OK;
}

int DataInputUsbCameraLinux::UninitBuffer()
{
	uint32_t i;

	if (NULL == bufferList)
		return STATUS_OK;

	for (i = 0; i < bufferNum; i++) {
		if ((bufferList[i].start != MAP_FAILED) &&
			(bufferList[i].start != NULL)) {
			munmap(bufferList[i].start, bufferList[i].length);
			bufferList[i].start = NULL;
		}
	}

	if (bufferList != NULL) {
		free(bufferList);
		bufferList = NULL;
	}

	if (imageInfoList != NULL) {
		free(imageInfoList);
		imageInfoList = NULL;
	}

	return STATUS_OK;
}

uint32_t DataInputUsbCameraLinux::GetV4l2PixFmt(PixelFmt_t fmt)
{
	uint32_t pixelformat;

	switch (fmt)
	{
	case PIXEL_FMT_YUV422P:
		pixelformat = V4L2_PIX_FMT_YUV422P;
		break;
	case PIXEL_FMT_YUV420P:
		pixelformat = V4L2_PIX_FMT_YUV420;
		break;
	case PIXEL_FMT_YUYV422:
		pixelformat = V4L2_PIX_FMT_YUYV;
		break;
	case PIXEL_FMT_RGB24:
		pixelformat = V4L2_PIX_FMT_RGB24;
		break;
	default:
		pixelformat = 0;
	}

	return pixelformat;
}

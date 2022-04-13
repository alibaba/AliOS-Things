
#include "HaasDataInput.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#ifdef SYSTEM_OS_LINUX
#include "HaasDataInputCameraUsb.h"
#include "HaasDataInputVideoFile.h"
#endif
#include "HaasDataInputCameraIp.h"
#include "HaasDataInputCameraLocal.h"
#include "HaasDataInputImageBmp.h"
#include "HaasDataInputImageGif.h"
#include "HaasDataInputImageJpg.h"
#include "HaasDataInputImagePng.h"
#include "HaasDataInputVideoRtmp.h"
#include "HaasDataInputVideoRtsp.h"

extern "C" {
    IHaasDataInput* IHaasDataInputInstance(DataInputType_t type)
    {
        IHaasDataInput* mIHaasDataInput = NULL;
        LOG_D("entern type = %d;\n", type);
        switch (type)
        {
            case DATA_INPUT_SOURCE_IMAGE_BMP:
                mIHaasDataInput = new HaasDataInputImageBmp();
				break;
            case DATA_INPUT_SOURCE_IMAGE_GIF:
                mIHaasDataInput = new HaasDataInputImageGif();
				break;
            case DATA_INPUT_SOURCE_IMAGE_PNG:
                mIHaasDataInput = new HaasDataInputImagePng();
				break;
            case DATA_INPUT_SOURCE_IMAGE_JPG:
                mIHaasDataInput = new HaasDataInputImageJpg();
				break;
            case DATA_INPUT_SOURCE_VIDEO_FILE:
                #ifdef SYSTEM_OS_LINUX
                mIHaasDataInput = new HaasDataInputVideoFile();
                #endif
				break;
            case DATA_INPUT_SOURCE_VIDEO_RTMP:
                mIHaasDataInput = new HaasDataInputVideoRtmp();
				break;
            case DATA_INPUT_SOURCE_VIDEO_RTSP:
                mIHaasDataInput = new HaasDataInputVideoRtsp();
				break;
            case DATA_INPUT_SOURCE_CAMERA_USB:
                #ifdef SYSTEM_OS_LINUX
                mIHaasDataInput = new HaasDataInputCameraUsb();
                #endif
				break;
            case DATA_INPUT_SOURCE_CAMERA_LOCAL:
                mIHaasDataInput = new HaasDataInputCameraLocal();
				break;
            case DATA_INPUT_SOURCE_CAMERA_IP:
                mIHaasDataInput = new HaasDataInputCameraIp();
				break;
			default:
                LOG_D("illegal type = %d;\n", type);
                mIHaasDataInput = NULL;
        }
        LOG_D("out type = %d;\n", type);
        return mIHaasDataInput;
    }
}

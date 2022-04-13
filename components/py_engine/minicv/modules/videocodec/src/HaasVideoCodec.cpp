
#include "HaasVideoCodec.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#ifdef SYSTEM_OS_LINUX
#include "HaasVideoCodecFFmpeg.h"
#endif
#include "HaasVideoCodecHal.h"

extern "C" {
    IHaasVideoCodec* IHaasVideoCodecInstance(VideoCodecType_t type)
    {
        LOG_D("entern type = %d;\n", type);
        IHaasVideoCodec* mIHaasVideoCodec = NULL;
        switch (type)
        {
            case VIDEO_CODEC_FFMPEG:
#ifdef SYSTEM_OS_LINUX
                mIHaasVideoCodec = new HaasVideoCodecFFmpeg();
#endif
                break;
            case VIDEO_CODEC_HAL:
                mIHaasVideoCodec = new HaasVideoCodecHal();
                break;
            default:
                LOG_D("illegal type = %d;\n", type);
                mIHaasVideoCodec = NULL;
        }
        LOG_D("out type = %d;\n", type);
        return mIHaasVideoCodec;
    }
}

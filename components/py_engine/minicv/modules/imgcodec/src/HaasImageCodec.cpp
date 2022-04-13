
#include "HaasImageCodec.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageCodecBmp.h"
#include "HaasImageCodecGif.h"
#include "HaasImageCodecJpeg.h"
#include "HaasImageCodecPng.h"

extern "C" {
    IHaasImageCodec* IHaasImageCodecInstance(CodecImageType_t type)
    {
        LOG_D("entern\n");
        IHaasImageCodec* mIHaasImageCodec = NULL;
        LOG_D("entern type = %d;\n", type);
        switch (type)
        {
            case CODEC_IMAG_SOURCE_IMAGE_BMP:
                mIHaasImageCodec = new HaasImageCodecBmp();
                break;
            case CODEC_IMAG_SOURCE_IMAGE_PNG:
                mIHaasImageCodec = new HaasImageCodecPng();
                break;
            case CODEC_IMAG_SOURCE_IMAGE_JPG:
                mIHaasImageCodec = new HaasImageCodecJpeg();
                break;
            case CODEC_IMAG_SOURCE_IMAGE_GIF:
                mIHaasImageCodec = new HaasImageCodecGif();
                break;
            default:
                LOG_D("illegal type = %d;\n", type);
                mIHaasImageCodec = NULL;
        }
        LOG_D("out type = %d;\n", type);
        return mIHaasImageCodec;
    }
}

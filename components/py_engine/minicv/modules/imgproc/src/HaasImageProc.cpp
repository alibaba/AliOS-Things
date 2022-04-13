
#include "HaasImageProc.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "HaasImageProcCpu.h"
#include "HaasImageProcHal.h"
#ifdef ENABLE_OPENCV
#include "HaasImageProcOpenCV.h"
#endif

extern "C" {
    IHaasImageProc* IHaasImageProcInstance(ImageProcType_t type)
    {
        IHaasImageProc* mIHaasImageProc = NULL;
        LOG_D("entern type = %d;\n", type);
        switch (type)
        {
            case IMAGE_PROC_CPU:
                mIHaasImageProc = new HaasImageProcCpu();
				break;
#ifdef ENABLE_OPENCV
            case IMAGE_PROC_OPENCV:
                mIHaasImageProc = new HaasImageProcOpenCV();
                break;
#endif
            case IMAGE_PROC_HAL:
                mIHaasImageProc = new HaasImageProcHal();
				break;
			default:
                LOG_D("illegal type = %d;\n", type);
                mIHaasImageProc = NULL;
        }
        LOG_D("out type = %d;\n", type);
        return mIHaasImageProc;
    }
}

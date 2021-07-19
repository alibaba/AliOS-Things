
#include "base/modules/ml/include/HaasML.h"
#include "ulog/ulog.h"
#include "base/modules/core/include/HaasErrno.h"
#include "base/modules/ml/include/HaasMLCloud.h"
#include "base/modules/ml/include/HaasMLMnn.h"

#define LOG_TAG "HAASML"

extern "C" {
    IHaasML* IHaasMLInstance(MLEngineType_t type)
    {
        LOGD(LOG_TAG, "entern type = %d;\n", type);
        IHaasML* mIHaasML = NULL;
        switch (type)
        {
            case ML_ENGINE_MNN:
                LOGD(LOG_TAG, "HaasMLMnn type = %d;\n", type);
                mIHaasML = new HaasMLMnn();
                break;
            case ML_ENGINE_CLOUD:
                LOGD(LOG_TAG, "HaasMLCloud type = %d;\n", type);
                mIHaasML = new HaasMLCloud();
                break;
            case ML_ENGINE_ODLA:
                LOGD(LOG_TAG, "HaasMLOlda type = %d;\n", type);
                //mIHaasML = new HaasMLOlda();
                mIHaasML = NULL;
                break;
            default:
                LOGD(LOG_TAG, "illegal type = %d;\n", type);
                mIHaasML = NULL;
        }
        LOGD(LOG_TAG, "out type = %d;\n", type);
        return mIHaasML;
    }
}

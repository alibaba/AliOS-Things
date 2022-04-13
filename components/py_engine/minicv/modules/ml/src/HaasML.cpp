
#include "HaasML.h"
#include "HaasLog.h"
#include "HaasErrno.h"
//#include "HaasMLOlda.h"
#include "HaasMLCloud.h"
#include "HaasMLMnn.h"

extern "C" {
    IHaasML* IHaasMLInstance(MLEngineType_t type)
    {
        LOG_D("entern type = %d;\n", type);
        IHaasML* mIHaasML = NULL;
        switch (type)
        {
            case ML_ENGINE_MNN:
                LOG_D("HaasMLMnn type = %d;\n", type);
                mIHaasML = new HaasMLMnn();
                break;
            case ML_ENGINE_CLOUD:
                LOG_D("HaasMLCloud type = %d;\n", type);
                mIHaasML = new HaasMLCloud();
                break;
            case ML_ENGINE_ODLA:
                LOG_D("HaasMLOlda type = %d;\n", type);
                //mIHaasML = new HaasMLOlda();
                mIHaasML = NULL;
                break;
            default:
                LOG_D("illegal type = %d;\n", type);
                mIHaasML = NULL;
        }
        LOG_D("out type = %d;\n", type);
        return mIHaasML;
    }
}

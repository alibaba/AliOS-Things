
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef IHAAS_ML_H
#define IHAAS_ML_H
#include "base/include/base/HaasCommonImage.h"
#include "base/include/base/HaasMLDef.h"

/**
* @brief Machine Learning Control Class
* @author HaasAI Group
*/
class IHaasML
{
public:
    virtual ~IHaasML() {}

    /**
        * @brief        config http info(for ucloud AI)
        * @param[in]    key : OSS Access Key
        * @param[in]    secret : OSS Access secret
        * @param[in]    endpoint : The region id
        * @param[in]    bucket : OSS endpoint
        * @note.
        * @return
        *     0 successfully \n
        *    -1 failed \n
        * @par Sample
        * @code
        *    IHaasML * mIHaasML = NULL;
        *    mIHaasML = IHaasMLInstance();
        *    res_ res = 0;
        *   res = mIHaasML->Config(NULL,NULL,NULL,NULL,NULL);
        * @endcode
        */
    virtual int Config(char *key, char *secret, char *endpoint,
            char *bucket) { return 0; }
    /**
        * @brief        set input data(come frome HaasDataInput)
        * @param[in]    dataPath : The path of DataSource
        * @note    Notice that the dataPath is the full path.
        * @return
        *     0 successfully \n
        *    -1 failed \n
        * @par Sample
        * @code
        *    IHaasML * mIHaasML = NULL;
        *    mIHaasML = IHaasMLInstance();
        *    char* dataPath = "/data/HaasAI/xxx"
        *    res_ res = 0;
        *   res = mIHaasML->SetInputData(dataPath.c_str());
        * @endcode
        */
    virtual int SetInputData(const char *dataPath, const char *compareDataPath) = 0;
    /**
        * @brief        Load AI mode object
        * @param[in]    modePath : The path of AI Mode
        * @note    Notice that the modePath is the full path.
        * @return
        *     0 successfully \n
        *    -1 failed \n
        * @par Sample
        * @code
        *    IHaasML * mIHaasML = NULL;
        *    mIHaasML = IHaasMLInstance();
        *    char* modePath = "/data/HaasAI/aimode"
        *    res_ res = 0;
        *   res = mIHaasML->LoadNet(modePath.c_str());
        * @endcode
        */
    virtual int LoadNet(const char* modePath) = 0;
    /**
        * @brief        Start AI Predict
        * @note
        * @return
        *     0 successfully \n
        *    -1 failed \n
        * @par Sample
        * @code
        *    IHaasML * mIHaasML = NULL;
        *    mIHaasML = IHaasMLInstance();
        *    res_ res = 0;
        *   res = mIHaasML->Predict();
        * @endcode
        */
    virtual int Predict() = 0;
    /**
        * @brief        Get AI Predict Result
        * @param[out]    outResult : The result of Predict
        * @param[int]    len : The len of readBuffer
        * @note
        * @return
        *     0 successfully \n
        *    -1 failed \n
        * @par Sample
        * @code
        *    IHaasML * mIHaasML = NULL;
        *    mIHaasML = IHaasMLInstance();
        *    int res = 0;
        *      char *outResult = NULL;
        *    int len;
        *   res = mIHaasML->GetPredictResponses(&outResult, &len);
        * @endcode
        */
    virtual int GetPredictResponses(char* outResult, int len) = 0;
    /**
        * @brief        un load AI Predict mode obj
        * @note
        * @return
        *     0 successfully \n
        *    -1 failed \n
        * @par Sample
        * @code
        *    IHaasML * mIHaasML = NULL;
        *    mIHaasML = IHaasMLInstance();
        *    res_ res = 0;
        *   res = mIHaasML->UnLoadNet();
        * @endcode
        */
    virtual int UnLoadNet() = 0;
};

extern "C" {
    /**
        * @brief        Get HaasML Instance
        * @note            NULL
        * @return
        *     !0  successfully \n
        *    NULL failed \n
        * @par Sample
        * @code
        *    IHaasML * mHaasML = NULL;
        *    mHaasML = IHaasMLInstance();
        * @endcode
        */
IHaasML* IHaasMLInstance(MLEngineType_t type);
}

#endif

#ifndef HAAS_ML_CLOUD_H
#define HAAS_ML_CLOUD_H
#include "base/include/IHaasML.h"
#include <string>
#include "ucloud_ai_common.h"
#include "ucloud_ai_objectdet.h"
#include "ucloud_ai_facebody.h"
#include "ucloud_ai_imagerecog.h"
#include "ucloud_ai_ocr.h"

class HaasMLCloud : public IHaasML
{
public:
    HaasMLCloud();
    virtual ~HaasMLCloud();

    virtual int Config(char *key, char *secret, char *endpoint,
            char *bucket);
    virtual int SetInputData(const char *dataPath, const char *compareDataPath);
    virtual int LoadNet(const char *modePath);
    virtual int Predict();
    virtual int GetPredictResponses(char *outResult, int len);
    virtual int UnLoadNet();
private:

    int PredictObjectDet();
    int PredictFacebodyComparing();
    int PredictAnimeStyle();
    int PredictRecognizeExpression();
    int PredictRecognizeCharacter();
    int PredictDetectPedestrian();
    int PredictDetectFruits();
    static int ObjectDetectCallback(ucloud_ai_result_t *result);
    static int AnimeStyleCallback(ucloud_ai_result_t *result);
    static int RecognizeExpressionCallback(ucloud_ai_result_t *result);
    static int FacebodyComparingCallback(ucloud_ai_result_t *result);
    static int RecognizeCharacterCallback(ucloud_ai_result_t *result);
    static int DetectPedestrianCallback(ucloud_ai_result_t *result);
    static int DetectFruitsCallback(ucloud_ai_result_t *result);
    char * mDataPath;
    char * mAiMode;
    std::string mFacePath;
};
#endif

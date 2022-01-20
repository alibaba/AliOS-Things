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
    int PredictRecognizeLicensePlate();
    static int ObjectDetectCallback(void *result);
    static int AnimeStyleCallback(void *result);
    static int RecognizeExpressionCallback(void *result);
    static int FacebodyComparingCallback(void *result);
    static int RecognizeCharacterCallback(void *result);
    static int DetectPedestrianCallback(void *result);
    static int DetectFruitsCallback(void *result);
    static int RecognizeLicensePlateCallback(void *result);
    char * mDataPath;
    char * mAiMode;
    std::string mFacePath;
};
#endif

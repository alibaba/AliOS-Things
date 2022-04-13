#ifndef HAAS_ML_CLOUD_H
#define HAAS_ML_CLOUD_H
#include "IHaasML.h"
#include "objectdet.h"
#include "facebody.h"
#include "ocr.h"
#include <httpclient.h>
#include <string>

#define HEAD_SIZE 2048
#define BODY_SZIE 1024*128+1
#if 0
#define OSS_ACCESS_KEY "LTAI4Fz4QLXLVzyPTMNJEyV3"
#define OSS_ACCESS_SECRET "XCKK9mPRDVYCwmm2AEtrGqChl81Wh7"
#define OSS_ENDPOINT "oss-cn-shanghai.aliyuncs.com"
#define OSS_BUCKET "oss-ai-dev-one"
#define ML_MYFACE_PATH "https://oss-ai-dev-one.oss-cn-shanghai.aliyuncs.com/data/myface.jpg"
#endif

#define ML_CLOUD_AI_REGION_ID "cn-shanghai"
#define ML_CLOUD_AI_FACEBODY_ENDPOINT "facebody.cn-shanghai.aliyuncs.com"
#define ML_CLOUD_AI_OBJECTDET_ENDPOINT "objectdet.cn-shanghai.aliyuncs.com"
#define ML_CLOUD_AI_OCR_ENDPOINT "ocr.cn-shanghai.aliyuncs.com"
class HaasMLCloud : public IHaasML
{
public:
	HaasMLCloud();
	virtual ~HaasMLCloud();

	virtual int Config(char *key, char *secret, char *region_id,
            char *endpoint, char *url);
	virtual int SetInputData(const char* dataPath);
	virtual int LoadNet(const char* modePath);
	virtual int Predict();
	virtual int GetPredictResponses(char* outResult, int len);
	virtual int UnLoadNet();
    httpclient_t* GetAiClient(){ return &ai_client;}
    httpclient_data_t* GetAiClientData(){ return &ai_client_data;}
private:

    int PredictObjectDet();
    int PredictFacebodyComparing();
    int PredictAnimeStyle();
    int PredictRecognizeExpression();
    int PredictRecognizeCharacter();
    static int ObjectDetectCallback(char *type, float score, object_rect_t *rect);
    static int AnimeStyleCallback(char *out_url, void* data);
    static int RecognizeExpressionCallback(char *expression, float face_probability, face_rect_t *face_rect);
    static int FacebodyComparingCallback(float confidence, face_rect_t *face_rect);
    static int RecognizeCharacterCallback(OCRResult *result);
    httpclient_t ai_client;
    httpclient_data_t ai_client_data;
    char * mDataPath;
    char * mAiMode;
    std::string mOssAccessKey;
    std::string mOssAccessSecret;
    std::string mOssEndPoint;
    std::string mBucket;
    std::string mFacePath;
};
#endif


#include "base/modules/ml/include/HaasMLCloud.h"
#include "ulog/ulog.h"
#include "base/modules/core/include/HaasErrno.h"
#include <string>
using namespace std;

#define LOG_TAG "HAAS_ML_CLOUD"

HaasMLCloud::HaasMLCloud()
{
    LOGD(LOG_TAG, "entern\n");
	int32_t ret;
}

HaasMLCloud::~HaasMLCloud()
{
    LOGD(LOG_TAG, "entern\n");
    int32_t ret;
}

int HaasMLCloud::Config(char *key, char *secret, char *endpoint,
        char *bucket, char *url)
{
    LOGD(LOG_TAG, "entern\n");
    LOGD(LOG_TAG, "key = %s;\n", key);
    LOGD(LOG_TAG, "secret = %s;\n", secret);
    LOGD(LOG_TAG, "endpoint = %s;\n", endpoint);
    LOGD(LOG_TAG, "bucket = %s;\n", bucket);
    LOGD(LOG_TAG, "url = %s;\n", url);
    mFacePath = url;
    LOGD(LOG_TAG, "mFacePath = %s;\n", mFacePath.c_str());
    ucloud_ai_set_key_secret(key, secret);
    ucloud_ai_set_oss_endpoint(endpoint);
    ucloud_ai_set_oss_bucket(bucket);

    return STATUS_OK;
}

int HaasMLCloud::SetInputData(const char* dataPath)
{
    LOGD(LOG_TAG, "entern dataPath = %s;\n", dataPath);
    mDataPath = dataPath;
    return STATUS_OK;
}

int HaasMLCloud::LoadNet(const char* modePath)
{
    LOGD(LOG_TAG, "entern modePath = %s;\n", modePath);
    mAiMode = modePath;
    return STATUS_OK;
}

int HaasMLCloud::Predict()
{
    LOGD(LOG_TAG, "entern mAiMode = %s;\n", mAiMode);
    int ret = -1;
    std::string mode(mAiMode);
    LOGD(LOG_TAG, "Train mode = %s;\n", mode.c_str());

    ret = mode.compare("ObjectDet");
    if (ret == 0)
    {
        LOGD(LOG_TAG, "Get ObjectDet Mode");
        return PredictObjectDet();
    }

    ret = mode.compare("FacebodyComparing");
    if (ret == 0)
    {
        LOGD(LOG_TAG, "Get FacebodyComparing Mode");
        return PredictFacebodyComparing();
    }

    ret = mode.compare("AnimeStyle");
    if (ret == 0)
    {
        LOGD(LOG_TAG, "Get AnimeStyle Mode");
        return PredictAnimeStyle();
    }

    ret = mode.compare("RecognizeExpression");
    if (ret == 0)
    {
        LOGD(LOG_TAG, "Get RecognizeExpression Mode");
        return PredictRecognizeExpression();
    }

    ret = mode.compare("RecognizeCharacter");
    if (ret == 0)
    {
        LOGD(LOG_TAG, "Get RecognizeCharacter Mode");
        return PredictRecognizeCharacter();
    }

    LOGE(LOG_TAG, "AI Mode Par Is illegal\n");
    return STATUS_ERROR;
}

int HaasMLCloud::GetPredictResponses(char* outResult, int len)
{
    LOGD(LOG_TAG, "entern\n");
    return STATUS_OK;
}

int HaasMLCloud::UnLoadNet()
{
    LOGD(LOG_TAG, "entern\n");
    return STATUS_OK;
}

int HaasMLCloud::PredictObjectDet()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_objectdet_detect_object(mDataPath, ObjectDetectCallback);
    return STATUS_OK;
}

int HaasMLCloud::ObjectDetectCallback(ucloud_ai_result_t *result)
{
    int len = 0;
    char *p_type = NULL;
    int x, y, w, h;
    char *type = NULL;
    float score;

    LOGD(LOG_TAG, "entern ObjectDetectCallback:\n");
    if (!result)
        return -1;

    type = result->objectdet.object.type;
    score = result->objectdet.object.score;
    x = result->objectdet.object.box.x;
    y = result->objectdet.object.box.y;
    w = result->objectdet.object.box.w;
    h = result->objectdet.object.box.h;

    if (!type) {
        LOGE(LOG_TAG, "type is null\n");
        return -1;
    }
    p_type = strdup(type);
    if (!p_type) {
        LOGE(LOG_TAG, "p_type strdup fail\n");
        return -1;
    }

    LOGD(LOG_TAG, "Detect object result:\n");
    LOGE(LOG_TAG, "type: %s, Score: %.1f, x: %d, y: %d, w: %d, h: %d\n", p_type, score, x, y, w, h);
    free(p_type);
    p_type = NULL;

    return 0;
}

int HaasMLCloud::PredictFacebodyComparing()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_facebody_comparing_face(mDataPath, mFacePath.c_str(), FacebodyComparingCallback);
    return STATUS_OK;
}

int HaasMLCloud::FacebodyComparingCallback(ucloud_ai_result_t *result)
{
    LOGD(LOG_TAG, "entern\n");
    float confidence;

    if (!result)
        return -1;

    LOGD(LOG_TAG, "Facebody comparing result:\n");
    confidence = result->facebody.face.confidence;
    float x = result->facebody.face.location.x;
    float y = result->facebody.face.location.y;
    float w = result->facebody.face.location.w;
    float h = result->facebody.face.location.h;
    LOGE(LOG_TAG, "confidence: %.1f\n", confidence);
    LOGE(LOG_TAG, "x: %.1f\n", x);
    LOGE(LOG_TAG, "y: %.1f\n", y);
    LOGE(LOG_TAG, "w: %.1f\n", w);
    LOGE(LOG_TAG, "h: %.1f\n", h);
    return STATUS_OK;
}

int HaasMLCloud::AnimeStyleCallback(ucloud_ai_result_t *result)
{
    LOGD(LOG_TAG, "entern\n");
    int ret;
    char *url = NULL;

    if (!result)
        return -1;

    url = result->facebody.anime.url;
    LOGD(LOG_TAG, "Generate human anime style result:\n");
    LOGE(LOG_TAG, "url: %s\n", url);
    return STATUS_OK;
}

int HaasMLCloud::PredictAnimeStyle()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_facebody_generate_human_anime_style(mDataPath, AnimeStyleCallback);
    return STATUS_OK;
}

int HaasMLCloud::PredictRecognizeExpression()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_facebody_recognize_expression(mDataPath, RecognizeExpressionCallback);
    return STATUS_OK;
}

int HaasMLCloud::RecognizeExpressionCallback(ucloud_ai_result_t *result)
{

    LOGD(LOG_TAG, "entern\n");
    int len;
    char *expression, *p_expression;
    float face_probability;

    if (!result)
        return -1;

    expression = result->facebody.expression.expression;
    face_probability = result->facebody.expression.probability;

    if (!expression)
        return -1;

    p_expression = strdup(expression);
    if (!p_expression) {
        LOGE(LOG_TAG, "p_expression strdup fail\n");
        return -1;
    }

    LOGE(LOG_TAG, "Recognize expression result:\n");
    LOGE(LOG_TAG, "type: %s, probability: %.1f\n", p_expression, face_probability);
    free(p_expression);
    p_expression = NULL;
    return STATUS_OK;
}

int HaasMLCloud::RecognizeCharacterCallback(ucloud_ai_result_t *result)
{

    LOGD(LOG_TAG, "entern\n");
    float probability;
    char *text = NULL;
    int left, top;
    int width, height;

    if (!result)
        return -1;

    LOGD(LOG_TAG, "Recognize character result:\n");
    text = result->ocr.character.text;
    left = result->ocr.character.left;
    top = result->ocr.character.top;
    width = result->ocr.character.width;
    height = result->ocr.character.height;
    probability = result->ocr.character.probability;
    if (text) {
        LOGE(LOG_TAG, "text: %s\n", text);
        LOGE(LOG_TAG, "probability: %.1f\n", probability);
        LOGE(LOG_TAG, "text area: left: %d, top: %d, weight: %d, height: %d\n", top, left, width, height);
    }
    return STATUS_OK;
}

int HaasMLCloud::PredictRecognizeCharacter()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_ocr_recognize_character(mDataPath, RecognizeCharacterCallback);
    return STATUS_OK;
}

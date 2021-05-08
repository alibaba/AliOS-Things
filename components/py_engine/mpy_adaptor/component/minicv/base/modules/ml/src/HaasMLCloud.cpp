
#include "HaasMLCloud.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include <string>
using namespace std;

HaasMLCloud::HaasMLCloud()
{
    LOG_D("entern\n");
	int32_t ret;
}

HaasMLCloud::~HaasMLCloud()
{
    LOG_D("entern\n");
    int32_t ret;
}

int HaasMLCloud::Config(char *key, char *secret, char *endpoint,
        char *bucket, char *url)
{
    LOG_D("entern\n");
    LOG_D("key = %s;\n", key);
    LOG_D("secret = %s;\n", secret);
    LOG_D("endpoint = %s;\n", endpoint);
    LOG_D("bucket = %s;\n", bucket);
    LOG_D("url = %s;\n", url);
    mFacePath = url;
    LOG_D("mFacePath = %s;\n", mFacePath.c_str());
    ucloud_ai_set_key_secret(key, secret);
    ucloud_ai_set_oss_endpoint(endpoint);
    ucloud_ai_set_oss_bucket(bucket);

    return STATUS_OK;
}

int HaasMLCloud::SetInputData(const char* dataPath)
{
    LOG_D("entern dataPath = %s;\n", dataPath);
    mDataPath = dataPath;
    return STATUS_OK;
}

int HaasMLCloud::LoadNet(const char* modePath)
{
    LOG_D("entern modePath = %s;\n", modePath);
    mAiMode = modePath;
    return STATUS_OK;
}

int HaasMLCloud::Predict()
{
    LOG_D("entern mAiMode = %s;\n", mAiMode);
    int ret = -1;
    std::string mode(mAiMode);
    LOG_D("Train mode = %s;\n", mode.c_str());

    ret = mode.compare("ObjectDet");
    if (ret == 0)
    {
        LOG_D("Get ObjectDet Mode");
        return PredictObjectDet();
    }

    ret = mode.compare("FacebodyComparing");
    if (ret == 0)
    {
        LOG_D("Get FacebodyComparing Mode");
        return PredictFacebodyComparing();
    }

    ret = mode.compare("AnimeStyle");
    if (ret == 0)
    {
        LOG_D("Get AnimeStyle Mode");
        return PredictAnimeStyle();
    }

    ret = mode.compare("RecognizeExpression");
    if (ret == 0)
    {
        LOG_D("Get RecognizeExpression Mode");
        return PredictRecognizeExpression();
    }

    ret = mode.compare("RecognizeCharacter");
    if (ret == 0)
    {
        LOG_D("Get RecognizeCharacter Mode");
        return PredictRecognizeCharacter();
    }

    LOG_E("AI Mode Par Is illegal\n");
    return STATUS_ERROR;
}

int HaasMLCloud::GetPredictResponses(char* outResult, int len)
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasMLCloud::UnLoadNet()
{
    LOG_D("entern\n");
    return STATUS_OK;
}

int HaasMLCloud::PredictObjectDet()
{
    LOG_D("entern\n");
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

    LOG_D("entern ObjectDetectCallback:\n");
    if (!result)
        return -1;

    type = result->objectdet.object.type;
    score = result->objectdet.object.score;
    x = result->objectdet.object.box.x;
    y = result->objectdet.object.box.y;
    w = result->objectdet.object.box.w;
    h = result->objectdet.object.box.h;

    if (!type) {
        LOG_E("type is null\n");
        return -1;
    }
    p_type = strdup(type);
    if (!p_type) {
        LOG_E("p_type strdup fail\n");
        return -1;
    }

    LOG_D("Detect object result:\n");
    LOG_E("type: %s, Score: %.1f, x: %d, y: %d, w: %d, h: %d\n", p_type, score, x, y, w, h);
    free(p_type);
    p_type = NULL;

    return 0;
}

int HaasMLCloud::PredictFacebodyComparing()
{
    LOG_D("entern\n");
    ucloud_ai_facebody_comparing_face(mDataPath, mFacePath.c_str(), FacebodyComparingCallback);
    return STATUS_OK;
}

int HaasMLCloud::FacebodyComparingCallback(ucloud_ai_result_t *result)
{
    LOG_D("entern\n");
    float confidence;

    if (!result)
        return -1;

    LOG_D("Facebody comparing result:\n");
    confidence = result->facebody.face.confidence;
    float x = result->facebody.face.location.x;
    float y = result->facebody.face.location.y;
    float w = result->facebody.face.location.w;
    float h = result->facebody.face.location.h;
    LOG_E("confidence: %.1f\n", confidence);
    LOG_E("x: %.1f\n", x);
    LOG_E("y: %.1f\n", y);
    LOG_E("w: %.1f\n", w);
    LOG_E("h: %.1f\n", h);
    return STATUS_OK;
}

int HaasMLCloud::AnimeStyleCallback(ucloud_ai_result_t *result)
{
    LOG_D("entern\n");
    int ret;
    char *url = NULL;

    if (!result)
        return -1;

    url = result->facebody.anime.url;
    LOG_D("Generate human anime style result:\n");
    LOG_E("url: %s\n", url);
    return STATUS_OK;
}

int HaasMLCloud::PredictAnimeStyle()
{
    LOG_D("entern\n");
    ucloud_ai_facebody_generate_human_anime_style(mDataPath, AnimeStyleCallback);
    return STATUS_OK;
}

int HaasMLCloud::PredictRecognizeExpression()
{
    LOG_D("entern\n");
    ucloud_ai_facebody_recognize_expression(mDataPath, RecognizeExpressionCallback);
    return STATUS_OK;
}

int HaasMLCloud::RecognizeExpressionCallback(ucloud_ai_result_t *result)
{

    LOG_D("entern\n");
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
        LOG_E("p_expression strdup fail\n");
        return -1;
    }

    LOG_E("Recognize expression result:\n");
    LOG_E("type: %s, probability: %.1f\n", p_expression, face_probability);
    free(p_expression);
    p_expression = NULL;
    return STATUS_OK;
}

int HaasMLCloud::RecognizeCharacterCallback(ucloud_ai_result_t *result)
{

    LOG_D("entern\n");
    float probability;
    char *text = NULL;
    int left, top;
    int width, height;

    if (!result)
        return -1;

    LOG_D("Recognize character result:\n");
    text = result->ocr.character.text;
    left = result->ocr.character.left;
    top = result->ocr.character.top;
    width = result->ocr.character.width;
    height = result->ocr.character.height;
    probability = result->ocr.character.probability;
    if (text) {
        LOG_E("text: %s\n", text);
        LOG_E("probability: %.1f\n", probability);
        LOG_E("text area: left: %d, top: %d, weight: %d, height: %d\n", top, left, width, height);
    }
    return STATUS_OK;
}

int HaasMLCloud::PredictRecognizeCharacter()
{
    LOG_D("entern\n");
    ucloud_ai_ocr_recognize_character(mDataPath, RecognizeCharacterCallback);
    return STATUS_OK;
}

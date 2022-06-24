
#include "base/modules/ml/include/HaasMLCloud.h"

#include <string>

#include "base/modules/core/include/HaasErrno.h"
#include "ulog/ulog.h"
using namespace std;

#define LOG_TAG "HAAS_ML_CLOUD"
static ucloud_ai_result_t g_result[16];
static int g_index = 0;

HaasMLCloud::HaasMLCloud()
{
    LOGD(LOG_TAG, "entern\n");
}

HaasMLCloud::~HaasMLCloud()
{
    LOGD(LOG_TAG, "entern\n");
}

int HaasMLCloud::Config(char *key, char *secret, char *endpoint, char *bucket)
{
    LOGD(LOG_TAG, "entern\n");
    LOGD(LOG_TAG, "key = %s;\n", key);
    LOGD(LOG_TAG, "secret = %s;\n", secret);
    LOGD(LOG_TAG, "endpoint = %s;\n", endpoint);
    LOGD(LOG_TAG, "bucket = %s;\n", bucket);

    ucloud_ai_set_key_secret(key, secret);
    ucloud_ai_set_oss_endpoint(endpoint);
    ucloud_ai_set_oss_bucket(bucket);

    return STATUS_OK;
}

int HaasMLCloud::SetInputData(const char *dataPath, const char *compareDataPath)
{
    LOGD(LOG_TAG, "entern dataPath = %s, compareDataPath: %s;\n", dataPath, compareDataPath);
    mDataPath = const_cast<char *>(dataPath);

    if (compareDataPath)
        mFacePath = compareDataPath;

    return STATUS_OK;
}

int HaasMLCloud::LoadNet(const char *modePath)
{
    LOGD(LOG_TAG, "entern modePath = %s;\n", modePath);
    mAiMode = const_cast<char *>(modePath);
    return STATUS_OK;
}

int HaasMLCloud::Predict()
{
    LOGD(LOG_TAG, "entern mAiMode = %s;\n", mAiMode);
    int ret = -1;
    std::string mode(mAiMode);
    LOGD(LOG_TAG, "Train mode = %s;\n", mode.c_str());
    g_index = 0;
    /*reset result*/
    memset(g_result, 0, sizeof(ucloud_ai_result_t) * 16);

    ret = mode.compare("ObjectDet");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get ObjectDet Mode");
        ret = PredictObjectDet();
        goto end;
    }

    ret = mode.compare("FacebodyComparing");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get FacebodyComparing Mode");
        ret = PredictFacebodyComparing();
        goto end;
    }

    ret = mode.compare("AnimeStyle");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get AnimeStyle Mode");
        ret = PredictAnimeStyle();
        goto end;
    }

    ret = mode.compare("RecognizeExpression");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get RecognizeExpression Mode");
        ret = PredictRecognizeExpression();
        goto end;
    }

    ret = mode.compare("RecognizeCharacter");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get RecognizeCharacter Mode");
        ret = PredictRecognizeCharacter();
        goto end;
    }

    ret = mode.compare("DetectFruits");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get DetectFruits Mode");
        for (int i = 0; i < g_index; i++)
            if (g_result[i].imagerecog.fruits.name != NULL) {
                free(g_result[i].imagerecog.fruits.name);
                g_result[i].imagerecog.fruits.name = NULL;
            }
        ret = PredictDetectFruits();
        goto end;
    }

    ret = mode.compare("DetectPedestrian");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get DetectPedestrian Mode");
        for (int i = 0; i < g_index; i++)
            if (g_result[i].facebody.pedestrian.type != NULL) {
                free(g_result[i].facebody.pedestrian.type);
                g_result[i].facebody.pedestrian.type = NULL;
            }
        ret = PredictDetectPedestrian();
        goto end;
    }

    ret = mode.compare("RecognizeLicensePlate");
    if (ret == 0) {
        LOGD(LOG_TAG, "Get RecognizeLicensePlate Mode");
        for (int i = 0; i < g_index; i++) {
            if (g_result[i].ocr.licensePlate.plateNumber != NULL) {
                free(const_cast<char *>(g_result[i].ocr.licensePlate.plateNumber));
                g_result[i].ocr.licensePlate.plateNumber = NULL;
            }

            if (g_result[i].ocr.licensePlate.plateType != NULL) {
                free(const_cast<char *>(g_result[i].ocr.licensePlate.plateType));
                g_result[i].ocr.licensePlate.plateType = NULL;
            }
        }
        ret = PredictRecognizeLicensePlate();
        goto end;
    }

end:
    if (ret != STATUS_OK)
        LOGE(LOG_TAG, "AI Mode Par Is illegal\n");
    return ret;
}

int HaasMLCloud::GetPredictResponses(char *outResult, int len)
{
    LOGD(LOG_TAG, "entern\n");
    if (g_index > 0)
        memcpy(outResult, &g_result[0], sizeof(ucloud_ai_result_t) * g_index);

    return g_index;
}

int HaasMLCloud::UnLoadNet()
{
    LOGD(LOG_TAG, "entern\n");
    int ret = 0;
    std::string mode(mAiMode);

    ret = mode.compare("DetectFruits");
    if (ret == 0) {
        for (int i = 0; i < g_index; i++)
            if (g_result[i].imagerecog.fruits.name != NULL) {
                free(g_result[i].imagerecog.fruits.name);
                g_result[i].imagerecog.fruits.name = NULL;
            }
        goto end;
    }

    ret = mode.compare("DetectPedestrian");
    if (ret == 0) {
        for (int i = 0; i < g_index; i++)
            if (g_result[i].facebody.pedestrian.type != NULL) {
                free(g_result[i].facebody.pedestrian.type);
                g_result[i].facebody.pedestrian.type = NULL;
            }
        goto end;
    }

    ret = mode.compare("RecognizeLicensePlate");
    if (ret == 0) {
        for (int i = 0; i < g_index; i++) {
            if (g_result[i].ocr.licensePlate.plateNumber != NULL) {
                free(const_cast<char *>(g_result[i].ocr.licensePlate.plateNumber));
                g_result[i].ocr.licensePlate.plateNumber = NULL;
            }

            if (g_result[i].ocr.licensePlate.plateType != NULL) {
                free(const_cast<char *>(g_result[i].ocr.licensePlate.plateType));
                g_result[i].ocr.licensePlate.plateType = NULL;
            }
        }
        goto end;
    }
end:
    return STATUS_OK;
}

int HaasMLCloud::PredictObjectDet()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_objectdet_detect_object(mDataPath, ObjectDetectCallback);
    return STATUS_OK;
}

int HaasMLCloud::ObjectDetectCallback(void *result_in)
{
    char *p_type = NULL;
    int x, y, w, h;
    char *type = NULL;
    float score;

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;

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
    ucloud_ai_facebody_comparing_face(mDataPath, (char *)mFacePath.c_str(), FacebodyComparingCallback);
    return STATUS_OK;
}

int HaasMLCloud::FacebodyComparingCallback(void *result_in)
{
    LOGD(LOG_TAG, "entern\n");

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;
    if (!result)
        return STATUS_ERROR;

    LOGD(LOG_TAG, "Facebody comparing result:\n");

    if (g_index >= 16)
        return STATUS_ERROR;

    g_result[g_index].facebody.face.confidence = result->facebody.face.confidence;
    g_result[g_index].facebody.face.location.x = result->facebody.face.location.x;
    g_result[g_index].facebody.face.location.y = result->facebody.face.location.y;
    g_result[g_index].facebody.face.location.w = result->facebody.face.location.w;
    g_result[g_index].facebody.face.location.h = result->facebody.face.location.x;
    g_index++;

    return STATUS_OK;
}

int HaasMLCloud::AnimeStyleCallback(void *result_in)
{
    LOGD(LOG_TAG, "entern\n");
    char *url = NULL;

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;
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

int HaasMLCloud::RecognizeExpressionCallback(void *result_in)
{
    LOGD(LOG_TAG, "entern\n");
    char *expression, *p_expression;
    float face_probability;

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;
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

int HaasMLCloud::DetectPedestrianCallback(void *result_in)
{
    LOGD(LOG_TAG, "entern\n");

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;
    if (!result)
        return STATUS_ERROR;

    LOGD(LOG_TAG, "Detect Fruits result:\n");
    if (g_index >= 16)
        return STATUS_ERROR;
    g_result[g_index].facebody.pedestrian.type = strdup(result->facebody.pedestrian.type);
    g_result[g_index].facebody.pedestrian.score = result->facebody.pedestrian.score;
    g_result[g_index].facebody.pedestrian.box.x = result->facebody.pedestrian.box.x;
    g_result[g_index].facebody.pedestrian.box.y = result->facebody.pedestrian.box.y;
    g_result[g_index].facebody.pedestrian.box.w = result->facebody.pedestrian.box.w;
    g_result[g_index].facebody.pedestrian.box.h = result->facebody.pedestrian.box.h;
    g_index++;

    return STATUS_OK;
}

int HaasMLCloud::PredictDetectPedestrian()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_facebody_detect_pedestrian(mDataPath, DetectPedestrianCallback);
    return STATUS_OK;
}

int HaasMLCloud::DetectFruitsCallback(void *result_in)
{
    LOGD(LOG_TAG, "entern\n");

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;
    if (!result)
        return STATUS_ERROR;

    LOGD(LOG_TAG, "Detect Fruits result:\n");
    if (g_index >= 16)
        return STATUS_ERROR;
    g_result[g_index].imagerecog.fruits.name = strdup(result->imagerecog.fruits.name);
    g_result[g_index].imagerecog.fruits.score = result->imagerecog.fruits.score;
    g_result[g_index].imagerecog.fruits.box.x = result->imagerecog.fruits.box.x;
    g_result[g_index].imagerecog.fruits.box.y = result->imagerecog.fruits.box.y;
    g_result[g_index].imagerecog.fruits.box.w = result->imagerecog.fruits.box.w;
    g_result[g_index].imagerecog.fruits.box.h = result->imagerecog.fruits.box.h;
    g_index++;

    return STATUS_OK;
}

int HaasMLCloud::PredictDetectFruits()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_imagerecog_detect_fruits(mDataPath, DetectFruitsCallback);
    return STATUS_OK;
}

int HaasMLCloud::RecognizeLicensePlateCallback(void *result_in)
{
    LOGD(LOG_TAG, "entern\n");

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;
    if (!result)
        return STATUS_ERROR;

    LOGD(LOG_TAG, "Detect Fruits result:\n");
    if (g_index >= 16)
        return STATUS_ERROR;
    g_result[g_index].ocr.licensePlate.plateNumber = strdup(result->ocr.licensePlate.plateNumber);
    g_result[g_index].ocr.licensePlate.plateType = strdup(result->ocr.licensePlate.plateType);
    g_result[g_index].ocr.licensePlate.confidence = result->ocr.licensePlate.confidence;
    g_result[g_index].ocr.licensePlate.plateTypeConfidence = result->ocr.licensePlate.plateTypeConfidence;
    g_result[g_index].ocr.licensePlate.roi.x = result->ocr.licensePlate.roi.x;
    g_result[g_index].ocr.licensePlate.roi.y = result->ocr.licensePlate.roi.y;
    g_result[g_index].ocr.licensePlate.roi.w = result->ocr.licensePlate.roi.w;
    g_result[g_index].ocr.licensePlate.roi.h = result->ocr.licensePlate.roi.h;
    g_index++;

    return STATUS_OK;
}

int HaasMLCloud::PredictRecognizeLicensePlate()
{
    LOGD(LOG_TAG, "entern\n");
    ucloud_ai_ocr_recognize_license_plate(mDataPath, RecognizeLicensePlateCallback);
    return STATUS_OK;
}

int HaasMLCloud::RecognizeCharacterCallback(void *result_in)
{
    LOGD(LOG_TAG, "entern\n");
    float probability;
    char *text = NULL;
    int left, top;
    int width, height;

    ucloud_ai_result_t *result = (ucloud_ai_result_t *)result_in;
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


#include "HaasMLCloud.h"
#include "HaasLog.h"
#include "HaasErrno.h"
#include "oss_app.h"

#include <string>
using namespace std;

HaasMLCloud::HaasMLCloud()
{
    LOG_D("entern\n");
	int32_t ret;
	char * customer_header = "Accept: */*\r\n";

    ai_client = { 0 };
    ai_client_data = {0};

	ret = httpclient_prepare(&ai_client_data, HEAD_SIZE, BODY_SZIE);
	if (ret != HTTP_SUCCESS)
		return;

	ai_client.is_http = true;
	httpclient_set_custom_header(&ai_client, customer_header);
}

HaasMLCloud::~HaasMLCloud()
{
    LOG_D("entern\n");
    int32_t ret;
    ret = httpclient_unprepare(&ai_client_data);
}

int HaasMLCloud::Config(char *key, char *secret, char *region_id,
        char *endpoint, char *url)
{
    LOG_D("entern\n");
    LOG_D("key = %s;\n", key);
    LOG_D("secret = %s;\n", secret);
    LOG_D("region_id = %s;\n", region_id);
    LOG_D("endpoint = %s;\n", endpoint);
    LOG_D("url = %s;\n", url);
    mOssAccessKey = key;
    mOssAccessSecret = secret;
    mOssEndPoint = region_id;
    mBucket = endpoint;
    mFacePath = url;
    LOG_D("mOssAccessKey = %s;\n", mOssAccessKey.c_str());
    LOG_D("mOssAccessSecret = %s;\n", mOssAccessSecret.c_str());
    LOG_D("mOssEndPoint = %s;\n", mOssEndPoint.c_str());
    LOG_D("mBucket = %s;\n", mBucket.c_str());
    LOG_D("mFacePath = %s;\n", mFacePath.c_str());

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
	int ret;
	int len;
	char *tmp_upload_url, p_upload_url[128];

	/*update capture.jpg to oss*/
	tmp_upload_url = upload_local_file(mDataPath, mOssAccessKey.c_str(), \
			mOssAccessSecret.c_str(), NULL , mOssEndPoint.c_str(), mBucket.c_str());
	if (!tmp_upload_url) {
		LOGE(TAG, "url is null\n");
		return -1;
	}
	len = strlen(tmp_upload_url);
	memcpy(p_upload_url, tmp_upload_url, len);
	p_upload_url[len] = '\0';

	//ret = objectdet_detect_object_ai(p_upload_url, object_detect_callback);
	ret = objectdet_config_and_detect_object_ai(mOssAccessKey.c_str(),
            mOssAccessSecret.c_str(), ML_CLOUD_AI_REGION_ID,
            ML_CLOUD_AI_OBJECTDET_ENDPOINT, p_upload_url, ObjectDetectCallback);
	if (ret < 0)
		return -1;

    return STATUS_OK;
}

int HaasMLCloud::ObjectDetectCallback(char *type, float score, object_rect_t *rect)
{
    LOG_D("entern object_detect_callback\n");
    int len = 0;
    char *p_type = NULL;

    if (!type || !rect) {
        LOG_E("type or rect is null\n");
        return -1;
    }
    p_type = strdup(type);
    if (!p_type) {
        LOG_E("p_type strdup fail\n");
        return -1;
    }
    if (p_type) {
        LOG_E("Object Detect type: %s\n", p_type);
        free(p_type);
        p_type = NULL;
    }
    LOG_D("out object_detect_callback\n");
    return 0;
}

int HaasMLCloud::PredictFacebodyComparing()
{
    LOG_D("entern\n");
	int ret;
	int len;
	char *tmp_upload_url, p_upload_url[128];

	/*update capture.jpg to oss*/
	tmp_upload_url = upload_local_file(mDataPath, mOssAccessKey.c_str(), \
			mOssAccessSecret.c_str(), NULL , mOssEndPoint.c_str(), mBucket.c_str());
	if (!tmp_upload_url) {
		LOGE(TAG, "url is null\n");
		return -1;
	}
	len = strlen(tmp_upload_url);
	memcpy(p_upload_url, tmp_upload_url, len);
	p_upload_url[len] = '\0';

	ret = facebody_config_and_comparing_ai(mOssAccessKey.c_str(),
            mOssAccessSecret.c_str(), ML_CLOUD_AI_REGION_ID,
            ML_CLOUD_AI_FACEBODY_ENDPOINT, p_upload_url, mFacePath.c_str(),
            FacebodyComparingCallback);
	if (ret < 0)
		return -1;

    return STATUS_OK;
}

int HaasMLCloud::FacebodyComparingCallback(float confidence, face_rect_t *face_rect)
{
    LOG_D("FaceBody Comparing confidence = %f;\n", confidence);
    if (face_rect != NULL)
    {
        LOG_E("AI RECOGNIZE rect is %d,%d,%d,%d;\n",
                (int)face_rect->x, (int)face_rect->y, (int)face_rect->w, (int)face_rect->h);
    }
    return STATUS_OK;
}

int HaasMLCloud::AnimeStyleCallback(char *url, void* data)
{
    LOG_E("AnimeStyle url:%s;\n", url);
    int32_t ret;

    HaasMLCloud * mHaasMLCloud = (HaasMLCloud *)data;
    httpclient_t* client = mHaasMLCloud->GetAiClient();
    httpclient_data_t* clientData = mHaasMLCloud->GetAiClientData();
    ret = httpclient_conn(client, (const char *)url);
    if(HTTP_SUCCESS != ret) {
        LOG_E("http connect failed\n");
        return -1;
    }

    int recv_len = 0;

    httpclient_reset(clientData);
    ret = httpclient_send(client, (const char *)url, HTTP_GET, clientData);
    if(HTTP_SUCCESS != ret) {
        LOG_E("http send request failed\n");
        return -1;
    }
    do {
        ret = httpclient_recv(client, clientData);
        if (ret < 0)
            break;
        recv_len = clientData->response_content_len;
    } while (ret == HTTP_EAGAIN);
    LOG_D("recv_len = %d;\n", recv_len);

    httpclient_clse(client);

    FILE *jpeg_file;

    if ((jpeg_file = fopen("/sdcard/humananime.png", "wb")) == NULL) {
        LOG_E("opening output file fail\n");
        return -1;
    }

    if (fwrite(clientData->response_buf, clientData->response_content_len, 1, jpeg_file) < 1) {
        LOG_E("write buf fail\n");
        fclose(jpeg_file);
        return -1;
    }
    fclose(jpeg_file);
    LOG_D("write ximage success recv_len = %d;\n", recv_len);
    return STATUS_OK;
}

int HaasMLCloud::PredictAnimeStyle()
{
    LOG_D("entern\n");
	int ret;
	int len;
	char *tmp_upload_url, p_upload_url[128];

	/*update capture.jpg to oss*/
	tmp_upload_url = upload_local_file(mDataPath, mOssAccessKey.c_str(), \
			mOssAccessSecret.c_str(), NULL , mOssEndPoint.c_str(), mBucket.c_str());
	if (!tmp_upload_url) {
		LOGE(TAG, "url is null\n");
		return -1;
	}
	len = strlen(tmp_upload_url);
	memcpy(p_upload_url, tmp_upload_url, len);
	p_upload_url[len] = '\0';

	ret = facebody_config_and_generate_human_anime_style_ai(mOssAccessKey.c_str(),
            mOssAccessSecret.c_str(), ML_CLOUD_AI_REGION_ID,
            ML_CLOUD_AI_FACEBODY_ENDPOINT, p_upload_url, AnimeStyleCallback, (void*)this);
	if (ret < 0)
		return -1;

    return STATUS_OK;
}

#if 0
int HaasMLCloud::AnimeStyleCallback(char *out_url)
{
    LOG_D("entern %s;\n", out_url);
    return STATUS_OK;
}
#endif

int HaasMLCloud::PredictRecognizeExpression()
{
    LOG_D("entern\n");
	int ret;
	int len;
	char *tmp_upload_url, p_upload_url[128];

	/*update capture.jpg to oss*/
	tmp_upload_url = upload_local_file(mDataPath, mOssAccessKey.c_str(), \
			mOssAccessSecret.c_str(), NULL , mOssEndPoint.c_str(), mBucket.c_str());
	if (!tmp_upload_url) {
		LOGE(TAG, "url is null\n");
		return -1;
	}
	len = strlen(tmp_upload_url);
	memcpy(p_upload_url, tmp_upload_url, len);
	p_upload_url[len] = '\0';

	ret = facebody_config_and_recognize_expression_ai(mOssAccessKey.c_str(),
            mOssAccessSecret.c_str(), ML_CLOUD_AI_REGION_ID,
            ML_CLOUD_AI_FACEBODY_ENDPOINT, p_upload_url, RecognizeExpressionCallback);
	if (ret < 0)
		return -1;

    return STATUS_OK;
}

int HaasMLCloud::RecognizeExpressionCallback(char *expression, float face_probability,
        face_rect_t *face_rect)
{

    LOG_E("Expression : %s;\n", expression);
    return STATUS_OK;
}

int HaasMLCloud::RecognizeCharacterCallback(OCRResult *result)
{
    if (result == NULL)
    {
        LOG_E("result is null : %s;\n");
        return -1;
    }
    LOG_E("probability:%f\n", result->probability);
    LOG_E("text:%s\n", result->text);
    LOG_E("left:%d\n", result->left);
    LOG_E("angle:%d\n", result->angle);
    LOG_E("top:%d\n", result->top);
    LOG_E("height:%d\n", result->height);
    LOG_E("width:%d\n", result->width);

    return STATUS_OK;
}

int HaasMLCloud::PredictRecognizeCharacter()
{
    LOG_D("entern\n");
	int ret;
	int len;
	char *tmp_upload_url, p_upload_url[128];

	/*update capture.jpg to oss*/
	tmp_upload_url = upload_local_file(mDataPath, mOssAccessKey.c_str(), \
			mOssAccessSecret.c_str(), NULL , mOssEndPoint.c_str(), mBucket.c_str());
	if (!tmp_upload_url) {
		LOGE(TAG, "url is null\n");
		return -1;
	}
	len = strlen(tmp_upload_url);
	memcpy(p_upload_url, tmp_upload_url, len);
	p_upload_url[len] = '\0';

	ret = recognizeCharacter_config_ai(mOssAccessKey.c_str(),
            mOssAccessSecret.c_str(), ML_CLOUD_AI_REGION_ID,
            ML_CLOUD_AI_OCR_ENDPOINT, p_upload_url, RecognizeCharacterCallback);
	if (ret < 0)
		return -1;

    return STATUS_OK;
}

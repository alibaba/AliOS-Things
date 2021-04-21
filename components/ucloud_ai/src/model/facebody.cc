#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/facebody/FacebodyClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "model/common.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Facebody;

int compareFace(char *urlA, char *urlB, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_FACEBODY_ENDPOINT);
    FacebodyClient client(key, secret, configuration);
    Model::CompareFaceRequest request;
    string tmpImageURLA, tmpImageURLB;
    FacebodyResultStruct result;
    float confidence;
    int ret = 0;

    tmpImageURLA = urlA;
    tmpImageURLB = urlB;

    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURLA(tmpImageURLA);
    request.setImageURLB(tmpImageURLB);

    auto outcome =  client.compareFace(request);
    cout << endl << "facebody describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;

    cout << "confidence:" << outcome.result().getData().confidence << endl;
    cout << "x:" << outcome.result().getData().rectAList[0] << endl;
    cout << "y:" << outcome.result().getData().rectAList[1] << endl;
    cout << "w:" << outcome.result().getData().rectAList[2] << endl;
    cout << "h" << outcome.result().getData().rectAList[3] << endl;

    if (cb) {
        result.face.confidence = outcome.result().getData().confidence;
        result.face.location.x = outcome.result().getData().rectAList[0];
        result.face.location.y = outcome.result().getData().rectAList[1];
        result.face.location.w = outcome.result().getData().rectAList[2];
        result.face.location.h = outcome.result().getData().rectAList[3];
        ret = cb((void *)&result);
    }
    ShutdownSdk();
    cout << "facebody comparing done" << endl << endl;
    return ret;
}

int recognizeExpression(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_FACEBODY_ENDPOINT);
    FacebodyClient client(key, secret, configuration);
    Model::RecognizeExpressionRequest request;
    string tmpImageURL;
    string expression;
    float faceProbability;
    FacebodyResultStruct result;
    int ret = 0;

    tmpImageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(tmpImageURL);
    auto outcome = client.recognizeExpression(request);
    cout << endl << "facebody describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;

    cout << "expression:" << outcome.result().getData().elements[0].expression << endl;
    cout << "face probablility:" << outcome.result().getData().elements[0].faceProbability << endl;
    cout << "x:" << outcome.result().getData().elements[0].faceRectangle.left << endl;
    cout << "y:" << outcome.result().getData().elements[0].faceRectangle.top << endl;
    cout << "w:" << outcome.result().getData().elements[0].faceRectangle.width << endl;
    cout << "h:" << outcome.result().getData().elements[0].faceRectangle.height << endl;

    if (cb) {
        expression = outcome.result().getData().elements[0].expression;
        if (!expression.empty()) {
            result.expression.expression = (char *)expression.c_str();
            cout << "expression:" << outcome.result().getData().elements[0].expression << endl;
            result.expression.probability = outcome.result().getData().elements[0].faceProbability;
            result.expression.location.x = 20; // outcome.result().getData().elements[0].faceRectangle.left;
            result.expression.location.y = 20; // outcome.result().getData().elements[0].faceRectangle.top;
            result.expression.location.w = outcome.result().getData().elements[0].faceRectangle.width;
            result.expression.location.h = outcome.result().getData().elements[0].faceRectangle.height;
            ret = cb((void *)&result);
        }
    }
    ShutdownSdk();
    return ret;
}

int generateHumanAnimeStyle(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_FACEBODY_ENDPOINT);
    FacebodyClient client(key, secret, configuration);
    Model::GenerateHumanAnimeStyleRequest request;
    string tmpImageURL;
    FacebodyResultStruct result;
    string outImageUrl;
    int len;
    int ret = 0;

    tmpImageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(tmpImageURL);

    auto outcome = client.generateHumanAnimeStyle(request);
    cout << endl << "facebody describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "image url:" << outcome.result().getData().imageURL << endl;

    outImageUrl = outcome.result().getData().imageURL;

    if (outImageUrl.size() > 0) {
        result.anime.url = (char *)outImageUrl.c_str();
        result.anime.imageLen = getResponseBodyByUrl(result.anime.url, &result.anime.image);
    }
    if (result.anime.image && cb) {
        ret = cb((void *)&result);
    }
    ShutdownSdk();
    return ret;
}

}

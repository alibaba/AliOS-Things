#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include <alibabacloud/ocr/OcrClient.h>
#include "alibabacloud/core/CommonClient.h"
#include "ocr.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

static ClientConfiguration configuration;

int recognizeCharacter_config_ai(char *key, char *secret, char *region_id,
                char *endpoint, char *url, recognizeCharacter_cb_t cb) {

    InitializeSdk();
    string sKey = key;
    string sSecret = secret;
    string sRegionId = region_id;
    string sEndPoint = endpoint;
    cout << "Key: " << sKey.c_str() << endl;
    cout << "Secret: " << sSecret.c_str() << endl;
    cout << "RegionId: " << sRegionId.c_str() << endl;
    cout << "EndPoint: " << sEndPoint.c_str() << endl;
    configuration.setRegionId(sRegionId);
    configuration.setEndpoint(sEndPoint);
    OcrClient client(sKey, sSecret, configuration);
    RecognizeCharacterRequest request;
    string tmpImageURL;
    int ret;

    tmpImageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(tmpImageURL);
    request.setMinHeight(10);
    request.setOutputProbability(true);
    auto outcome = client.recognizeCharacter(request);
    cout << endl << "RecognizeCharacter returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;

    cout << "results size:" << outcome.result().getData().results.size() << endl;
    int size = outcome.result().getData().results.size();
    for(int i = 0; i < size; i++)
    {
        cout << "index:" << i << endl;
        cout << "probability:" << outcome.result().getData().results[i].probability << endl;
        cout << "text:" << outcome.result().getData().results[i].text << endl;
        cout << "left:" << outcome.result().getData().results[i].textRectangles.left << endl;
        cout << "angle:" << outcome.result().getData().results[i].textRectangles.angle << endl;
        cout << "top:" << outcome.result().getData().results[i].textRectangles.top << endl;
        cout << "height:" << outcome.result().getData().results[i].textRectangles.height << endl;
        cout << "width:" << outcome.result().getData().results[i].textRectangles.width << endl;
        OCRResult mOcrResult;
        mOcrResult.left = outcome.result().getData().results[i].textRectangles.left;
        mOcrResult.angle = outcome.result().getData().results[i].textRectangles.angle;
        mOcrResult.top = outcome.result().getData().results[i].textRectangles.top;
        mOcrResult.height = outcome.result().getData().results[i].textRectangles.height;
        mOcrResult.width = outcome.result().getData().results[i].textRectangles.width;
        mOcrResult.probability = outcome.result().getData().results[i].probability;
        mOcrResult.text = outcome.result().getData().results[i].text.c_str();
        if (cb) {
            cout << "call callback:" << i << endl;
            cb(&mOcrResult);
        }
    }
    ShutdownSdk();
    return ret;
}

}

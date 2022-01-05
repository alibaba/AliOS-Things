#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/objectdet/ObjectdetClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "model/common.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Objectdet;

int detectObject(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_OBJECTDET_ENDPOINT);
    ObjectdetClient client(key, secret, configuration);
    Model::DetectObjectRequest request;
    string tmpImageURL;
    ObjectDetResultStruct result;
    string type;
    double left, top, right, bottom; 
    int ret = 0;
    int objectNum, i;

    tmpImageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(tmpImageURL);

    auto outcome = client.detectObject(request);
    cout << endl << "objectdet describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    // cout << "requestId: " << outcome.result().requestId() << endl << endl;

    cout << "object num:" << outcome.result().getData().elements.size() << endl;
    cout << "object height:" << outcome.result().getData().height << endl;
    cout << "object width:" << outcome.result().getData().width << endl;
    objectNum = outcome.result().getData().elements.size();
    for (i = 0; i < objectNum; i++) {
        cout << "object score:" << outcome.result().getData().elements[i].score << endl;
        cout << "object type:" << outcome.result().getData().elements[i].type << endl;
        cout << "object boxes.left:" << outcome.result().getData().elements[i].boxes[0] << endl;
        cout << "object boxes.top:" << outcome.result().getData().elements[i].boxes[1] << endl;
        cout << "object boxes.right:" << outcome.result().getData().elements[i].boxes[2] << endl;
        cout << "object boxes.bottom:" << outcome.result().getData().elements[i].boxes[3] << endl;

        type = outcome.result().getData().elements[i].type;
        left = outcome.result().getData().elements[i].boxes[0];
        top = outcome.result().getData().elements[i].boxes[1];
        right = outcome.result().getData().elements[i].boxes[2];
        bottom = outcome.result().getData().elements[i].boxes[3];
        result.object.type = (char *)type.c_str();
        result.object.score = outcome.result().getData().elements[i].score;
        result.object.box.x = left;
        result.object.box.y = top;
        result.object.box.w = right - left;
        result.object.box.h = bottom - top;
        if (cb) {
            ret = cb((void *)&result);
        }
    }
    ShutdownSdk();
    return ret;
}

int detectMainBody(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_OBJECTDET_ENDPOINT);
    ObjectdetClient client(key, secret, configuration);
    Model::DetectMainBodyRequest request;
    string tmpImageURL;
    ObjectDetResultStruct result;
    int ret = 0;

    tmpImageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(tmpImageURL);

    auto outcome = client.detectMainBody(request);
    cout << endl << "objectdet detectMainBody describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "object x:" << outcome.result().getData().location.x << endl;
    cout << "object y:" << outcome.result().getData().location.y << endl;
    cout << "object width:" << outcome.result().getData().location.width << endl;
    cout << "object height:" << outcome.result().getData().location.height << endl;
    result.mainbody.location.x = outcome.result().getData().location.x;
    result.mainbody.location.y = outcome.result().getData().location.y;
    result.mainbody.location.w = outcome.result().getData().location.width;
    result.mainbody.location.h = outcome.result().getData().location.height;

    if (cb) {
        ret = cb((void *)&result);
    }
    ShutdownSdk();
    return ret;
}

}

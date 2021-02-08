#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/objectdet/ObjectdetClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "objectdet.h"
#include "sdkconfig.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Objectdet;

static ClientConfiguration configuration;
static string key = CLOUD_AI_ACCESS_KEY;
static string secret = CLOUD_AI_ACCESS_SECRET;

int objectdet_detect_object_ai(char *in_url, objectdet_detect_object_cb_t cb) {
    int ret;
    int object_num, i;

    InitializeSdk();
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_OBJECTDET_ENDPOINT);
    ObjectdetClient client(key, secret, configuration);
    Model::DetectObjectRequest request;
    string tmpImageURL;
    string type;
    int width, height;
    float score;
    object_rect_t rect;

    tmpImageURL = in_url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(tmpImageURL);

    auto outcome = client.detectObject(request);
    cout << endl << "objectdet describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    //cout << "requestId: " << outcome.result().requestId() << endl << endl;

    cout << "object num:" << outcome.result().getData().elements.size() << endl;
    //cout << "object height:" << outcome.result().getData().height << endl;
    //cout << "object width:" << outcome.result().getData().width << endl;
    object_num = outcome.result().getData().elements.size();
    for (i = 0; i < object_num; i++) {
        //cout << "object score:" << outcome.result().getData().elements[i].score << endl;
        //cout << "object type:" << outcome.result().getData().elements[i].type << endl;
        //cout << "object boxes.x:" << outcome.result().getData().elements[i].boxes[0] << endl;
        //cout << "object boxes.y:" << outcome.result().getData().elements[i].boxes[1] << endl;
        //cout << "object boxes.w:" << outcome.result().getData().elements[i].boxes[2] << endl;
        //cout << "object boxes.h:" << outcome.result().getData().elements[i].boxes[3] << endl;

        width = outcome.result().getData().width;
        height = outcome.result().getData().height;
        score = outcome.result().getData().elements[i].score;
        type = outcome.result().getData().elements[i].type;
        rect.x = 20; //outcome.result().getData().elements[i].boxes[0];
        rect.y = 20*i; //outcome.result().getData().elements[i].boxes[1];
        rect.w = outcome.result().getData().elements[i].boxes[2];
        rect.h = outcome.result().getData().elements[i].boxes[3];

        ret = cb((char *)type.c_str(), score, &rect);
        if (ret < 0) {
            ShutdownSdk();
            return -1;
        }
    }
    ShutdownSdk();
    return 0;
}

}
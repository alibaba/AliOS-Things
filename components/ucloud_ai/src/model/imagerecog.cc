#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/imagerecog/ImagerecogClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "model/common.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Imagerecog;

int classifyingRubbish(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_IMAGERECOG_ENDPOINT);
    ImagerecogClient client(key, secret, configuration);
    Model::ClassifyingRubbishRequest request;
    string imageURL;
    ImageRecogResultStruct result;
    string rubbish, category;
    int ret = 0, i;

    imageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(imageURL);

    auto outcome = client.classifyingRubbish(request);
    cout << endl << "imageseg describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "sensitive: " << outcome.result().getData().sensitive << endl;

    for (i = 0; i < outcome.result().getData().elements.size(); i++) {
        cout << i << "rubbish: " << outcome.result().getData().elements[i].rubbish << endl;
        cout << i << "category: " << outcome.result().getData().elements[i].category << endl;
        cout << i << "categoryScore: " << outcome.result().getData().elements[i].categoryScore << endl;
        cout << i << "rubbishScore: " << outcome.result().getData().elements[i].rubbishScore << endl;
        rubbish = outcome.result().getData().elements[i].rubbish;
        category = outcome.result().getData().elements[i].category;
        result.rubbish.rubbish = rubbish.c_str();
        result.rubbish.category = category.c_str();
        result.rubbish.categoryScore = outcome.result().getData().elements[i].categoryScore;
        result.rubbish.rubbishScore = outcome.result().getData().elements[i].rubbishScore;

        if (cb) {
            ret = cb((void *)&result);
        }
    }

    ShutdownSdk();
    return ret;
}

int detectFruits(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_IMAGERECOG_ENDPOINT);
    ImagerecogClient client(key, secret, configuration);
    Model::DetectFruitsRequest request;
    ImageRecogResultStruct result;
    string imageURL;
    string name;
    int ret = 0, i;

    imageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(imageURL);

    auto outcome = client.detectFruits(request);
    cout << endl << "imageseg describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;

    for (i = 0; i < outcome.result().getData().elements.size(); i++) {
        cout << i << "score: " << outcome.result().getData().elements[i].score << endl;
        cout << i << "name: " << outcome.result().getData().elements[i].name << endl;
        result.fruits.score = outcome.result().getData().elements[i].score;
        name = outcome.result().getData().elements[i].name;
        result.fruits.name = (char *)name.c_str();

        cout << "box.xmin: " << outcome.result().getData().elements[i].box[0] << endl;
        cout << "box.ymin: " << outcome.result().getData().elements[i].box[1] << endl;
        cout << "box.xmax: " << outcome.result().getData().elements[i].box[2] << endl;
        cout << "box.ymax: " << outcome.result().getData().elements[i].box[3] << endl;
        result.fruits.box.x = atoi(outcome.result().getData().elements[i].box[0].c_str());
        result.fruits.box.y = atoi(outcome.result().getData().elements[i].box[1].c_str());
        result.fruits.box.w = atoi(outcome.result().getData().elements[i].box[2].c_str()) - result.fruits.box.x;
        result.fruits.box.h = atoi(outcome.result().getData().elements[i].box[3].c_str()) - result.fruits.box.y;
        if (cb) {
            ret = cb((void *)&result);
        }
    }

    ShutdownSdk();
    return ret;
}

}

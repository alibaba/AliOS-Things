#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/imageseg/ImagesegClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "model/common.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Imageseg;

int segmentCommonImage(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_IMAGESEG_ENDPOINT);
    ImagesegClient client(key, secret, configuration);
    Model::SegmentCommonImageRequest request;
    string inImageURL;
    ImageSegResultStruct result;
    string outImageUrl;
    int ret = 0;

    inImageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(inImageURL);
    request.setReturnForm("whiteBK"); // set png format, mask or whiteBK, please refer to parameters of vision platform

    auto outcome = client.segmentCommonImage(request);
    cout << endl << "imageseg describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "image url: " << outcome.result().getData().imageURL << endl;

    outImageUrl = outcome.result().getData().imageURL;
    if (outImageUrl.size() > 0) {
        result.common.url = (char *)outImageUrl.c_str();
        result.common.imageLen = getResponseBodyByUrl(outcome.result().getData().imageURL.c_str(), &result.common.image);
        if (result.common.image && cb) {
            ret = cb((void *)&result);
        }
    }
    ShutdownSdk();
    return ret;
}

int segmentFace(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_IMAGESEG_ENDPOINT);
    ImagesegClient client(key, secret, configuration);
    Model::SegmentFaceRequest request;
    string inImageURL;
    ImageSegResultStruct result;
    string outImageUrl;
    int ret = 0, i = 0;

    inImageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(inImageURL);

    auto outcome = client.segmentFace(request);
    cout << endl << "imageseg describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;

    if (outcome.result().getData().elements.size() == 0)
        return -1;
    // for (i = 0; i < outcome.result().getData().elements.size(); i++)
    {
        cout << i << "image url: " << outcome.result().getData().elements[i].imageURL << endl;
        cout << i << "width: " << outcome.result().getData().elements[i].width << endl;
        cout << i << "height: " << outcome.result().getData().elements[i].height << endl;
        cout << i << "x: " << outcome.result().getData().elements[i].x << endl;
        cout << i << "y: " << outcome.result().getData().elements[i].y << endl;

        outImageUrl = outcome.result().getData().elements[i].imageURL;
        if (outImageUrl.size() > 0) {
            result.face.url = (char *)outImageUrl.c_str();
            result.face.imageLen = getResponseBodyByUrl(result.face.url, &result.face.image);
            if (result.face.image && cb) {
                result.face.location.x = outcome.result().getData().elements[i].x;
                result.face.location.y = outcome.result().getData().elements[i].y;
                result.face.location.w = outcome.result().getData().elements[i].width;
                result.face.location.h = outcome.result().getData().elements[i].height;
                ret = cb((void *)&result);
            }
        }
    }
    ShutdownSdk();
    return ret;
}

}

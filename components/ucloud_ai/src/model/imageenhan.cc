#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/imageenhan/ImageenhanClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "model/common.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Imageenhan;

int erasePerson(char *imageUrl, char *userMaskUrl, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_IMAGEENHAN_ENDPOINT);
    ImageenhanClient client(key, secret, configuration);
    Model::ErasePersonRequest request;
    string imageURL, userMaskURL;
    ImageEnhanResultStruct result;
    string url;
    int ret = 0;

    imageURL = imageUrl;
    userMaskURL = userMaskUrl;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(imageURL);
    request.setUserMask(userMaskURL);

    auto outcome = client.erasePerson(request);
    cout << endl << "result erasePerson describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "imageUrl: " << outcome.result().getData().imageUrl << endl;

    url = outcome.result().getData().imageUrl;

    if (url.size() > 0) {
        result.person.url = (char *)url.c_str();
        result.person.imageLen = getResponseBodyByUrl(result.person.url, &result.person.image);

        if (result.person.image && cb) {
            ret = cb((void *)&result);
        }
    }
    ShutdownSdk();
    return ret;
}

int extendImageStyle(char *majorUrl, char *styleUrl, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_IMAGEENHAN_ENDPOINT);
    ImageenhanClient client(key, secret, configuration);
    Model::ExtendImageStyleRequest request;
    string inMajorURL, outMajorURL;
    string styleURL, outImageURL;
    ImageEnhanResultStruct result;
    int ret = 0;

    inMajorURL = majorUrl;
    styleURL = styleUrl;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setMajorUrl(inMajorURL);
    request.setStyleUrl(styleURL);

    auto outcome = client.extendImageStyle(request);
    cout << endl << "imageenhan extendImageStyle describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "outMajorURL: " << outcome.result().getData().majorUrl << endl;
    cout << "outImageURL: " << outcome.result().getData().url << endl;

    outMajorURL = outcome.result().getData().majorUrl;
    outImageURL = outcome.result().getData().url;

    if (outMajorURL.size() > 0 && cb) {
        result.style.majorUrl = (char *)outMajorURL.c_str();
        result.style.majorImageLen = getResponseBodyByUrl(outMajorURL.c_str(), &result.style.majorImage);
        ret = cb((void *)&result);
    } else if (outImageURL.size() > 0 && cb) {
        result.style.majorUrl = NULL;
        result.style.outImageUrl = (char *)outImageURL.c_str();
        result.style.outImageLen = getResponseBodyByUrl(outImageURL.c_str(), &result.style.outImage);
        ret = cb((void *)&result);
    }

    ShutdownSdk();
    return ret;
}

}

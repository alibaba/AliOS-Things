#include <iostream>
#include "../utils.h"
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/facebody/FacebodyClient.h"
#include "alibabacloud/core/CommonClient.h"

using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Facebody;

int main() {
    utUtils utils;
    string key = "Your-Access-Key";
    string secret = "Your-Access-Secret";
    InitializeSdk();
    ClientConfiguration configuration("cn-shanghai");

    configuration.setEndpoint("facebody.cn-shanghai.aliyuncs.com");
    FacebodyClient client(key, secret, configuration);
    Model::CompareFaceRequest request;

    request.setMethod(HttpRequest::Method::Post);
    request.setImageURLA("https://aibox-train-bucket.oss-cn-shanghai.aliyuncs.com/face/9_Press_Conference_Press_Conference_9_472.jpg?Expires=1608266970&OSSAccessKeyId=TMP.3Kdiw8VbXJ4JEYeZxcdf6DsDE2H43Q4QgiJKreabL6Mkym1JAgJvkapV3HPKq4jjvnALNUefBbzgGF5qJjoh4u2G2YbHhp&Signature=CL8WSF75gzeaAzWub%2BXw9nYxgaE%3D&versionId=CAEQEhiBgIDen.7PsxciIDlhYjQwMmMzZWJjZjQzYTQ4OGVlMGIyN2NlZWZjNTcw&response-content-type=application%2Foctet-stream");
    request.setImageURLB("https://aibox-train-bucket.oss-cn-shanghai.aliyuncs.com/face/9_Press_Conference_Press_Conference_9_472.jpg?Expires=1608266970&OSSAccessKeyId=TMP.3Kdiw8VbXJ4JEYeZxcdf6DsDE2H43Q4QgiJKreabL6Mkym1JAgJvkapV3HPKq4jjvnALNUefBbzgGF5qJjoh4u2G2YbHhp&Signature=CL8WSF75gzeaAzWub%2BXw9nYxgaE%3D&versionId=CAEQEhiBgIDen.7PsxciIDlhYjQwMmMzZWJjZjQzYTQ4OGVlMGIyN2NlZWZjNTcw&response-content-type=application%2Foctet-stream");

    auto outcome =  client.compareFace(request);
    cout << endl << "facebody describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;

    cout << outcome.result().getData().confidence << endl;
    cout << outcome.result().getData().rectAList[0] << endl;
    cout << outcome.result().getData().rectAList[1] << endl;
    cout << outcome.result().getData().rectAList[2] << endl;
    cout << outcome.result().getData().rectAList[3] << endl;

    ShutdownSdk();
}

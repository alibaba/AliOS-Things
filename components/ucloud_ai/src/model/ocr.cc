#include <iostream>
#include "alibabacloud/core/AlibabaCloud.h"
#include "alibabacloud/ocr/OcrClient.h"
#include "alibabacloud/core/CommonClient.h"
#include "model/common.h"

extern "C" {
using namespace std;
using namespace AlibabaCloud;
using namespace AlibabaCloud::Ocr;

int recognizeIdentityCardFaceSide(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_OCR_ENDPOINT);
    OcrClient client(key, secret, configuration);
    Model::RecognizeIdentityCardRequest request;
    string imageURL;
    OcrResultStruct result;
    string address, birthDate, gender, nationality, iDNumber;
    int ret = 0, i;

    imageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(imageURL);
    request.setSide("face");

    auto outcome = client.recognizeIdentityCard(request);
    cout << endl << "ocr describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "Address: " << outcome.result().getData().frontResult.address << endl;
    cout << "BirthDate: " << outcome.result().getData().frontResult.birthDate << endl;
    cout << "gender: " << outcome.result().getData().frontResult.gender << endl;
    cout << "nationality: " << outcome.result().getData().frontResult.nationality << endl;
    cout << "iDNumber: " << outcome.result().getData().frontResult.iDNumber << endl;
    cout << "faceRectangle.x: " << outcome.result().getData().frontResult.faceRectangle.center.x << endl;
    cout << "faceRectangle.y: " << outcome.result().getData().frontResult.faceRectangle.center.y << endl;
    cout << "faceRectangle.width: " << outcome.result().getData().frontResult.faceRectangle.size.width << endl;
    cout << "faceRectangle.height: " << outcome.result().getData().frontResult.faceRectangle.size.height << endl;
    cout << "faceRectangle.angle: " << outcome.result().getData().frontResult.faceRectangle.angle << endl;
    result.identity.face.location.x = outcome.result().getData().frontResult.faceRectangle.center.x;
    result.identity.face.location.y = outcome.result().getData().frontResult.faceRectangle.center.y;
    result.identity.face.location.w = outcome.result().getData().frontResult.faceRectangle.size.width;
    result.identity.face.location.h = outcome.result().getData().frontResult.faceRectangle.size.height;

    for (i = 0; i < 4; i++) {
        cout << i << "cardAreas.x: " << outcome.result().getData().frontResult.cardAreas[i].x << endl;
        cout << i << "cardAreas.y: " << outcome.result().getData().frontResult.cardAreas[i].y << endl;
        cout << i << "faceRectVertices.x: " << outcome.result().getData().frontResult.faceRectVertices[i].x << endl;
        cout << i << "faceRectVertices.y: " << outcome.result().getData().frontResult.faceRectVertices[i].y << endl;

        result.identity.face.cardX[i] = outcome.result().getData().frontResult.cardAreas[i].x;
        result.identity.face.cardY[i] = outcome.result().getData().frontResult.cardAreas[i].y;
        result.identity.face.faceX[i] = outcome.result().getData().frontResult.faceRectVertices[i].x;
        result.identity.face.faceY[i] = outcome.result().getData().frontResult.faceRectVertices[i].y;
    }
    address = outcome.result().getData().frontResult.address;
    birthDate = outcome.result().getData().frontResult.birthDate;
    gender = outcome.result().getData().frontResult.gender;
    nationality = outcome.result().getData().frontResult.nationality;
    iDNumber = outcome.result().getData().frontResult.iDNumber;
    result.identity.face.address = (char *)address.c_str();
    result.identity.face.birthDate = (char *)birthDate.c_str();
    result.identity.face.gender = (char *)gender.c_str();
    result.identity.face.nationality = (char *)nationality.c_str();
    result.identity.face.iDNumber = (char *)iDNumber.c_str();

    if (cb) {
        ret = cb((void *)&result);
    }

    ShutdownSdk();
    return ret;
}

int recognizeIdentityCardBackSide(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_OCR_ENDPOINT);
    OcrClient client(key, secret, configuration);
    Model::RecognizeIdentityCardRequest request;
    OcrResultStruct result;
    string startDate, issue, endDate;
    string imageURL;
    int ret = 0;

    imageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(imageURL);
    request.setSide("back");

    auto outcome = client.recognizeIdentityCard(request);
    cout << endl << "ocr describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "startDate: " << outcome.result().getData().backResult.startDate << endl;
    cout << "issue: " << outcome.result().getData().backResult.issue << endl;
    cout << "endDate: " << outcome.result().getData().backResult.endDate << endl;
    startDate = outcome.result().getData().backResult.startDate;
    issue = outcome.result().getData().backResult.issue;
    endDate = outcome.result().getData().backResult.endDate;
    result.identity.back.startDate = (char *)startDate.c_str();
    result.identity.back.issue = (char *)issue.c_str();
    result.identity.back.endDate = (char *)endDate.c_str();

    if (cb) {
        ret = cb((void *)&result);
    }
    ShutdownSdk();
    return ret;
}

int recognizeBankCard(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_OCR_ENDPOINT);
    OcrClient client(key, secret, configuration);
    Model::RecognizeBankCardRequest request;
    OcrResultStruct result;
    string bankName, cardNumber, validDate;
    string imageURL;
    int ret = 0;

    imageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(imageURL);

    auto outcome = client.recognizeBankCard(request);
    cout << endl << "ocr describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "bankName: " << outcome.result().getData().bankName << endl;
    cout << "cardNumber: " << outcome.result().getData().cardNumber << endl;
    cout << "validDate: " << outcome.result().getData().validDate << endl;

    bankName = outcome.result().getData().bankName;
    cardNumber = outcome.result().getData().cardNumber;
    validDate = outcome.result().getData().validDate;
    result.bank.bankName = (char *)bankName.c_str();
    result.bank.cardNumber = (char *)cardNumber.c_str();
    result.bank.validDate = (char *)validDate.c_str();

    if (cb) {
        ret = cb((void *)&result);
    }
    ShutdownSdk();
    return ret;
}

int recognizeCharacter(char *url, AIModelCBFunc cb)
{
    InitializeSdk();
    string key = getAccessKey();
    string secret = getAccessSecret();
    ClientConfiguration configuration;
    configuration.setRegionId(CLOUD_AI_REGION_ID);
    configuration.setEndpoint(CLOUD_AI_OCR_ENDPOINT);
    OcrClient client(key, secret, configuration);
    Model::RecognizeCharacterRequest request;
    OcrResultStruct result;
    string text;
    string imageURL;
    int ret = 0, i;

    imageURL = url;
    request.setScheme("http");
    request.setMethod(HttpRequest::Method::Post);
    request.setImageURL(imageURL);
    request.setMinHeight(10);
    request.setOutputProbability(true);

    auto outcome = client.recognizeCharacter(request);
    cout << endl << "ocr recognizeCharacter describeInstances returned:" << endl;
    cout << "error code: " << outcome.error().errorCode() << endl;
    cout << "requestId: " << outcome.result().requestId() << endl << endl;
    cout << "results size: " << outcome.result().getData().results.size() << endl;

    for (i = 0; i < outcome.result().getData().results.size(); i++) {
        cout << i << "text: " << outcome.result().getData().results[i].text << endl;
        cout << i << "probability: " << outcome.result().getData().results[i].probability << endl;
        cout << i << "text left: " << outcome.result().getData().results[i].textRectangles.left << endl;
        cout << i << "text angle: " << outcome.result().getData().results[i].textRectangles.angle << endl;
        cout << i << "text top: " << outcome.result().getData().results[i].textRectangles.top << endl;
        cout << i << "text height: " << outcome.result().getData().results[i].textRectangles.height << endl;
        cout << i << "text: width:" << outcome.result().getData().results[i].textRectangles.width << endl;
        text = outcome.result().getData().results[i].text;
        result.character.text = (char *)text.c_str();
        result.character.probability = outcome.result().getData().results[i].probability;
        result.character.left = outcome.result().getData().results[i].textRectangles.left;
        result.character.angle = outcome.result().getData().results[i].textRectangles.angle;
        result.character.top = outcome.result().getData().results[i].textRectangles.top;
        result.character.height = outcome.result().getData().results[i].textRectangles.height;
        result.character.width = outcome.result().getData().results[i].textRectangles.width;
        if (cb) {
            ret = cb((void *)&result);
        }
    }
    ShutdownSdk();
    return ret;
}

}

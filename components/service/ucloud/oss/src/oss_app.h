//
// Created by fengcai on 2020/6/17.
//

#ifndef SSH_ERLANGSHEN_OSS_APP_H
#define SSH_ERLANGSHEN_OSS_APP_H
#include <string.h>

//using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
#if 0
    struct oss_infor{
        std::string oss_AccessKeyId;
        std::string oss_AccessKeySecret;
        std::string oss_SecretToken;
        std::string oss_Endpoint;
        std::string oss_BucketName;
    };
#endif
char* upload_local_file(char* localfilepath, char *keyId, char *keySecret, char *secretToken, char *endPoint, char *bucketName);
#ifdef __cplusplus
}
#endif

#endif //SSH_ERLANGSHEN_OSS_APP_H

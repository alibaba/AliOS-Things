/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef SSH_ERLANGSHEN_OSS_APP_H
#define SSH_ERLANGSHEN_OSS_APP_H
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

char* oss_upload_local_file(char *keyId, char *keySecret, char *endPoint, char *bucketName, char* localfilepath);
char* oss_upload_local_content(char *keyId, char *keySecret, char *endPoint, char *bucketName, char *scontent);
char* oss_upload_file(char *keyId, char *keySecret, char *endPoint, char *bucketName, char *objectName, char* localfilepath);
char* oss_download_file(char *keyId, char *keySecret, char *endPoint, char *bucketName, char *objectName, char* localfilepath);


#ifdef __cplusplus
}
#endif

#endif //OSS_APP_H

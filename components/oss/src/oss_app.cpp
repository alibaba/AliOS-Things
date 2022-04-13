/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "OssClient.h"
#include "iostream"
#include "fstream"
#include "string.h"
#include "oss_app.h"

#ifdef USE_SD_FOR_OSS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "aos/kernel.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ulog/ulog.h"
int sd_fd;
#endif

using namespace AlibabaCloud::OSS;

static void ProgressCallback(size_t increment, int64_t transfered, int64_t total, void* userData)
{
    std::cout << "ProgressCallback[" << userData << "] => " <<
              increment <<" ," << transfered << "," << total << std::endl;
}

static int64_t getFileSize(const std::string& file)
{
    std::fstream f(file, std::ios::in | std::ios::binary);
    f.seekg(0, f.end);
    int64_t size = f.tellg();
    f.close();
    return size;
}

extern "C"{

#ifdef USE_SD_FOR_OSS
int sd_file_open(char * read_file)
{
    struct stat s;
    const char file[30]={0};
 
    if(!read_file){
        memcpy(file,"/sdcard/test.h264",strlen("/sdcard/test.h264"));
    }
    else{
        memcpy(file,read_file,strlen(read_file));
    }
    LOG("open file %s",file);
    sd_fd = open(file, O_RDONLY);
    if (sd_fd < 0) {
        LOG("Failed to open file %s\r\n", file);
        return -1;
    }
    stat(file,&s);
    return s.st_size;
}

int sd_file_close(void)
{
    close(sd_fd);
}

void sd_file_read(unsigned char * buf,int read_size)
{
    int rc, i;

    rc = read(sd_fd, buf, read_size);
    if (rc < 0) {
        LOG("Failed to read file\r\n");
    } 

    return;
}
#endif

extern Url g_ags_url;
std::string g_url;
char* oss_upload_local_file(char *keyId, char *keySecret, char *endPoint, char *bucketName, char* localfilepath)
{
    /* 初始化OSS账号信息 */
    std::string AccessKeyId;
    std::string AccessKeySecret;
    std::string Endpoint;
    std::string BucketName;
    /* yourObjectName表示上传文件到OSS时需要指定包含文件后缀在内的完整路径，例如abc/efg/123.jpg */
    std::string ObjectName ;

    char *p_url;
    char *pfile_path,file_path[256];

    if((keyId == NULL)||(keySecret == NULL)||(endPoint == NULL)||(bucketName == NULL)||(localfilepath == NULL))
    {
        return NULL;
    }

    AccessKeyId = keyId;
    AccessKeySecret = keySecret;
    Endpoint = endPoint;
    BucketName = bucketName;

#ifdef OSS_DEBUG
    std::cout << "Input_AccessKeyId:" << AccessKeyId <<std::endl;
    std::cout << "Input_AccessKeySecret:" << AccessKeySecret <<std::endl;
    std::cout << "Input_Endpoint:" << Endpoint <<std::endl;
    std::cout << "Input_BucketName:" << BucketName <<std::endl;
#endif
    memset(file_path,0,256);
    pfile_path = localfilepath;
    strncpy(file_path,&pfile_path[1],strlen(pfile_path)-1);
    ObjectName = file_path;

#ifdef USE_SD_FOR_OSS
    int file_total_size;
    char *alloc_file_content;
    std::string sContent;

    /* 初始化网络等资源 */
    InitializeSdk();

    ClientConfiguration conf;
    OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);

    /* 上传文件 */
#ifdef OSS_DEBUG
    std::cout << "objectfile_path:" << ObjectName <<std::endl;
    std::cout << "localfile_path:" << localfilepath <<std::endl;
#endif
    file_total_size = sd_file_open(localfilepath);
    if(file_total_size > READ_SD_SIZE_MAX){
        LOG("---SD open file size too Large %d > 10K",file_total_size);
        sd_file_close();
        ShutdownSdk();
        return NULL;
    }
    LOG("SD open file size <%d>.",file_total_size);
    alloc_file_content = (unsigned char *) aos_malloc(file_total_size);
    if(!alloc_file_content){
        LOG("malloc err");
        aos_free(alloc_file_content);
        sd_file_close();
        ShutdownSdk();
        return NULL;
    }
    sd_file_read(alloc_file_content,file_total_size);
    sContent = alloc_file_content;
    std::shared_ptr<std::iostream> content = std::make_shared<std::stringstream>();
    *content << sContent;

    PutObjectRequest request(BucketName, ObjectName, content);
    TransferProgress progressCallback = { ProgressCallback };
    request.setTransferProgress(progressCallback);

    auto outcome = client.PutObject(request);
    g_url = g_ags_url.toString();
    std::cout << "oss ->url:" << g_url << std::endl;
    p_url = (char *)(g_url.data());

    if (!outcome.isSuccess()) {
        /* 异常处理 */
        std::cout << "PutObject fail" <<
                  ",code:" << outcome.error().Code() <<
                  ",message:" << outcome.error().Message() <<
                  ",requestId:" << outcome.error().RequestId() << std::endl;
        aos_free(alloc_file_content);
        sd_file_close();
        ShutdownSdk();
        return NULL;
    }
    std::cout << __FUNCTION__ << " success, ETag:" << outcome.result().ETag() << std::endl;
    /* 释放网络等资源 */
    aos_free(alloc_file_content);
    sd_file_close();
    ShutdownSdk();
    return p_url;
#else
    /* 初始化网络等资源 */
    InitializeSdk();

    ClientConfiguration conf;
    OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
    //OssClient client(Endpoint, AccessKeyId, AccessKeySecret,SecretToken, conf);

    /* 上传文件 */
    auto fileSize = getFileSize(localfilepath);
#ifdef OSS_DEBUG
    std::cout << "objectfile_path:" << ObjectName <<std::endl;
    std::cout << "localfile_path:" << localfilepath <<std::endl;
    std::cout << "localfile_path size:" << fileSize <<std::endl;
#endif

    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(localfilepath, std::ios::in | std::ios::binary);
    PutObjectRequest request(BucketName, ObjectName, content);
    TransferProgress progressCallback = { ProgressCallback };
    request.setTransferProgress(progressCallback);

    auto outcome = client.PutObject(request);
    g_url = g_ags_url.toString();
    std::cout << "oss ->url:" << g_url << std::endl;
    p_url = (char *)(g_url.data());

    if (!outcome.isSuccess()) {
        /* 异常处理 */
        std::cout << "PutObject fail" <<
                  ",code:" << outcome.error().Code() <<
                  ",message:" << outcome.error().Message() <<
                  ",requestId:" << outcome.error().RequestId() << std::endl;
        ShutdownSdk();
        return NULL;
    }
    std::cout << __FUNCTION__ << " success, ETag:" << outcome.result().ETag() << std::endl;
    /* 释放网络等资源 */
    ShutdownSdk();
    return p_url;
#endif
}

char* oss_upload_local_content(char *keyId, char *keySecret, char *endPoint, char *bucketName, char *scontent)
{
    /* 初始化OSS账号信息 */
    std::string AccessKeyId;
    std::string AccessKeySecret;
    std::string Endpoint;
    std::string BucketName;
    /* yourObjectName表示上传文件到OSS时需要指定包含文件后缀在内的完整路径，例如abc/efg/123.jpg */
    std::string ObjectName ;

    std::string sContent;

    if((keyId == NULL)||(keySecret == NULL)||(endPoint == NULL)||(bucketName == NULL))
    {
        return NULL;
    }
    AccessKeyId = keyId;
    AccessKeySecret = keySecret;
    Endpoint = endPoint;
    BucketName = bucketName;

    if(scontent == NULL){
        sContent = "Welcome to HaaS";
    }
    else{
        sContent = scontent;
    }

#ifdef OSS_DEBUG
    std::cout << "Input_AccessKeyId:" << AccessKeyId <<std::endl;
    std::cout << "Input_AccessKeySecret:" << AccessKeySecret <<std::endl;
    std::cout << "Input_Endpoint:" << Endpoint <<std::endl;
    std::cout << "Input_BucketName:" << BucketName <<std::endl;
#endif

    /* 初始化网络等资源 */
    InitializeSdk();

    ClientConfiguration conf;
    OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);

    /* 上传文件 */
    std::shared_ptr<std::iostream> content = std::make_shared<std::stringstream>();
    *content << sContent;

    ObjectName = "oss/test/oss.txt";
    std::cout << "objectfile_path:" << ObjectName <<std::endl;
    PutObjectRequest request(BucketName, ObjectName, content);

    TransferProgress progressCallback = { ProgressCallback };
    request.setTransferProgress(progressCallback);

    auto outcome = client.PutObject(request);
    g_url = g_ags_url.toString();
    std::cout << "oss ->url:" << g_url << std::endl;

    if (!outcome.isSuccess()) {
        /* 异常处理 */
        std::cout << "PutObject fail: " <<
                  ",code:" << outcome.error().Code() <<
                  ",message:" << outcome.error().Message() <<
                  ",requestId:" << outcome.error().RequestId() << std::endl;
        ShutdownSdk();
        return NULL;
    }
    std::cout << __FUNCTION__ << " success, ETag:" << outcome.result().ETag() << std::endl;
    /* 释放网络等资源 */
    ShutdownSdk();
    return NULL;
}

//cplusplus
}

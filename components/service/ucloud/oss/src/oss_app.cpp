//
// Created by fengcai on 2020/6/17.
//


#include "OssClient.h"
#include "iostream"
#include "fstream"
#include "string.h"
#include "oss_app.h"

//#define OSS_DEBUG

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

extern Url g_ags_url;
std::string g_url;
char* upload_local_file(char* localfilepath, char *keyId, char *keySecret, char *secretToken, char *endPoint, char *bucketName)
{
    /* 初始化OSS账号信息 */
    std::string AccessKeyId;
    std::string AccessKeySecret;
    std::string Endpoint;
    std::string BucketName;
    std::string SecretToken;
    /* yourObjectName表示上传文件到OSS时需要指定包含文件后缀在内的完整路径，例如abc/efg/123.jpg */
    std::string ObjectName ;

    char *p_url;
    char *pfile_path,file_path[256];

    AccessKeyId = keyId;
    AccessKeySecret = keySecret;
    Endpoint = endPoint;
    BucketName = bucketName;
    SecretToken = secretToken;

#ifdef OSS_DEBUG
    std::cout << "Input_AccessKeyId:" << AccessKeyId <<std::endl;
    std::cout << "Input_AccessKeySecret:" << AccessKeySecret <<std::endl;
    std::cout << "Input_Endpoint:" << Endpoint <<std::endl;
    std::cout << "Input_BucketName:" << BucketName <<std::endl;
    std::cout << "Input_SecretToken:" << SecretToken <<std::endl;
#endif
    memset(file_path,0,256);
    pfile_path = localfilepath;
    strncpy(file_path,&pfile_path[1],255);
    file_path[255] = '\0';

    /* 初始化网络等资源 */
    InitializeSdk();

    ClientConfiguration conf;
    OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
    //OssClient client(Endpoint, AccessKeyId, AccessKeySecret,SecretToken, conf);

    /* 上传文件 */
    auto fileSize = getFileSize(localfilepath);
#ifdef OSS_DEBUG
    std::cout << "localfile_path size:" << fileSize <<std::endl;
#endif
    std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(localfilepath, std::ios::in | std::ios::binary);
#ifdef OSS_DEBUG
    std::cout << "localfile_path:" << localfilepath <<std::endl;
#endif
    ObjectName = file_path;
    std::cout << "objectfile_path:" << ObjectName <<std::endl;
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
}

}

# component name
NAME := ucloud
# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := add aliyun open api include ai vision and so on.
# source files and the folder of internal include files

$(NAME)_SOURCES += core/src/AlibabaCloud.cc \
                core/src/AsyncCallerContext.cc \
                core/src/ClientConfiguration.cc \
                core/src/CommonClient.cc \
                core/src/CommonRequest.cc \
                core/src/CommonResponse.cc \
                core/src/CoreClient.cc \
                core/src/Credentials.cc \
                core/src/CredentialsProvider.cc \
                core/src/CurlHttpClient.cc \
                core/src/EcsMetadataFetcher.cc \
                core/src/EndpointProvider.cc \
                core/src/Error.cc \
                core/src/Executor.cc \
                core/src/HmacSha1Signer.cc \
                core/src/HttpClient.cc \
                core/src/HttpMessage.cc \
                core/src/HttpResponse.cc \
                core/src/HttpRequest.cc \
                core/src/InstanceProfileCredentialsProvider.cc \
                core/src/NetworkProxy.cc \
                core/src/Outcome.cc \
                core/src/Runnable.cc \
                core/src/RoaServiceClient.cc \
                core/src/RoaServiceRequest.cc \
                core/src/RpcServiceClient.cc \
                core/src/RpcServiceRequest.cc \
                core/src/ServiceRequest.cc \
                core/src/ServiceResult.cc \
                core/src/Signer.cc \
                core/src/SimpleCredentialsProvider.cc \
                core/src/StsAssumeRoleCredentialsProvider.cc \
                core/src/Url.cc \
                core/src/Utils.cc \
                core/src/location/LocationClient.cc \
                core/src/location/LocationRequest.cc \
                core/src/location/model/DescribeEndpointsRequest.cc \
                core/src/location/model/DescribeEndpointsResult.cc \
                core/src/sts/StsClient.cc \
                core/src/sts/StsRequest.cc \
                core/src/sts/model/AssumeRoleRequest.cc \
                core/src/sts/model/AssumeRoleResult.cc \
                core/src/sts/model/GetCallerIdentityRequest.cc \
                core/src/sts/model/GetCallerIdentityResult.cc

$(NAME)_INCLUDES += core/include
$(NAME)_INCLUDES += core/src

GLOBAL_INCLUDES += core/include
GLOBAL_INCLUDES += core/src

$(NAME)_COMPONENTS += mbedtls jsoncpp osal_posix

#face and body AI detection
ifeq ($(UCLOUD_AI_FACEBODY_ENABLE),y)
$(NAME)_COMPONENTS += facebody
endif

ifeq ($(UCLOUD_AI_OBJECTDET_ENABLE),y)
$(NAME)_COMPONENTS += objectdet
endif

#ifeq ($(UCLOUD_CONFIG_AI_OCR_ENABLE),y)
#$(NAME)_COMPONENTS += ocr
#endif

GLOBAL_DEFINES += USE_CRYPTO_MBEDTLS USE_AOS_TIME_POSIX_API

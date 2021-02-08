NAME := oss

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := oss component

$(NAME)_COMPONENTS += mbedtls

GLOBAL_DEFINES += OSS_DISABLE_BUCKET OSS_DISABLE_LIVECHANNEL OSS_DISABLE_RESUAMABLE OSS_DISABLE_ENCRYPTION

GLOBAL_DEFINES += USE_CRYPTO_MBEDTLS \
                    MBEDTLS_CONFIG_CRYPTO_MD5
                    #OPENSSL_NO_SOCK \
                    OPENSSL_NO_EC_NISTP_64_GCC_128 \
                    OPENSSL_NO_GMP \
                    OPENSSL_NO_JPAKE \
                    OPENSSL_NO_LIBUNBOUND \
                    OPENSSL_NO_MD2 \
                    OPENSSL_NO_RC5 \
                    OPENSSL_NO_RFC3779 \
                    OPENSSL_NO_SCTP \
                    OPENSSL_NO_SSL_TRACE \
                    OPENSSL_NO_SSL2 \
                    OPENSSL_NO_STORE \
                    OPENSSL_NO_UNIT_TEST \
                    OPENSSL_NO_WEAK_SSL_CIPHERS

sdk_auth_header := include/alibabacloud/oss/auth/
sdk_client_header := include/alibabacloud/oss/client/
sdk_http_header := include/alibabacloud/oss/http/
sdk_utils_header := include/alibabacloud/oss/utils/
sdk_model_header := include/alibabacloud/oss/model/
sdk_encryption_headregister_chrdever := include/alibabacloud/oss/encryption/
sdk_public_header := include/alibabacloud/oss/
sdk_api_header := src/

$(NAME)_INCLUDES += include/ \
                    $(sdk_auth_header) \
                    $(sdk_client_header) \
                    $(sdk_http_header) \
                    $(sdk_utils_header) \
                    $(sdk_model_header) \
                    $(sdk_encryption_header) \
                    $(sdk_public_header) \
                    $(sdk_api_header) \
                    src/external/ \
                    ../../kernel/components/network/lwip/include/posix \
                    ../../component/mbedtls/include/

sdk_auth_src := ./src/auth/Credentials.cc \
                ./src/auth/CredentialsProvider.cc \
                ./src/auth/HmacSha1Signer.cc \
                ./src/auth/Signer.cc \
                ./src/auth/SimpleCredentialsProvider.cc

sdk_client_src := ./src/client/AsyncCallerContext.cc \
                  ./src/client/ClientConfiguration.cc \
                  ./src/client/Client.cc

sdk_http_src := ./src/http/CurlHttpClient.cc \
                ./src/http/HttpClient.cc \
                ./src/http/HttpMessage.cc \
                ./src/http/HttpRequest.cc \
                ./src/http/HttpResponse.cc \
                ./src/http/Url.cc

sdk_utils_src := ./src/utils/Crc32.cc \
                 ./src/utils/Crc64.cc \
                 ./src/utils/Executor.cc \
                 ./src/utils/FileSystemUtils.cc \
                 ./src/utils/LogUtils.cc \
                 ./src/utils/Runnable.cc \
                 ./src/utils/SignUtils.cc \
                 ./src/utils/ThreadExecutor.cc \
                 ./src/utils/Utils.cc

sdk_public_src := ./src/OssClient.cc \
                  ./src/OssClientImpl.cc \
                  ./src/OssRequest.cc \
                  ./src/OssResponse.cc \
                  ./src/OssResult.cc \
                  ./src/ServiceRequest.cc \
                  ./src/oss_app.cpp

sdk_external_tinnyxml2_src := ./src/external/tinyxml2/tinyxml2.cpp

sdk_model_common_src := ./src/model/ModelError.cc


sdk_model_object_src := ./src/model/AppendObjectRequest.cc \
                       ./src/model/AppendObjectResult.cc \
                       ./src/model/CopyObjectRequest.cc \
                       ./src/model/CopyObjectResult.cc \
                       ./src/model/CreateSelectObjectMetaRequest.cc \
                       ./src/model/CreateSelectObjectMetaResult.cc \
                       ./src/model/DeleteObjectResult.cc \
                       ./src/model/DeleteObjectsRequest.cc \
                       ./src/model/DeleteObjectsResult.cc \
                       ./src/model/DeleteObjectTaggingRequest.cc \
                       ./src/model/DeleteObjectVersionsRequest.cc \
                       ./src/model/DeleteObjectVersionsResult.cc \
                       ./src/model/GetObjectAclRequest.cc \
                       ./src/model/GetObjectAclResult.cc \
                       ./src/model/GetObjectByUrlRequest.cc \
                       ./src/model/GetObjectMetaRequest.cc \
                       ./src/model/GetObjectRequest.cc \
                       ./src/model/GetObjectResult.cc \
                       ./src/model/GetObjectTaggingRequest.cc \
                       ./src/model/GetObjectTaggingResult.cc \
                       ./src/model/ListObjectsRequest.cc \
                       ./src/model/ListObjectsResult.cc \
                       ./src/model/ListObjectVersionsResult.cc \
                       ./src/model/ObjectCallbackBuilder.cc \
                       ./src/model/ObjectMetaData.cc \
                       ./src/model/ProcessObjectRequest.cc \
                       ./src/model/PutObjectByUrlRequest.cc \
                       ./src/model/PutObjectRequest.cc \
                       ./src/model/PutObjectResult.cc \
                       ./src/model/RestoreObjectRequest.cc \
                       ./src/model/RestoreObjectResult.cc \
                       ./src/model/SelectObjectRequest.cc \
                       ./src/model/SetObjectAclRequest.cc \
                       ./src/model/SetObjectAclResult.cc \
                       ./src/model/SetObjectTaggingRequest.cc

sdk_model_object_src += ./src/model/CreateSymlinkRequest.cc \
                       ./src/model/CreateSymlinkResult.cc \
                       ./src/model/GetSymlinkRequest.cc \
                       ./src/model/GetSymlinkResult.cc

sdk_model_object_src += ./src/model/InputFormat.cc \
                        ./src/model/OutputFormat.cc \
                        ./src/model/Tagging.cc \
                        ./src/model/GeneratePresignedUrlRequest.cc

sdk_model_multipart_src := ./src/model/AbortMultipartUploadRequest.cc \
                       ./src/model/CompleteMultipartUploadRequest.cc \
                       ./src/model/CompleteMultipartUploadResult.cc \
                       ./src/model/InitiateMultipartUploadRequest.cc \
                       ./src/model/InitiateMultipartUploadResult.cc \
                       ./src/model/ListMultipartUploadsRequest.cc \
                       ./src/model/ListMultipartUploadsResult.cc \
                       ./src/model/UploadPartCopyRequest.cc \
                       ./src/model/UploadPartCopyResult.cc \
                       ./src/model/UploadPartRequest.cc \
                       ./src/model/ListPartsRequest.cc \
                       ./src/model/ListPartsResult.cc

#### the following module is not included
#sdk_resumable_src
#sdk_encryption_src
#sdk_external_json_src
# OSS_DISABLE_BUCKET

sdk_src := $(sdk_client_src) \
                 $(sdk_auth_src) \
                 $(sdk_http_src) \
                 $(sdk_utils_src) \
                 $(sdk_public_src) \
                 $(sdk_external_tinnyxml2_src) \
                 $(sdk_model_common_src) \
                 $(sdk_model_object_src) \
                 $(sdk_model_multipart_src)

$(NAME)_SOURCES := $(sdk_src)
$(warn "sdk_src" $(NAME)_SOURCES)

GLOBAL_INCLUDES += src

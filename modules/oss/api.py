# -*- coding: utf-8 -*-

"""
Provider OSS(Object Storage Service) API for Alibaba Cloud.

For detailed API description, please refer to :
https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1LqDCqX#/Python/docs/zh-CN/haas_extended_api/oss

*** Note: The system time should be synced, otherwise Alibaba Cloud OSS server will deny the client's request. ***
"""

from . import auth
from . import bucket

def uploadFile(accessKey, accessSecret, endpoint, bucketName, filePath):
    """Upload local file to Alibaba Cloud OSS.

    Prerequisite: You have activated the Alibaba Cloud OSS service and created a bucket in it.
    This API upload a local file to the bucket you have created in Alibaba Cloud OSS, And keep
    the same file name in OSS bucket.

    Args:
        accessKey(string): The access key of your Alibaba Cloud account
        accessSecret(string): The access secret of your Alibaba Cloud account
        endpoint(string): The domain name of your Alibaba Cloud OSS's endpoint
        bucketName(string): The bucket name
        filePath(string): The file path to be uploaded

    Returns:
        url(string): The url that point to the file in OSS
    """
    auth_obj = auth.Auth(accessKey, accessSecret)
    bucket_obj = bucket.Bucket(auth_obj, endpoint, bucketName)
    key_index = filePath.rfind("/") + 1
    key = filePath[key_index:]
    url = bucket_obj.put_object_from_file(key, filePath)
    return url


def uploadContent(accessKey, accessSecret, endpoint, bucketName, content, ossFilePath):
    """Upload data to Alibaba Cloud OSS.

    Prerequisite: You have activated the Alibaba Cloud OSS service and created a bucket in it.
    This API upload a local data to the bucket you have created in Alibaba Cloud OSS named as ossFilePath

    Args:
        accessKey(string): The access key of your Alibaba Cloud account
        accessSecret(string): The access secret of your Alibaba Cloud account
        endpoint(string): The domain name of your Alibaba Cloud OSS's endpoint
        bucketName(string): The bucket name
        content: The data to be uploaded
        ossFilePath(string): The file name stored as in OSS

    Returns:
        url(string): The url that point to the file in OSS
    """

    auth_obj = auth.Auth(accessKey, accessSecret)
    bucket_obj = bucket.Bucket(auth_obj, endpoint, bucketName)
    url = bucket_obj.put_object(ossFilePath, content)
    return url

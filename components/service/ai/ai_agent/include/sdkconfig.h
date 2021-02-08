/*
 * Copyright (C) 2020-2023 Alibaba Group Holding Limited
 */

#ifndef _SDK_CONFIG_H_
#define _SDK_CONFIG_H_

/*Please fill your own key, secret and so on, which are created on aliyun website*/
/*Access http://oss.console.aliyun.com to get these information*/
//for oss sdk
#define OSS_ACCESS_KEY "YourAccessKeyFromAliyunAccount"
#define OSS_ACCESS_SECRET "YourAccessSecretFromYourAliyunAccount"
#define OSS_ENDPOINT "oss-cn-shanghai.aliyuncs.com"
#define OSS_BUCKET "YourOSSBucketName"

/*for vision ai sdk
Access http://vision.aliyun.com to get these information*/
#define CLOUD_AI_ACCESS_KEY OSS_ACCESS_KEY
#define CLOUD_AI_ACCESS_SECRET OSS_ACCESS_SECRET
#define CLOUD_AI_REGION_ID "cn-shanghai"
#define CLOUD_AI_FACEBODY_ENDPOINT "facebody.cn-shanghai.aliyuncs.com"
#define CLOUD_AI_OBJECTDET_ENDPOINT "objectdet.cn-shanghai.aliyuncs.com"

/*for linkkit sdk
Access http://iot.console.aliyun.com or http://studio.iot.aliyun.com to get these information*/
#define PRODUCT_KEY "YourProductKey"
#define PRODUCT_SECRET "YourProductSecret"
#define DEVICE_NAME "YourDeviceName"
#define DEVICE_SECRET "YourDeviceSecret"

/*wificamera url
Get from your wifi camera log*/
#define WIFICAMERA_URL "http://192.168.1.160:80/capture"

//Choose AI Capability
#define AI_MODEL AI_MODEL_COMPARING_FACEBODY

/*Fill your name image url on oss*/
#define MYFACE_PATH "http://your_bucket_name.oss-cn-shanghai.aliyuncs.com/data/your_face_image.jpg"

#endif //_SDK_CONFIG_H_
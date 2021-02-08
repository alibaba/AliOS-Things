/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <alibabacloud/cloudphoto/model/CreatePhotoStoreRequest.h>

using AlibabaCloud::CloudPhoto::Model::CreatePhotoStoreRequest;

CreatePhotoStoreRequest::CreatePhotoStoreRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "CreatePhotoStore")
{
	setMethod(HttpRequest::Method::Post);
}

CreatePhotoStoreRequest::~CreatePhotoStoreRequest()
{}

std::string CreatePhotoStoreRequest::getStoreName()const
{
	return storeName_;
}

void CreatePhotoStoreRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string CreatePhotoStoreRequest::getRemark()const
{
	return remark_;
}

void CreatePhotoStoreRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

long CreatePhotoStoreRequest::getDefaultQuota()const
{
	return defaultQuota_;
}

void CreatePhotoStoreRequest::setDefaultQuota(long defaultQuota)
{
	defaultQuota_ = defaultQuota;
	setParameter("DefaultQuota", std::to_string(defaultQuota));
}

std::string CreatePhotoStoreRequest::getBucketName()const
{
	return bucketName_;
}

void CreatePhotoStoreRequest::setBucketName(const std::string& bucketName)
{
	bucketName_ = bucketName;
	setParameter("BucketName", bucketName);
}


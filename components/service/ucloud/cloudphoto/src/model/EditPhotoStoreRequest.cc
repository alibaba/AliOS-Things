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

#include <alibabacloud/cloudphoto/model/EditPhotoStoreRequest.h>

using AlibabaCloud::CloudPhoto::Model::EditPhotoStoreRequest;

EditPhotoStoreRequest::EditPhotoStoreRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "EditPhotoStore")
{
	setMethod(HttpRequest::Method::Post);
}

EditPhotoStoreRequest::~EditPhotoStoreRequest()
{}

std::string EditPhotoStoreRequest::getAutoCleanEnabled()const
{
	return autoCleanEnabled_;
}

void EditPhotoStoreRequest::setAutoCleanEnabled(const std::string& autoCleanEnabled)
{
	autoCleanEnabled_ = autoCleanEnabled;
	setParameter("AutoCleanEnabled", autoCleanEnabled);
}

std::string EditPhotoStoreRequest::getStoreName()const
{
	return storeName_;
}

void EditPhotoStoreRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string EditPhotoStoreRequest::getRemark()const
{
	return remark_;
}

void EditPhotoStoreRequest::setRemark(const std::string& remark)
{
	remark_ = remark;
	setParameter("Remark", remark);
}

long EditPhotoStoreRequest::getDefaultQuota()const
{
	return defaultQuota_;
}

void EditPhotoStoreRequest::setDefaultQuota(long defaultQuota)
{
	defaultQuota_ = defaultQuota;
	setParameter("DefaultQuota", std::to_string(defaultQuota));
}

int EditPhotoStoreRequest::getAutoCleanDays()const
{
	return autoCleanDays_;
}

void EditPhotoStoreRequest::setAutoCleanDays(int autoCleanDays)
{
	autoCleanDays_ = autoCleanDays;
	setParameter("AutoCleanDays", std::to_string(autoCleanDays));
}

long EditPhotoStoreRequest::getDefaultTrashQuota()const
{
	return defaultTrashQuota_;
}

void EditPhotoStoreRequest::setDefaultTrashQuota(long defaultTrashQuota)
{
	defaultTrashQuota_ = defaultTrashQuota;
	setParameter("DefaultTrashQuota", std::to_string(defaultTrashQuota));
}


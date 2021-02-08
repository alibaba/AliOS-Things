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

#include <alibabacloud/cloudphoto/model/SetQuotaRequest.h>

using AlibabaCloud::CloudPhoto::Model::SetQuotaRequest;

SetQuotaRequest::SetQuotaRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "SetQuota")
{
	setMethod(HttpRequest::Method::Post);
}

SetQuotaRequest::~SetQuotaRequest()
{}

long SetQuotaRequest::getTotalQuota()const
{
	return totalQuota_;
}

void SetQuotaRequest::setTotalQuota(long totalQuota)
{
	totalQuota_ = totalQuota;
	setParameter("TotalQuota", std::to_string(totalQuota));
}

std::string SetQuotaRequest::getLibraryId()const
{
	return libraryId_;
}

void SetQuotaRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string SetQuotaRequest::getStoreName()const
{
	return storeName_;
}

void SetQuotaRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}


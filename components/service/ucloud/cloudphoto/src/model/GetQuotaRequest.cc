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

#include <alibabacloud/cloudphoto/model/GetQuotaRequest.h>

using AlibabaCloud::CloudPhoto::Model::GetQuotaRequest;

GetQuotaRequest::GetQuotaRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "GetQuota")
{
	setMethod(HttpRequest::Method::Post);
}

GetQuotaRequest::~GetQuotaRequest()
{}

std::string GetQuotaRequest::getLibraryId()const
{
	return libraryId_;
}

void GetQuotaRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string GetQuotaRequest::getStoreName()const
{
	return storeName_;
}

void GetQuotaRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}


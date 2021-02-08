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

#include <alibabacloud/cloudphoto/model/FetchLibrariesRequest.h>

using AlibabaCloud::CloudPhoto::Model::FetchLibrariesRequest;

FetchLibrariesRequest::FetchLibrariesRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "FetchLibraries")
{
	setMethod(HttpRequest::Method::Post);
}

FetchLibrariesRequest::~FetchLibrariesRequest()
{}

bool FetchLibrariesRequest::getNeedQuota()const
{
	return needQuota_;
}

void FetchLibrariesRequest::setNeedQuota(bool needQuota)
{
	needQuota_ = needQuota;
	setParameter("NeedQuota", needQuota ? "true" : "false");
}

std::string FetchLibrariesRequest::getStoreName()const
{
	return storeName_;
}

void FetchLibrariesRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

int FetchLibrariesRequest::getSize()const
{
	return size_;
}

void FetchLibrariesRequest::setSize(int size)
{
	size_ = size;
	setParameter("Size", std::to_string(size));
}

int FetchLibrariesRequest::getPage()const
{
	return page_;
}

void FetchLibrariesRequest::setPage(int page)
{
	page_ = page;
	setParameter("Page", std::to_string(page));
}


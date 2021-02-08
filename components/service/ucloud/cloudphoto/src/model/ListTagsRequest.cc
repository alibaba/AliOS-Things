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

#include <alibabacloud/cloudphoto/model/ListTagsRequest.h>

using AlibabaCloud::CloudPhoto::Model::ListTagsRequest;

ListTagsRequest::ListTagsRequest() :
	RpcServiceRequest("cloudphoto", "2017-07-11", "ListTags")
{
	setMethod(HttpRequest::Method::Post);
}

ListTagsRequest::~ListTagsRequest()
{}

std::string ListTagsRequest::getLibraryId()const
{
	return libraryId_;
}

void ListTagsRequest::setLibraryId(const std::string& libraryId)
{
	libraryId_ = libraryId;
	setParameter("LibraryId", libraryId);
}

std::string ListTagsRequest::getStoreName()const
{
	return storeName_;
}

void ListTagsRequest::setStoreName(const std::string& storeName)
{
	storeName_ = storeName;
	setParameter("StoreName", storeName);
}

std::string ListTagsRequest::getLang()const
{
	return lang_;
}

void ListTagsRequest::setLang(const std::string& lang)
{
	lang_ = lang;
	setParameter("Lang", lang);
}


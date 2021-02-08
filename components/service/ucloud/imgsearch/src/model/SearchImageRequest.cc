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

#include <alibabacloud/imgsearch/model/SearchImageRequest.h>

using AlibabaCloud::Imgsearch::Model::SearchImageRequest;

SearchImageRequest::SearchImageRequest() :
	RpcServiceRequest("imgsearch", "2020-03-20", "SearchImage")
{
	setMethod(HttpRequest::Method::Post);
}

SearchImageRequest::~SearchImageRequest()
{}

std::string SearchImageRequest::getDbName()const
{
	return dbName_;
}

void SearchImageRequest::setDbName(const std::string& dbName)
{
	dbName_ = dbName;
	setBodyParameter("DbName", dbName);
}

std::string SearchImageRequest::getImageUrl()const
{
	return imageUrl_;
}

void SearchImageRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setBodyParameter("ImageUrl", imageUrl);
}

int SearchImageRequest::getLimit()const
{
	return limit_;
}

void SearchImageRequest::setLimit(int limit)
{
	limit_ = limit;
	setBodyParameter("Limit", std::to_string(limit));
}


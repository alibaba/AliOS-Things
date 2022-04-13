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

#include <alibabacloud/facebody/model/SearchFaceRequest.h>

using AlibabaCloud::Facebody::Model::SearchFaceRequest;

SearchFaceRequest::SearchFaceRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "SearchFace")
{
	setMethod(HttpRequest::Method::Post);
}

SearchFaceRequest::~SearchFaceRequest()
{}

std::string SearchFaceRequest::getDbNames()const
{
	return dbNames_;
}

void SearchFaceRequest::setDbNames(const std::string& dbNames)
{
	dbNames_ = dbNames;
	setParameter("DbNames", dbNames);
}

std::string SearchFaceRequest::getDbName()const
{
	return dbName_;
}

void SearchFaceRequest::setDbName(const std::string& dbName)
{
	dbName_ = dbName;
	setBodyParameter("DbName", dbName);
}

std::string SearchFaceRequest::getImageUrl()const
{
	return imageUrl_;
}

void SearchFaceRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setBodyParameter("ImageUrl", imageUrl);
}

int SearchFaceRequest::getLimit()const
{
	return limit_;
}

void SearchFaceRequest::setLimit(int limit)
{
	limit_ = limit;
	setBodyParameter("Limit", std::to_string(limit));
}


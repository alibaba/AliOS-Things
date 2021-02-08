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

#include <alibabacloud/imgsearch/model/ListImagesRequest.h>

using AlibabaCloud::Imgsearch::Model::ListImagesRequest;

ListImagesRequest::ListImagesRequest() :
	RpcServiceRequest("imgsearch", "2020-03-20", "ListImages")
{
	setMethod(HttpRequest::Method::Post);
}

ListImagesRequest::~ListImagesRequest()
{}

std::string ListImagesRequest::getEntityIdPrefix()const
{
	return entityIdPrefix_;
}

void ListImagesRequest::setEntityIdPrefix(const std::string& entityIdPrefix)
{
	entityIdPrefix_ = entityIdPrefix;
	setBodyParameter("EntityIdPrefix", entityIdPrefix);
}

int ListImagesRequest::getLimit()const
{
	return limit_;
}

void ListImagesRequest::setLimit(int limit)
{
	limit_ = limit;
	setBodyParameter("Limit", std::to_string(limit));
}

std::string ListImagesRequest::getOrder()const
{
	return order_;
}

void ListImagesRequest::setOrder(const std::string& order)
{
	order_ = order;
	setBodyParameter("Order", order);
}

int ListImagesRequest::getOffset()const
{
	return offset_;
}

void ListImagesRequest::setOffset(int offset)
{
	offset_ = offset;
	setBodyParameter("Offset", std::to_string(offset));
}

std::string ListImagesRequest::getToken()const
{
	return token_;
}

void ListImagesRequest::setToken(const std::string& token)
{
	token_ = token;
	setBodyParameter("Token", token);
}

std::string ListImagesRequest::getDbName()const
{
	return dbName_;
}

void ListImagesRequest::setDbName(const std::string& dbName)
{
	dbName_ = dbName;
	setBodyParameter("DbName", dbName);
}


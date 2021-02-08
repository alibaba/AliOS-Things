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

#include <alibabacloud/imgsearch/model/AddImageRequest.h>

using AlibabaCloud::Imgsearch::Model::AddImageRequest;

AddImageRequest::AddImageRequest() :
	RpcServiceRequest("imgsearch", "2020-03-20", "AddImage")
{
	setMethod(HttpRequest::Method::Post);
}

AddImageRequest::~AddImageRequest()
{}

std::string AddImageRequest::getEntityId()const
{
	return entityId_;
}

void AddImageRequest::setEntityId(const std::string& entityId)
{
	entityId_ = entityId;
	setBodyParameter("EntityId", entityId);
}

std::string AddImageRequest::getDbName()const
{
	return dbName_;
}

void AddImageRequest::setDbName(const std::string& dbName)
{
	dbName_ = dbName;
	setBodyParameter("DbName", dbName);
}

std::string AddImageRequest::getImageUrl()const
{
	return imageUrl_;
}

void AddImageRequest::setImageUrl(const std::string& imageUrl)
{
	imageUrl_ = imageUrl;
	setBodyParameter("ImageUrl", imageUrl);
}

std::string AddImageRequest::getExtraData()const
{
	return extraData_;
}

void AddImageRequest::setExtraData(const std::string& extraData)
{
	extraData_ = extraData;
	setBodyParameter("ExtraData", extraData);
}


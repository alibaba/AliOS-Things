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

#include <alibabacloud/facebody/model/AddFaceEntityRequest.h>

using AlibabaCloud::Facebody::Model::AddFaceEntityRequest;

AddFaceEntityRequest::AddFaceEntityRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "AddFaceEntity")
{
	setMethod(HttpRequest::Method::Post);
}

AddFaceEntityRequest::~AddFaceEntityRequest()
{}

std::string AddFaceEntityRequest::getEntityId()const
{
	return entityId_;
}

void AddFaceEntityRequest::setEntityId(const std::string& entityId)
{
	entityId_ = entityId;
	setBodyParameter("EntityId", entityId);
}

std::string AddFaceEntityRequest::getLabels()const
{
	return labels_;
}

void AddFaceEntityRequest::setLabels(const std::string& labels)
{
	labels_ = labels;
	setBodyParameter("Labels", labels);
}

std::string AddFaceEntityRequest::getDbName()const
{
	return dbName_;
}

void AddFaceEntityRequest::setDbName(const std::string& dbName)
{
	dbName_ = dbName;
	setBodyParameter("DbName", dbName);
}


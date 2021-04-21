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

#include <alibabacloud/facebody/model/DeleteFaceRequest.h>

using AlibabaCloud::Facebody::Model::DeleteFaceRequest;

DeleteFaceRequest::DeleteFaceRequest() :
	RpcServiceRequest("facebody", "2019-12-30", "DeleteFace")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteFaceRequest::~DeleteFaceRequest()
{}

std::string DeleteFaceRequest::getFaceId()const
{
	return faceId_;
}

void DeleteFaceRequest::setFaceId(const std::string& faceId)
{
	faceId_ = faceId;
	setBodyParameter("FaceId", faceId);
}

std::string DeleteFaceRequest::getDbName()const
{
	return dbName_;
}

void DeleteFaceRequest::setDbName(const std::string& dbName)
{
	dbName_ = dbName;
	setBodyParameter("DbName", dbName);
}


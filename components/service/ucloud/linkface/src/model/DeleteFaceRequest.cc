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

#include <alibabacloud/linkface/model/DeleteFaceRequest.h>

using AlibabaCloud::LinkFace::Model::DeleteFaceRequest;

DeleteFaceRequest::DeleteFaceRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "DeleteFace")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteFaceRequest::~DeleteFaceRequest()
{}

std::string DeleteFaceRequest::getUserId()const
{
	return userId_;
}

void DeleteFaceRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

std::string DeleteFaceRequest::getGroupId()const
{
	return groupId_;
}

void DeleteFaceRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setBodyParameter("GroupId", groupId);
}

std::string DeleteFaceRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteFaceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteFaceRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteFaceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


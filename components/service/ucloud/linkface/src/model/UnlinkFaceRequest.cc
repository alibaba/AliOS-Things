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

#include <alibabacloud/linkface/model/UnlinkFaceRequest.h>

using AlibabaCloud::LinkFace::Model::UnlinkFaceRequest;

UnlinkFaceRequest::UnlinkFaceRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "UnlinkFace")
{
	setMethod(HttpRequest::Method::Post);
}

UnlinkFaceRequest::~UnlinkFaceRequest()
{}

std::string UnlinkFaceRequest::getUserId()const
{
	return userId_;
}

void UnlinkFaceRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}

std::string UnlinkFaceRequest::getGroupId()const
{
	return groupId_;
}

void UnlinkFaceRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setBodyParameter("GroupId", groupId);
}

std::string UnlinkFaceRequest::getApiProduct()const
{
	return apiProduct_;
}

void UnlinkFaceRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string UnlinkFaceRequest::getApiRevision()const
{
	return apiRevision_;
}

void UnlinkFaceRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


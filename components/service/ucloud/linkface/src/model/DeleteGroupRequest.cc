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

#include <alibabacloud/linkface/model/DeleteGroupRequest.h>

using AlibabaCloud::LinkFace::Model::DeleteGroupRequest;

DeleteGroupRequest::DeleteGroupRequest() :
	RpcServiceRequest("linkface", "2018-07-20", "DeleteGroup")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteGroupRequest::~DeleteGroupRequest()
{}

std::string DeleteGroupRequest::getGroupId()const
{
	return groupId_;
}

void DeleteGroupRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setBodyParameter("GroupId", groupId);
}

std::string DeleteGroupRequest::getApiProduct()const
{
	return apiProduct_;
}

void DeleteGroupRequest::setApiProduct(const std::string& apiProduct)
{
	apiProduct_ = apiProduct;
	setBodyParameter("ApiProduct", apiProduct);
}

std::string DeleteGroupRequest::getApiRevision()const
{
	return apiRevision_;
}

void DeleteGroupRequest::setApiRevision(const std::string& apiRevision)
{
	apiRevision_ = apiRevision;
	setBodyParameter("ApiRevision", apiRevision);
}


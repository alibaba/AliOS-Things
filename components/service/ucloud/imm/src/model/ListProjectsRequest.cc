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

#include <alibabacloud/imm/model/ListProjectsRequest.h>

using AlibabaCloud::Imm::Model::ListProjectsRequest;

ListProjectsRequest::ListProjectsRequest() :
	RpcServiceRequest("imm", "2017-09-06", "ListProjects")
{
	setMethod(HttpRequest::Method::Post);
}

ListProjectsRequest::~ListProjectsRequest()
{}

int ListProjectsRequest::getMaxKeys()const
{
	return maxKeys_;
}

void ListProjectsRequest::setMaxKeys(int maxKeys)
{
	maxKeys_ = maxKeys;
	setParameter("MaxKeys", std::to_string(maxKeys));
}

std::string ListProjectsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListProjectsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListProjectsRequest::getMarker()const
{
	return marker_;
}

void ListProjectsRequest::setMarker(const std::string& marker)
{
	marker_ = marker;
	setParameter("Marker", marker);
}


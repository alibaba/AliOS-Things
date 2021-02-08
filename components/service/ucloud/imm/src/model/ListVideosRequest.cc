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

#include <alibabacloud/imm/model/ListVideosRequest.h>

using AlibabaCloud::Imm::Model::ListVideosRequest;

ListVideosRequest::ListVideosRequest() :
	RpcServiceRequest("imm", "2017-09-06", "ListVideos")
{
	setMethod(HttpRequest::Method::Post);
}

ListVideosRequest::~ListVideosRequest()
{}

std::string ListVideosRequest::getProject()const
{
	return project_;
}

void ListVideosRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string ListVideosRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListVideosRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string ListVideosRequest::getMarker()const
{
	return marker_;
}

void ListVideosRequest::setMarker(const std::string& marker)
{
	marker_ = marker;
	setParameter("Marker", marker);
}

std::string ListVideosRequest::getSetId()const
{
	return setId_;
}

void ListVideosRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}

std::string ListVideosRequest::getCreateTimeStart()const
{
	return createTimeStart_;
}

void ListVideosRequest::setCreateTimeStart(const std::string& createTimeStart)
{
	createTimeStart_ = createTimeStart;
	setParameter("CreateTimeStart", createTimeStart);
}


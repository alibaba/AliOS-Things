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

#include <alibabacloud/imm/model/ListImagesRequest.h>

using AlibabaCloud::Imm::Model::ListImagesRequest;

ListImagesRequest::ListImagesRequest() :
	RpcServiceRequest("imm", "2017-09-06", "ListImages")
{
	setMethod(HttpRequest::Method::Post);
}

ListImagesRequest::~ListImagesRequest()
{}

std::string ListImagesRequest::getProject()const
{
	return project_;
}

void ListImagesRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string ListImagesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void ListImagesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

int ListImagesRequest::getLimit()const
{
	return limit_;
}

void ListImagesRequest::setLimit(int limit)
{
	limit_ = limit;
	setParameter("Limit", std::to_string(limit));
}

std::string ListImagesRequest::getMarker()const
{
	return marker_;
}

void ListImagesRequest::setMarker(const std::string& marker)
{
	marker_ = marker;
	setParameter("Marker", marker);
}

std::string ListImagesRequest::getSetId()const
{
	return setId_;
}

void ListImagesRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}

std::string ListImagesRequest::getCreateTimeStart()const
{
	return createTimeStart_;
}

void ListImagesRequest::setCreateTimeStart(const std::string& createTimeStart)
{
	createTimeStart_ = createTimeStart;
	setParameter("CreateTimeStart", createTimeStart);
}


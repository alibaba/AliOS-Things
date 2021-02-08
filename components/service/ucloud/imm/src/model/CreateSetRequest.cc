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

#include <alibabacloud/imm/model/CreateSetRequest.h>

using AlibabaCloud::Imm::Model::CreateSetRequest;

CreateSetRequest::CreateSetRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateSet")
{
	setMethod(HttpRequest::Method::Post);
}

CreateSetRequest::~CreateSetRequest()
{}

std::string CreateSetRequest::getProject()const
{
	return project_;
}

void CreateSetRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateSetRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateSetRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateSetRequest::getSetName()const
{
	return setName_;
}

void CreateSetRequest::setSetName(const std::string& setName)
{
	setName_ = setName;
	setParameter("SetName", setName);
}

std::string CreateSetRequest::getSetId()const
{
	return setId_;
}

void CreateSetRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}


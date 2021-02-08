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

#include <alibabacloud/imm/model/UpdateProjectRequest.h>

using AlibabaCloud::Imm::Model::UpdateProjectRequest;

UpdateProjectRequest::UpdateProjectRequest() :
	RpcServiceRequest("imm", "2017-09-06", "UpdateProject")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateProjectRequest::~UpdateProjectRequest()
{}

std::string UpdateProjectRequest::getProject()const
{
	return project_;
}

void UpdateProjectRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string UpdateProjectRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateProjectRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateProjectRequest::getNewServiceRole()const
{
	return newServiceRole_;
}

void UpdateProjectRequest::setNewServiceRole(const std::string& newServiceRole)
{
	newServiceRole_ = newServiceRole;
	setParameter("NewServiceRole", newServiceRole);
}

int UpdateProjectRequest::getNewCU()const
{
	return newCU_;
}

void UpdateProjectRequest::setNewCU(int newCU)
{
	newCU_ = newCU;
	setParameter("NewCU", std::to_string(newCU));
}


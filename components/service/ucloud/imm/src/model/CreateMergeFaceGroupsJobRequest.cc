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

#include <alibabacloud/imm/model/CreateMergeFaceGroupsJobRequest.h>

using AlibabaCloud::Imm::Model::CreateMergeFaceGroupsJobRequest;

CreateMergeFaceGroupsJobRequest::CreateMergeFaceGroupsJobRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateMergeFaceGroupsJob")
{
	setMethod(HttpRequest::Method::Post);
}

CreateMergeFaceGroupsJobRequest::~CreateMergeFaceGroupsJobRequest()
{}

std::string CreateMergeFaceGroupsJobRequest::getProject()const
{
	return project_;
}

void CreateMergeFaceGroupsJobRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateMergeFaceGroupsJobRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateMergeFaceGroupsJobRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateMergeFaceGroupsJobRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateMergeFaceGroupsJobRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string CreateMergeFaceGroupsJobRequest::getGroupIdFrom()const
{
	return groupIdFrom_;
}

void CreateMergeFaceGroupsJobRequest::setGroupIdFrom(const std::string& groupIdFrom)
{
	groupIdFrom_ = groupIdFrom;
	setParameter("GroupIdFrom", groupIdFrom);
}

std::string CreateMergeFaceGroupsJobRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateMergeFaceGroupsJobRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string CreateMergeFaceGroupsJobRequest::getGroupIdTo()const
{
	return groupIdTo_;
}

void CreateMergeFaceGroupsJobRequest::setGroupIdTo(const std::string& groupIdTo)
{
	groupIdTo_ = groupIdTo;
	setParameter("GroupIdTo", groupIdTo);
}

std::string CreateMergeFaceGroupsJobRequest::getSetId()const
{
	return setId_;
}

void CreateMergeFaceGroupsJobRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}


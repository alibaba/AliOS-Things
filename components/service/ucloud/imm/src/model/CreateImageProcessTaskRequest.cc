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

#include <alibabacloud/imm/model/CreateImageProcessTaskRequest.h>

using AlibabaCloud::Imm::Model::CreateImageProcessTaskRequest;

CreateImageProcessTaskRequest::CreateImageProcessTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateImageProcessTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateImageProcessTaskRequest::~CreateImageProcessTaskRequest()
{}

std::string CreateImageProcessTaskRequest::getProject()const
{
	return project_;
}

void CreateImageProcessTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateImageProcessTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateImageProcessTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateImageProcessTaskRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateImageProcessTaskRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string CreateImageProcessTaskRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateImageProcessTaskRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string CreateImageProcessTaskRequest::getTargetList()const
{
	return targetList_;
}

void CreateImageProcessTaskRequest::setTargetList(const std::string& targetList)
{
	targetList_ = targetList;
	setParameter("TargetList", targetList);
}

std::string CreateImageProcessTaskRequest::getImageUri()const
{
	return imageUri_;
}

void CreateImageProcessTaskRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}


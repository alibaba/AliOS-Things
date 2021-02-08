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

#include <alibabacloud/imm/model/CreateGrabFrameTaskRequest.h>

using AlibabaCloud::Imm::Model::CreateGrabFrameTaskRequest;

CreateGrabFrameTaskRequest::CreateGrabFrameTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateGrabFrameTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateGrabFrameTaskRequest::~CreateGrabFrameTaskRequest()
{}

std::string CreateGrabFrameTaskRequest::getProject()const
{
	return project_;
}

void CreateGrabFrameTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateGrabFrameTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateGrabFrameTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateGrabFrameTaskRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateGrabFrameTaskRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string CreateGrabFrameTaskRequest::getCustomMessage()const
{
	return customMessage_;
}

void CreateGrabFrameTaskRequest::setCustomMessage(const std::string& customMessage)
{
	customMessage_ = customMessage;
	setParameter("CustomMessage", customMessage);
}

std::string CreateGrabFrameTaskRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateGrabFrameTaskRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string CreateGrabFrameTaskRequest::getTargetList()const
{
	return targetList_;
}

void CreateGrabFrameTaskRequest::setTargetList(const std::string& targetList)
{
	targetList_ = targetList;
	setParameter("TargetList", targetList);
}

std::string CreateGrabFrameTaskRequest::getVideoUri()const
{
	return videoUri_;
}

void CreateGrabFrameTaskRequest::setVideoUri(const std::string& videoUri)
{
	videoUri_ = videoUri;
	setParameter("VideoUri", videoUri);
}


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

#include <alibabacloud/imm/model/CreateVideoAbstractTaskRequest.h>

using AlibabaCloud::Imm::Model::CreateVideoAbstractTaskRequest;

CreateVideoAbstractTaskRequest::CreateVideoAbstractTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateVideoAbstractTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateVideoAbstractTaskRequest::~CreateVideoAbstractTaskRequest()
{}

std::string CreateVideoAbstractTaskRequest::getTargetVideoUri()const
{
	return targetVideoUri_;
}

void CreateVideoAbstractTaskRequest::setTargetVideoUri(const std::string& targetVideoUri)
{
	targetVideoUri_ = targetVideoUri;
	setParameter("TargetVideoUri", targetVideoUri);
}

std::string CreateVideoAbstractTaskRequest::getProject()const
{
	return project_;
}

void CreateVideoAbstractTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateVideoAbstractTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateVideoAbstractTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateVideoAbstractTaskRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateVideoAbstractTaskRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string CreateVideoAbstractTaskRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateVideoAbstractTaskRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string CreateVideoAbstractTaskRequest::getVideoUri()const
{
	return videoUri_;
}

void CreateVideoAbstractTaskRequest::setVideoUri(const std::string& videoUri)
{
	videoUri_ = videoUri;
	setParameter("VideoUri", videoUri);
}

int CreateVideoAbstractTaskRequest::getAbstractLength()const
{
	return abstractLength_;
}

void CreateVideoAbstractTaskRequest::setAbstractLength(int abstractLength)
{
	abstractLength_ = abstractLength;
	setParameter("AbstractLength", std::to_string(abstractLength));
}

std::string CreateVideoAbstractTaskRequest::getTargetClipsUri()const
{
	return targetClipsUri_;
}

void CreateVideoAbstractTaskRequest::setTargetClipsUri(const std::string& targetClipsUri)
{
	targetClipsUri_ = targetClipsUri;
	setParameter("TargetClipsUri", targetClipsUri);
}


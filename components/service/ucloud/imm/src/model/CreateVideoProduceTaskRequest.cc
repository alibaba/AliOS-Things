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

#include <alibabacloud/imm/model/CreateVideoProduceTaskRequest.h>

using AlibabaCloud::Imm::Model::CreateVideoProduceTaskRequest;

CreateVideoProduceTaskRequest::CreateVideoProduceTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateVideoProduceTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateVideoProduceTaskRequest::~CreateVideoProduceTaskRequest()
{}

std::string CreateVideoProduceTaskRequest::getProject()const
{
	return project_;
}

void CreateVideoProduceTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateVideoProduceTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateVideoProduceTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateVideoProduceTaskRequest::getMusic()const
{
	return music_;
}

void CreateVideoProduceTaskRequest::setMusic(const std::string& music)
{
	music_ = music;
	setParameter("Music", music);
}

std::string CreateVideoProduceTaskRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateVideoProduceTaskRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string CreateVideoProduceTaskRequest::getTargetUri()const
{
	return targetUri_;
}

void CreateVideoProduceTaskRequest::setTargetUri(const std::string& targetUri)
{
	targetUri_ = targetUri;
	setParameter("TargetUri", targetUri);
}

std::string CreateVideoProduceTaskRequest::getTemplateName()const
{
	return templateName_;
}

void CreateVideoProduceTaskRequest::setTemplateName(const std::string& templateName)
{
	templateName_ = templateName;
	setParameter("TemplateName", templateName);
}

int CreateVideoProduceTaskRequest::getHeight()const
{
	return height_;
}

void CreateVideoProduceTaskRequest::setHeight(int height)
{
	height_ = height;
	setParameter("Height", std::to_string(height));
}

std::string CreateVideoProduceTaskRequest::getCustomMessage()const
{
	return customMessage_;
}

void CreateVideoProduceTaskRequest::setCustomMessage(const std::string& customMessage)
{
	customMessage_ = customMessage;
	setParameter("CustomMessage", customMessage);
}

std::string CreateVideoProduceTaskRequest::getImages()const
{
	return images_;
}

void CreateVideoProduceTaskRequest::setImages(const std::string& images)
{
	images_ = images;
	setParameter("Images", images);
}

std::string CreateVideoProduceTaskRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateVideoProduceTaskRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

int CreateVideoProduceTaskRequest::getWidth()const
{
	return width_;
}

void CreateVideoProduceTaskRequest::setWidth(int width)
{
	width_ = width;
	setParameter("Width", std::to_string(width));
}


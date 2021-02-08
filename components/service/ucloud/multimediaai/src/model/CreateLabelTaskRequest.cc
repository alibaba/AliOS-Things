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

#include <alibabacloud/multimediaai/model/CreateLabelTaskRequest.h>

using AlibabaCloud::Multimediaai::Model::CreateLabelTaskRequest;

CreateLabelTaskRequest::CreateLabelTaskRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "CreateLabelTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateLabelTaskRequest::~CreateLabelTaskRequest()
{}

long CreateLabelTaskRequest::getTemplateId()const
{
	return templateId_;
}

void CreateLabelTaskRequest::setTemplateId(long templateId)
{
	templateId_ = templateId;
	setParameter("TemplateId", std::to_string(templateId));
}

std::string CreateLabelTaskRequest::getVideoUrl()const
{
	return videoUrl_;
}

void CreateLabelTaskRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setParameter("VideoUrl", videoUrl);
}

std::string CreateLabelTaskRequest::getVideoName()const
{
	return videoName_;
}

void CreateLabelTaskRequest::setVideoName(const std::string& videoName)
{
	videoName_ = videoName;
	setParameter("VideoName", videoName);
}

std::string CreateLabelTaskRequest::getCallbackUrl()const
{
	return callbackUrl_;
}

void CreateLabelTaskRequest::setCallbackUrl(const std::string& callbackUrl)
{
	callbackUrl_ = callbackUrl;
	setParameter("CallbackUrl", callbackUrl);
}

std::string CreateLabelTaskRequest::getApplicationId()const
{
	return applicationId_;
}

void CreateLabelTaskRequest::setApplicationId(const std::string& applicationId)
{
	applicationId_ = applicationId;
	setParameter("ApplicationId", applicationId);
}


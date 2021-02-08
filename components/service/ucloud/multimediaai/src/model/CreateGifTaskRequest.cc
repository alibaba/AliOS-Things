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

#include <alibabacloud/multimediaai/model/CreateGifTaskRequest.h>

using AlibabaCloud::Multimediaai::Model::CreateGifTaskRequest;

CreateGifTaskRequest::CreateGifTaskRequest() :
	RpcServiceRequest("multimediaai", "2019-08-10", "CreateGifTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateGifTaskRequest::~CreateGifTaskRequest()
{}

long CreateGifTaskRequest::getTemplateId()const
{
	return templateId_;
}

void CreateGifTaskRequest::setTemplateId(long templateId)
{
	templateId_ = templateId;
	setParameter("TemplateId", std::to_string(templateId));
}

std::string CreateGifTaskRequest::getVideoUrl()const
{
	return videoUrl_;
}

void CreateGifTaskRequest::setVideoUrl(const std::string& videoUrl)
{
	videoUrl_ = videoUrl;
	setParameter("VideoUrl", videoUrl);
}

std::string CreateGifTaskRequest::getScales()const
{
	return scales_;
}

void CreateGifTaskRequest::setScales(const std::string& scales)
{
	scales_ = scales;
	setBodyParameter("Scales", scales);
}

std::string CreateGifTaskRequest::getVideoName()const
{
	return videoName_;
}

void CreateGifTaskRequest::setVideoName(const std::string& videoName)
{
	videoName_ = videoName;
	setParameter("VideoName", videoName);
}

std::string CreateGifTaskRequest::getCallbackUrl()const
{
	return callbackUrl_;
}

void CreateGifTaskRequest::setCallbackUrl(const std::string& callbackUrl)
{
	callbackUrl_ = callbackUrl;
	setParameter("CallbackUrl", callbackUrl);
}

std::string CreateGifTaskRequest::getApplicationId()const
{
	return applicationId_;
}

void CreateGifTaskRequest::setApplicationId(const std::string& applicationId)
{
	applicationId_ = applicationId;
	setParameter("ApplicationId", applicationId);
}


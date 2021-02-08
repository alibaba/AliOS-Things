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

#include <alibabacloud/imm/model/GetVideoRequest.h>

using AlibabaCloud::Imm::Model::GetVideoRequest;

GetVideoRequest::GetVideoRequest() :
	RpcServiceRequest("imm", "2017-09-06", "GetVideo")
{
	setMethod(HttpRequest::Method::Post);
}

GetVideoRequest::~GetVideoRequest()
{}

std::string GetVideoRequest::getProject()const
{
	return project_;
}

void GetVideoRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string GetVideoRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetVideoRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetVideoRequest::getVideoUri()const
{
	return videoUri_;
}

void GetVideoRequest::setVideoUri(const std::string& videoUri)
{
	videoUri_ = videoUri;
	setParameter("VideoUri", videoUri);
}

std::string GetVideoRequest::getSetId()const
{
	return setId_;
}

void GetVideoRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}


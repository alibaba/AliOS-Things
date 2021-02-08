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

#include <alibabacloud/imm/model/DetectImageLogosRequest.h>

using AlibabaCloud::Imm::Model::DetectImageLogosRequest;

DetectImageLogosRequest::DetectImageLogosRequest() :
	RpcServiceRequest("imm", "2017-09-06", "DetectImageLogos")
{
	setMethod(HttpRequest::Method::Post);
}

DetectImageLogosRequest::~DetectImageLogosRequest()
{}

std::string DetectImageLogosRequest::getProject()const
{
	return project_;
}

void DetectImageLogosRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string DetectImageLogosRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DetectImageLogosRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DetectImageLogosRequest::getImageUri()const
{
	return imageUri_;
}

void DetectImageLogosRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}


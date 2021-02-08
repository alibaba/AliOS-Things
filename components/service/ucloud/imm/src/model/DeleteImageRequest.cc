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

#include <alibabacloud/imm/model/DeleteImageRequest.h>

using AlibabaCloud::Imm::Model::DeleteImageRequest;

DeleteImageRequest::DeleteImageRequest() :
	RpcServiceRequest("imm", "2017-09-06", "DeleteImage")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteImageRequest::~DeleteImageRequest()
{}

std::string DeleteImageRequest::getProject()const
{
	return project_;
}

void DeleteImageRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string DeleteImageRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteImageRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteImageRequest::getImageUri()const
{
	return imageUri_;
}

void DeleteImageRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}

std::string DeleteImageRequest::getSetId()const
{
	return setId_;
}

void DeleteImageRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}


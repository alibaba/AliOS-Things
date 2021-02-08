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

#include <alibabacloud/imm/model/DetectImageFacesRequest.h>

using AlibabaCloud::Imm::Model::DetectImageFacesRequest;

DetectImageFacesRequest::DetectImageFacesRequest() :
	RpcServiceRequest("imm", "2017-09-06", "DetectImageFaces")
{
	setMethod(HttpRequest::Method::Post);
}

DetectImageFacesRequest::~DetectImageFacesRequest()
{}

std::string DetectImageFacesRequest::getProject()const
{
	return project_;
}

void DetectImageFacesRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string DetectImageFacesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DetectImageFacesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DetectImageFacesRequest::getRealUid()const
{
	return realUid_;
}

void DetectImageFacesRequest::setRealUid(const std::string& realUid)
{
	realUid_ = realUid;
	setParameter("RealUid", realUid);
}

std::string DetectImageFacesRequest::getImageUri()const
{
	return imageUri_;
}

void DetectImageFacesRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}


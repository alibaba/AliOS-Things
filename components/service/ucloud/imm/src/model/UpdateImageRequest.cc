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

#include <alibabacloud/imm/model/UpdateImageRequest.h>

using AlibabaCloud::Imm::Model::UpdateImageRequest;

UpdateImageRequest::UpdateImageRequest() :
	RpcServiceRequest("imm", "2017-09-06", "UpdateImage")
{
	setMethod(HttpRequest::Method::Post);
}

UpdateImageRequest::~UpdateImageRequest()
{}

std::string UpdateImageRequest::getProject()const
{
	return project_;
}

void UpdateImageRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string UpdateImageRequest::getExternalId()const
{
	return externalId_;
}

void UpdateImageRequest::setExternalId(const std::string& externalId)
{
	externalId_ = externalId;
	setParameter("ExternalId", externalId);
}

std::string UpdateImageRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void UpdateImageRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string UpdateImageRequest::getSourceType()const
{
	return sourceType_;
}

void UpdateImageRequest::setSourceType(const std::string& sourceType)
{
	sourceType_ = sourceType;
	setParameter("SourceType", sourceType);
}

std::string UpdateImageRequest::getRemarksB()const
{
	return remarksB_;
}

void UpdateImageRequest::setRemarksB(const std::string& remarksB)
{
	remarksB_ = remarksB;
	setParameter("RemarksB", remarksB);
}

std::string UpdateImageRequest::getRemarksA()const
{
	return remarksA_;
}

void UpdateImageRequest::setRemarksA(const std::string& remarksA)
{
	remarksA_ = remarksA;
	setParameter("RemarksA", remarksA);
}

std::string UpdateImageRequest::getImageUri()const
{
	return imageUri_;
}

void UpdateImageRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}

std::string UpdateImageRequest::getRemarksArrayA()const
{
	return remarksArrayA_;
}

void UpdateImageRequest::setRemarksArrayA(const std::string& remarksArrayA)
{
	remarksArrayA_ = remarksArrayA;
	setParameter("RemarksArrayA", remarksArrayA);
}

std::string UpdateImageRequest::getRemarksArrayB()const
{
	return remarksArrayB_;
}

void UpdateImageRequest::setRemarksArrayB(const std::string& remarksArrayB)
{
	remarksArrayB_ = remarksArrayB;
	setParameter("RemarksArrayB", remarksArrayB);
}

std::string UpdateImageRequest::getSourceUri()const
{
	return sourceUri_;
}

void UpdateImageRequest::setSourceUri(const std::string& sourceUri)
{
	sourceUri_ = sourceUri;
	setParameter("SourceUri", sourceUri);
}

std::string UpdateImageRequest::getSourcePosition()const
{
	return sourcePosition_;
}

void UpdateImageRequest::setSourcePosition(const std::string& sourcePosition)
{
	sourcePosition_ = sourcePosition;
	setParameter("SourcePosition", sourcePosition);
}

std::string UpdateImageRequest::getRemarksD()const
{
	return remarksD_;
}

void UpdateImageRequest::setRemarksD(const std::string& remarksD)
{
	remarksD_ = remarksD;
	setParameter("RemarksD", remarksD);
}

std::string UpdateImageRequest::getRemarksC()const
{
	return remarksC_;
}

void UpdateImageRequest::setRemarksC(const std::string& remarksC)
{
	remarksC_ = remarksC;
	setParameter("RemarksC", remarksC);
}

std::string UpdateImageRequest::getSetId()const
{
	return setId_;
}

void UpdateImageRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}


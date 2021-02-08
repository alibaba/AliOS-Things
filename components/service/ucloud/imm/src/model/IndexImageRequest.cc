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

#include <alibabacloud/imm/model/IndexImageRequest.h>

using AlibabaCloud::Imm::Model::IndexImageRequest;

IndexImageRequest::IndexImageRequest() :
	RpcServiceRequest("imm", "2017-09-06", "IndexImage")
{
	setMethod(HttpRequest::Method::Post);
}

IndexImageRequest::~IndexImageRequest()
{}

std::string IndexImageRequest::getProject()const
{
	return project_;
}

void IndexImageRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string IndexImageRequest::getExternalId()const
{
	return externalId_;
}

void IndexImageRequest::setExternalId(const std::string& externalId)
{
	externalId_ = externalId;
	setParameter("ExternalId", externalId);
}

std::string IndexImageRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void IndexImageRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string IndexImageRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void IndexImageRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string IndexImageRequest::getSourceType()const
{
	return sourceType_;
}

void IndexImageRequest::setSourceType(const std::string& sourceType)
{
	sourceType_ = sourceType;
	setParameter("SourceType", sourceType);
}

std::string IndexImageRequest::getRealUid()const
{
	return realUid_;
}

void IndexImageRequest::setRealUid(const std::string& realUid)
{
	realUid_ = realUid;
	setParameter("RealUid", realUid);
}

std::string IndexImageRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void IndexImageRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string IndexImageRequest::getRemarksB()const
{
	return remarksB_;
}

void IndexImageRequest::setRemarksB(const std::string& remarksB)
{
	remarksB_ = remarksB;
	setParameter("RemarksB", remarksB);
}

std::string IndexImageRequest::getRemarksA()const
{
	return remarksA_;
}

void IndexImageRequest::setRemarksA(const std::string& remarksA)
{
	remarksA_ = remarksA;
	setParameter("RemarksA", remarksA);
}

std::string IndexImageRequest::getImageUri()const
{
	return imageUri_;
}

void IndexImageRequest::setImageUri(const std::string& imageUri)
{
	imageUri_ = imageUri;
	setParameter("ImageUri", imageUri);
}

std::string IndexImageRequest::getRemarksArrayA()const
{
	return remarksArrayA_;
}

void IndexImageRequest::setRemarksArrayA(const std::string& remarksArrayA)
{
	remarksArrayA_ = remarksArrayA;
	setParameter("RemarksArrayA", remarksArrayA);
}

std::string IndexImageRequest::getRemarksArrayB()const
{
	return remarksArrayB_;
}

void IndexImageRequest::setRemarksArrayB(const std::string& remarksArrayB)
{
	remarksArrayB_ = remarksArrayB;
	setParameter("RemarksArrayB", remarksArrayB);
}

std::string IndexImageRequest::getSourceUri()const
{
	return sourceUri_;
}

void IndexImageRequest::setSourceUri(const std::string& sourceUri)
{
	sourceUri_ = sourceUri;
	setParameter("SourceUri", sourceUri);
}

std::string IndexImageRequest::getSourcePosition()const
{
	return sourcePosition_;
}

void IndexImageRequest::setSourcePosition(const std::string& sourcePosition)
{
	sourcePosition_ = sourcePosition;
	setParameter("SourcePosition", sourcePosition);
}

std::string IndexImageRequest::getRemarksD()const
{
	return remarksD_;
}

void IndexImageRequest::setRemarksD(const std::string& remarksD)
{
	remarksD_ = remarksD;
	setParameter("RemarksD", remarksD);
}

std::string IndexImageRequest::getRemarksC()const
{
	return remarksC_;
}

void IndexImageRequest::setRemarksC(const std::string& remarksC)
{
	remarksC_ = remarksC;
	setParameter("RemarksC", remarksC);
}

std::string IndexImageRequest::getSetId()const
{
	return setId_;
}

void IndexImageRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}


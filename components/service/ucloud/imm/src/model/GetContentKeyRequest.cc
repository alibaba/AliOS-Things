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

#include <alibabacloud/imm/model/GetContentKeyRequest.h>

using AlibabaCloud::Imm::Model::GetContentKeyRequest;

GetContentKeyRequest::GetContentKeyRequest() :
	RpcServiceRequest("imm", "2017-09-06", "GetContentKey")
{
	setMethod(HttpRequest::Method::Post);
}

GetContentKeyRequest::~GetContentKeyRequest()
{}

std::string GetContentKeyRequest::getProject()const
{
	return project_;
}

void GetContentKeyRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string GetContentKeyRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void GetContentKeyRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string GetContentKeyRequest::getVersionId()const
{
	return versionId_;
}

void GetContentKeyRequest::setVersionId(const std::string& versionId)
{
	versionId_ = versionId;
	setParameter("VersionId", versionId);
}

std::string GetContentKeyRequest::getDRMServerId()const
{
	return dRMServerId_;
}

void GetContentKeyRequest::setDRMServerId(const std::string& dRMServerId)
{
	dRMServerId_ = dRMServerId;
	setParameter("DRMServerId", dRMServerId);
}

std::string GetContentKeyRequest::getKeyIds()const
{
	return keyIds_;
}

void GetContentKeyRequest::setKeyIds(const std::string& keyIds)
{
	keyIds_ = keyIds;
	setParameter("KeyIds", keyIds);
}


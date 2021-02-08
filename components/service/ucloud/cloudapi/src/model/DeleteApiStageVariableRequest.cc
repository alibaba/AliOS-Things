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

#include <alibabacloud/cloudapi/model/DeleteApiStageVariableRequest.h>

using AlibabaCloud::CloudAPI::Model::DeleteApiStageVariableRequest;

DeleteApiStageVariableRequest::DeleteApiStageVariableRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DeleteApiStageVariable")
{
	setMethod(HttpRequest::Method::Post);
}

DeleteApiStageVariableRequest::~DeleteApiStageVariableRequest()
{}

std::string DeleteApiStageVariableRequest::getGroupId()const
{
	return groupId_;
}

void DeleteApiStageVariableRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DeleteApiStageVariableRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DeleteApiStageVariableRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DeleteApiStageVariableRequest::getSecurityToken()const
{
	return securityToken_;
}

void DeleteApiStageVariableRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DeleteApiStageVariableRequest::getVariableName()const
{
	return variableName_;
}

void DeleteApiStageVariableRequest::setVariableName(const std::string& variableName)
{
	variableName_ = variableName;
	setParameter("VariableName", variableName);
}

std::string DeleteApiStageVariableRequest::getStageId()const
{
	return stageId_;
}

void DeleteApiStageVariableRequest::setStageId(const std::string& stageId)
{
	stageId_ = stageId;
	setParameter("StageId", stageId);
}


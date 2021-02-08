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

#include <alibabacloud/cloudapi/model/CreateApiStageVariableRequest.h>

using AlibabaCloud::CloudAPI::Model::CreateApiStageVariableRequest;

CreateApiStageVariableRequest::CreateApiStageVariableRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "CreateApiStageVariable")
{
	setMethod(HttpRequest::Method::Post);
}

CreateApiStageVariableRequest::~CreateApiStageVariableRequest()
{}

bool CreateApiStageVariableRequest::getSupportRoute()const
{
	return supportRoute_;
}

void CreateApiStageVariableRequest::setSupportRoute(bool supportRoute)
{
	supportRoute_ = supportRoute;
	setParameter("SupportRoute", supportRoute ? "true" : "false");
}

std::string CreateApiStageVariableRequest::getVariableValue()const
{
	return variableValue_;
}

void CreateApiStageVariableRequest::setVariableValue(const std::string& variableValue)
{
	variableValue_ = variableValue;
	setParameter("VariableValue", variableValue);
}

std::string CreateApiStageVariableRequest::getGroupId()const
{
	return groupId_;
}

void CreateApiStageVariableRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string CreateApiStageVariableRequest::getStageRouteModel()const
{
	return stageRouteModel_;
}

void CreateApiStageVariableRequest::setStageRouteModel(const std::string& stageRouteModel)
{
	stageRouteModel_ = stageRouteModel;
	setParameter("StageRouteModel", stageRouteModel);
}

std::string CreateApiStageVariableRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateApiStageVariableRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateApiStageVariableRequest::getSecurityToken()const
{
	return securityToken_;
}

void CreateApiStageVariableRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string CreateApiStageVariableRequest::getVariableName()const
{
	return variableName_;
}

void CreateApiStageVariableRequest::setVariableName(const std::string& variableName)
{
	variableName_ = variableName;
	setParameter("VariableName", variableName);
}

std::string CreateApiStageVariableRequest::getStageId()const
{
	return stageId_;
}

void CreateApiStageVariableRequest::setStageId(const std::string& stageId)
{
	stageId_ = stageId;
	setParameter("StageId", stageId);
}


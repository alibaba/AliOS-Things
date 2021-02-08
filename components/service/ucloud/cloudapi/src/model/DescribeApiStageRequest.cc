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

#include <alibabacloud/cloudapi/model/DescribeApiStageRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiStageRequest;

DescribeApiStageRequest::DescribeApiStageRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiStage")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiStageRequest::~DescribeApiStageRequest()
{}

std::string DescribeApiStageRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiStageRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

std::string DescribeApiStageRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiStageRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiStageRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiStageRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

std::string DescribeApiStageRequest::getStageId()const
{
	return stageId_;
}

void DescribeApiStageRequest::setStageId(const std::string& stageId)
{
	stageId_ = stageId;
	setParameter("StageId", stageId);
}


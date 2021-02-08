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

#include <alibabacloud/cloudapi/model/DescribeDeployedApisRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeDeployedApisRequest;

DescribeDeployedApisRequest::DescribeDeployedApisRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeDeployedApis")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeDeployedApisRequest::~DescribeDeployedApisRequest()
{}

std::string DescribeDeployedApisRequest::getStageName()const
{
	return stageName_;
}

void DescribeDeployedApisRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DescribeDeployedApisRequest::getGroupId()const
{
	return groupId_;
}

void DescribeDeployedApisRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

bool DescribeDeployedApisRequest::getEnableTagAuth()const
{
	return enableTagAuth_;
}

void DescribeDeployedApisRequest::setEnableTagAuth(bool enableTagAuth)
{
	enableTagAuth_ = enableTagAuth;
	setParameter("EnableTagAuth", enableTagAuth ? "true" : "false");
}

int DescribeDeployedApisRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeDeployedApisRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeDeployedApisRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeDeployedApisRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeDeployedApisRequest::getApiName()const
{
	return apiName_;
}

void DescribeDeployedApisRequest::setApiName(const std::string& apiName)
{
	apiName_ = apiName;
	setParameter("ApiName", apiName);
}

std::string DescribeDeployedApisRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeDeployedApisRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeDeployedApisRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeDeployedApisRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::vector<DescribeDeployedApisRequest::Tag> DescribeDeployedApisRequest::getTag()const
{
	return tag_;
}

void DescribeDeployedApisRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}

std::string DescribeDeployedApisRequest::getApiId()const
{
	return apiId_;
}

void DescribeDeployedApisRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}


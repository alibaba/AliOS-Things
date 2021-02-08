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

#include <alibabacloud/cloudapi/model/DescribeApisRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApisRequest;

DescribeApisRequest::DescribeApisRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApis")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApisRequest::~DescribeApisRequest()
{}

std::string DescribeApisRequest::getVisibility()const
{
	return visibility_;
}

void DescribeApisRequest::setVisibility(const std::string& visibility)
{
	visibility_ = visibility;
	setParameter("Visibility", visibility);
}

std::string DescribeApisRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApisRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

bool DescribeApisRequest::getEnableTagAuth()const
{
	return enableTagAuth_;
}

void DescribeApisRequest::setEnableTagAuth(bool enableTagAuth)
{
	enableTagAuth_ = enableTagAuth;
	setParameter("EnableTagAuth", enableTagAuth ? "true" : "false");
}

int DescribeApisRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApisRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApisRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApisRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApisRequest::getApiName()const
{
	return apiName_;
}

void DescribeApisRequest::setApiName(const std::string& apiName)
{
	apiName_ = apiName;
	setParameter("ApiName", apiName);
}

std::string DescribeApisRequest::getCatalogId()const
{
	return catalogId_;
}

void DescribeApisRequest::setCatalogId(const std::string& catalogId)
{
	catalogId_ = catalogId;
	setParameter("CatalogId", catalogId);
}

std::string DescribeApisRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApisRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeApisRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApisRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::vector<DescribeApisRequest::Tag> DescribeApisRequest::getTag()const
{
	return tag_;
}

void DescribeApisRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}

std::string DescribeApisRequest::getApiId()const
{
	return apiId_;
}

void DescribeApisRequest::setApiId(const std::string& apiId)
{
	apiId_ = apiId;
	setParameter("ApiId", apiId);
}


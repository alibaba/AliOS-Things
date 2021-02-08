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

#include <alibabacloud/cloudapi/model/DescribeAppAttributesRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeAppAttributesRequest;

DescribeAppAttributesRequest::DescribeAppAttributesRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeAppAttributes")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeAppAttributesRequest::~DescribeAppAttributesRequest()
{}

bool DescribeAppAttributesRequest::getEnableTagAuth()const
{
	return enableTagAuth_;
}

void DescribeAppAttributesRequest::setEnableTagAuth(bool enableTagAuth)
{
	enableTagAuth_ = enableTagAuth;
	setParameter("EnableTagAuth", enableTagAuth ? "true" : "false");
}

int DescribeAppAttributesRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeAppAttributesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeAppAttributesRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeAppAttributesRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeAppAttributesRequest::getAppName()const
{
	return appName_;
}

void DescribeAppAttributesRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string DescribeAppAttributesRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeAppAttributesRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

long DescribeAppAttributesRequest::getAppId()const
{
	return appId_;
}

void DescribeAppAttributesRequest::setAppId(long appId)
{
	appId_ = appId;
	setParameter("AppId", std::to_string(appId));
}

int DescribeAppAttributesRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeAppAttributesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::vector<DescribeAppAttributesRequest::Tag> DescribeAppAttributesRequest::getTag()const
{
	return tag_;
}

void DescribeAppAttributesRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}


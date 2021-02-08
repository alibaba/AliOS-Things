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

#include <alibabacloud/cloudapi/model/DescribeApiGroupsRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiGroupsRequest;

DescribeApiGroupsRequest::DescribeApiGroupsRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiGroups")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiGroupsRequest::~DescribeApiGroupsRequest()
{}

std::string DescribeApiGroupsRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiGroupsRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

bool DescribeApiGroupsRequest::getEnableTagAuth()const
{
	return enableTagAuth_;
}

void DescribeApiGroupsRequest::setEnableTagAuth(bool enableTagAuth)
{
	enableTagAuth_ = enableTagAuth;
	setParameter("EnableTagAuth", enableTagAuth ? "true" : "false");
}

std::string DescribeApiGroupsRequest::getGroupName()const
{
	return groupName_;
}

void DescribeApiGroupsRequest::setGroupName(const std::string& groupName)
{
	groupName_ = groupName;
	setParameter("GroupName", groupName);
}

int DescribeApiGroupsRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApiGroupsRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApiGroupsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiGroupsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiGroupsRequest::getInstanceId()const
{
	return instanceId_;
}

void DescribeApiGroupsRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string DescribeApiGroupsRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiGroupsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeApiGroupsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApiGroupsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::vector<DescribeApiGroupsRequest::Tag> DescribeApiGroupsRequest::getTag()const
{
	return tag_;
}

void DescribeApiGroupsRequest::setTag(const std::vector<Tag>& tag)
{
	tag_ = tag;
	for(int dep1 = 0; dep1!= tag.size(); dep1++) {
		auto tagObj = tag.at(dep1);
		std::string tagObjStr = "Tag." + std::to_string(dep1 + 1);
		setParameter(tagObjStr + ".Value", tagObj.value);
		setParameter(tagObjStr + ".Key", tagObj.key);
	}
}


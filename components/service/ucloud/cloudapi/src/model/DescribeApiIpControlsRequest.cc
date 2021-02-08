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

#include <alibabacloud/cloudapi/model/DescribeApiIpControlsRequest.h>

using AlibabaCloud::CloudAPI::Model::DescribeApiIpControlsRequest;

DescribeApiIpControlsRequest::DescribeApiIpControlsRequest() :
	RpcServiceRequest("cloudapi", "2016-07-14", "DescribeApiIpControls")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeApiIpControlsRequest::~DescribeApiIpControlsRequest()
{}

std::string DescribeApiIpControlsRequest::getStageName()const
{
	return stageName_;
}

void DescribeApiIpControlsRequest::setStageName(const std::string& stageName)
{
	stageName_ = stageName;
	setParameter("StageName", stageName);
}

std::string DescribeApiIpControlsRequest::getGroupId()const
{
	return groupId_;
}

void DescribeApiIpControlsRequest::setGroupId(const std::string& groupId)
{
	groupId_ = groupId;
	setParameter("GroupId", groupId);
}

int DescribeApiIpControlsRequest::getPageNumber()const
{
	return pageNumber_;
}

void DescribeApiIpControlsRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

std::string DescribeApiIpControlsRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void DescribeApiIpControlsRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string DescribeApiIpControlsRequest::getSecurityToken()const
{
	return securityToken_;
}

void DescribeApiIpControlsRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int DescribeApiIpControlsRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeApiIpControlsRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeApiIpControlsRequest::getApiIds()const
{
	return apiIds_;
}

void DescribeApiIpControlsRequest::setApiIds(const std::string& apiIds)
{
	apiIds_ = apiIds;
	setParameter("ApiIds", apiIds);
}


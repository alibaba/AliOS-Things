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

#include <alibabacloud/rtc/model/DescribeRecordTemplatesRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRecordTemplatesRequest;

DescribeRecordTemplatesRequest::DescribeRecordTemplatesRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRecordTemplates")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRecordTemplatesRequest::~DescribeRecordTemplatesRequest()
{}

std::vector<std::string> DescribeRecordTemplatesRequest::getTemplateIds()const
{
	return templateIds_;
}

void DescribeRecordTemplatesRequest::setTemplateIds(const std::vector<std::string>& templateIds)
{
	templateIds_ = templateIds;
	for(int dep1 = 0; dep1!= templateIds.size(); dep1++) {
		setParameter("TemplateIds."+ std::to_string(dep1), templateIds.at(dep1));
	}
}

int DescribeRecordTemplatesRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeRecordTemplatesRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeRecordTemplatesRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRecordTemplatesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeRecordTemplatesRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRecordTemplatesRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long DescribeRecordTemplatesRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRecordTemplatesRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRecordTemplatesRequest::getAppId()const
{
	return appId_;
}

void DescribeRecordTemplatesRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}


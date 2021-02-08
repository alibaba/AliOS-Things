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

#include <alibabacloud/rtc/model/DescribeRecordTasksRequest.h>

using AlibabaCloud::Rtc::Model::DescribeRecordTasksRequest;

DescribeRecordTasksRequest::DescribeRecordTasksRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "DescribeRecordTasks")
{
	setMethod(HttpRequest::Method::Post);
}

DescribeRecordTasksRequest::~DescribeRecordTasksRequest()
{}

std::string DescribeRecordTasksRequest::getStartTime()const
{
	return startTime_;
}

void DescribeRecordTasksRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::vector<std::string> DescribeRecordTasksRequest::getTaskIds()const
{
	return taskIds_;
}

void DescribeRecordTasksRequest::setTaskIds(const std::vector<std::string>& taskIds)
{
	taskIds_ = taskIds;
	for(int dep1 = 0; dep1!= taskIds.size(); dep1++) {
		setParameter("TaskIds."+ std::to_string(dep1), taskIds.at(dep1));
	}
}

int DescribeRecordTasksRequest::getPageNum()const
{
	return pageNum_;
}

void DescribeRecordTasksRequest::setPageNum(int pageNum)
{
	pageNum_ = pageNum;
	setParameter("PageNum", std::to_string(pageNum));
}

int DescribeRecordTasksRequest::getPageSize()const
{
	return pageSize_;
}

void DescribeRecordTasksRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string DescribeRecordTasksRequest::getShowLog()const
{
	return showLog_;
}

void DescribeRecordTasksRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

std::string DescribeRecordTasksRequest::getEndTime()const
{
	return endTime_;
}

void DescribeRecordTasksRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long DescribeRecordTasksRequest::getOwnerId()const
{
	return ownerId_;
}

void DescribeRecordTasksRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string DescribeRecordTasksRequest::getAppId()const
{
	return appId_;
}

void DescribeRecordTasksRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}

std::string DescribeRecordTasksRequest::getChannelId()const
{
	return channelId_;
}

void DescribeRecordTasksRequest::setChannelId(const std::string& channelId)
{
	channelId_ = channelId;
	setParameter("ChannelId", channelId);
}

std::string DescribeRecordTasksRequest::getStatus()const
{
	return status_;
}

void DescribeRecordTasksRequest::setStatus(const std::string& status)
{
	status_ = status;
	setParameter("Status", status);
}


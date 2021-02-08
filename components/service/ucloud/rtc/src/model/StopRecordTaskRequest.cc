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

#include <alibabacloud/rtc/model/StopRecordTaskRequest.h>

using AlibabaCloud::Rtc::Model::StopRecordTaskRequest;

StopRecordTaskRequest::StopRecordTaskRequest() :
	RpcServiceRequest("rtc", "2018-01-11", "StopRecordTask")
{
	setMethod(HttpRequest::Method::Post);
}

StopRecordTaskRequest::~StopRecordTaskRequest()
{}

std::string StopRecordTaskRequest::getTaskId()const
{
	return taskId_;
}

void StopRecordTaskRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setParameter("TaskId", taskId);
}

std::string StopRecordTaskRequest::getShowLog()const
{
	return showLog_;
}

void StopRecordTaskRequest::setShowLog(const std::string& showLog)
{
	showLog_ = showLog;
	setParameter("ShowLog", showLog);
}

long StopRecordTaskRequest::getOwnerId()const
{
	return ownerId_;
}

void StopRecordTaskRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::string StopRecordTaskRequest::getAppId()const
{
	return appId_;
}

void StopRecordTaskRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}


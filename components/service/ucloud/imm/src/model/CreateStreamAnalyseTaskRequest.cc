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

#include <alibabacloud/imm/model/CreateStreamAnalyseTaskRequest.h>

using AlibabaCloud::Imm::Model::CreateStreamAnalyseTaskRequest;

CreateStreamAnalyseTaskRequest::CreateStreamAnalyseTaskRequest() :
	RpcServiceRequest("imm", "2017-09-06", "CreateStreamAnalyseTask")
{
	setMethod(HttpRequest::Method::Post);
}

CreateStreamAnalyseTaskRequest::~CreateStreamAnalyseTaskRequest()
{}

std::string CreateStreamAnalyseTaskRequest::getGrabType()const
{
	return grabType_;
}

void CreateStreamAnalyseTaskRequest::setGrabType(const std::string& grabType)
{
	grabType_ = grabType;
	setParameter("GrabType", grabType);
}

std::string CreateStreamAnalyseTaskRequest::getProject()const
{
	return project_;
}

void CreateStreamAnalyseTaskRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string CreateStreamAnalyseTaskRequest::getStartTime()const
{
	return startTime_;
}

void CreateStreamAnalyseTaskRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string CreateStreamAnalyseTaskRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void CreateStreamAnalyseTaskRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string CreateStreamAnalyseTaskRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void CreateStreamAnalyseTaskRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string CreateStreamAnalyseTaskRequest::getStreamUri()const
{
	return streamUri_;
}

void CreateStreamAnalyseTaskRequest::setStreamUri(const std::string& streamUri)
{
	streamUri_ = streamUri;
	setParameter("StreamUri", streamUri);
}

std::string CreateStreamAnalyseTaskRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void CreateStreamAnalyseTaskRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string CreateStreamAnalyseTaskRequest::getEndTime()const
{
	return endTime_;
}

void CreateStreamAnalyseTaskRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

bool CreateStreamAnalyseTaskRequest::getSaveType()const
{
	return saveType_;
}

void CreateStreamAnalyseTaskRequest::setSaveType(bool saveType)
{
	saveType_ = saveType;
	setParameter("SaveType", saveType ? "true" : "false");
}

std::string CreateStreamAnalyseTaskRequest::getInterval()const
{
	return interval_;
}

void CreateStreamAnalyseTaskRequest::setInterval(const std::string& interval)
{
	interval_ = interval;
	setParameter("Interval", interval);
}

std::string CreateStreamAnalyseTaskRequest::getTgtUri()const
{
	return tgtUri_;
}

void CreateStreamAnalyseTaskRequest::setTgtUri(const std::string& tgtUri)
{
	tgtUri_ = tgtUri;
	setParameter("TgtUri", tgtUri);
}


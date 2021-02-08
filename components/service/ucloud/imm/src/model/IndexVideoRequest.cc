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

#include <alibabacloud/imm/model/IndexVideoRequest.h>

using AlibabaCloud::Imm::Model::IndexVideoRequest;

IndexVideoRequest::IndexVideoRequest() :
	RpcServiceRequest("imm", "2017-09-06", "IndexVideo")
{
	setMethod(HttpRequest::Method::Post);
}

IndexVideoRequest::~IndexVideoRequest()
{}

std::string IndexVideoRequest::getGrabType()const
{
	return grabType_;
}

void IndexVideoRequest::setGrabType(const std::string& grabType)
{
	grabType_ = grabType;
	setParameter("GrabType", grabType);
}

std::string IndexVideoRequest::getProject()const
{
	return project_;
}

void IndexVideoRequest::setProject(const std::string& project)
{
	project_ = project;
	setParameter("Project", project);
}

std::string IndexVideoRequest::getExternalId()const
{
	return externalId_;
}

void IndexVideoRequest::setExternalId(const std::string& externalId)
{
	externalId_ = externalId;
	setParameter("ExternalId", externalId);
}

std::string IndexVideoRequest::getStartTime()const
{
	return startTime_;
}

void IndexVideoRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string IndexVideoRequest::getAccessKeyId()const
{
	return accessKeyId_;
}

void IndexVideoRequest::setAccessKeyId(const std::string& accessKeyId)
{
	accessKeyId_ = accessKeyId;
	setParameter("AccessKeyId", accessKeyId);
}

std::string IndexVideoRequest::getNotifyEndpoint()const
{
	return notifyEndpoint_;
}

void IndexVideoRequest::setNotifyEndpoint(const std::string& notifyEndpoint)
{
	notifyEndpoint_ = notifyEndpoint;
	setParameter("NotifyEndpoint", notifyEndpoint);
}

std::string IndexVideoRequest::getNotifyTopicName()const
{
	return notifyTopicName_;
}

void IndexVideoRequest::setNotifyTopicName(const std::string& notifyTopicName)
{
	notifyTopicName_ = notifyTopicName;
	setParameter("NotifyTopicName", notifyTopicName);
}

std::string IndexVideoRequest::getRemarksB()const
{
	return remarksB_;
}

void IndexVideoRequest::setRemarksB(const std::string& remarksB)
{
	remarksB_ = remarksB;
	setParameter("RemarksB", remarksB);
}

std::string IndexVideoRequest::getRemarksA()const
{
	return remarksA_;
}

void IndexVideoRequest::setRemarksA(const std::string& remarksA)
{
	remarksA_ = remarksA;
	setParameter("RemarksA", remarksA);
}

std::string IndexVideoRequest::getEndTime()const
{
	return endTime_;
}

void IndexVideoRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

std::string IndexVideoRequest::getVideoUri()const
{
	return videoUri_;
}

void IndexVideoRequest::setVideoUri(const std::string& videoUri)
{
	videoUri_ = videoUri;
	setParameter("VideoUri", videoUri);
}

bool IndexVideoRequest::getSaveType()const
{
	return saveType_;
}

void IndexVideoRequest::setSaveType(bool saveType)
{
	saveType_ = saveType;
	setParameter("SaveType", saveType ? "true" : "false");
}

std::string IndexVideoRequest::getRemarksD()const
{
	return remarksD_;
}

void IndexVideoRequest::setRemarksD(const std::string& remarksD)
{
	remarksD_ = remarksD;
	setParameter("RemarksD", remarksD);
}

std::string IndexVideoRequest::getRemarksC()const
{
	return remarksC_;
}

void IndexVideoRequest::setRemarksC(const std::string& remarksC)
{
	remarksC_ = remarksC;
	setParameter("RemarksC", remarksC);
}

std::string IndexVideoRequest::getSetId()const
{
	return setId_;
}

void IndexVideoRequest::setSetId(const std::string& setId)
{
	setId_ = setId;
	setParameter("SetId", setId);
}

std::string IndexVideoRequest::getInterval()const
{
	return interval_;
}

void IndexVideoRequest::setInterval(const std::string& interval)
{
	interval_ = interval;
	setParameter("Interval", interval);
}

std::string IndexVideoRequest::getTgtUri()const
{
	return tgtUri_;
}

void IndexVideoRequest::setTgtUri(const std::string& tgtUri)
{
	tgtUri_ = tgtUri;
	setParameter("TgtUri", tgtUri);
}


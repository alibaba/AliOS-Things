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

#include <alibabacloud/live/model/AddLiveAppRecordConfigRequest.h>

using AlibabaCloud::Live::Model::AddLiveAppRecordConfigRequest;

AddLiveAppRecordConfigRequest::AddLiveAppRecordConfigRequest() :
	RpcServiceRequest("live", "2016-11-01", "AddLiveAppRecordConfig")
{
	setMethod(HttpRequest::Method::Post);
}

AddLiveAppRecordConfigRequest::~AddLiveAppRecordConfigRequest()
{}

std::string AddLiveAppRecordConfigRequest::getOssEndpoint()const
{
	return ossEndpoint_;
}

void AddLiveAppRecordConfigRequest::setOssEndpoint(const std::string& ossEndpoint)
{
	ossEndpoint_ = ossEndpoint;
	setParameter("OssEndpoint", ossEndpoint);
}

std::string AddLiveAppRecordConfigRequest::getStartTime()const
{
	return startTime_;
}

void AddLiveAppRecordConfigRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string AddLiveAppRecordConfigRequest::getAppName()const
{
	return appName_;
}

void AddLiveAppRecordConfigRequest::setAppName(const std::string& appName)
{
	appName_ = appName;
	setParameter("AppName", appName);
}

std::string AddLiveAppRecordConfigRequest::getSecurityToken()const
{
	return securityToken_;
}

void AddLiveAppRecordConfigRequest::setSecurityToken(const std::string& securityToken)
{
	securityToken_ = securityToken;
	setParameter("SecurityToken", securityToken);
}

int AddLiveAppRecordConfigRequest::getOnDemand()const
{
	return onDemand_;
}

void AddLiveAppRecordConfigRequest::setOnDemand(int onDemand)
{
	onDemand_ = onDemand;
	setParameter("OnDemand", std::to_string(onDemand));
}

std::string AddLiveAppRecordConfigRequest::getStreamName()const
{
	return streamName_;
}

void AddLiveAppRecordConfigRequest::setStreamName(const std::string& streamName)
{
	streamName_ = streamName;
	setParameter("StreamName", streamName);
}

std::string AddLiveAppRecordConfigRequest::getOssBucket()const
{
	return ossBucket_;
}

void AddLiveAppRecordConfigRequest::setOssBucket(const std::string& ossBucket)
{
	ossBucket_ = ossBucket;
	setParameter("OssBucket", ossBucket);
}

std::string AddLiveAppRecordConfigRequest::getDomainName()const
{
	return domainName_;
}

void AddLiveAppRecordConfigRequest::setDomainName(const std::string& domainName)
{
	domainName_ = domainName;
	setParameter("DomainName", domainName);
}

std::string AddLiveAppRecordConfigRequest::getEndTime()const
{
	return endTime_;
}

void AddLiveAppRecordConfigRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

long AddLiveAppRecordConfigRequest::getOwnerId()const
{
	return ownerId_;
}

void AddLiveAppRecordConfigRequest::setOwnerId(long ownerId)
{
	ownerId_ = ownerId;
	setParameter("OwnerId", std::to_string(ownerId));
}

std::vector<AddLiveAppRecordConfigRequest::RecordFormat> AddLiveAppRecordConfigRequest::getRecordFormat()const
{
	return recordFormat_;
}

void AddLiveAppRecordConfigRequest::setRecordFormat(const std::vector<RecordFormat>& recordFormat)
{
	recordFormat_ = recordFormat;
	for(int dep1 = 0; dep1!= recordFormat.size(); dep1++) {
		auto recordFormatObj = recordFormat.at(dep1);
		std::string recordFormatObjStr = "RecordFormat." + std::to_string(dep1 + 1);
		setParameter(recordFormatObjStr + ".SliceOssObjectPrefix", recordFormatObj.sliceOssObjectPrefix);
		setParameter(recordFormatObjStr + ".Format", recordFormatObj.format);
		setParameter(recordFormatObjStr + ".OssObjectPrefix", recordFormatObj.ossObjectPrefix);
		setParameter(recordFormatObjStr + ".CycleDuration", std::to_string(recordFormatObj.cycleDuration));
	}
}


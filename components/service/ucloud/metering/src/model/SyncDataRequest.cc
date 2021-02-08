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

#include <alibabacloud/metering/model/SyncDataRequest.h>

using AlibabaCloud::Metering::Model::SyncDataRequest;

SyncDataRequest::SyncDataRequest() :
	RoaServiceRequest("metering", "2020-09-21")
{
	setResourcePath("/api/dataSync");
	setMethod(HttpRequest::Method::Post);
}

SyncDataRequest::~SyncDataRequest()
{}

std::string SyncDataRequest::getInstanceId()const
{
	return instanceId_;
}

void SyncDataRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setParameter("InstanceId", instanceId);
}

std::string SyncDataRequest::getData()const
{
	return data_;
}

void SyncDataRequest::setData(const std::string& data)
{
	data_ = data;
	setParameter("Data", data);
}

std::string SyncDataRequest::getDataType()const
{
	return dataType_;
}

void SyncDataRequest::setDataType(const std::string& dataType)
{
	dataType_ = dataType;
	setParameter("DataType", dataType);
}

std::string SyncDataRequest::getEndTime()const
{
	return endTime_;
}

void SyncDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setParameter("EndTime", endTime);
}

std::string SyncDataRequest::getStartTime()const
{
	return startTime_;
}

void SyncDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setParameter("StartTime", startTime);
}

std::string SyncDataRequest::getCommodityCode()const
{
	return commodityCode_;
}

void SyncDataRequest::setCommodityCode(const std::string& commodityCode)
{
	commodityCode_ = commodityCode;
	setParameter("CommodityCode", commodityCode);
}

std::string SyncDataRequest::getRegion()const
{
	return region_;
}

void SyncDataRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}

std::string SyncDataRequest::getServiceId()const
{
	return serviceId_;
}

void SyncDataRequest::setServiceId(const std::string& serviceId)
{
	serviceId_ = serviceId;
	setParameter("ServiceId", serviceId);
}

std::string SyncDataRequest::getUserId()const
{
	return userId_;
}

void SyncDataRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setParameter("UserId", userId);
}


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

#include <alibabacloud/metering/model/PostDataRequest.h>

using AlibabaCloud::Metering::Model::PostDataRequest;

PostDataRequest::PostDataRequest() :
	RoaServiceRequest("metering", "2020-09-21")
{
	setResourcePath("/api/dataPost");
	setMethod(HttpRequest::Method::Post);
}

PostDataRequest::~PostDataRequest()
{}

std::string PostDataRequest::getInstanceId()const
{
	return instanceId_;
}

void PostDataRequest::setInstanceId(const std::string& instanceId)
{
	instanceId_ = instanceId;
	setBodyParameter("InstanceId", instanceId);
}

std::string PostDataRequest::getData()const
{
	return data_;
}

void PostDataRequest::setData(const std::string& data)
{
	data_ = data;
	setBodyParameter("Data", data);
}

std::string PostDataRequest::getDataType()const
{
	return dataType_;
}

void PostDataRequest::setDataType(const std::string& dataType)
{
	dataType_ = dataType;
	setBodyParameter("DataType", dataType);
}

std::string PostDataRequest::getEndTime()const
{
	return endTime_;
}

void PostDataRequest::setEndTime(const std::string& endTime)
{
	endTime_ = endTime;
	setBodyParameter("EndTime", endTime);
}

std::string PostDataRequest::getStartTime()const
{
	return startTime_;
}

void PostDataRequest::setStartTime(const std::string& startTime)
{
	startTime_ = startTime;
	setBodyParameter("StartTime", startTime);
}

std::string PostDataRequest::getCommodityCode()const
{
	return commodityCode_;
}

void PostDataRequest::setCommodityCode(const std::string& commodityCode)
{
	commodityCode_ = commodityCode;
	setBodyParameter("CommodityCode", commodityCode);
}

std::string PostDataRequest::getRegion()const
{
	return region_;
}

void PostDataRequest::setRegion(const std::string& region)
{
	region_ = region;
	setBodyParameter("Region", region);
}

std::string PostDataRequest::getServiceId()const
{
	return serviceId_;
}

void PostDataRequest::setServiceId(const std::string& serviceId)
{
	serviceId_ = serviceId;
	setBodyParameter("ServiceId", serviceId);
}

std::string PostDataRequest::getUserId()const
{
	return userId_;
}

void PostDataRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setBodyParameter("UserId", userId);
}


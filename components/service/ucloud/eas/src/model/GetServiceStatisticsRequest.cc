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

#include <alibabacloud/eas/model/GetServiceStatisticsRequest.h>

using AlibabaCloud::Eas::Model::GetServiceStatisticsRequest;

GetServiceStatisticsRequest::GetServiceStatisticsRequest() :
	RoaServiceRequest("eas", "2018-05-22")
{
	setResourcePath("/api/services/[region]/[service_name]/statistics");
	setMethod(HttpRequest::Method::Get);
}

GetServiceStatisticsRequest::~GetServiceStatisticsRequest()
{}

std::string GetServiceStatisticsRequest::getMetric()const
{
	return metric_;
}

void GetServiceStatisticsRequest::setMetric(const std::string& metric)
{
	metric_ = metric;
	setParameter("Metric", metric);
}

std::string GetServiceStatisticsRequest::getService_name()const
{
	return service_name_;
}

void GetServiceStatisticsRequest::setService_name(const std::string& service_name)
{
	service_name_ = service_name;
	setParameter("Service_name", service_name);
}

int GetServiceStatisticsRequest::getCount()const
{
	return count_;
}

void GetServiceStatisticsRequest::setCount(int count)
{
	count_ = count;
	setParameter("Count", std::to_string(count));
}

std::string GetServiceStatisticsRequest::getFrom()const
{
	return from_;
}

void GetServiceStatisticsRequest::setFrom(const std::string& from)
{
	from_ = from;
	setParameter("From", from);
}

std::string GetServiceStatisticsRequest::getTo()const
{
	return to_;
}

void GetServiceStatisticsRequest::setTo(const std::string& to)
{
	to_ = to;
	setParameter("To", to);
}

std::string GetServiceStatisticsRequest::getRegion()const
{
	return region_;
}

void GetServiceStatisticsRequest::setRegion(const std::string& region)
{
	region_ = region;
	setParameter("Region", region);
}


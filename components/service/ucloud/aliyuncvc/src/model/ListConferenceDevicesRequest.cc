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

#include <alibabacloud/aliyuncvc/model/ListConferenceDevicesRequest.h>

using AlibabaCloud::Aliyuncvc::Model::ListConferenceDevicesRequest;

ListConferenceDevicesRequest::ListConferenceDevicesRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "ListConferenceDevices")
{
	setMethod(HttpRequest::Method::Post);
}

ListConferenceDevicesRequest::~ListConferenceDevicesRequest()
{}

std::string ListConferenceDevicesRequest::getSerialNumber()const
{
	return serialNumber_;
}

void ListConferenceDevicesRequest::setSerialNumber(const std::string& serialNumber)
{
	serialNumber_ = serialNumber;
	setParameter("SerialNumber", serialNumber);
}

int ListConferenceDevicesRequest::getPageNumber()const
{
	return pageNumber_;
}

void ListConferenceDevicesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int ListConferenceDevicesRequest::getPageSize()const
{
	return pageSize_;
}

void ListConferenceDevicesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}


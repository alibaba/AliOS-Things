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

#include <alibabacloud/aliyuncvc/model/ListDevicesRequest.h>

using AlibabaCloud::Aliyuncvc::Model::ListDevicesRequest;

ListDevicesRequest::ListDevicesRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "ListDevices")
{
	setMethod(HttpRequest::Method::Post);
}

ListDevicesRequest::~ListDevicesRequest()
{}

std::string ListDevicesRequest::getCastScreenCode()const
{
	return castScreenCode_;
}

void ListDevicesRequest::setCastScreenCode(const std::string& castScreenCode)
{
	castScreenCode_ = castScreenCode;
	setParameter("CastScreenCode", castScreenCode);
}

int ListDevicesRequest::getPageNumber()const
{
	return pageNumber_;
}

void ListDevicesRequest::setPageNumber(int pageNumber)
{
	pageNumber_ = pageNumber;
	setParameter("PageNumber", std::to_string(pageNumber));
}

int ListDevicesRequest::getPageSize()const
{
	return pageSize_;
}

void ListDevicesRequest::setPageSize(int pageSize)
{
	pageSize_ = pageSize;
	setParameter("PageSize", std::to_string(pageSize));
}

std::string ListDevicesRequest::getSN()const
{
	return sN_;
}

void ListDevicesRequest::setSN(const std::string& sN)
{
	sN_ = sN;
	setParameter("SN", sN);
}


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

#include <alibabacloud/aliyuncvc/model/GetStatisticRequest.h>

using AlibabaCloud::Aliyuncvc::Model::GetStatisticRequest;

GetStatisticRequest::GetStatisticRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "GetStatistic")
{
	setMethod(HttpRequest::Method::Post);
}

GetStatisticRequest::~GetStatisticRequest()
{}

long GetStatisticRequest::getEndTime()const
{
	return endTime_;
}

void GetStatisticRequest::setEndTime(long endTime)
{
	endTime_ = endTime;
	setBodyParameter("EndTime", std::to_string(endTime));
}

long GetStatisticRequest::getStartTime()const
{
	return startTime_;
}

void GetStatisticRequest::setStartTime(long startTime)
{
	startTime_ = startTime;
	setBodyParameter("StartTime", std::to_string(startTime));
}


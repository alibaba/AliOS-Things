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

#include <alibabacloud/cloudapi/model/DescribeApiErrorDataResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeApiErrorDataResult::DescribeApiErrorDataResult() :
	ServiceResult()
{}

DescribeApiErrorDataResult::DescribeApiErrorDataResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeApiErrorDataResult::~DescribeApiErrorDataResult()
{}

void DescribeApiErrorDataResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allClientErrorsNode = value["ClientErrors"]["MonitorItem"];
	for (auto valueClientErrorsMonitorItem : allClientErrorsNode)
	{
		MonitorItem clientErrorsObject;
		if(!valueClientErrorsMonitorItem["ItemTime"].isNull())
			clientErrorsObject.itemTime = valueClientErrorsMonitorItem["ItemTime"].asString();
		if(!valueClientErrorsMonitorItem["ItemValue"].isNull())
			clientErrorsObject.itemValue = valueClientErrorsMonitorItem["ItemValue"].asString();
		clientErrors_.push_back(clientErrorsObject);
	}
	auto allServerErrorsNode = value["ServerErrors"]["MonitorItem"];
	for (auto valueServerErrorsMonitorItem : allServerErrorsNode)
	{
		MonitorItem serverErrorsObject;
		if(!valueServerErrorsMonitorItem["ItemTime"].isNull())
			serverErrorsObject.itemTime = valueServerErrorsMonitorItem["ItemTime"].asString();
		if(!valueServerErrorsMonitorItem["ItemValue"].isNull())
			serverErrorsObject.itemValue = valueServerErrorsMonitorItem["ItemValue"].asString();
		serverErrors_.push_back(serverErrorsObject);
	}

}

std::vector<DescribeApiErrorDataResult::MonitorItem> DescribeApiErrorDataResult::getClientErrors()const
{
	return clientErrors_;
}

std::vector<DescribeApiErrorDataResult::MonitorItem> DescribeApiErrorDataResult::getServerErrors()const
{
	return serverErrors_;
}


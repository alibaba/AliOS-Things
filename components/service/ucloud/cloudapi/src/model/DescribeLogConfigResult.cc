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

#include <alibabacloud/cloudapi/model/DescribeLogConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::CloudAPI;
using namespace AlibabaCloud::CloudAPI::Model;

DescribeLogConfigResult::DescribeLogConfigResult() :
	ServiceResult()
{}

DescribeLogConfigResult::DescribeLogConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLogConfigResult::~DescribeLogConfigResult()
{}

void DescribeLogConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allLogInfosNode = value["LogInfos"]["LogInfo"];
	for (auto valueLogInfosLogInfo : allLogInfosNode)
	{
		LogInfo logInfosObject;
		if(!valueLogInfosLogInfo["RegionId"].isNull())
			logInfosObject.regionId = valueLogInfosLogInfo["RegionId"].asString();
		if(!valueLogInfosLogInfo["SlsProject"].isNull())
			logInfosObject.slsProject = valueLogInfosLogInfo["SlsProject"].asString();
		if(!valueLogInfosLogInfo["SlsLogStore"].isNull())
			logInfosObject.slsLogStore = valueLogInfosLogInfo["SlsLogStore"].asString();
		if(!valueLogInfosLogInfo["LogType"].isNull())
			logInfosObject.logType = valueLogInfosLogInfo["LogType"].asString();
		logInfos_.push_back(logInfosObject);
	}

}

std::vector<DescribeLogConfigResult::LogInfo> DescribeLogConfigResult::getLogInfos()const
{
	return logInfos_;
}


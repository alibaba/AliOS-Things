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

#include <alibabacloud/iot/model/BatchGetEdgeInstanceDeviceConfigResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchGetEdgeInstanceDeviceConfigResult::BatchGetEdgeInstanceDeviceConfigResult() :
	ServiceResult()
{}

BatchGetEdgeInstanceDeviceConfigResult::BatchGetEdgeInstanceDeviceConfigResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchGetEdgeInstanceDeviceConfigResult::~BatchGetEdgeInstanceDeviceConfigResult()
{}

void BatchGetEdgeInstanceDeviceConfigResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDeviceConfigListNode = value["DeviceConfigList"]["DeviceConfig"];
	for (auto valueDeviceConfigListDeviceConfig : allDeviceConfigListNode)
	{
		DeviceConfig deviceConfigListObject;
		if(!valueDeviceConfigListDeviceConfig["IotId"].isNull())
			deviceConfigListObject.iotId = valueDeviceConfigListDeviceConfig["IotId"].asString();
		auto configNode = value["Config"];
		if(!configNode["Format"].isNull())
			deviceConfigListObject.config.format = configNode["Format"].asString();
		if(!configNode["Content"].isNull())
			deviceConfigListObject.config.content = configNode["Content"].asString();
		deviceConfigList_.push_back(deviceConfigListObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<BatchGetEdgeInstanceDeviceConfigResult::DeviceConfig> BatchGetEdgeInstanceDeviceConfigResult::getDeviceConfigList()const
{
	return deviceConfigList_;
}

std::string BatchGetEdgeInstanceDeviceConfigResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string BatchGetEdgeInstanceDeviceConfigResult::getCode()const
{
	return code_;
}

bool BatchGetEdgeInstanceDeviceConfigResult::getSuccess()const
{
	return success_;
}


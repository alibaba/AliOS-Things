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

#include <alibabacloud/iot/model/BatchGetEdgeInstanceDeviceChannelResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchGetEdgeInstanceDeviceChannelResult::BatchGetEdgeInstanceDeviceChannelResult() :
	ServiceResult()
{}

BatchGetEdgeInstanceDeviceChannelResult::BatchGetEdgeInstanceDeviceChannelResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchGetEdgeInstanceDeviceChannelResult::~BatchGetEdgeInstanceDeviceChannelResult()
{}

void BatchGetEdgeInstanceDeviceChannelResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDeviceChannelListNode = value["DeviceChannelList"]["DeviceChannel"];
	for (auto valueDeviceChannelListDeviceChannel : allDeviceChannelListNode)
	{
		DeviceChannel deviceChannelListObject;
		if(!valueDeviceChannelListDeviceChannel["ChannelId"].isNull())
			deviceChannelListObject.channelId = valueDeviceChannelListDeviceChannel["ChannelId"].asString();
		if(!valueDeviceChannelListDeviceChannel["ChannelName"].isNull())
			deviceChannelListObject.channelName = valueDeviceChannelListDeviceChannel["ChannelName"].asString();
		if(!valueDeviceChannelListDeviceChannel["IotId"].isNull())
			deviceChannelListObject.iotId = valueDeviceChannelListDeviceChannel["IotId"].asString();
		deviceChannelList_.push_back(deviceChannelListObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::string BatchGetEdgeInstanceDeviceChannelResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string BatchGetEdgeInstanceDeviceChannelResult::getCode()const
{
	return code_;
}

bool BatchGetEdgeInstanceDeviceChannelResult::getSuccess()const
{
	return success_;
}

std::vector<BatchGetEdgeInstanceDeviceChannelResult::DeviceChannel> BatchGetEdgeInstanceDeviceChannelResult::getDeviceChannelList()const
{
	return deviceChannelList_;
}


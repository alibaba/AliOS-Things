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

#include <alibabacloud/iot/model/BatchGetEdgeInstanceChannelResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

BatchGetEdgeInstanceChannelResult::BatchGetEdgeInstanceChannelResult() :
	ServiceResult()
{}

BatchGetEdgeInstanceChannelResult::BatchGetEdgeInstanceChannelResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

BatchGetEdgeInstanceChannelResult::~BatchGetEdgeInstanceChannelResult()
{}

void BatchGetEdgeInstanceChannelResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDataNode = value["Data"]["Channel"];
	for (auto valueDataChannel : allDataNode)
	{
		Channel dataObject;
		if(!valueDataChannel["ChannelId"].isNull())
			dataObject.channelId = valueDataChannel["ChannelId"].asString();
		if(!valueDataChannel["ChannelName"].isNull())
			dataObject.channelName = valueDataChannel["ChannelName"].asString();
		auto allConfigListNode = allDataNode["ConfigList"]["Config"];
		for (auto allDataNodeConfigListConfig : allConfigListNode)
		{
			Channel::Config configListObject;
			if(!allDataNodeConfigListConfig["ConfigId"].isNull())
				configListObject.configId = allDataNodeConfigListConfig["ConfigId"].asString();
			if(!allDataNodeConfigListConfig["Format"].isNull())
				configListObject.format = allDataNodeConfigListConfig["Format"].asString();
			if(!allDataNodeConfigListConfig["Content"].isNull())
				configListObject.content = allDataNodeConfigListConfig["Content"].asString();
			if(!allDataNodeConfigListConfig["Key"].isNull())
				configListObject.key = allDataNodeConfigListConfig["Key"].asString();
			dataObject.configList.push_back(configListObject);
		}
		data_.push_back(dataObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

std::vector<BatchGetEdgeInstanceChannelResult::Channel> BatchGetEdgeInstanceChannelResult::getData()const
{
	return data_;
}

std::string BatchGetEdgeInstanceChannelResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string BatchGetEdgeInstanceChannelResult::getCode()const
{
	return code_;
}

bool BatchGetEdgeInstanceChannelResult::getSuccess()const
{
	return success_;
}


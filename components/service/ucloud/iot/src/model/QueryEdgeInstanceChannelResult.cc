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

#include <alibabacloud/iot/model/QueryEdgeInstanceChannelResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryEdgeInstanceChannelResult::QueryEdgeInstanceChannelResult() :
	ServiceResult()
{}

QueryEdgeInstanceChannelResult::QueryEdgeInstanceChannelResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryEdgeInstanceChannelResult::~QueryEdgeInstanceChannelResult()
{}

void QueryEdgeInstanceChannelResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["Total"].isNull())
		data_.total = std::stoi(dataNode["Total"].asString());
	if(!dataNode["PageSize"].isNull())
		data_.pageSize = std::stoi(dataNode["PageSize"].asString());
	if(!dataNode["CurrentPage"].isNull())
		data_.currentPage = std::stoi(dataNode["CurrentPage"].asString());
	auto allChannelListNode = dataNode["ChannelList"]["Channel"];
	for (auto dataNodeChannelListChannel : allChannelListNode)
	{
		Data::Channel channelObject;
		if(!dataNodeChannelListChannel["ChannelId"].isNull())
			channelObject.channelId = dataNodeChannelListChannel["ChannelId"].asString();
		if(!dataNodeChannelListChannel["ChannelName"].isNull())
			channelObject.channelName = dataNodeChannelListChannel["ChannelName"].asString();
		if(!dataNodeChannelListChannel["GmtCreate"].isNull())
			channelObject.gmtCreate = dataNodeChannelListChannel["GmtCreate"].asString();
		if(!dataNodeChannelListChannel["GmtModified"].isNull())
			channelObject.gmtModified = dataNodeChannelListChannel["GmtModified"].asString();
		if(!dataNodeChannelListChannel["GmtCreateTimestamp"].isNull())
			channelObject.gmtCreateTimestamp = std::stol(dataNodeChannelListChannel["GmtCreateTimestamp"].asString());
		if(!dataNodeChannelListChannel["GmtModifiedTimestamp"].isNull())
			channelObject.gmtModifiedTimestamp = std::stol(dataNodeChannelListChannel["GmtModifiedTimestamp"].asString());
		auto allConfigListNode = allChannelListNode["ConfigList"]["Config"];
		for (auto allChannelListNodeConfigListConfig : allConfigListNode)
		{
			Data::Channel::Config configListObject;
			if(!allChannelListNodeConfigListConfig["ConfigId"].isNull())
				configListObject.configId = allChannelListNodeConfigListConfig["ConfigId"].asString();
			if(!allChannelListNodeConfigListConfig["Format"].isNull())
				configListObject.format = allChannelListNodeConfigListConfig["Format"].asString();
			if(!allChannelListNodeConfigListConfig["Content"].isNull())
				configListObject.content = allChannelListNodeConfigListConfig["Content"].asString();
			if(!allChannelListNodeConfigListConfig["Key"].isNull())
				configListObject.key = allChannelListNodeConfigListConfig["Key"].asString();
			channelObject.configList.push_back(configListObject);
		}
		data_.channelList.push_back(channelObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();

}

QueryEdgeInstanceChannelResult::Data QueryEdgeInstanceChannelResult::getData()const
{
	return data_;
}

std::string QueryEdgeInstanceChannelResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryEdgeInstanceChannelResult::getCode()const
{
	return code_;
}

bool QueryEdgeInstanceChannelResult::getSuccess()const
{
	return success_;
}


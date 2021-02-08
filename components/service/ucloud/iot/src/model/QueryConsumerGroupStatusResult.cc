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

#include <alibabacloud/iot/model/QueryConsumerGroupStatusResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Iot;
using namespace AlibabaCloud::Iot::Model;

QueryConsumerGroupStatusResult::QueryConsumerGroupStatusResult() :
	ServiceResult()
{}

QueryConsumerGroupStatusResult::QueryConsumerGroupStatusResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

QueryConsumerGroupStatusResult::~QueryConsumerGroupStatusResult()
{}

void QueryConsumerGroupStatusResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allClientConnectionStatusListNode = value["ClientConnectionStatusList"]["ConsumerGroupClientConnectionInfo"];
	for (auto valueClientConnectionStatusListConsumerGroupClientConnectionInfo : allClientConnectionStatusListNode)
	{
		ConsumerGroupClientConnectionInfo clientConnectionStatusListObject;
		if(!valueClientConnectionStatusListConsumerGroupClientConnectionInfo["ClientId"].isNull())
			clientConnectionStatusListObject.clientId = valueClientConnectionStatusListConsumerGroupClientConnectionInfo["ClientId"].asString();
		if(!valueClientConnectionStatusListConsumerGroupClientConnectionInfo["ClientIpPort"].isNull())
			clientConnectionStatusListObject.clientIpPort = valueClientConnectionStatusListConsumerGroupClientConnectionInfo["ClientIpPort"].asString();
		if(!valueClientConnectionStatusListConsumerGroupClientConnectionInfo["OnlineTime"].isNull())
			clientConnectionStatusListObject.onlineTime = std::stol(valueClientConnectionStatusListConsumerGroupClientConnectionInfo["OnlineTime"].asString());
		clientConnectionStatusList_.push_back(clientConnectionStatusListObject);
	}
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";
	if(!value["ErrorMessage"].isNull())
		errorMessage_ = value["ErrorMessage"].asString();
	if(!value["AccumulationCount"].isNull())
		accumulationCount_ = std::stoi(value["AccumulationCount"].asString());
	if(!value["ConsumerSpeed"].isNull())
		consumerSpeed_ = std::stoi(value["ConsumerSpeed"].asString());
	if(!value["LastConsumerTime"].isNull())
		lastConsumerTime_ = value["LastConsumerTime"].asString();
	if(!value["Code"].isNull())
		code_ = value["Code"].asString();

}

std::vector<QueryConsumerGroupStatusResult::ConsumerGroupClientConnectionInfo> QueryConsumerGroupStatusResult::getClientConnectionStatusList()const
{
	return clientConnectionStatusList_;
}

std::string QueryConsumerGroupStatusResult::getLastConsumerTime()const
{
	return lastConsumerTime_;
}

std::string QueryConsumerGroupStatusResult::getErrorMessage()const
{
	return errorMessage_;
}

std::string QueryConsumerGroupStatusResult::getCode()const
{
	return code_;
}

bool QueryConsumerGroupStatusResult::getSuccess()const
{
	return success_;
}

int QueryConsumerGroupStatusResult::getAccumulationCount()const
{
	return accumulationCount_;
}

int QueryConsumerGroupStatusResult::getConsumerSpeed()const
{
	return consumerSpeed_;
}


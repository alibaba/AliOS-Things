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

#include <alibabacloud/config/model/DescribeDeliveryChannelsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Config;
using namespace AlibabaCloud::Config::Model;

DescribeDeliveryChannelsResult::DescribeDeliveryChannelsResult() :
	ServiceResult()
{}

DescribeDeliveryChannelsResult::DescribeDeliveryChannelsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeDeliveryChannelsResult::~DescribeDeliveryChannelsResult()
{}

void DescribeDeliveryChannelsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allDeliveryChannelsNode = value["DeliveryChannels"]["DeliveryChannel"];
	for (auto valueDeliveryChannelsDeliveryChannel : allDeliveryChannelsNode)
	{
		DeliveryChannel deliveryChannelsObject;
		if(!valueDeliveryChannelsDeliveryChannel["DeliveryChannelId"].isNull())
			deliveryChannelsObject.deliveryChannelId = valueDeliveryChannelsDeliveryChannel["DeliveryChannelId"].asString();
		if(!valueDeliveryChannelsDeliveryChannel["DeliveryChannelName"].isNull())
			deliveryChannelsObject.deliveryChannelName = valueDeliveryChannelsDeliveryChannel["DeliveryChannelName"].asString();
		if(!valueDeliveryChannelsDeliveryChannel["DeliveryChannelType"].isNull())
			deliveryChannelsObject.deliveryChannelType = valueDeliveryChannelsDeliveryChannel["DeliveryChannelType"].asString();
		if(!valueDeliveryChannelsDeliveryChannel["DeliveryChannelTargetArn"].isNull())
			deliveryChannelsObject.deliveryChannelTargetArn = valueDeliveryChannelsDeliveryChannel["DeliveryChannelTargetArn"].asString();
		if(!valueDeliveryChannelsDeliveryChannel["DeliveryChannelAssumeRoleArn"].isNull())
			deliveryChannelsObject.deliveryChannelAssumeRoleArn = valueDeliveryChannelsDeliveryChannel["DeliveryChannelAssumeRoleArn"].asString();
		if(!valueDeliveryChannelsDeliveryChannel["DeliveryChannelCondition"].isNull())
			deliveryChannelsObject.deliveryChannelCondition = valueDeliveryChannelsDeliveryChannel["DeliveryChannelCondition"].asString();
		if(!valueDeliveryChannelsDeliveryChannel["Description"].isNull())
			deliveryChannelsObject.description = valueDeliveryChannelsDeliveryChannel["Description"].asString();
		if(!valueDeliveryChannelsDeliveryChannel["Status"].isNull())
			deliveryChannelsObject.status = std::stoi(valueDeliveryChannelsDeliveryChannel["Status"].asString());
		deliveryChannels_.push_back(deliveryChannelsObject);
	}

}

std::vector<DescribeDeliveryChannelsResult::DeliveryChannel> DescribeDeliveryChannelsResult::getDeliveryChannels()const
{
	return deliveryChannels_;
}


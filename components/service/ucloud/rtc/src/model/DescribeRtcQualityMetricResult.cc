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

#include <alibabacloud/rtc/model/DescribeRtcQualityMetricResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcQualityMetricResult::DescribeRtcQualityMetricResult() :
	ServiceResult()
{}

DescribeRtcQualityMetricResult::DescribeRtcQualityMetricResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcQualityMetricResult::~DescribeRtcQualityMetricResult()
{}

void DescribeRtcQualityMetricResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto qualityMetricNode = value["QualityMetric"];
	auto subInfoNode = qualityMetricNode["SubInfo"];
	auto allDataNode = subInfoNode["Data"]["DataItem"];
	for (auto subInfoNodeDataDataItem : allDataNode)
	{
		QualityMetric::SubInfo::DataItem dataItemObject;
		if(!subInfoNodeDataDataItem["Time"].isNull())
			dataItemObject.time = subInfoNodeDataDataItem["Time"].asString();
		auto fpsNode = value["Fps"];
		if(!fpsNode["Small"].isNull())
			dataItemObject.fps.small = std::stol(fpsNode["Small"].asString());
		if(!fpsNode["Large"].isNull())
			dataItemObject.fps.large = std::stol(fpsNode["Large"].asString());
		if(!fpsNode["Super"].isNull())
			dataItemObject.fps.super = std::stol(fpsNode["Super"].asString());
		if(!fpsNode["Share"].isNull())
			dataItemObject.fps.share = std::stol(fpsNode["Share"].asString());
		auto resolutionNode = value["Resolution"];
		if(!resolutionNode["Small"].isNull())
			dataItemObject.resolution.small = std::stol(resolutionNode["Small"].asString());
		if(!resolutionNode["Large"].isNull())
			dataItemObject.resolution.large = std::stol(resolutionNode["Large"].asString());
		if(!resolutionNode["Super"].isNull())
			dataItemObject.resolution.super = std::stol(resolutionNode["Super"].asString());
		if(!resolutionNode["Share"].isNull())
			dataItemObject.resolution.share = std::stol(resolutionNode["Share"].asString());
		auto bitRateNode = value["BitRate"];
		if(!bitRateNode["Audio"].isNull())
			dataItemObject.bitRate.audio = std::stol(bitRateNode["Audio"].asString());
		if(!bitRateNode["Small"].isNull())
			dataItemObject.bitRate.small = std::stol(bitRateNode["Small"].asString());
		if(!bitRateNode["Large"].isNull())
			dataItemObject.bitRate.large = std::stol(bitRateNode["Large"].asString());
		if(!bitRateNode["Super"].isNull())
			dataItemObject.bitRate.super = std::stol(bitRateNode["Super"].asString());
		if(!bitRateNode["Share"].isNull())
			dataItemObject.bitRate.share = std::stol(bitRateNode["Share"].asString());
		auto networkNode = value["Network"];
		if(!networkNode["Loss"].isNull())
			dataItemObject.network.loss = std::stof(networkNode["Loss"].asString());
		if(!networkNode["Rtt"].isNull())
			dataItemObject.network.rtt = std::stol(networkNode["Rtt"].asString());
		qualityMetric_.subInfo.data.push_back(dataItemObject);
	}
	auto allEventsNode = subInfoNode["Events"]["EventsItem"];
	for (auto subInfoNodeEventsEventsItem : allEventsNode)
	{
		QualityMetric::SubInfo::EventsItem eventsItemObject;
		if(!subInfoNodeEventsEventsItem["Time"].isNull())
			eventsItemObject.time = subInfoNodeEventsEventsItem["Time"].asString();
		if(!subInfoNodeEventsEventsItem["EventId"].isNull())
			eventsItemObject.eventId = std::stol(subInfoNodeEventsEventsItem["EventId"].asString());
		if(!subInfoNodeEventsEventsItem["Name"].isNull())
			eventsItemObject.name = subInfoNodeEventsEventsItem["Name"].asString();
		if(!subInfoNodeEventsEventsItem["Category"].isNull())
			eventsItemObject.category = subInfoNodeEventsEventsItem["Category"].asString();
		if(!subInfoNodeEventsEventsItem["Result"].isNull())
			eventsItemObject.result = subInfoNodeEventsEventsItem["Result"].asString();
		if(!subInfoNodeEventsEventsItem["Description"].isNull())
			eventsItemObject.description = subInfoNodeEventsEventsItem["Description"].asString();
		qualityMetric_.subInfo.events.push_back(eventsItemObject);
	}
	auto userNode = subInfoNode["User"];
	if(!userNode["UserId"].isNull())
		qualityMetric_.subInfo.user.userId = userNode["UserId"].asString();
	if(!userNode["SDKVersion"].isNull())
		qualityMetric_.subInfo.user.sDKVersion = userNode["SDKVersion"].asString();
	if(!userNode["OS"].isNull())
		qualityMetric_.subInfo.user.oS = userNode["OS"].asString();
	if(!userNode["Province"].isNull())
		qualityMetric_.subInfo.user.province = userNode["Province"].asString();
	auto pubInfoNode = qualityMetricNode["PubInfo"];
	auto allData2Node = pubInfoNode["Data"]["DataItem"];
	for (auto pubInfoNodeDataDataItem : allData2Node)
	{
		QualityMetric::PubInfo::DataItem4 dataItem4Object;
		if(!pubInfoNodeDataDataItem["Time"].isNull())
			dataItem4Object.time = pubInfoNodeDataDataItem["Time"].asString();
		auto fps5Node = value["Fps"];
		if(!fps5Node["Small"].isNull())
			dataItem4Object.fps5.small = std::stol(fps5Node["Small"].asString());
		if(!fps5Node["Large"].isNull())
			dataItem4Object.fps5.large = std::stol(fps5Node["Large"].asString());
		if(!fps5Node["Super"].isNull())
			dataItem4Object.fps5.super = std::stol(fps5Node["Super"].asString());
		if(!fps5Node["Share"].isNull())
			dataItem4Object.fps5.share = std::stol(fps5Node["Share"].asString());
		auto resolution6Node = value["Resolution"];
		if(!resolution6Node["Small"].isNull())
			dataItem4Object.resolution6.small = std::stol(resolution6Node["Small"].asString());
		if(!resolution6Node["Large"].isNull())
			dataItem4Object.resolution6.large = std::stol(resolution6Node["Large"].asString());
		if(!resolution6Node["Super"].isNull())
			dataItem4Object.resolution6.super = std::stol(resolution6Node["Super"].asString());
		if(!resolution6Node["Share"].isNull())
			dataItem4Object.resolution6.share = std::stol(resolution6Node["Share"].asString());
		auto bitRate7Node = value["BitRate"];
		if(!bitRate7Node["Audio"].isNull())
			dataItem4Object.bitRate7.audio = std::stol(bitRate7Node["Audio"].asString());
		if(!bitRate7Node["Small"].isNull())
			dataItem4Object.bitRate7.small = std::stol(bitRate7Node["Small"].asString());
		if(!bitRate7Node["Large"].isNull())
			dataItem4Object.bitRate7.large = std::stol(bitRate7Node["Large"].asString());
		if(!bitRate7Node["Super"].isNull())
			dataItem4Object.bitRate7.super = std::stol(bitRate7Node["Super"].asString());
		if(!bitRate7Node["Share"].isNull())
			dataItem4Object.bitRate7.share = std::stol(bitRate7Node["Share"].asString());
		auto network8Node = value["Network"];
		if(!network8Node["Loss"].isNull())
			dataItem4Object.network8.loss = std::stof(network8Node["Loss"].asString());
		if(!network8Node["Rtt"].isNull())
			dataItem4Object.network8.rtt = std::stol(network8Node["Rtt"].asString());
		qualityMetric_.pubInfo.data2.push_back(dataItem4Object);
	}
	auto allEvents3Node = pubInfoNode["Events"]["EventsItem"];
	for (auto pubInfoNodeEventsEventsItem : allEvents3Node)
	{
		QualityMetric::PubInfo::EventsItem9 eventsItem9Object;
		if(!pubInfoNodeEventsEventsItem["Time"].isNull())
			eventsItem9Object.time = pubInfoNodeEventsEventsItem["Time"].asString();
		if(!pubInfoNodeEventsEventsItem["EventId"].isNull())
			eventsItem9Object.eventId = std::stol(pubInfoNodeEventsEventsItem["EventId"].asString());
		if(!pubInfoNodeEventsEventsItem["Name"].isNull())
			eventsItem9Object.name = pubInfoNodeEventsEventsItem["Name"].asString();
		if(!pubInfoNodeEventsEventsItem["Category"].isNull())
			eventsItem9Object.category = pubInfoNodeEventsEventsItem["Category"].asString();
		if(!pubInfoNodeEventsEventsItem["Result"].isNull())
			eventsItem9Object.result = pubInfoNodeEventsEventsItem["Result"].asString();
		if(!pubInfoNodeEventsEventsItem["Description"].isNull())
			eventsItem9Object.description = pubInfoNodeEventsEventsItem["Description"].asString();
		qualityMetric_.pubInfo.events3.push_back(eventsItem9Object);
	}
	auto user1Node = pubInfoNode["User"];
	if(!user1Node["UserId"].isNull())
		qualityMetric_.pubInfo.user1.userId = user1Node["UserId"].asString();
	if(!user1Node["SDKVersion"].isNull())
		qualityMetric_.pubInfo.user1.sDKVersion = user1Node["SDKVersion"].asString();
	if(!user1Node["OS"].isNull())
		qualityMetric_.pubInfo.user1.oS = user1Node["OS"].asString();
	if(!user1Node["Province"].isNull())
		qualityMetric_.pubInfo.user1.province = user1Node["Province"].asString();

}

DescribeRtcQualityMetricResult::QualityMetric DescribeRtcQualityMetricResult::getQualityMetric()const
{
	return qualityMetric_;
}


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

#include <alibabacloud/rtc/model/DescribeRtcChannelMetricsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Rtc;
using namespace AlibabaCloud::Rtc::Model;

DescribeRtcChannelMetricsResult::DescribeRtcChannelMetricsResult() :
	ServiceResult()
{}

DescribeRtcChannelMetricsResult::DescribeRtcChannelMetricsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRtcChannelMetricsResult::~DescribeRtcChannelMetricsResult()
{}

void DescribeRtcChannelMetricsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allMetricsNode = value["Metrics"]["Metric"];
	for (auto valueMetricsMetric : allMetricsNode)
	{
		Metric metricsObject;
		if(!valueMetricsMetric["Uid"].isNull())
			metricsObject.uid = valueMetricsMetric["Uid"].asString();
		if(!valueMetricsMetric["Mid"].isNull())
			metricsObject.mid = valueMetricsMetric["Mid"].asString();
		auto allKVs = value["KVs"]["KV"];
		for (auto value : allKVs)
			metricsObject.kVs.push_back(value.asString());
		metrics_.push_back(metricsObject);
	}

}

std::vector<DescribeRtcChannelMetricsResult::Metric> DescribeRtcChannelMetricsResult::getMetrics()const
{
	return metrics_;
}


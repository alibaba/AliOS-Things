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

#include <alibabacloud/schedulerx2/model/GetJobInfoResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Schedulerx2;
using namespace AlibabaCloud::Schedulerx2::Model;

GetJobInfoResult::GetJobInfoResult() :
	ServiceResult()
{}

GetJobInfoResult::GetJobInfoResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetJobInfoResult::~GetJobInfoResult()
{}

void GetJobInfoResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	auto jobConfigInfoNode = dataNode["JobConfigInfo"];
	if(!jobConfigInfoNode["ClassName"].isNull())
		data_.jobConfigInfo.className = jobConfigInfoNode["ClassName"].asString();
	if(!jobConfigInfoNode["JarUrl"].isNull())
		data_.jobConfigInfo.jarUrl = jobConfigInfoNode["JarUrl"].asString();
	if(!jobConfigInfoNode["Content"].isNull())
		data_.jobConfigInfo.content = jobConfigInfoNode["Content"].asString();
	if(!jobConfigInfoNode["Name"].isNull())
		data_.jobConfigInfo.name = jobConfigInfoNode["Name"].asString();
	if(!jobConfigInfoNode["Description"].isNull())
		data_.jobConfigInfo.description = jobConfigInfoNode["Description"].asString();
	if(!jobConfigInfoNode["Status"].isNull())
		data_.jobConfigInfo.status = std::stoi(jobConfigInfoNode["Status"].asString());
	if(!jobConfigInfoNode["ExecuteMode"].isNull())
		data_.jobConfigInfo.executeMode = jobConfigInfoNode["ExecuteMode"].asString();
	if(!jobConfigInfoNode["Parameters"].isNull())
		data_.jobConfigInfo.parameters = jobConfigInfoNode["Parameters"].asString();
	if(!jobConfigInfoNode["MaxConcurrency"].isNull())
		data_.jobConfigInfo.maxConcurrency = jobConfigInfoNode["MaxConcurrency"].asString();
	if(!jobConfigInfoNode["MaxAttempt"].isNull())
		data_.jobConfigInfo.maxAttempt = std::stoi(jobConfigInfoNode["MaxAttempt"].asString());
	if(!jobConfigInfoNode["AttemptInterval"].isNull())
		data_.jobConfigInfo.attemptInterval = std::stoi(jobConfigInfoNode["AttemptInterval"].asString());
	auto mapTaskXAttrsNode = jobConfigInfoNode["MapTaskXAttrs"];
	if(!mapTaskXAttrsNode["PageSize"].isNull())
		data_.jobConfigInfo.mapTaskXAttrs.pageSize = std::stoi(mapTaskXAttrsNode["PageSize"].asString());
	if(!mapTaskXAttrsNode["ConsumerSize"].isNull())
		data_.jobConfigInfo.mapTaskXAttrs.consumerSize = std::stoi(mapTaskXAttrsNode["ConsumerSize"].asString());
	if(!mapTaskXAttrsNode["QueueSize"].isNull())
		data_.jobConfigInfo.mapTaskXAttrs.queueSize = std::stoi(mapTaskXAttrsNode["QueueSize"].asString());
	if(!mapTaskXAttrsNode["DispatcherSize"].isNull())
		data_.jobConfigInfo.mapTaskXAttrs.dispatcherSize = std::stoi(mapTaskXAttrsNode["DispatcherSize"].asString());
	if(!mapTaskXAttrsNode["TaskMaxAttempt"].isNull())
		data_.jobConfigInfo.mapTaskXAttrs.taskMaxAttempt = std::stoi(mapTaskXAttrsNode["TaskMaxAttempt"].asString());
	if(!mapTaskXAttrsNode["TaskAttemptInterval"].isNull())
		data_.jobConfigInfo.mapTaskXAttrs.taskAttemptInterval = std::stoi(mapTaskXAttrsNode["TaskAttemptInterval"].asString());
	auto timeConfigNode = jobConfigInfoNode["TimeConfig"];
	if(!timeConfigNode["TimeType"].isNull())
		data_.jobConfigInfo.timeConfig.timeType = std::stoi(timeConfigNode["TimeType"].asString());
	if(!timeConfigNode["TimeExpression"].isNull())
		data_.jobConfigInfo.timeConfig.timeExpression = timeConfigNode["TimeExpression"].asString();
	if(!timeConfigNode["Calendar"].isNull())
		data_.jobConfigInfo.timeConfig.calendar = timeConfigNode["Calendar"].asString();
	if(!timeConfigNode["DataOffset"].isNull())
		data_.jobConfigInfo.timeConfig.dataOffset = std::stoi(timeConfigNode["DataOffset"].asString());
	auto jobMonitorInfoNode = jobConfigInfoNode["JobMonitorInfo"];
	auto allContactInfoNode = jobMonitorInfoNode["ContactInfo"]["ContactInfoItem"];
	for (auto jobMonitorInfoNodeContactInfoContactInfoItem : allContactInfoNode)
	{
		Data::JobConfigInfo::JobMonitorInfo::ContactInfoItem contactInfoItemObject;
		if(!jobMonitorInfoNodeContactInfoContactInfoItem["UserName"].isNull())
			contactInfoItemObject.userName = jobMonitorInfoNodeContactInfoContactInfoItem["UserName"].asString();
		if(!jobMonitorInfoNodeContactInfoContactInfoItem["UserPhone"].isNull())
			contactInfoItemObject.userPhone = jobMonitorInfoNodeContactInfoContactInfoItem["UserPhone"].asString();
		data_.jobConfigInfo.jobMonitorInfo.contactInfo.push_back(contactInfoItemObject);
	}
	auto monitorConfigNode = jobMonitorInfoNode["MonitorConfig"];
	if(!monitorConfigNode["TimeoutEnable"].isNull())
		data_.jobConfigInfo.jobMonitorInfo.monitorConfig.timeoutEnable = monitorConfigNode["TimeoutEnable"].asString() == "true";
	if(!monitorConfigNode["Timeout"].isNull())
		data_.jobConfigInfo.jobMonitorInfo.monitorConfig.timeout = std::stol(monitorConfigNode["Timeout"].asString());
	if(!monitorConfigNode["TimeoutKillEnable"].isNull())
		data_.jobConfigInfo.jobMonitorInfo.monitorConfig.timeoutKillEnable = monitorConfigNode["TimeoutKillEnable"].asString() == "true";
	if(!monitorConfigNode["FailEnable"].isNull())
		data_.jobConfigInfo.jobMonitorInfo.monitorConfig.failEnable = monitorConfigNode["FailEnable"].asString() == "true";
	if(!monitorConfigNode["SendChannel"].isNull())
		data_.jobConfigInfo.jobMonitorInfo.monitorConfig.sendChannel = monitorConfigNode["SendChannel"].asString();
	if(!value["Code"].isNull())
		code_ = std::stoi(value["Code"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

std::string GetJobInfoResult::getMessage()const
{
	return message_;
}

GetJobInfoResult::Data GetJobInfoResult::getData()const
{
	return data_;
}

int GetJobInfoResult::getCode()const
{
	return code_;
}

bool GetJobInfoResult::getSuccess()const
{
	return success_;
}


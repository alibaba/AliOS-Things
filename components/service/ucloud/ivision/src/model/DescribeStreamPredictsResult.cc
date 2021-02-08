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

#include <alibabacloud/ivision/model/DescribeStreamPredictsResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ivision;
using namespace AlibabaCloud::Ivision::Model;

DescribeStreamPredictsResult::DescribeStreamPredictsResult() :
	ServiceResult()
{}

DescribeStreamPredictsResult::DescribeStreamPredictsResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeStreamPredictsResult::~DescribeStreamPredictsResult()
{}

void DescribeStreamPredictsResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allStreamPredictsNode = value["StreamPredicts"]["StreamPredict"];
	for (auto valueStreamPredictsStreamPredict : allStreamPredictsNode)
	{
		StreamPredict streamPredictsObject;
		if(!valueStreamPredictsStreamPredict["PredictId"].isNull())
			streamPredictsObject.predictId = valueStreamPredictsStreamPredict["PredictId"].asString();
		if(!valueStreamPredictsStreamPredict["PredictTemplateId"].isNull())
			streamPredictsObject.predictTemplateId = valueStreamPredictsStreamPredict["PredictTemplateId"].asString();
		if(!valueStreamPredictsStreamPredict["StreamType"].isNull())
			streamPredictsObject.streamType = valueStreamPredictsStreamPredict["StreamType"].asString();
		if(!valueStreamPredictsStreamPredict["StreamId"].isNull())
			streamPredictsObject.streamId = valueStreamPredictsStreamPredict["StreamId"].asString();
		if(!valueStreamPredictsStreamPredict["ModelIds"].isNull())
			streamPredictsObject.modelIds = valueStreamPredictsStreamPredict["ModelIds"].asString();
		if(!valueStreamPredictsStreamPredict["ProbabilityThresholds"].isNull())
			streamPredictsObject.probabilityThresholds = valueStreamPredictsStreamPredict["ProbabilityThresholds"].asString();
		if(!valueStreamPredictsStreamPredict["DetectIntervals"].isNull())
			streamPredictsObject.detectIntervals = valueStreamPredictsStreamPredict["DetectIntervals"].asString();
		if(!valueStreamPredictsStreamPredict["Output"].isNull())
			streamPredictsObject.output = valueStreamPredictsStreamPredict["Output"].asString();
		if(!valueStreamPredictsStreamPredict["Notify"].isNull())
			streamPredictsObject.notify = valueStreamPredictsStreamPredict["Notify"].asString();
		if(!valueStreamPredictsStreamPredict["UserData"].isNull())
			streamPredictsObject.userData = valueStreamPredictsStreamPredict["UserData"].asString();
		if(!valueStreamPredictsStreamPredict["FaceGroupId"].isNull())
			streamPredictsObject.faceGroupId = valueStreamPredictsStreamPredict["FaceGroupId"].asString();
		if(!valueStreamPredictsStreamPredict["ModelUserData"].isNull())
			streamPredictsObject.modelUserData = valueStreamPredictsStreamPredict["ModelUserData"].asString();
		if(!valueStreamPredictsStreamPredict["CreationTime"].isNull())
			streamPredictsObject.creationTime = valueStreamPredictsStreamPredict["CreationTime"].asString();
		if(!valueStreamPredictsStreamPredict["AutoStart"].isNull())
			streamPredictsObject.autoStart = valueStreamPredictsStreamPredict["AutoStart"].asString();
		if(!valueStreamPredictsStreamPredict["Status"].isNull())
			streamPredictsObject.status = valueStreamPredictsStreamPredict["Status"].asString();
		streamPredicts_.push_back(streamPredictsObject);
	}
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stol(value["TotalNum"].asString());
	if(!value["CurrentPage"].isNull())
		currentPage_ = std::stol(value["CurrentPage"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stol(value["PageSize"].asString());
	if(!value["NextPageToken"].isNull())
		nextPageToken_ = value["NextPageToken"].asString();

}

long DescribeStreamPredictsResult::getTotalNum()const
{
	return totalNum_;
}

long DescribeStreamPredictsResult::getPageSize()const
{
	return pageSize_;
}

long DescribeStreamPredictsResult::getCurrentPage()const
{
	return currentPage_;
}

std::string DescribeStreamPredictsResult::getNextPageToken()const
{
	return nextPageToken_;
}

std::vector<DescribeStreamPredictsResult::StreamPredict> DescribeStreamPredictsResult::getStreamPredicts()const
{
	return streamPredicts_;
}


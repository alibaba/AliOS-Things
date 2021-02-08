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

#include <alibabacloud/ivision/model/DescribeStreamPredictResultResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Ivision;
using namespace AlibabaCloud::Ivision::Model;

DescribeStreamPredictResultResult::DescribeStreamPredictResultResult() :
	ServiceResult()
{}

DescribeStreamPredictResultResult::DescribeStreamPredictResultResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeStreamPredictResultResult::~DescribeStreamPredictResultResult()
{}

void DescribeStreamPredictResultResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allStreamPredictDatasNode = value["StreamPredictDatas"]["StreamPredictData"];
	for (auto valueStreamPredictDatasStreamPredictData : allStreamPredictDatasNode)
	{
		StreamPredictData streamPredictDatasObject;
		if(!valueStreamPredictDatasStreamPredictData["PredictId"].isNull())
			streamPredictDatasObject.predictId = valueStreamPredictDatasStreamPredictData["PredictId"].asString();
		if(!valueStreamPredictDatasStreamPredictData["ModelId"].isNull())
			streamPredictDatasObject.modelId = valueStreamPredictDatasStreamPredictData["ModelId"].asString();
		if(!valueStreamPredictDatasStreamPredictData["DataUrl"].isNull())
			streamPredictDatasObject.dataUrl = valueStreamPredictDatasStreamPredictData["DataUrl"].asString();
		if(!valueStreamPredictDatasStreamPredictData["Timestamp"].isNull())
			streamPredictDatasObject.timestamp = std::stol(valueStreamPredictDatasStreamPredictData["Timestamp"].asString());
		if(!valueStreamPredictDatasStreamPredictData["PredictTime"].isNull())
			streamPredictDatasObject.predictTime = valueStreamPredictDatasStreamPredictData["PredictTime"].asString();
		if(!valueStreamPredictDatasStreamPredictData["Status"].isNull())
			streamPredictDatasObject.status = valueStreamPredictDatasStreamPredictData["Status"].asString();
		if(!valueStreamPredictDatasStreamPredictData["PredictResult"].isNull())
			streamPredictDatasObject.predictResult = valueStreamPredictDatasStreamPredictData["PredictResult"].asString();
		streamPredictDatas_.push_back(streamPredictDatasObject);
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

std::vector<DescribeStreamPredictResultResult::StreamPredictData> DescribeStreamPredictResultResult::getStreamPredictDatas()const
{
	return streamPredictDatas_;
}

long DescribeStreamPredictResultResult::getTotalNum()const
{
	return totalNum_;
}

long DescribeStreamPredictResultResult::getPageSize()const
{
	return pageSize_;
}

long DescribeStreamPredictResultResult::getCurrentPage()const
{
	return currentPage_;
}

std::string DescribeStreamPredictResultResult::getNextPageToken()const
{
	return nextPageToken_;
}


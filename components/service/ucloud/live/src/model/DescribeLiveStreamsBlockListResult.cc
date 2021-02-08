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

#include <alibabacloud/live/model/DescribeLiveStreamsBlockListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamsBlockListResult::DescribeLiveStreamsBlockListResult() :
	ServiceResult()
{}

DescribeLiveStreamsBlockListResult::DescribeLiveStreamsBlockListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamsBlockListResult::~DescribeLiveStreamsBlockListResult()
{}

void DescribeLiveStreamsBlockListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allStreamUrls = value["StreamUrls"]["StreamUrl"];
	for (const auto &item : allStreamUrls)
		streamUrls_.push_back(item.asString());
	if(!value["DomainName"].isNull())
		domainName_ = value["DomainName"].asString();
	if(!value["PageNum"].isNull())
		pageNum_ = std::stoi(value["PageNum"].asString());
	if(!value["PageSize"].isNull())
		pageSize_ = std::stoi(value["PageSize"].asString());
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stoi(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stoi(value["TotalPage"].asString());

}

int DescribeLiveStreamsBlockListResult::getTotalNum()const
{
	return totalNum_;
}

int DescribeLiveStreamsBlockListResult::getPageNum()const
{
	return pageNum_;
}

int DescribeLiveStreamsBlockListResult::getPageSize()const
{
	return pageSize_;
}

int DescribeLiveStreamsBlockListResult::getTotalPage()const
{
	return totalPage_;
}

std::string DescribeLiveStreamsBlockListResult::getDomainName()const
{
	return domainName_;
}

std::vector<std::string> DescribeLiveStreamsBlockListResult::getStreamUrls()const
{
	return streamUrls_;
}


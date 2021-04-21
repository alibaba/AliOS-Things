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

#include <alibabacloud/imageseg/model/SegmentCommonImageResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Imageseg;
using namespace AlibabaCloud::Imageseg::Model;

SegmentCommonImageResult::SegmentCommonImageResult() :
	ServiceResult()
{}

SegmentCommonImageResult::SegmentCommonImageResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

SegmentCommonImageResult::~SegmentCommonImageResult()
{}

void SegmentCommonImageResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto dataNode = value["Data"];
	if(!dataNode["ImageURL"].isNull())
		data_.imageURL = dataNode["ImageURL"].asString();

}

SegmentCommonImageResult::Data SegmentCommonImageResult::getData()const
{
	return data_;
}


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

#include <alibabacloud/live/model/DescribeLiveStreamRecordIndexFileResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeLiveStreamRecordIndexFileResult::DescribeLiveStreamRecordIndexFileResult() :
	ServiceResult()
{}

DescribeLiveStreamRecordIndexFileResult::DescribeLiveStreamRecordIndexFileResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeLiveStreamRecordIndexFileResult::~DescribeLiveStreamRecordIndexFileResult()
{}

void DescribeLiveStreamRecordIndexFileResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto recordIndexInfoNode = value["RecordIndexInfo"];
	if(!recordIndexInfoNode["RecordId"].isNull())
		recordIndexInfo_.recordId = recordIndexInfoNode["RecordId"].asString();
	if(!recordIndexInfoNode["RecordUrl"].isNull())
		recordIndexInfo_.recordUrl = recordIndexInfoNode["RecordUrl"].asString();
	if(!recordIndexInfoNode["DomainName"].isNull())
		recordIndexInfo_.domainName = recordIndexInfoNode["DomainName"].asString();
	if(!recordIndexInfoNode["AppName"].isNull())
		recordIndexInfo_.appName = recordIndexInfoNode["AppName"].asString();
	if(!recordIndexInfoNode["StreamName"].isNull())
		recordIndexInfo_.streamName = recordIndexInfoNode["StreamName"].asString();
	if(!recordIndexInfoNode["OssBucket"].isNull())
		recordIndexInfo_.ossBucket = recordIndexInfoNode["OssBucket"].asString();
	if(!recordIndexInfoNode["OssEndpoint"].isNull())
		recordIndexInfo_.ossEndpoint = recordIndexInfoNode["OssEndpoint"].asString();
	if(!recordIndexInfoNode["OssObject"].isNull())
		recordIndexInfo_.ossObject = recordIndexInfoNode["OssObject"].asString();
	if(!recordIndexInfoNode["StartTime"].isNull())
		recordIndexInfo_.startTime = recordIndexInfoNode["StartTime"].asString();
	if(!recordIndexInfoNode["EndTime"].isNull())
		recordIndexInfo_.endTime = recordIndexInfoNode["EndTime"].asString();
	if(!recordIndexInfoNode["Duration"].isNull())
		recordIndexInfo_.duration = std::stof(recordIndexInfoNode["Duration"].asString());
	if(!recordIndexInfoNode["Height"].isNull())
		recordIndexInfo_.height = std::stoi(recordIndexInfoNode["Height"].asString());
	if(!recordIndexInfoNode["Width"].isNull())
		recordIndexInfo_.width = std::stoi(recordIndexInfoNode["Width"].asString());
	if(!recordIndexInfoNode["CreateTime"].isNull())
		recordIndexInfo_.createTime = recordIndexInfoNode["CreateTime"].asString();

}

DescribeLiveStreamRecordIndexFileResult::RecordIndexInfo DescribeLiveStreamRecordIndexFileResult::getRecordIndexInfo()const
{
	return recordIndexInfo_;
}


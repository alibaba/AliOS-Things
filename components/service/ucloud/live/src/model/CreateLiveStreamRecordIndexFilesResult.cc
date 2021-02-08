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

#include <alibabacloud/live/model/CreateLiveStreamRecordIndexFilesResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

CreateLiveStreamRecordIndexFilesResult::CreateLiveStreamRecordIndexFilesResult() :
	ServiceResult()
{}

CreateLiveStreamRecordIndexFilesResult::CreateLiveStreamRecordIndexFilesResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

CreateLiveStreamRecordIndexFilesResult::~CreateLiveStreamRecordIndexFilesResult()
{}

void CreateLiveStreamRecordIndexFilesResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto recordInfoNode = value["RecordInfo"];
	if(!recordInfoNode["RecordId"].isNull())
		recordInfo_.recordId = recordInfoNode["RecordId"].asString();
	if(!recordInfoNode["RecordUrl"].isNull())
		recordInfo_.recordUrl = recordInfoNode["RecordUrl"].asString();
	if(!recordInfoNode["DomainName"].isNull())
		recordInfo_.domainName = recordInfoNode["DomainName"].asString();
	if(!recordInfoNode["AppName"].isNull())
		recordInfo_.appName = recordInfoNode["AppName"].asString();
	if(!recordInfoNode["StreamName"].isNull())
		recordInfo_.streamName = recordInfoNode["StreamName"].asString();
	if(!recordInfoNode["OssBucket"].isNull())
		recordInfo_.ossBucket = recordInfoNode["OssBucket"].asString();
	if(!recordInfoNode["OssEndpoint"].isNull())
		recordInfo_.ossEndpoint = recordInfoNode["OssEndpoint"].asString();
	if(!recordInfoNode["OssObject"].isNull())
		recordInfo_.ossObject = recordInfoNode["OssObject"].asString();
	if(!recordInfoNode["StartTime"].isNull())
		recordInfo_.startTime = recordInfoNode["StartTime"].asString();
	if(!recordInfoNode["EndTime"].isNull())
		recordInfo_.endTime = recordInfoNode["EndTime"].asString();
	if(!recordInfoNode["Duration"].isNull())
		recordInfo_.duration = std::stof(recordInfoNode["Duration"].asString());
	if(!recordInfoNode["Height"].isNull())
		recordInfo_.height = std::stoi(recordInfoNode["Height"].asString());
	if(!recordInfoNode["Width"].isNull())
		recordInfo_.width = std::stoi(recordInfoNode["Width"].asString());
	if(!recordInfoNode["CreateTime"].isNull())
		recordInfo_.createTime = recordInfoNode["CreateTime"].asString();

}

CreateLiveStreamRecordIndexFilesResult::RecordInfo CreateLiveStreamRecordIndexFilesResult::getRecordInfo()const
{
	return recordInfo_;
}


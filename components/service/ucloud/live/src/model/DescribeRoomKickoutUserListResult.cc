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

#include <alibabacloud/live/model/DescribeRoomKickoutUserListResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Live;
using namespace AlibabaCloud::Live::Model;

DescribeRoomKickoutUserListResult::DescribeRoomKickoutUserListResult() :
	ServiceResult()
{}

DescribeRoomKickoutUserListResult::DescribeRoomKickoutUserListResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

DescribeRoomKickoutUserListResult::~DescribeRoomKickoutUserListResult()
{}

void DescribeRoomKickoutUserListResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto allUserListNode = value["UserList"]["User"];
	for (auto valueUserListUser : allUserListNode)
	{
		User userListObject;
		if(!valueUserListUser["AppUid"].isNull())
			userListObject.appUid = valueUserListUser["AppUid"].asString();
		if(!valueUserListUser["OpStartTime"].isNull())
			userListObject.opStartTime = valueUserListUser["OpStartTime"].asString();
		if(!valueUserListUser["OpEndTime"].isNull())
			userListObject.opEndTime = valueUserListUser["OpEndTime"].asString();
		userList_.push_back(userListObject);
	}
	if(!value["TotalNum"].isNull())
		totalNum_ = std::stoi(value["TotalNum"].asString());
	if(!value["TotalPage"].isNull())
		totalPage_ = std::stoi(value["TotalPage"].asString());

}

int DescribeRoomKickoutUserListResult::getTotalNum()const
{
	return totalNum_;
}

int DescribeRoomKickoutUserListResult::getTotalPage()const
{
	return totalPage_;
}

std::vector<DescribeRoomKickoutUserListResult::User> DescribeRoomKickoutUserListResult::getUserList()const
{
	return userList_;
}


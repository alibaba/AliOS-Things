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

#include <alibabacloud/aliyuncvc/model/GetMembersResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

GetMembersResult::GetMembersResult() :
	ServiceResult()
{}

GetMembersResult::GetMembersResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

GetMembersResult::~GetMembersResult()
{}

void GetMembersResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto meetingInfoNode = value["MeetingInfo"];
	if(!meetingInfoNode["MeetingName"].isNull())
		meetingInfo_.meetingName = meetingInfoNode["MeetingName"].asString();
	if(!meetingInfoNode["ValidTime"].isNull())
		meetingInfo_.validTime = std::stol(meetingInfoNode["ValidTime"].asString());
	if(!meetingInfoNode["MeetingCode"].isNull())
		meetingInfo_.meetingCode = meetingInfoNode["MeetingCode"].asString();
	if(!meetingInfoNode["CreateTime"].isNull())
		meetingInfo_.createTime = std::stol(meetingInfoNode["CreateTime"].asString());
	if(!meetingInfoNode["UserId"].isNull())
		meetingInfo_.userId = meetingInfoNode["UserId"].asString();
	if(!meetingInfoNode["MeetingUUID"].isNull())
		meetingInfo_.meetingUUID = meetingInfoNode["MeetingUUID"].asString();
	auto allMemberListNode = meetingInfoNode["MemberList"]["MemberListItem"];
	for (auto meetingInfoNodeMemberListMemberListItem : allMemberListNode)
	{
		MeetingInfo::MemberListItem memberListItemObject;
		if(!meetingInfoNodeMemberListMemberListItem["UserAvatarUrl"].isNull())
			memberListItemObject.userAvatarUrl = meetingInfoNodeMemberListMemberListItem["UserAvatarUrl"].asString();
		if(!meetingInfoNodeMemberListMemberListItem["MemberUUID"].isNull())
			memberListItemObject.memberUUID = meetingInfoNodeMemberListMemberListItem["MemberUUID"].asString();
		if(!meetingInfoNodeMemberListMemberListItem["UserName"].isNull())
			memberListItemObject.userName = meetingInfoNodeMemberListMemberListItem["UserName"].asString();
		if(!meetingInfoNodeMemberListMemberListItem["UserId"].isNull())
			memberListItemObject.userId = meetingInfoNodeMemberListMemberListItem["UserId"].asString();
		if(!meetingInfoNodeMemberListMemberListItem["Status"].isNull())
			memberListItemObject.status = meetingInfoNodeMemberListMemberListItem["Status"].asString();
		meetingInfo_.memberList.push_back(memberListItemObject);
	}
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

GetMembersResult::MeetingInfo GetMembersResult::getMeetingInfo()const
{
	return meetingInfo_;
}

std::string GetMembersResult::getMessage()const
{
	return message_;
}

int GetMembersResult::getErrorCode()const
{
	return errorCode_;
}

bool GetMembersResult::getSuccess()const
{
	return success_;
}


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

#include <alibabacloud/aliyuncvc/model/ListMembersResult.h>
#include <json/json.h>

using namespace AlibabaCloud::Aliyuncvc;
using namespace AlibabaCloud::Aliyuncvc::Model;

ListMembersResult::ListMembersResult() :
	ServiceResult()
{}

ListMembersResult::ListMembersResult(const std::string &payload) :
	ServiceResult()
{
	parse(payload);
}

ListMembersResult::~ListMembersResult()
{}

void ListMembersResult::parse(const std::string &payload)
{
	Json::Reader reader;
	Json::Value value;
	reader.parse(payload, value);
	setRequestId(value["RequestId"].asString());
	auto meetingInfoNode = value["MeetingInfo"];
	if(!meetingInfoNode["MeetingName"].isNull())
		meetingInfo_.meetingName = meetingInfoNode["MeetingName"].asString();
	if(!meetingInfoNode["MeetingCode"].isNull())
		meetingInfo_.meetingCode = meetingInfoNode["MeetingCode"].asString();
	if(!meetingInfoNode["Memo"].isNull())
		meetingInfo_.memo = meetingInfoNode["Memo"].asString();
	if(!meetingInfoNode["UserName"].isNull())
		meetingInfo_.userName = meetingInfoNode["UserName"].asString();
	if(!meetingInfoNode["CreateTime"].isNull())
		meetingInfo_.createTime = std::stol(meetingInfoNode["CreateTime"].asString());
	if(!meetingInfoNode["UserId"].isNull())
		meetingInfo_.userId = meetingInfoNode["UserId"].asString();
	if(!meetingInfoNode["MeetingUUID"].isNull())
		meetingInfo_.meetingUUID = meetingInfoNode["MeetingUUID"].asString();
	auto allMemberInfosNode = meetingInfoNode["MemberInfos"]["MemberRecord"];
	for (auto meetingInfoNodeMemberInfosMemberRecord : allMemberInfosNode)
	{
		MeetingInfo::MemberRecord memberRecordObject;
		if(!meetingInfoNodeMemberInfosMemberRecord["MemberUUID"].isNull())
			memberRecordObject.memberUUID = meetingInfoNodeMemberInfosMemberRecord["MemberUUID"].asString();
		if(!meetingInfoNodeMemberInfosMemberRecord["UserId"].isNull())
			memberRecordObject.userId = meetingInfoNodeMemberInfosMemberRecord["UserId"].asString();
		if(!meetingInfoNodeMemberInfosMemberRecord["Status"].isNull())
			memberRecordObject.status = meetingInfoNodeMemberInfosMemberRecord["Status"].asString();
		if(!meetingInfoNodeMemberInfosMemberRecord["BeginTime"].isNull())
			memberRecordObject.beginTime = std::stol(meetingInfoNodeMemberInfosMemberRecord["BeginTime"].asString());
		if(!meetingInfoNodeMemberInfosMemberRecord["EndTime"].isNull())
			memberRecordObject.endTime = std::stol(meetingInfoNodeMemberInfosMemberRecord["EndTime"].asString());
		if(!meetingInfoNodeMemberInfosMemberRecord["UserName"].isNull())
			memberRecordObject.userName = meetingInfoNodeMemberInfosMemberRecord["UserName"].asString();
		meetingInfo_.memberInfos.push_back(memberRecordObject);
	}
	if(!value["ErrorCode"].isNull())
		errorCode_ = std::stoi(value["ErrorCode"].asString());
	if(!value["Message"].isNull())
		message_ = value["Message"].asString();
	if(!value["Success"].isNull())
		success_ = value["Success"].asString() == "true";

}

ListMembersResult::MeetingInfo ListMembersResult::getMeetingInfo()const
{
	return meetingInfo_;
}

std::string ListMembersResult::getMessage()const
{
	return message_;
}

int ListMembersResult::getErrorCode()const
{
	return errorCode_;
}

bool ListMembersResult::getSuccess()const
{
	return success_;
}


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

#include <alibabacloud/aliyuncvc/model/CreateEvaluationRequest.h>

using AlibabaCloud::Aliyuncvc::Model::CreateEvaluationRequest;

CreateEvaluationRequest::CreateEvaluationRequest() :
	RpcServiceRequest("aliyuncvc", "2019-10-30", "CreateEvaluation")
{
	setMethod(HttpRequest::Method::Post);
}

CreateEvaluationRequest::~CreateEvaluationRequest()
{}

long CreateEvaluationRequest::getCreateTime()const
{
	return createTime_;
}

void CreateEvaluationRequest::setCreateTime(long createTime)
{
	createTime_ = createTime;
	setParameter("CreateTime", std::to_string(createTime));
}

std::string CreateEvaluationRequest::getMemo()const
{
	return memo_;
}

void CreateEvaluationRequest::setMemo(const std::string& memo)
{
	memo_ = memo;
	setParameter("Memo", memo);
}

std::string CreateEvaluationRequest::getDescription()const
{
	return description_;
}

void CreateEvaluationRequest::setDescription(const std::string& description)
{
	description_ = description;
	setParameter("Description", description);
}

std::string CreateEvaluationRequest::getMemberUUID()const
{
	return memberUUID_;
}

void CreateEvaluationRequest::setMemberUUID(const std::string& memberUUID)
{
	memberUUID_ = memberUUID;
	setParameter("MemberUUID", memberUUID);
}

std::string CreateEvaluationRequest::getUserId()const
{
	return userId_;
}

void CreateEvaluationRequest::setUserId(const std::string& userId)
{
	userId_ = userId;
	setParameter("UserId", userId);
}

std::string CreateEvaluationRequest::getEvaluation()const
{
	return evaluation_;
}

void CreateEvaluationRequest::setEvaluation(const std::string& evaluation)
{
	evaluation_ = evaluation;
	setParameter("Evaluation", evaluation);
}

std::string CreateEvaluationRequest::getScore()const
{
	return score_;
}

void CreateEvaluationRequest::setScore(const std::string& score)
{
	score_ = score;
	setParameter("Score", score);
}

std::string CreateEvaluationRequest::getMeetingUUID()const
{
	return meetingUUID_;
}

void CreateEvaluationRequest::setMeetingUUID(const std::string& meetingUUID)
{
	meetingUUID_ = meetingUUID;
	setParameter("MeetingUUID", meetingUUID);
}

std::string CreateEvaluationRequest::getAppId()const
{
	return appId_;
}

void CreateEvaluationRequest::setAppId(const std::string& appId)
{
	appId_ = appId;
	setParameter("AppId", appId);
}


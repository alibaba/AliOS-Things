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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_CREATEEVALUATIONREQUEST_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_CREATEEVALUATIONREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/aliyuncvc/AliyuncvcExport.h>

namespace AlibabaCloud
{
	namespace Aliyuncvc
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIYUNCVC_EXPORT CreateEvaluationRequest : public RpcServiceRequest
			{

			public:
				CreateEvaluationRequest();
				~CreateEvaluationRequest();

				long getCreateTime()const;
				void setCreateTime(long createTime);
				std::string getMemo()const;
				void setMemo(const std::string& memo);
				std::string getDescription()const;
				void setDescription(const std::string& description);
				std::string getMemberUUID()const;
				void setMemberUUID(const std::string& memberUUID);
				std::string getUserId()const;
				void setUserId(const std::string& userId);
				std::string getEvaluation()const;
				void setEvaluation(const std::string& evaluation);
				std::string getScore()const;
				void setScore(const std::string& score);
				std::string getMeetingUUID()const;
				void setMeetingUUID(const std::string& meetingUUID);
				std::string getAppId()const;
				void setAppId(const std::string& appId);

            private:
				long createTime_;
				std::string memo_;
				std::string description_;
				std::string memberUUID_;
				std::string userId_;
				std::string evaluation_;
				std::string score_;
				std::string meetingUUID_;
				std::string appId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_CREATEEVALUATIONREQUEST_H_
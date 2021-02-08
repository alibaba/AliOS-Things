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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_LISTMEMBERSRESULT_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_LISTMEMBERSRESULT_H_

#include <string>
#include <vector>
#include <utility>
#include <alibabacloud/core/ServiceResult.h>
#include <alibabacloud/aliyuncvc/AliyuncvcExport.h>

namespace AlibabaCloud
{
	namespace Aliyuncvc
	{
		namespace Model
		{
			class ALIBABACLOUD_ALIYUNCVC_EXPORT ListMembersResult : public ServiceResult
			{
			public:
				struct MeetingInfo
				{
					struct MemberRecord
					{
						std::string status;
						std::string userName;
						std::string memberUUID;
						long endTime;
						std::string userId;
						long beginTime;
					};
					std::string meetingCode;
					std::string userName;
					std::vector<MemberRecord> memberInfos;
					std::string meetingUUID;
					std::string userId;
					long createTime;
					std::string meetingName;
					std::string memo;
				};


				ListMembersResult();
				explicit ListMembersResult(const std::string &payload);
				~ListMembersResult();
				MeetingInfo getMeetingInfo()const;
				std::string getMessage()const;
				int getErrorCode()const;
				bool getSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				MeetingInfo meetingInfo_;
				std::string message_;
				int errorCode_;
				bool success_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_LISTMEMBERSRESULT_H_
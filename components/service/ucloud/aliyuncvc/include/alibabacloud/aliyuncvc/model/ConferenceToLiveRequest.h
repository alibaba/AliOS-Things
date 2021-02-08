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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_CONFERENCETOLIVEREQUEST_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_CONFERENCETOLIVEREQUEST_H_

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
			class ALIBABACLOUD_ALIYUNCVC_EXPORT ConferenceToLiveRequest : public RpcServiceRequest
			{

			public:
				ConferenceToLiveRequest();
				~ConferenceToLiveRequest();

				std::string getUserId()const;
				void setUserId(const std::string& userId);
				bool getOpenPasswordFlag()const;
				void setOpenPasswordFlag(bool openPasswordFlag);
				std::string getMeetingUUID()const;
				void setMeetingUUID(const std::string& meetingUUID);
				std::string getPassword()const;
				void setPassword(const std::string& password);
				std::string getLiveName()const;
				void setLiveName(const std::string& liveName);

            private:
				std::string userId_;
				bool openPasswordFlag_;
				std::string meetingUUID_;
				std::string password_;
				std::string liveName_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_CONFERENCETOLIVEREQUEST_H_
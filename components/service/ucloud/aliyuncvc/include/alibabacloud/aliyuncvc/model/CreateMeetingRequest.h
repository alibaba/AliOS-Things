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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_CREATEMEETINGREQUEST_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_CREATEMEETINGREQUEST_H_

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
			class ALIBABACLOUD_ALIYUNCVC_EXPORT CreateMeetingRequest : public RpcServiceRequest
			{

			public:
				CreateMeetingRequest();
				~CreateMeetingRequest();

				std::string getMeetingName()const;
				void setMeetingName(const std::string& meetingName);
				std::string getUserId()const;
				void setUserId(const std::string& userId);
				bool getOpenPasswordFlag()const;
				void setOpenPasswordFlag(bool openPasswordFlag);
				std::string getPassword()const;
				void setPassword(const std::string& password);
				bool getMasterEnableFlag()const;
				void setMasterEnableFlag(bool masterEnableFlag);
				std::string getMeetingMode()const;
				void setMeetingMode(const std::string& meetingMode);

            private:
				std::string meetingName_;
				std::string userId_;
				bool openPasswordFlag_;
				std::string password_;
				bool masterEnableFlag_;
				std::string meetingMode_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_CREATEMEETINGREQUEST_H_
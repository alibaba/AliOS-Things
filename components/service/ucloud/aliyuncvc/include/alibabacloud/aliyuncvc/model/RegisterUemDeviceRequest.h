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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_REGISTERUEMDEVICEREQUEST_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_REGISTERUEMDEVICEREQUEST_H_

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
			class ALIBABACLOUD_ALIYUNCVC_EXPORT RegisterUemDeviceRequest : public RpcServiceRequest
			{

			public:
				RegisterUemDeviceRequest();
				~RegisterUemDeviceRequest();

				std::string getIP()const;
				void setIP(const std::string& iP);
				std::string getGroupId()const;
				void setGroupId(const std::string& groupId);
				std::string getOwnerId()const;
				void setOwnerId(const std::string& ownerId);
				std::string getDeviceId()const;
				void setDeviceId(const std::string& deviceId);
				std::string getGroupName()const;
				void setGroupName(const std::string& groupName);
				std::string getMac()const;
				void setMac(const std::string& mac);
				std::string getDeviceVersion()const;
				void setDeviceVersion(const std::string& deviceVersion);

            private:
				std::string iP_;
				std::string groupId_;
				std::string ownerId_;
				std::string deviceId_;
				std::string groupName_;
				std::string mac_;
				std::string deviceVersion_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_REGISTERUEMDEVICEREQUEST_H_
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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_REGISTERUEMDEVICERESULT_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_REGISTERUEMDEVICERESULT_H_

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
			class ALIBABACLOUD_ALIYUNCVC_EXPORT RegisterUemDeviceResult : public ServiceResult
			{
			public:
				struct DeviceInfo
				{
					struct MqttParam
					{
						std::string userName;
						std::string reconnectTimeout;
						std::string cleanSession;
						std::string port;
						std::string sDKClientPort;
						std::string clientId;
						std::string host;
						std::string useTLS;
						std::string topic;
						std::string groupId;
						std::string password;
						std::string tLSPort;
					};
					struct AuthWsChannelConfig
					{
						int wsOuterReconnTime;
						std::string token;
						std::string authWsUrl;
					};
					struct SlsConfig
					{
						std::string project;
						std::string logStore;
						std::string logServiceEndpoint;
					};
					std::string screenCode;
					long registerTime;
					SlsConfig slsConfig;
					std::string messageKey;
					std::string deviceSessionId;
					std::string token;
					MqttParam mqttParam;
					AuthWsChannelConfig authWsChannelConfig;
					std::string channelType;
					std::string deviceName;
				};


				RegisterUemDeviceResult();
				explicit RegisterUemDeviceResult(const std::string &payload);
				~RegisterUemDeviceResult();
				std::string getMessage()const;
				DeviceInfo getDeviceInfo()const;
				int getErrorCode()const;
				bool getSuccess()const;

			protected:
				void parse(const std::string &payload);
			private:
				std::string message_;
				DeviceInfo deviceInfo_;
				int errorCode_;
				bool success_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_REGISTERUEMDEVICERESULT_H_
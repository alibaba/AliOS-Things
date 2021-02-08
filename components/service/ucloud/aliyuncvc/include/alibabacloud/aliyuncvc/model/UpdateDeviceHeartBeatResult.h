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

#ifndef ALIBABACLOUD_ALIYUNCVC_MODEL_UPDATEDEVICEHEARTBEATRESULT_H_
#define ALIBABACLOUD_ALIYUNCVC_MODEL_UPDATEDEVICEHEARTBEATRESULT_H_

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
			class ALIBABACLOUD_ALIYUNCVC_EXPORT UpdateDeviceHeartBeatResult : public ServiceResult
			{
			public:
				struct DeviceInfo
				{
					std::string channelType;
				};


				UpdateDeviceHeartBeatResult();
				explicit UpdateDeviceHeartBeatResult(const std::string &payload);
				~UpdateDeviceHeartBeatResult();
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
#endif // !ALIBABACLOUD_ALIYUNCVC_MODEL_UPDATEDEVICEHEARTBEATRESULT_H_
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

#ifndef ALIBABACLOUD_METERING_MODEL_SYNCDATAREQUEST_H_
#define ALIBABACLOUD_METERING_MODEL_SYNCDATAREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RoaServiceRequest.h>
#include <alibabacloud/metering/MeteringExport.h>

namespace AlibabaCloud
{
	namespace Metering
	{
		namespace Model
		{
			class ALIBABACLOUD_METERING_EXPORT SyncDataRequest : public RoaServiceRequest
			{

			public:
				SyncDataRequest();
				~SyncDataRequest();

				std::string getInstanceId()const;
				void setInstanceId(const std::string& instanceId);
				std::string getData()const;
				void setData(const std::string& data);
				std::string getDataType()const;
				void setDataType(const std::string& dataType);
				std::string getEndTime()const;
				void setEndTime(const std::string& endTime);
				std::string getStartTime()const;
				void setStartTime(const std::string& startTime);
				std::string getCommodityCode()const;
				void setCommodityCode(const std::string& commodityCode);
				std::string getRegion()const;
				void setRegion(const std::string& region);
				std::string getServiceId()const;
				void setServiceId(const std::string& serviceId);
				std::string getUserId()const;
				void setUserId(const std::string& userId);

            private:
				std::string instanceId_;
				std::string data_;
				std::string dataType_;
				std::string endTime_;
				std::string startTime_;
				std::string commodityCode_;
				std::string region_;
				std::string serviceId_;
				std::string userId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_METERING_MODEL_SYNCDATAREQUEST_H_
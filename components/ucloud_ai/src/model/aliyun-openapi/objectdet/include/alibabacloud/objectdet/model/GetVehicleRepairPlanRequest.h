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

#ifndef ALIBABACLOUD_OBJECTDET_MODEL_GETVEHICLEREPAIRPLANREQUEST_H_
#define ALIBABACLOUD_OBJECTDET_MODEL_GETVEHICLEREPAIRPLANREQUEST_H_

#include <string>
#include <vector>
#include <alibabacloud/core/RpcServiceRequest.h>
#include <alibabacloud/objectdet/ObjectdetExport.h>

namespace AlibabaCloud
{
	namespace Objectdet
	{
		namespace Model
		{
			class ALIBABACLOUD_OBJECTDET_EXPORT GetVehicleRepairPlanRequest : public RpcServiceRequest
			{

			public:
				GetVehicleRepairPlanRequest();
				~GetVehicleRepairPlanRequest();

				std::string getVinCodeImage()const;
				void setVinCodeImage(const std::string& vinCodeImage);
				std::string getCarNumberImage()const;
				void setCarNumberImage(const std::string& carNumberImage);
				std::string getTaskId()const;
				void setTaskId(const std::string& taskId);

            private:
				std::string vinCodeImage_;
				std::string carNumberImage_;
				std::string taskId_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OBJECTDET_MODEL_GETVEHICLEREPAIRPLANREQUEST_H_
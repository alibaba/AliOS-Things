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

#ifndef ALIBABACLOUD_OBJECTDET_MODEL_GENERATEVEHICLEREPAIRPLANREQUEST_H_
#define ALIBABACLOUD_OBJECTDET_MODEL_GENERATEVEHICLEREPAIRPLANREQUEST_H_

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
			class ALIBABACLOUD_OBJECTDET_EXPORT GenerateVehicleRepairPlanRequest : public RpcServiceRequest
			{
			public:
				struct DamageImageList
				{
					std::string imageUrl;
					std::string createTimeStamp;
				};

			public:
				GenerateVehicleRepairPlanRequest();
				~GenerateVehicleRepairPlanRequest();

				std::vector<DamageImageList> getDamageImageList()const;
				void setDamageImageList(const std::vector<DamageImageList>& damageImageList);

            private:
				std::vector<DamageImageList> damageImageList_;

			};
		}
	}
}
#endif // !ALIBABACLOUD_OBJECTDET_MODEL_GENERATEVEHICLEREPAIRPLANREQUEST_H_
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

#include <alibabacloud/objectdet/model/GenerateVehicleRepairPlanRequest.h>

using AlibabaCloud::Objectdet::Model::GenerateVehicleRepairPlanRequest;

GenerateVehicleRepairPlanRequest::GenerateVehicleRepairPlanRequest() :
	RpcServiceRequest("objectdet", "2019-12-30", "GenerateVehicleRepairPlan")
{
	setMethod(HttpRequest::Method::Post);
}

GenerateVehicleRepairPlanRequest::~GenerateVehicleRepairPlanRequest()
{}

std::vector<GenerateVehicleRepairPlanRequest::DamageImageList> GenerateVehicleRepairPlanRequest::getDamageImageList()const
{
	return damageImageList_;
}

void GenerateVehicleRepairPlanRequest::setDamageImageList(const std::vector<DamageImageList>& damageImageList)
{
	damageImageList_ = damageImageList;
	for(int dep1 = 0; dep1!= damageImageList.size(); dep1++) {
		auto damageImageListObj = damageImageList.at(dep1);
		std::string damageImageListObjStr = "DamageImageList." + std::to_string(dep1 + 1);
		setParameter(damageImageListObjStr + ".ImageUrl", damageImageListObj.imageUrl);
		setParameter(damageImageListObjStr + ".CreateTimeStamp", damageImageListObj.createTimeStamp);
	}
}


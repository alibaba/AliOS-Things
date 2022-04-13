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

#include <alibabacloud/objectdet/model/GetVehicleRepairPlanRequest.h>

using AlibabaCloud::Objectdet::Model::GetVehicleRepairPlanRequest;

GetVehicleRepairPlanRequest::GetVehicleRepairPlanRequest() :
	RpcServiceRequest("objectdet", "2019-12-30", "GetVehicleRepairPlan")
{
	setMethod(HttpRequest::Method::Post);
}

GetVehicleRepairPlanRequest::~GetVehicleRepairPlanRequest()
{}

std::string GetVehicleRepairPlanRequest::getVinCodeImage()const
{
	return vinCodeImage_;
}

void GetVehicleRepairPlanRequest::setVinCodeImage(const std::string& vinCodeImage)
{
	vinCodeImage_ = vinCodeImage;
	setBodyParameter("VinCodeImage", vinCodeImage);
}

std::string GetVehicleRepairPlanRequest::getCarNumberImage()const
{
	return carNumberImage_;
}

void GetVehicleRepairPlanRequest::setCarNumberImage(const std::string& carNumberImage)
{
	carNumberImage_ = carNumberImage;
	setBodyParameter("CarNumberImage", carNumberImage);
}

std::string GetVehicleRepairPlanRequest::getTaskId()const
{
	return taskId_;
}

void GetVehicleRepairPlanRequest::setTaskId(const std::string& taskId)
{
	taskId_ = taskId;
	setBodyParameter("TaskId", taskId);
}


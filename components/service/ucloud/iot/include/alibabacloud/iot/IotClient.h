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

#ifndef ALIBABACLOUD_IOT_IOTCLIENT_H_
#define ALIBABACLOUD_IOT_IOTCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "IotExport.h"
#include "model/BatchAddDeviceGroupRelationsRequest.h"
#include "model/BatchAddDeviceGroupRelationsResult.h"
#include "model/BatchAddThingTopoRequest.h"
#include "model/BatchAddThingTopoResult.h"
#include "model/BatchBindDeviceToEdgeInstanceWithDriverRequest.h"
#include "model/BatchBindDeviceToEdgeInstanceWithDriverResult.h"
#include "model/BatchCheckDeviceNamesRequest.h"
#include "model/BatchCheckDeviceNamesResult.h"
#include "model/BatchClearEdgeInstanceDeviceConfigRequest.h"
#include "model/BatchClearEdgeInstanceDeviceConfigResult.h"
#include "model/BatchDeleteDeviceGroupRelationsRequest.h"
#include "model/BatchDeleteDeviceGroupRelationsResult.h"
#include "model/BatchDeleteEdgeInstanceChannelRequest.h"
#include "model/BatchDeleteEdgeInstanceChannelResult.h"
#include "model/BatchGetDeviceBindStatusRequest.h"
#include "model/BatchGetDeviceBindStatusResult.h"
#include "model/BatchGetDeviceStateRequest.h"
#include "model/BatchGetDeviceStateResult.h"
#include "model/BatchGetEdgeDriverRequest.h"
#include "model/BatchGetEdgeDriverResult.h"
#include "model/BatchGetEdgeInstanceChannelRequest.h"
#include "model/BatchGetEdgeInstanceChannelResult.h"
#include "model/BatchGetEdgeInstanceDeviceChannelRequest.h"
#include "model/BatchGetEdgeInstanceDeviceChannelResult.h"
#include "model/BatchGetEdgeInstanceDeviceConfigRequest.h"
#include "model/BatchGetEdgeInstanceDeviceConfigResult.h"
#include "model/BatchGetEdgeInstanceDeviceDriverRequest.h"
#include "model/BatchGetEdgeInstanceDeviceDriverResult.h"
#include "model/BatchGetEdgeInstanceDriverConfigsRequest.h"
#include "model/BatchGetEdgeInstanceDriverConfigsResult.h"
#include "model/BatchQueryDeviceDetailRequest.h"
#include "model/BatchQueryDeviceDetailResult.h"
#include "model/BatchRegisterDeviceRequest.h"
#include "model/BatchRegisterDeviceResult.h"
#include "model/BatchRegisterDeviceWithApplyIdRequest.h"
#include "model/BatchRegisterDeviceWithApplyIdResult.h"
#include "model/BatchSetEdgeInstanceDeviceChannelRequest.h"
#include "model/BatchSetEdgeInstanceDeviceChannelResult.h"
#include "model/BatchSetEdgeInstanceDeviceConfigRequest.h"
#include "model/BatchSetEdgeInstanceDeviceConfigResult.h"
#include "model/BatchUnbindDeviceFromEdgeInstanceRequest.h"
#include "model/BatchUnbindDeviceFromEdgeInstanceResult.h"
#include "model/BatchUpdateDeviceNicknameRequest.h"
#include "model/BatchUpdateDeviceNicknameResult.h"
#include "model/BindApplicationToEdgeInstanceRequest.h"
#include "model/BindApplicationToEdgeInstanceResult.h"
#include "model/BindDriverToEdgeInstanceRequest.h"
#include "model/BindDriverToEdgeInstanceResult.h"
#include "model/BindGatewayToEdgeInstanceRequest.h"
#include "model/BindGatewayToEdgeInstanceResult.h"
#include "model/BindSceneRuleToEdgeInstanceRequest.h"
#include "model/BindSceneRuleToEdgeInstanceResult.h"
#include "model/CancelOTAStrategyByJobRequest.h"
#include "model/CancelOTAStrategyByJobResult.h"
#include "model/CancelOTATaskByDeviceRequest.h"
#include "model/CancelOTATaskByDeviceResult.h"
#include "model/CancelOTATaskByJobRequest.h"
#include "model/CancelOTATaskByJobResult.h"
#include "model/ClearEdgeInstanceDriverConfigsRequest.h"
#include "model/ClearEdgeInstanceDriverConfigsResult.h"
#include "model/CloseEdgeInstanceDeploymentRequest.h"
#include "model/CloseEdgeInstanceDeploymentResult.h"
#include "model/CopyThingModelRequest.h"
#include "model/CopyThingModelResult.h"
#include "model/CreateConsumerGroupRequest.h"
#include "model/CreateConsumerGroupResult.h"
#include "model/CreateConsumerGroupSubscribeRelationRequest.h"
#include "model/CreateConsumerGroupSubscribeRelationResult.h"
#include "model/CreateDataAPIServiceRequest.h"
#include "model/CreateDataAPIServiceResult.h"
#include "model/CreateDeviceGroupRequest.h"
#include "model/CreateDeviceGroupResult.h"
#include "model/CreateEdgeDriverRequest.h"
#include "model/CreateEdgeDriverResult.h"
#include "model/CreateEdgeDriverVersionRequest.h"
#include "model/CreateEdgeDriverVersionResult.h"
#include "model/CreateEdgeInstanceRequest.h"
#include "model/CreateEdgeInstanceResult.h"
#include "model/CreateEdgeInstanceChannelRequest.h"
#include "model/CreateEdgeInstanceChannelResult.h"
#include "model/CreateEdgeInstanceDeploymentRequest.h"
#include "model/CreateEdgeInstanceDeploymentResult.h"
#include "model/CreateEdgeOssPreSignedAddressRequest.h"
#include "model/CreateEdgeOssPreSignedAddressResult.h"
#include "model/CreateLoRaNodesTaskRequest.h"
#include "model/CreateLoRaNodesTaskResult.h"
#include "model/CreateOTADynamicUpgradeJobRequest.h"
#include "model/CreateOTADynamicUpgradeJobResult.h"
#include "model/CreateOTAFirmwareRequest.h"
#include "model/CreateOTAFirmwareResult.h"
#include "model/CreateOTAStaticUpgradeJobRequest.h"
#include "model/CreateOTAStaticUpgradeJobResult.h"
#include "model/CreateOTAVerifyJobRequest.h"
#include "model/CreateOTAVerifyJobResult.h"
#include "model/CreateProductRequest.h"
#include "model/CreateProductResult.h"
#include "model/CreateProductTagsRequest.h"
#include "model/CreateProductTagsResult.h"
#include "model/CreateProductTopicRequest.h"
#include "model/CreateProductTopicResult.h"
#include "model/CreateRuleRequest.h"
#include "model/CreateRuleResult.h"
#include "model/CreateRuleActionRequest.h"
#include "model/CreateRuleActionResult.h"
#include "model/CreateSceneRuleRequest.h"
#include "model/CreateSceneRuleResult.h"
#include "model/CreateSubscribeRelationRequest.h"
#include "model/CreateSubscribeRelationResult.h"
#include "model/CreateThingModelRequest.h"
#include "model/CreateThingModelResult.h"
#include "model/CreateTopicRouteTableRequest.h"
#include "model/CreateTopicRouteTableResult.h"
#include "model/DeleteConsumerGroupRequest.h"
#include "model/DeleteConsumerGroupResult.h"
#include "model/DeleteConsumerGroupSubscribeRelationRequest.h"
#include "model/DeleteConsumerGroupSubscribeRelationResult.h"
#include "model/DeleteDeviceRequest.h"
#include "model/DeleteDeviceResult.h"
#include "model/DeleteDeviceFileRequest.h"
#include "model/DeleteDeviceFileResult.h"
#include "model/DeleteDeviceGroupRequest.h"
#include "model/DeleteDeviceGroupResult.h"
#include "model/DeleteDevicePropRequest.h"
#include "model/DeleteDevicePropResult.h"
#include "model/DeleteEdgeDriverRequest.h"
#include "model/DeleteEdgeDriverResult.h"
#include "model/DeleteEdgeDriverVersionRequest.h"
#include "model/DeleteEdgeDriverVersionResult.h"
#include "model/DeleteEdgeInstanceRequest.h"
#include "model/DeleteEdgeInstanceResult.h"
#include "model/DeleteOTAFirmwareRequest.h"
#include "model/DeleteOTAFirmwareResult.h"
#include "model/DeleteProductRequest.h"
#include "model/DeleteProductResult.h"
#include "model/DeleteProductTagsRequest.h"
#include "model/DeleteProductTagsResult.h"
#include "model/DeleteProductTopicRequest.h"
#include "model/DeleteProductTopicResult.h"
#include "model/DeleteRuleRequest.h"
#include "model/DeleteRuleResult.h"
#include "model/DeleteRuleActionRequest.h"
#include "model/DeleteRuleActionResult.h"
#include "model/DeleteSceneRuleRequest.h"
#include "model/DeleteSceneRuleResult.h"
#include "model/DeleteSubscribeRelationRequest.h"
#include "model/DeleteSubscribeRelationResult.h"
#include "model/DeleteThingModelRequest.h"
#include "model/DeleteThingModelResult.h"
#include "model/DeleteTopicRouteTableRequest.h"
#include "model/DeleteTopicRouteTableResult.h"
#include "model/DisableSceneRuleRequest.h"
#include "model/DisableSceneRuleResult.h"
#include "model/DisableThingRequest.h"
#include "model/DisableThingResult.h"
#include "model/EnableSceneRuleRequest.h"
#include "model/EnableSceneRuleResult.h"
#include "model/EnableThingRequest.h"
#include "model/EnableThingResult.h"
#include "model/GenerateOTAUploadURLRequest.h"
#include "model/GenerateOTAUploadURLResult.h"
#include "model/GetDataAPIServiceDetailRequest.h"
#include "model/GetDataAPIServiceDetailResult.h"
#include "model/GetDeviceShadowRequest.h"
#include "model/GetDeviceShadowResult.h"
#include "model/GetDeviceStatusRequest.h"
#include "model/GetDeviceStatusResult.h"
#include "model/GetEdgeDriverVersionRequest.h"
#include "model/GetEdgeDriverVersionResult.h"
#include "model/GetEdgeInstanceRequest.h"
#include "model/GetEdgeInstanceResult.h"
#include "model/GetEdgeInstanceDeploymentRequest.h"
#include "model/GetEdgeInstanceDeploymentResult.h"
#include "model/GetGatewayBySubDeviceRequest.h"
#include "model/GetGatewayBySubDeviceResult.h"
#include "model/GetLoraNodesTaskRequest.h"
#include "model/GetLoraNodesTaskResult.h"
#include "model/GetNodesAddingTaskRequest.h"
#include "model/GetNodesAddingTaskResult.h"
#include "model/GetRuleRequest.h"
#include "model/GetRuleResult.h"
#include "model/GetRuleActionRequest.h"
#include "model/GetRuleActionResult.h"
#include "model/GetSceneRuleRequest.h"
#include "model/GetSceneRuleResult.h"
#include "model/GetThingModelTslRequest.h"
#include "model/GetThingModelTslResult.h"
#include "model/GetThingTemplateRequest.h"
#include "model/GetThingTemplateResult.h"
#include "model/GetThingTopoRequest.h"
#include "model/GetThingTopoResult.h"
#include "model/ImportThingModelTslRequest.h"
#include "model/ImportThingModelTslResult.h"
#include "model/InvokeDataAPIServiceRequest.h"
#include "model/InvokeDataAPIServiceResult.h"
#include "model/InvokeThingServiceRequest.h"
#include "model/InvokeThingServiceResult.h"
#include "model/InvokeThingsServiceRequest.h"
#include "model/InvokeThingsServiceResult.h"
#include "model/ListOTAFirmwareRequest.h"
#include "model/ListOTAFirmwareResult.h"
#include "model/ListOTAJobByDeviceRequest.h"
#include "model/ListOTAJobByDeviceResult.h"
#include "model/ListOTAJobByFirmwareRequest.h"
#include "model/ListOTAJobByFirmwareResult.h"
#include "model/ListOTATaskByJobRequest.h"
#include "model/ListOTATaskByJobResult.h"
#include "model/ListProductByTagsRequest.h"
#include "model/ListProductByTagsResult.h"
#include "model/ListProductTagsRequest.h"
#include "model/ListProductTagsResult.h"
#include "model/ListRuleRequest.h"
#include "model/ListRuleResult.h"
#include "model/ListRuleActionsRequest.h"
#include "model/ListRuleActionsResult.h"
#include "model/ListThingModelVersionRequest.h"
#include "model/ListThingModelVersionResult.h"
#include "model/ListThingTemplatesRequest.h"
#include "model/ListThingTemplatesResult.h"
#include "model/NotifyAddThingTopoRequest.h"
#include "model/NotifyAddThingTopoResult.h"
#include "model/PubRequest.h"
#include "model/PubResult.h"
#include "model/PubBroadcastRequest.h"
#include "model/PubBroadcastResult.h"
#include "model/PublishThingModelRequest.h"
#include "model/PublishThingModelResult.h"
#include "model/QueryAppDeviceListRequest.h"
#include "model/QueryAppDeviceListResult.h"
#include "model/QueryBatchRegisterDeviceStatusRequest.h"
#include "model/QueryBatchRegisterDeviceStatusResult.h"
#include "model/QueryCertUrlByApplyIdRequest.h"
#include "model/QueryCertUrlByApplyIdResult.h"
#include "model/QueryConsumerGroupByGroupIdRequest.h"
#include "model/QueryConsumerGroupByGroupIdResult.h"
#include "model/QueryConsumerGroupListRequest.h"
#include "model/QueryConsumerGroupListResult.h"
#include "model/QueryConsumerGroupStatusRequest.h"
#include "model/QueryConsumerGroupStatusResult.h"
#include "model/QueryDetailSceneRuleLogRequest.h"
#include "model/QueryDetailSceneRuleLogResult.h"
#include "model/QueryDeviceRequest.h"
#include "model/QueryDeviceResult.h"
#include "model/QueryDeviceByStatusRequest.h"
#include "model/QueryDeviceByStatusResult.h"
#include "model/QueryDeviceByTagsRequest.h"
#include "model/QueryDeviceByTagsResult.h"
#include "model/QueryDeviceCertRequest.h"
#include "model/QueryDeviceCertResult.h"
#include "model/QueryDeviceDesiredPropertyRequest.h"
#include "model/QueryDeviceDesiredPropertyResult.h"
#include "model/QueryDeviceDetailRequest.h"
#include "model/QueryDeviceDetailResult.h"
#include "model/QueryDeviceEventDataRequest.h"
#include "model/QueryDeviceEventDataResult.h"
#include "model/QueryDeviceFileRequest.h"
#include "model/QueryDeviceFileResult.h"
#include "model/QueryDeviceFileListRequest.h"
#include "model/QueryDeviceFileListResult.h"
#include "model/QueryDeviceGroupByDeviceRequest.h"
#include "model/QueryDeviceGroupByDeviceResult.h"
#include "model/QueryDeviceGroupByTagsRequest.h"
#include "model/QueryDeviceGroupByTagsResult.h"
#include "model/QueryDeviceGroupInfoRequest.h"
#include "model/QueryDeviceGroupInfoResult.h"
#include "model/QueryDeviceGroupListRequest.h"
#include "model/QueryDeviceGroupListResult.h"
#include "model/QueryDeviceGroupTagListRequest.h"
#include "model/QueryDeviceGroupTagListResult.h"
#include "model/QueryDeviceListByDeviceGroupRequest.h"
#include "model/QueryDeviceListByDeviceGroupResult.h"
#include "model/QueryDevicePropRequest.h"
#include "model/QueryDevicePropResult.h"
#include "model/QueryDevicePropertiesDataRequest.h"
#include "model/QueryDevicePropertiesDataResult.h"
#include "model/QueryDevicePropertyDataRequest.h"
#include "model/QueryDevicePropertyDataResult.h"
#include "model/QueryDevicePropertyStatusRequest.h"
#include "model/QueryDevicePropertyStatusResult.h"
#include "model/QueryDeviceServiceDataRequest.h"
#include "model/QueryDeviceServiceDataResult.h"
#include "model/QueryDeviceStatisticsRequest.h"
#include "model/QueryDeviceStatisticsResult.h"
#include "model/QueryEdgeDriverRequest.h"
#include "model/QueryEdgeDriverResult.h"
#include "model/QueryEdgeDriverVersionRequest.h"
#include "model/QueryEdgeDriverVersionResult.h"
#include "model/QueryEdgeInstanceRequest.h"
#include "model/QueryEdgeInstanceResult.h"
#include "model/QueryEdgeInstanceChannelRequest.h"
#include "model/QueryEdgeInstanceChannelResult.h"
#include "model/QueryEdgeInstanceDeviceRequest.h"
#include "model/QueryEdgeInstanceDeviceResult.h"
#include "model/QueryEdgeInstanceDeviceByDriverRequest.h"
#include "model/QueryEdgeInstanceDeviceByDriverResult.h"
#include "model/QueryEdgeInstanceDriverRequest.h"
#include "model/QueryEdgeInstanceDriverResult.h"
#include "model/QueryEdgeInstanceGatewayRequest.h"
#include "model/QueryEdgeInstanceGatewayResult.h"
#include "model/QueryEdgeInstanceHistoricDeploymentRequest.h"
#include "model/QueryEdgeInstanceHistoricDeploymentResult.h"
#include "model/QueryEdgeInstanceSceneRuleRequest.h"
#include "model/QueryEdgeInstanceSceneRuleResult.h"
#include "model/QueryLoRaJoinPermissionsRequest.h"
#include "model/QueryLoRaJoinPermissionsResult.h"
#include "model/QueryOTAFirmwareRequest.h"
#include "model/QueryOTAFirmwareResult.h"
#include "model/QueryOTAJobRequest.h"
#include "model/QueryOTAJobResult.h"
#include "model/QueryPageByApplyIdRequest.h"
#include "model/QueryPageByApplyIdResult.h"
#include "model/QueryProductRequest.h"
#include "model/QueryProductResult.h"
#include "model/QueryProductCertInfoRequest.h"
#include "model/QueryProductCertInfoResult.h"
#include "model/QueryProductListRequest.h"
#include "model/QueryProductListResult.h"
#include "model/QueryProductTopicRequest.h"
#include "model/QueryProductTopicResult.h"
#include "model/QuerySceneRuleRequest.h"
#include "model/QuerySceneRuleResult.h"
#include "model/QuerySubscribeRelationRequest.h"
#include "model/QuerySubscribeRelationResult.h"
#include "model/QuerySummarySceneRuleLogRequest.h"
#include "model/QuerySummarySceneRuleLogResult.h"
#include "model/QuerySuperDeviceGroupRequest.h"
#include "model/QuerySuperDeviceGroupResult.h"
#include "model/QueryThingModelRequest.h"
#include "model/QueryThingModelResult.h"
#include "model/QueryTopicReverseRouteTableRequest.h"
#include "model/QueryTopicReverseRouteTableResult.h"
#include "model/QueryTopicRouteTableRequest.h"
#include "model/QueryTopicRouteTableResult.h"
#include "model/RRpcRequest.h"
#include "model/RRpcResult.h"
#include "model/RegisterDeviceRequest.h"
#include "model/RegisterDeviceResult.h"
#include "model/ReleaseEdgeDriverVersionRequest.h"
#include "model/ReleaseEdgeDriverVersionResult.h"
#include "model/RemoveThingTopoRequest.h"
#include "model/RemoveThingTopoResult.h"
#include "model/ReplaceEdgeInstanceGatewayRequest.h"
#include "model/ReplaceEdgeInstanceGatewayResult.h"
#include "model/ResetConsumerGroupPositionRequest.h"
#include "model/ResetConsumerGroupPositionResult.h"
#include "model/ResetThingRequest.h"
#include "model/ResetThingResult.h"
#include "model/SaveDevicePropRequest.h"
#include "model/SaveDevicePropResult.h"
#include "model/SetDeviceDesiredPropertyRequest.h"
#include "model/SetDeviceDesiredPropertyResult.h"
#include "model/SetDeviceGroupTagsRequest.h"
#include "model/SetDeviceGroupTagsResult.h"
#include "model/SetDevicePropertyRequest.h"
#include "model/SetDevicePropertyResult.h"
#include "model/SetDevicesPropertyRequest.h"
#include "model/SetDevicesPropertyResult.h"
#include "model/SetEdgeInstanceDriverConfigsRequest.h"
#include "model/SetEdgeInstanceDriverConfigsResult.h"
#include "model/SetProductCertInfoRequest.h"
#include "model/SetProductCertInfoResult.h"
#include "model/StartRuleRequest.h"
#include "model/StartRuleResult.h"
#include "model/StopRuleRequest.h"
#include "model/StopRuleResult.h"
#include "model/TriggerSceneRuleRequest.h"
#include "model/TriggerSceneRuleResult.h"
#include "model/UnbindApplicationFromEdgeInstanceRequest.h"
#include "model/UnbindApplicationFromEdgeInstanceResult.h"
#include "model/UnbindDriverFromEdgeInstanceRequest.h"
#include "model/UnbindDriverFromEdgeInstanceResult.h"
#include "model/UnbindSceneRuleFromEdgeInstanceRequest.h"
#include "model/UnbindSceneRuleFromEdgeInstanceResult.h"
#include "model/UpdateConsumerGroupRequest.h"
#include "model/UpdateConsumerGroupResult.h"
#include "model/UpdateDeviceGroupRequest.h"
#include "model/UpdateDeviceGroupResult.h"
#include "model/UpdateDeviceShadowRequest.h"
#include "model/UpdateDeviceShadowResult.h"
#include "model/UpdateEdgeDriverVersionRequest.h"
#include "model/UpdateEdgeDriverVersionResult.h"
#include "model/UpdateEdgeInstanceRequest.h"
#include "model/UpdateEdgeInstanceResult.h"
#include "model/UpdateEdgeInstanceChannelRequest.h"
#include "model/UpdateEdgeInstanceChannelResult.h"
#include "model/UpdateProductRequest.h"
#include "model/UpdateProductResult.h"
#include "model/UpdateProductFilterConfigRequest.h"
#include "model/UpdateProductFilterConfigResult.h"
#include "model/UpdateProductTagsRequest.h"
#include "model/UpdateProductTagsResult.h"
#include "model/UpdateProductTopicRequest.h"
#include "model/UpdateProductTopicResult.h"
#include "model/UpdateRuleRequest.h"
#include "model/UpdateRuleResult.h"
#include "model/UpdateRuleActionRequest.h"
#include "model/UpdateRuleActionResult.h"
#include "model/UpdateSceneRuleRequest.h"
#include "model/UpdateSceneRuleResult.h"
#include "model/UpdateSubscribeRelationRequest.h"
#include "model/UpdateSubscribeRelationResult.h"
#include "model/UpdateThingModelRequest.h"
#include "model/UpdateThingModelResult.h"


namespace AlibabaCloud
{
	namespace Iot
	{
		class ALIBABACLOUD_IOT_EXPORT IotClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::BatchAddDeviceGroupRelationsResult> BatchAddDeviceGroupRelationsOutcome;
			typedef std::future<BatchAddDeviceGroupRelationsOutcome> BatchAddDeviceGroupRelationsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchAddDeviceGroupRelationsRequest&, const BatchAddDeviceGroupRelationsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchAddDeviceGroupRelationsAsyncHandler;
			typedef Outcome<Error, Model::BatchAddThingTopoResult> BatchAddThingTopoOutcome;
			typedef std::future<BatchAddThingTopoOutcome> BatchAddThingTopoOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchAddThingTopoRequest&, const BatchAddThingTopoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchAddThingTopoAsyncHandler;
			typedef Outcome<Error, Model::BatchBindDeviceToEdgeInstanceWithDriverResult> BatchBindDeviceToEdgeInstanceWithDriverOutcome;
			typedef std::future<BatchBindDeviceToEdgeInstanceWithDriverOutcome> BatchBindDeviceToEdgeInstanceWithDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchBindDeviceToEdgeInstanceWithDriverRequest&, const BatchBindDeviceToEdgeInstanceWithDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchBindDeviceToEdgeInstanceWithDriverAsyncHandler;
			typedef Outcome<Error, Model::BatchCheckDeviceNamesResult> BatchCheckDeviceNamesOutcome;
			typedef std::future<BatchCheckDeviceNamesOutcome> BatchCheckDeviceNamesOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchCheckDeviceNamesRequest&, const BatchCheckDeviceNamesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchCheckDeviceNamesAsyncHandler;
			typedef Outcome<Error, Model::BatchClearEdgeInstanceDeviceConfigResult> BatchClearEdgeInstanceDeviceConfigOutcome;
			typedef std::future<BatchClearEdgeInstanceDeviceConfigOutcome> BatchClearEdgeInstanceDeviceConfigOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchClearEdgeInstanceDeviceConfigRequest&, const BatchClearEdgeInstanceDeviceConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchClearEdgeInstanceDeviceConfigAsyncHandler;
			typedef Outcome<Error, Model::BatchDeleteDeviceGroupRelationsResult> BatchDeleteDeviceGroupRelationsOutcome;
			typedef std::future<BatchDeleteDeviceGroupRelationsOutcome> BatchDeleteDeviceGroupRelationsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchDeleteDeviceGroupRelationsRequest&, const BatchDeleteDeviceGroupRelationsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchDeleteDeviceGroupRelationsAsyncHandler;
			typedef Outcome<Error, Model::BatchDeleteEdgeInstanceChannelResult> BatchDeleteEdgeInstanceChannelOutcome;
			typedef std::future<BatchDeleteEdgeInstanceChannelOutcome> BatchDeleteEdgeInstanceChannelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchDeleteEdgeInstanceChannelRequest&, const BatchDeleteEdgeInstanceChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchDeleteEdgeInstanceChannelAsyncHandler;
			typedef Outcome<Error, Model::BatchGetDeviceBindStatusResult> BatchGetDeviceBindStatusOutcome;
			typedef std::future<BatchGetDeviceBindStatusOutcome> BatchGetDeviceBindStatusOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetDeviceBindStatusRequest&, const BatchGetDeviceBindStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetDeviceBindStatusAsyncHandler;
			typedef Outcome<Error, Model::BatchGetDeviceStateResult> BatchGetDeviceStateOutcome;
			typedef std::future<BatchGetDeviceStateOutcome> BatchGetDeviceStateOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetDeviceStateRequest&, const BatchGetDeviceStateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetDeviceStateAsyncHandler;
			typedef Outcome<Error, Model::BatchGetEdgeDriverResult> BatchGetEdgeDriverOutcome;
			typedef std::future<BatchGetEdgeDriverOutcome> BatchGetEdgeDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetEdgeDriverRequest&, const BatchGetEdgeDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetEdgeDriverAsyncHandler;
			typedef Outcome<Error, Model::BatchGetEdgeInstanceChannelResult> BatchGetEdgeInstanceChannelOutcome;
			typedef std::future<BatchGetEdgeInstanceChannelOutcome> BatchGetEdgeInstanceChannelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetEdgeInstanceChannelRequest&, const BatchGetEdgeInstanceChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetEdgeInstanceChannelAsyncHandler;
			typedef Outcome<Error, Model::BatchGetEdgeInstanceDeviceChannelResult> BatchGetEdgeInstanceDeviceChannelOutcome;
			typedef std::future<BatchGetEdgeInstanceDeviceChannelOutcome> BatchGetEdgeInstanceDeviceChannelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetEdgeInstanceDeviceChannelRequest&, const BatchGetEdgeInstanceDeviceChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetEdgeInstanceDeviceChannelAsyncHandler;
			typedef Outcome<Error, Model::BatchGetEdgeInstanceDeviceConfigResult> BatchGetEdgeInstanceDeviceConfigOutcome;
			typedef std::future<BatchGetEdgeInstanceDeviceConfigOutcome> BatchGetEdgeInstanceDeviceConfigOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetEdgeInstanceDeviceConfigRequest&, const BatchGetEdgeInstanceDeviceConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetEdgeInstanceDeviceConfigAsyncHandler;
			typedef Outcome<Error, Model::BatchGetEdgeInstanceDeviceDriverResult> BatchGetEdgeInstanceDeviceDriverOutcome;
			typedef std::future<BatchGetEdgeInstanceDeviceDriverOutcome> BatchGetEdgeInstanceDeviceDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetEdgeInstanceDeviceDriverRequest&, const BatchGetEdgeInstanceDeviceDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetEdgeInstanceDeviceDriverAsyncHandler;
			typedef Outcome<Error, Model::BatchGetEdgeInstanceDriverConfigsResult> BatchGetEdgeInstanceDriverConfigsOutcome;
			typedef std::future<BatchGetEdgeInstanceDriverConfigsOutcome> BatchGetEdgeInstanceDriverConfigsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchGetEdgeInstanceDriverConfigsRequest&, const BatchGetEdgeInstanceDriverConfigsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchGetEdgeInstanceDriverConfigsAsyncHandler;
			typedef Outcome<Error, Model::BatchQueryDeviceDetailResult> BatchQueryDeviceDetailOutcome;
			typedef std::future<BatchQueryDeviceDetailOutcome> BatchQueryDeviceDetailOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchQueryDeviceDetailRequest&, const BatchQueryDeviceDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchQueryDeviceDetailAsyncHandler;
			typedef Outcome<Error, Model::BatchRegisterDeviceResult> BatchRegisterDeviceOutcome;
			typedef std::future<BatchRegisterDeviceOutcome> BatchRegisterDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchRegisterDeviceRequest&, const BatchRegisterDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchRegisterDeviceAsyncHandler;
			typedef Outcome<Error, Model::BatchRegisterDeviceWithApplyIdResult> BatchRegisterDeviceWithApplyIdOutcome;
			typedef std::future<BatchRegisterDeviceWithApplyIdOutcome> BatchRegisterDeviceWithApplyIdOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchRegisterDeviceWithApplyIdRequest&, const BatchRegisterDeviceWithApplyIdOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchRegisterDeviceWithApplyIdAsyncHandler;
			typedef Outcome<Error, Model::BatchSetEdgeInstanceDeviceChannelResult> BatchSetEdgeInstanceDeviceChannelOutcome;
			typedef std::future<BatchSetEdgeInstanceDeviceChannelOutcome> BatchSetEdgeInstanceDeviceChannelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchSetEdgeInstanceDeviceChannelRequest&, const BatchSetEdgeInstanceDeviceChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchSetEdgeInstanceDeviceChannelAsyncHandler;
			typedef Outcome<Error, Model::BatchSetEdgeInstanceDeviceConfigResult> BatchSetEdgeInstanceDeviceConfigOutcome;
			typedef std::future<BatchSetEdgeInstanceDeviceConfigOutcome> BatchSetEdgeInstanceDeviceConfigOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchSetEdgeInstanceDeviceConfigRequest&, const BatchSetEdgeInstanceDeviceConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchSetEdgeInstanceDeviceConfigAsyncHandler;
			typedef Outcome<Error, Model::BatchUnbindDeviceFromEdgeInstanceResult> BatchUnbindDeviceFromEdgeInstanceOutcome;
			typedef std::future<BatchUnbindDeviceFromEdgeInstanceOutcome> BatchUnbindDeviceFromEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchUnbindDeviceFromEdgeInstanceRequest&, const BatchUnbindDeviceFromEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchUnbindDeviceFromEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::BatchUpdateDeviceNicknameResult> BatchUpdateDeviceNicknameOutcome;
			typedef std::future<BatchUpdateDeviceNicknameOutcome> BatchUpdateDeviceNicknameOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BatchUpdateDeviceNicknameRequest&, const BatchUpdateDeviceNicknameOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchUpdateDeviceNicknameAsyncHandler;
			typedef Outcome<Error, Model::BindApplicationToEdgeInstanceResult> BindApplicationToEdgeInstanceOutcome;
			typedef std::future<BindApplicationToEdgeInstanceOutcome> BindApplicationToEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BindApplicationToEdgeInstanceRequest&, const BindApplicationToEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BindApplicationToEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::BindDriverToEdgeInstanceResult> BindDriverToEdgeInstanceOutcome;
			typedef std::future<BindDriverToEdgeInstanceOutcome> BindDriverToEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BindDriverToEdgeInstanceRequest&, const BindDriverToEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BindDriverToEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::BindGatewayToEdgeInstanceResult> BindGatewayToEdgeInstanceOutcome;
			typedef std::future<BindGatewayToEdgeInstanceOutcome> BindGatewayToEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BindGatewayToEdgeInstanceRequest&, const BindGatewayToEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BindGatewayToEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::BindSceneRuleToEdgeInstanceResult> BindSceneRuleToEdgeInstanceOutcome;
			typedef std::future<BindSceneRuleToEdgeInstanceOutcome> BindSceneRuleToEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::BindSceneRuleToEdgeInstanceRequest&, const BindSceneRuleToEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BindSceneRuleToEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::CancelOTAStrategyByJobResult> CancelOTAStrategyByJobOutcome;
			typedef std::future<CancelOTAStrategyByJobOutcome> CancelOTAStrategyByJobOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CancelOTAStrategyByJobRequest&, const CancelOTAStrategyByJobOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CancelOTAStrategyByJobAsyncHandler;
			typedef Outcome<Error, Model::CancelOTATaskByDeviceResult> CancelOTATaskByDeviceOutcome;
			typedef std::future<CancelOTATaskByDeviceOutcome> CancelOTATaskByDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CancelOTATaskByDeviceRequest&, const CancelOTATaskByDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CancelOTATaskByDeviceAsyncHandler;
			typedef Outcome<Error, Model::CancelOTATaskByJobResult> CancelOTATaskByJobOutcome;
			typedef std::future<CancelOTATaskByJobOutcome> CancelOTATaskByJobOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CancelOTATaskByJobRequest&, const CancelOTATaskByJobOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CancelOTATaskByJobAsyncHandler;
			typedef Outcome<Error, Model::ClearEdgeInstanceDriverConfigsResult> ClearEdgeInstanceDriverConfigsOutcome;
			typedef std::future<ClearEdgeInstanceDriverConfigsOutcome> ClearEdgeInstanceDriverConfigsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ClearEdgeInstanceDriverConfigsRequest&, const ClearEdgeInstanceDriverConfigsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ClearEdgeInstanceDriverConfigsAsyncHandler;
			typedef Outcome<Error, Model::CloseEdgeInstanceDeploymentResult> CloseEdgeInstanceDeploymentOutcome;
			typedef std::future<CloseEdgeInstanceDeploymentOutcome> CloseEdgeInstanceDeploymentOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CloseEdgeInstanceDeploymentRequest&, const CloseEdgeInstanceDeploymentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CloseEdgeInstanceDeploymentAsyncHandler;
			typedef Outcome<Error, Model::CopyThingModelResult> CopyThingModelOutcome;
			typedef std::future<CopyThingModelOutcome> CopyThingModelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CopyThingModelRequest&, const CopyThingModelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CopyThingModelAsyncHandler;
			typedef Outcome<Error, Model::CreateConsumerGroupResult> CreateConsumerGroupOutcome;
			typedef std::future<CreateConsumerGroupOutcome> CreateConsumerGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateConsumerGroupRequest&, const CreateConsumerGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateConsumerGroupAsyncHandler;
			typedef Outcome<Error, Model::CreateConsumerGroupSubscribeRelationResult> CreateConsumerGroupSubscribeRelationOutcome;
			typedef std::future<CreateConsumerGroupSubscribeRelationOutcome> CreateConsumerGroupSubscribeRelationOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateConsumerGroupSubscribeRelationRequest&, const CreateConsumerGroupSubscribeRelationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateConsumerGroupSubscribeRelationAsyncHandler;
			typedef Outcome<Error, Model::CreateDataAPIServiceResult> CreateDataAPIServiceOutcome;
			typedef std::future<CreateDataAPIServiceOutcome> CreateDataAPIServiceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateDataAPIServiceRequest&, const CreateDataAPIServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateDataAPIServiceAsyncHandler;
			typedef Outcome<Error, Model::CreateDeviceGroupResult> CreateDeviceGroupOutcome;
			typedef std::future<CreateDeviceGroupOutcome> CreateDeviceGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateDeviceGroupRequest&, const CreateDeviceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateDeviceGroupAsyncHandler;
			typedef Outcome<Error, Model::CreateEdgeDriverResult> CreateEdgeDriverOutcome;
			typedef std::future<CreateEdgeDriverOutcome> CreateEdgeDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateEdgeDriverRequest&, const CreateEdgeDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEdgeDriverAsyncHandler;
			typedef Outcome<Error, Model::CreateEdgeDriverVersionResult> CreateEdgeDriverVersionOutcome;
			typedef std::future<CreateEdgeDriverVersionOutcome> CreateEdgeDriverVersionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateEdgeDriverVersionRequest&, const CreateEdgeDriverVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEdgeDriverVersionAsyncHandler;
			typedef Outcome<Error, Model::CreateEdgeInstanceResult> CreateEdgeInstanceOutcome;
			typedef std::future<CreateEdgeInstanceOutcome> CreateEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateEdgeInstanceRequest&, const CreateEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::CreateEdgeInstanceChannelResult> CreateEdgeInstanceChannelOutcome;
			typedef std::future<CreateEdgeInstanceChannelOutcome> CreateEdgeInstanceChannelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateEdgeInstanceChannelRequest&, const CreateEdgeInstanceChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEdgeInstanceChannelAsyncHandler;
			typedef Outcome<Error, Model::CreateEdgeInstanceDeploymentResult> CreateEdgeInstanceDeploymentOutcome;
			typedef std::future<CreateEdgeInstanceDeploymentOutcome> CreateEdgeInstanceDeploymentOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateEdgeInstanceDeploymentRequest&, const CreateEdgeInstanceDeploymentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEdgeInstanceDeploymentAsyncHandler;
			typedef Outcome<Error, Model::CreateEdgeOssPreSignedAddressResult> CreateEdgeOssPreSignedAddressOutcome;
			typedef std::future<CreateEdgeOssPreSignedAddressOutcome> CreateEdgeOssPreSignedAddressOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateEdgeOssPreSignedAddressRequest&, const CreateEdgeOssPreSignedAddressOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEdgeOssPreSignedAddressAsyncHandler;
			typedef Outcome<Error, Model::CreateLoRaNodesTaskResult> CreateLoRaNodesTaskOutcome;
			typedef std::future<CreateLoRaNodesTaskOutcome> CreateLoRaNodesTaskOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateLoRaNodesTaskRequest&, const CreateLoRaNodesTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateLoRaNodesTaskAsyncHandler;
			typedef Outcome<Error, Model::CreateOTADynamicUpgradeJobResult> CreateOTADynamicUpgradeJobOutcome;
			typedef std::future<CreateOTADynamicUpgradeJobOutcome> CreateOTADynamicUpgradeJobOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateOTADynamicUpgradeJobRequest&, const CreateOTADynamicUpgradeJobOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateOTADynamicUpgradeJobAsyncHandler;
			typedef Outcome<Error, Model::CreateOTAFirmwareResult> CreateOTAFirmwareOutcome;
			typedef std::future<CreateOTAFirmwareOutcome> CreateOTAFirmwareOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateOTAFirmwareRequest&, const CreateOTAFirmwareOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateOTAFirmwareAsyncHandler;
			typedef Outcome<Error, Model::CreateOTAStaticUpgradeJobResult> CreateOTAStaticUpgradeJobOutcome;
			typedef std::future<CreateOTAStaticUpgradeJobOutcome> CreateOTAStaticUpgradeJobOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateOTAStaticUpgradeJobRequest&, const CreateOTAStaticUpgradeJobOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateOTAStaticUpgradeJobAsyncHandler;
			typedef Outcome<Error, Model::CreateOTAVerifyJobResult> CreateOTAVerifyJobOutcome;
			typedef std::future<CreateOTAVerifyJobOutcome> CreateOTAVerifyJobOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateOTAVerifyJobRequest&, const CreateOTAVerifyJobOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateOTAVerifyJobAsyncHandler;
			typedef Outcome<Error, Model::CreateProductResult> CreateProductOutcome;
			typedef std::future<CreateProductOutcome> CreateProductOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateProductRequest&, const CreateProductOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateProductAsyncHandler;
			typedef Outcome<Error, Model::CreateProductTagsResult> CreateProductTagsOutcome;
			typedef std::future<CreateProductTagsOutcome> CreateProductTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateProductTagsRequest&, const CreateProductTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateProductTagsAsyncHandler;
			typedef Outcome<Error, Model::CreateProductTopicResult> CreateProductTopicOutcome;
			typedef std::future<CreateProductTopicOutcome> CreateProductTopicOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateProductTopicRequest&, const CreateProductTopicOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateProductTopicAsyncHandler;
			typedef Outcome<Error, Model::CreateRuleResult> CreateRuleOutcome;
			typedef std::future<CreateRuleOutcome> CreateRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateRuleRequest&, const CreateRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRuleAsyncHandler;
			typedef Outcome<Error, Model::CreateRuleActionResult> CreateRuleActionOutcome;
			typedef std::future<CreateRuleActionOutcome> CreateRuleActionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateRuleActionRequest&, const CreateRuleActionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRuleActionAsyncHandler;
			typedef Outcome<Error, Model::CreateSceneRuleResult> CreateSceneRuleOutcome;
			typedef std::future<CreateSceneRuleOutcome> CreateSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateSceneRuleRequest&, const CreateSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::CreateSubscribeRelationResult> CreateSubscribeRelationOutcome;
			typedef std::future<CreateSubscribeRelationOutcome> CreateSubscribeRelationOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateSubscribeRelationRequest&, const CreateSubscribeRelationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateSubscribeRelationAsyncHandler;
			typedef Outcome<Error, Model::CreateThingModelResult> CreateThingModelOutcome;
			typedef std::future<CreateThingModelOutcome> CreateThingModelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateThingModelRequest&, const CreateThingModelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateThingModelAsyncHandler;
			typedef Outcome<Error, Model::CreateTopicRouteTableResult> CreateTopicRouteTableOutcome;
			typedef std::future<CreateTopicRouteTableOutcome> CreateTopicRouteTableOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::CreateTopicRouteTableRequest&, const CreateTopicRouteTableOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateTopicRouteTableAsyncHandler;
			typedef Outcome<Error, Model::DeleteConsumerGroupResult> DeleteConsumerGroupOutcome;
			typedef std::future<DeleteConsumerGroupOutcome> DeleteConsumerGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteConsumerGroupRequest&, const DeleteConsumerGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteConsumerGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteConsumerGroupSubscribeRelationResult> DeleteConsumerGroupSubscribeRelationOutcome;
			typedef std::future<DeleteConsumerGroupSubscribeRelationOutcome> DeleteConsumerGroupSubscribeRelationOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteConsumerGroupSubscribeRelationRequest&, const DeleteConsumerGroupSubscribeRelationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteConsumerGroupSubscribeRelationAsyncHandler;
			typedef Outcome<Error, Model::DeleteDeviceResult> DeleteDeviceOutcome;
			typedef std::future<DeleteDeviceOutcome> DeleteDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteDeviceRequest&, const DeleteDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteDeviceAsyncHandler;
			typedef Outcome<Error, Model::DeleteDeviceFileResult> DeleteDeviceFileOutcome;
			typedef std::future<DeleteDeviceFileOutcome> DeleteDeviceFileOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteDeviceFileRequest&, const DeleteDeviceFileOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteDeviceFileAsyncHandler;
			typedef Outcome<Error, Model::DeleteDeviceGroupResult> DeleteDeviceGroupOutcome;
			typedef std::future<DeleteDeviceGroupOutcome> DeleteDeviceGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteDeviceGroupRequest&, const DeleteDeviceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteDeviceGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteDevicePropResult> DeleteDevicePropOutcome;
			typedef std::future<DeleteDevicePropOutcome> DeleteDevicePropOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteDevicePropRequest&, const DeleteDevicePropOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteDevicePropAsyncHandler;
			typedef Outcome<Error, Model::DeleteEdgeDriverResult> DeleteEdgeDriverOutcome;
			typedef std::future<DeleteEdgeDriverOutcome> DeleteEdgeDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteEdgeDriverRequest&, const DeleteEdgeDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteEdgeDriverAsyncHandler;
			typedef Outcome<Error, Model::DeleteEdgeDriverVersionResult> DeleteEdgeDriverVersionOutcome;
			typedef std::future<DeleteEdgeDriverVersionOutcome> DeleteEdgeDriverVersionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteEdgeDriverVersionRequest&, const DeleteEdgeDriverVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteEdgeDriverVersionAsyncHandler;
			typedef Outcome<Error, Model::DeleteEdgeInstanceResult> DeleteEdgeInstanceOutcome;
			typedef std::future<DeleteEdgeInstanceOutcome> DeleteEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteEdgeInstanceRequest&, const DeleteEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::DeleteOTAFirmwareResult> DeleteOTAFirmwareOutcome;
			typedef std::future<DeleteOTAFirmwareOutcome> DeleteOTAFirmwareOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteOTAFirmwareRequest&, const DeleteOTAFirmwareOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteOTAFirmwareAsyncHandler;
			typedef Outcome<Error, Model::DeleteProductResult> DeleteProductOutcome;
			typedef std::future<DeleteProductOutcome> DeleteProductOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteProductRequest&, const DeleteProductOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteProductAsyncHandler;
			typedef Outcome<Error, Model::DeleteProductTagsResult> DeleteProductTagsOutcome;
			typedef std::future<DeleteProductTagsOutcome> DeleteProductTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteProductTagsRequest&, const DeleteProductTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteProductTagsAsyncHandler;
			typedef Outcome<Error, Model::DeleteProductTopicResult> DeleteProductTopicOutcome;
			typedef std::future<DeleteProductTopicOutcome> DeleteProductTopicOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteProductTopicRequest&, const DeleteProductTopicOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteProductTopicAsyncHandler;
			typedef Outcome<Error, Model::DeleteRuleResult> DeleteRuleOutcome;
			typedef std::future<DeleteRuleOutcome> DeleteRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteRuleRequest&, const DeleteRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRuleAsyncHandler;
			typedef Outcome<Error, Model::DeleteRuleActionResult> DeleteRuleActionOutcome;
			typedef std::future<DeleteRuleActionOutcome> DeleteRuleActionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteRuleActionRequest&, const DeleteRuleActionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRuleActionAsyncHandler;
			typedef Outcome<Error, Model::DeleteSceneRuleResult> DeleteSceneRuleOutcome;
			typedef std::future<DeleteSceneRuleOutcome> DeleteSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteSceneRuleRequest&, const DeleteSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::DeleteSubscribeRelationResult> DeleteSubscribeRelationOutcome;
			typedef std::future<DeleteSubscribeRelationOutcome> DeleteSubscribeRelationOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteSubscribeRelationRequest&, const DeleteSubscribeRelationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteSubscribeRelationAsyncHandler;
			typedef Outcome<Error, Model::DeleteThingModelResult> DeleteThingModelOutcome;
			typedef std::future<DeleteThingModelOutcome> DeleteThingModelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteThingModelRequest&, const DeleteThingModelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteThingModelAsyncHandler;
			typedef Outcome<Error, Model::DeleteTopicRouteTableResult> DeleteTopicRouteTableOutcome;
			typedef std::future<DeleteTopicRouteTableOutcome> DeleteTopicRouteTableOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DeleteTopicRouteTableRequest&, const DeleteTopicRouteTableOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteTopicRouteTableAsyncHandler;
			typedef Outcome<Error, Model::DisableSceneRuleResult> DisableSceneRuleOutcome;
			typedef std::future<DisableSceneRuleOutcome> DisableSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DisableSceneRuleRequest&, const DisableSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DisableSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::DisableThingResult> DisableThingOutcome;
			typedef std::future<DisableThingOutcome> DisableThingOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::DisableThingRequest&, const DisableThingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DisableThingAsyncHandler;
			typedef Outcome<Error, Model::EnableSceneRuleResult> EnableSceneRuleOutcome;
			typedef std::future<EnableSceneRuleOutcome> EnableSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::EnableSceneRuleRequest&, const EnableSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnableSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::EnableThingResult> EnableThingOutcome;
			typedef std::future<EnableThingOutcome> EnableThingOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::EnableThingRequest&, const EnableThingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnableThingAsyncHandler;
			typedef Outcome<Error, Model::GenerateOTAUploadURLResult> GenerateOTAUploadURLOutcome;
			typedef std::future<GenerateOTAUploadURLOutcome> GenerateOTAUploadURLOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GenerateOTAUploadURLRequest&, const GenerateOTAUploadURLOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GenerateOTAUploadURLAsyncHandler;
			typedef Outcome<Error, Model::GetDataAPIServiceDetailResult> GetDataAPIServiceDetailOutcome;
			typedef std::future<GetDataAPIServiceDetailOutcome> GetDataAPIServiceDetailOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetDataAPIServiceDetailRequest&, const GetDataAPIServiceDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDataAPIServiceDetailAsyncHandler;
			typedef Outcome<Error, Model::GetDeviceShadowResult> GetDeviceShadowOutcome;
			typedef std::future<GetDeviceShadowOutcome> GetDeviceShadowOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetDeviceShadowRequest&, const GetDeviceShadowOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDeviceShadowAsyncHandler;
			typedef Outcome<Error, Model::GetDeviceStatusResult> GetDeviceStatusOutcome;
			typedef std::future<GetDeviceStatusOutcome> GetDeviceStatusOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetDeviceStatusRequest&, const GetDeviceStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDeviceStatusAsyncHandler;
			typedef Outcome<Error, Model::GetEdgeDriverVersionResult> GetEdgeDriverVersionOutcome;
			typedef std::future<GetEdgeDriverVersionOutcome> GetEdgeDriverVersionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetEdgeDriverVersionRequest&, const GetEdgeDriverVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetEdgeDriverVersionAsyncHandler;
			typedef Outcome<Error, Model::GetEdgeInstanceResult> GetEdgeInstanceOutcome;
			typedef std::future<GetEdgeInstanceOutcome> GetEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetEdgeInstanceRequest&, const GetEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::GetEdgeInstanceDeploymentResult> GetEdgeInstanceDeploymentOutcome;
			typedef std::future<GetEdgeInstanceDeploymentOutcome> GetEdgeInstanceDeploymentOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetEdgeInstanceDeploymentRequest&, const GetEdgeInstanceDeploymentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetEdgeInstanceDeploymentAsyncHandler;
			typedef Outcome<Error, Model::GetGatewayBySubDeviceResult> GetGatewayBySubDeviceOutcome;
			typedef std::future<GetGatewayBySubDeviceOutcome> GetGatewayBySubDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetGatewayBySubDeviceRequest&, const GetGatewayBySubDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetGatewayBySubDeviceAsyncHandler;
			typedef Outcome<Error, Model::GetLoraNodesTaskResult> GetLoraNodesTaskOutcome;
			typedef std::future<GetLoraNodesTaskOutcome> GetLoraNodesTaskOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetLoraNodesTaskRequest&, const GetLoraNodesTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetLoraNodesTaskAsyncHandler;
			typedef Outcome<Error, Model::GetNodesAddingTaskResult> GetNodesAddingTaskOutcome;
			typedef std::future<GetNodesAddingTaskOutcome> GetNodesAddingTaskOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetNodesAddingTaskRequest&, const GetNodesAddingTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetNodesAddingTaskAsyncHandler;
			typedef Outcome<Error, Model::GetRuleResult> GetRuleOutcome;
			typedef std::future<GetRuleOutcome> GetRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetRuleRequest&, const GetRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRuleAsyncHandler;
			typedef Outcome<Error, Model::GetRuleActionResult> GetRuleActionOutcome;
			typedef std::future<GetRuleActionOutcome> GetRuleActionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetRuleActionRequest&, const GetRuleActionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetRuleActionAsyncHandler;
			typedef Outcome<Error, Model::GetSceneRuleResult> GetSceneRuleOutcome;
			typedef std::future<GetSceneRuleOutcome> GetSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetSceneRuleRequest&, const GetSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::GetThingModelTslResult> GetThingModelTslOutcome;
			typedef std::future<GetThingModelTslOutcome> GetThingModelTslOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetThingModelTslRequest&, const GetThingModelTslOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetThingModelTslAsyncHandler;
			typedef Outcome<Error, Model::GetThingTemplateResult> GetThingTemplateOutcome;
			typedef std::future<GetThingTemplateOutcome> GetThingTemplateOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetThingTemplateRequest&, const GetThingTemplateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetThingTemplateAsyncHandler;
			typedef Outcome<Error, Model::GetThingTopoResult> GetThingTopoOutcome;
			typedef std::future<GetThingTopoOutcome> GetThingTopoOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::GetThingTopoRequest&, const GetThingTopoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetThingTopoAsyncHandler;
			typedef Outcome<Error, Model::ImportThingModelTslResult> ImportThingModelTslOutcome;
			typedef std::future<ImportThingModelTslOutcome> ImportThingModelTslOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ImportThingModelTslRequest&, const ImportThingModelTslOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ImportThingModelTslAsyncHandler;
			typedef Outcome<Error, Model::InvokeDataAPIServiceResult> InvokeDataAPIServiceOutcome;
			typedef std::future<InvokeDataAPIServiceOutcome> InvokeDataAPIServiceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::InvokeDataAPIServiceRequest&, const InvokeDataAPIServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> InvokeDataAPIServiceAsyncHandler;
			typedef Outcome<Error, Model::InvokeThingServiceResult> InvokeThingServiceOutcome;
			typedef std::future<InvokeThingServiceOutcome> InvokeThingServiceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::InvokeThingServiceRequest&, const InvokeThingServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> InvokeThingServiceAsyncHandler;
			typedef Outcome<Error, Model::InvokeThingsServiceResult> InvokeThingsServiceOutcome;
			typedef std::future<InvokeThingsServiceOutcome> InvokeThingsServiceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::InvokeThingsServiceRequest&, const InvokeThingsServiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> InvokeThingsServiceAsyncHandler;
			typedef Outcome<Error, Model::ListOTAFirmwareResult> ListOTAFirmwareOutcome;
			typedef std::future<ListOTAFirmwareOutcome> ListOTAFirmwareOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListOTAFirmwareRequest&, const ListOTAFirmwareOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListOTAFirmwareAsyncHandler;
			typedef Outcome<Error, Model::ListOTAJobByDeviceResult> ListOTAJobByDeviceOutcome;
			typedef std::future<ListOTAJobByDeviceOutcome> ListOTAJobByDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListOTAJobByDeviceRequest&, const ListOTAJobByDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListOTAJobByDeviceAsyncHandler;
			typedef Outcome<Error, Model::ListOTAJobByFirmwareResult> ListOTAJobByFirmwareOutcome;
			typedef std::future<ListOTAJobByFirmwareOutcome> ListOTAJobByFirmwareOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListOTAJobByFirmwareRequest&, const ListOTAJobByFirmwareOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListOTAJobByFirmwareAsyncHandler;
			typedef Outcome<Error, Model::ListOTATaskByJobResult> ListOTATaskByJobOutcome;
			typedef std::future<ListOTATaskByJobOutcome> ListOTATaskByJobOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListOTATaskByJobRequest&, const ListOTATaskByJobOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListOTATaskByJobAsyncHandler;
			typedef Outcome<Error, Model::ListProductByTagsResult> ListProductByTagsOutcome;
			typedef std::future<ListProductByTagsOutcome> ListProductByTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListProductByTagsRequest&, const ListProductByTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListProductByTagsAsyncHandler;
			typedef Outcome<Error, Model::ListProductTagsResult> ListProductTagsOutcome;
			typedef std::future<ListProductTagsOutcome> ListProductTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListProductTagsRequest&, const ListProductTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListProductTagsAsyncHandler;
			typedef Outcome<Error, Model::ListRuleResult> ListRuleOutcome;
			typedef std::future<ListRuleOutcome> ListRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListRuleRequest&, const ListRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListRuleAsyncHandler;
			typedef Outcome<Error, Model::ListRuleActionsResult> ListRuleActionsOutcome;
			typedef std::future<ListRuleActionsOutcome> ListRuleActionsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListRuleActionsRequest&, const ListRuleActionsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListRuleActionsAsyncHandler;
			typedef Outcome<Error, Model::ListThingModelVersionResult> ListThingModelVersionOutcome;
			typedef std::future<ListThingModelVersionOutcome> ListThingModelVersionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListThingModelVersionRequest&, const ListThingModelVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListThingModelVersionAsyncHandler;
			typedef Outcome<Error, Model::ListThingTemplatesResult> ListThingTemplatesOutcome;
			typedef std::future<ListThingTemplatesOutcome> ListThingTemplatesOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ListThingTemplatesRequest&, const ListThingTemplatesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListThingTemplatesAsyncHandler;
			typedef Outcome<Error, Model::NotifyAddThingTopoResult> NotifyAddThingTopoOutcome;
			typedef std::future<NotifyAddThingTopoOutcome> NotifyAddThingTopoOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::NotifyAddThingTopoRequest&, const NotifyAddThingTopoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> NotifyAddThingTopoAsyncHandler;
			typedef Outcome<Error, Model::PubResult> PubOutcome;
			typedef std::future<PubOutcome> PubOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::PubRequest&, const PubOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PubAsyncHandler;
			typedef Outcome<Error, Model::PubBroadcastResult> PubBroadcastOutcome;
			typedef std::future<PubBroadcastOutcome> PubBroadcastOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::PubBroadcastRequest&, const PubBroadcastOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PubBroadcastAsyncHandler;
			typedef Outcome<Error, Model::PublishThingModelResult> PublishThingModelOutcome;
			typedef std::future<PublishThingModelOutcome> PublishThingModelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::PublishThingModelRequest&, const PublishThingModelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> PublishThingModelAsyncHandler;
			typedef Outcome<Error, Model::QueryAppDeviceListResult> QueryAppDeviceListOutcome;
			typedef std::future<QueryAppDeviceListOutcome> QueryAppDeviceListOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryAppDeviceListRequest&, const QueryAppDeviceListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryAppDeviceListAsyncHandler;
			typedef Outcome<Error, Model::QueryBatchRegisterDeviceStatusResult> QueryBatchRegisterDeviceStatusOutcome;
			typedef std::future<QueryBatchRegisterDeviceStatusOutcome> QueryBatchRegisterDeviceStatusOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryBatchRegisterDeviceStatusRequest&, const QueryBatchRegisterDeviceStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryBatchRegisterDeviceStatusAsyncHandler;
			typedef Outcome<Error, Model::QueryCertUrlByApplyIdResult> QueryCertUrlByApplyIdOutcome;
			typedef std::future<QueryCertUrlByApplyIdOutcome> QueryCertUrlByApplyIdOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryCertUrlByApplyIdRequest&, const QueryCertUrlByApplyIdOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryCertUrlByApplyIdAsyncHandler;
			typedef Outcome<Error, Model::QueryConsumerGroupByGroupIdResult> QueryConsumerGroupByGroupIdOutcome;
			typedef std::future<QueryConsumerGroupByGroupIdOutcome> QueryConsumerGroupByGroupIdOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryConsumerGroupByGroupIdRequest&, const QueryConsumerGroupByGroupIdOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryConsumerGroupByGroupIdAsyncHandler;
			typedef Outcome<Error, Model::QueryConsumerGroupListResult> QueryConsumerGroupListOutcome;
			typedef std::future<QueryConsumerGroupListOutcome> QueryConsumerGroupListOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryConsumerGroupListRequest&, const QueryConsumerGroupListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryConsumerGroupListAsyncHandler;
			typedef Outcome<Error, Model::QueryConsumerGroupStatusResult> QueryConsumerGroupStatusOutcome;
			typedef std::future<QueryConsumerGroupStatusOutcome> QueryConsumerGroupStatusOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryConsumerGroupStatusRequest&, const QueryConsumerGroupStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryConsumerGroupStatusAsyncHandler;
			typedef Outcome<Error, Model::QueryDetailSceneRuleLogResult> QueryDetailSceneRuleLogOutcome;
			typedef std::future<QueryDetailSceneRuleLogOutcome> QueryDetailSceneRuleLogOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDetailSceneRuleLogRequest&, const QueryDetailSceneRuleLogOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDetailSceneRuleLogAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceResult> QueryDeviceOutcome;
			typedef std::future<QueryDeviceOutcome> QueryDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceRequest&, const QueryDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceByStatusResult> QueryDeviceByStatusOutcome;
			typedef std::future<QueryDeviceByStatusOutcome> QueryDeviceByStatusOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceByStatusRequest&, const QueryDeviceByStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceByStatusAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceByTagsResult> QueryDeviceByTagsOutcome;
			typedef std::future<QueryDeviceByTagsOutcome> QueryDeviceByTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceByTagsRequest&, const QueryDeviceByTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceByTagsAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceCertResult> QueryDeviceCertOutcome;
			typedef std::future<QueryDeviceCertOutcome> QueryDeviceCertOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceCertRequest&, const QueryDeviceCertOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceCertAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceDesiredPropertyResult> QueryDeviceDesiredPropertyOutcome;
			typedef std::future<QueryDeviceDesiredPropertyOutcome> QueryDeviceDesiredPropertyOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceDesiredPropertyRequest&, const QueryDeviceDesiredPropertyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceDesiredPropertyAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceDetailResult> QueryDeviceDetailOutcome;
			typedef std::future<QueryDeviceDetailOutcome> QueryDeviceDetailOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceDetailRequest&, const QueryDeviceDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceDetailAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceEventDataResult> QueryDeviceEventDataOutcome;
			typedef std::future<QueryDeviceEventDataOutcome> QueryDeviceEventDataOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceEventDataRequest&, const QueryDeviceEventDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceEventDataAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceFileResult> QueryDeviceFileOutcome;
			typedef std::future<QueryDeviceFileOutcome> QueryDeviceFileOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceFileRequest&, const QueryDeviceFileOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceFileAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceFileListResult> QueryDeviceFileListOutcome;
			typedef std::future<QueryDeviceFileListOutcome> QueryDeviceFileListOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceFileListRequest&, const QueryDeviceFileListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceFileListAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceGroupByDeviceResult> QueryDeviceGroupByDeviceOutcome;
			typedef std::future<QueryDeviceGroupByDeviceOutcome> QueryDeviceGroupByDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceGroupByDeviceRequest&, const QueryDeviceGroupByDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceGroupByDeviceAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceGroupByTagsResult> QueryDeviceGroupByTagsOutcome;
			typedef std::future<QueryDeviceGroupByTagsOutcome> QueryDeviceGroupByTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceGroupByTagsRequest&, const QueryDeviceGroupByTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceGroupByTagsAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceGroupInfoResult> QueryDeviceGroupInfoOutcome;
			typedef std::future<QueryDeviceGroupInfoOutcome> QueryDeviceGroupInfoOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceGroupInfoRequest&, const QueryDeviceGroupInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceGroupInfoAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceGroupListResult> QueryDeviceGroupListOutcome;
			typedef std::future<QueryDeviceGroupListOutcome> QueryDeviceGroupListOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceGroupListRequest&, const QueryDeviceGroupListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceGroupListAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceGroupTagListResult> QueryDeviceGroupTagListOutcome;
			typedef std::future<QueryDeviceGroupTagListOutcome> QueryDeviceGroupTagListOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceGroupTagListRequest&, const QueryDeviceGroupTagListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceGroupTagListAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceListByDeviceGroupResult> QueryDeviceListByDeviceGroupOutcome;
			typedef std::future<QueryDeviceListByDeviceGroupOutcome> QueryDeviceListByDeviceGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceListByDeviceGroupRequest&, const QueryDeviceListByDeviceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceListByDeviceGroupAsyncHandler;
			typedef Outcome<Error, Model::QueryDevicePropResult> QueryDevicePropOutcome;
			typedef std::future<QueryDevicePropOutcome> QueryDevicePropOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDevicePropRequest&, const QueryDevicePropOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDevicePropAsyncHandler;
			typedef Outcome<Error, Model::QueryDevicePropertiesDataResult> QueryDevicePropertiesDataOutcome;
			typedef std::future<QueryDevicePropertiesDataOutcome> QueryDevicePropertiesDataOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDevicePropertiesDataRequest&, const QueryDevicePropertiesDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDevicePropertiesDataAsyncHandler;
			typedef Outcome<Error, Model::QueryDevicePropertyDataResult> QueryDevicePropertyDataOutcome;
			typedef std::future<QueryDevicePropertyDataOutcome> QueryDevicePropertyDataOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDevicePropertyDataRequest&, const QueryDevicePropertyDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDevicePropertyDataAsyncHandler;
			typedef Outcome<Error, Model::QueryDevicePropertyStatusResult> QueryDevicePropertyStatusOutcome;
			typedef std::future<QueryDevicePropertyStatusOutcome> QueryDevicePropertyStatusOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDevicePropertyStatusRequest&, const QueryDevicePropertyStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDevicePropertyStatusAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceServiceDataResult> QueryDeviceServiceDataOutcome;
			typedef std::future<QueryDeviceServiceDataOutcome> QueryDeviceServiceDataOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceServiceDataRequest&, const QueryDeviceServiceDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceServiceDataAsyncHandler;
			typedef Outcome<Error, Model::QueryDeviceStatisticsResult> QueryDeviceStatisticsOutcome;
			typedef std::future<QueryDeviceStatisticsOutcome> QueryDeviceStatisticsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryDeviceStatisticsRequest&, const QueryDeviceStatisticsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryDeviceStatisticsAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeDriverResult> QueryEdgeDriverOutcome;
			typedef std::future<QueryEdgeDriverOutcome> QueryEdgeDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeDriverRequest&, const QueryEdgeDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeDriverAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeDriverVersionResult> QueryEdgeDriverVersionOutcome;
			typedef std::future<QueryEdgeDriverVersionOutcome> QueryEdgeDriverVersionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeDriverVersionRequest&, const QueryEdgeDriverVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeDriverVersionAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceResult> QueryEdgeInstanceOutcome;
			typedef std::future<QueryEdgeInstanceOutcome> QueryEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceRequest&, const QueryEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceChannelResult> QueryEdgeInstanceChannelOutcome;
			typedef std::future<QueryEdgeInstanceChannelOutcome> QueryEdgeInstanceChannelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceChannelRequest&, const QueryEdgeInstanceChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceChannelAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceDeviceResult> QueryEdgeInstanceDeviceOutcome;
			typedef std::future<QueryEdgeInstanceDeviceOutcome> QueryEdgeInstanceDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceDeviceRequest&, const QueryEdgeInstanceDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceDeviceAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceDeviceByDriverResult> QueryEdgeInstanceDeviceByDriverOutcome;
			typedef std::future<QueryEdgeInstanceDeviceByDriverOutcome> QueryEdgeInstanceDeviceByDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceDeviceByDriverRequest&, const QueryEdgeInstanceDeviceByDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceDeviceByDriverAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceDriverResult> QueryEdgeInstanceDriverOutcome;
			typedef std::future<QueryEdgeInstanceDriverOutcome> QueryEdgeInstanceDriverOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceDriverRequest&, const QueryEdgeInstanceDriverOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceDriverAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceGatewayResult> QueryEdgeInstanceGatewayOutcome;
			typedef std::future<QueryEdgeInstanceGatewayOutcome> QueryEdgeInstanceGatewayOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceGatewayRequest&, const QueryEdgeInstanceGatewayOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceGatewayAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceHistoricDeploymentResult> QueryEdgeInstanceHistoricDeploymentOutcome;
			typedef std::future<QueryEdgeInstanceHistoricDeploymentOutcome> QueryEdgeInstanceHistoricDeploymentOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceHistoricDeploymentRequest&, const QueryEdgeInstanceHistoricDeploymentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceHistoricDeploymentAsyncHandler;
			typedef Outcome<Error, Model::QueryEdgeInstanceSceneRuleResult> QueryEdgeInstanceSceneRuleOutcome;
			typedef std::future<QueryEdgeInstanceSceneRuleOutcome> QueryEdgeInstanceSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryEdgeInstanceSceneRuleRequest&, const QueryEdgeInstanceSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryEdgeInstanceSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::QueryLoRaJoinPermissionsResult> QueryLoRaJoinPermissionsOutcome;
			typedef std::future<QueryLoRaJoinPermissionsOutcome> QueryLoRaJoinPermissionsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryLoRaJoinPermissionsRequest&, const QueryLoRaJoinPermissionsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryLoRaJoinPermissionsAsyncHandler;
			typedef Outcome<Error, Model::QueryOTAFirmwareResult> QueryOTAFirmwareOutcome;
			typedef std::future<QueryOTAFirmwareOutcome> QueryOTAFirmwareOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryOTAFirmwareRequest&, const QueryOTAFirmwareOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryOTAFirmwareAsyncHandler;
			typedef Outcome<Error, Model::QueryOTAJobResult> QueryOTAJobOutcome;
			typedef std::future<QueryOTAJobOutcome> QueryOTAJobOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryOTAJobRequest&, const QueryOTAJobOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryOTAJobAsyncHandler;
			typedef Outcome<Error, Model::QueryPageByApplyIdResult> QueryPageByApplyIdOutcome;
			typedef std::future<QueryPageByApplyIdOutcome> QueryPageByApplyIdOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryPageByApplyIdRequest&, const QueryPageByApplyIdOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryPageByApplyIdAsyncHandler;
			typedef Outcome<Error, Model::QueryProductResult> QueryProductOutcome;
			typedef std::future<QueryProductOutcome> QueryProductOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryProductRequest&, const QueryProductOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryProductAsyncHandler;
			typedef Outcome<Error, Model::QueryProductCertInfoResult> QueryProductCertInfoOutcome;
			typedef std::future<QueryProductCertInfoOutcome> QueryProductCertInfoOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryProductCertInfoRequest&, const QueryProductCertInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryProductCertInfoAsyncHandler;
			typedef Outcome<Error, Model::QueryProductListResult> QueryProductListOutcome;
			typedef std::future<QueryProductListOutcome> QueryProductListOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryProductListRequest&, const QueryProductListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryProductListAsyncHandler;
			typedef Outcome<Error, Model::QueryProductTopicResult> QueryProductTopicOutcome;
			typedef std::future<QueryProductTopicOutcome> QueryProductTopicOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryProductTopicRequest&, const QueryProductTopicOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryProductTopicAsyncHandler;
			typedef Outcome<Error, Model::QuerySceneRuleResult> QuerySceneRuleOutcome;
			typedef std::future<QuerySceneRuleOutcome> QuerySceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QuerySceneRuleRequest&, const QuerySceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QuerySceneRuleAsyncHandler;
			typedef Outcome<Error, Model::QuerySubscribeRelationResult> QuerySubscribeRelationOutcome;
			typedef std::future<QuerySubscribeRelationOutcome> QuerySubscribeRelationOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QuerySubscribeRelationRequest&, const QuerySubscribeRelationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QuerySubscribeRelationAsyncHandler;
			typedef Outcome<Error, Model::QuerySummarySceneRuleLogResult> QuerySummarySceneRuleLogOutcome;
			typedef std::future<QuerySummarySceneRuleLogOutcome> QuerySummarySceneRuleLogOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QuerySummarySceneRuleLogRequest&, const QuerySummarySceneRuleLogOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QuerySummarySceneRuleLogAsyncHandler;
			typedef Outcome<Error, Model::QuerySuperDeviceGroupResult> QuerySuperDeviceGroupOutcome;
			typedef std::future<QuerySuperDeviceGroupOutcome> QuerySuperDeviceGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QuerySuperDeviceGroupRequest&, const QuerySuperDeviceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QuerySuperDeviceGroupAsyncHandler;
			typedef Outcome<Error, Model::QueryThingModelResult> QueryThingModelOutcome;
			typedef std::future<QueryThingModelOutcome> QueryThingModelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryThingModelRequest&, const QueryThingModelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryThingModelAsyncHandler;
			typedef Outcome<Error, Model::QueryTopicReverseRouteTableResult> QueryTopicReverseRouteTableOutcome;
			typedef std::future<QueryTopicReverseRouteTableOutcome> QueryTopicReverseRouteTableOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryTopicReverseRouteTableRequest&, const QueryTopicReverseRouteTableOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryTopicReverseRouteTableAsyncHandler;
			typedef Outcome<Error, Model::QueryTopicRouteTableResult> QueryTopicRouteTableOutcome;
			typedef std::future<QueryTopicRouteTableOutcome> QueryTopicRouteTableOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::QueryTopicRouteTableRequest&, const QueryTopicRouteTableOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> QueryTopicRouteTableAsyncHandler;
			typedef Outcome<Error, Model::RRpcResult> RRpcOutcome;
			typedef std::future<RRpcOutcome> RRpcOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::RRpcRequest&, const RRpcOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RRpcAsyncHandler;
			typedef Outcome<Error, Model::RegisterDeviceResult> RegisterDeviceOutcome;
			typedef std::future<RegisterDeviceOutcome> RegisterDeviceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::RegisterDeviceRequest&, const RegisterDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterDeviceAsyncHandler;
			typedef Outcome<Error, Model::ReleaseEdgeDriverVersionResult> ReleaseEdgeDriverVersionOutcome;
			typedef std::future<ReleaseEdgeDriverVersionOutcome> ReleaseEdgeDriverVersionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ReleaseEdgeDriverVersionRequest&, const ReleaseEdgeDriverVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ReleaseEdgeDriverVersionAsyncHandler;
			typedef Outcome<Error, Model::RemoveThingTopoResult> RemoveThingTopoOutcome;
			typedef std::future<RemoveThingTopoOutcome> RemoveThingTopoOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::RemoveThingTopoRequest&, const RemoveThingTopoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveThingTopoAsyncHandler;
			typedef Outcome<Error, Model::ReplaceEdgeInstanceGatewayResult> ReplaceEdgeInstanceGatewayOutcome;
			typedef std::future<ReplaceEdgeInstanceGatewayOutcome> ReplaceEdgeInstanceGatewayOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ReplaceEdgeInstanceGatewayRequest&, const ReplaceEdgeInstanceGatewayOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ReplaceEdgeInstanceGatewayAsyncHandler;
			typedef Outcome<Error, Model::ResetConsumerGroupPositionResult> ResetConsumerGroupPositionOutcome;
			typedef std::future<ResetConsumerGroupPositionOutcome> ResetConsumerGroupPositionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ResetConsumerGroupPositionRequest&, const ResetConsumerGroupPositionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ResetConsumerGroupPositionAsyncHandler;
			typedef Outcome<Error, Model::ResetThingResult> ResetThingOutcome;
			typedef std::future<ResetThingOutcome> ResetThingOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::ResetThingRequest&, const ResetThingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ResetThingAsyncHandler;
			typedef Outcome<Error, Model::SaveDevicePropResult> SaveDevicePropOutcome;
			typedef std::future<SaveDevicePropOutcome> SaveDevicePropOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::SaveDevicePropRequest&, const SaveDevicePropOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SaveDevicePropAsyncHandler;
			typedef Outcome<Error, Model::SetDeviceDesiredPropertyResult> SetDeviceDesiredPropertyOutcome;
			typedef std::future<SetDeviceDesiredPropertyOutcome> SetDeviceDesiredPropertyOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::SetDeviceDesiredPropertyRequest&, const SetDeviceDesiredPropertyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetDeviceDesiredPropertyAsyncHandler;
			typedef Outcome<Error, Model::SetDeviceGroupTagsResult> SetDeviceGroupTagsOutcome;
			typedef std::future<SetDeviceGroupTagsOutcome> SetDeviceGroupTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::SetDeviceGroupTagsRequest&, const SetDeviceGroupTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetDeviceGroupTagsAsyncHandler;
			typedef Outcome<Error, Model::SetDevicePropertyResult> SetDevicePropertyOutcome;
			typedef std::future<SetDevicePropertyOutcome> SetDevicePropertyOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::SetDevicePropertyRequest&, const SetDevicePropertyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetDevicePropertyAsyncHandler;
			typedef Outcome<Error, Model::SetDevicesPropertyResult> SetDevicesPropertyOutcome;
			typedef std::future<SetDevicesPropertyOutcome> SetDevicesPropertyOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::SetDevicesPropertyRequest&, const SetDevicesPropertyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetDevicesPropertyAsyncHandler;
			typedef Outcome<Error, Model::SetEdgeInstanceDriverConfigsResult> SetEdgeInstanceDriverConfigsOutcome;
			typedef std::future<SetEdgeInstanceDriverConfigsOutcome> SetEdgeInstanceDriverConfigsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::SetEdgeInstanceDriverConfigsRequest&, const SetEdgeInstanceDriverConfigsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetEdgeInstanceDriverConfigsAsyncHandler;
			typedef Outcome<Error, Model::SetProductCertInfoResult> SetProductCertInfoOutcome;
			typedef std::future<SetProductCertInfoOutcome> SetProductCertInfoOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::SetProductCertInfoRequest&, const SetProductCertInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetProductCertInfoAsyncHandler;
			typedef Outcome<Error, Model::StartRuleResult> StartRuleOutcome;
			typedef std::future<StartRuleOutcome> StartRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::StartRuleRequest&, const StartRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartRuleAsyncHandler;
			typedef Outcome<Error, Model::StopRuleResult> StopRuleOutcome;
			typedef std::future<StopRuleOutcome> StopRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::StopRuleRequest&, const StopRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopRuleAsyncHandler;
			typedef Outcome<Error, Model::TriggerSceneRuleResult> TriggerSceneRuleOutcome;
			typedef std::future<TriggerSceneRuleOutcome> TriggerSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::TriggerSceneRuleRequest&, const TriggerSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TriggerSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::UnbindApplicationFromEdgeInstanceResult> UnbindApplicationFromEdgeInstanceOutcome;
			typedef std::future<UnbindApplicationFromEdgeInstanceOutcome> UnbindApplicationFromEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UnbindApplicationFromEdgeInstanceRequest&, const UnbindApplicationFromEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnbindApplicationFromEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::UnbindDriverFromEdgeInstanceResult> UnbindDriverFromEdgeInstanceOutcome;
			typedef std::future<UnbindDriverFromEdgeInstanceOutcome> UnbindDriverFromEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UnbindDriverFromEdgeInstanceRequest&, const UnbindDriverFromEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnbindDriverFromEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::UnbindSceneRuleFromEdgeInstanceResult> UnbindSceneRuleFromEdgeInstanceOutcome;
			typedef std::future<UnbindSceneRuleFromEdgeInstanceOutcome> UnbindSceneRuleFromEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UnbindSceneRuleFromEdgeInstanceRequest&, const UnbindSceneRuleFromEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnbindSceneRuleFromEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::UpdateConsumerGroupResult> UpdateConsumerGroupOutcome;
			typedef std::future<UpdateConsumerGroupOutcome> UpdateConsumerGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateConsumerGroupRequest&, const UpdateConsumerGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateConsumerGroupAsyncHandler;
			typedef Outcome<Error, Model::UpdateDeviceGroupResult> UpdateDeviceGroupOutcome;
			typedef std::future<UpdateDeviceGroupOutcome> UpdateDeviceGroupOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateDeviceGroupRequest&, const UpdateDeviceGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateDeviceGroupAsyncHandler;
			typedef Outcome<Error, Model::UpdateDeviceShadowResult> UpdateDeviceShadowOutcome;
			typedef std::future<UpdateDeviceShadowOutcome> UpdateDeviceShadowOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateDeviceShadowRequest&, const UpdateDeviceShadowOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateDeviceShadowAsyncHandler;
			typedef Outcome<Error, Model::UpdateEdgeDriverVersionResult> UpdateEdgeDriverVersionOutcome;
			typedef std::future<UpdateEdgeDriverVersionOutcome> UpdateEdgeDriverVersionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateEdgeDriverVersionRequest&, const UpdateEdgeDriverVersionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateEdgeDriverVersionAsyncHandler;
			typedef Outcome<Error, Model::UpdateEdgeInstanceResult> UpdateEdgeInstanceOutcome;
			typedef std::future<UpdateEdgeInstanceOutcome> UpdateEdgeInstanceOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateEdgeInstanceRequest&, const UpdateEdgeInstanceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateEdgeInstanceAsyncHandler;
			typedef Outcome<Error, Model::UpdateEdgeInstanceChannelResult> UpdateEdgeInstanceChannelOutcome;
			typedef std::future<UpdateEdgeInstanceChannelOutcome> UpdateEdgeInstanceChannelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateEdgeInstanceChannelRequest&, const UpdateEdgeInstanceChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateEdgeInstanceChannelAsyncHandler;
			typedef Outcome<Error, Model::UpdateProductResult> UpdateProductOutcome;
			typedef std::future<UpdateProductOutcome> UpdateProductOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateProductRequest&, const UpdateProductOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateProductAsyncHandler;
			typedef Outcome<Error, Model::UpdateProductFilterConfigResult> UpdateProductFilterConfigOutcome;
			typedef std::future<UpdateProductFilterConfigOutcome> UpdateProductFilterConfigOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateProductFilterConfigRequest&, const UpdateProductFilterConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateProductFilterConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateProductTagsResult> UpdateProductTagsOutcome;
			typedef std::future<UpdateProductTagsOutcome> UpdateProductTagsOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateProductTagsRequest&, const UpdateProductTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateProductTagsAsyncHandler;
			typedef Outcome<Error, Model::UpdateProductTopicResult> UpdateProductTopicOutcome;
			typedef std::future<UpdateProductTopicOutcome> UpdateProductTopicOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateProductTopicRequest&, const UpdateProductTopicOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateProductTopicAsyncHandler;
			typedef Outcome<Error, Model::UpdateRuleResult> UpdateRuleOutcome;
			typedef std::future<UpdateRuleOutcome> UpdateRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateRuleRequest&, const UpdateRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRuleAsyncHandler;
			typedef Outcome<Error, Model::UpdateRuleActionResult> UpdateRuleActionOutcome;
			typedef std::future<UpdateRuleActionOutcome> UpdateRuleActionOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateRuleActionRequest&, const UpdateRuleActionOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRuleActionAsyncHandler;
			typedef Outcome<Error, Model::UpdateSceneRuleResult> UpdateSceneRuleOutcome;
			typedef std::future<UpdateSceneRuleOutcome> UpdateSceneRuleOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateSceneRuleRequest&, const UpdateSceneRuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateSceneRuleAsyncHandler;
			typedef Outcome<Error, Model::UpdateSubscribeRelationResult> UpdateSubscribeRelationOutcome;
			typedef std::future<UpdateSubscribeRelationOutcome> UpdateSubscribeRelationOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateSubscribeRelationRequest&, const UpdateSubscribeRelationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateSubscribeRelationAsyncHandler;
			typedef Outcome<Error, Model::UpdateThingModelResult> UpdateThingModelOutcome;
			typedef std::future<UpdateThingModelOutcome> UpdateThingModelOutcomeCallable;
			typedef std::function<void(const IotClient*, const Model::UpdateThingModelRequest&, const UpdateThingModelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateThingModelAsyncHandler;

			IotClient(const Credentials &credentials, const ClientConfiguration &configuration);
			IotClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			IotClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~IotClient();
			BatchAddDeviceGroupRelationsOutcome batchAddDeviceGroupRelations(const Model::BatchAddDeviceGroupRelationsRequest &request)const;
			void batchAddDeviceGroupRelationsAsync(const Model::BatchAddDeviceGroupRelationsRequest& request, const BatchAddDeviceGroupRelationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchAddDeviceGroupRelationsOutcomeCallable batchAddDeviceGroupRelationsCallable(const Model::BatchAddDeviceGroupRelationsRequest& request) const;
			BatchAddThingTopoOutcome batchAddThingTopo(const Model::BatchAddThingTopoRequest &request)const;
			void batchAddThingTopoAsync(const Model::BatchAddThingTopoRequest& request, const BatchAddThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchAddThingTopoOutcomeCallable batchAddThingTopoCallable(const Model::BatchAddThingTopoRequest& request) const;
			BatchBindDeviceToEdgeInstanceWithDriverOutcome batchBindDeviceToEdgeInstanceWithDriver(const Model::BatchBindDeviceToEdgeInstanceWithDriverRequest &request)const;
			void batchBindDeviceToEdgeInstanceWithDriverAsync(const Model::BatchBindDeviceToEdgeInstanceWithDriverRequest& request, const BatchBindDeviceToEdgeInstanceWithDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchBindDeviceToEdgeInstanceWithDriverOutcomeCallable batchBindDeviceToEdgeInstanceWithDriverCallable(const Model::BatchBindDeviceToEdgeInstanceWithDriverRequest& request) const;
			BatchCheckDeviceNamesOutcome batchCheckDeviceNames(const Model::BatchCheckDeviceNamesRequest &request)const;
			void batchCheckDeviceNamesAsync(const Model::BatchCheckDeviceNamesRequest& request, const BatchCheckDeviceNamesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchCheckDeviceNamesOutcomeCallable batchCheckDeviceNamesCallable(const Model::BatchCheckDeviceNamesRequest& request) const;
			BatchClearEdgeInstanceDeviceConfigOutcome batchClearEdgeInstanceDeviceConfig(const Model::BatchClearEdgeInstanceDeviceConfigRequest &request)const;
			void batchClearEdgeInstanceDeviceConfigAsync(const Model::BatchClearEdgeInstanceDeviceConfigRequest& request, const BatchClearEdgeInstanceDeviceConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchClearEdgeInstanceDeviceConfigOutcomeCallable batchClearEdgeInstanceDeviceConfigCallable(const Model::BatchClearEdgeInstanceDeviceConfigRequest& request) const;
			BatchDeleteDeviceGroupRelationsOutcome batchDeleteDeviceGroupRelations(const Model::BatchDeleteDeviceGroupRelationsRequest &request)const;
			void batchDeleteDeviceGroupRelationsAsync(const Model::BatchDeleteDeviceGroupRelationsRequest& request, const BatchDeleteDeviceGroupRelationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchDeleteDeviceGroupRelationsOutcomeCallable batchDeleteDeviceGroupRelationsCallable(const Model::BatchDeleteDeviceGroupRelationsRequest& request) const;
			BatchDeleteEdgeInstanceChannelOutcome batchDeleteEdgeInstanceChannel(const Model::BatchDeleteEdgeInstanceChannelRequest &request)const;
			void batchDeleteEdgeInstanceChannelAsync(const Model::BatchDeleteEdgeInstanceChannelRequest& request, const BatchDeleteEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchDeleteEdgeInstanceChannelOutcomeCallable batchDeleteEdgeInstanceChannelCallable(const Model::BatchDeleteEdgeInstanceChannelRequest& request) const;
			BatchGetDeviceBindStatusOutcome batchGetDeviceBindStatus(const Model::BatchGetDeviceBindStatusRequest &request)const;
			void batchGetDeviceBindStatusAsync(const Model::BatchGetDeviceBindStatusRequest& request, const BatchGetDeviceBindStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetDeviceBindStatusOutcomeCallable batchGetDeviceBindStatusCallable(const Model::BatchGetDeviceBindStatusRequest& request) const;
			BatchGetDeviceStateOutcome batchGetDeviceState(const Model::BatchGetDeviceStateRequest &request)const;
			void batchGetDeviceStateAsync(const Model::BatchGetDeviceStateRequest& request, const BatchGetDeviceStateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetDeviceStateOutcomeCallable batchGetDeviceStateCallable(const Model::BatchGetDeviceStateRequest& request) const;
			BatchGetEdgeDriverOutcome batchGetEdgeDriver(const Model::BatchGetEdgeDriverRequest &request)const;
			void batchGetEdgeDriverAsync(const Model::BatchGetEdgeDriverRequest& request, const BatchGetEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetEdgeDriverOutcomeCallable batchGetEdgeDriverCallable(const Model::BatchGetEdgeDriverRequest& request) const;
			BatchGetEdgeInstanceChannelOutcome batchGetEdgeInstanceChannel(const Model::BatchGetEdgeInstanceChannelRequest &request)const;
			void batchGetEdgeInstanceChannelAsync(const Model::BatchGetEdgeInstanceChannelRequest& request, const BatchGetEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetEdgeInstanceChannelOutcomeCallable batchGetEdgeInstanceChannelCallable(const Model::BatchGetEdgeInstanceChannelRequest& request) const;
			BatchGetEdgeInstanceDeviceChannelOutcome batchGetEdgeInstanceDeviceChannel(const Model::BatchGetEdgeInstanceDeviceChannelRequest &request)const;
			void batchGetEdgeInstanceDeviceChannelAsync(const Model::BatchGetEdgeInstanceDeviceChannelRequest& request, const BatchGetEdgeInstanceDeviceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetEdgeInstanceDeviceChannelOutcomeCallable batchGetEdgeInstanceDeviceChannelCallable(const Model::BatchGetEdgeInstanceDeviceChannelRequest& request) const;
			BatchGetEdgeInstanceDeviceConfigOutcome batchGetEdgeInstanceDeviceConfig(const Model::BatchGetEdgeInstanceDeviceConfigRequest &request)const;
			void batchGetEdgeInstanceDeviceConfigAsync(const Model::BatchGetEdgeInstanceDeviceConfigRequest& request, const BatchGetEdgeInstanceDeviceConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetEdgeInstanceDeviceConfigOutcomeCallable batchGetEdgeInstanceDeviceConfigCallable(const Model::BatchGetEdgeInstanceDeviceConfigRequest& request) const;
			BatchGetEdgeInstanceDeviceDriverOutcome batchGetEdgeInstanceDeviceDriver(const Model::BatchGetEdgeInstanceDeviceDriverRequest &request)const;
			void batchGetEdgeInstanceDeviceDriverAsync(const Model::BatchGetEdgeInstanceDeviceDriverRequest& request, const BatchGetEdgeInstanceDeviceDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetEdgeInstanceDeviceDriverOutcomeCallable batchGetEdgeInstanceDeviceDriverCallable(const Model::BatchGetEdgeInstanceDeviceDriverRequest& request) const;
			BatchGetEdgeInstanceDriverConfigsOutcome batchGetEdgeInstanceDriverConfigs(const Model::BatchGetEdgeInstanceDriverConfigsRequest &request)const;
			void batchGetEdgeInstanceDriverConfigsAsync(const Model::BatchGetEdgeInstanceDriverConfigsRequest& request, const BatchGetEdgeInstanceDriverConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchGetEdgeInstanceDriverConfigsOutcomeCallable batchGetEdgeInstanceDriverConfigsCallable(const Model::BatchGetEdgeInstanceDriverConfigsRequest& request) const;
			BatchQueryDeviceDetailOutcome batchQueryDeviceDetail(const Model::BatchQueryDeviceDetailRequest &request)const;
			void batchQueryDeviceDetailAsync(const Model::BatchQueryDeviceDetailRequest& request, const BatchQueryDeviceDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchQueryDeviceDetailOutcomeCallable batchQueryDeviceDetailCallable(const Model::BatchQueryDeviceDetailRequest& request) const;
			BatchRegisterDeviceOutcome batchRegisterDevice(const Model::BatchRegisterDeviceRequest &request)const;
			void batchRegisterDeviceAsync(const Model::BatchRegisterDeviceRequest& request, const BatchRegisterDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchRegisterDeviceOutcomeCallable batchRegisterDeviceCallable(const Model::BatchRegisterDeviceRequest& request) const;
			BatchRegisterDeviceWithApplyIdOutcome batchRegisterDeviceWithApplyId(const Model::BatchRegisterDeviceWithApplyIdRequest &request)const;
			void batchRegisterDeviceWithApplyIdAsync(const Model::BatchRegisterDeviceWithApplyIdRequest& request, const BatchRegisterDeviceWithApplyIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchRegisterDeviceWithApplyIdOutcomeCallable batchRegisterDeviceWithApplyIdCallable(const Model::BatchRegisterDeviceWithApplyIdRequest& request) const;
			BatchSetEdgeInstanceDeviceChannelOutcome batchSetEdgeInstanceDeviceChannel(const Model::BatchSetEdgeInstanceDeviceChannelRequest &request)const;
			void batchSetEdgeInstanceDeviceChannelAsync(const Model::BatchSetEdgeInstanceDeviceChannelRequest& request, const BatchSetEdgeInstanceDeviceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchSetEdgeInstanceDeviceChannelOutcomeCallable batchSetEdgeInstanceDeviceChannelCallable(const Model::BatchSetEdgeInstanceDeviceChannelRequest& request) const;
			BatchSetEdgeInstanceDeviceConfigOutcome batchSetEdgeInstanceDeviceConfig(const Model::BatchSetEdgeInstanceDeviceConfigRequest &request)const;
			void batchSetEdgeInstanceDeviceConfigAsync(const Model::BatchSetEdgeInstanceDeviceConfigRequest& request, const BatchSetEdgeInstanceDeviceConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchSetEdgeInstanceDeviceConfigOutcomeCallable batchSetEdgeInstanceDeviceConfigCallable(const Model::BatchSetEdgeInstanceDeviceConfigRequest& request) const;
			BatchUnbindDeviceFromEdgeInstanceOutcome batchUnbindDeviceFromEdgeInstance(const Model::BatchUnbindDeviceFromEdgeInstanceRequest &request)const;
			void batchUnbindDeviceFromEdgeInstanceAsync(const Model::BatchUnbindDeviceFromEdgeInstanceRequest& request, const BatchUnbindDeviceFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchUnbindDeviceFromEdgeInstanceOutcomeCallable batchUnbindDeviceFromEdgeInstanceCallable(const Model::BatchUnbindDeviceFromEdgeInstanceRequest& request) const;
			BatchUpdateDeviceNicknameOutcome batchUpdateDeviceNickname(const Model::BatchUpdateDeviceNicknameRequest &request)const;
			void batchUpdateDeviceNicknameAsync(const Model::BatchUpdateDeviceNicknameRequest& request, const BatchUpdateDeviceNicknameAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchUpdateDeviceNicknameOutcomeCallable batchUpdateDeviceNicknameCallable(const Model::BatchUpdateDeviceNicknameRequest& request) const;
			BindApplicationToEdgeInstanceOutcome bindApplicationToEdgeInstance(const Model::BindApplicationToEdgeInstanceRequest &request)const;
			void bindApplicationToEdgeInstanceAsync(const Model::BindApplicationToEdgeInstanceRequest& request, const BindApplicationToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BindApplicationToEdgeInstanceOutcomeCallable bindApplicationToEdgeInstanceCallable(const Model::BindApplicationToEdgeInstanceRequest& request) const;
			BindDriverToEdgeInstanceOutcome bindDriverToEdgeInstance(const Model::BindDriverToEdgeInstanceRequest &request)const;
			void bindDriverToEdgeInstanceAsync(const Model::BindDriverToEdgeInstanceRequest& request, const BindDriverToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BindDriverToEdgeInstanceOutcomeCallable bindDriverToEdgeInstanceCallable(const Model::BindDriverToEdgeInstanceRequest& request) const;
			BindGatewayToEdgeInstanceOutcome bindGatewayToEdgeInstance(const Model::BindGatewayToEdgeInstanceRequest &request)const;
			void bindGatewayToEdgeInstanceAsync(const Model::BindGatewayToEdgeInstanceRequest& request, const BindGatewayToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BindGatewayToEdgeInstanceOutcomeCallable bindGatewayToEdgeInstanceCallable(const Model::BindGatewayToEdgeInstanceRequest& request) const;
			BindSceneRuleToEdgeInstanceOutcome bindSceneRuleToEdgeInstance(const Model::BindSceneRuleToEdgeInstanceRequest &request)const;
			void bindSceneRuleToEdgeInstanceAsync(const Model::BindSceneRuleToEdgeInstanceRequest& request, const BindSceneRuleToEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BindSceneRuleToEdgeInstanceOutcomeCallable bindSceneRuleToEdgeInstanceCallable(const Model::BindSceneRuleToEdgeInstanceRequest& request) const;
			CancelOTAStrategyByJobOutcome cancelOTAStrategyByJob(const Model::CancelOTAStrategyByJobRequest &request)const;
			void cancelOTAStrategyByJobAsync(const Model::CancelOTAStrategyByJobRequest& request, const CancelOTAStrategyByJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CancelOTAStrategyByJobOutcomeCallable cancelOTAStrategyByJobCallable(const Model::CancelOTAStrategyByJobRequest& request) const;
			CancelOTATaskByDeviceOutcome cancelOTATaskByDevice(const Model::CancelOTATaskByDeviceRequest &request)const;
			void cancelOTATaskByDeviceAsync(const Model::CancelOTATaskByDeviceRequest& request, const CancelOTATaskByDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CancelOTATaskByDeviceOutcomeCallable cancelOTATaskByDeviceCallable(const Model::CancelOTATaskByDeviceRequest& request) const;
			CancelOTATaskByJobOutcome cancelOTATaskByJob(const Model::CancelOTATaskByJobRequest &request)const;
			void cancelOTATaskByJobAsync(const Model::CancelOTATaskByJobRequest& request, const CancelOTATaskByJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CancelOTATaskByJobOutcomeCallable cancelOTATaskByJobCallable(const Model::CancelOTATaskByJobRequest& request) const;
			ClearEdgeInstanceDriverConfigsOutcome clearEdgeInstanceDriverConfigs(const Model::ClearEdgeInstanceDriverConfigsRequest &request)const;
			void clearEdgeInstanceDriverConfigsAsync(const Model::ClearEdgeInstanceDriverConfigsRequest& request, const ClearEdgeInstanceDriverConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ClearEdgeInstanceDriverConfigsOutcomeCallable clearEdgeInstanceDriverConfigsCallable(const Model::ClearEdgeInstanceDriverConfigsRequest& request) const;
			CloseEdgeInstanceDeploymentOutcome closeEdgeInstanceDeployment(const Model::CloseEdgeInstanceDeploymentRequest &request)const;
			void closeEdgeInstanceDeploymentAsync(const Model::CloseEdgeInstanceDeploymentRequest& request, const CloseEdgeInstanceDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CloseEdgeInstanceDeploymentOutcomeCallable closeEdgeInstanceDeploymentCallable(const Model::CloseEdgeInstanceDeploymentRequest& request) const;
			CopyThingModelOutcome copyThingModel(const Model::CopyThingModelRequest &request)const;
			void copyThingModelAsync(const Model::CopyThingModelRequest& request, const CopyThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CopyThingModelOutcomeCallable copyThingModelCallable(const Model::CopyThingModelRequest& request) const;
			CreateConsumerGroupOutcome createConsumerGroup(const Model::CreateConsumerGroupRequest &request)const;
			void createConsumerGroupAsync(const Model::CreateConsumerGroupRequest& request, const CreateConsumerGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateConsumerGroupOutcomeCallable createConsumerGroupCallable(const Model::CreateConsumerGroupRequest& request) const;
			CreateConsumerGroupSubscribeRelationOutcome createConsumerGroupSubscribeRelation(const Model::CreateConsumerGroupSubscribeRelationRequest &request)const;
			void createConsumerGroupSubscribeRelationAsync(const Model::CreateConsumerGroupSubscribeRelationRequest& request, const CreateConsumerGroupSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateConsumerGroupSubscribeRelationOutcomeCallable createConsumerGroupSubscribeRelationCallable(const Model::CreateConsumerGroupSubscribeRelationRequest& request) const;
			CreateDataAPIServiceOutcome createDataAPIService(const Model::CreateDataAPIServiceRequest &request)const;
			void createDataAPIServiceAsync(const Model::CreateDataAPIServiceRequest& request, const CreateDataAPIServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateDataAPIServiceOutcomeCallable createDataAPIServiceCallable(const Model::CreateDataAPIServiceRequest& request) const;
			CreateDeviceGroupOutcome createDeviceGroup(const Model::CreateDeviceGroupRequest &request)const;
			void createDeviceGroupAsync(const Model::CreateDeviceGroupRequest& request, const CreateDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateDeviceGroupOutcomeCallable createDeviceGroupCallable(const Model::CreateDeviceGroupRequest& request) const;
			CreateEdgeDriverOutcome createEdgeDriver(const Model::CreateEdgeDriverRequest &request)const;
			void createEdgeDriverAsync(const Model::CreateEdgeDriverRequest& request, const CreateEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEdgeDriverOutcomeCallable createEdgeDriverCallable(const Model::CreateEdgeDriverRequest& request) const;
			CreateEdgeDriverVersionOutcome createEdgeDriverVersion(const Model::CreateEdgeDriverVersionRequest &request)const;
			void createEdgeDriverVersionAsync(const Model::CreateEdgeDriverVersionRequest& request, const CreateEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEdgeDriverVersionOutcomeCallable createEdgeDriverVersionCallable(const Model::CreateEdgeDriverVersionRequest& request) const;
			CreateEdgeInstanceOutcome createEdgeInstance(const Model::CreateEdgeInstanceRequest &request)const;
			void createEdgeInstanceAsync(const Model::CreateEdgeInstanceRequest& request, const CreateEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEdgeInstanceOutcomeCallable createEdgeInstanceCallable(const Model::CreateEdgeInstanceRequest& request) const;
			CreateEdgeInstanceChannelOutcome createEdgeInstanceChannel(const Model::CreateEdgeInstanceChannelRequest &request)const;
			void createEdgeInstanceChannelAsync(const Model::CreateEdgeInstanceChannelRequest& request, const CreateEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEdgeInstanceChannelOutcomeCallable createEdgeInstanceChannelCallable(const Model::CreateEdgeInstanceChannelRequest& request) const;
			CreateEdgeInstanceDeploymentOutcome createEdgeInstanceDeployment(const Model::CreateEdgeInstanceDeploymentRequest &request)const;
			void createEdgeInstanceDeploymentAsync(const Model::CreateEdgeInstanceDeploymentRequest& request, const CreateEdgeInstanceDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEdgeInstanceDeploymentOutcomeCallable createEdgeInstanceDeploymentCallable(const Model::CreateEdgeInstanceDeploymentRequest& request) const;
			CreateEdgeOssPreSignedAddressOutcome createEdgeOssPreSignedAddress(const Model::CreateEdgeOssPreSignedAddressRequest &request)const;
			void createEdgeOssPreSignedAddressAsync(const Model::CreateEdgeOssPreSignedAddressRequest& request, const CreateEdgeOssPreSignedAddressAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEdgeOssPreSignedAddressOutcomeCallable createEdgeOssPreSignedAddressCallable(const Model::CreateEdgeOssPreSignedAddressRequest& request) const;
			CreateLoRaNodesTaskOutcome createLoRaNodesTask(const Model::CreateLoRaNodesTaskRequest &request)const;
			void createLoRaNodesTaskAsync(const Model::CreateLoRaNodesTaskRequest& request, const CreateLoRaNodesTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateLoRaNodesTaskOutcomeCallable createLoRaNodesTaskCallable(const Model::CreateLoRaNodesTaskRequest& request) const;
			CreateOTADynamicUpgradeJobOutcome createOTADynamicUpgradeJob(const Model::CreateOTADynamicUpgradeJobRequest &request)const;
			void createOTADynamicUpgradeJobAsync(const Model::CreateOTADynamicUpgradeJobRequest& request, const CreateOTADynamicUpgradeJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateOTADynamicUpgradeJobOutcomeCallable createOTADynamicUpgradeJobCallable(const Model::CreateOTADynamicUpgradeJobRequest& request) const;
			CreateOTAFirmwareOutcome createOTAFirmware(const Model::CreateOTAFirmwareRequest &request)const;
			void createOTAFirmwareAsync(const Model::CreateOTAFirmwareRequest& request, const CreateOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateOTAFirmwareOutcomeCallable createOTAFirmwareCallable(const Model::CreateOTAFirmwareRequest& request) const;
			CreateOTAStaticUpgradeJobOutcome createOTAStaticUpgradeJob(const Model::CreateOTAStaticUpgradeJobRequest &request)const;
			void createOTAStaticUpgradeJobAsync(const Model::CreateOTAStaticUpgradeJobRequest& request, const CreateOTAStaticUpgradeJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateOTAStaticUpgradeJobOutcomeCallable createOTAStaticUpgradeJobCallable(const Model::CreateOTAStaticUpgradeJobRequest& request) const;
			CreateOTAVerifyJobOutcome createOTAVerifyJob(const Model::CreateOTAVerifyJobRequest &request)const;
			void createOTAVerifyJobAsync(const Model::CreateOTAVerifyJobRequest& request, const CreateOTAVerifyJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateOTAVerifyJobOutcomeCallable createOTAVerifyJobCallable(const Model::CreateOTAVerifyJobRequest& request) const;
			CreateProductOutcome createProduct(const Model::CreateProductRequest &request)const;
			void createProductAsync(const Model::CreateProductRequest& request, const CreateProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateProductOutcomeCallable createProductCallable(const Model::CreateProductRequest& request) const;
			CreateProductTagsOutcome createProductTags(const Model::CreateProductTagsRequest &request)const;
			void createProductTagsAsync(const Model::CreateProductTagsRequest& request, const CreateProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateProductTagsOutcomeCallable createProductTagsCallable(const Model::CreateProductTagsRequest& request) const;
			CreateProductTopicOutcome createProductTopic(const Model::CreateProductTopicRequest &request)const;
			void createProductTopicAsync(const Model::CreateProductTopicRequest& request, const CreateProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateProductTopicOutcomeCallable createProductTopicCallable(const Model::CreateProductTopicRequest& request) const;
			CreateRuleOutcome createRule(const Model::CreateRuleRequest &request)const;
			void createRuleAsync(const Model::CreateRuleRequest& request, const CreateRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRuleOutcomeCallable createRuleCallable(const Model::CreateRuleRequest& request) const;
			CreateRuleActionOutcome createRuleAction(const Model::CreateRuleActionRequest &request)const;
			void createRuleActionAsync(const Model::CreateRuleActionRequest& request, const CreateRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRuleActionOutcomeCallable createRuleActionCallable(const Model::CreateRuleActionRequest& request) const;
			CreateSceneRuleOutcome createSceneRule(const Model::CreateSceneRuleRequest &request)const;
			void createSceneRuleAsync(const Model::CreateSceneRuleRequest& request, const CreateSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateSceneRuleOutcomeCallable createSceneRuleCallable(const Model::CreateSceneRuleRequest& request) const;
			CreateSubscribeRelationOutcome createSubscribeRelation(const Model::CreateSubscribeRelationRequest &request)const;
			void createSubscribeRelationAsync(const Model::CreateSubscribeRelationRequest& request, const CreateSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateSubscribeRelationOutcomeCallable createSubscribeRelationCallable(const Model::CreateSubscribeRelationRequest& request) const;
			CreateThingModelOutcome createThingModel(const Model::CreateThingModelRequest &request)const;
			void createThingModelAsync(const Model::CreateThingModelRequest& request, const CreateThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateThingModelOutcomeCallable createThingModelCallable(const Model::CreateThingModelRequest& request) const;
			CreateTopicRouteTableOutcome createTopicRouteTable(const Model::CreateTopicRouteTableRequest &request)const;
			void createTopicRouteTableAsync(const Model::CreateTopicRouteTableRequest& request, const CreateTopicRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateTopicRouteTableOutcomeCallable createTopicRouteTableCallable(const Model::CreateTopicRouteTableRequest& request) const;
			DeleteConsumerGroupOutcome deleteConsumerGroup(const Model::DeleteConsumerGroupRequest &request)const;
			void deleteConsumerGroupAsync(const Model::DeleteConsumerGroupRequest& request, const DeleteConsumerGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteConsumerGroupOutcomeCallable deleteConsumerGroupCallable(const Model::DeleteConsumerGroupRequest& request) const;
			DeleteConsumerGroupSubscribeRelationOutcome deleteConsumerGroupSubscribeRelation(const Model::DeleteConsumerGroupSubscribeRelationRequest &request)const;
			void deleteConsumerGroupSubscribeRelationAsync(const Model::DeleteConsumerGroupSubscribeRelationRequest& request, const DeleteConsumerGroupSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteConsumerGroupSubscribeRelationOutcomeCallable deleteConsumerGroupSubscribeRelationCallable(const Model::DeleteConsumerGroupSubscribeRelationRequest& request) const;
			DeleteDeviceOutcome deleteDevice(const Model::DeleteDeviceRequest &request)const;
			void deleteDeviceAsync(const Model::DeleteDeviceRequest& request, const DeleteDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteDeviceOutcomeCallable deleteDeviceCallable(const Model::DeleteDeviceRequest& request) const;
			DeleteDeviceFileOutcome deleteDeviceFile(const Model::DeleteDeviceFileRequest &request)const;
			void deleteDeviceFileAsync(const Model::DeleteDeviceFileRequest& request, const DeleteDeviceFileAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteDeviceFileOutcomeCallable deleteDeviceFileCallable(const Model::DeleteDeviceFileRequest& request) const;
			DeleteDeviceGroupOutcome deleteDeviceGroup(const Model::DeleteDeviceGroupRequest &request)const;
			void deleteDeviceGroupAsync(const Model::DeleteDeviceGroupRequest& request, const DeleteDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteDeviceGroupOutcomeCallable deleteDeviceGroupCallable(const Model::DeleteDeviceGroupRequest& request) const;
			DeleteDevicePropOutcome deleteDeviceProp(const Model::DeleteDevicePropRequest &request)const;
			void deleteDevicePropAsync(const Model::DeleteDevicePropRequest& request, const DeleteDevicePropAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteDevicePropOutcomeCallable deleteDevicePropCallable(const Model::DeleteDevicePropRequest& request) const;
			DeleteEdgeDriverOutcome deleteEdgeDriver(const Model::DeleteEdgeDriverRequest &request)const;
			void deleteEdgeDriverAsync(const Model::DeleteEdgeDriverRequest& request, const DeleteEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteEdgeDriverOutcomeCallable deleteEdgeDriverCallable(const Model::DeleteEdgeDriverRequest& request) const;
			DeleteEdgeDriverVersionOutcome deleteEdgeDriverVersion(const Model::DeleteEdgeDriverVersionRequest &request)const;
			void deleteEdgeDriverVersionAsync(const Model::DeleteEdgeDriverVersionRequest& request, const DeleteEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteEdgeDriverVersionOutcomeCallable deleteEdgeDriverVersionCallable(const Model::DeleteEdgeDriverVersionRequest& request) const;
			DeleteEdgeInstanceOutcome deleteEdgeInstance(const Model::DeleteEdgeInstanceRequest &request)const;
			void deleteEdgeInstanceAsync(const Model::DeleteEdgeInstanceRequest& request, const DeleteEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteEdgeInstanceOutcomeCallable deleteEdgeInstanceCallable(const Model::DeleteEdgeInstanceRequest& request) const;
			DeleteOTAFirmwareOutcome deleteOTAFirmware(const Model::DeleteOTAFirmwareRequest &request)const;
			void deleteOTAFirmwareAsync(const Model::DeleteOTAFirmwareRequest& request, const DeleteOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteOTAFirmwareOutcomeCallable deleteOTAFirmwareCallable(const Model::DeleteOTAFirmwareRequest& request) const;
			DeleteProductOutcome deleteProduct(const Model::DeleteProductRequest &request)const;
			void deleteProductAsync(const Model::DeleteProductRequest& request, const DeleteProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteProductOutcomeCallable deleteProductCallable(const Model::DeleteProductRequest& request) const;
			DeleteProductTagsOutcome deleteProductTags(const Model::DeleteProductTagsRequest &request)const;
			void deleteProductTagsAsync(const Model::DeleteProductTagsRequest& request, const DeleteProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteProductTagsOutcomeCallable deleteProductTagsCallable(const Model::DeleteProductTagsRequest& request) const;
			DeleteProductTopicOutcome deleteProductTopic(const Model::DeleteProductTopicRequest &request)const;
			void deleteProductTopicAsync(const Model::DeleteProductTopicRequest& request, const DeleteProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteProductTopicOutcomeCallable deleteProductTopicCallable(const Model::DeleteProductTopicRequest& request) const;
			DeleteRuleOutcome deleteRule(const Model::DeleteRuleRequest &request)const;
			void deleteRuleAsync(const Model::DeleteRuleRequest& request, const DeleteRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRuleOutcomeCallable deleteRuleCallable(const Model::DeleteRuleRequest& request) const;
			DeleteRuleActionOutcome deleteRuleAction(const Model::DeleteRuleActionRequest &request)const;
			void deleteRuleActionAsync(const Model::DeleteRuleActionRequest& request, const DeleteRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRuleActionOutcomeCallable deleteRuleActionCallable(const Model::DeleteRuleActionRequest& request) const;
			DeleteSceneRuleOutcome deleteSceneRule(const Model::DeleteSceneRuleRequest &request)const;
			void deleteSceneRuleAsync(const Model::DeleteSceneRuleRequest& request, const DeleteSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteSceneRuleOutcomeCallable deleteSceneRuleCallable(const Model::DeleteSceneRuleRequest& request) const;
			DeleteSubscribeRelationOutcome deleteSubscribeRelation(const Model::DeleteSubscribeRelationRequest &request)const;
			void deleteSubscribeRelationAsync(const Model::DeleteSubscribeRelationRequest& request, const DeleteSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteSubscribeRelationOutcomeCallable deleteSubscribeRelationCallable(const Model::DeleteSubscribeRelationRequest& request) const;
			DeleteThingModelOutcome deleteThingModel(const Model::DeleteThingModelRequest &request)const;
			void deleteThingModelAsync(const Model::DeleteThingModelRequest& request, const DeleteThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteThingModelOutcomeCallable deleteThingModelCallable(const Model::DeleteThingModelRequest& request) const;
			DeleteTopicRouteTableOutcome deleteTopicRouteTable(const Model::DeleteTopicRouteTableRequest &request)const;
			void deleteTopicRouteTableAsync(const Model::DeleteTopicRouteTableRequest& request, const DeleteTopicRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteTopicRouteTableOutcomeCallable deleteTopicRouteTableCallable(const Model::DeleteTopicRouteTableRequest& request) const;
			DisableSceneRuleOutcome disableSceneRule(const Model::DisableSceneRuleRequest &request)const;
			void disableSceneRuleAsync(const Model::DisableSceneRuleRequest& request, const DisableSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DisableSceneRuleOutcomeCallable disableSceneRuleCallable(const Model::DisableSceneRuleRequest& request) const;
			DisableThingOutcome disableThing(const Model::DisableThingRequest &request)const;
			void disableThingAsync(const Model::DisableThingRequest& request, const DisableThingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DisableThingOutcomeCallable disableThingCallable(const Model::DisableThingRequest& request) const;
			EnableSceneRuleOutcome enableSceneRule(const Model::EnableSceneRuleRequest &request)const;
			void enableSceneRuleAsync(const Model::EnableSceneRuleRequest& request, const EnableSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnableSceneRuleOutcomeCallable enableSceneRuleCallable(const Model::EnableSceneRuleRequest& request) const;
			EnableThingOutcome enableThing(const Model::EnableThingRequest &request)const;
			void enableThingAsync(const Model::EnableThingRequest& request, const EnableThingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnableThingOutcomeCallable enableThingCallable(const Model::EnableThingRequest& request) const;
			GenerateOTAUploadURLOutcome generateOTAUploadURL(const Model::GenerateOTAUploadURLRequest &request)const;
			void generateOTAUploadURLAsync(const Model::GenerateOTAUploadURLRequest& request, const GenerateOTAUploadURLAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GenerateOTAUploadURLOutcomeCallable generateOTAUploadURLCallable(const Model::GenerateOTAUploadURLRequest& request) const;
			GetDataAPIServiceDetailOutcome getDataAPIServiceDetail(const Model::GetDataAPIServiceDetailRequest &request)const;
			void getDataAPIServiceDetailAsync(const Model::GetDataAPIServiceDetailRequest& request, const GetDataAPIServiceDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDataAPIServiceDetailOutcomeCallable getDataAPIServiceDetailCallable(const Model::GetDataAPIServiceDetailRequest& request) const;
			GetDeviceShadowOutcome getDeviceShadow(const Model::GetDeviceShadowRequest &request)const;
			void getDeviceShadowAsync(const Model::GetDeviceShadowRequest& request, const GetDeviceShadowAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDeviceShadowOutcomeCallable getDeviceShadowCallable(const Model::GetDeviceShadowRequest& request) const;
			GetDeviceStatusOutcome getDeviceStatus(const Model::GetDeviceStatusRequest &request)const;
			void getDeviceStatusAsync(const Model::GetDeviceStatusRequest& request, const GetDeviceStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDeviceStatusOutcomeCallable getDeviceStatusCallable(const Model::GetDeviceStatusRequest& request) const;
			GetEdgeDriverVersionOutcome getEdgeDriverVersion(const Model::GetEdgeDriverVersionRequest &request)const;
			void getEdgeDriverVersionAsync(const Model::GetEdgeDriverVersionRequest& request, const GetEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetEdgeDriverVersionOutcomeCallable getEdgeDriverVersionCallable(const Model::GetEdgeDriverVersionRequest& request) const;
			GetEdgeInstanceOutcome getEdgeInstance(const Model::GetEdgeInstanceRequest &request)const;
			void getEdgeInstanceAsync(const Model::GetEdgeInstanceRequest& request, const GetEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetEdgeInstanceOutcomeCallable getEdgeInstanceCallable(const Model::GetEdgeInstanceRequest& request) const;
			GetEdgeInstanceDeploymentOutcome getEdgeInstanceDeployment(const Model::GetEdgeInstanceDeploymentRequest &request)const;
			void getEdgeInstanceDeploymentAsync(const Model::GetEdgeInstanceDeploymentRequest& request, const GetEdgeInstanceDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetEdgeInstanceDeploymentOutcomeCallable getEdgeInstanceDeploymentCallable(const Model::GetEdgeInstanceDeploymentRequest& request) const;
			GetGatewayBySubDeviceOutcome getGatewayBySubDevice(const Model::GetGatewayBySubDeviceRequest &request)const;
			void getGatewayBySubDeviceAsync(const Model::GetGatewayBySubDeviceRequest& request, const GetGatewayBySubDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetGatewayBySubDeviceOutcomeCallable getGatewayBySubDeviceCallable(const Model::GetGatewayBySubDeviceRequest& request) const;
			GetLoraNodesTaskOutcome getLoraNodesTask(const Model::GetLoraNodesTaskRequest &request)const;
			void getLoraNodesTaskAsync(const Model::GetLoraNodesTaskRequest& request, const GetLoraNodesTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetLoraNodesTaskOutcomeCallable getLoraNodesTaskCallable(const Model::GetLoraNodesTaskRequest& request) const;
			GetNodesAddingTaskOutcome getNodesAddingTask(const Model::GetNodesAddingTaskRequest &request)const;
			void getNodesAddingTaskAsync(const Model::GetNodesAddingTaskRequest& request, const GetNodesAddingTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetNodesAddingTaskOutcomeCallable getNodesAddingTaskCallable(const Model::GetNodesAddingTaskRequest& request) const;
			GetRuleOutcome getRule(const Model::GetRuleRequest &request)const;
			void getRuleAsync(const Model::GetRuleRequest& request, const GetRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRuleOutcomeCallable getRuleCallable(const Model::GetRuleRequest& request) const;
			GetRuleActionOutcome getRuleAction(const Model::GetRuleActionRequest &request)const;
			void getRuleActionAsync(const Model::GetRuleActionRequest& request, const GetRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetRuleActionOutcomeCallable getRuleActionCallable(const Model::GetRuleActionRequest& request) const;
			GetSceneRuleOutcome getSceneRule(const Model::GetSceneRuleRequest &request)const;
			void getSceneRuleAsync(const Model::GetSceneRuleRequest& request, const GetSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetSceneRuleOutcomeCallable getSceneRuleCallable(const Model::GetSceneRuleRequest& request) const;
			GetThingModelTslOutcome getThingModelTsl(const Model::GetThingModelTslRequest &request)const;
			void getThingModelTslAsync(const Model::GetThingModelTslRequest& request, const GetThingModelTslAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetThingModelTslOutcomeCallable getThingModelTslCallable(const Model::GetThingModelTslRequest& request) const;
			GetThingTemplateOutcome getThingTemplate(const Model::GetThingTemplateRequest &request)const;
			void getThingTemplateAsync(const Model::GetThingTemplateRequest& request, const GetThingTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetThingTemplateOutcomeCallable getThingTemplateCallable(const Model::GetThingTemplateRequest& request) const;
			GetThingTopoOutcome getThingTopo(const Model::GetThingTopoRequest &request)const;
			void getThingTopoAsync(const Model::GetThingTopoRequest& request, const GetThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetThingTopoOutcomeCallable getThingTopoCallable(const Model::GetThingTopoRequest& request) const;
			ImportThingModelTslOutcome importThingModelTsl(const Model::ImportThingModelTslRequest &request)const;
			void importThingModelTslAsync(const Model::ImportThingModelTslRequest& request, const ImportThingModelTslAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ImportThingModelTslOutcomeCallable importThingModelTslCallable(const Model::ImportThingModelTslRequest& request) const;
			InvokeDataAPIServiceOutcome invokeDataAPIService(const Model::InvokeDataAPIServiceRequest &request)const;
			void invokeDataAPIServiceAsync(const Model::InvokeDataAPIServiceRequest& request, const InvokeDataAPIServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			InvokeDataAPIServiceOutcomeCallable invokeDataAPIServiceCallable(const Model::InvokeDataAPIServiceRequest& request) const;
			InvokeThingServiceOutcome invokeThingService(const Model::InvokeThingServiceRequest &request)const;
			void invokeThingServiceAsync(const Model::InvokeThingServiceRequest& request, const InvokeThingServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			InvokeThingServiceOutcomeCallable invokeThingServiceCallable(const Model::InvokeThingServiceRequest& request) const;
			InvokeThingsServiceOutcome invokeThingsService(const Model::InvokeThingsServiceRequest &request)const;
			void invokeThingsServiceAsync(const Model::InvokeThingsServiceRequest& request, const InvokeThingsServiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			InvokeThingsServiceOutcomeCallable invokeThingsServiceCallable(const Model::InvokeThingsServiceRequest& request) const;
			ListOTAFirmwareOutcome listOTAFirmware(const Model::ListOTAFirmwareRequest &request)const;
			void listOTAFirmwareAsync(const Model::ListOTAFirmwareRequest& request, const ListOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListOTAFirmwareOutcomeCallable listOTAFirmwareCallable(const Model::ListOTAFirmwareRequest& request) const;
			ListOTAJobByDeviceOutcome listOTAJobByDevice(const Model::ListOTAJobByDeviceRequest &request)const;
			void listOTAJobByDeviceAsync(const Model::ListOTAJobByDeviceRequest& request, const ListOTAJobByDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListOTAJobByDeviceOutcomeCallable listOTAJobByDeviceCallable(const Model::ListOTAJobByDeviceRequest& request) const;
			ListOTAJobByFirmwareOutcome listOTAJobByFirmware(const Model::ListOTAJobByFirmwareRequest &request)const;
			void listOTAJobByFirmwareAsync(const Model::ListOTAJobByFirmwareRequest& request, const ListOTAJobByFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListOTAJobByFirmwareOutcomeCallable listOTAJobByFirmwareCallable(const Model::ListOTAJobByFirmwareRequest& request) const;
			ListOTATaskByJobOutcome listOTATaskByJob(const Model::ListOTATaskByJobRequest &request)const;
			void listOTATaskByJobAsync(const Model::ListOTATaskByJobRequest& request, const ListOTATaskByJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListOTATaskByJobOutcomeCallable listOTATaskByJobCallable(const Model::ListOTATaskByJobRequest& request) const;
			ListProductByTagsOutcome listProductByTags(const Model::ListProductByTagsRequest &request)const;
			void listProductByTagsAsync(const Model::ListProductByTagsRequest& request, const ListProductByTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListProductByTagsOutcomeCallable listProductByTagsCallable(const Model::ListProductByTagsRequest& request) const;
			ListProductTagsOutcome listProductTags(const Model::ListProductTagsRequest &request)const;
			void listProductTagsAsync(const Model::ListProductTagsRequest& request, const ListProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListProductTagsOutcomeCallable listProductTagsCallable(const Model::ListProductTagsRequest& request) const;
			ListRuleOutcome listRule(const Model::ListRuleRequest &request)const;
			void listRuleAsync(const Model::ListRuleRequest& request, const ListRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListRuleOutcomeCallable listRuleCallable(const Model::ListRuleRequest& request) const;
			ListRuleActionsOutcome listRuleActions(const Model::ListRuleActionsRequest &request)const;
			void listRuleActionsAsync(const Model::ListRuleActionsRequest& request, const ListRuleActionsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListRuleActionsOutcomeCallable listRuleActionsCallable(const Model::ListRuleActionsRequest& request) const;
			ListThingModelVersionOutcome listThingModelVersion(const Model::ListThingModelVersionRequest &request)const;
			void listThingModelVersionAsync(const Model::ListThingModelVersionRequest& request, const ListThingModelVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListThingModelVersionOutcomeCallable listThingModelVersionCallable(const Model::ListThingModelVersionRequest& request) const;
			ListThingTemplatesOutcome listThingTemplates(const Model::ListThingTemplatesRequest &request)const;
			void listThingTemplatesAsync(const Model::ListThingTemplatesRequest& request, const ListThingTemplatesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListThingTemplatesOutcomeCallable listThingTemplatesCallable(const Model::ListThingTemplatesRequest& request) const;
			NotifyAddThingTopoOutcome notifyAddThingTopo(const Model::NotifyAddThingTopoRequest &request)const;
			void notifyAddThingTopoAsync(const Model::NotifyAddThingTopoRequest& request, const NotifyAddThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			NotifyAddThingTopoOutcomeCallable notifyAddThingTopoCallable(const Model::NotifyAddThingTopoRequest& request) const;
			PubOutcome pub(const Model::PubRequest &request)const;
			void pubAsync(const Model::PubRequest& request, const PubAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PubOutcomeCallable pubCallable(const Model::PubRequest& request) const;
			PubBroadcastOutcome pubBroadcast(const Model::PubBroadcastRequest &request)const;
			void pubBroadcastAsync(const Model::PubBroadcastRequest& request, const PubBroadcastAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PubBroadcastOutcomeCallable pubBroadcastCallable(const Model::PubBroadcastRequest& request) const;
			PublishThingModelOutcome publishThingModel(const Model::PublishThingModelRequest &request)const;
			void publishThingModelAsync(const Model::PublishThingModelRequest& request, const PublishThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			PublishThingModelOutcomeCallable publishThingModelCallable(const Model::PublishThingModelRequest& request) const;
			QueryAppDeviceListOutcome queryAppDeviceList(const Model::QueryAppDeviceListRequest &request)const;
			void queryAppDeviceListAsync(const Model::QueryAppDeviceListRequest& request, const QueryAppDeviceListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryAppDeviceListOutcomeCallable queryAppDeviceListCallable(const Model::QueryAppDeviceListRequest& request) const;
			QueryBatchRegisterDeviceStatusOutcome queryBatchRegisterDeviceStatus(const Model::QueryBatchRegisterDeviceStatusRequest &request)const;
			void queryBatchRegisterDeviceStatusAsync(const Model::QueryBatchRegisterDeviceStatusRequest& request, const QueryBatchRegisterDeviceStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryBatchRegisterDeviceStatusOutcomeCallable queryBatchRegisterDeviceStatusCallable(const Model::QueryBatchRegisterDeviceStatusRequest& request) const;
			QueryCertUrlByApplyIdOutcome queryCertUrlByApplyId(const Model::QueryCertUrlByApplyIdRequest &request)const;
			void queryCertUrlByApplyIdAsync(const Model::QueryCertUrlByApplyIdRequest& request, const QueryCertUrlByApplyIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryCertUrlByApplyIdOutcomeCallable queryCertUrlByApplyIdCallable(const Model::QueryCertUrlByApplyIdRequest& request) const;
			QueryConsumerGroupByGroupIdOutcome queryConsumerGroupByGroupId(const Model::QueryConsumerGroupByGroupIdRequest &request)const;
			void queryConsumerGroupByGroupIdAsync(const Model::QueryConsumerGroupByGroupIdRequest& request, const QueryConsumerGroupByGroupIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryConsumerGroupByGroupIdOutcomeCallable queryConsumerGroupByGroupIdCallable(const Model::QueryConsumerGroupByGroupIdRequest& request) const;
			QueryConsumerGroupListOutcome queryConsumerGroupList(const Model::QueryConsumerGroupListRequest &request)const;
			void queryConsumerGroupListAsync(const Model::QueryConsumerGroupListRequest& request, const QueryConsumerGroupListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryConsumerGroupListOutcomeCallable queryConsumerGroupListCallable(const Model::QueryConsumerGroupListRequest& request) const;
			QueryConsumerGroupStatusOutcome queryConsumerGroupStatus(const Model::QueryConsumerGroupStatusRequest &request)const;
			void queryConsumerGroupStatusAsync(const Model::QueryConsumerGroupStatusRequest& request, const QueryConsumerGroupStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryConsumerGroupStatusOutcomeCallable queryConsumerGroupStatusCallable(const Model::QueryConsumerGroupStatusRequest& request) const;
			QueryDetailSceneRuleLogOutcome queryDetailSceneRuleLog(const Model::QueryDetailSceneRuleLogRequest &request)const;
			void queryDetailSceneRuleLogAsync(const Model::QueryDetailSceneRuleLogRequest& request, const QueryDetailSceneRuleLogAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDetailSceneRuleLogOutcomeCallable queryDetailSceneRuleLogCallable(const Model::QueryDetailSceneRuleLogRequest& request) const;
			QueryDeviceOutcome queryDevice(const Model::QueryDeviceRequest &request)const;
			void queryDeviceAsync(const Model::QueryDeviceRequest& request, const QueryDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceOutcomeCallable queryDeviceCallable(const Model::QueryDeviceRequest& request) const;
			QueryDeviceByStatusOutcome queryDeviceByStatus(const Model::QueryDeviceByStatusRequest &request)const;
			void queryDeviceByStatusAsync(const Model::QueryDeviceByStatusRequest& request, const QueryDeviceByStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceByStatusOutcomeCallable queryDeviceByStatusCallable(const Model::QueryDeviceByStatusRequest& request) const;
			QueryDeviceByTagsOutcome queryDeviceByTags(const Model::QueryDeviceByTagsRequest &request)const;
			void queryDeviceByTagsAsync(const Model::QueryDeviceByTagsRequest& request, const QueryDeviceByTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceByTagsOutcomeCallable queryDeviceByTagsCallable(const Model::QueryDeviceByTagsRequest& request) const;
			QueryDeviceCertOutcome queryDeviceCert(const Model::QueryDeviceCertRequest &request)const;
			void queryDeviceCertAsync(const Model::QueryDeviceCertRequest& request, const QueryDeviceCertAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceCertOutcomeCallable queryDeviceCertCallable(const Model::QueryDeviceCertRequest& request) const;
			QueryDeviceDesiredPropertyOutcome queryDeviceDesiredProperty(const Model::QueryDeviceDesiredPropertyRequest &request)const;
			void queryDeviceDesiredPropertyAsync(const Model::QueryDeviceDesiredPropertyRequest& request, const QueryDeviceDesiredPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceDesiredPropertyOutcomeCallable queryDeviceDesiredPropertyCallable(const Model::QueryDeviceDesiredPropertyRequest& request) const;
			QueryDeviceDetailOutcome queryDeviceDetail(const Model::QueryDeviceDetailRequest &request)const;
			void queryDeviceDetailAsync(const Model::QueryDeviceDetailRequest& request, const QueryDeviceDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceDetailOutcomeCallable queryDeviceDetailCallable(const Model::QueryDeviceDetailRequest& request) const;
			QueryDeviceEventDataOutcome queryDeviceEventData(const Model::QueryDeviceEventDataRequest &request)const;
			void queryDeviceEventDataAsync(const Model::QueryDeviceEventDataRequest& request, const QueryDeviceEventDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceEventDataOutcomeCallable queryDeviceEventDataCallable(const Model::QueryDeviceEventDataRequest& request) const;
			QueryDeviceFileOutcome queryDeviceFile(const Model::QueryDeviceFileRequest &request)const;
			void queryDeviceFileAsync(const Model::QueryDeviceFileRequest& request, const QueryDeviceFileAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceFileOutcomeCallable queryDeviceFileCallable(const Model::QueryDeviceFileRequest& request) const;
			QueryDeviceFileListOutcome queryDeviceFileList(const Model::QueryDeviceFileListRequest &request)const;
			void queryDeviceFileListAsync(const Model::QueryDeviceFileListRequest& request, const QueryDeviceFileListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceFileListOutcomeCallable queryDeviceFileListCallable(const Model::QueryDeviceFileListRequest& request) const;
			QueryDeviceGroupByDeviceOutcome queryDeviceGroupByDevice(const Model::QueryDeviceGroupByDeviceRequest &request)const;
			void queryDeviceGroupByDeviceAsync(const Model::QueryDeviceGroupByDeviceRequest& request, const QueryDeviceGroupByDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceGroupByDeviceOutcomeCallable queryDeviceGroupByDeviceCallable(const Model::QueryDeviceGroupByDeviceRequest& request) const;
			QueryDeviceGroupByTagsOutcome queryDeviceGroupByTags(const Model::QueryDeviceGroupByTagsRequest &request)const;
			void queryDeviceGroupByTagsAsync(const Model::QueryDeviceGroupByTagsRequest& request, const QueryDeviceGroupByTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceGroupByTagsOutcomeCallable queryDeviceGroupByTagsCallable(const Model::QueryDeviceGroupByTagsRequest& request) const;
			QueryDeviceGroupInfoOutcome queryDeviceGroupInfo(const Model::QueryDeviceGroupInfoRequest &request)const;
			void queryDeviceGroupInfoAsync(const Model::QueryDeviceGroupInfoRequest& request, const QueryDeviceGroupInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceGroupInfoOutcomeCallable queryDeviceGroupInfoCallable(const Model::QueryDeviceGroupInfoRequest& request) const;
			QueryDeviceGroupListOutcome queryDeviceGroupList(const Model::QueryDeviceGroupListRequest &request)const;
			void queryDeviceGroupListAsync(const Model::QueryDeviceGroupListRequest& request, const QueryDeviceGroupListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceGroupListOutcomeCallable queryDeviceGroupListCallable(const Model::QueryDeviceGroupListRequest& request) const;
			QueryDeviceGroupTagListOutcome queryDeviceGroupTagList(const Model::QueryDeviceGroupTagListRequest &request)const;
			void queryDeviceGroupTagListAsync(const Model::QueryDeviceGroupTagListRequest& request, const QueryDeviceGroupTagListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceGroupTagListOutcomeCallable queryDeviceGroupTagListCallable(const Model::QueryDeviceGroupTagListRequest& request) const;
			QueryDeviceListByDeviceGroupOutcome queryDeviceListByDeviceGroup(const Model::QueryDeviceListByDeviceGroupRequest &request)const;
			void queryDeviceListByDeviceGroupAsync(const Model::QueryDeviceListByDeviceGroupRequest& request, const QueryDeviceListByDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceListByDeviceGroupOutcomeCallable queryDeviceListByDeviceGroupCallable(const Model::QueryDeviceListByDeviceGroupRequest& request) const;
			QueryDevicePropOutcome queryDeviceProp(const Model::QueryDevicePropRequest &request)const;
			void queryDevicePropAsync(const Model::QueryDevicePropRequest& request, const QueryDevicePropAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDevicePropOutcomeCallable queryDevicePropCallable(const Model::QueryDevicePropRequest& request) const;
			QueryDevicePropertiesDataOutcome queryDevicePropertiesData(const Model::QueryDevicePropertiesDataRequest &request)const;
			void queryDevicePropertiesDataAsync(const Model::QueryDevicePropertiesDataRequest& request, const QueryDevicePropertiesDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDevicePropertiesDataOutcomeCallable queryDevicePropertiesDataCallable(const Model::QueryDevicePropertiesDataRequest& request) const;
			QueryDevicePropertyDataOutcome queryDevicePropertyData(const Model::QueryDevicePropertyDataRequest &request)const;
			void queryDevicePropertyDataAsync(const Model::QueryDevicePropertyDataRequest& request, const QueryDevicePropertyDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDevicePropertyDataOutcomeCallable queryDevicePropertyDataCallable(const Model::QueryDevicePropertyDataRequest& request) const;
			QueryDevicePropertyStatusOutcome queryDevicePropertyStatus(const Model::QueryDevicePropertyStatusRequest &request)const;
			void queryDevicePropertyStatusAsync(const Model::QueryDevicePropertyStatusRequest& request, const QueryDevicePropertyStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDevicePropertyStatusOutcomeCallable queryDevicePropertyStatusCallable(const Model::QueryDevicePropertyStatusRequest& request) const;
			QueryDeviceServiceDataOutcome queryDeviceServiceData(const Model::QueryDeviceServiceDataRequest &request)const;
			void queryDeviceServiceDataAsync(const Model::QueryDeviceServiceDataRequest& request, const QueryDeviceServiceDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceServiceDataOutcomeCallable queryDeviceServiceDataCallable(const Model::QueryDeviceServiceDataRequest& request) const;
			QueryDeviceStatisticsOutcome queryDeviceStatistics(const Model::QueryDeviceStatisticsRequest &request)const;
			void queryDeviceStatisticsAsync(const Model::QueryDeviceStatisticsRequest& request, const QueryDeviceStatisticsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryDeviceStatisticsOutcomeCallable queryDeviceStatisticsCallable(const Model::QueryDeviceStatisticsRequest& request) const;
			QueryEdgeDriverOutcome queryEdgeDriver(const Model::QueryEdgeDriverRequest &request)const;
			void queryEdgeDriverAsync(const Model::QueryEdgeDriverRequest& request, const QueryEdgeDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeDriverOutcomeCallable queryEdgeDriverCallable(const Model::QueryEdgeDriverRequest& request) const;
			QueryEdgeDriverVersionOutcome queryEdgeDriverVersion(const Model::QueryEdgeDriverVersionRequest &request)const;
			void queryEdgeDriverVersionAsync(const Model::QueryEdgeDriverVersionRequest& request, const QueryEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeDriverVersionOutcomeCallable queryEdgeDriverVersionCallable(const Model::QueryEdgeDriverVersionRequest& request) const;
			QueryEdgeInstanceOutcome queryEdgeInstance(const Model::QueryEdgeInstanceRequest &request)const;
			void queryEdgeInstanceAsync(const Model::QueryEdgeInstanceRequest& request, const QueryEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceOutcomeCallable queryEdgeInstanceCallable(const Model::QueryEdgeInstanceRequest& request) const;
			QueryEdgeInstanceChannelOutcome queryEdgeInstanceChannel(const Model::QueryEdgeInstanceChannelRequest &request)const;
			void queryEdgeInstanceChannelAsync(const Model::QueryEdgeInstanceChannelRequest& request, const QueryEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceChannelOutcomeCallable queryEdgeInstanceChannelCallable(const Model::QueryEdgeInstanceChannelRequest& request) const;
			QueryEdgeInstanceDeviceOutcome queryEdgeInstanceDevice(const Model::QueryEdgeInstanceDeviceRequest &request)const;
			void queryEdgeInstanceDeviceAsync(const Model::QueryEdgeInstanceDeviceRequest& request, const QueryEdgeInstanceDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceDeviceOutcomeCallable queryEdgeInstanceDeviceCallable(const Model::QueryEdgeInstanceDeviceRequest& request) const;
			QueryEdgeInstanceDeviceByDriverOutcome queryEdgeInstanceDeviceByDriver(const Model::QueryEdgeInstanceDeviceByDriverRequest &request)const;
			void queryEdgeInstanceDeviceByDriverAsync(const Model::QueryEdgeInstanceDeviceByDriverRequest& request, const QueryEdgeInstanceDeviceByDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceDeviceByDriverOutcomeCallable queryEdgeInstanceDeviceByDriverCallable(const Model::QueryEdgeInstanceDeviceByDriverRequest& request) const;
			QueryEdgeInstanceDriverOutcome queryEdgeInstanceDriver(const Model::QueryEdgeInstanceDriverRequest &request)const;
			void queryEdgeInstanceDriverAsync(const Model::QueryEdgeInstanceDriverRequest& request, const QueryEdgeInstanceDriverAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceDriverOutcomeCallable queryEdgeInstanceDriverCallable(const Model::QueryEdgeInstanceDriverRequest& request) const;
			QueryEdgeInstanceGatewayOutcome queryEdgeInstanceGateway(const Model::QueryEdgeInstanceGatewayRequest &request)const;
			void queryEdgeInstanceGatewayAsync(const Model::QueryEdgeInstanceGatewayRequest& request, const QueryEdgeInstanceGatewayAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceGatewayOutcomeCallable queryEdgeInstanceGatewayCallable(const Model::QueryEdgeInstanceGatewayRequest& request) const;
			QueryEdgeInstanceHistoricDeploymentOutcome queryEdgeInstanceHistoricDeployment(const Model::QueryEdgeInstanceHistoricDeploymentRequest &request)const;
			void queryEdgeInstanceHistoricDeploymentAsync(const Model::QueryEdgeInstanceHistoricDeploymentRequest& request, const QueryEdgeInstanceHistoricDeploymentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceHistoricDeploymentOutcomeCallable queryEdgeInstanceHistoricDeploymentCallable(const Model::QueryEdgeInstanceHistoricDeploymentRequest& request) const;
			QueryEdgeInstanceSceneRuleOutcome queryEdgeInstanceSceneRule(const Model::QueryEdgeInstanceSceneRuleRequest &request)const;
			void queryEdgeInstanceSceneRuleAsync(const Model::QueryEdgeInstanceSceneRuleRequest& request, const QueryEdgeInstanceSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryEdgeInstanceSceneRuleOutcomeCallable queryEdgeInstanceSceneRuleCallable(const Model::QueryEdgeInstanceSceneRuleRequest& request) const;
			QueryLoRaJoinPermissionsOutcome queryLoRaJoinPermissions(const Model::QueryLoRaJoinPermissionsRequest &request)const;
			void queryLoRaJoinPermissionsAsync(const Model::QueryLoRaJoinPermissionsRequest& request, const QueryLoRaJoinPermissionsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryLoRaJoinPermissionsOutcomeCallable queryLoRaJoinPermissionsCallable(const Model::QueryLoRaJoinPermissionsRequest& request) const;
			QueryOTAFirmwareOutcome queryOTAFirmware(const Model::QueryOTAFirmwareRequest &request)const;
			void queryOTAFirmwareAsync(const Model::QueryOTAFirmwareRequest& request, const QueryOTAFirmwareAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryOTAFirmwareOutcomeCallable queryOTAFirmwareCallable(const Model::QueryOTAFirmwareRequest& request) const;
			QueryOTAJobOutcome queryOTAJob(const Model::QueryOTAJobRequest &request)const;
			void queryOTAJobAsync(const Model::QueryOTAJobRequest& request, const QueryOTAJobAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryOTAJobOutcomeCallable queryOTAJobCallable(const Model::QueryOTAJobRequest& request) const;
			QueryPageByApplyIdOutcome queryPageByApplyId(const Model::QueryPageByApplyIdRequest &request)const;
			void queryPageByApplyIdAsync(const Model::QueryPageByApplyIdRequest& request, const QueryPageByApplyIdAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryPageByApplyIdOutcomeCallable queryPageByApplyIdCallable(const Model::QueryPageByApplyIdRequest& request) const;
			QueryProductOutcome queryProduct(const Model::QueryProductRequest &request)const;
			void queryProductAsync(const Model::QueryProductRequest& request, const QueryProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryProductOutcomeCallable queryProductCallable(const Model::QueryProductRequest& request) const;
			QueryProductCertInfoOutcome queryProductCertInfo(const Model::QueryProductCertInfoRequest &request)const;
			void queryProductCertInfoAsync(const Model::QueryProductCertInfoRequest& request, const QueryProductCertInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryProductCertInfoOutcomeCallable queryProductCertInfoCallable(const Model::QueryProductCertInfoRequest& request) const;
			QueryProductListOutcome queryProductList(const Model::QueryProductListRequest &request)const;
			void queryProductListAsync(const Model::QueryProductListRequest& request, const QueryProductListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryProductListOutcomeCallable queryProductListCallable(const Model::QueryProductListRequest& request) const;
			QueryProductTopicOutcome queryProductTopic(const Model::QueryProductTopicRequest &request)const;
			void queryProductTopicAsync(const Model::QueryProductTopicRequest& request, const QueryProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryProductTopicOutcomeCallable queryProductTopicCallable(const Model::QueryProductTopicRequest& request) const;
			QuerySceneRuleOutcome querySceneRule(const Model::QuerySceneRuleRequest &request)const;
			void querySceneRuleAsync(const Model::QuerySceneRuleRequest& request, const QuerySceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QuerySceneRuleOutcomeCallable querySceneRuleCallable(const Model::QuerySceneRuleRequest& request) const;
			QuerySubscribeRelationOutcome querySubscribeRelation(const Model::QuerySubscribeRelationRequest &request)const;
			void querySubscribeRelationAsync(const Model::QuerySubscribeRelationRequest& request, const QuerySubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QuerySubscribeRelationOutcomeCallable querySubscribeRelationCallable(const Model::QuerySubscribeRelationRequest& request) const;
			QuerySummarySceneRuleLogOutcome querySummarySceneRuleLog(const Model::QuerySummarySceneRuleLogRequest &request)const;
			void querySummarySceneRuleLogAsync(const Model::QuerySummarySceneRuleLogRequest& request, const QuerySummarySceneRuleLogAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QuerySummarySceneRuleLogOutcomeCallable querySummarySceneRuleLogCallable(const Model::QuerySummarySceneRuleLogRequest& request) const;
			QuerySuperDeviceGroupOutcome querySuperDeviceGroup(const Model::QuerySuperDeviceGroupRequest &request)const;
			void querySuperDeviceGroupAsync(const Model::QuerySuperDeviceGroupRequest& request, const QuerySuperDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QuerySuperDeviceGroupOutcomeCallable querySuperDeviceGroupCallable(const Model::QuerySuperDeviceGroupRequest& request) const;
			QueryThingModelOutcome queryThingModel(const Model::QueryThingModelRequest &request)const;
			void queryThingModelAsync(const Model::QueryThingModelRequest& request, const QueryThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryThingModelOutcomeCallable queryThingModelCallable(const Model::QueryThingModelRequest& request) const;
			QueryTopicReverseRouteTableOutcome queryTopicReverseRouteTable(const Model::QueryTopicReverseRouteTableRequest &request)const;
			void queryTopicReverseRouteTableAsync(const Model::QueryTopicReverseRouteTableRequest& request, const QueryTopicReverseRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryTopicReverseRouteTableOutcomeCallable queryTopicReverseRouteTableCallable(const Model::QueryTopicReverseRouteTableRequest& request) const;
			QueryTopicRouteTableOutcome queryTopicRouteTable(const Model::QueryTopicRouteTableRequest &request)const;
			void queryTopicRouteTableAsync(const Model::QueryTopicRouteTableRequest& request, const QueryTopicRouteTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			QueryTopicRouteTableOutcomeCallable queryTopicRouteTableCallable(const Model::QueryTopicRouteTableRequest& request) const;
			RRpcOutcome rRpc(const Model::RRpcRequest &request)const;
			void rRpcAsync(const Model::RRpcRequest& request, const RRpcAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RRpcOutcomeCallable rRpcCallable(const Model::RRpcRequest& request) const;
			RegisterDeviceOutcome registerDevice(const Model::RegisterDeviceRequest &request)const;
			void registerDeviceAsync(const Model::RegisterDeviceRequest& request, const RegisterDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterDeviceOutcomeCallable registerDeviceCallable(const Model::RegisterDeviceRequest& request) const;
			ReleaseEdgeDriverVersionOutcome releaseEdgeDriverVersion(const Model::ReleaseEdgeDriverVersionRequest &request)const;
			void releaseEdgeDriverVersionAsync(const Model::ReleaseEdgeDriverVersionRequest& request, const ReleaseEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ReleaseEdgeDriverVersionOutcomeCallable releaseEdgeDriverVersionCallable(const Model::ReleaseEdgeDriverVersionRequest& request) const;
			RemoveThingTopoOutcome removeThingTopo(const Model::RemoveThingTopoRequest &request)const;
			void removeThingTopoAsync(const Model::RemoveThingTopoRequest& request, const RemoveThingTopoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveThingTopoOutcomeCallable removeThingTopoCallable(const Model::RemoveThingTopoRequest& request) const;
			ReplaceEdgeInstanceGatewayOutcome replaceEdgeInstanceGateway(const Model::ReplaceEdgeInstanceGatewayRequest &request)const;
			void replaceEdgeInstanceGatewayAsync(const Model::ReplaceEdgeInstanceGatewayRequest& request, const ReplaceEdgeInstanceGatewayAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ReplaceEdgeInstanceGatewayOutcomeCallable replaceEdgeInstanceGatewayCallable(const Model::ReplaceEdgeInstanceGatewayRequest& request) const;
			ResetConsumerGroupPositionOutcome resetConsumerGroupPosition(const Model::ResetConsumerGroupPositionRequest &request)const;
			void resetConsumerGroupPositionAsync(const Model::ResetConsumerGroupPositionRequest& request, const ResetConsumerGroupPositionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ResetConsumerGroupPositionOutcomeCallable resetConsumerGroupPositionCallable(const Model::ResetConsumerGroupPositionRequest& request) const;
			ResetThingOutcome resetThing(const Model::ResetThingRequest &request)const;
			void resetThingAsync(const Model::ResetThingRequest& request, const ResetThingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ResetThingOutcomeCallable resetThingCallable(const Model::ResetThingRequest& request) const;
			SaveDevicePropOutcome saveDeviceProp(const Model::SaveDevicePropRequest &request)const;
			void saveDevicePropAsync(const Model::SaveDevicePropRequest& request, const SaveDevicePropAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SaveDevicePropOutcomeCallable saveDevicePropCallable(const Model::SaveDevicePropRequest& request) const;
			SetDeviceDesiredPropertyOutcome setDeviceDesiredProperty(const Model::SetDeviceDesiredPropertyRequest &request)const;
			void setDeviceDesiredPropertyAsync(const Model::SetDeviceDesiredPropertyRequest& request, const SetDeviceDesiredPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetDeviceDesiredPropertyOutcomeCallable setDeviceDesiredPropertyCallable(const Model::SetDeviceDesiredPropertyRequest& request) const;
			SetDeviceGroupTagsOutcome setDeviceGroupTags(const Model::SetDeviceGroupTagsRequest &request)const;
			void setDeviceGroupTagsAsync(const Model::SetDeviceGroupTagsRequest& request, const SetDeviceGroupTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetDeviceGroupTagsOutcomeCallable setDeviceGroupTagsCallable(const Model::SetDeviceGroupTagsRequest& request) const;
			SetDevicePropertyOutcome setDeviceProperty(const Model::SetDevicePropertyRequest &request)const;
			void setDevicePropertyAsync(const Model::SetDevicePropertyRequest& request, const SetDevicePropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetDevicePropertyOutcomeCallable setDevicePropertyCallable(const Model::SetDevicePropertyRequest& request) const;
			SetDevicesPropertyOutcome setDevicesProperty(const Model::SetDevicesPropertyRequest &request)const;
			void setDevicesPropertyAsync(const Model::SetDevicesPropertyRequest& request, const SetDevicesPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetDevicesPropertyOutcomeCallable setDevicesPropertyCallable(const Model::SetDevicesPropertyRequest& request) const;
			SetEdgeInstanceDriverConfigsOutcome setEdgeInstanceDriverConfigs(const Model::SetEdgeInstanceDriverConfigsRequest &request)const;
			void setEdgeInstanceDriverConfigsAsync(const Model::SetEdgeInstanceDriverConfigsRequest& request, const SetEdgeInstanceDriverConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetEdgeInstanceDriverConfigsOutcomeCallable setEdgeInstanceDriverConfigsCallable(const Model::SetEdgeInstanceDriverConfigsRequest& request) const;
			SetProductCertInfoOutcome setProductCertInfo(const Model::SetProductCertInfoRequest &request)const;
			void setProductCertInfoAsync(const Model::SetProductCertInfoRequest& request, const SetProductCertInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetProductCertInfoOutcomeCallable setProductCertInfoCallable(const Model::SetProductCertInfoRequest& request) const;
			StartRuleOutcome startRule(const Model::StartRuleRequest &request)const;
			void startRuleAsync(const Model::StartRuleRequest& request, const StartRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartRuleOutcomeCallable startRuleCallable(const Model::StartRuleRequest& request) const;
			StopRuleOutcome stopRule(const Model::StopRuleRequest &request)const;
			void stopRuleAsync(const Model::StopRuleRequest& request, const StopRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopRuleOutcomeCallable stopRuleCallable(const Model::StopRuleRequest& request) const;
			TriggerSceneRuleOutcome triggerSceneRule(const Model::TriggerSceneRuleRequest &request)const;
			void triggerSceneRuleAsync(const Model::TriggerSceneRuleRequest& request, const TriggerSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TriggerSceneRuleOutcomeCallable triggerSceneRuleCallable(const Model::TriggerSceneRuleRequest& request) const;
			UnbindApplicationFromEdgeInstanceOutcome unbindApplicationFromEdgeInstance(const Model::UnbindApplicationFromEdgeInstanceRequest &request)const;
			void unbindApplicationFromEdgeInstanceAsync(const Model::UnbindApplicationFromEdgeInstanceRequest& request, const UnbindApplicationFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnbindApplicationFromEdgeInstanceOutcomeCallable unbindApplicationFromEdgeInstanceCallable(const Model::UnbindApplicationFromEdgeInstanceRequest& request) const;
			UnbindDriverFromEdgeInstanceOutcome unbindDriverFromEdgeInstance(const Model::UnbindDriverFromEdgeInstanceRequest &request)const;
			void unbindDriverFromEdgeInstanceAsync(const Model::UnbindDriverFromEdgeInstanceRequest& request, const UnbindDriverFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnbindDriverFromEdgeInstanceOutcomeCallable unbindDriverFromEdgeInstanceCallable(const Model::UnbindDriverFromEdgeInstanceRequest& request) const;
			UnbindSceneRuleFromEdgeInstanceOutcome unbindSceneRuleFromEdgeInstance(const Model::UnbindSceneRuleFromEdgeInstanceRequest &request)const;
			void unbindSceneRuleFromEdgeInstanceAsync(const Model::UnbindSceneRuleFromEdgeInstanceRequest& request, const UnbindSceneRuleFromEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnbindSceneRuleFromEdgeInstanceOutcomeCallable unbindSceneRuleFromEdgeInstanceCallable(const Model::UnbindSceneRuleFromEdgeInstanceRequest& request) const;
			UpdateConsumerGroupOutcome updateConsumerGroup(const Model::UpdateConsumerGroupRequest &request)const;
			void updateConsumerGroupAsync(const Model::UpdateConsumerGroupRequest& request, const UpdateConsumerGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateConsumerGroupOutcomeCallable updateConsumerGroupCallable(const Model::UpdateConsumerGroupRequest& request) const;
			UpdateDeviceGroupOutcome updateDeviceGroup(const Model::UpdateDeviceGroupRequest &request)const;
			void updateDeviceGroupAsync(const Model::UpdateDeviceGroupRequest& request, const UpdateDeviceGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateDeviceGroupOutcomeCallable updateDeviceGroupCallable(const Model::UpdateDeviceGroupRequest& request) const;
			UpdateDeviceShadowOutcome updateDeviceShadow(const Model::UpdateDeviceShadowRequest &request)const;
			void updateDeviceShadowAsync(const Model::UpdateDeviceShadowRequest& request, const UpdateDeviceShadowAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateDeviceShadowOutcomeCallable updateDeviceShadowCallable(const Model::UpdateDeviceShadowRequest& request) const;
			UpdateEdgeDriverVersionOutcome updateEdgeDriverVersion(const Model::UpdateEdgeDriverVersionRequest &request)const;
			void updateEdgeDriverVersionAsync(const Model::UpdateEdgeDriverVersionRequest& request, const UpdateEdgeDriverVersionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateEdgeDriverVersionOutcomeCallable updateEdgeDriverVersionCallable(const Model::UpdateEdgeDriverVersionRequest& request) const;
			UpdateEdgeInstanceOutcome updateEdgeInstance(const Model::UpdateEdgeInstanceRequest &request)const;
			void updateEdgeInstanceAsync(const Model::UpdateEdgeInstanceRequest& request, const UpdateEdgeInstanceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateEdgeInstanceOutcomeCallable updateEdgeInstanceCallable(const Model::UpdateEdgeInstanceRequest& request) const;
			UpdateEdgeInstanceChannelOutcome updateEdgeInstanceChannel(const Model::UpdateEdgeInstanceChannelRequest &request)const;
			void updateEdgeInstanceChannelAsync(const Model::UpdateEdgeInstanceChannelRequest& request, const UpdateEdgeInstanceChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateEdgeInstanceChannelOutcomeCallable updateEdgeInstanceChannelCallable(const Model::UpdateEdgeInstanceChannelRequest& request) const;
			UpdateProductOutcome updateProduct(const Model::UpdateProductRequest &request)const;
			void updateProductAsync(const Model::UpdateProductRequest& request, const UpdateProductAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateProductOutcomeCallable updateProductCallable(const Model::UpdateProductRequest& request) const;
			UpdateProductFilterConfigOutcome updateProductFilterConfig(const Model::UpdateProductFilterConfigRequest &request)const;
			void updateProductFilterConfigAsync(const Model::UpdateProductFilterConfigRequest& request, const UpdateProductFilterConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateProductFilterConfigOutcomeCallable updateProductFilterConfigCallable(const Model::UpdateProductFilterConfigRequest& request) const;
			UpdateProductTagsOutcome updateProductTags(const Model::UpdateProductTagsRequest &request)const;
			void updateProductTagsAsync(const Model::UpdateProductTagsRequest& request, const UpdateProductTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateProductTagsOutcomeCallable updateProductTagsCallable(const Model::UpdateProductTagsRequest& request) const;
			UpdateProductTopicOutcome updateProductTopic(const Model::UpdateProductTopicRequest &request)const;
			void updateProductTopicAsync(const Model::UpdateProductTopicRequest& request, const UpdateProductTopicAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateProductTopicOutcomeCallable updateProductTopicCallable(const Model::UpdateProductTopicRequest& request) const;
			UpdateRuleOutcome updateRule(const Model::UpdateRuleRequest &request)const;
			void updateRuleAsync(const Model::UpdateRuleRequest& request, const UpdateRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRuleOutcomeCallable updateRuleCallable(const Model::UpdateRuleRequest& request) const;
			UpdateRuleActionOutcome updateRuleAction(const Model::UpdateRuleActionRequest &request)const;
			void updateRuleActionAsync(const Model::UpdateRuleActionRequest& request, const UpdateRuleActionAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRuleActionOutcomeCallable updateRuleActionCallable(const Model::UpdateRuleActionRequest& request) const;
			UpdateSceneRuleOutcome updateSceneRule(const Model::UpdateSceneRuleRequest &request)const;
			void updateSceneRuleAsync(const Model::UpdateSceneRuleRequest& request, const UpdateSceneRuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateSceneRuleOutcomeCallable updateSceneRuleCallable(const Model::UpdateSceneRuleRequest& request) const;
			UpdateSubscribeRelationOutcome updateSubscribeRelation(const Model::UpdateSubscribeRelationRequest &request)const;
			void updateSubscribeRelationAsync(const Model::UpdateSubscribeRelationRequest& request, const UpdateSubscribeRelationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateSubscribeRelationOutcomeCallable updateSubscribeRelationCallable(const Model::UpdateSubscribeRelationRequest& request) const;
			UpdateThingModelOutcome updateThingModel(const Model::UpdateThingModelRequest &request)const;
			void updateThingModelAsync(const Model::UpdateThingModelRequest& request, const UpdateThingModelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateThingModelOutcomeCallable updateThingModelCallable(const Model::UpdateThingModelRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_IOT_IOTCLIENT_H_

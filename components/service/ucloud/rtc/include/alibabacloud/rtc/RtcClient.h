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

#ifndef ALIBABACLOUD_RTC_RTCCLIENT_H_
#define ALIBABACLOUD_RTC_RTCCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "RtcExport.h"
#include "model/AddRecordTemplateRequest.h"
#include "model/AddRecordTemplateResult.h"
#include "model/CreateChannelRequest.h"
#include "model/CreateChannelResult.h"
#include "model/CreateConferenceRequest.h"
#include "model/CreateConferenceResult.h"
#include "model/CreateMPULayoutRequest.h"
#include "model/CreateMPULayoutResult.h"
#include "model/CreateMPURuleRequest.h"
#include "model/CreateMPURuleResult.h"
#include "model/CreateServiceLinkedRoleForRtcRequest.h"
#include "model/CreateServiceLinkedRoleForRtcResult.h"
#include "model/DeleteChannelRequest.h"
#include "model/DeleteChannelResult.h"
#include "model/DeleteConferenceRequest.h"
#include "model/DeleteConferenceResult.h"
#include "model/DeleteMPULayoutRequest.h"
#include "model/DeleteMPULayoutResult.h"
#include "model/DeleteMPURuleRequest.h"
#include "model/DeleteMPURuleResult.h"
#include "model/DeleteRecordTemplateRequest.h"
#include "model/DeleteRecordTemplateResult.h"
#include "model/DescribeAppsRequest.h"
#include "model/DescribeAppsResult.h"
#include "model/DescribeChannelParticipantsRequest.h"
#include "model/DescribeChannelParticipantsResult.h"
#include "model/DescribeChannelUsersRequest.h"
#include "model/DescribeChannelUsersResult.h"
#include "model/DescribeConferenceAuthInfoRequest.h"
#include "model/DescribeConferenceAuthInfoResult.h"
#include "model/DescribeMPULayoutInfoRequest.h"
#include "model/DescribeMPULayoutInfoResult.h"
#include "model/DescribeMPULayoutInfoListRequest.h"
#include "model/DescribeMPULayoutInfoListResult.h"
#include "model/DescribeMPULayoutListRequest.h"
#include "model/DescribeMPULayoutListResult.h"
#include "model/DescribeMPURuleRequest.h"
#include "model/DescribeMPURuleResult.h"
#include "model/DescribeRTCAppKeyRequest.h"
#include "model/DescribeRTCAppKeyResult.h"
#include "model/DescribeRecordFilesRequest.h"
#include "model/DescribeRecordFilesResult.h"
#include "model/DescribeRecordTasksRequest.h"
#include "model/DescribeRecordTasksResult.h"
#include "model/DescribeRecordTemplatesRequest.h"
#include "model/DescribeRecordTemplatesResult.h"
#include "model/DescribeRtcChannelCntDataRequest.h"
#include "model/DescribeRtcChannelCntDataResult.h"
#include "model/DescribeRtcChannelDetailRequest.h"
#include "model/DescribeRtcChannelDetailResult.h"
#include "model/DescribeRtcChannelListRequest.h"
#include "model/DescribeRtcChannelListResult.h"
#include "model/DescribeRtcChannelMetricRequest.h"
#include "model/DescribeRtcChannelMetricResult.h"
#include "model/DescribeRtcChannelMetricsRequest.h"
#include "model/DescribeRtcChannelMetricsResult.h"
#include "model/DescribeRtcChannelUserListRequest.h"
#include "model/DescribeRtcChannelUserListResult.h"
#include "model/DescribeRtcChannelsRequest.h"
#include "model/DescribeRtcChannelsResult.h"
#include "model/DescribeRtcDurationDataRequest.h"
#include "model/DescribeRtcDurationDataResult.h"
#include "model/DescribeRtcPeakChannelCntDataRequest.h"
#include "model/DescribeRtcPeakChannelCntDataResult.h"
#include "model/DescribeRtcPeakUserCntDataRequest.h"
#include "model/DescribeRtcPeakUserCntDataResult.h"
#include "model/DescribeRtcQualityMetricRequest.h"
#include "model/DescribeRtcQualityMetricResult.h"
#include "model/DescribeRtcScaleRequest.h"
#include "model/DescribeRtcScaleResult.h"
#include "model/DescribeRtcScaleDetailRequest.h"
#include "model/DescribeRtcScaleDetailResult.h"
#include "model/DescribeRtcUserCntDataRequest.h"
#include "model/DescribeRtcUserCntDataResult.h"
#include "model/DescribeRtcUserEventsRequest.h"
#include "model/DescribeRtcUserEventsResult.h"
#include "model/DescribeRtcUserListRequest.h"
#include "model/DescribeRtcUserListResult.h"
#include "model/DescribeUserInfoInChannelRequest.h"
#include "model/DescribeUserInfoInChannelResult.h"
#include "model/DisableMPURuleRequest.h"
#include "model/DisableMPURuleResult.h"
#include "model/EnableMPURuleRequest.h"
#include "model/EnableMPURuleResult.h"
#include "model/GetMPUTaskStatusRequest.h"
#include "model/GetMPUTaskStatusResult.h"
#include "model/ModifyAppRequest.h"
#include "model/ModifyAppResult.h"
#include "model/ModifyConferenceRequest.h"
#include "model/ModifyConferenceResult.h"
#include "model/ModifyMPULayoutRequest.h"
#include "model/ModifyMPULayoutResult.h"
#include "model/MuteAudioRequest.h"
#include "model/MuteAudioResult.h"
#include "model/MuteAudioAllRequest.h"
#include "model/MuteAudioAllResult.h"
#include "model/ReceiveNotifyRequest.h"
#include "model/ReceiveNotifyResult.h"
#include "model/RemoveParticipantsRequest.h"
#include "model/RemoveParticipantsResult.h"
#include "model/RemoveTerminalsRequest.h"
#include "model/RemoveTerminalsResult.h"
#include "model/SetChannelPropertyRequest.h"
#include "model/SetChannelPropertyResult.h"
#include "model/StartMPUTaskRequest.h"
#include "model/StartMPUTaskResult.h"
#include "model/StartRecordTaskRequest.h"
#include "model/StartRecordTaskResult.h"
#include "model/StopChannelUserPublishRequest.h"
#include "model/StopChannelUserPublishResult.h"
#include "model/StopMPUTaskRequest.h"
#include "model/StopMPUTaskResult.h"
#include "model/StopRecordTaskRequest.h"
#include "model/StopRecordTaskResult.h"
#include "model/UnmuteAudioRequest.h"
#include "model/UnmuteAudioResult.h"
#include "model/UnmuteAudioAllRequest.h"
#include "model/UnmuteAudioAllResult.h"
#include "model/UpdateChannelRequest.h"
#include "model/UpdateChannelResult.h"
#include "model/UpdateMPULayoutRequest.h"
#include "model/UpdateMPULayoutResult.h"
#include "model/UpdateRecordTaskRequest.h"
#include "model/UpdateRecordTaskResult.h"
#include "model/UpdateRecordTemplateRequest.h"
#include "model/UpdateRecordTemplateResult.h"


namespace AlibabaCloud
{
	namespace Rtc
	{
		class ALIBABACLOUD_RTC_EXPORT RtcClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AddRecordTemplateResult> AddRecordTemplateOutcome;
			typedef std::future<AddRecordTemplateOutcome> AddRecordTemplateOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::AddRecordTemplateRequest&, const AddRecordTemplateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddRecordTemplateAsyncHandler;
			typedef Outcome<Error, Model::CreateChannelResult> CreateChannelOutcome;
			typedef std::future<CreateChannelOutcome> CreateChannelOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::CreateChannelRequest&, const CreateChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateChannelAsyncHandler;
			typedef Outcome<Error, Model::CreateConferenceResult> CreateConferenceOutcome;
			typedef std::future<CreateConferenceOutcome> CreateConferenceOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::CreateConferenceRequest&, const CreateConferenceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateConferenceAsyncHandler;
			typedef Outcome<Error, Model::CreateMPULayoutResult> CreateMPULayoutOutcome;
			typedef std::future<CreateMPULayoutOutcome> CreateMPULayoutOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::CreateMPULayoutRequest&, const CreateMPULayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateMPULayoutAsyncHandler;
			typedef Outcome<Error, Model::CreateMPURuleResult> CreateMPURuleOutcome;
			typedef std::future<CreateMPURuleOutcome> CreateMPURuleOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::CreateMPURuleRequest&, const CreateMPURuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateMPURuleAsyncHandler;
			typedef Outcome<Error, Model::CreateServiceLinkedRoleForRtcResult> CreateServiceLinkedRoleForRtcOutcome;
			typedef std::future<CreateServiceLinkedRoleForRtcOutcome> CreateServiceLinkedRoleForRtcOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::CreateServiceLinkedRoleForRtcRequest&, const CreateServiceLinkedRoleForRtcOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateServiceLinkedRoleForRtcAsyncHandler;
			typedef Outcome<Error, Model::DeleteChannelResult> DeleteChannelOutcome;
			typedef std::future<DeleteChannelOutcome> DeleteChannelOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DeleteChannelRequest&, const DeleteChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteChannelAsyncHandler;
			typedef Outcome<Error, Model::DeleteConferenceResult> DeleteConferenceOutcome;
			typedef std::future<DeleteConferenceOutcome> DeleteConferenceOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DeleteConferenceRequest&, const DeleteConferenceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteConferenceAsyncHandler;
			typedef Outcome<Error, Model::DeleteMPULayoutResult> DeleteMPULayoutOutcome;
			typedef std::future<DeleteMPULayoutOutcome> DeleteMPULayoutOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DeleteMPULayoutRequest&, const DeleteMPULayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteMPULayoutAsyncHandler;
			typedef Outcome<Error, Model::DeleteMPURuleResult> DeleteMPURuleOutcome;
			typedef std::future<DeleteMPURuleOutcome> DeleteMPURuleOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DeleteMPURuleRequest&, const DeleteMPURuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteMPURuleAsyncHandler;
			typedef Outcome<Error, Model::DeleteRecordTemplateResult> DeleteRecordTemplateOutcome;
			typedef std::future<DeleteRecordTemplateOutcome> DeleteRecordTemplateOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DeleteRecordTemplateRequest&, const DeleteRecordTemplateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRecordTemplateAsyncHandler;
			typedef Outcome<Error, Model::DescribeAppsResult> DescribeAppsOutcome;
			typedef std::future<DescribeAppsOutcome> DescribeAppsOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeAppsRequest&, const DescribeAppsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeAppsAsyncHandler;
			typedef Outcome<Error, Model::DescribeChannelParticipantsResult> DescribeChannelParticipantsOutcome;
			typedef std::future<DescribeChannelParticipantsOutcome> DescribeChannelParticipantsOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeChannelParticipantsRequest&, const DescribeChannelParticipantsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeChannelParticipantsAsyncHandler;
			typedef Outcome<Error, Model::DescribeChannelUsersResult> DescribeChannelUsersOutcome;
			typedef std::future<DescribeChannelUsersOutcome> DescribeChannelUsersOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeChannelUsersRequest&, const DescribeChannelUsersOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeChannelUsersAsyncHandler;
			typedef Outcome<Error, Model::DescribeConferenceAuthInfoResult> DescribeConferenceAuthInfoOutcome;
			typedef std::future<DescribeConferenceAuthInfoOutcome> DescribeConferenceAuthInfoOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeConferenceAuthInfoRequest&, const DescribeConferenceAuthInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeConferenceAuthInfoAsyncHandler;
			typedef Outcome<Error, Model::DescribeMPULayoutInfoResult> DescribeMPULayoutInfoOutcome;
			typedef std::future<DescribeMPULayoutInfoOutcome> DescribeMPULayoutInfoOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeMPULayoutInfoRequest&, const DescribeMPULayoutInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeMPULayoutInfoAsyncHandler;
			typedef Outcome<Error, Model::DescribeMPULayoutInfoListResult> DescribeMPULayoutInfoListOutcome;
			typedef std::future<DescribeMPULayoutInfoListOutcome> DescribeMPULayoutInfoListOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeMPULayoutInfoListRequest&, const DescribeMPULayoutInfoListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeMPULayoutInfoListAsyncHandler;
			typedef Outcome<Error, Model::DescribeMPULayoutListResult> DescribeMPULayoutListOutcome;
			typedef std::future<DescribeMPULayoutListOutcome> DescribeMPULayoutListOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeMPULayoutListRequest&, const DescribeMPULayoutListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeMPULayoutListAsyncHandler;
			typedef Outcome<Error, Model::DescribeMPURuleResult> DescribeMPURuleOutcome;
			typedef std::future<DescribeMPURuleOutcome> DescribeMPURuleOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeMPURuleRequest&, const DescribeMPURuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeMPURuleAsyncHandler;
			typedef Outcome<Error, Model::DescribeRTCAppKeyResult> DescribeRTCAppKeyOutcome;
			typedef std::future<DescribeRTCAppKeyOutcome> DescribeRTCAppKeyOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRTCAppKeyRequest&, const DescribeRTCAppKeyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRTCAppKeyAsyncHandler;
			typedef Outcome<Error, Model::DescribeRecordFilesResult> DescribeRecordFilesOutcome;
			typedef std::future<DescribeRecordFilesOutcome> DescribeRecordFilesOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRecordFilesRequest&, const DescribeRecordFilesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRecordFilesAsyncHandler;
			typedef Outcome<Error, Model::DescribeRecordTasksResult> DescribeRecordTasksOutcome;
			typedef std::future<DescribeRecordTasksOutcome> DescribeRecordTasksOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRecordTasksRequest&, const DescribeRecordTasksOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRecordTasksAsyncHandler;
			typedef Outcome<Error, Model::DescribeRecordTemplatesResult> DescribeRecordTemplatesOutcome;
			typedef std::future<DescribeRecordTemplatesOutcome> DescribeRecordTemplatesOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRecordTemplatesRequest&, const DescribeRecordTemplatesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRecordTemplatesAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcChannelCntDataResult> DescribeRtcChannelCntDataOutcome;
			typedef std::future<DescribeRtcChannelCntDataOutcome> DescribeRtcChannelCntDataOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcChannelCntDataRequest&, const DescribeRtcChannelCntDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcChannelCntDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcChannelDetailResult> DescribeRtcChannelDetailOutcome;
			typedef std::future<DescribeRtcChannelDetailOutcome> DescribeRtcChannelDetailOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcChannelDetailRequest&, const DescribeRtcChannelDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcChannelDetailAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcChannelListResult> DescribeRtcChannelListOutcome;
			typedef std::future<DescribeRtcChannelListOutcome> DescribeRtcChannelListOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcChannelListRequest&, const DescribeRtcChannelListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcChannelListAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcChannelMetricResult> DescribeRtcChannelMetricOutcome;
			typedef std::future<DescribeRtcChannelMetricOutcome> DescribeRtcChannelMetricOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcChannelMetricRequest&, const DescribeRtcChannelMetricOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcChannelMetricAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcChannelMetricsResult> DescribeRtcChannelMetricsOutcome;
			typedef std::future<DescribeRtcChannelMetricsOutcome> DescribeRtcChannelMetricsOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcChannelMetricsRequest&, const DescribeRtcChannelMetricsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcChannelMetricsAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcChannelUserListResult> DescribeRtcChannelUserListOutcome;
			typedef std::future<DescribeRtcChannelUserListOutcome> DescribeRtcChannelUserListOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcChannelUserListRequest&, const DescribeRtcChannelUserListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcChannelUserListAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcChannelsResult> DescribeRtcChannelsOutcome;
			typedef std::future<DescribeRtcChannelsOutcome> DescribeRtcChannelsOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcChannelsRequest&, const DescribeRtcChannelsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcChannelsAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcDurationDataResult> DescribeRtcDurationDataOutcome;
			typedef std::future<DescribeRtcDurationDataOutcome> DescribeRtcDurationDataOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcDurationDataRequest&, const DescribeRtcDurationDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcDurationDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcPeakChannelCntDataResult> DescribeRtcPeakChannelCntDataOutcome;
			typedef std::future<DescribeRtcPeakChannelCntDataOutcome> DescribeRtcPeakChannelCntDataOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcPeakChannelCntDataRequest&, const DescribeRtcPeakChannelCntDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcPeakChannelCntDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcPeakUserCntDataResult> DescribeRtcPeakUserCntDataOutcome;
			typedef std::future<DescribeRtcPeakUserCntDataOutcome> DescribeRtcPeakUserCntDataOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcPeakUserCntDataRequest&, const DescribeRtcPeakUserCntDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcPeakUserCntDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcQualityMetricResult> DescribeRtcQualityMetricOutcome;
			typedef std::future<DescribeRtcQualityMetricOutcome> DescribeRtcQualityMetricOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcQualityMetricRequest&, const DescribeRtcQualityMetricOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcQualityMetricAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcScaleResult> DescribeRtcScaleOutcome;
			typedef std::future<DescribeRtcScaleOutcome> DescribeRtcScaleOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcScaleRequest&, const DescribeRtcScaleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcScaleAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcScaleDetailResult> DescribeRtcScaleDetailOutcome;
			typedef std::future<DescribeRtcScaleDetailOutcome> DescribeRtcScaleDetailOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcScaleDetailRequest&, const DescribeRtcScaleDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcScaleDetailAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcUserCntDataResult> DescribeRtcUserCntDataOutcome;
			typedef std::future<DescribeRtcUserCntDataOutcome> DescribeRtcUserCntDataOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcUserCntDataRequest&, const DescribeRtcUserCntDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcUserCntDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcUserEventsResult> DescribeRtcUserEventsOutcome;
			typedef std::future<DescribeRtcUserEventsOutcome> DescribeRtcUserEventsOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcUserEventsRequest&, const DescribeRtcUserEventsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcUserEventsAsyncHandler;
			typedef Outcome<Error, Model::DescribeRtcUserListResult> DescribeRtcUserListOutcome;
			typedef std::future<DescribeRtcUserListOutcome> DescribeRtcUserListOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeRtcUserListRequest&, const DescribeRtcUserListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRtcUserListAsyncHandler;
			typedef Outcome<Error, Model::DescribeUserInfoInChannelResult> DescribeUserInfoInChannelOutcome;
			typedef std::future<DescribeUserInfoInChannelOutcome> DescribeUserInfoInChannelOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DescribeUserInfoInChannelRequest&, const DescribeUserInfoInChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeUserInfoInChannelAsyncHandler;
			typedef Outcome<Error, Model::DisableMPURuleResult> DisableMPURuleOutcome;
			typedef std::future<DisableMPURuleOutcome> DisableMPURuleOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::DisableMPURuleRequest&, const DisableMPURuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DisableMPURuleAsyncHandler;
			typedef Outcome<Error, Model::EnableMPURuleResult> EnableMPURuleOutcome;
			typedef std::future<EnableMPURuleOutcome> EnableMPURuleOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::EnableMPURuleRequest&, const EnableMPURuleOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnableMPURuleAsyncHandler;
			typedef Outcome<Error, Model::GetMPUTaskStatusResult> GetMPUTaskStatusOutcome;
			typedef std::future<GetMPUTaskStatusOutcome> GetMPUTaskStatusOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::GetMPUTaskStatusRequest&, const GetMPUTaskStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetMPUTaskStatusAsyncHandler;
			typedef Outcome<Error, Model::ModifyAppResult> ModifyAppOutcome;
			typedef std::future<ModifyAppOutcome> ModifyAppOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::ModifyAppRequest&, const ModifyAppOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyAppAsyncHandler;
			typedef Outcome<Error, Model::ModifyConferenceResult> ModifyConferenceOutcome;
			typedef std::future<ModifyConferenceOutcome> ModifyConferenceOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::ModifyConferenceRequest&, const ModifyConferenceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyConferenceAsyncHandler;
			typedef Outcome<Error, Model::ModifyMPULayoutResult> ModifyMPULayoutOutcome;
			typedef std::future<ModifyMPULayoutOutcome> ModifyMPULayoutOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::ModifyMPULayoutRequest&, const ModifyMPULayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyMPULayoutAsyncHandler;
			typedef Outcome<Error, Model::MuteAudioResult> MuteAudioOutcome;
			typedef std::future<MuteAudioOutcome> MuteAudioOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::MuteAudioRequest&, const MuteAudioOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> MuteAudioAsyncHandler;
			typedef Outcome<Error, Model::MuteAudioAllResult> MuteAudioAllOutcome;
			typedef std::future<MuteAudioAllOutcome> MuteAudioAllOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::MuteAudioAllRequest&, const MuteAudioAllOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> MuteAudioAllAsyncHandler;
			typedef Outcome<Error, Model::ReceiveNotifyResult> ReceiveNotifyOutcome;
			typedef std::future<ReceiveNotifyOutcome> ReceiveNotifyOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::ReceiveNotifyRequest&, const ReceiveNotifyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ReceiveNotifyAsyncHandler;
			typedef Outcome<Error, Model::RemoveParticipantsResult> RemoveParticipantsOutcome;
			typedef std::future<RemoveParticipantsOutcome> RemoveParticipantsOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::RemoveParticipantsRequest&, const RemoveParticipantsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveParticipantsAsyncHandler;
			typedef Outcome<Error, Model::RemoveTerminalsResult> RemoveTerminalsOutcome;
			typedef std::future<RemoveTerminalsOutcome> RemoveTerminalsOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::RemoveTerminalsRequest&, const RemoveTerminalsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RemoveTerminalsAsyncHandler;
			typedef Outcome<Error, Model::SetChannelPropertyResult> SetChannelPropertyOutcome;
			typedef std::future<SetChannelPropertyOutcome> SetChannelPropertyOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::SetChannelPropertyRequest&, const SetChannelPropertyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetChannelPropertyAsyncHandler;
			typedef Outcome<Error, Model::StartMPUTaskResult> StartMPUTaskOutcome;
			typedef std::future<StartMPUTaskOutcome> StartMPUTaskOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::StartMPUTaskRequest&, const StartMPUTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartMPUTaskAsyncHandler;
			typedef Outcome<Error, Model::StartRecordTaskResult> StartRecordTaskOutcome;
			typedef std::future<StartRecordTaskOutcome> StartRecordTaskOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::StartRecordTaskRequest&, const StartRecordTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartRecordTaskAsyncHandler;
			typedef Outcome<Error, Model::StopChannelUserPublishResult> StopChannelUserPublishOutcome;
			typedef std::future<StopChannelUserPublishOutcome> StopChannelUserPublishOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::StopChannelUserPublishRequest&, const StopChannelUserPublishOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopChannelUserPublishAsyncHandler;
			typedef Outcome<Error, Model::StopMPUTaskResult> StopMPUTaskOutcome;
			typedef std::future<StopMPUTaskOutcome> StopMPUTaskOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::StopMPUTaskRequest&, const StopMPUTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopMPUTaskAsyncHandler;
			typedef Outcome<Error, Model::StopRecordTaskResult> StopRecordTaskOutcome;
			typedef std::future<StopRecordTaskOutcome> StopRecordTaskOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::StopRecordTaskRequest&, const StopRecordTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopRecordTaskAsyncHandler;
			typedef Outcome<Error, Model::UnmuteAudioResult> UnmuteAudioOutcome;
			typedef std::future<UnmuteAudioOutcome> UnmuteAudioOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::UnmuteAudioRequest&, const UnmuteAudioOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnmuteAudioAsyncHandler;
			typedef Outcome<Error, Model::UnmuteAudioAllResult> UnmuteAudioAllOutcome;
			typedef std::future<UnmuteAudioAllOutcome> UnmuteAudioAllOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::UnmuteAudioAllRequest&, const UnmuteAudioAllOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnmuteAudioAllAsyncHandler;
			typedef Outcome<Error, Model::UpdateChannelResult> UpdateChannelOutcome;
			typedef std::future<UpdateChannelOutcome> UpdateChannelOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::UpdateChannelRequest&, const UpdateChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateChannelAsyncHandler;
			typedef Outcome<Error, Model::UpdateMPULayoutResult> UpdateMPULayoutOutcome;
			typedef std::future<UpdateMPULayoutOutcome> UpdateMPULayoutOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::UpdateMPULayoutRequest&, const UpdateMPULayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateMPULayoutAsyncHandler;
			typedef Outcome<Error, Model::UpdateRecordTaskResult> UpdateRecordTaskOutcome;
			typedef std::future<UpdateRecordTaskOutcome> UpdateRecordTaskOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::UpdateRecordTaskRequest&, const UpdateRecordTaskOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRecordTaskAsyncHandler;
			typedef Outcome<Error, Model::UpdateRecordTemplateResult> UpdateRecordTemplateOutcome;
			typedef std::future<UpdateRecordTemplateOutcome> UpdateRecordTemplateOutcomeCallable;
			typedef std::function<void(const RtcClient*, const Model::UpdateRecordTemplateRequest&, const UpdateRecordTemplateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateRecordTemplateAsyncHandler;

			RtcClient(const Credentials &credentials, const ClientConfiguration &configuration);
			RtcClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			RtcClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~RtcClient();
			AddRecordTemplateOutcome addRecordTemplate(const Model::AddRecordTemplateRequest &request)const;
			void addRecordTemplateAsync(const Model::AddRecordTemplateRequest& request, const AddRecordTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddRecordTemplateOutcomeCallable addRecordTemplateCallable(const Model::AddRecordTemplateRequest& request) const;
			CreateChannelOutcome createChannel(const Model::CreateChannelRequest &request)const;
			void createChannelAsync(const Model::CreateChannelRequest& request, const CreateChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateChannelOutcomeCallable createChannelCallable(const Model::CreateChannelRequest& request) const;
			CreateConferenceOutcome createConference(const Model::CreateConferenceRequest &request)const;
			void createConferenceAsync(const Model::CreateConferenceRequest& request, const CreateConferenceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateConferenceOutcomeCallable createConferenceCallable(const Model::CreateConferenceRequest& request) const;
			CreateMPULayoutOutcome createMPULayout(const Model::CreateMPULayoutRequest &request)const;
			void createMPULayoutAsync(const Model::CreateMPULayoutRequest& request, const CreateMPULayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateMPULayoutOutcomeCallable createMPULayoutCallable(const Model::CreateMPULayoutRequest& request) const;
			CreateMPURuleOutcome createMPURule(const Model::CreateMPURuleRequest &request)const;
			void createMPURuleAsync(const Model::CreateMPURuleRequest& request, const CreateMPURuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateMPURuleOutcomeCallable createMPURuleCallable(const Model::CreateMPURuleRequest& request) const;
			CreateServiceLinkedRoleForRtcOutcome createServiceLinkedRoleForRtc(const Model::CreateServiceLinkedRoleForRtcRequest &request)const;
			void createServiceLinkedRoleForRtcAsync(const Model::CreateServiceLinkedRoleForRtcRequest& request, const CreateServiceLinkedRoleForRtcAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateServiceLinkedRoleForRtcOutcomeCallable createServiceLinkedRoleForRtcCallable(const Model::CreateServiceLinkedRoleForRtcRequest& request) const;
			DeleteChannelOutcome deleteChannel(const Model::DeleteChannelRequest &request)const;
			void deleteChannelAsync(const Model::DeleteChannelRequest& request, const DeleteChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteChannelOutcomeCallable deleteChannelCallable(const Model::DeleteChannelRequest& request) const;
			DeleteConferenceOutcome deleteConference(const Model::DeleteConferenceRequest &request)const;
			void deleteConferenceAsync(const Model::DeleteConferenceRequest& request, const DeleteConferenceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteConferenceOutcomeCallable deleteConferenceCallable(const Model::DeleteConferenceRequest& request) const;
			DeleteMPULayoutOutcome deleteMPULayout(const Model::DeleteMPULayoutRequest &request)const;
			void deleteMPULayoutAsync(const Model::DeleteMPULayoutRequest& request, const DeleteMPULayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteMPULayoutOutcomeCallable deleteMPULayoutCallable(const Model::DeleteMPULayoutRequest& request) const;
			DeleteMPURuleOutcome deleteMPURule(const Model::DeleteMPURuleRequest &request)const;
			void deleteMPURuleAsync(const Model::DeleteMPURuleRequest& request, const DeleteMPURuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteMPURuleOutcomeCallable deleteMPURuleCallable(const Model::DeleteMPURuleRequest& request) const;
			DeleteRecordTemplateOutcome deleteRecordTemplate(const Model::DeleteRecordTemplateRequest &request)const;
			void deleteRecordTemplateAsync(const Model::DeleteRecordTemplateRequest& request, const DeleteRecordTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRecordTemplateOutcomeCallable deleteRecordTemplateCallable(const Model::DeleteRecordTemplateRequest& request) const;
			DescribeAppsOutcome describeApps(const Model::DescribeAppsRequest &request)const;
			void describeAppsAsync(const Model::DescribeAppsRequest& request, const DescribeAppsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeAppsOutcomeCallable describeAppsCallable(const Model::DescribeAppsRequest& request) const;
			DescribeChannelParticipantsOutcome describeChannelParticipants(const Model::DescribeChannelParticipantsRequest &request)const;
			void describeChannelParticipantsAsync(const Model::DescribeChannelParticipantsRequest& request, const DescribeChannelParticipantsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeChannelParticipantsOutcomeCallable describeChannelParticipantsCallable(const Model::DescribeChannelParticipantsRequest& request) const;
			DescribeChannelUsersOutcome describeChannelUsers(const Model::DescribeChannelUsersRequest &request)const;
			void describeChannelUsersAsync(const Model::DescribeChannelUsersRequest& request, const DescribeChannelUsersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeChannelUsersOutcomeCallable describeChannelUsersCallable(const Model::DescribeChannelUsersRequest& request) const;
			DescribeConferenceAuthInfoOutcome describeConferenceAuthInfo(const Model::DescribeConferenceAuthInfoRequest &request)const;
			void describeConferenceAuthInfoAsync(const Model::DescribeConferenceAuthInfoRequest& request, const DescribeConferenceAuthInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeConferenceAuthInfoOutcomeCallable describeConferenceAuthInfoCallable(const Model::DescribeConferenceAuthInfoRequest& request) const;
			DescribeMPULayoutInfoOutcome describeMPULayoutInfo(const Model::DescribeMPULayoutInfoRequest &request)const;
			void describeMPULayoutInfoAsync(const Model::DescribeMPULayoutInfoRequest& request, const DescribeMPULayoutInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeMPULayoutInfoOutcomeCallable describeMPULayoutInfoCallable(const Model::DescribeMPULayoutInfoRequest& request) const;
			DescribeMPULayoutInfoListOutcome describeMPULayoutInfoList(const Model::DescribeMPULayoutInfoListRequest &request)const;
			void describeMPULayoutInfoListAsync(const Model::DescribeMPULayoutInfoListRequest& request, const DescribeMPULayoutInfoListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeMPULayoutInfoListOutcomeCallable describeMPULayoutInfoListCallable(const Model::DescribeMPULayoutInfoListRequest& request) const;
			DescribeMPULayoutListOutcome describeMPULayoutList(const Model::DescribeMPULayoutListRequest &request)const;
			void describeMPULayoutListAsync(const Model::DescribeMPULayoutListRequest& request, const DescribeMPULayoutListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeMPULayoutListOutcomeCallable describeMPULayoutListCallable(const Model::DescribeMPULayoutListRequest& request) const;
			DescribeMPURuleOutcome describeMPURule(const Model::DescribeMPURuleRequest &request)const;
			void describeMPURuleAsync(const Model::DescribeMPURuleRequest& request, const DescribeMPURuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeMPURuleOutcomeCallable describeMPURuleCallable(const Model::DescribeMPURuleRequest& request) const;
			DescribeRTCAppKeyOutcome describeRTCAppKey(const Model::DescribeRTCAppKeyRequest &request)const;
			void describeRTCAppKeyAsync(const Model::DescribeRTCAppKeyRequest& request, const DescribeRTCAppKeyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRTCAppKeyOutcomeCallable describeRTCAppKeyCallable(const Model::DescribeRTCAppKeyRequest& request) const;
			DescribeRecordFilesOutcome describeRecordFiles(const Model::DescribeRecordFilesRequest &request)const;
			void describeRecordFilesAsync(const Model::DescribeRecordFilesRequest& request, const DescribeRecordFilesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRecordFilesOutcomeCallable describeRecordFilesCallable(const Model::DescribeRecordFilesRequest& request) const;
			DescribeRecordTasksOutcome describeRecordTasks(const Model::DescribeRecordTasksRequest &request)const;
			void describeRecordTasksAsync(const Model::DescribeRecordTasksRequest& request, const DescribeRecordTasksAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRecordTasksOutcomeCallable describeRecordTasksCallable(const Model::DescribeRecordTasksRequest& request) const;
			DescribeRecordTemplatesOutcome describeRecordTemplates(const Model::DescribeRecordTemplatesRequest &request)const;
			void describeRecordTemplatesAsync(const Model::DescribeRecordTemplatesRequest& request, const DescribeRecordTemplatesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRecordTemplatesOutcomeCallable describeRecordTemplatesCallable(const Model::DescribeRecordTemplatesRequest& request) const;
			DescribeRtcChannelCntDataOutcome describeRtcChannelCntData(const Model::DescribeRtcChannelCntDataRequest &request)const;
			void describeRtcChannelCntDataAsync(const Model::DescribeRtcChannelCntDataRequest& request, const DescribeRtcChannelCntDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcChannelCntDataOutcomeCallable describeRtcChannelCntDataCallable(const Model::DescribeRtcChannelCntDataRequest& request) const;
			DescribeRtcChannelDetailOutcome describeRtcChannelDetail(const Model::DescribeRtcChannelDetailRequest &request)const;
			void describeRtcChannelDetailAsync(const Model::DescribeRtcChannelDetailRequest& request, const DescribeRtcChannelDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcChannelDetailOutcomeCallable describeRtcChannelDetailCallable(const Model::DescribeRtcChannelDetailRequest& request) const;
			DescribeRtcChannelListOutcome describeRtcChannelList(const Model::DescribeRtcChannelListRequest &request)const;
			void describeRtcChannelListAsync(const Model::DescribeRtcChannelListRequest& request, const DescribeRtcChannelListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcChannelListOutcomeCallable describeRtcChannelListCallable(const Model::DescribeRtcChannelListRequest& request) const;
			DescribeRtcChannelMetricOutcome describeRtcChannelMetric(const Model::DescribeRtcChannelMetricRequest &request)const;
			void describeRtcChannelMetricAsync(const Model::DescribeRtcChannelMetricRequest& request, const DescribeRtcChannelMetricAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcChannelMetricOutcomeCallable describeRtcChannelMetricCallable(const Model::DescribeRtcChannelMetricRequest& request) const;
			DescribeRtcChannelMetricsOutcome describeRtcChannelMetrics(const Model::DescribeRtcChannelMetricsRequest &request)const;
			void describeRtcChannelMetricsAsync(const Model::DescribeRtcChannelMetricsRequest& request, const DescribeRtcChannelMetricsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcChannelMetricsOutcomeCallable describeRtcChannelMetricsCallable(const Model::DescribeRtcChannelMetricsRequest& request) const;
			DescribeRtcChannelUserListOutcome describeRtcChannelUserList(const Model::DescribeRtcChannelUserListRequest &request)const;
			void describeRtcChannelUserListAsync(const Model::DescribeRtcChannelUserListRequest& request, const DescribeRtcChannelUserListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcChannelUserListOutcomeCallable describeRtcChannelUserListCallable(const Model::DescribeRtcChannelUserListRequest& request) const;
			DescribeRtcChannelsOutcome describeRtcChannels(const Model::DescribeRtcChannelsRequest &request)const;
			void describeRtcChannelsAsync(const Model::DescribeRtcChannelsRequest& request, const DescribeRtcChannelsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcChannelsOutcomeCallable describeRtcChannelsCallable(const Model::DescribeRtcChannelsRequest& request) const;
			DescribeRtcDurationDataOutcome describeRtcDurationData(const Model::DescribeRtcDurationDataRequest &request)const;
			void describeRtcDurationDataAsync(const Model::DescribeRtcDurationDataRequest& request, const DescribeRtcDurationDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcDurationDataOutcomeCallable describeRtcDurationDataCallable(const Model::DescribeRtcDurationDataRequest& request) const;
			DescribeRtcPeakChannelCntDataOutcome describeRtcPeakChannelCntData(const Model::DescribeRtcPeakChannelCntDataRequest &request)const;
			void describeRtcPeakChannelCntDataAsync(const Model::DescribeRtcPeakChannelCntDataRequest& request, const DescribeRtcPeakChannelCntDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcPeakChannelCntDataOutcomeCallable describeRtcPeakChannelCntDataCallable(const Model::DescribeRtcPeakChannelCntDataRequest& request) const;
			DescribeRtcPeakUserCntDataOutcome describeRtcPeakUserCntData(const Model::DescribeRtcPeakUserCntDataRequest &request)const;
			void describeRtcPeakUserCntDataAsync(const Model::DescribeRtcPeakUserCntDataRequest& request, const DescribeRtcPeakUserCntDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcPeakUserCntDataOutcomeCallable describeRtcPeakUserCntDataCallable(const Model::DescribeRtcPeakUserCntDataRequest& request) const;
			DescribeRtcQualityMetricOutcome describeRtcQualityMetric(const Model::DescribeRtcQualityMetricRequest &request)const;
			void describeRtcQualityMetricAsync(const Model::DescribeRtcQualityMetricRequest& request, const DescribeRtcQualityMetricAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcQualityMetricOutcomeCallable describeRtcQualityMetricCallable(const Model::DescribeRtcQualityMetricRequest& request) const;
			DescribeRtcScaleOutcome describeRtcScale(const Model::DescribeRtcScaleRequest &request)const;
			void describeRtcScaleAsync(const Model::DescribeRtcScaleRequest& request, const DescribeRtcScaleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcScaleOutcomeCallable describeRtcScaleCallable(const Model::DescribeRtcScaleRequest& request) const;
			DescribeRtcScaleDetailOutcome describeRtcScaleDetail(const Model::DescribeRtcScaleDetailRequest &request)const;
			void describeRtcScaleDetailAsync(const Model::DescribeRtcScaleDetailRequest& request, const DescribeRtcScaleDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcScaleDetailOutcomeCallable describeRtcScaleDetailCallable(const Model::DescribeRtcScaleDetailRequest& request) const;
			DescribeRtcUserCntDataOutcome describeRtcUserCntData(const Model::DescribeRtcUserCntDataRequest &request)const;
			void describeRtcUserCntDataAsync(const Model::DescribeRtcUserCntDataRequest& request, const DescribeRtcUserCntDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcUserCntDataOutcomeCallable describeRtcUserCntDataCallable(const Model::DescribeRtcUserCntDataRequest& request) const;
			DescribeRtcUserEventsOutcome describeRtcUserEvents(const Model::DescribeRtcUserEventsRequest &request)const;
			void describeRtcUserEventsAsync(const Model::DescribeRtcUserEventsRequest& request, const DescribeRtcUserEventsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcUserEventsOutcomeCallable describeRtcUserEventsCallable(const Model::DescribeRtcUserEventsRequest& request) const;
			DescribeRtcUserListOutcome describeRtcUserList(const Model::DescribeRtcUserListRequest &request)const;
			void describeRtcUserListAsync(const Model::DescribeRtcUserListRequest& request, const DescribeRtcUserListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRtcUserListOutcomeCallable describeRtcUserListCallable(const Model::DescribeRtcUserListRequest& request) const;
			DescribeUserInfoInChannelOutcome describeUserInfoInChannel(const Model::DescribeUserInfoInChannelRequest &request)const;
			void describeUserInfoInChannelAsync(const Model::DescribeUserInfoInChannelRequest& request, const DescribeUserInfoInChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeUserInfoInChannelOutcomeCallable describeUserInfoInChannelCallable(const Model::DescribeUserInfoInChannelRequest& request) const;
			DisableMPURuleOutcome disableMPURule(const Model::DisableMPURuleRequest &request)const;
			void disableMPURuleAsync(const Model::DisableMPURuleRequest& request, const DisableMPURuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DisableMPURuleOutcomeCallable disableMPURuleCallable(const Model::DisableMPURuleRequest& request) const;
			EnableMPURuleOutcome enableMPURule(const Model::EnableMPURuleRequest &request)const;
			void enableMPURuleAsync(const Model::EnableMPURuleRequest& request, const EnableMPURuleAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnableMPURuleOutcomeCallable enableMPURuleCallable(const Model::EnableMPURuleRequest& request) const;
			GetMPUTaskStatusOutcome getMPUTaskStatus(const Model::GetMPUTaskStatusRequest &request)const;
			void getMPUTaskStatusAsync(const Model::GetMPUTaskStatusRequest& request, const GetMPUTaskStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetMPUTaskStatusOutcomeCallable getMPUTaskStatusCallable(const Model::GetMPUTaskStatusRequest& request) const;
			ModifyAppOutcome modifyApp(const Model::ModifyAppRequest &request)const;
			void modifyAppAsync(const Model::ModifyAppRequest& request, const ModifyAppAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyAppOutcomeCallable modifyAppCallable(const Model::ModifyAppRequest& request) const;
			ModifyConferenceOutcome modifyConference(const Model::ModifyConferenceRequest &request)const;
			void modifyConferenceAsync(const Model::ModifyConferenceRequest& request, const ModifyConferenceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyConferenceOutcomeCallable modifyConferenceCallable(const Model::ModifyConferenceRequest& request) const;
			ModifyMPULayoutOutcome modifyMPULayout(const Model::ModifyMPULayoutRequest &request)const;
			void modifyMPULayoutAsync(const Model::ModifyMPULayoutRequest& request, const ModifyMPULayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyMPULayoutOutcomeCallable modifyMPULayoutCallable(const Model::ModifyMPULayoutRequest& request) const;
			MuteAudioOutcome muteAudio(const Model::MuteAudioRequest &request)const;
			void muteAudioAsync(const Model::MuteAudioRequest& request, const MuteAudioAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			MuteAudioOutcomeCallable muteAudioCallable(const Model::MuteAudioRequest& request) const;
			MuteAudioAllOutcome muteAudioAll(const Model::MuteAudioAllRequest &request)const;
			void muteAudioAllAsync(const Model::MuteAudioAllRequest& request, const MuteAudioAllAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			MuteAudioAllOutcomeCallable muteAudioAllCallable(const Model::MuteAudioAllRequest& request) const;
			ReceiveNotifyOutcome receiveNotify(const Model::ReceiveNotifyRequest &request)const;
			void receiveNotifyAsync(const Model::ReceiveNotifyRequest& request, const ReceiveNotifyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ReceiveNotifyOutcomeCallable receiveNotifyCallable(const Model::ReceiveNotifyRequest& request) const;
			RemoveParticipantsOutcome removeParticipants(const Model::RemoveParticipantsRequest &request)const;
			void removeParticipantsAsync(const Model::RemoveParticipantsRequest& request, const RemoveParticipantsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveParticipantsOutcomeCallable removeParticipantsCallable(const Model::RemoveParticipantsRequest& request) const;
			RemoveTerminalsOutcome removeTerminals(const Model::RemoveTerminalsRequest &request)const;
			void removeTerminalsAsync(const Model::RemoveTerminalsRequest& request, const RemoveTerminalsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RemoveTerminalsOutcomeCallable removeTerminalsCallable(const Model::RemoveTerminalsRequest& request) const;
			SetChannelPropertyOutcome setChannelProperty(const Model::SetChannelPropertyRequest &request)const;
			void setChannelPropertyAsync(const Model::SetChannelPropertyRequest& request, const SetChannelPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetChannelPropertyOutcomeCallable setChannelPropertyCallable(const Model::SetChannelPropertyRequest& request) const;
			StartMPUTaskOutcome startMPUTask(const Model::StartMPUTaskRequest &request)const;
			void startMPUTaskAsync(const Model::StartMPUTaskRequest& request, const StartMPUTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartMPUTaskOutcomeCallable startMPUTaskCallable(const Model::StartMPUTaskRequest& request) const;
			StartRecordTaskOutcome startRecordTask(const Model::StartRecordTaskRequest &request)const;
			void startRecordTaskAsync(const Model::StartRecordTaskRequest& request, const StartRecordTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartRecordTaskOutcomeCallable startRecordTaskCallable(const Model::StartRecordTaskRequest& request) const;
			StopChannelUserPublishOutcome stopChannelUserPublish(const Model::StopChannelUserPublishRequest &request)const;
			void stopChannelUserPublishAsync(const Model::StopChannelUserPublishRequest& request, const StopChannelUserPublishAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopChannelUserPublishOutcomeCallable stopChannelUserPublishCallable(const Model::StopChannelUserPublishRequest& request) const;
			StopMPUTaskOutcome stopMPUTask(const Model::StopMPUTaskRequest &request)const;
			void stopMPUTaskAsync(const Model::StopMPUTaskRequest& request, const StopMPUTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopMPUTaskOutcomeCallable stopMPUTaskCallable(const Model::StopMPUTaskRequest& request) const;
			StopRecordTaskOutcome stopRecordTask(const Model::StopRecordTaskRequest &request)const;
			void stopRecordTaskAsync(const Model::StopRecordTaskRequest& request, const StopRecordTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopRecordTaskOutcomeCallable stopRecordTaskCallable(const Model::StopRecordTaskRequest& request) const;
			UnmuteAudioOutcome unmuteAudio(const Model::UnmuteAudioRequest &request)const;
			void unmuteAudioAsync(const Model::UnmuteAudioRequest& request, const UnmuteAudioAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnmuteAudioOutcomeCallable unmuteAudioCallable(const Model::UnmuteAudioRequest& request) const;
			UnmuteAudioAllOutcome unmuteAudioAll(const Model::UnmuteAudioAllRequest &request)const;
			void unmuteAudioAllAsync(const Model::UnmuteAudioAllRequest& request, const UnmuteAudioAllAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnmuteAudioAllOutcomeCallable unmuteAudioAllCallable(const Model::UnmuteAudioAllRequest& request) const;
			UpdateChannelOutcome updateChannel(const Model::UpdateChannelRequest &request)const;
			void updateChannelAsync(const Model::UpdateChannelRequest& request, const UpdateChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateChannelOutcomeCallable updateChannelCallable(const Model::UpdateChannelRequest& request) const;
			UpdateMPULayoutOutcome updateMPULayout(const Model::UpdateMPULayoutRequest &request)const;
			void updateMPULayoutAsync(const Model::UpdateMPULayoutRequest& request, const UpdateMPULayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateMPULayoutOutcomeCallable updateMPULayoutCallable(const Model::UpdateMPULayoutRequest& request) const;
			UpdateRecordTaskOutcome updateRecordTask(const Model::UpdateRecordTaskRequest &request)const;
			void updateRecordTaskAsync(const Model::UpdateRecordTaskRequest& request, const UpdateRecordTaskAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRecordTaskOutcomeCallable updateRecordTaskCallable(const Model::UpdateRecordTaskRequest& request) const;
			UpdateRecordTemplateOutcome updateRecordTemplate(const Model::UpdateRecordTemplateRequest &request)const;
			void updateRecordTemplateAsync(const Model::UpdateRecordTemplateRequest& request, const UpdateRecordTemplateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateRecordTemplateOutcomeCallable updateRecordTemplateCallable(const Model::UpdateRecordTemplateRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_RTC_RTCCLIENT_H_

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

#ifndef ALIBABACLOUD_LIVE_LIVECLIENT_H_
#define ALIBABACLOUD_LIVE_LIVECLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "LiveExport.h"
#include "model/AddCasterComponentRequest.h"
#include "model/AddCasterComponentResult.h"
#include "model/AddCasterEpisodeRequest.h"
#include "model/AddCasterEpisodeResult.h"
#include "model/AddCasterEpisodeGroupRequest.h"
#include "model/AddCasterEpisodeGroupResult.h"
#include "model/AddCasterEpisodeGroupContentRequest.h"
#include "model/AddCasterEpisodeGroupContentResult.h"
#include "model/AddCasterLayoutRequest.h"
#include "model/AddCasterLayoutResult.h"
#include "model/AddCasterProgramRequest.h"
#include "model/AddCasterProgramResult.h"
#include "model/AddCasterVideoResourceRequest.h"
#include "model/AddCasterVideoResourceResult.h"
#include "model/AddCustomLiveStreamTranscodeRequest.h"
#include "model/AddCustomLiveStreamTranscodeResult.h"
#include "model/AddLiveASRConfigRequest.h"
#include "model/AddLiveASRConfigResult.h"
#include "model/AddLiveAppRecordConfigRequest.h"
#include "model/AddLiveAppRecordConfigResult.h"
#include "model/AddLiveAppSnapshotConfigRequest.h"
#include "model/AddLiveAppSnapshotConfigResult.h"
#include "model/AddLiveAudioAuditConfigRequest.h"
#include "model/AddLiveAudioAuditConfigResult.h"
#include "model/AddLiveAudioAuditNotifyConfigRequest.h"
#include "model/AddLiveAudioAuditNotifyConfigResult.h"
#include "model/AddLiveDetectNotifyConfigRequest.h"
#include "model/AddLiveDetectNotifyConfigResult.h"
#include "model/AddLiveDomainRequest.h"
#include "model/AddLiveDomainResult.h"
#include "model/AddLiveDomainMappingRequest.h"
#include "model/AddLiveDomainMappingResult.h"
#include "model/AddLiveDomainPlayMappingRequest.h"
#include "model/AddLiveDomainPlayMappingResult.h"
#include "model/AddLivePullStreamInfoConfigRequest.h"
#include "model/AddLivePullStreamInfoConfigResult.h"
#include "model/AddLiveRecordNotifyConfigRequest.h"
#include "model/AddLiveRecordNotifyConfigResult.h"
#include "model/AddLiveRecordVodConfigRequest.h"
#include "model/AddLiveRecordVodConfigResult.h"
#include "model/AddLiveSnapshotDetectPornConfigRequest.h"
#include "model/AddLiveSnapshotDetectPornConfigResult.h"
#include "model/AddLiveStreamTranscodeRequest.h"
#include "model/AddLiveStreamTranscodeResult.h"
#include "model/AddRtsLiveStreamTranscodeRequest.h"
#include "model/AddRtsLiveStreamTranscodeResult.h"
#include "model/AddTrancodeSEIRequest.h"
#include "model/AddTrancodeSEIResult.h"
#include "model/AllowPushStreamRequest.h"
#include "model/AllowPushStreamResult.h"
#include "model/ApplyBoardTokenRequest.h"
#include "model/ApplyBoardTokenResult.h"
#include "model/ApplyRecordTokenRequest.h"
#include "model/ApplyRecordTokenResult.h"
#include "model/BatchDeleteLiveDomainConfigsRequest.h"
#include "model/BatchDeleteLiveDomainConfigsResult.h"
#include "model/BatchSetLiveDomainConfigsRequest.h"
#include "model/BatchSetLiveDomainConfigsResult.h"
#include "model/CompleteBoardRequest.h"
#include "model/CompleteBoardResult.h"
#include "model/CompleteBoardRecordRequest.h"
#include "model/CompleteBoardRecordResult.h"
#include "model/ControlHtmlResourceRequest.h"
#include "model/ControlHtmlResourceResult.h"
#include "model/CopyCasterRequest.h"
#include "model/CopyCasterResult.h"
#include "model/CopyCasterSceneConfigRequest.h"
#include "model/CopyCasterSceneConfigResult.h"
#include "model/CreateBoardRequest.h"
#include "model/CreateBoardResult.h"
#include "model/CreateCasterRequest.h"
#include "model/CreateCasterResult.h"
#include "model/CreateLiveRealTimeLogDeliveryRequest.h"
#include "model/CreateLiveRealTimeLogDeliveryResult.h"
#include "model/CreateLiveStreamRecordIndexFilesRequest.h"
#include "model/CreateLiveStreamRecordIndexFilesResult.h"
#include "model/CreateMixStreamRequest.h"
#include "model/CreateMixStreamResult.h"
#include "model/CreateRoomRequest.h"
#include "model/CreateRoomResult.h"
#include "model/DeleteBoardRequest.h"
#include "model/DeleteBoardResult.h"
#include "model/DeleteCasterRequest.h"
#include "model/DeleteCasterResult.h"
#include "model/DeleteCasterComponentRequest.h"
#include "model/DeleteCasterComponentResult.h"
#include "model/DeleteCasterEpisodeRequest.h"
#include "model/DeleteCasterEpisodeResult.h"
#include "model/DeleteCasterEpisodeGroupRequest.h"
#include "model/DeleteCasterEpisodeGroupResult.h"
#include "model/DeleteCasterLayoutRequest.h"
#include "model/DeleteCasterLayoutResult.h"
#include "model/DeleteCasterProgramRequest.h"
#include "model/DeleteCasterProgramResult.h"
#include "model/DeleteCasterSceneConfigRequest.h"
#include "model/DeleteCasterSceneConfigResult.h"
#include "model/DeleteCasterVideoResourceRequest.h"
#include "model/DeleteCasterVideoResourceResult.h"
#include "model/DeleteHtmlResourceRequest.h"
#include "model/DeleteHtmlResourceResult.h"
#include "model/DeleteLiveASRConfigRequest.h"
#include "model/DeleteLiveASRConfigResult.h"
#include "model/DeleteLiveAppRecordConfigRequest.h"
#include "model/DeleteLiveAppRecordConfigResult.h"
#include "model/DeleteLiveAppSnapshotConfigRequest.h"
#include "model/DeleteLiveAppSnapshotConfigResult.h"
#include "model/DeleteLiveAudioAuditConfigRequest.h"
#include "model/DeleteLiveAudioAuditConfigResult.h"
#include "model/DeleteLiveAudioAuditNotifyConfigRequest.h"
#include "model/DeleteLiveAudioAuditNotifyConfigResult.h"
#include "model/DeleteLiveDetectNotifyConfigRequest.h"
#include "model/DeleteLiveDetectNotifyConfigResult.h"
#include "model/DeleteLiveDomainRequest.h"
#include "model/DeleteLiveDomainResult.h"
#include "model/DeleteLiveDomainMappingRequest.h"
#include "model/DeleteLiveDomainMappingResult.h"
#include "model/DeleteLiveLazyPullStreamInfoConfigRequest.h"
#include "model/DeleteLiveLazyPullStreamInfoConfigResult.h"
#include "model/DeleteLivePullStreamInfoConfigRequest.h"
#include "model/DeleteLivePullStreamInfoConfigResult.h"
#include "model/DeleteLiveRealTimeLogLogstoreRequest.h"
#include "model/DeleteLiveRealTimeLogLogstoreResult.h"
#include "model/DeleteLiveRealtimeLogDeliveryRequest.h"
#include "model/DeleteLiveRealtimeLogDeliveryResult.h"
#include "model/DeleteLiveRecordNotifyConfigRequest.h"
#include "model/DeleteLiveRecordNotifyConfigResult.h"
#include "model/DeleteLiveRecordVodConfigRequest.h"
#include "model/DeleteLiveRecordVodConfigResult.h"
#include "model/DeleteLiveSnapshotDetectPornConfigRequest.h"
#include "model/DeleteLiveSnapshotDetectPornConfigResult.h"
#include "model/DeleteLiveStreamTranscodeRequest.h"
#include "model/DeleteLiveStreamTranscodeResult.h"
#include "model/DeleteLiveStreamsNotifyUrlConfigRequest.h"
#include "model/DeleteLiveStreamsNotifyUrlConfigResult.h"
#include "model/DeleteMixStreamRequest.h"
#include "model/DeleteMixStreamResult.h"
#include "model/DeleteRoomRequest.h"
#include "model/DeleteRoomResult.h"
#include "model/DescribeBoardEventsRequest.h"
#include "model/DescribeBoardEventsResult.h"
#include "model/DescribeBoardSnapshotRequest.h"
#include "model/DescribeBoardSnapshotResult.h"
#include "model/DescribeBoardsRequest.h"
#include "model/DescribeBoardsResult.h"
#include "model/DescribeCasterChannelsRequest.h"
#include "model/DescribeCasterChannelsResult.h"
#include "model/DescribeCasterComponentsRequest.h"
#include "model/DescribeCasterComponentsResult.h"
#include "model/DescribeCasterConfigRequest.h"
#include "model/DescribeCasterConfigResult.h"
#include "model/DescribeCasterLayoutsRequest.h"
#include "model/DescribeCasterLayoutsResult.h"
#include "model/DescribeCasterProgramRequest.h"
#include "model/DescribeCasterProgramResult.h"
#include "model/DescribeCasterRtcInfoRequest.h"
#include "model/DescribeCasterRtcInfoResult.h"
#include "model/DescribeCasterSceneAudioRequest.h"
#include "model/DescribeCasterSceneAudioResult.h"
#include "model/DescribeCasterScenesRequest.h"
#include "model/DescribeCasterScenesResult.h"
#include "model/DescribeCasterStreamUrlRequest.h"
#include "model/DescribeCasterStreamUrlResult.h"
#include "model/DescribeCasterSyncGroupRequest.h"
#include "model/DescribeCasterSyncGroupResult.h"
#include "model/DescribeCasterVideoResourcesRequest.h"
#include "model/DescribeCasterVideoResourcesResult.h"
#include "model/DescribeCastersRequest.h"
#include "model/DescribeCastersResult.h"
#include "model/DescribeForbidPushStreamRoomListRequest.h"
#include "model/DescribeForbidPushStreamRoomListResult.h"
#include "model/DescribeHlsLiveStreamRealTimeBpsDataRequest.h"
#include "model/DescribeHlsLiveStreamRealTimeBpsDataResult.h"
#include "model/DescribeHtmlResourceRequest.h"
#include "model/DescribeHtmlResourceResult.h"
#include "model/DescribeLiveAsrConfigRequest.h"
#include "model/DescribeLiveAsrConfigResult.h"
#include "model/DescribeLiveAudioAuditConfigRequest.h"
#include "model/DescribeLiveAudioAuditConfigResult.h"
#include "model/DescribeLiveAudioAuditNotifyConfigRequest.h"
#include "model/DescribeLiveAudioAuditNotifyConfigResult.h"
#include "model/DescribeLiveCertificateDetailRequest.h"
#include "model/DescribeLiveCertificateDetailResult.h"
#include "model/DescribeLiveCertificateListRequest.h"
#include "model/DescribeLiveCertificateListResult.h"
#include "model/DescribeLiveDetectNotifyConfigRequest.h"
#include "model/DescribeLiveDetectNotifyConfigResult.h"
#include "model/DescribeLiveDetectPornDataRequest.h"
#include "model/DescribeLiveDetectPornDataResult.h"
#include "model/DescribeLiveDomainBpsDataRequest.h"
#include "model/DescribeLiveDomainBpsDataResult.h"
#include "model/DescribeLiveDomainBpsDataByTimeStampRequest.h"
#include "model/DescribeLiveDomainBpsDataByTimeStampResult.h"
#include "model/DescribeLiveDomainCertificateInfoRequest.h"
#include "model/DescribeLiveDomainCertificateInfoResult.h"
#include "model/DescribeLiveDomainConfigsRequest.h"
#include "model/DescribeLiveDomainConfigsResult.h"
#include "model/DescribeLiveDomainDetailRequest.h"
#include "model/DescribeLiveDomainDetailResult.h"
#include "model/DescribeLiveDomainFrameRateAndBitRateDataRequest.h"
#include "model/DescribeLiveDomainFrameRateAndBitRateDataResult.h"
#include "model/DescribeLiveDomainLimitRequest.h"
#include "model/DescribeLiveDomainLimitResult.h"
#include "model/DescribeLiveDomainMappingRequest.h"
#include "model/DescribeLiveDomainMappingResult.h"
#include "model/DescribeLiveDomainOnlineUserNumRequest.h"
#include "model/DescribeLiveDomainOnlineUserNumResult.h"
#include "model/DescribeLiveDomainPushBpsDataRequest.h"
#include "model/DescribeLiveDomainPushBpsDataResult.h"
#include "model/DescribeLiveDomainPushTrafficDataRequest.h"
#include "model/DescribeLiveDomainPushTrafficDataResult.h"
#include "model/DescribeLiveDomainRealTimeBpsDataRequest.h"
#include "model/DescribeLiveDomainRealTimeBpsDataResult.h"
#include "model/DescribeLiveDomainRealTimeHttpCodeDataRequest.h"
#include "model/DescribeLiveDomainRealTimeHttpCodeDataResult.h"
#include "model/DescribeLiveDomainRealTimeTrafficDataRequest.h"
#include "model/DescribeLiveDomainRealTimeTrafficDataResult.h"
#include "model/DescribeLiveDomainRealtimeLogDeliveryRequest.h"
#include "model/DescribeLiveDomainRealtimeLogDeliveryResult.h"
#include "model/DescribeLiveDomainRecordDataRequest.h"
#include "model/DescribeLiveDomainRecordDataResult.h"
#include "model/DescribeLiveDomainSnapshotDataRequest.h"
#include "model/DescribeLiveDomainSnapshotDataResult.h"
#include "model/DescribeLiveDomainTimeShiftDataRequest.h"
#include "model/DescribeLiveDomainTimeShiftDataResult.h"
#include "model/DescribeLiveDomainTrafficDataRequest.h"
#include "model/DescribeLiveDomainTrafficDataResult.h"
#include "model/DescribeLiveDomainTranscodeDataRequest.h"
#include "model/DescribeLiveDomainTranscodeDataResult.h"
#include "model/DescribeLiveLazyPullStreamConfigRequest.h"
#include "model/DescribeLiveLazyPullStreamConfigResult.h"
#include "model/DescribeLivePullStreamConfigRequest.h"
#include "model/DescribeLivePullStreamConfigResult.h"
#include "model/DescribeLiveRealtimeDeliveryAccRequest.h"
#include "model/DescribeLiveRealtimeDeliveryAccResult.h"
#include "model/DescribeLiveRealtimeLogAuthorizedRequest.h"
#include "model/DescribeLiveRealtimeLogAuthorizedResult.h"
#include "model/DescribeLiveRecordConfigRequest.h"
#include "model/DescribeLiveRecordConfigResult.h"
#include "model/DescribeLiveRecordNotifyConfigRequest.h"
#include "model/DescribeLiveRecordNotifyConfigResult.h"
#include "model/DescribeLiveRecordVodConfigsRequest.h"
#include "model/DescribeLiveRecordVodConfigsResult.h"
#include "model/DescribeLiveSnapshotConfigRequest.h"
#include "model/DescribeLiveSnapshotConfigResult.h"
#include "model/DescribeLiveSnapshotDetectPornConfigRequest.h"
#include "model/DescribeLiveSnapshotDetectPornConfigResult.h"
#include "model/DescribeLiveStreamBitRateDataRequest.h"
#include "model/DescribeLiveStreamBitRateDataResult.h"
#include "model/DescribeLiveStreamCountRequest.h"
#include "model/DescribeLiveStreamCountResult.h"
#include "model/DescribeLiveStreamDelayConfigRequest.h"
#include "model/DescribeLiveStreamDelayConfigResult.h"
#include "model/DescribeLiveStreamHistoryUserNumRequest.h"
#include "model/DescribeLiveStreamHistoryUserNumResult.h"
#include "model/DescribeLiveStreamOnlineUserNumRequest.h"
#include "model/DescribeLiveStreamOnlineUserNumResult.h"
#include "model/DescribeLiveStreamOptimizedFeatureConfigRequest.h"
#include "model/DescribeLiveStreamOptimizedFeatureConfigResult.h"
#include "model/DescribeLiveStreamRecordContentRequest.h"
#include "model/DescribeLiveStreamRecordContentResult.h"
#include "model/DescribeLiveStreamRecordIndexFileRequest.h"
#include "model/DescribeLiveStreamRecordIndexFileResult.h"
#include "model/DescribeLiveStreamRecordIndexFilesRequest.h"
#include "model/DescribeLiveStreamRecordIndexFilesResult.h"
#include "model/DescribeLiveStreamSnapshotInfoRequest.h"
#include "model/DescribeLiveStreamSnapshotInfoResult.h"
#include "model/DescribeLiveStreamTranscodeInfoRequest.h"
#include "model/DescribeLiveStreamTranscodeInfoResult.h"
#include "model/DescribeLiveStreamTranscodeStreamNumRequest.h"
#include "model/DescribeLiveStreamTranscodeStreamNumResult.h"
#include "model/DescribeLiveStreamsBlockListRequest.h"
#include "model/DescribeLiveStreamsBlockListResult.h"
#include "model/DescribeLiveStreamsControlHistoryRequest.h"
#include "model/DescribeLiveStreamsControlHistoryResult.h"
#include "model/DescribeLiveStreamsFrameRateAndBitRateDataRequest.h"
#include "model/DescribeLiveStreamsFrameRateAndBitRateDataResult.h"
#include "model/DescribeLiveStreamsNotifyUrlConfigRequest.h"
#include "model/DescribeLiveStreamsNotifyUrlConfigResult.h"
#include "model/DescribeLiveStreamsOnlineListRequest.h"
#include "model/DescribeLiveStreamsOnlineListResult.h"
#include "model/DescribeLiveStreamsPublishListRequest.h"
#include "model/DescribeLiveStreamsPublishListResult.h"
#include "model/DescribeLiveTagResourcesRequest.h"
#include "model/DescribeLiveTagResourcesResult.h"
#include "model/DescribeLiveTopDomainsByFlowRequest.h"
#include "model/DescribeLiveTopDomainsByFlowResult.h"
#include "model/DescribeLiveUserDomainsRequest.h"
#include "model/DescribeLiveUserDomainsResult.h"
#include "model/DescribeLiveUserTagsRequest.h"
#include "model/DescribeLiveUserTagsResult.h"
#include "model/DescribeMixStreamListRequest.h"
#include "model/DescribeMixStreamListResult.h"
#include "model/DescribeRecordRequest.h"
#include "model/DescribeRecordResult.h"
#include "model/DescribeRecordsRequest.h"
#include "model/DescribeRecordsResult.h"
#include "model/DescribeRoomKickoutUserListRequest.h"
#include "model/DescribeRoomKickoutUserListResult.h"
#include "model/DescribeRoomListRequest.h"
#include "model/DescribeRoomListResult.h"
#include "model/DescribeRoomStatusRequest.h"
#include "model/DescribeRoomStatusResult.h"
#include "model/DescribeUpBpsPeakDataRequest.h"
#include "model/DescribeUpBpsPeakDataResult.h"
#include "model/DescribeUpBpsPeakOfLineRequest.h"
#include "model/DescribeUpBpsPeakOfLineResult.h"
#include "model/DescribeUpPeakPublishStreamDataRequest.h"
#include "model/DescribeUpPeakPublishStreamDataResult.h"
#include "model/DisableLiveRealtimeLogDeliveryRequest.h"
#include "model/DisableLiveRealtimeLogDeliveryResult.h"
#include "model/EditHtmlResourceRequest.h"
#include "model/EditHtmlResourceResult.h"
#include "model/EffectCasterUrgentRequest.h"
#include "model/EffectCasterUrgentResult.h"
#include "model/EffectCasterVideoResourceRequest.h"
#include "model/EffectCasterVideoResourceResult.h"
#include "model/EnableLiveRealtimeLogDeliveryRequest.h"
#include "model/EnableLiveRealtimeLogDeliveryResult.h"
#include "model/ForbidLiveStreamRequest.h"
#include "model/ForbidLiveStreamResult.h"
#include "model/ForbidPushStreamRequest.h"
#include "model/ForbidPushStreamResult.h"
#include "model/JoinBoardRequest.h"
#include "model/JoinBoardResult.h"
#include "model/ListLiveRealtimeLogDeliveryRequest.h"
#include "model/ListLiveRealtimeLogDeliveryResult.h"
#include "model/ListLiveRealtimeLogDeliveryDomainsRequest.h"
#include "model/ListLiveRealtimeLogDeliveryDomainsResult.h"
#include "model/ListLiveRealtimeLogDeliveryInfosRequest.h"
#include "model/ListLiveRealtimeLogDeliveryInfosResult.h"
#include "model/ModifyCasterComponentRequest.h"
#include "model/ModifyCasterComponentResult.h"
#include "model/ModifyCasterEpisodeRequest.h"
#include "model/ModifyCasterEpisodeResult.h"
#include "model/ModifyCasterLayoutRequest.h"
#include "model/ModifyCasterLayoutResult.h"
#include "model/ModifyCasterProgramRequest.h"
#include "model/ModifyCasterProgramResult.h"
#include "model/ModifyCasterVideoResourceRequest.h"
#include "model/ModifyCasterVideoResourceResult.h"
#include "model/ModifyLiveDomainSchdmByPropertyRequest.h"
#include "model/ModifyLiveDomainSchdmByPropertyResult.h"
#include "model/ModifyLiveRealtimeLogDeliveryRequest.h"
#include "model/ModifyLiveRealtimeLogDeliveryResult.h"
#include "model/RealTimeRecordCommandRequest.h"
#include "model/RealTimeRecordCommandResult.h"
#include "model/RealTimeSnapshotCommandRequest.h"
#include "model/RealTimeSnapshotCommandResult.h"
#include "model/ResumeLiveStreamRequest.h"
#include "model/ResumeLiveStreamResult.h"
#include "model/SendRoomNotificationRequest.h"
#include "model/SendRoomNotificationResult.h"
#include "model/SendRoomUserNotificationRequest.h"
#include "model/SendRoomUserNotificationResult.h"
#include "model/SetBoardCallbackRequest.h"
#include "model/SetBoardCallbackResult.h"
#include "model/SetCasterChannelRequest.h"
#include "model/SetCasterChannelResult.h"
#include "model/SetCasterConfigRequest.h"
#include "model/SetCasterConfigResult.h"
#include "model/SetCasterSceneConfigRequest.h"
#include "model/SetCasterSceneConfigResult.h"
#include "model/SetCasterSyncGroupRequest.h"
#include "model/SetCasterSyncGroupResult.h"
#include "model/SetLiveDomainCertificateRequest.h"
#include "model/SetLiveDomainCertificateResult.h"
#include "model/SetLiveLazyPullStreamInfoConfigRequest.h"
#include "model/SetLiveLazyPullStreamInfoConfigResult.h"
#include "model/SetLiveStreamDelayConfigRequest.h"
#include "model/SetLiveStreamDelayConfigResult.h"
#include "model/SetLiveStreamOptimizedFeatureConfigRequest.h"
#include "model/SetLiveStreamOptimizedFeatureConfigResult.h"
#include "model/SetLiveStreamsNotifyUrlConfigRequest.h"
#include "model/SetLiveStreamsNotifyUrlConfigResult.h"
#include "model/StartBoardRecordRequest.h"
#include "model/StartBoardRecordResult.h"
#include "model/StartCasterRequest.h"
#include "model/StartCasterResult.h"
#include "model/StartCasterSceneRequest.h"
#include "model/StartCasterSceneResult.h"
#include "model/StartLiveDomainRequest.h"
#include "model/StartLiveDomainResult.h"
#include "model/StartLiveIndexRequest.h"
#include "model/StartLiveIndexResult.h"
#include "model/StopCasterRequest.h"
#include "model/StopCasterResult.h"
#include "model/StopCasterSceneRequest.h"
#include "model/StopCasterSceneResult.h"
#include "model/StopLiveDomainRequest.h"
#include "model/StopLiveDomainResult.h"
#include "model/StopLiveIndexRequest.h"
#include "model/StopLiveIndexResult.h"
#include "model/TagLiveResourcesRequest.h"
#include "model/TagLiveResourcesResult.h"
#include "model/UnTagLiveResourcesRequest.h"
#include "model/UnTagLiveResourcesResult.h"
#include "model/UpdateBoardRequest.h"
#include "model/UpdateBoardResult.h"
#include "model/UpdateBoardCallbackRequest.h"
#include "model/UpdateBoardCallbackResult.h"
#include "model/UpdateCasterSceneAudioRequest.h"
#include "model/UpdateCasterSceneAudioResult.h"
#include "model/UpdateCasterSceneConfigRequest.h"
#include "model/UpdateCasterSceneConfigResult.h"
#include "model/UpdateLiveASRConfigRequest.h"
#include "model/UpdateLiveASRConfigResult.h"
#include "model/UpdateLiveAppSnapshotConfigRequest.h"
#include "model/UpdateLiveAppSnapshotConfigResult.h"
#include "model/UpdateLiveAudioAuditConfigRequest.h"
#include "model/UpdateLiveAudioAuditConfigResult.h"
#include "model/UpdateLiveAudioAuditNotifyConfigRequest.h"
#include "model/UpdateLiveAudioAuditNotifyConfigResult.h"
#include "model/UpdateLiveDetectNotifyConfigRequest.h"
#include "model/UpdateLiveDetectNotifyConfigResult.h"
#include "model/UpdateLiveRecordNotifyConfigRequest.h"
#include "model/UpdateLiveRecordNotifyConfigResult.h"
#include "model/UpdateLiveSnapshotDetectPornConfigRequest.h"
#include "model/UpdateLiveSnapshotDetectPornConfigResult.h"
#include "model/UpdateLiveTopLevelDomainRequest.h"
#include "model/UpdateLiveTopLevelDomainResult.h"
#include "model/UpdateMixStreamRequest.h"
#include "model/UpdateMixStreamResult.h"


namespace AlibabaCloud
{
	namespace Live
	{
		class ALIBABACLOUD_LIVE_EXPORT LiveClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::AddCasterComponentResult> AddCasterComponentOutcome;
			typedef std::future<AddCasterComponentOutcome> AddCasterComponentOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCasterComponentRequest&, const AddCasterComponentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCasterComponentAsyncHandler;
			typedef Outcome<Error, Model::AddCasterEpisodeResult> AddCasterEpisodeOutcome;
			typedef std::future<AddCasterEpisodeOutcome> AddCasterEpisodeOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCasterEpisodeRequest&, const AddCasterEpisodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCasterEpisodeAsyncHandler;
			typedef Outcome<Error, Model::AddCasterEpisodeGroupResult> AddCasterEpisodeGroupOutcome;
			typedef std::future<AddCasterEpisodeGroupOutcome> AddCasterEpisodeGroupOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCasterEpisodeGroupRequest&, const AddCasterEpisodeGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCasterEpisodeGroupAsyncHandler;
			typedef Outcome<Error, Model::AddCasterEpisodeGroupContentResult> AddCasterEpisodeGroupContentOutcome;
			typedef std::future<AddCasterEpisodeGroupContentOutcome> AddCasterEpisodeGroupContentOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCasterEpisodeGroupContentRequest&, const AddCasterEpisodeGroupContentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCasterEpisodeGroupContentAsyncHandler;
			typedef Outcome<Error, Model::AddCasterLayoutResult> AddCasterLayoutOutcome;
			typedef std::future<AddCasterLayoutOutcome> AddCasterLayoutOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCasterLayoutRequest&, const AddCasterLayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCasterLayoutAsyncHandler;
			typedef Outcome<Error, Model::AddCasterProgramResult> AddCasterProgramOutcome;
			typedef std::future<AddCasterProgramOutcome> AddCasterProgramOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCasterProgramRequest&, const AddCasterProgramOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCasterProgramAsyncHandler;
			typedef Outcome<Error, Model::AddCasterVideoResourceResult> AddCasterVideoResourceOutcome;
			typedef std::future<AddCasterVideoResourceOutcome> AddCasterVideoResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCasterVideoResourceRequest&, const AddCasterVideoResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCasterVideoResourceAsyncHandler;
			typedef Outcome<Error, Model::AddCustomLiveStreamTranscodeResult> AddCustomLiveStreamTranscodeOutcome;
			typedef std::future<AddCustomLiveStreamTranscodeOutcome> AddCustomLiveStreamTranscodeOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddCustomLiveStreamTranscodeRequest&, const AddCustomLiveStreamTranscodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddCustomLiveStreamTranscodeAsyncHandler;
			typedef Outcome<Error, Model::AddLiveASRConfigResult> AddLiveASRConfigOutcome;
			typedef std::future<AddLiveASRConfigOutcome> AddLiveASRConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveASRConfigRequest&, const AddLiveASRConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveASRConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveAppRecordConfigResult> AddLiveAppRecordConfigOutcome;
			typedef std::future<AddLiveAppRecordConfigOutcome> AddLiveAppRecordConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveAppRecordConfigRequest&, const AddLiveAppRecordConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveAppRecordConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveAppSnapshotConfigResult> AddLiveAppSnapshotConfigOutcome;
			typedef std::future<AddLiveAppSnapshotConfigOutcome> AddLiveAppSnapshotConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveAppSnapshotConfigRequest&, const AddLiveAppSnapshotConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveAppSnapshotConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveAudioAuditConfigResult> AddLiveAudioAuditConfigOutcome;
			typedef std::future<AddLiveAudioAuditConfigOutcome> AddLiveAudioAuditConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveAudioAuditConfigRequest&, const AddLiveAudioAuditConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveAudioAuditConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveAudioAuditNotifyConfigResult> AddLiveAudioAuditNotifyConfigOutcome;
			typedef std::future<AddLiveAudioAuditNotifyConfigOutcome> AddLiveAudioAuditNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveAudioAuditNotifyConfigRequest&, const AddLiveAudioAuditNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveAudioAuditNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveDetectNotifyConfigResult> AddLiveDetectNotifyConfigOutcome;
			typedef std::future<AddLiveDetectNotifyConfigOutcome> AddLiveDetectNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveDetectNotifyConfigRequest&, const AddLiveDetectNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveDetectNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveDomainResult> AddLiveDomainOutcome;
			typedef std::future<AddLiveDomainOutcome> AddLiveDomainOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveDomainRequest&, const AddLiveDomainOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveDomainAsyncHandler;
			typedef Outcome<Error, Model::AddLiveDomainMappingResult> AddLiveDomainMappingOutcome;
			typedef std::future<AddLiveDomainMappingOutcome> AddLiveDomainMappingOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveDomainMappingRequest&, const AddLiveDomainMappingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveDomainMappingAsyncHandler;
			typedef Outcome<Error, Model::AddLiveDomainPlayMappingResult> AddLiveDomainPlayMappingOutcome;
			typedef std::future<AddLiveDomainPlayMappingOutcome> AddLiveDomainPlayMappingOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveDomainPlayMappingRequest&, const AddLiveDomainPlayMappingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveDomainPlayMappingAsyncHandler;
			typedef Outcome<Error, Model::AddLivePullStreamInfoConfigResult> AddLivePullStreamInfoConfigOutcome;
			typedef std::future<AddLivePullStreamInfoConfigOutcome> AddLivePullStreamInfoConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLivePullStreamInfoConfigRequest&, const AddLivePullStreamInfoConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLivePullStreamInfoConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveRecordNotifyConfigResult> AddLiveRecordNotifyConfigOutcome;
			typedef std::future<AddLiveRecordNotifyConfigOutcome> AddLiveRecordNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveRecordNotifyConfigRequest&, const AddLiveRecordNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveRecordNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveRecordVodConfigResult> AddLiveRecordVodConfigOutcome;
			typedef std::future<AddLiveRecordVodConfigOutcome> AddLiveRecordVodConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveRecordVodConfigRequest&, const AddLiveRecordVodConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveRecordVodConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveSnapshotDetectPornConfigResult> AddLiveSnapshotDetectPornConfigOutcome;
			typedef std::future<AddLiveSnapshotDetectPornConfigOutcome> AddLiveSnapshotDetectPornConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveSnapshotDetectPornConfigRequest&, const AddLiveSnapshotDetectPornConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveSnapshotDetectPornConfigAsyncHandler;
			typedef Outcome<Error, Model::AddLiveStreamTranscodeResult> AddLiveStreamTranscodeOutcome;
			typedef std::future<AddLiveStreamTranscodeOutcome> AddLiveStreamTranscodeOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddLiveStreamTranscodeRequest&, const AddLiveStreamTranscodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddLiveStreamTranscodeAsyncHandler;
			typedef Outcome<Error, Model::AddRtsLiveStreamTranscodeResult> AddRtsLiveStreamTranscodeOutcome;
			typedef std::future<AddRtsLiveStreamTranscodeOutcome> AddRtsLiveStreamTranscodeOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddRtsLiveStreamTranscodeRequest&, const AddRtsLiveStreamTranscodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddRtsLiveStreamTranscodeAsyncHandler;
			typedef Outcome<Error, Model::AddTrancodeSEIResult> AddTrancodeSEIOutcome;
			typedef std::future<AddTrancodeSEIOutcome> AddTrancodeSEIOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AddTrancodeSEIRequest&, const AddTrancodeSEIOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AddTrancodeSEIAsyncHandler;
			typedef Outcome<Error, Model::AllowPushStreamResult> AllowPushStreamOutcome;
			typedef std::future<AllowPushStreamOutcome> AllowPushStreamOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::AllowPushStreamRequest&, const AllowPushStreamOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> AllowPushStreamAsyncHandler;
			typedef Outcome<Error, Model::ApplyBoardTokenResult> ApplyBoardTokenOutcome;
			typedef std::future<ApplyBoardTokenOutcome> ApplyBoardTokenOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ApplyBoardTokenRequest&, const ApplyBoardTokenOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ApplyBoardTokenAsyncHandler;
			typedef Outcome<Error, Model::ApplyRecordTokenResult> ApplyRecordTokenOutcome;
			typedef std::future<ApplyRecordTokenOutcome> ApplyRecordTokenOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ApplyRecordTokenRequest&, const ApplyRecordTokenOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ApplyRecordTokenAsyncHandler;
			typedef Outcome<Error, Model::BatchDeleteLiveDomainConfigsResult> BatchDeleteLiveDomainConfigsOutcome;
			typedef std::future<BatchDeleteLiveDomainConfigsOutcome> BatchDeleteLiveDomainConfigsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::BatchDeleteLiveDomainConfigsRequest&, const BatchDeleteLiveDomainConfigsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchDeleteLiveDomainConfigsAsyncHandler;
			typedef Outcome<Error, Model::BatchSetLiveDomainConfigsResult> BatchSetLiveDomainConfigsOutcome;
			typedef std::future<BatchSetLiveDomainConfigsOutcome> BatchSetLiveDomainConfigsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::BatchSetLiveDomainConfigsRequest&, const BatchSetLiveDomainConfigsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchSetLiveDomainConfigsAsyncHandler;
			typedef Outcome<Error, Model::CompleteBoardResult> CompleteBoardOutcome;
			typedef std::future<CompleteBoardOutcome> CompleteBoardOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CompleteBoardRequest&, const CompleteBoardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CompleteBoardAsyncHandler;
			typedef Outcome<Error, Model::CompleteBoardRecordResult> CompleteBoardRecordOutcome;
			typedef std::future<CompleteBoardRecordOutcome> CompleteBoardRecordOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CompleteBoardRecordRequest&, const CompleteBoardRecordOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CompleteBoardRecordAsyncHandler;
			typedef Outcome<Error, Model::ControlHtmlResourceResult> ControlHtmlResourceOutcome;
			typedef std::future<ControlHtmlResourceOutcome> ControlHtmlResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ControlHtmlResourceRequest&, const ControlHtmlResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ControlHtmlResourceAsyncHandler;
			typedef Outcome<Error, Model::CopyCasterResult> CopyCasterOutcome;
			typedef std::future<CopyCasterOutcome> CopyCasterOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CopyCasterRequest&, const CopyCasterOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CopyCasterAsyncHandler;
			typedef Outcome<Error, Model::CopyCasterSceneConfigResult> CopyCasterSceneConfigOutcome;
			typedef std::future<CopyCasterSceneConfigOutcome> CopyCasterSceneConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CopyCasterSceneConfigRequest&, const CopyCasterSceneConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CopyCasterSceneConfigAsyncHandler;
			typedef Outcome<Error, Model::CreateBoardResult> CreateBoardOutcome;
			typedef std::future<CreateBoardOutcome> CreateBoardOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CreateBoardRequest&, const CreateBoardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateBoardAsyncHandler;
			typedef Outcome<Error, Model::CreateCasterResult> CreateCasterOutcome;
			typedef std::future<CreateCasterOutcome> CreateCasterOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CreateCasterRequest&, const CreateCasterOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateCasterAsyncHandler;
			typedef Outcome<Error, Model::CreateLiveRealTimeLogDeliveryResult> CreateLiveRealTimeLogDeliveryOutcome;
			typedef std::future<CreateLiveRealTimeLogDeliveryOutcome> CreateLiveRealTimeLogDeliveryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CreateLiveRealTimeLogDeliveryRequest&, const CreateLiveRealTimeLogDeliveryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateLiveRealTimeLogDeliveryAsyncHandler;
			typedef Outcome<Error, Model::CreateLiveStreamRecordIndexFilesResult> CreateLiveStreamRecordIndexFilesOutcome;
			typedef std::future<CreateLiveStreamRecordIndexFilesOutcome> CreateLiveStreamRecordIndexFilesOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CreateLiveStreamRecordIndexFilesRequest&, const CreateLiveStreamRecordIndexFilesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateLiveStreamRecordIndexFilesAsyncHandler;
			typedef Outcome<Error, Model::CreateMixStreamResult> CreateMixStreamOutcome;
			typedef std::future<CreateMixStreamOutcome> CreateMixStreamOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CreateMixStreamRequest&, const CreateMixStreamOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateMixStreamAsyncHandler;
			typedef Outcome<Error, Model::CreateRoomResult> CreateRoomOutcome;
			typedef std::future<CreateRoomOutcome> CreateRoomOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::CreateRoomRequest&, const CreateRoomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateRoomAsyncHandler;
			typedef Outcome<Error, Model::DeleteBoardResult> DeleteBoardOutcome;
			typedef std::future<DeleteBoardOutcome> DeleteBoardOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteBoardRequest&, const DeleteBoardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteBoardAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterResult> DeleteCasterOutcome;
			typedef std::future<DeleteCasterOutcome> DeleteCasterOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterRequest&, const DeleteCasterOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterComponentResult> DeleteCasterComponentOutcome;
			typedef std::future<DeleteCasterComponentOutcome> DeleteCasterComponentOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterComponentRequest&, const DeleteCasterComponentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterComponentAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterEpisodeResult> DeleteCasterEpisodeOutcome;
			typedef std::future<DeleteCasterEpisodeOutcome> DeleteCasterEpisodeOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterEpisodeRequest&, const DeleteCasterEpisodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterEpisodeAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterEpisodeGroupResult> DeleteCasterEpisodeGroupOutcome;
			typedef std::future<DeleteCasterEpisodeGroupOutcome> DeleteCasterEpisodeGroupOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterEpisodeGroupRequest&, const DeleteCasterEpisodeGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterEpisodeGroupAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterLayoutResult> DeleteCasterLayoutOutcome;
			typedef std::future<DeleteCasterLayoutOutcome> DeleteCasterLayoutOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterLayoutRequest&, const DeleteCasterLayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterLayoutAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterProgramResult> DeleteCasterProgramOutcome;
			typedef std::future<DeleteCasterProgramOutcome> DeleteCasterProgramOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterProgramRequest&, const DeleteCasterProgramOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterProgramAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterSceneConfigResult> DeleteCasterSceneConfigOutcome;
			typedef std::future<DeleteCasterSceneConfigOutcome> DeleteCasterSceneConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterSceneConfigRequest&, const DeleteCasterSceneConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterSceneConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteCasterVideoResourceResult> DeleteCasterVideoResourceOutcome;
			typedef std::future<DeleteCasterVideoResourceOutcome> DeleteCasterVideoResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteCasterVideoResourceRequest&, const DeleteCasterVideoResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteCasterVideoResourceAsyncHandler;
			typedef Outcome<Error, Model::DeleteHtmlResourceResult> DeleteHtmlResourceOutcome;
			typedef std::future<DeleteHtmlResourceOutcome> DeleteHtmlResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteHtmlResourceRequest&, const DeleteHtmlResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteHtmlResourceAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveASRConfigResult> DeleteLiveASRConfigOutcome;
			typedef std::future<DeleteLiveASRConfigOutcome> DeleteLiveASRConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveASRConfigRequest&, const DeleteLiveASRConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveASRConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveAppRecordConfigResult> DeleteLiveAppRecordConfigOutcome;
			typedef std::future<DeleteLiveAppRecordConfigOutcome> DeleteLiveAppRecordConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveAppRecordConfigRequest&, const DeleteLiveAppRecordConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveAppRecordConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveAppSnapshotConfigResult> DeleteLiveAppSnapshotConfigOutcome;
			typedef std::future<DeleteLiveAppSnapshotConfigOutcome> DeleteLiveAppSnapshotConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveAppSnapshotConfigRequest&, const DeleteLiveAppSnapshotConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveAppSnapshotConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveAudioAuditConfigResult> DeleteLiveAudioAuditConfigOutcome;
			typedef std::future<DeleteLiveAudioAuditConfigOutcome> DeleteLiveAudioAuditConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveAudioAuditConfigRequest&, const DeleteLiveAudioAuditConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveAudioAuditConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveAudioAuditNotifyConfigResult> DeleteLiveAudioAuditNotifyConfigOutcome;
			typedef std::future<DeleteLiveAudioAuditNotifyConfigOutcome> DeleteLiveAudioAuditNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveAudioAuditNotifyConfigRequest&, const DeleteLiveAudioAuditNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveAudioAuditNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveDetectNotifyConfigResult> DeleteLiveDetectNotifyConfigOutcome;
			typedef std::future<DeleteLiveDetectNotifyConfigOutcome> DeleteLiveDetectNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveDetectNotifyConfigRequest&, const DeleteLiveDetectNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveDetectNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveDomainResult> DeleteLiveDomainOutcome;
			typedef std::future<DeleteLiveDomainOutcome> DeleteLiveDomainOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveDomainRequest&, const DeleteLiveDomainOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveDomainAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveDomainMappingResult> DeleteLiveDomainMappingOutcome;
			typedef std::future<DeleteLiveDomainMappingOutcome> DeleteLiveDomainMappingOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveDomainMappingRequest&, const DeleteLiveDomainMappingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveDomainMappingAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveLazyPullStreamInfoConfigResult> DeleteLiveLazyPullStreamInfoConfigOutcome;
			typedef std::future<DeleteLiveLazyPullStreamInfoConfigOutcome> DeleteLiveLazyPullStreamInfoConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveLazyPullStreamInfoConfigRequest&, const DeleteLiveLazyPullStreamInfoConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveLazyPullStreamInfoConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLivePullStreamInfoConfigResult> DeleteLivePullStreamInfoConfigOutcome;
			typedef std::future<DeleteLivePullStreamInfoConfigOutcome> DeleteLivePullStreamInfoConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLivePullStreamInfoConfigRequest&, const DeleteLivePullStreamInfoConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLivePullStreamInfoConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveRealTimeLogLogstoreResult> DeleteLiveRealTimeLogLogstoreOutcome;
			typedef std::future<DeleteLiveRealTimeLogLogstoreOutcome> DeleteLiveRealTimeLogLogstoreOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveRealTimeLogLogstoreRequest&, const DeleteLiveRealTimeLogLogstoreOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveRealTimeLogLogstoreAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveRealtimeLogDeliveryResult> DeleteLiveRealtimeLogDeliveryOutcome;
			typedef std::future<DeleteLiveRealtimeLogDeliveryOutcome> DeleteLiveRealtimeLogDeliveryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveRealtimeLogDeliveryRequest&, const DeleteLiveRealtimeLogDeliveryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveRealtimeLogDeliveryAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveRecordNotifyConfigResult> DeleteLiveRecordNotifyConfigOutcome;
			typedef std::future<DeleteLiveRecordNotifyConfigOutcome> DeleteLiveRecordNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveRecordNotifyConfigRequest&, const DeleteLiveRecordNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveRecordNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveRecordVodConfigResult> DeleteLiveRecordVodConfigOutcome;
			typedef std::future<DeleteLiveRecordVodConfigOutcome> DeleteLiveRecordVodConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveRecordVodConfigRequest&, const DeleteLiveRecordVodConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveRecordVodConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveSnapshotDetectPornConfigResult> DeleteLiveSnapshotDetectPornConfigOutcome;
			typedef std::future<DeleteLiveSnapshotDetectPornConfigOutcome> DeleteLiveSnapshotDetectPornConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveSnapshotDetectPornConfigRequest&, const DeleteLiveSnapshotDetectPornConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveSnapshotDetectPornConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveStreamTranscodeResult> DeleteLiveStreamTranscodeOutcome;
			typedef std::future<DeleteLiveStreamTranscodeOutcome> DeleteLiveStreamTranscodeOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveStreamTranscodeRequest&, const DeleteLiveStreamTranscodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveStreamTranscodeAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveStreamsNotifyUrlConfigResult> DeleteLiveStreamsNotifyUrlConfigOutcome;
			typedef std::future<DeleteLiveStreamsNotifyUrlConfigOutcome> DeleteLiveStreamsNotifyUrlConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteLiveStreamsNotifyUrlConfigRequest&, const DeleteLiveStreamsNotifyUrlConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveStreamsNotifyUrlConfigAsyncHandler;
			typedef Outcome<Error, Model::DeleteMixStreamResult> DeleteMixStreamOutcome;
			typedef std::future<DeleteMixStreamOutcome> DeleteMixStreamOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteMixStreamRequest&, const DeleteMixStreamOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteMixStreamAsyncHandler;
			typedef Outcome<Error, Model::DeleteRoomResult> DeleteRoomOutcome;
			typedef std::future<DeleteRoomOutcome> DeleteRoomOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DeleteRoomRequest&, const DeleteRoomOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteRoomAsyncHandler;
			typedef Outcome<Error, Model::DescribeBoardEventsResult> DescribeBoardEventsOutcome;
			typedef std::future<DescribeBoardEventsOutcome> DescribeBoardEventsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeBoardEventsRequest&, const DescribeBoardEventsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeBoardEventsAsyncHandler;
			typedef Outcome<Error, Model::DescribeBoardSnapshotResult> DescribeBoardSnapshotOutcome;
			typedef std::future<DescribeBoardSnapshotOutcome> DescribeBoardSnapshotOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeBoardSnapshotRequest&, const DescribeBoardSnapshotOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeBoardSnapshotAsyncHandler;
			typedef Outcome<Error, Model::DescribeBoardsResult> DescribeBoardsOutcome;
			typedef std::future<DescribeBoardsOutcome> DescribeBoardsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeBoardsRequest&, const DescribeBoardsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeBoardsAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterChannelsResult> DescribeCasterChannelsOutcome;
			typedef std::future<DescribeCasterChannelsOutcome> DescribeCasterChannelsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterChannelsRequest&, const DescribeCasterChannelsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterChannelsAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterComponentsResult> DescribeCasterComponentsOutcome;
			typedef std::future<DescribeCasterComponentsOutcome> DescribeCasterComponentsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterComponentsRequest&, const DescribeCasterComponentsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterComponentsAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterConfigResult> DescribeCasterConfigOutcome;
			typedef std::future<DescribeCasterConfigOutcome> DescribeCasterConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterConfigRequest&, const DescribeCasterConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterLayoutsResult> DescribeCasterLayoutsOutcome;
			typedef std::future<DescribeCasterLayoutsOutcome> DescribeCasterLayoutsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterLayoutsRequest&, const DescribeCasterLayoutsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterLayoutsAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterProgramResult> DescribeCasterProgramOutcome;
			typedef std::future<DescribeCasterProgramOutcome> DescribeCasterProgramOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterProgramRequest&, const DescribeCasterProgramOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterProgramAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterRtcInfoResult> DescribeCasterRtcInfoOutcome;
			typedef std::future<DescribeCasterRtcInfoOutcome> DescribeCasterRtcInfoOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterRtcInfoRequest&, const DescribeCasterRtcInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterRtcInfoAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterSceneAudioResult> DescribeCasterSceneAudioOutcome;
			typedef std::future<DescribeCasterSceneAudioOutcome> DescribeCasterSceneAudioOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterSceneAudioRequest&, const DescribeCasterSceneAudioOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterSceneAudioAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterScenesResult> DescribeCasterScenesOutcome;
			typedef std::future<DescribeCasterScenesOutcome> DescribeCasterScenesOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterScenesRequest&, const DescribeCasterScenesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterScenesAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterStreamUrlResult> DescribeCasterStreamUrlOutcome;
			typedef std::future<DescribeCasterStreamUrlOutcome> DescribeCasterStreamUrlOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterStreamUrlRequest&, const DescribeCasterStreamUrlOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterStreamUrlAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterSyncGroupResult> DescribeCasterSyncGroupOutcome;
			typedef std::future<DescribeCasterSyncGroupOutcome> DescribeCasterSyncGroupOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterSyncGroupRequest&, const DescribeCasterSyncGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterSyncGroupAsyncHandler;
			typedef Outcome<Error, Model::DescribeCasterVideoResourcesResult> DescribeCasterVideoResourcesOutcome;
			typedef std::future<DescribeCasterVideoResourcesOutcome> DescribeCasterVideoResourcesOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCasterVideoResourcesRequest&, const DescribeCasterVideoResourcesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCasterVideoResourcesAsyncHandler;
			typedef Outcome<Error, Model::DescribeCastersResult> DescribeCastersOutcome;
			typedef std::future<DescribeCastersOutcome> DescribeCastersOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeCastersRequest&, const DescribeCastersOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeCastersAsyncHandler;
			typedef Outcome<Error, Model::DescribeForbidPushStreamRoomListResult> DescribeForbidPushStreamRoomListOutcome;
			typedef std::future<DescribeForbidPushStreamRoomListOutcome> DescribeForbidPushStreamRoomListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeForbidPushStreamRoomListRequest&, const DescribeForbidPushStreamRoomListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeForbidPushStreamRoomListAsyncHandler;
			typedef Outcome<Error, Model::DescribeHlsLiveStreamRealTimeBpsDataResult> DescribeHlsLiveStreamRealTimeBpsDataOutcome;
			typedef std::future<DescribeHlsLiveStreamRealTimeBpsDataOutcome> DescribeHlsLiveStreamRealTimeBpsDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeHlsLiveStreamRealTimeBpsDataRequest&, const DescribeHlsLiveStreamRealTimeBpsDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeHlsLiveStreamRealTimeBpsDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeHtmlResourceResult> DescribeHtmlResourceOutcome;
			typedef std::future<DescribeHtmlResourceOutcome> DescribeHtmlResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeHtmlResourceRequest&, const DescribeHtmlResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeHtmlResourceAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveAsrConfigResult> DescribeLiveAsrConfigOutcome;
			typedef std::future<DescribeLiveAsrConfigOutcome> DescribeLiveAsrConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveAsrConfigRequest&, const DescribeLiveAsrConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveAsrConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveAudioAuditConfigResult> DescribeLiveAudioAuditConfigOutcome;
			typedef std::future<DescribeLiveAudioAuditConfigOutcome> DescribeLiveAudioAuditConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveAudioAuditConfigRequest&, const DescribeLiveAudioAuditConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveAudioAuditConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveAudioAuditNotifyConfigResult> DescribeLiveAudioAuditNotifyConfigOutcome;
			typedef std::future<DescribeLiveAudioAuditNotifyConfigOutcome> DescribeLiveAudioAuditNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveAudioAuditNotifyConfigRequest&, const DescribeLiveAudioAuditNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveAudioAuditNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveCertificateDetailResult> DescribeLiveCertificateDetailOutcome;
			typedef std::future<DescribeLiveCertificateDetailOutcome> DescribeLiveCertificateDetailOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveCertificateDetailRequest&, const DescribeLiveCertificateDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveCertificateDetailAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveCertificateListResult> DescribeLiveCertificateListOutcome;
			typedef std::future<DescribeLiveCertificateListOutcome> DescribeLiveCertificateListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveCertificateListRequest&, const DescribeLiveCertificateListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveCertificateListAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDetectNotifyConfigResult> DescribeLiveDetectNotifyConfigOutcome;
			typedef std::future<DescribeLiveDetectNotifyConfigOutcome> DescribeLiveDetectNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDetectNotifyConfigRequest&, const DescribeLiveDetectNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDetectNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDetectPornDataResult> DescribeLiveDetectPornDataOutcome;
			typedef std::future<DescribeLiveDetectPornDataOutcome> DescribeLiveDetectPornDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDetectPornDataRequest&, const DescribeLiveDetectPornDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDetectPornDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainBpsDataResult> DescribeLiveDomainBpsDataOutcome;
			typedef std::future<DescribeLiveDomainBpsDataOutcome> DescribeLiveDomainBpsDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainBpsDataRequest&, const DescribeLiveDomainBpsDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainBpsDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainBpsDataByTimeStampResult> DescribeLiveDomainBpsDataByTimeStampOutcome;
			typedef std::future<DescribeLiveDomainBpsDataByTimeStampOutcome> DescribeLiveDomainBpsDataByTimeStampOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainBpsDataByTimeStampRequest&, const DescribeLiveDomainBpsDataByTimeStampOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainBpsDataByTimeStampAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainCertificateInfoResult> DescribeLiveDomainCertificateInfoOutcome;
			typedef std::future<DescribeLiveDomainCertificateInfoOutcome> DescribeLiveDomainCertificateInfoOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainCertificateInfoRequest&, const DescribeLiveDomainCertificateInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainCertificateInfoAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainConfigsResult> DescribeLiveDomainConfigsOutcome;
			typedef std::future<DescribeLiveDomainConfigsOutcome> DescribeLiveDomainConfigsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainConfigsRequest&, const DescribeLiveDomainConfigsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainConfigsAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainDetailResult> DescribeLiveDomainDetailOutcome;
			typedef std::future<DescribeLiveDomainDetailOutcome> DescribeLiveDomainDetailOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainDetailRequest&, const DescribeLiveDomainDetailOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainDetailAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainFrameRateAndBitRateDataResult> DescribeLiveDomainFrameRateAndBitRateDataOutcome;
			typedef std::future<DescribeLiveDomainFrameRateAndBitRateDataOutcome> DescribeLiveDomainFrameRateAndBitRateDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainFrameRateAndBitRateDataRequest&, const DescribeLiveDomainFrameRateAndBitRateDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainFrameRateAndBitRateDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainLimitResult> DescribeLiveDomainLimitOutcome;
			typedef std::future<DescribeLiveDomainLimitOutcome> DescribeLiveDomainLimitOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainLimitRequest&, const DescribeLiveDomainLimitOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainLimitAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainMappingResult> DescribeLiveDomainMappingOutcome;
			typedef std::future<DescribeLiveDomainMappingOutcome> DescribeLiveDomainMappingOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainMappingRequest&, const DescribeLiveDomainMappingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainMappingAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainOnlineUserNumResult> DescribeLiveDomainOnlineUserNumOutcome;
			typedef std::future<DescribeLiveDomainOnlineUserNumOutcome> DescribeLiveDomainOnlineUserNumOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainOnlineUserNumRequest&, const DescribeLiveDomainOnlineUserNumOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainOnlineUserNumAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainPushBpsDataResult> DescribeLiveDomainPushBpsDataOutcome;
			typedef std::future<DescribeLiveDomainPushBpsDataOutcome> DescribeLiveDomainPushBpsDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainPushBpsDataRequest&, const DescribeLiveDomainPushBpsDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainPushBpsDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainPushTrafficDataResult> DescribeLiveDomainPushTrafficDataOutcome;
			typedef std::future<DescribeLiveDomainPushTrafficDataOutcome> DescribeLiveDomainPushTrafficDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainPushTrafficDataRequest&, const DescribeLiveDomainPushTrafficDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainPushTrafficDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainRealTimeBpsDataResult> DescribeLiveDomainRealTimeBpsDataOutcome;
			typedef std::future<DescribeLiveDomainRealTimeBpsDataOutcome> DescribeLiveDomainRealTimeBpsDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainRealTimeBpsDataRequest&, const DescribeLiveDomainRealTimeBpsDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainRealTimeBpsDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainRealTimeHttpCodeDataResult> DescribeLiveDomainRealTimeHttpCodeDataOutcome;
			typedef std::future<DescribeLiveDomainRealTimeHttpCodeDataOutcome> DescribeLiveDomainRealTimeHttpCodeDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainRealTimeHttpCodeDataRequest&, const DescribeLiveDomainRealTimeHttpCodeDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainRealTimeHttpCodeDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainRealTimeTrafficDataResult> DescribeLiveDomainRealTimeTrafficDataOutcome;
			typedef std::future<DescribeLiveDomainRealTimeTrafficDataOutcome> DescribeLiveDomainRealTimeTrafficDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainRealTimeTrafficDataRequest&, const DescribeLiveDomainRealTimeTrafficDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainRealTimeTrafficDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainRealtimeLogDeliveryResult> DescribeLiveDomainRealtimeLogDeliveryOutcome;
			typedef std::future<DescribeLiveDomainRealtimeLogDeliveryOutcome> DescribeLiveDomainRealtimeLogDeliveryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainRealtimeLogDeliveryRequest&, const DescribeLiveDomainRealtimeLogDeliveryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainRealtimeLogDeliveryAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainRecordDataResult> DescribeLiveDomainRecordDataOutcome;
			typedef std::future<DescribeLiveDomainRecordDataOutcome> DescribeLiveDomainRecordDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainRecordDataRequest&, const DescribeLiveDomainRecordDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainRecordDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainSnapshotDataResult> DescribeLiveDomainSnapshotDataOutcome;
			typedef std::future<DescribeLiveDomainSnapshotDataOutcome> DescribeLiveDomainSnapshotDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainSnapshotDataRequest&, const DescribeLiveDomainSnapshotDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainSnapshotDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainTimeShiftDataResult> DescribeLiveDomainTimeShiftDataOutcome;
			typedef std::future<DescribeLiveDomainTimeShiftDataOutcome> DescribeLiveDomainTimeShiftDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainTimeShiftDataRequest&, const DescribeLiveDomainTimeShiftDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainTimeShiftDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainTrafficDataResult> DescribeLiveDomainTrafficDataOutcome;
			typedef std::future<DescribeLiveDomainTrafficDataOutcome> DescribeLiveDomainTrafficDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainTrafficDataRequest&, const DescribeLiveDomainTrafficDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainTrafficDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveDomainTranscodeDataResult> DescribeLiveDomainTranscodeDataOutcome;
			typedef std::future<DescribeLiveDomainTranscodeDataOutcome> DescribeLiveDomainTranscodeDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveDomainTranscodeDataRequest&, const DescribeLiveDomainTranscodeDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveDomainTranscodeDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveLazyPullStreamConfigResult> DescribeLiveLazyPullStreamConfigOutcome;
			typedef std::future<DescribeLiveLazyPullStreamConfigOutcome> DescribeLiveLazyPullStreamConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveLazyPullStreamConfigRequest&, const DescribeLiveLazyPullStreamConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveLazyPullStreamConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLivePullStreamConfigResult> DescribeLivePullStreamConfigOutcome;
			typedef std::future<DescribeLivePullStreamConfigOutcome> DescribeLivePullStreamConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLivePullStreamConfigRequest&, const DescribeLivePullStreamConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLivePullStreamConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveRealtimeDeliveryAccResult> DescribeLiveRealtimeDeliveryAccOutcome;
			typedef std::future<DescribeLiveRealtimeDeliveryAccOutcome> DescribeLiveRealtimeDeliveryAccOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveRealtimeDeliveryAccRequest&, const DescribeLiveRealtimeDeliveryAccOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveRealtimeDeliveryAccAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveRealtimeLogAuthorizedResult> DescribeLiveRealtimeLogAuthorizedOutcome;
			typedef std::future<DescribeLiveRealtimeLogAuthorizedOutcome> DescribeLiveRealtimeLogAuthorizedOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveRealtimeLogAuthorizedRequest&, const DescribeLiveRealtimeLogAuthorizedOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveRealtimeLogAuthorizedAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveRecordConfigResult> DescribeLiveRecordConfigOutcome;
			typedef std::future<DescribeLiveRecordConfigOutcome> DescribeLiveRecordConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveRecordConfigRequest&, const DescribeLiveRecordConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveRecordConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveRecordNotifyConfigResult> DescribeLiveRecordNotifyConfigOutcome;
			typedef std::future<DescribeLiveRecordNotifyConfigOutcome> DescribeLiveRecordNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveRecordNotifyConfigRequest&, const DescribeLiveRecordNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveRecordNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveRecordVodConfigsResult> DescribeLiveRecordVodConfigsOutcome;
			typedef std::future<DescribeLiveRecordVodConfigsOutcome> DescribeLiveRecordVodConfigsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveRecordVodConfigsRequest&, const DescribeLiveRecordVodConfigsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveRecordVodConfigsAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveSnapshotConfigResult> DescribeLiveSnapshotConfigOutcome;
			typedef std::future<DescribeLiveSnapshotConfigOutcome> DescribeLiveSnapshotConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveSnapshotConfigRequest&, const DescribeLiveSnapshotConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveSnapshotConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveSnapshotDetectPornConfigResult> DescribeLiveSnapshotDetectPornConfigOutcome;
			typedef std::future<DescribeLiveSnapshotDetectPornConfigOutcome> DescribeLiveSnapshotDetectPornConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveSnapshotDetectPornConfigRequest&, const DescribeLiveSnapshotDetectPornConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveSnapshotDetectPornConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamBitRateDataResult> DescribeLiveStreamBitRateDataOutcome;
			typedef std::future<DescribeLiveStreamBitRateDataOutcome> DescribeLiveStreamBitRateDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamBitRateDataRequest&, const DescribeLiveStreamBitRateDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamBitRateDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamCountResult> DescribeLiveStreamCountOutcome;
			typedef std::future<DescribeLiveStreamCountOutcome> DescribeLiveStreamCountOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamCountRequest&, const DescribeLiveStreamCountOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamCountAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamDelayConfigResult> DescribeLiveStreamDelayConfigOutcome;
			typedef std::future<DescribeLiveStreamDelayConfigOutcome> DescribeLiveStreamDelayConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamDelayConfigRequest&, const DescribeLiveStreamDelayConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamDelayConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamHistoryUserNumResult> DescribeLiveStreamHistoryUserNumOutcome;
			typedef std::future<DescribeLiveStreamHistoryUserNumOutcome> DescribeLiveStreamHistoryUserNumOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamHistoryUserNumRequest&, const DescribeLiveStreamHistoryUserNumOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamHistoryUserNumAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamOnlineUserNumResult> DescribeLiveStreamOnlineUserNumOutcome;
			typedef std::future<DescribeLiveStreamOnlineUserNumOutcome> DescribeLiveStreamOnlineUserNumOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamOnlineUserNumRequest&, const DescribeLiveStreamOnlineUserNumOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamOnlineUserNumAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamOptimizedFeatureConfigResult> DescribeLiveStreamOptimizedFeatureConfigOutcome;
			typedef std::future<DescribeLiveStreamOptimizedFeatureConfigOutcome> DescribeLiveStreamOptimizedFeatureConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamOptimizedFeatureConfigRequest&, const DescribeLiveStreamOptimizedFeatureConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamOptimizedFeatureConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamRecordContentResult> DescribeLiveStreamRecordContentOutcome;
			typedef std::future<DescribeLiveStreamRecordContentOutcome> DescribeLiveStreamRecordContentOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamRecordContentRequest&, const DescribeLiveStreamRecordContentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamRecordContentAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamRecordIndexFileResult> DescribeLiveStreamRecordIndexFileOutcome;
			typedef std::future<DescribeLiveStreamRecordIndexFileOutcome> DescribeLiveStreamRecordIndexFileOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamRecordIndexFileRequest&, const DescribeLiveStreamRecordIndexFileOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamRecordIndexFileAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamRecordIndexFilesResult> DescribeLiveStreamRecordIndexFilesOutcome;
			typedef std::future<DescribeLiveStreamRecordIndexFilesOutcome> DescribeLiveStreamRecordIndexFilesOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamRecordIndexFilesRequest&, const DescribeLiveStreamRecordIndexFilesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamRecordIndexFilesAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamSnapshotInfoResult> DescribeLiveStreamSnapshotInfoOutcome;
			typedef std::future<DescribeLiveStreamSnapshotInfoOutcome> DescribeLiveStreamSnapshotInfoOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamSnapshotInfoRequest&, const DescribeLiveStreamSnapshotInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamSnapshotInfoAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamTranscodeInfoResult> DescribeLiveStreamTranscodeInfoOutcome;
			typedef std::future<DescribeLiveStreamTranscodeInfoOutcome> DescribeLiveStreamTranscodeInfoOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamTranscodeInfoRequest&, const DescribeLiveStreamTranscodeInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamTranscodeInfoAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamTranscodeStreamNumResult> DescribeLiveStreamTranscodeStreamNumOutcome;
			typedef std::future<DescribeLiveStreamTranscodeStreamNumOutcome> DescribeLiveStreamTranscodeStreamNumOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamTranscodeStreamNumRequest&, const DescribeLiveStreamTranscodeStreamNumOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamTranscodeStreamNumAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamsBlockListResult> DescribeLiveStreamsBlockListOutcome;
			typedef std::future<DescribeLiveStreamsBlockListOutcome> DescribeLiveStreamsBlockListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamsBlockListRequest&, const DescribeLiveStreamsBlockListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamsBlockListAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamsControlHistoryResult> DescribeLiveStreamsControlHistoryOutcome;
			typedef std::future<DescribeLiveStreamsControlHistoryOutcome> DescribeLiveStreamsControlHistoryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamsControlHistoryRequest&, const DescribeLiveStreamsControlHistoryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamsControlHistoryAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamsFrameRateAndBitRateDataResult> DescribeLiveStreamsFrameRateAndBitRateDataOutcome;
			typedef std::future<DescribeLiveStreamsFrameRateAndBitRateDataOutcome> DescribeLiveStreamsFrameRateAndBitRateDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamsFrameRateAndBitRateDataRequest&, const DescribeLiveStreamsFrameRateAndBitRateDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamsFrameRateAndBitRateDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamsNotifyUrlConfigResult> DescribeLiveStreamsNotifyUrlConfigOutcome;
			typedef std::future<DescribeLiveStreamsNotifyUrlConfigOutcome> DescribeLiveStreamsNotifyUrlConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamsNotifyUrlConfigRequest&, const DescribeLiveStreamsNotifyUrlConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamsNotifyUrlConfigAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamsOnlineListResult> DescribeLiveStreamsOnlineListOutcome;
			typedef std::future<DescribeLiveStreamsOnlineListOutcome> DescribeLiveStreamsOnlineListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamsOnlineListRequest&, const DescribeLiveStreamsOnlineListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamsOnlineListAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveStreamsPublishListResult> DescribeLiveStreamsPublishListOutcome;
			typedef std::future<DescribeLiveStreamsPublishListOutcome> DescribeLiveStreamsPublishListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveStreamsPublishListRequest&, const DescribeLiveStreamsPublishListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveStreamsPublishListAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveTagResourcesResult> DescribeLiveTagResourcesOutcome;
			typedef std::future<DescribeLiveTagResourcesOutcome> DescribeLiveTagResourcesOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveTagResourcesRequest&, const DescribeLiveTagResourcesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveTagResourcesAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveTopDomainsByFlowResult> DescribeLiveTopDomainsByFlowOutcome;
			typedef std::future<DescribeLiveTopDomainsByFlowOutcome> DescribeLiveTopDomainsByFlowOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveTopDomainsByFlowRequest&, const DescribeLiveTopDomainsByFlowOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveTopDomainsByFlowAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveUserDomainsResult> DescribeLiveUserDomainsOutcome;
			typedef std::future<DescribeLiveUserDomainsOutcome> DescribeLiveUserDomainsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveUserDomainsRequest&, const DescribeLiveUserDomainsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveUserDomainsAsyncHandler;
			typedef Outcome<Error, Model::DescribeLiveUserTagsResult> DescribeLiveUserTagsOutcome;
			typedef std::future<DescribeLiveUserTagsOutcome> DescribeLiveUserTagsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeLiveUserTagsRequest&, const DescribeLiveUserTagsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeLiveUserTagsAsyncHandler;
			typedef Outcome<Error, Model::DescribeMixStreamListResult> DescribeMixStreamListOutcome;
			typedef std::future<DescribeMixStreamListOutcome> DescribeMixStreamListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeMixStreamListRequest&, const DescribeMixStreamListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeMixStreamListAsyncHandler;
			typedef Outcome<Error, Model::DescribeRecordResult> DescribeRecordOutcome;
			typedef std::future<DescribeRecordOutcome> DescribeRecordOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeRecordRequest&, const DescribeRecordOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRecordAsyncHandler;
			typedef Outcome<Error, Model::DescribeRecordsResult> DescribeRecordsOutcome;
			typedef std::future<DescribeRecordsOutcome> DescribeRecordsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeRecordsRequest&, const DescribeRecordsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRecordsAsyncHandler;
			typedef Outcome<Error, Model::DescribeRoomKickoutUserListResult> DescribeRoomKickoutUserListOutcome;
			typedef std::future<DescribeRoomKickoutUserListOutcome> DescribeRoomKickoutUserListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeRoomKickoutUserListRequest&, const DescribeRoomKickoutUserListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRoomKickoutUserListAsyncHandler;
			typedef Outcome<Error, Model::DescribeRoomListResult> DescribeRoomListOutcome;
			typedef std::future<DescribeRoomListOutcome> DescribeRoomListOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeRoomListRequest&, const DescribeRoomListOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRoomListAsyncHandler;
			typedef Outcome<Error, Model::DescribeRoomStatusResult> DescribeRoomStatusOutcome;
			typedef std::future<DescribeRoomStatusOutcome> DescribeRoomStatusOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeRoomStatusRequest&, const DescribeRoomStatusOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeRoomStatusAsyncHandler;
			typedef Outcome<Error, Model::DescribeUpBpsPeakDataResult> DescribeUpBpsPeakDataOutcome;
			typedef std::future<DescribeUpBpsPeakDataOutcome> DescribeUpBpsPeakDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeUpBpsPeakDataRequest&, const DescribeUpBpsPeakDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeUpBpsPeakDataAsyncHandler;
			typedef Outcome<Error, Model::DescribeUpBpsPeakOfLineResult> DescribeUpBpsPeakOfLineOutcome;
			typedef std::future<DescribeUpBpsPeakOfLineOutcome> DescribeUpBpsPeakOfLineOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeUpBpsPeakOfLineRequest&, const DescribeUpBpsPeakOfLineOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeUpBpsPeakOfLineAsyncHandler;
			typedef Outcome<Error, Model::DescribeUpPeakPublishStreamDataResult> DescribeUpPeakPublishStreamDataOutcome;
			typedef std::future<DescribeUpPeakPublishStreamDataOutcome> DescribeUpPeakPublishStreamDataOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DescribeUpPeakPublishStreamDataRequest&, const DescribeUpPeakPublishStreamDataOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DescribeUpPeakPublishStreamDataAsyncHandler;
			typedef Outcome<Error, Model::DisableLiveRealtimeLogDeliveryResult> DisableLiveRealtimeLogDeliveryOutcome;
			typedef std::future<DisableLiveRealtimeLogDeliveryOutcome> DisableLiveRealtimeLogDeliveryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::DisableLiveRealtimeLogDeliveryRequest&, const DisableLiveRealtimeLogDeliveryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DisableLiveRealtimeLogDeliveryAsyncHandler;
			typedef Outcome<Error, Model::EditHtmlResourceResult> EditHtmlResourceOutcome;
			typedef std::future<EditHtmlResourceOutcome> EditHtmlResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::EditHtmlResourceRequest&, const EditHtmlResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EditHtmlResourceAsyncHandler;
			typedef Outcome<Error, Model::EffectCasterUrgentResult> EffectCasterUrgentOutcome;
			typedef std::future<EffectCasterUrgentOutcome> EffectCasterUrgentOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::EffectCasterUrgentRequest&, const EffectCasterUrgentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EffectCasterUrgentAsyncHandler;
			typedef Outcome<Error, Model::EffectCasterVideoResourceResult> EffectCasterVideoResourceOutcome;
			typedef std::future<EffectCasterVideoResourceOutcome> EffectCasterVideoResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::EffectCasterVideoResourceRequest&, const EffectCasterVideoResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EffectCasterVideoResourceAsyncHandler;
			typedef Outcome<Error, Model::EnableLiveRealtimeLogDeliveryResult> EnableLiveRealtimeLogDeliveryOutcome;
			typedef std::future<EnableLiveRealtimeLogDeliveryOutcome> EnableLiveRealtimeLogDeliveryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::EnableLiveRealtimeLogDeliveryRequest&, const EnableLiveRealtimeLogDeliveryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnableLiveRealtimeLogDeliveryAsyncHandler;
			typedef Outcome<Error, Model::ForbidLiveStreamResult> ForbidLiveStreamOutcome;
			typedef std::future<ForbidLiveStreamOutcome> ForbidLiveStreamOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ForbidLiveStreamRequest&, const ForbidLiveStreamOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ForbidLiveStreamAsyncHandler;
			typedef Outcome<Error, Model::ForbidPushStreamResult> ForbidPushStreamOutcome;
			typedef std::future<ForbidPushStreamOutcome> ForbidPushStreamOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ForbidPushStreamRequest&, const ForbidPushStreamOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ForbidPushStreamAsyncHandler;
			typedef Outcome<Error, Model::JoinBoardResult> JoinBoardOutcome;
			typedef std::future<JoinBoardOutcome> JoinBoardOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::JoinBoardRequest&, const JoinBoardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> JoinBoardAsyncHandler;
			typedef Outcome<Error, Model::ListLiveRealtimeLogDeliveryResult> ListLiveRealtimeLogDeliveryOutcome;
			typedef std::future<ListLiveRealtimeLogDeliveryOutcome> ListLiveRealtimeLogDeliveryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ListLiveRealtimeLogDeliveryRequest&, const ListLiveRealtimeLogDeliveryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListLiveRealtimeLogDeliveryAsyncHandler;
			typedef Outcome<Error, Model::ListLiveRealtimeLogDeliveryDomainsResult> ListLiveRealtimeLogDeliveryDomainsOutcome;
			typedef std::future<ListLiveRealtimeLogDeliveryDomainsOutcome> ListLiveRealtimeLogDeliveryDomainsOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ListLiveRealtimeLogDeliveryDomainsRequest&, const ListLiveRealtimeLogDeliveryDomainsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListLiveRealtimeLogDeliveryDomainsAsyncHandler;
			typedef Outcome<Error, Model::ListLiveRealtimeLogDeliveryInfosResult> ListLiveRealtimeLogDeliveryInfosOutcome;
			typedef std::future<ListLiveRealtimeLogDeliveryInfosOutcome> ListLiveRealtimeLogDeliveryInfosOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ListLiveRealtimeLogDeliveryInfosRequest&, const ListLiveRealtimeLogDeliveryInfosOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListLiveRealtimeLogDeliveryInfosAsyncHandler;
			typedef Outcome<Error, Model::ModifyCasterComponentResult> ModifyCasterComponentOutcome;
			typedef std::future<ModifyCasterComponentOutcome> ModifyCasterComponentOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ModifyCasterComponentRequest&, const ModifyCasterComponentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyCasterComponentAsyncHandler;
			typedef Outcome<Error, Model::ModifyCasterEpisodeResult> ModifyCasterEpisodeOutcome;
			typedef std::future<ModifyCasterEpisodeOutcome> ModifyCasterEpisodeOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ModifyCasterEpisodeRequest&, const ModifyCasterEpisodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyCasterEpisodeAsyncHandler;
			typedef Outcome<Error, Model::ModifyCasterLayoutResult> ModifyCasterLayoutOutcome;
			typedef std::future<ModifyCasterLayoutOutcome> ModifyCasterLayoutOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ModifyCasterLayoutRequest&, const ModifyCasterLayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyCasterLayoutAsyncHandler;
			typedef Outcome<Error, Model::ModifyCasterProgramResult> ModifyCasterProgramOutcome;
			typedef std::future<ModifyCasterProgramOutcome> ModifyCasterProgramOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ModifyCasterProgramRequest&, const ModifyCasterProgramOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyCasterProgramAsyncHandler;
			typedef Outcome<Error, Model::ModifyCasterVideoResourceResult> ModifyCasterVideoResourceOutcome;
			typedef std::future<ModifyCasterVideoResourceOutcome> ModifyCasterVideoResourceOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ModifyCasterVideoResourceRequest&, const ModifyCasterVideoResourceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyCasterVideoResourceAsyncHandler;
			typedef Outcome<Error, Model::ModifyLiveDomainSchdmByPropertyResult> ModifyLiveDomainSchdmByPropertyOutcome;
			typedef std::future<ModifyLiveDomainSchdmByPropertyOutcome> ModifyLiveDomainSchdmByPropertyOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ModifyLiveDomainSchdmByPropertyRequest&, const ModifyLiveDomainSchdmByPropertyOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyLiveDomainSchdmByPropertyAsyncHandler;
			typedef Outcome<Error, Model::ModifyLiveRealtimeLogDeliveryResult> ModifyLiveRealtimeLogDeliveryOutcome;
			typedef std::future<ModifyLiveRealtimeLogDeliveryOutcome> ModifyLiveRealtimeLogDeliveryOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ModifyLiveRealtimeLogDeliveryRequest&, const ModifyLiveRealtimeLogDeliveryOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyLiveRealtimeLogDeliveryAsyncHandler;
			typedef Outcome<Error, Model::RealTimeRecordCommandResult> RealTimeRecordCommandOutcome;
			typedef std::future<RealTimeRecordCommandOutcome> RealTimeRecordCommandOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::RealTimeRecordCommandRequest&, const RealTimeRecordCommandOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RealTimeRecordCommandAsyncHandler;
			typedef Outcome<Error, Model::RealTimeSnapshotCommandResult> RealTimeSnapshotCommandOutcome;
			typedef std::future<RealTimeSnapshotCommandOutcome> RealTimeSnapshotCommandOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::RealTimeSnapshotCommandRequest&, const RealTimeSnapshotCommandOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RealTimeSnapshotCommandAsyncHandler;
			typedef Outcome<Error, Model::ResumeLiveStreamResult> ResumeLiveStreamOutcome;
			typedef std::future<ResumeLiveStreamOutcome> ResumeLiveStreamOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::ResumeLiveStreamRequest&, const ResumeLiveStreamOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ResumeLiveStreamAsyncHandler;
			typedef Outcome<Error, Model::SendRoomNotificationResult> SendRoomNotificationOutcome;
			typedef std::future<SendRoomNotificationOutcome> SendRoomNotificationOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SendRoomNotificationRequest&, const SendRoomNotificationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SendRoomNotificationAsyncHandler;
			typedef Outcome<Error, Model::SendRoomUserNotificationResult> SendRoomUserNotificationOutcome;
			typedef std::future<SendRoomUserNotificationOutcome> SendRoomUserNotificationOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SendRoomUserNotificationRequest&, const SendRoomUserNotificationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SendRoomUserNotificationAsyncHandler;
			typedef Outcome<Error, Model::SetBoardCallbackResult> SetBoardCallbackOutcome;
			typedef std::future<SetBoardCallbackOutcome> SetBoardCallbackOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetBoardCallbackRequest&, const SetBoardCallbackOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetBoardCallbackAsyncHandler;
			typedef Outcome<Error, Model::SetCasterChannelResult> SetCasterChannelOutcome;
			typedef std::future<SetCasterChannelOutcome> SetCasterChannelOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetCasterChannelRequest&, const SetCasterChannelOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetCasterChannelAsyncHandler;
			typedef Outcome<Error, Model::SetCasterConfigResult> SetCasterConfigOutcome;
			typedef std::future<SetCasterConfigOutcome> SetCasterConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetCasterConfigRequest&, const SetCasterConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetCasterConfigAsyncHandler;
			typedef Outcome<Error, Model::SetCasterSceneConfigResult> SetCasterSceneConfigOutcome;
			typedef std::future<SetCasterSceneConfigOutcome> SetCasterSceneConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetCasterSceneConfigRequest&, const SetCasterSceneConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetCasterSceneConfigAsyncHandler;
			typedef Outcome<Error, Model::SetCasterSyncGroupResult> SetCasterSyncGroupOutcome;
			typedef std::future<SetCasterSyncGroupOutcome> SetCasterSyncGroupOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetCasterSyncGroupRequest&, const SetCasterSyncGroupOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetCasterSyncGroupAsyncHandler;
			typedef Outcome<Error, Model::SetLiveDomainCertificateResult> SetLiveDomainCertificateOutcome;
			typedef std::future<SetLiveDomainCertificateOutcome> SetLiveDomainCertificateOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetLiveDomainCertificateRequest&, const SetLiveDomainCertificateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetLiveDomainCertificateAsyncHandler;
			typedef Outcome<Error, Model::SetLiveLazyPullStreamInfoConfigResult> SetLiveLazyPullStreamInfoConfigOutcome;
			typedef std::future<SetLiveLazyPullStreamInfoConfigOutcome> SetLiveLazyPullStreamInfoConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetLiveLazyPullStreamInfoConfigRequest&, const SetLiveLazyPullStreamInfoConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetLiveLazyPullStreamInfoConfigAsyncHandler;
			typedef Outcome<Error, Model::SetLiveStreamDelayConfigResult> SetLiveStreamDelayConfigOutcome;
			typedef std::future<SetLiveStreamDelayConfigOutcome> SetLiveStreamDelayConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetLiveStreamDelayConfigRequest&, const SetLiveStreamDelayConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetLiveStreamDelayConfigAsyncHandler;
			typedef Outcome<Error, Model::SetLiveStreamOptimizedFeatureConfigResult> SetLiveStreamOptimizedFeatureConfigOutcome;
			typedef std::future<SetLiveStreamOptimizedFeatureConfigOutcome> SetLiveStreamOptimizedFeatureConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetLiveStreamOptimizedFeatureConfigRequest&, const SetLiveStreamOptimizedFeatureConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetLiveStreamOptimizedFeatureConfigAsyncHandler;
			typedef Outcome<Error, Model::SetLiveStreamsNotifyUrlConfigResult> SetLiveStreamsNotifyUrlConfigOutcome;
			typedef std::future<SetLiveStreamsNotifyUrlConfigOutcome> SetLiveStreamsNotifyUrlConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::SetLiveStreamsNotifyUrlConfigRequest&, const SetLiveStreamsNotifyUrlConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SetLiveStreamsNotifyUrlConfigAsyncHandler;
			typedef Outcome<Error, Model::StartBoardRecordResult> StartBoardRecordOutcome;
			typedef std::future<StartBoardRecordOutcome> StartBoardRecordOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StartBoardRecordRequest&, const StartBoardRecordOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartBoardRecordAsyncHandler;
			typedef Outcome<Error, Model::StartCasterResult> StartCasterOutcome;
			typedef std::future<StartCasterOutcome> StartCasterOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StartCasterRequest&, const StartCasterOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartCasterAsyncHandler;
			typedef Outcome<Error, Model::StartCasterSceneResult> StartCasterSceneOutcome;
			typedef std::future<StartCasterSceneOutcome> StartCasterSceneOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StartCasterSceneRequest&, const StartCasterSceneOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartCasterSceneAsyncHandler;
			typedef Outcome<Error, Model::StartLiveDomainResult> StartLiveDomainOutcome;
			typedef std::future<StartLiveDomainOutcome> StartLiveDomainOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StartLiveDomainRequest&, const StartLiveDomainOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartLiveDomainAsyncHandler;
			typedef Outcome<Error, Model::StartLiveIndexResult> StartLiveIndexOutcome;
			typedef std::future<StartLiveIndexOutcome> StartLiveIndexOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StartLiveIndexRequest&, const StartLiveIndexOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartLiveIndexAsyncHandler;
			typedef Outcome<Error, Model::StopCasterResult> StopCasterOutcome;
			typedef std::future<StopCasterOutcome> StopCasterOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StopCasterRequest&, const StopCasterOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopCasterAsyncHandler;
			typedef Outcome<Error, Model::StopCasterSceneResult> StopCasterSceneOutcome;
			typedef std::future<StopCasterSceneOutcome> StopCasterSceneOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StopCasterSceneRequest&, const StopCasterSceneOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopCasterSceneAsyncHandler;
			typedef Outcome<Error, Model::StopLiveDomainResult> StopLiveDomainOutcome;
			typedef std::future<StopLiveDomainOutcome> StopLiveDomainOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StopLiveDomainRequest&, const StopLiveDomainOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopLiveDomainAsyncHandler;
			typedef Outcome<Error, Model::StopLiveIndexResult> StopLiveIndexOutcome;
			typedef std::future<StopLiveIndexOutcome> StopLiveIndexOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::StopLiveIndexRequest&, const StopLiveIndexOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StopLiveIndexAsyncHandler;
			typedef Outcome<Error, Model::TagLiveResourcesResult> TagLiveResourcesOutcome;
			typedef std::future<TagLiveResourcesOutcome> TagLiveResourcesOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::TagLiveResourcesRequest&, const TagLiveResourcesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TagLiveResourcesAsyncHandler;
			typedef Outcome<Error, Model::UnTagLiveResourcesResult> UnTagLiveResourcesOutcome;
			typedef std::future<UnTagLiveResourcesOutcome> UnTagLiveResourcesOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UnTagLiveResourcesRequest&, const UnTagLiveResourcesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UnTagLiveResourcesAsyncHandler;
			typedef Outcome<Error, Model::UpdateBoardResult> UpdateBoardOutcome;
			typedef std::future<UpdateBoardOutcome> UpdateBoardOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateBoardRequest&, const UpdateBoardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateBoardAsyncHandler;
			typedef Outcome<Error, Model::UpdateBoardCallbackResult> UpdateBoardCallbackOutcome;
			typedef std::future<UpdateBoardCallbackOutcome> UpdateBoardCallbackOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateBoardCallbackRequest&, const UpdateBoardCallbackOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateBoardCallbackAsyncHandler;
			typedef Outcome<Error, Model::UpdateCasterSceneAudioResult> UpdateCasterSceneAudioOutcome;
			typedef std::future<UpdateCasterSceneAudioOutcome> UpdateCasterSceneAudioOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateCasterSceneAudioRequest&, const UpdateCasterSceneAudioOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateCasterSceneAudioAsyncHandler;
			typedef Outcome<Error, Model::UpdateCasterSceneConfigResult> UpdateCasterSceneConfigOutcome;
			typedef std::future<UpdateCasterSceneConfigOutcome> UpdateCasterSceneConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateCasterSceneConfigRequest&, const UpdateCasterSceneConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateCasterSceneConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveASRConfigResult> UpdateLiveASRConfigOutcome;
			typedef std::future<UpdateLiveASRConfigOutcome> UpdateLiveASRConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveASRConfigRequest&, const UpdateLiveASRConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveASRConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveAppSnapshotConfigResult> UpdateLiveAppSnapshotConfigOutcome;
			typedef std::future<UpdateLiveAppSnapshotConfigOutcome> UpdateLiveAppSnapshotConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveAppSnapshotConfigRequest&, const UpdateLiveAppSnapshotConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveAppSnapshotConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveAudioAuditConfigResult> UpdateLiveAudioAuditConfigOutcome;
			typedef std::future<UpdateLiveAudioAuditConfigOutcome> UpdateLiveAudioAuditConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveAudioAuditConfigRequest&, const UpdateLiveAudioAuditConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveAudioAuditConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveAudioAuditNotifyConfigResult> UpdateLiveAudioAuditNotifyConfigOutcome;
			typedef std::future<UpdateLiveAudioAuditNotifyConfigOutcome> UpdateLiveAudioAuditNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveAudioAuditNotifyConfigRequest&, const UpdateLiveAudioAuditNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveAudioAuditNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveDetectNotifyConfigResult> UpdateLiveDetectNotifyConfigOutcome;
			typedef std::future<UpdateLiveDetectNotifyConfigOutcome> UpdateLiveDetectNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveDetectNotifyConfigRequest&, const UpdateLiveDetectNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveDetectNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveRecordNotifyConfigResult> UpdateLiveRecordNotifyConfigOutcome;
			typedef std::future<UpdateLiveRecordNotifyConfigOutcome> UpdateLiveRecordNotifyConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveRecordNotifyConfigRequest&, const UpdateLiveRecordNotifyConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveRecordNotifyConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveSnapshotDetectPornConfigResult> UpdateLiveSnapshotDetectPornConfigOutcome;
			typedef std::future<UpdateLiveSnapshotDetectPornConfigOutcome> UpdateLiveSnapshotDetectPornConfigOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveSnapshotDetectPornConfigRequest&, const UpdateLiveSnapshotDetectPornConfigOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveSnapshotDetectPornConfigAsyncHandler;
			typedef Outcome<Error, Model::UpdateLiveTopLevelDomainResult> UpdateLiveTopLevelDomainOutcome;
			typedef std::future<UpdateLiveTopLevelDomainOutcome> UpdateLiveTopLevelDomainOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateLiveTopLevelDomainRequest&, const UpdateLiveTopLevelDomainOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLiveTopLevelDomainAsyncHandler;
			typedef Outcome<Error, Model::UpdateMixStreamResult> UpdateMixStreamOutcome;
			typedef std::future<UpdateMixStreamOutcome> UpdateMixStreamOutcomeCallable;
			typedef std::function<void(const LiveClient*, const Model::UpdateMixStreamRequest&, const UpdateMixStreamOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateMixStreamAsyncHandler;

			LiveClient(const Credentials &credentials, const ClientConfiguration &configuration);
			LiveClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			LiveClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~LiveClient();
			AddCasterComponentOutcome addCasterComponent(const Model::AddCasterComponentRequest &request)const;
			void addCasterComponentAsync(const Model::AddCasterComponentRequest& request, const AddCasterComponentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCasterComponentOutcomeCallable addCasterComponentCallable(const Model::AddCasterComponentRequest& request) const;
			AddCasterEpisodeOutcome addCasterEpisode(const Model::AddCasterEpisodeRequest &request)const;
			void addCasterEpisodeAsync(const Model::AddCasterEpisodeRequest& request, const AddCasterEpisodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCasterEpisodeOutcomeCallable addCasterEpisodeCallable(const Model::AddCasterEpisodeRequest& request) const;
			AddCasterEpisodeGroupOutcome addCasterEpisodeGroup(const Model::AddCasterEpisodeGroupRequest &request)const;
			void addCasterEpisodeGroupAsync(const Model::AddCasterEpisodeGroupRequest& request, const AddCasterEpisodeGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCasterEpisodeGroupOutcomeCallable addCasterEpisodeGroupCallable(const Model::AddCasterEpisodeGroupRequest& request) const;
			AddCasterEpisodeGroupContentOutcome addCasterEpisodeGroupContent(const Model::AddCasterEpisodeGroupContentRequest &request)const;
			void addCasterEpisodeGroupContentAsync(const Model::AddCasterEpisodeGroupContentRequest& request, const AddCasterEpisodeGroupContentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCasterEpisodeGroupContentOutcomeCallable addCasterEpisodeGroupContentCallable(const Model::AddCasterEpisodeGroupContentRequest& request) const;
			AddCasterLayoutOutcome addCasterLayout(const Model::AddCasterLayoutRequest &request)const;
			void addCasterLayoutAsync(const Model::AddCasterLayoutRequest& request, const AddCasterLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCasterLayoutOutcomeCallable addCasterLayoutCallable(const Model::AddCasterLayoutRequest& request) const;
			AddCasterProgramOutcome addCasterProgram(const Model::AddCasterProgramRequest &request)const;
			void addCasterProgramAsync(const Model::AddCasterProgramRequest& request, const AddCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCasterProgramOutcomeCallable addCasterProgramCallable(const Model::AddCasterProgramRequest& request) const;
			AddCasterVideoResourceOutcome addCasterVideoResource(const Model::AddCasterVideoResourceRequest &request)const;
			void addCasterVideoResourceAsync(const Model::AddCasterVideoResourceRequest& request, const AddCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCasterVideoResourceOutcomeCallable addCasterVideoResourceCallable(const Model::AddCasterVideoResourceRequest& request) const;
			AddCustomLiveStreamTranscodeOutcome addCustomLiveStreamTranscode(const Model::AddCustomLiveStreamTranscodeRequest &request)const;
			void addCustomLiveStreamTranscodeAsync(const Model::AddCustomLiveStreamTranscodeRequest& request, const AddCustomLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddCustomLiveStreamTranscodeOutcomeCallable addCustomLiveStreamTranscodeCallable(const Model::AddCustomLiveStreamTranscodeRequest& request) const;
			AddLiveASRConfigOutcome addLiveASRConfig(const Model::AddLiveASRConfigRequest &request)const;
			void addLiveASRConfigAsync(const Model::AddLiveASRConfigRequest& request, const AddLiveASRConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveASRConfigOutcomeCallable addLiveASRConfigCallable(const Model::AddLiveASRConfigRequest& request) const;
			AddLiveAppRecordConfigOutcome addLiveAppRecordConfig(const Model::AddLiveAppRecordConfigRequest &request)const;
			void addLiveAppRecordConfigAsync(const Model::AddLiveAppRecordConfigRequest& request, const AddLiveAppRecordConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveAppRecordConfigOutcomeCallable addLiveAppRecordConfigCallable(const Model::AddLiveAppRecordConfigRequest& request) const;
			AddLiveAppSnapshotConfigOutcome addLiveAppSnapshotConfig(const Model::AddLiveAppSnapshotConfigRequest &request)const;
			void addLiveAppSnapshotConfigAsync(const Model::AddLiveAppSnapshotConfigRequest& request, const AddLiveAppSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveAppSnapshotConfigOutcomeCallable addLiveAppSnapshotConfigCallable(const Model::AddLiveAppSnapshotConfigRequest& request) const;
			AddLiveAudioAuditConfigOutcome addLiveAudioAuditConfig(const Model::AddLiveAudioAuditConfigRequest &request)const;
			void addLiveAudioAuditConfigAsync(const Model::AddLiveAudioAuditConfigRequest& request, const AddLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveAudioAuditConfigOutcomeCallable addLiveAudioAuditConfigCallable(const Model::AddLiveAudioAuditConfigRequest& request) const;
			AddLiveAudioAuditNotifyConfigOutcome addLiveAudioAuditNotifyConfig(const Model::AddLiveAudioAuditNotifyConfigRequest &request)const;
			void addLiveAudioAuditNotifyConfigAsync(const Model::AddLiveAudioAuditNotifyConfigRequest& request, const AddLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveAudioAuditNotifyConfigOutcomeCallable addLiveAudioAuditNotifyConfigCallable(const Model::AddLiveAudioAuditNotifyConfigRequest& request) const;
			AddLiveDetectNotifyConfigOutcome addLiveDetectNotifyConfig(const Model::AddLiveDetectNotifyConfigRequest &request)const;
			void addLiveDetectNotifyConfigAsync(const Model::AddLiveDetectNotifyConfigRequest& request, const AddLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveDetectNotifyConfigOutcomeCallable addLiveDetectNotifyConfigCallable(const Model::AddLiveDetectNotifyConfigRequest& request) const;
			AddLiveDomainOutcome addLiveDomain(const Model::AddLiveDomainRequest &request)const;
			void addLiveDomainAsync(const Model::AddLiveDomainRequest& request, const AddLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveDomainOutcomeCallable addLiveDomainCallable(const Model::AddLiveDomainRequest& request) const;
			AddLiveDomainMappingOutcome addLiveDomainMapping(const Model::AddLiveDomainMappingRequest &request)const;
			void addLiveDomainMappingAsync(const Model::AddLiveDomainMappingRequest& request, const AddLiveDomainMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveDomainMappingOutcomeCallable addLiveDomainMappingCallable(const Model::AddLiveDomainMappingRequest& request) const;
			AddLiveDomainPlayMappingOutcome addLiveDomainPlayMapping(const Model::AddLiveDomainPlayMappingRequest &request)const;
			void addLiveDomainPlayMappingAsync(const Model::AddLiveDomainPlayMappingRequest& request, const AddLiveDomainPlayMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveDomainPlayMappingOutcomeCallable addLiveDomainPlayMappingCallable(const Model::AddLiveDomainPlayMappingRequest& request) const;
			AddLivePullStreamInfoConfigOutcome addLivePullStreamInfoConfig(const Model::AddLivePullStreamInfoConfigRequest &request)const;
			void addLivePullStreamInfoConfigAsync(const Model::AddLivePullStreamInfoConfigRequest& request, const AddLivePullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLivePullStreamInfoConfigOutcomeCallable addLivePullStreamInfoConfigCallable(const Model::AddLivePullStreamInfoConfigRequest& request) const;
			AddLiveRecordNotifyConfigOutcome addLiveRecordNotifyConfig(const Model::AddLiveRecordNotifyConfigRequest &request)const;
			void addLiveRecordNotifyConfigAsync(const Model::AddLiveRecordNotifyConfigRequest& request, const AddLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveRecordNotifyConfigOutcomeCallable addLiveRecordNotifyConfigCallable(const Model::AddLiveRecordNotifyConfigRequest& request) const;
			AddLiveRecordVodConfigOutcome addLiveRecordVodConfig(const Model::AddLiveRecordVodConfigRequest &request)const;
			void addLiveRecordVodConfigAsync(const Model::AddLiveRecordVodConfigRequest& request, const AddLiveRecordVodConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveRecordVodConfigOutcomeCallable addLiveRecordVodConfigCallable(const Model::AddLiveRecordVodConfigRequest& request) const;
			AddLiveSnapshotDetectPornConfigOutcome addLiveSnapshotDetectPornConfig(const Model::AddLiveSnapshotDetectPornConfigRequest &request)const;
			void addLiveSnapshotDetectPornConfigAsync(const Model::AddLiveSnapshotDetectPornConfigRequest& request, const AddLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveSnapshotDetectPornConfigOutcomeCallable addLiveSnapshotDetectPornConfigCallable(const Model::AddLiveSnapshotDetectPornConfigRequest& request) const;
			AddLiveStreamTranscodeOutcome addLiveStreamTranscode(const Model::AddLiveStreamTranscodeRequest &request)const;
			void addLiveStreamTranscodeAsync(const Model::AddLiveStreamTranscodeRequest& request, const AddLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddLiveStreamTranscodeOutcomeCallable addLiveStreamTranscodeCallable(const Model::AddLiveStreamTranscodeRequest& request) const;
			AddRtsLiveStreamTranscodeOutcome addRtsLiveStreamTranscode(const Model::AddRtsLiveStreamTranscodeRequest &request)const;
			void addRtsLiveStreamTranscodeAsync(const Model::AddRtsLiveStreamTranscodeRequest& request, const AddRtsLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddRtsLiveStreamTranscodeOutcomeCallable addRtsLiveStreamTranscodeCallable(const Model::AddRtsLiveStreamTranscodeRequest& request) const;
			AddTrancodeSEIOutcome addTrancodeSEI(const Model::AddTrancodeSEIRequest &request)const;
			void addTrancodeSEIAsync(const Model::AddTrancodeSEIRequest& request, const AddTrancodeSEIAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AddTrancodeSEIOutcomeCallable addTrancodeSEICallable(const Model::AddTrancodeSEIRequest& request) const;
			AllowPushStreamOutcome allowPushStream(const Model::AllowPushStreamRequest &request)const;
			void allowPushStreamAsync(const Model::AllowPushStreamRequest& request, const AllowPushStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			AllowPushStreamOutcomeCallable allowPushStreamCallable(const Model::AllowPushStreamRequest& request) const;
			ApplyBoardTokenOutcome applyBoardToken(const Model::ApplyBoardTokenRequest &request)const;
			void applyBoardTokenAsync(const Model::ApplyBoardTokenRequest& request, const ApplyBoardTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ApplyBoardTokenOutcomeCallable applyBoardTokenCallable(const Model::ApplyBoardTokenRequest& request) const;
			ApplyRecordTokenOutcome applyRecordToken(const Model::ApplyRecordTokenRequest &request)const;
			void applyRecordTokenAsync(const Model::ApplyRecordTokenRequest& request, const ApplyRecordTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ApplyRecordTokenOutcomeCallable applyRecordTokenCallable(const Model::ApplyRecordTokenRequest& request) const;
			BatchDeleteLiveDomainConfigsOutcome batchDeleteLiveDomainConfigs(const Model::BatchDeleteLiveDomainConfigsRequest &request)const;
			void batchDeleteLiveDomainConfigsAsync(const Model::BatchDeleteLiveDomainConfigsRequest& request, const BatchDeleteLiveDomainConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchDeleteLiveDomainConfigsOutcomeCallable batchDeleteLiveDomainConfigsCallable(const Model::BatchDeleteLiveDomainConfigsRequest& request) const;
			BatchSetLiveDomainConfigsOutcome batchSetLiveDomainConfigs(const Model::BatchSetLiveDomainConfigsRequest &request)const;
			void batchSetLiveDomainConfigsAsync(const Model::BatchSetLiveDomainConfigsRequest& request, const BatchSetLiveDomainConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchSetLiveDomainConfigsOutcomeCallable batchSetLiveDomainConfigsCallable(const Model::BatchSetLiveDomainConfigsRequest& request) const;
			CompleteBoardOutcome completeBoard(const Model::CompleteBoardRequest &request)const;
			void completeBoardAsync(const Model::CompleteBoardRequest& request, const CompleteBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CompleteBoardOutcomeCallable completeBoardCallable(const Model::CompleteBoardRequest& request) const;
			CompleteBoardRecordOutcome completeBoardRecord(const Model::CompleteBoardRecordRequest &request)const;
			void completeBoardRecordAsync(const Model::CompleteBoardRecordRequest& request, const CompleteBoardRecordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CompleteBoardRecordOutcomeCallable completeBoardRecordCallable(const Model::CompleteBoardRecordRequest& request) const;
			ControlHtmlResourceOutcome controlHtmlResource(const Model::ControlHtmlResourceRequest &request)const;
			void controlHtmlResourceAsync(const Model::ControlHtmlResourceRequest& request, const ControlHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ControlHtmlResourceOutcomeCallable controlHtmlResourceCallable(const Model::ControlHtmlResourceRequest& request) const;
			CopyCasterOutcome copyCaster(const Model::CopyCasterRequest &request)const;
			void copyCasterAsync(const Model::CopyCasterRequest& request, const CopyCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CopyCasterOutcomeCallable copyCasterCallable(const Model::CopyCasterRequest& request) const;
			CopyCasterSceneConfigOutcome copyCasterSceneConfig(const Model::CopyCasterSceneConfigRequest &request)const;
			void copyCasterSceneConfigAsync(const Model::CopyCasterSceneConfigRequest& request, const CopyCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CopyCasterSceneConfigOutcomeCallable copyCasterSceneConfigCallable(const Model::CopyCasterSceneConfigRequest& request) const;
			CreateBoardOutcome createBoard(const Model::CreateBoardRequest &request)const;
			void createBoardAsync(const Model::CreateBoardRequest& request, const CreateBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateBoardOutcomeCallable createBoardCallable(const Model::CreateBoardRequest& request) const;
			CreateCasterOutcome createCaster(const Model::CreateCasterRequest &request)const;
			void createCasterAsync(const Model::CreateCasterRequest& request, const CreateCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateCasterOutcomeCallable createCasterCallable(const Model::CreateCasterRequest& request) const;
			CreateLiveRealTimeLogDeliveryOutcome createLiveRealTimeLogDelivery(const Model::CreateLiveRealTimeLogDeliveryRequest &request)const;
			void createLiveRealTimeLogDeliveryAsync(const Model::CreateLiveRealTimeLogDeliveryRequest& request, const CreateLiveRealTimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateLiveRealTimeLogDeliveryOutcomeCallable createLiveRealTimeLogDeliveryCallable(const Model::CreateLiveRealTimeLogDeliveryRequest& request) const;
			CreateLiveStreamRecordIndexFilesOutcome createLiveStreamRecordIndexFiles(const Model::CreateLiveStreamRecordIndexFilesRequest &request)const;
			void createLiveStreamRecordIndexFilesAsync(const Model::CreateLiveStreamRecordIndexFilesRequest& request, const CreateLiveStreamRecordIndexFilesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateLiveStreamRecordIndexFilesOutcomeCallable createLiveStreamRecordIndexFilesCallable(const Model::CreateLiveStreamRecordIndexFilesRequest& request) const;
			CreateMixStreamOutcome createMixStream(const Model::CreateMixStreamRequest &request)const;
			void createMixStreamAsync(const Model::CreateMixStreamRequest& request, const CreateMixStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateMixStreamOutcomeCallable createMixStreamCallable(const Model::CreateMixStreamRequest& request) const;
			CreateRoomOutcome createRoom(const Model::CreateRoomRequest &request)const;
			void createRoomAsync(const Model::CreateRoomRequest& request, const CreateRoomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateRoomOutcomeCallable createRoomCallable(const Model::CreateRoomRequest& request) const;
			DeleteBoardOutcome deleteBoard(const Model::DeleteBoardRequest &request)const;
			void deleteBoardAsync(const Model::DeleteBoardRequest& request, const DeleteBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteBoardOutcomeCallable deleteBoardCallable(const Model::DeleteBoardRequest& request) const;
			DeleteCasterOutcome deleteCaster(const Model::DeleteCasterRequest &request)const;
			void deleteCasterAsync(const Model::DeleteCasterRequest& request, const DeleteCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterOutcomeCallable deleteCasterCallable(const Model::DeleteCasterRequest& request) const;
			DeleteCasterComponentOutcome deleteCasterComponent(const Model::DeleteCasterComponentRequest &request)const;
			void deleteCasterComponentAsync(const Model::DeleteCasterComponentRequest& request, const DeleteCasterComponentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterComponentOutcomeCallable deleteCasterComponentCallable(const Model::DeleteCasterComponentRequest& request) const;
			DeleteCasterEpisodeOutcome deleteCasterEpisode(const Model::DeleteCasterEpisodeRequest &request)const;
			void deleteCasterEpisodeAsync(const Model::DeleteCasterEpisodeRequest& request, const DeleteCasterEpisodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterEpisodeOutcomeCallable deleteCasterEpisodeCallable(const Model::DeleteCasterEpisodeRequest& request) const;
			DeleteCasterEpisodeGroupOutcome deleteCasterEpisodeGroup(const Model::DeleteCasterEpisodeGroupRequest &request)const;
			void deleteCasterEpisodeGroupAsync(const Model::DeleteCasterEpisodeGroupRequest& request, const DeleteCasterEpisodeGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterEpisodeGroupOutcomeCallable deleteCasterEpisodeGroupCallable(const Model::DeleteCasterEpisodeGroupRequest& request) const;
			DeleteCasterLayoutOutcome deleteCasterLayout(const Model::DeleteCasterLayoutRequest &request)const;
			void deleteCasterLayoutAsync(const Model::DeleteCasterLayoutRequest& request, const DeleteCasterLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterLayoutOutcomeCallable deleteCasterLayoutCallable(const Model::DeleteCasterLayoutRequest& request) const;
			DeleteCasterProgramOutcome deleteCasterProgram(const Model::DeleteCasterProgramRequest &request)const;
			void deleteCasterProgramAsync(const Model::DeleteCasterProgramRequest& request, const DeleteCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterProgramOutcomeCallable deleteCasterProgramCallable(const Model::DeleteCasterProgramRequest& request) const;
			DeleteCasterSceneConfigOutcome deleteCasterSceneConfig(const Model::DeleteCasterSceneConfigRequest &request)const;
			void deleteCasterSceneConfigAsync(const Model::DeleteCasterSceneConfigRequest& request, const DeleteCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterSceneConfigOutcomeCallable deleteCasterSceneConfigCallable(const Model::DeleteCasterSceneConfigRequest& request) const;
			DeleteCasterVideoResourceOutcome deleteCasterVideoResource(const Model::DeleteCasterVideoResourceRequest &request)const;
			void deleteCasterVideoResourceAsync(const Model::DeleteCasterVideoResourceRequest& request, const DeleteCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteCasterVideoResourceOutcomeCallable deleteCasterVideoResourceCallable(const Model::DeleteCasterVideoResourceRequest& request) const;
			DeleteHtmlResourceOutcome deleteHtmlResource(const Model::DeleteHtmlResourceRequest &request)const;
			void deleteHtmlResourceAsync(const Model::DeleteHtmlResourceRequest& request, const DeleteHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteHtmlResourceOutcomeCallable deleteHtmlResourceCallable(const Model::DeleteHtmlResourceRequest& request) const;
			DeleteLiveASRConfigOutcome deleteLiveASRConfig(const Model::DeleteLiveASRConfigRequest &request)const;
			void deleteLiveASRConfigAsync(const Model::DeleteLiveASRConfigRequest& request, const DeleteLiveASRConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveASRConfigOutcomeCallable deleteLiveASRConfigCallable(const Model::DeleteLiveASRConfigRequest& request) const;
			DeleteLiveAppRecordConfigOutcome deleteLiveAppRecordConfig(const Model::DeleteLiveAppRecordConfigRequest &request)const;
			void deleteLiveAppRecordConfigAsync(const Model::DeleteLiveAppRecordConfigRequest& request, const DeleteLiveAppRecordConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveAppRecordConfigOutcomeCallable deleteLiveAppRecordConfigCallable(const Model::DeleteLiveAppRecordConfigRequest& request) const;
			DeleteLiveAppSnapshotConfigOutcome deleteLiveAppSnapshotConfig(const Model::DeleteLiveAppSnapshotConfigRequest &request)const;
			void deleteLiveAppSnapshotConfigAsync(const Model::DeleteLiveAppSnapshotConfigRequest& request, const DeleteLiveAppSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveAppSnapshotConfigOutcomeCallable deleteLiveAppSnapshotConfigCallable(const Model::DeleteLiveAppSnapshotConfigRequest& request) const;
			DeleteLiveAudioAuditConfigOutcome deleteLiveAudioAuditConfig(const Model::DeleteLiveAudioAuditConfigRequest &request)const;
			void deleteLiveAudioAuditConfigAsync(const Model::DeleteLiveAudioAuditConfigRequest& request, const DeleteLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveAudioAuditConfigOutcomeCallable deleteLiveAudioAuditConfigCallable(const Model::DeleteLiveAudioAuditConfigRequest& request) const;
			DeleteLiveAudioAuditNotifyConfigOutcome deleteLiveAudioAuditNotifyConfig(const Model::DeleteLiveAudioAuditNotifyConfigRequest &request)const;
			void deleteLiveAudioAuditNotifyConfigAsync(const Model::DeleteLiveAudioAuditNotifyConfigRequest& request, const DeleteLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveAudioAuditNotifyConfigOutcomeCallable deleteLiveAudioAuditNotifyConfigCallable(const Model::DeleteLiveAudioAuditNotifyConfigRequest& request) const;
			DeleteLiveDetectNotifyConfigOutcome deleteLiveDetectNotifyConfig(const Model::DeleteLiveDetectNotifyConfigRequest &request)const;
			void deleteLiveDetectNotifyConfigAsync(const Model::DeleteLiveDetectNotifyConfigRequest& request, const DeleteLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveDetectNotifyConfigOutcomeCallable deleteLiveDetectNotifyConfigCallable(const Model::DeleteLiveDetectNotifyConfigRequest& request) const;
			DeleteLiveDomainOutcome deleteLiveDomain(const Model::DeleteLiveDomainRequest &request)const;
			void deleteLiveDomainAsync(const Model::DeleteLiveDomainRequest& request, const DeleteLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveDomainOutcomeCallable deleteLiveDomainCallable(const Model::DeleteLiveDomainRequest& request) const;
			DeleteLiveDomainMappingOutcome deleteLiveDomainMapping(const Model::DeleteLiveDomainMappingRequest &request)const;
			void deleteLiveDomainMappingAsync(const Model::DeleteLiveDomainMappingRequest& request, const DeleteLiveDomainMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveDomainMappingOutcomeCallable deleteLiveDomainMappingCallable(const Model::DeleteLiveDomainMappingRequest& request) const;
			DeleteLiveLazyPullStreamInfoConfigOutcome deleteLiveLazyPullStreamInfoConfig(const Model::DeleteLiveLazyPullStreamInfoConfigRequest &request)const;
			void deleteLiveLazyPullStreamInfoConfigAsync(const Model::DeleteLiveLazyPullStreamInfoConfigRequest& request, const DeleteLiveLazyPullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveLazyPullStreamInfoConfigOutcomeCallable deleteLiveLazyPullStreamInfoConfigCallable(const Model::DeleteLiveLazyPullStreamInfoConfigRequest& request) const;
			DeleteLivePullStreamInfoConfigOutcome deleteLivePullStreamInfoConfig(const Model::DeleteLivePullStreamInfoConfigRequest &request)const;
			void deleteLivePullStreamInfoConfigAsync(const Model::DeleteLivePullStreamInfoConfigRequest& request, const DeleteLivePullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLivePullStreamInfoConfigOutcomeCallable deleteLivePullStreamInfoConfigCallable(const Model::DeleteLivePullStreamInfoConfigRequest& request) const;
			DeleteLiveRealTimeLogLogstoreOutcome deleteLiveRealTimeLogLogstore(const Model::DeleteLiveRealTimeLogLogstoreRequest &request)const;
			void deleteLiveRealTimeLogLogstoreAsync(const Model::DeleteLiveRealTimeLogLogstoreRequest& request, const DeleteLiveRealTimeLogLogstoreAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveRealTimeLogLogstoreOutcomeCallable deleteLiveRealTimeLogLogstoreCallable(const Model::DeleteLiveRealTimeLogLogstoreRequest& request) const;
			DeleteLiveRealtimeLogDeliveryOutcome deleteLiveRealtimeLogDelivery(const Model::DeleteLiveRealtimeLogDeliveryRequest &request)const;
			void deleteLiveRealtimeLogDeliveryAsync(const Model::DeleteLiveRealtimeLogDeliveryRequest& request, const DeleteLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveRealtimeLogDeliveryOutcomeCallable deleteLiveRealtimeLogDeliveryCallable(const Model::DeleteLiveRealtimeLogDeliveryRequest& request) const;
			DeleteLiveRecordNotifyConfigOutcome deleteLiveRecordNotifyConfig(const Model::DeleteLiveRecordNotifyConfigRequest &request)const;
			void deleteLiveRecordNotifyConfigAsync(const Model::DeleteLiveRecordNotifyConfigRequest& request, const DeleteLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveRecordNotifyConfigOutcomeCallable deleteLiveRecordNotifyConfigCallable(const Model::DeleteLiveRecordNotifyConfigRequest& request) const;
			DeleteLiveRecordVodConfigOutcome deleteLiveRecordVodConfig(const Model::DeleteLiveRecordVodConfigRequest &request)const;
			void deleteLiveRecordVodConfigAsync(const Model::DeleteLiveRecordVodConfigRequest& request, const DeleteLiveRecordVodConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveRecordVodConfigOutcomeCallable deleteLiveRecordVodConfigCallable(const Model::DeleteLiveRecordVodConfigRequest& request) const;
			DeleteLiveSnapshotDetectPornConfigOutcome deleteLiveSnapshotDetectPornConfig(const Model::DeleteLiveSnapshotDetectPornConfigRequest &request)const;
			void deleteLiveSnapshotDetectPornConfigAsync(const Model::DeleteLiveSnapshotDetectPornConfigRequest& request, const DeleteLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveSnapshotDetectPornConfigOutcomeCallable deleteLiveSnapshotDetectPornConfigCallable(const Model::DeleteLiveSnapshotDetectPornConfigRequest& request) const;
			DeleteLiveStreamTranscodeOutcome deleteLiveStreamTranscode(const Model::DeleteLiveStreamTranscodeRequest &request)const;
			void deleteLiveStreamTranscodeAsync(const Model::DeleteLiveStreamTranscodeRequest& request, const DeleteLiveStreamTranscodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveStreamTranscodeOutcomeCallable deleteLiveStreamTranscodeCallable(const Model::DeleteLiveStreamTranscodeRequest& request) const;
			DeleteLiveStreamsNotifyUrlConfigOutcome deleteLiveStreamsNotifyUrlConfig(const Model::DeleteLiveStreamsNotifyUrlConfigRequest &request)const;
			void deleteLiveStreamsNotifyUrlConfigAsync(const Model::DeleteLiveStreamsNotifyUrlConfigRequest& request, const DeleteLiveStreamsNotifyUrlConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveStreamsNotifyUrlConfigOutcomeCallable deleteLiveStreamsNotifyUrlConfigCallable(const Model::DeleteLiveStreamsNotifyUrlConfigRequest& request) const;
			DeleteMixStreamOutcome deleteMixStream(const Model::DeleteMixStreamRequest &request)const;
			void deleteMixStreamAsync(const Model::DeleteMixStreamRequest& request, const DeleteMixStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteMixStreamOutcomeCallable deleteMixStreamCallable(const Model::DeleteMixStreamRequest& request) const;
			DeleteRoomOutcome deleteRoom(const Model::DeleteRoomRequest &request)const;
			void deleteRoomAsync(const Model::DeleteRoomRequest& request, const DeleteRoomAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteRoomOutcomeCallable deleteRoomCallable(const Model::DeleteRoomRequest& request) const;
			DescribeBoardEventsOutcome describeBoardEvents(const Model::DescribeBoardEventsRequest &request)const;
			void describeBoardEventsAsync(const Model::DescribeBoardEventsRequest& request, const DescribeBoardEventsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeBoardEventsOutcomeCallable describeBoardEventsCallable(const Model::DescribeBoardEventsRequest& request) const;
			DescribeBoardSnapshotOutcome describeBoardSnapshot(const Model::DescribeBoardSnapshotRequest &request)const;
			void describeBoardSnapshotAsync(const Model::DescribeBoardSnapshotRequest& request, const DescribeBoardSnapshotAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeBoardSnapshotOutcomeCallable describeBoardSnapshotCallable(const Model::DescribeBoardSnapshotRequest& request) const;
			DescribeBoardsOutcome describeBoards(const Model::DescribeBoardsRequest &request)const;
			void describeBoardsAsync(const Model::DescribeBoardsRequest& request, const DescribeBoardsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeBoardsOutcomeCallable describeBoardsCallable(const Model::DescribeBoardsRequest& request) const;
			DescribeCasterChannelsOutcome describeCasterChannels(const Model::DescribeCasterChannelsRequest &request)const;
			void describeCasterChannelsAsync(const Model::DescribeCasterChannelsRequest& request, const DescribeCasterChannelsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterChannelsOutcomeCallable describeCasterChannelsCallable(const Model::DescribeCasterChannelsRequest& request) const;
			DescribeCasterComponentsOutcome describeCasterComponents(const Model::DescribeCasterComponentsRequest &request)const;
			void describeCasterComponentsAsync(const Model::DescribeCasterComponentsRequest& request, const DescribeCasterComponentsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterComponentsOutcomeCallable describeCasterComponentsCallable(const Model::DescribeCasterComponentsRequest& request) const;
			DescribeCasterConfigOutcome describeCasterConfig(const Model::DescribeCasterConfigRequest &request)const;
			void describeCasterConfigAsync(const Model::DescribeCasterConfigRequest& request, const DescribeCasterConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterConfigOutcomeCallable describeCasterConfigCallable(const Model::DescribeCasterConfigRequest& request) const;
			DescribeCasterLayoutsOutcome describeCasterLayouts(const Model::DescribeCasterLayoutsRequest &request)const;
			void describeCasterLayoutsAsync(const Model::DescribeCasterLayoutsRequest& request, const DescribeCasterLayoutsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterLayoutsOutcomeCallable describeCasterLayoutsCallable(const Model::DescribeCasterLayoutsRequest& request) const;
			DescribeCasterProgramOutcome describeCasterProgram(const Model::DescribeCasterProgramRequest &request)const;
			void describeCasterProgramAsync(const Model::DescribeCasterProgramRequest& request, const DescribeCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterProgramOutcomeCallable describeCasterProgramCallable(const Model::DescribeCasterProgramRequest& request) const;
			DescribeCasterRtcInfoOutcome describeCasterRtcInfo(const Model::DescribeCasterRtcInfoRequest &request)const;
			void describeCasterRtcInfoAsync(const Model::DescribeCasterRtcInfoRequest& request, const DescribeCasterRtcInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterRtcInfoOutcomeCallable describeCasterRtcInfoCallable(const Model::DescribeCasterRtcInfoRequest& request) const;
			DescribeCasterSceneAudioOutcome describeCasterSceneAudio(const Model::DescribeCasterSceneAudioRequest &request)const;
			void describeCasterSceneAudioAsync(const Model::DescribeCasterSceneAudioRequest& request, const DescribeCasterSceneAudioAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterSceneAudioOutcomeCallable describeCasterSceneAudioCallable(const Model::DescribeCasterSceneAudioRequest& request) const;
			DescribeCasterScenesOutcome describeCasterScenes(const Model::DescribeCasterScenesRequest &request)const;
			void describeCasterScenesAsync(const Model::DescribeCasterScenesRequest& request, const DescribeCasterScenesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterScenesOutcomeCallable describeCasterScenesCallable(const Model::DescribeCasterScenesRequest& request) const;
			DescribeCasterStreamUrlOutcome describeCasterStreamUrl(const Model::DescribeCasterStreamUrlRequest &request)const;
			void describeCasterStreamUrlAsync(const Model::DescribeCasterStreamUrlRequest& request, const DescribeCasterStreamUrlAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterStreamUrlOutcomeCallable describeCasterStreamUrlCallable(const Model::DescribeCasterStreamUrlRequest& request) const;
			DescribeCasterSyncGroupOutcome describeCasterSyncGroup(const Model::DescribeCasterSyncGroupRequest &request)const;
			void describeCasterSyncGroupAsync(const Model::DescribeCasterSyncGroupRequest& request, const DescribeCasterSyncGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterSyncGroupOutcomeCallable describeCasterSyncGroupCallable(const Model::DescribeCasterSyncGroupRequest& request) const;
			DescribeCasterVideoResourcesOutcome describeCasterVideoResources(const Model::DescribeCasterVideoResourcesRequest &request)const;
			void describeCasterVideoResourcesAsync(const Model::DescribeCasterVideoResourcesRequest& request, const DescribeCasterVideoResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCasterVideoResourcesOutcomeCallable describeCasterVideoResourcesCallable(const Model::DescribeCasterVideoResourcesRequest& request) const;
			DescribeCastersOutcome describeCasters(const Model::DescribeCastersRequest &request)const;
			void describeCastersAsync(const Model::DescribeCastersRequest& request, const DescribeCastersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeCastersOutcomeCallable describeCastersCallable(const Model::DescribeCastersRequest& request) const;
			DescribeForbidPushStreamRoomListOutcome describeForbidPushStreamRoomList(const Model::DescribeForbidPushStreamRoomListRequest &request)const;
			void describeForbidPushStreamRoomListAsync(const Model::DescribeForbidPushStreamRoomListRequest& request, const DescribeForbidPushStreamRoomListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeForbidPushStreamRoomListOutcomeCallable describeForbidPushStreamRoomListCallable(const Model::DescribeForbidPushStreamRoomListRequest& request) const;
			DescribeHlsLiveStreamRealTimeBpsDataOutcome describeHlsLiveStreamRealTimeBpsData(const Model::DescribeHlsLiveStreamRealTimeBpsDataRequest &request)const;
			void describeHlsLiveStreamRealTimeBpsDataAsync(const Model::DescribeHlsLiveStreamRealTimeBpsDataRequest& request, const DescribeHlsLiveStreamRealTimeBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeHlsLiveStreamRealTimeBpsDataOutcomeCallable describeHlsLiveStreamRealTimeBpsDataCallable(const Model::DescribeHlsLiveStreamRealTimeBpsDataRequest& request) const;
			DescribeHtmlResourceOutcome describeHtmlResource(const Model::DescribeHtmlResourceRequest &request)const;
			void describeHtmlResourceAsync(const Model::DescribeHtmlResourceRequest& request, const DescribeHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeHtmlResourceOutcomeCallable describeHtmlResourceCallable(const Model::DescribeHtmlResourceRequest& request) const;
			DescribeLiveAsrConfigOutcome describeLiveAsrConfig(const Model::DescribeLiveAsrConfigRequest &request)const;
			void describeLiveAsrConfigAsync(const Model::DescribeLiveAsrConfigRequest& request, const DescribeLiveAsrConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveAsrConfigOutcomeCallable describeLiveAsrConfigCallable(const Model::DescribeLiveAsrConfigRequest& request) const;
			DescribeLiveAudioAuditConfigOutcome describeLiveAudioAuditConfig(const Model::DescribeLiveAudioAuditConfigRequest &request)const;
			void describeLiveAudioAuditConfigAsync(const Model::DescribeLiveAudioAuditConfigRequest& request, const DescribeLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveAudioAuditConfigOutcomeCallable describeLiveAudioAuditConfigCallable(const Model::DescribeLiveAudioAuditConfigRequest& request) const;
			DescribeLiveAudioAuditNotifyConfigOutcome describeLiveAudioAuditNotifyConfig(const Model::DescribeLiveAudioAuditNotifyConfigRequest &request)const;
			void describeLiveAudioAuditNotifyConfigAsync(const Model::DescribeLiveAudioAuditNotifyConfigRequest& request, const DescribeLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveAudioAuditNotifyConfigOutcomeCallable describeLiveAudioAuditNotifyConfigCallable(const Model::DescribeLiveAudioAuditNotifyConfigRequest& request) const;
			DescribeLiveCertificateDetailOutcome describeLiveCertificateDetail(const Model::DescribeLiveCertificateDetailRequest &request)const;
			void describeLiveCertificateDetailAsync(const Model::DescribeLiveCertificateDetailRequest& request, const DescribeLiveCertificateDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveCertificateDetailOutcomeCallable describeLiveCertificateDetailCallable(const Model::DescribeLiveCertificateDetailRequest& request) const;
			DescribeLiveCertificateListOutcome describeLiveCertificateList(const Model::DescribeLiveCertificateListRequest &request)const;
			void describeLiveCertificateListAsync(const Model::DescribeLiveCertificateListRequest& request, const DescribeLiveCertificateListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveCertificateListOutcomeCallable describeLiveCertificateListCallable(const Model::DescribeLiveCertificateListRequest& request) const;
			DescribeLiveDetectNotifyConfigOutcome describeLiveDetectNotifyConfig(const Model::DescribeLiveDetectNotifyConfigRequest &request)const;
			void describeLiveDetectNotifyConfigAsync(const Model::DescribeLiveDetectNotifyConfigRequest& request, const DescribeLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDetectNotifyConfigOutcomeCallable describeLiveDetectNotifyConfigCallable(const Model::DescribeLiveDetectNotifyConfigRequest& request) const;
			DescribeLiveDetectPornDataOutcome describeLiveDetectPornData(const Model::DescribeLiveDetectPornDataRequest &request)const;
			void describeLiveDetectPornDataAsync(const Model::DescribeLiveDetectPornDataRequest& request, const DescribeLiveDetectPornDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDetectPornDataOutcomeCallable describeLiveDetectPornDataCallable(const Model::DescribeLiveDetectPornDataRequest& request) const;
			DescribeLiveDomainBpsDataOutcome describeLiveDomainBpsData(const Model::DescribeLiveDomainBpsDataRequest &request)const;
			void describeLiveDomainBpsDataAsync(const Model::DescribeLiveDomainBpsDataRequest& request, const DescribeLiveDomainBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainBpsDataOutcomeCallable describeLiveDomainBpsDataCallable(const Model::DescribeLiveDomainBpsDataRequest& request) const;
			DescribeLiveDomainBpsDataByTimeStampOutcome describeLiveDomainBpsDataByTimeStamp(const Model::DescribeLiveDomainBpsDataByTimeStampRequest &request)const;
			void describeLiveDomainBpsDataByTimeStampAsync(const Model::DescribeLiveDomainBpsDataByTimeStampRequest& request, const DescribeLiveDomainBpsDataByTimeStampAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainBpsDataByTimeStampOutcomeCallable describeLiveDomainBpsDataByTimeStampCallable(const Model::DescribeLiveDomainBpsDataByTimeStampRequest& request) const;
			DescribeLiveDomainCertificateInfoOutcome describeLiveDomainCertificateInfo(const Model::DescribeLiveDomainCertificateInfoRequest &request)const;
			void describeLiveDomainCertificateInfoAsync(const Model::DescribeLiveDomainCertificateInfoRequest& request, const DescribeLiveDomainCertificateInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainCertificateInfoOutcomeCallable describeLiveDomainCertificateInfoCallable(const Model::DescribeLiveDomainCertificateInfoRequest& request) const;
			DescribeLiveDomainConfigsOutcome describeLiveDomainConfigs(const Model::DescribeLiveDomainConfigsRequest &request)const;
			void describeLiveDomainConfigsAsync(const Model::DescribeLiveDomainConfigsRequest& request, const DescribeLiveDomainConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainConfigsOutcomeCallable describeLiveDomainConfigsCallable(const Model::DescribeLiveDomainConfigsRequest& request) const;
			DescribeLiveDomainDetailOutcome describeLiveDomainDetail(const Model::DescribeLiveDomainDetailRequest &request)const;
			void describeLiveDomainDetailAsync(const Model::DescribeLiveDomainDetailRequest& request, const DescribeLiveDomainDetailAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainDetailOutcomeCallable describeLiveDomainDetailCallable(const Model::DescribeLiveDomainDetailRequest& request) const;
			DescribeLiveDomainFrameRateAndBitRateDataOutcome describeLiveDomainFrameRateAndBitRateData(const Model::DescribeLiveDomainFrameRateAndBitRateDataRequest &request)const;
			void describeLiveDomainFrameRateAndBitRateDataAsync(const Model::DescribeLiveDomainFrameRateAndBitRateDataRequest& request, const DescribeLiveDomainFrameRateAndBitRateDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainFrameRateAndBitRateDataOutcomeCallable describeLiveDomainFrameRateAndBitRateDataCallable(const Model::DescribeLiveDomainFrameRateAndBitRateDataRequest& request) const;
			DescribeLiveDomainLimitOutcome describeLiveDomainLimit(const Model::DescribeLiveDomainLimitRequest &request)const;
			void describeLiveDomainLimitAsync(const Model::DescribeLiveDomainLimitRequest& request, const DescribeLiveDomainLimitAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainLimitOutcomeCallable describeLiveDomainLimitCallable(const Model::DescribeLiveDomainLimitRequest& request) const;
			DescribeLiveDomainMappingOutcome describeLiveDomainMapping(const Model::DescribeLiveDomainMappingRequest &request)const;
			void describeLiveDomainMappingAsync(const Model::DescribeLiveDomainMappingRequest& request, const DescribeLiveDomainMappingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainMappingOutcomeCallable describeLiveDomainMappingCallable(const Model::DescribeLiveDomainMappingRequest& request) const;
			DescribeLiveDomainOnlineUserNumOutcome describeLiveDomainOnlineUserNum(const Model::DescribeLiveDomainOnlineUserNumRequest &request)const;
			void describeLiveDomainOnlineUserNumAsync(const Model::DescribeLiveDomainOnlineUserNumRequest& request, const DescribeLiveDomainOnlineUserNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainOnlineUserNumOutcomeCallable describeLiveDomainOnlineUserNumCallable(const Model::DescribeLiveDomainOnlineUserNumRequest& request) const;
			DescribeLiveDomainPushBpsDataOutcome describeLiveDomainPushBpsData(const Model::DescribeLiveDomainPushBpsDataRequest &request)const;
			void describeLiveDomainPushBpsDataAsync(const Model::DescribeLiveDomainPushBpsDataRequest& request, const DescribeLiveDomainPushBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainPushBpsDataOutcomeCallable describeLiveDomainPushBpsDataCallable(const Model::DescribeLiveDomainPushBpsDataRequest& request) const;
			DescribeLiveDomainPushTrafficDataOutcome describeLiveDomainPushTrafficData(const Model::DescribeLiveDomainPushTrafficDataRequest &request)const;
			void describeLiveDomainPushTrafficDataAsync(const Model::DescribeLiveDomainPushTrafficDataRequest& request, const DescribeLiveDomainPushTrafficDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainPushTrafficDataOutcomeCallable describeLiveDomainPushTrafficDataCallable(const Model::DescribeLiveDomainPushTrafficDataRequest& request) const;
			DescribeLiveDomainRealTimeBpsDataOutcome describeLiveDomainRealTimeBpsData(const Model::DescribeLiveDomainRealTimeBpsDataRequest &request)const;
			void describeLiveDomainRealTimeBpsDataAsync(const Model::DescribeLiveDomainRealTimeBpsDataRequest& request, const DescribeLiveDomainRealTimeBpsDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainRealTimeBpsDataOutcomeCallable describeLiveDomainRealTimeBpsDataCallable(const Model::DescribeLiveDomainRealTimeBpsDataRequest& request) const;
			DescribeLiveDomainRealTimeHttpCodeDataOutcome describeLiveDomainRealTimeHttpCodeData(const Model::DescribeLiveDomainRealTimeHttpCodeDataRequest &request)const;
			void describeLiveDomainRealTimeHttpCodeDataAsync(const Model::DescribeLiveDomainRealTimeHttpCodeDataRequest& request, const DescribeLiveDomainRealTimeHttpCodeDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainRealTimeHttpCodeDataOutcomeCallable describeLiveDomainRealTimeHttpCodeDataCallable(const Model::DescribeLiveDomainRealTimeHttpCodeDataRequest& request) const;
			DescribeLiveDomainRealTimeTrafficDataOutcome describeLiveDomainRealTimeTrafficData(const Model::DescribeLiveDomainRealTimeTrafficDataRequest &request)const;
			void describeLiveDomainRealTimeTrafficDataAsync(const Model::DescribeLiveDomainRealTimeTrafficDataRequest& request, const DescribeLiveDomainRealTimeTrafficDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainRealTimeTrafficDataOutcomeCallable describeLiveDomainRealTimeTrafficDataCallable(const Model::DescribeLiveDomainRealTimeTrafficDataRequest& request) const;
			DescribeLiveDomainRealtimeLogDeliveryOutcome describeLiveDomainRealtimeLogDelivery(const Model::DescribeLiveDomainRealtimeLogDeliveryRequest &request)const;
			void describeLiveDomainRealtimeLogDeliveryAsync(const Model::DescribeLiveDomainRealtimeLogDeliveryRequest& request, const DescribeLiveDomainRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainRealtimeLogDeliveryOutcomeCallable describeLiveDomainRealtimeLogDeliveryCallable(const Model::DescribeLiveDomainRealtimeLogDeliveryRequest& request) const;
			DescribeLiveDomainRecordDataOutcome describeLiveDomainRecordData(const Model::DescribeLiveDomainRecordDataRequest &request)const;
			void describeLiveDomainRecordDataAsync(const Model::DescribeLiveDomainRecordDataRequest& request, const DescribeLiveDomainRecordDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainRecordDataOutcomeCallable describeLiveDomainRecordDataCallable(const Model::DescribeLiveDomainRecordDataRequest& request) const;
			DescribeLiveDomainSnapshotDataOutcome describeLiveDomainSnapshotData(const Model::DescribeLiveDomainSnapshotDataRequest &request)const;
			void describeLiveDomainSnapshotDataAsync(const Model::DescribeLiveDomainSnapshotDataRequest& request, const DescribeLiveDomainSnapshotDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainSnapshotDataOutcomeCallable describeLiveDomainSnapshotDataCallable(const Model::DescribeLiveDomainSnapshotDataRequest& request) const;
			DescribeLiveDomainTimeShiftDataOutcome describeLiveDomainTimeShiftData(const Model::DescribeLiveDomainTimeShiftDataRequest &request)const;
			void describeLiveDomainTimeShiftDataAsync(const Model::DescribeLiveDomainTimeShiftDataRequest& request, const DescribeLiveDomainTimeShiftDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainTimeShiftDataOutcomeCallable describeLiveDomainTimeShiftDataCallable(const Model::DescribeLiveDomainTimeShiftDataRequest& request) const;
			DescribeLiveDomainTrafficDataOutcome describeLiveDomainTrafficData(const Model::DescribeLiveDomainTrafficDataRequest &request)const;
			void describeLiveDomainTrafficDataAsync(const Model::DescribeLiveDomainTrafficDataRequest& request, const DescribeLiveDomainTrafficDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainTrafficDataOutcomeCallable describeLiveDomainTrafficDataCallable(const Model::DescribeLiveDomainTrafficDataRequest& request) const;
			DescribeLiveDomainTranscodeDataOutcome describeLiveDomainTranscodeData(const Model::DescribeLiveDomainTranscodeDataRequest &request)const;
			void describeLiveDomainTranscodeDataAsync(const Model::DescribeLiveDomainTranscodeDataRequest& request, const DescribeLiveDomainTranscodeDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveDomainTranscodeDataOutcomeCallable describeLiveDomainTranscodeDataCallable(const Model::DescribeLiveDomainTranscodeDataRequest& request) const;
			DescribeLiveLazyPullStreamConfigOutcome describeLiveLazyPullStreamConfig(const Model::DescribeLiveLazyPullStreamConfigRequest &request)const;
			void describeLiveLazyPullStreamConfigAsync(const Model::DescribeLiveLazyPullStreamConfigRequest& request, const DescribeLiveLazyPullStreamConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveLazyPullStreamConfigOutcomeCallable describeLiveLazyPullStreamConfigCallable(const Model::DescribeLiveLazyPullStreamConfigRequest& request) const;
			DescribeLivePullStreamConfigOutcome describeLivePullStreamConfig(const Model::DescribeLivePullStreamConfigRequest &request)const;
			void describeLivePullStreamConfigAsync(const Model::DescribeLivePullStreamConfigRequest& request, const DescribeLivePullStreamConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLivePullStreamConfigOutcomeCallable describeLivePullStreamConfigCallable(const Model::DescribeLivePullStreamConfigRequest& request) const;
			DescribeLiveRealtimeDeliveryAccOutcome describeLiveRealtimeDeliveryAcc(const Model::DescribeLiveRealtimeDeliveryAccRequest &request)const;
			void describeLiveRealtimeDeliveryAccAsync(const Model::DescribeLiveRealtimeDeliveryAccRequest& request, const DescribeLiveRealtimeDeliveryAccAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveRealtimeDeliveryAccOutcomeCallable describeLiveRealtimeDeliveryAccCallable(const Model::DescribeLiveRealtimeDeliveryAccRequest& request) const;
			DescribeLiveRealtimeLogAuthorizedOutcome describeLiveRealtimeLogAuthorized(const Model::DescribeLiveRealtimeLogAuthorizedRequest &request)const;
			void describeLiveRealtimeLogAuthorizedAsync(const Model::DescribeLiveRealtimeLogAuthorizedRequest& request, const DescribeLiveRealtimeLogAuthorizedAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveRealtimeLogAuthorizedOutcomeCallable describeLiveRealtimeLogAuthorizedCallable(const Model::DescribeLiveRealtimeLogAuthorizedRequest& request) const;
			DescribeLiveRecordConfigOutcome describeLiveRecordConfig(const Model::DescribeLiveRecordConfigRequest &request)const;
			void describeLiveRecordConfigAsync(const Model::DescribeLiveRecordConfigRequest& request, const DescribeLiveRecordConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveRecordConfigOutcomeCallable describeLiveRecordConfigCallable(const Model::DescribeLiveRecordConfigRequest& request) const;
			DescribeLiveRecordNotifyConfigOutcome describeLiveRecordNotifyConfig(const Model::DescribeLiveRecordNotifyConfigRequest &request)const;
			void describeLiveRecordNotifyConfigAsync(const Model::DescribeLiveRecordNotifyConfigRequest& request, const DescribeLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveRecordNotifyConfigOutcomeCallable describeLiveRecordNotifyConfigCallable(const Model::DescribeLiveRecordNotifyConfigRequest& request) const;
			DescribeLiveRecordVodConfigsOutcome describeLiveRecordVodConfigs(const Model::DescribeLiveRecordVodConfigsRequest &request)const;
			void describeLiveRecordVodConfigsAsync(const Model::DescribeLiveRecordVodConfigsRequest& request, const DescribeLiveRecordVodConfigsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveRecordVodConfigsOutcomeCallable describeLiveRecordVodConfigsCallable(const Model::DescribeLiveRecordVodConfigsRequest& request) const;
			DescribeLiveSnapshotConfigOutcome describeLiveSnapshotConfig(const Model::DescribeLiveSnapshotConfigRequest &request)const;
			void describeLiveSnapshotConfigAsync(const Model::DescribeLiveSnapshotConfigRequest& request, const DescribeLiveSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveSnapshotConfigOutcomeCallable describeLiveSnapshotConfigCallable(const Model::DescribeLiveSnapshotConfigRequest& request) const;
			DescribeLiveSnapshotDetectPornConfigOutcome describeLiveSnapshotDetectPornConfig(const Model::DescribeLiveSnapshotDetectPornConfigRequest &request)const;
			void describeLiveSnapshotDetectPornConfigAsync(const Model::DescribeLiveSnapshotDetectPornConfigRequest& request, const DescribeLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveSnapshotDetectPornConfigOutcomeCallable describeLiveSnapshotDetectPornConfigCallable(const Model::DescribeLiveSnapshotDetectPornConfigRequest& request) const;
			DescribeLiveStreamBitRateDataOutcome describeLiveStreamBitRateData(const Model::DescribeLiveStreamBitRateDataRequest &request)const;
			void describeLiveStreamBitRateDataAsync(const Model::DescribeLiveStreamBitRateDataRequest& request, const DescribeLiveStreamBitRateDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamBitRateDataOutcomeCallable describeLiveStreamBitRateDataCallable(const Model::DescribeLiveStreamBitRateDataRequest& request) const;
			DescribeLiveStreamCountOutcome describeLiveStreamCount(const Model::DescribeLiveStreamCountRequest &request)const;
			void describeLiveStreamCountAsync(const Model::DescribeLiveStreamCountRequest& request, const DescribeLiveStreamCountAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamCountOutcomeCallable describeLiveStreamCountCallable(const Model::DescribeLiveStreamCountRequest& request) const;
			DescribeLiveStreamDelayConfigOutcome describeLiveStreamDelayConfig(const Model::DescribeLiveStreamDelayConfigRequest &request)const;
			void describeLiveStreamDelayConfigAsync(const Model::DescribeLiveStreamDelayConfigRequest& request, const DescribeLiveStreamDelayConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamDelayConfigOutcomeCallable describeLiveStreamDelayConfigCallable(const Model::DescribeLiveStreamDelayConfigRequest& request) const;
			DescribeLiveStreamHistoryUserNumOutcome describeLiveStreamHistoryUserNum(const Model::DescribeLiveStreamHistoryUserNumRequest &request)const;
			void describeLiveStreamHistoryUserNumAsync(const Model::DescribeLiveStreamHistoryUserNumRequest& request, const DescribeLiveStreamHistoryUserNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamHistoryUserNumOutcomeCallable describeLiveStreamHistoryUserNumCallable(const Model::DescribeLiveStreamHistoryUserNumRequest& request) const;
			DescribeLiveStreamOnlineUserNumOutcome describeLiveStreamOnlineUserNum(const Model::DescribeLiveStreamOnlineUserNumRequest &request)const;
			void describeLiveStreamOnlineUserNumAsync(const Model::DescribeLiveStreamOnlineUserNumRequest& request, const DescribeLiveStreamOnlineUserNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamOnlineUserNumOutcomeCallable describeLiveStreamOnlineUserNumCallable(const Model::DescribeLiveStreamOnlineUserNumRequest& request) const;
			DescribeLiveStreamOptimizedFeatureConfigOutcome describeLiveStreamOptimizedFeatureConfig(const Model::DescribeLiveStreamOptimizedFeatureConfigRequest &request)const;
			void describeLiveStreamOptimizedFeatureConfigAsync(const Model::DescribeLiveStreamOptimizedFeatureConfigRequest& request, const DescribeLiveStreamOptimizedFeatureConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamOptimizedFeatureConfigOutcomeCallable describeLiveStreamOptimizedFeatureConfigCallable(const Model::DescribeLiveStreamOptimizedFeatureConfigRequest& request) const;
			DescribeLiveStreamRecordContentOutcome describeLiveStreamRecordContent(const Model::DescribeLiveStreamRecordContentRequest &request)const;
			void describeLiveStreamRecordContentAsync(const Model::DescribeLiveStreamRecordContentRequest& request, const DescribeLiveStreamRecordContentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamRecordContentOutcomeCallable describeLiveStreamRecordContentCallable(const Model::DescribeLiveStreamRecordContentRequest& request) const;
			DescribeLiveStreamRecordIndexFileOutcome describeLiveStreamRecordIndexFile(const Model::DescribeLiveStreamRecordIndexFileRequest &request)const;
			void describeLiveStreamRecordIndexFileAsync(const Model::DescribeLiveStreamRecordIndexFileRequest& request, const DescribeLiveStreamRecordIndexFileAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamRecordIndexFileOutcomeCallable describeLiveStreamRecordIndexFileCallable(const Model::DescribeLiveStreamRecordIndexFileRequest& request) const;
			DescribeLiveStreamRecordIndexFilesOutcome describeLiveStreamRecordIndexFiles(const Model::DescribeLiveStreamRecordIndexFilesRequest &request)const;
			void describeLiveStreamRecordIndexFilesAsync(const Model::DescribeLiveStreamRecordIndexFilesRequest& request, const DescribeLiveStreamRecordIndexFilesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamRecordIndexFilesOutcomeCallable describeLiveStreamRecordIndexFilesCallable(const Model::DescribeLiveStreamRecordIndexFilesRequest& request) const;
			DescribeLiveStreamSnapshotInfoOutcome describeLiveStreamSnapshotInfo(const Model::DescribeLiveStreamSnapshotInfoRequest &request)const;
			void describeLiveStreamSnapshotInfoAsync(const Model::DescribeLiveStreamSnapshotInfoRequest& request, const DescribeLiveStreamSnapshotInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamSnapshotInfoOutcomeCallable describeLiveStreamSnapshotInfoCallable(const Model::DescribeLiveStreamSnapshotInfoRequest& request) const;
			DescribeLiveStreamTranscodeInfoOutcome describeLiveStreamTranscodeInfo(const Model::DescribeLiveStreamTranscodeInfoRequest &request)const;
			void describeLiveStreamTranscodeInfoAsync(const Model::DescribeLiveStreamTranscodeInfoRequest& request, const DescribeLiveStreamTranscodeInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamTranscodeInfoOutcomeCallable describeLiveStreamTranscodeInfoCallable(const Model::DescribeLiveStreamTranscodeInfoRequest& request) const;
			DescribeLiveStreamTranscodeStreamNumOutcome describeLiveStreamTranscodeStreamNum(const Model::DescribeLiveStreamTranscodeStreamNumRequest &request)const;
			void describeLiveStreamTranscodeStreamNumAsync(const Model::DescribeLiveStreamTranscodeStreamNumRequest& request, const DescribeLiveStreamTranscodeStreamNumAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamTranscodeStreamNumOutcomeCallable describeLiveStreamTranscodeStreamNumCallable(const Model::DescribeLiveStreamTranscodeStreamNumRequest& request) const;
			DescribeLiveStreamsBlockListOutcome describeLiveStreamsBlockList(const Model::DescribeLiveStreamsBlockListRequest &request)const;
			void describeLiveStreamsBlockListAsync(const Model::DescribeLiveStreamsBlockListRequest& request, const DescribeLiveStreamsBlockListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamsBlockListOutcomeCallable describeLiveStreamsBlockListCallable(const Model::DescribeLiveStreamsBlockListRequest& request) const;
			DescribeLiveStreamsControlHistoryOutcome describeLiveStreamsControlHistory(const Model::DescribeLiveStreamsControlHistoryRequest &request)const;
			void describeLiveStreamsControlHistoryAsync(const Model::DescribeLiveStreamsControlHistoryRequest& request, const DescribeLiveStreamsControlHistoryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamsControlHistoryOutcomeCallable describeLiveStreamsControlHistoryCallable(const Model::DescribeLiveStreamsControlHistoryRequest& request) const;
			DescribeLiveStreamsFrameRateAndBitRateDataOutcome describeLiveStreamsFrameRateAndBitRateData(const Model::DescribeLiveStreamsFrameRateAndBitRateDataRequest &request)const;
			void describeLiveStreamsFrameRateAndBitRateDataAsync(const Model::DescribeLiveStreamsFrameRateAndBitRateDataRequest& request, const DescribeLiveStreamsFrameRateAndBitRateDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamsFrameRateAndBitRateDataOutcomeCallable describeLiveStreamsFrameRateAndBitRateDataCallable(const Model::DescribeLiveStreamsFrameRateAndBitRateDataRequest& request) const;
			DescribeLiveStreamsNotifyUrlConfigOutcome describeLiveStreamsNotifyUrlConfig(const Model::DescribeLiveStreamsNotifyUrlConfigRequest &request)const;
			void describeLiveStreamsNotifyUrlConfigAsync(const Model::DescribeLiveStreamsNotifyUrlConfigRequest& request, const DescribeLiveStreamsNotifyUrlConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamsNotifyUrlConfigOutcomeCallable describeLiveStreamsNotifyUrlConfigCallable(const Model::DescribeLiveStreamsNotifyUrlConfigRequest& request) const;
			DescribeLiveStreamsOnlineListOutcome describeLiveStreamsOnlineList(const Model::DescribeLiveStreamsOnlineListRequest &request)const;
			void describeLiveStreamsOnlineListAsync(const Model::DescribeLiveStreamsOnlineListRequest& request, const DescribeLiveStreamsOnlineListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamsOnlineListOutcomeCallable describeLiveStreamsOnlineListCallable(const Model::DescribeLiveStreamsOnlineListRequest& request) const;
			DescribeLiveStreamsPublishListOutcome describeLiveStreamsPublishList(const Model::DescribeLiveStreamsPublishListRequest &request)const;
			void describeLiveStreamsPublishListAsync(const Model::DescribeLiveStreamsPublishListRequest& request, const DescribeLiveStreamsPublishListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveStreamsPublishListOutcomeCallable describeLiveStreamsPublishListCallable(const Model::DescribeLiveStreamsPublishListRequest& request) const;
			DescribeLiveTagResourcesOutcome describeLiveTagResources(const Model::DescribeLiveTagResourcesRequest &request)const;
			void describeLiveTagResourcesAsync(const Model::DescribeLiveTagResourcesRequest& request, const DescribeLiveTagResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveTagResourcesOutcomeCallable describeLiveTagResourcesCallable(const Model::DescribeLiveTagResourcesRequest& request) const;
			DescribeLiveTopDomainsByFlowOutcome describeLiveTopDomainsByFlow(const Model::DescribeLiveTopDomainsByFlowRequest &request)const;
			void describeLiveTopDomainsByFlowAsync(const Model::DescribeLiveTopDomainsByFlowRequest& request, const DescribeLiveTopDomainsByFlowAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveTopDomainsByFlowOutcomeCallable describeLiveTopDomainsByFlowCallable(const Model::DescribeLiveTopDomainsByFlowRequest& request) const;
			DescribeLiveUserDomainsOutcome describeLiveUserDomains(const Model::DescribeLiveUserDomainsRequest &request)const;
			void describeLiveUserDomainsAsync(const Model::DescribeLiveUserDomainsRequest& request, const DescribeLiveUserDomainsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveUserDomainsOutcomeCallable describeLiveUserDomainsCallable(const Model::DescribeLiveUserDomainsRequest& request) const;
			DescribeLiveUserTagsOutcome describeLiveUserTags(const Model::DescribeLiveUserTagsRequest &request)const;
			void describeLiveUserTagsAsync(const Model::DescribeLiveUserTagsRequest& request, const DescribeLiveUserTagsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeLiveUserTagsOutcomeCallable describeLiveUserTagsCallable(const Model::DescribeLiveUserTagsRequest& request) const;
			DescribeMixStreamListOutcome describeMixStreamList(const Model::DescribeMixStreamListRequest &request)const;
			void describeMixStreamListAsync(const Model::DescribeMixStreamListRequest& request, const DescribeMixStreamListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeMixStreamListOutcomeCallable describeMixStreamListCallable(const Model::DescribeMixStreamListRequest& request) const;
			DescribeRecordOutcome describeRecord(const Model::DescribeRecordRequest &request)const;
			void describeRecordAsync(const Model::DescribeRecordRequest& request, const DescribeRecordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRecordOutcomeCallable describeRecordCallable(const Model::DescribeRecordRequest& request) const;
			DescribeRecordsOutcome describeRecords(const Model::DescribeRecordsRequest &request)const;
			void describeRecordsAsync(const Model::DescribeRecordsRequest& request, const DescribeRecordsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRecordsOutcomeCallable describeRecordsCallable(const Model::DescribeRecordsRequest& request) const;
			DescribeRoomKickoutUserListOutcome describeRoomKickoutUserList(const Model::DescribeRoomKickoutUserListRequest &request)const;
			void describeRoomKickoutUserListAsync(const Model::DescribeRoomKickoutUserListRequest& request, const DescribeRoomKickoutUserListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRoomKickoutUserListOutcomeCallable describeRoomKickoutUserListCallable(const Model::DescribeRoomKickoutUserListRequest& request) const;
			DescribeRoomListOutcome describeRoomList(const Model::DescribeRoomListRequest &request)const;
			void describeRoomListAsync(const Model::DescribeRoomListRequest& request, const DescribeRoomListAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRoomListOutcomeCallable describeRoomListCallable(const Model::DescribeRoomListRequest& request) const;
			DescribeRoomStatusOutcome describeRoomStatus(const Model::DescribeRoomStatusRequest &request)const;
			void describeRoomStatusAsync(const Model::DescribeRoomStatusRequest& request, const DescribeRoomStatusAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeRoomStatusOutcomeCallable describeRoomStatusCallable(const Model::DescribeRoomStatusRequest& request) const;
			DescribeUpBpsPeakDataOutcome describeUpBpsPeakData(const Model::DescribeUpBpsPeakDataRequest &request)const;
			void describeUpBpsPeakDataAsync(const Model::DescribeUpBpsPeakDataRequest& request, const DescribeUpBpsPeakDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeUpBpsPeakDataOutcomeCallable describeUpBpsPeakDataCallable(const Model::DescribeUpBpsPeakDataRequest& request) const;
			DescribeUpBpsPeakOfLineOutcome describeUpBpsPeakOfLine(const Model::DescribeUpBpsPeakOfLineRequest &request)const;
			void describeUpBpsPeakOfLineAsync(const Model::DescribeUpBpsPeakOfLineRequest& request, const DescribeUpBpsPeakOfLineAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeUpBpsPeakOfLineOutcomeCallable describeUpBpsPeakOfLineCallable(const Model::DescribeUpBpsPeakOfLineRequest& request) const;
			DescribeUpPeakPublishStreamDataOutcome describeUpPeakPublishStreamData(const Model::DescribeUpPeakPublishStreamDataRequest &request)const;
			void describeUpPeakPublishStreamDataAsync(const Model::DescribeUpPeakPublishStreamDataRequest& request, const DescribeUpPeakPublishStreamDataAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DescribeUpPeakPublishStreamDataOutcomeCallable describeUpPeakPublishStreamDataCallable(const Model::DescribeUpPeakPublishStreamDataRequest& request) const;
			DisableLiveRealtimeLogDeliveryOutcome disableLiveRealtimeLogDelivery(const Model::DisableLiveRealtimeLogDeliveryRequest &request)const;
			void disableLiveRealtimeLogDeliveryAsync(const Model::DisableLiveRealtimeLogDeliveryRequest& request, const DisableLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DisableLiveRealtimeLogDeliveryOutcomeCallable disableLiveRealtimeLogDeliveryCallable(const Model::DisableLiveRealtimeLogDeliveryRequest& request) const;
			EditHtmlResourceOutcome editHtmlResource(const Model::EditHtmlResourceRequest &request)const;
			void editHtmlResourceAsync(const Model::EditHtmlResourceRequest& request, const EditHtmlResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EditHtmlResourceOutcomeCallable editHtmlResourceCallable(const Model::EditHtmlResourceRequest& request) const;
			EffectCasterUrgentOutcome effectCasterUrgent(const Model::EffectCasterUrgentRequest &request)const;
			void effectCasterUrgentAsync(const Model::EffectCasterUrgentRequest& request, const EffectCasterUrgentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EffectCasterUrgentOutcomeCallable effectCasterUrgentCallable(const Model::EffectCasterUrgentRequest& request) const;
			EffectCasterVideoResourceOutcome effectCasterVideoResource(const Model::EffectCasterVideoResourceRequest &request)const;
			void effectCasterVideoResourceAsync(const Model::EffectCasterVideoResourceRequest& request, const EffectCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EffectCasterVideoResourceOutcomeCallable effectCasterVideoResourceCallable(const Model::EffectCasterVideoResourceRequest& request) const;
			EnableLiveRealtimeLogDeliveryOutcome enableLiveRealtimeLogDelivery(const Model::EnableLiveRealtimeLogDeliveryRequest &request)const;
			void enableLiveRealtimeLogDeliveryAsync(const Model::EnableLiveRealtimeLogDeliveryRequest& request, const EnableLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnableLiveRealtimeLogDeliveryOutcomeCallable enableLiveRealtimeLogDeliveryCallable(const Model::EnableLiveRealtimeLogDeliveryRequest& request) const;
			ForbidLiveStreamOutcome forbidLiveStream(const Model::ForbidLiveStreamRequest &request)const;
			void forbidLiveStreamAsync(const Model::ForbidLiveStreamRequest& request, const ForbidLiveStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ForbidLiveStreamOutcomeCallable forbidLiveStreamCallable(const Model::ForbidLiveStreamRequest& request) const;
			ForbidPushStreamOutcome forbidPushStream(const Model::ForbidPushStreamRequest &request)const;
			void forbidPushStreamAsync(const Model::ForbidPushStreamRequest& request, const ForbidPushStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ForbidPushStreamOutcomeCallable forbidPushStreamCallable(const Model::ForbidPushStreamRequest& request) const;
			JoinBoardOutcome joinBoard(const Model::JoinBoardRequest &request)const;
			void joinBoardAsync(const Model::JoinBoardRequest& request, const JoinBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			JoinBoardOutcomeCallable joinBoardCallable(const Model::JoinBoardRequest& request) const;
			ListLiveRealtimeLogDeliveryOutcome listLiveRealtimeLogDelivery(const Model::ListLiveRealtimeLogDeliveryRequest &request)const;
			void listLiveRealtimeLogDeliveryAsync(const Model::ListLiveRealtimeLogDeliveryRequest& request, const ListLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListLiveRealtimeLogDeliveryOutcomeCallable listLiveRealtimeLogDeliveryCallable(const Model::ListLiveRealtimeLogDeliveryRequest& request) const;
			ListLiveRealtimeLogDeliveryDomainsOutcome listLiveRealtimeLogDeliveryDomains(const Model::ListLiveRealtimeLogDeliveryDomainsRequest &request)const;
			void listLiveRealtimeLogDeliveryDomainsAsync(const Model::ListLiveRealtimeLogDeliveryDomainsRequest& request, const ListLiveRealtimeLogDeliveryDomainsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListLiveRealtimeLogDeliveryDomainsOutcomeCallable listLiveRealtimeLogDeliveryDomainsCallable(const Model::ListLiveRealtimeLogDeliveryDomainsRequest& request) const;
			ListLiveRealtimeLogDeliveryInfosOutcome listLiveRealtimeLogDeliveryInfos(const Model::ListLiveRealtimeLogDeliveryInfosRequest &request)const;
			void listLiveRealtimeLogDeliveryInfosAsync(const Model::ListLiveRealtimeLogDeliveryInfosRequest& request, const ListLiveRealtimeLogDeliveryInfosAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListLiveRealtimeLogDeliveryInfosOutcomeCallable listLiveRealtimeLogDeliveryInfosCallable(const Model::ListLiveRealtimeLogDeliveryInfosRequest& request) const;
			ModifyCasterComponentOutcome modifyCasterComponent(const Model::ModifyCasterComponentRequest &request)const;
			void modifyCasterComponentAsync(const Model::ModifyCasterComponentRequest& request, const ModifyCasterComponentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyCasterComponentOutcomeCallable modifyCasterComponentCallable(const Model::ModifyCasterComponentRequest& request) const;
			ModifyCasterEpisodeOutcome modifyCasterEpisode(const Model::ModifyCasterEpisodeRequest &request)const;
			void modifyCasterEpisodeAsync(const Model::ModifyCasterEpisodeRequest& request, const ModifyCasterEpisodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyCasterEpisodeOutcomeCallable modifyCasterEpisodeCallable(const Model::ModifyCasterEpisodeRequest& request) const;
			ModifyCasterLayoutOutcome modifyCasterLayout(const Model::ModifyCasterLayoutRequest &request)const;
			void modifyCasterLayoutAsync(const Model::ModifyCasterLayoutRequest& request, const ModifyCasterLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyCasterLayoutOutcomeCallable modifyCasterLayoutCallable(const Model::ModifyCasterLayoutRequest& request) const;
			ModifyCasterProgramOutcome modifyCasterProgram(const Model::ModifyCasterProgramRequest &request)const;
			void modifyCasterProgramAsync(const Model::ModifyCasterProgramRequest& request, const ModifyCasterProgramAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyCasterProgramOutcomeCallable modifyCasterProgramCallable(const Model::ModifyCasterProgramRequest& request) const;
			ModifyCasterVideoResourceOutcome modifyCasterVideoResource(const Model::ModifyCasterVideoResourceRequest &request)const;
			void modifyCasterVideoResourceAsync(const Model::ModifyCasterVideoResourceRequest& request, const ModifyCasterVideoResourceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyCasterVideoResourceOutcomeCallable modifyCasterVideoResourceCallable(const Model::ModifyCasterVideoResourceRequest& request) const;
			ModifyLiveDomainSchdmByPropertyOutcome modifyLiveDomainSchdmByProperty(const Model::ModifyLiveDomainSchdmByPropertyRequest &request)const;
			void modifyLiveDomainSchdmByPropertyAsync(const Model::ModifyLiveDomainSchdmByPropertyRequest& request, const ModifyLiveDomainSchdmByPropertyAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyLiveDomainSchdmByPropertyOutcomeCallable modifyLiveDomainSchdmByPropertyCallable(const Model::ModifyLiveDomainSchdmByPropertyRequest& request) const;
			ModifyLiveRealtimeLogDeliveryOutcome modifyLiveRealtimeLogDelivery(const Model::ModifyLiveRealtimeLogDeliveryRequest &request)const;
			void modifyLiveRealtimeLogDeliveryAsync(const Model::ModifyLiveRealtimeLogDeliveryRequest& request, const ModifyLiveRealtimeLogDeliveryAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyLiveRealtimeLogDeliveryOutcomeCallable modifyLiveRealtimeLogDeliveryCallable(const Model::ModifyLiveRealtimeLogDeliveryRequest& request) const;
			RealTimeRecordCommandOutcome realTimeRecordCommand(const Model::RealTimeRecordCommandRequest &request)const;
			void realTimeRecordCommandAsync(const Model::RealTimeRecordCommandRequest& request, const RealTimeRecordCommandAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RealTimeRecordCommandOutcomeCallable realTimeRecordCommandCallable(const Model::RealTimeRecordCommandRequest& request) const;
			RealTimeSnapshotCommandOutcome realTimeSnapshotCommand(const Model::RealTimeSnapshotCommandRequest &request)const;
			void realTimeSnapshotCommandAsync(const Model::RealTimeSnapshotCommandRequest& request, const RealTimeSnapshotCommandAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RealTimeSnapshotCommandOutcomeCallable realTimeSnapshotCommandCallable(const Model::RealTimeSnapshotCommandRequest& request) const;
			ResumeLiveStreamOutcome resumeLiveStream(const Model::ResumeLiveStreamRequest &request)const;
			void resumeLiveStreamAsync(const Model::ResumeLiveStreamRequest& request, const ResumeLiveStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ResumeLiveStreamOutcomeCallable resumeLiveStreamCallable(const Model::ResumeLiveStreamRequest& request) const;
			SendRoomNotificationOutcome sendRoomNotification(const Model::SendRoomNotificationRequest &request)const;
			void sendRoomNotificationAsync(const Model::SendRoomNotificationRequest& request, const SendRoomNotificationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SendRoomNotificationOutcomeCallable sendRoomNotificationCallable(const Model::SendRoomNotificationRequest& request) const;
			SendRoomUserNotificationOutcome sendRoomUserNotification(const Model::SendRoomUserNotificationRequest &request)const;
			void sendRoomUserNotificationAsync(const Model::SendRoomUserNotificationRequest& request, const SendRoomUserNotificationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SendRoomUserNotificationOutcomeCallable sendRoomUserNotificationCallable(const Model::SendRoomUserNotificationRequest& request) const;
			SetBoardCallbackOutcome setBoardCallback(const Model::SetBoardCallbackRequest &request)const;
			void setBoardCallbackAsync(const Model::SetBoardCallbackRequest& request, const SetBoardCallbackAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetBoardCallbackOutcomeCallable setBoardCallbackCallable(const Model::SetBoardCallbackRequest& request) const;
			SetCasterChannelOutcome setCasterChannel(const Model::SetCasterChannelRequest &request)const;
			void setCasterChannelAsync(const Model::SetCasterChannelRequest& request, const SetCasterChannelAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetCasterChannelOutcomeCallable setCasterChannelCallable(const Model::SetCasterChannelRequest& request) const;
			SetCasterConfigOutcome setCasterConfig(const Model::SetCasterConfigRequest &request)const;
			void setCasterConfigAsync(const Model::SetCasterConfigRequest& request, const SetCasterConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetCasterConfigOutcomeCallable setCasterConfigCallable(const Model::SetCasterConfigRequest& request) const;
			SetCasterSceneConfigOutcome setCasterSceneConfig(const Model::SetCasterSceneConfigRequest &request)const;
			void setCasterSceneConfigAsync(const Model::SetCasterSceneConfigRequest& request, const SetCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetCasterSceneConfigOutcomeCallable setCasterSceneConfigCallable(const Model::SetCasterSceneConfigRequest& request) const;
			SetCasterSyncGroupOutcome setCasterSyncGroup(const Model::SetCasterSyncGroupRequest &request)const;
			void setCasterSyncGroupAsync(const Model::SetCasterSyncGroupRequest& request, const SetCasterSyncGroupAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetCasterSyncGroupOutcomeCallable setCasterSyncGroupCallable(const Model::SetCasterSyncGroupRequest& request) const;
			SetLiveDomainCertificateOutcome setLiveDomainCertificate(const Model::SetLiveDomainCertificateRequest &request)const;
			void setLiveDomainCertificateAsync(const Model::SetLiveDomainCertificateRequest& request, const SetLiveDomainCertificateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetLiveDomainCertificateOutcomeCallable setLiveDomainCertificateCallable(const Model::SetLiveDomainCertificateRequest& request) const;
			SetLiveLazyPullStreamInfoConfigOutcome setLiveLazyPullStreamInfoConfig(const Model::SetLiveLazyPullStreamInfoConfigRequest &request)const;
			void setLiveLazyPullStreamInfoConfigAsync(const Model::SetLiveLazyPullStreamInfoConfigRequest& request, const SetLiveLazyPullStreamInfoConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetLiveLazyPullStreamInfoConfigOutcomeCallable setLiveLazyPullStreamInfoConfigCallable(const Model::SetLiveLazyPullStreamInfoConfigRequest& request) const;
			SetLiveStreamDelayConfigOutcome setLiveStreamDelayConfig(const Model::SetLiveStreamDelayConfigRequest &request)const;
			void setLiveStreamDelayConfigAsync(const Model::SetLiveStreamDelayConfigRequest& request, const SetLiveStreamDelayConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetLiveStreamDelayConfigOutcomeCallable setLiveStreamDelayConfigCallable(const Model::SetLiveStreamDelayConfigRequest& request) const;
			SetLiveStreamOptimizedFeatureConfigOutcome setLiveStreamOptimizedFeatureConfig(const Model::SetLiveStreamOptimizedFeatureConfigRequest &request)const;
			void setLiveStreamOptimizedFeatureConfigAsync(const Model::SetLiveStreamOptimizedFeatureConfigRequest& request, const SetLiveStreamOptimizedFeatureConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetLiveStreamOptimizedFeatureConfigOutcomeCallable setLiveStreamOptimizedFeatureConfigCallable(const Model::SetLiveStreamOptimizedFeatureConfigRequest& request) const;
			SetLiveStreamsNotifyUrlConfigOutcome setLiveStreamsNotifyUrlConfig(const Model::SetLiveStreamsNotifyUrlConfigRequest &request)const;
			void setLiveStreamsNotifyUrlConfigAsync(const Model::SetLiveStreamsNotifyUrlConfigRequest& request, const SetLiveStreamsNotifyUrlConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SetLiveStreamsNotifyUrlConfigOutcomeCallable setLiveStreamsNotifyUrlConfigCallable(const Model::SetLiveStreamsNotifyUrlConfigRequest& request) const;
			StartBoardRecordOutcome startBoardRecord(const Model::StartBoardRecordRequest &request)const;
			void startBoardRecordAsync(const Model::StartBoardRecordRequest& request, const StartBoardRecordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartBoardRecordOutcomeCallable startBoardRecordCallable(const Model::StartBoardRecordRequest& request) const;
			StartCasterOutcome startCaster(const Model::StartCasterRequest &request)const;
			void startCasterAsync(const Model::StartCasterRequest& request, const StartCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartCasterOutcomeCallable startCasterCallable(const Model::StartCasterRequest& request) const;
			StartCasterSceneOutcome startCasterScene(const Model::StartCasterSceneRequest &request)const;
			void startCasterSceneAsync(const Model::StartCasterSceneRequest& request, const StartCasterSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartCasterSceneOutcomeCallable startCasterSceneCallable(const Model::StartCasterSceneRequest& request) const;
			StartLiveDomainOutcome startLiveDomain(const Model::StartLiveDomainRequest &request)const;
			void startLiveDomainAsync(const Model::StartLiveDomainRequest& request, const StartLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartLiveDomainOutcomeCallable startLiveDomainCallable(const Model::StartLiveDomainRequest& request) const;
			StartLiveIndexOutcome startLiveIndex(const Model::StartLiveIndexRequest &request)const;
			void startLiveIndexAsync(const Model::StartLiveIndexRequest& request, const StartLiveIndexAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartLiveIndexOutcomeCallable startLiveIndexCallable(const Model::StartLiveIndexRequest& request) const;
			StopCasterOutcome stopCaster(const Model::StopCasterRequest &request)const;
			void stopCasterAsync(const Model::StopCasterRequest& request, const StopCasterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopCasterOutcomeCallable stopCasterCallable(const Model::StopCasterRequest& request) const;
			StopCasterSceneOutcome stopCasterScene(const Model::StopCasterSceneRequest &request)const;
			void stopCasterSceneAsync(const Model::StopCasterSceneRequest& request, const StopCasterSceneAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopCasterSceneOutcomeCallable stopCasterSceneCallable(const Model::StopCasterSceneRequest& request) const;
			StopLiveDomainOutcome stopLiveDomain(const Model::StopLiveDomainRequest &request)const;
			void stopLiveDomainAsync(const Model::StopLiveDomainRequest& request, const StopLiveDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopLiveDomainOutcomeCallable stopLiveDomainCallable(const Model::StopLiveDomainRequest& request) const;
			StopLiveIndexOutcome stopLiveIndex(const Model::StopLiveIndexRequest &request)const;
			void stopLiveIndexAsync(const Model::StopLiveIndexRequest& request, const StopLiveIndexAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StopLiveIndexOutcomeCallable stopLiveIndexCallable(const Model::StopLiveIndexRequest& request) const;
			TagLiveResourcesOutcome tagLiveResources(const Model::TagLiveResourcesRequest &request)const;
			void tagLiveResourcesAsync(const Model::TagLiveResourcesRequest& request, const TagLiveResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TagLiveResourcesOutcomeCallable tagLiveResourcesCallable(const Model::TagLiveResourcesRequest& request) const;
			UnTagLiveResourcesOutcome unTagLiveResources(const Model::UnTagLiveResourcesRequest &request)const;
			void unTagLiveResourcesAsync(const Model::UnTagLiveResourcesRequest& request, const UnTagLiveResourcesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UnTagLiveResourcesOutcomeCallable unTagLiveResourcesCallable(const Model::UnTagLiveResourcesRequest& request) const;
			UpdateBoardOutcome updateBoard(const Model::UpdateBoardRequest &request)const;
			void updateBoardAsync(const Model::UpdateBoardRequest& request, const UpdateBoardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateBoardOutcomeCallable updateBoardCallable(const Model::UpdateBoardRequest& request) const;
			UpdateBoardCallbackOutcome updateBoardCallback(const Model::UpdateBoardCallbackRequest &request)const;
			void updateBoardCallbackAsync(const Model::UpdateBoardCallbackRequest& request, const UpdateBoardCallbackAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateBoardCallbackOutcomeCallable updateBoardCallbackCallable(const Model::UpdateBoardCallbackRequest& request) const;
			UpdateCasterSceneAudioOutcome updateCasterSceneAudio(const Model::UpdateCasterSceneAudioRequest &request)const;
			void updateCasterSceneAudioAsync(const Model::UpdateCasterSceneAudioRequest& request, const UpdateCasterSceneAudioAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateCasterSceneAudioOutcomeCallable updateCasterSceneAudioCallable(const Model::UpdateCasterSceneAudioRequest& request) const;
			UpdateCasterSceneConfigOutcome updateCasterSceneConfig(const Model::UpdateCasterSceneConfigRequest &request)const;
			void updateCasterSceneConfigAsync(const Model::UpdateCasterSceneConfigRequest& request, const UpdateCasterSceneConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateCasterSceneConfigOutcomeCallable updateCasterSceneConfigCallable(const Model::UpdateCasterSceneConfigRequest& request) const;
			UpdateLiveASRConfigOutcome updateLiveASRConfig(const Model::UpdateLiveASRConfigRequest &request)const;
			void updateLiveASRConfigAsync(const Model::UpdateLiveASRConfigRequest& request, const UpdateLiveASRConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveASRConfigOutcomeCallable updateLiveASRConfigCallable(const Model::UpdateLiveASRConfigRequest& request) const;
			UpdateLiveAppSnapshotConfigOutcome updateLiveAppSnapshotConfig(const Model::UpdateLiveAppSnapshotConfigRequest &request)const;
			void updateLiveAppSnapshotConfigAsync(const Model::UpdateLiveAppSnapshotConfigRequest& request, const UpdateLiveAppSnapshotConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveAppSnapshotConfigOutcomeCallable updateLiveAppSnapshotConfigCallable(const Model::UpdateLiveAppSnapshotConfigRequest& request) const;
			UpdateLiveAudioAuditConfigOutcome updateLiveAudioAuditConfig(const Model::UpdateLiveAudioAuditConfigRequest &request)const;
			void updateLiveAudioAuditConfigAsync(const Model::UpdateLiveAudioAuditConfigRequest& request, const UpdateLiveAudioAuditConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveAudioAuditConfigOutcomeCallable updateLiveAudioAuditConfigCallable(const Model::UpdateLiveAudioAuditConfigRequest& request) const;
			UpdateLiveAudioAuditNotifyConfigOutcome updateLiveAudioAuditNotifyConfig(const Model::UpdateLiveAudioAuditNotifyConfigRequest &request)const;
			void updateLiveAudioAuditNotifyConfigAsync(const Model::UpdateLiveAudioAuditNotifyConfigRequest& request, const UpdateLiveAudioAuditNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveAudioAuditNotifyConfigOutcomeCallable updateLiveAudioAuditNotifyConfigCallable(const Model::UpdateLiveAudioAuditNotifyConfigRequest& request) const;
			UpdateLiveDetectNotifyConfigOutcome updateLiveDetectNotifyConfig(const Model::UpdateLiveDetectNotifyConfigRequest &request)const;
			void updateLiveDetectNotifyConfigAsync(const Model::UpdateLiveDetectNotifyConfigRequest& request, const UpdateLiveDetectNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveDetectNotifyConfigOutcomeCallable updateLiveDetectNotifyConfigCallable(const Model::UpdateLiveDetectNotifyConfigRequest& request) const;
			UpdateLiveRecordNotifyConfigOutcome updateLiveRecordNotifyConfig(const Model::UpdateLiveRecordNotifyConfigRequest &request)const;
			void updateLiveRecordNotifyConfigAsync(const Model::UpdateLiveRecordNotifyConfigRequest& request, const UpdateLiveRecordNotifyConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveRecordNotifyConfigOutcomeCallable updateLiveRecordNotifyConfigCallable(const Model::UpdateLiveRecordNotifyConfigRequest& request) const;
			UpdateLiveSnapshotDetectPornConfigOutcome updateLiveSnapshotDetectPornConfig(const Model::UpdateLiveSnapshotDetectPornConfigRequest &request)const;
			void updateLiveSnapshotDetectPornConfigAsync(const Model::UpdateLiveSnapshotDetectPornConfigRequest& request, const UpdateLiveSnapshotDetectPornConfigAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveSnapshotDetectPornConfigOutcomeCallable updateLiveSnapshotDetectPornConfigCallable(const Model::UpdateLiveSnapshotDetectPornConfigRequest& request) const;
			UpdateLiveTopLevelDomainOutcome updateLiveTopLevelDomain(const Model::UpdateLiveTopLevelDomainRequest &request)const;
			void updateLiveTopLevelDomainAsync(const Model::UpdateLiveTopLevelDomainRequest& request, const UpdateLiveTopLevelDomainAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLiveTopLevelDomainOutcomeCallable updateLiveTopLevelDomainCallable(const Model::UpdateLiveTopLevelDomainRequest& request) const;
			UpdateMixStreamOutcome updateMixStream(const Model::UpdateMixStreamRequest &request)const;
			void updateMixStreamAsync(const Model::UpdateMixStreamRequest& request, const UpdateMixStreamAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateMixStreamOutcomeCallable updateMixStreamCallable(const Model::UpdateMixStreamRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_LIVE_LIVECLIENT_H_

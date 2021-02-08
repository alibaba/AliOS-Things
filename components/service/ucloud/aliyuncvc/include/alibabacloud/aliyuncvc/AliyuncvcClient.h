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

#ifndef ALIBABACLOUD_ALIYUNCVC_ALIYUNCVCCLIENT_H_
#define ALIBABACLOUD_ALIYUNCVC_ALIYUNCVCCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "AliyuncvcExport.h"
#include "model/ActiveDeviceRequest.h"
#include "model/ActiveDeviceResult.h"
#include "model/ActiveMeetingRequest.h"
#include "model/ActiveMeetingResult.h"
#include "model/BatchCreateDeviceRequest.h"
#include "model/BatchCreateDeviceResult.h"
#include "model/BatchDeleteDevicesRequest.h"
#include "model/BatchDeleteDevicesResult.h"
#include "model/CallDeviceRequest.h"
#include "model/CallDeviceResult.h"
#include "model/ConferenceToLiveRequest.h"
#include "model/ConferenceToLiveResult.h"
#include "model/CreateDeviceMeetingRequest.h"
#include "model/CreateDeviceMeetingResult.h"
#include "model/CreateEvaluationRequest.h"
#include "model/CreateEvaluationResult.h"
#include "model/CreateLiveRequest.h"
#include "model/CreateLiveResult.h"
#include "model/CreateMeetingRequest.h"
#include "model/CreateMeetingResult.h"
#include "model/CreateMeetingInternationalRequest.h"
#include "model/CreateMeetingInternationalResult.h"
#include "model/CreateUserRequest.h"
#include "model/CreateUserResult.h"
#include "model/CreateUserInternationalRequest.h"
#include "model/CreateUserInternationalResult.h"
#include "model/CustomGonggeLayoutRequest.h"
#include "model/CustomGonggeLayoutResult.h"
#include "model/CustomLayoutRequest.h"
#include "model/CustomLayoutResult.h"
#include "model/DeleteDeviceRequest.h"
#include "model/DeleteDeviceResult.h"
#include "model/DeleteLiveRequest.h"
#include "model/DeleteLiveResult.h"
#include "model/DeleteMeetingRequest.h"
#include "model/DeleteMeetingResult.h"
#include "model/DeleteUserRequest.h"
#include "model/DeleteUserResult.h"
#include "model/EnableLiveSpeakerRequest.h"
#include "model/EnableLiveSpeakerResult.h"
#include "model/EndDeviceMeetingRequest.h"
#include "model/EndDeviceMeetingResult.h"
#include "model/EndLiveRequest.h"
#include "model/EndLiveResult.h"
#include "model/GetAccountInfoRequest.h"
#include "model/GetAccountInfoResult.h"
#include "model/GetDeviceActiveCodeRequest.h"
#include "model/GetDeviceActiveCodeResult.h"
#include "model/GetDeviceInfoRequest.h"
#include "model/GetDeviceInfoResult.h"
#include "model/GetDeviceTokenRequest.h"
#include "model/GetDeviceTokenResult.h"
#include "model/GetMeetingRequest.h"
#include "model/GetMeetingResult.h"
#include "model/GetMeetingInternationalRequest.h"
#include "model/GetMeetingInternationalResult.h"
#include "model/GetMeetingMemberRequest.h"
#include "model/GetMeetingMemberResult.h"
#include "model/GetMembersRequest.h"
#include "model/GetMembersResult.h"
#include "model/GetStatisticRequest.h"
#include "model/GetStatisticResult.h"
#include "model/GetUserRequest.h"
#include "model/GetUserResult.h"
#include "model/InviteUserRequest.h"
#include "model/InviteUserResult.h"
#include "model/JoinDeviceMeetingRequest.h"
#include "model/JoinDeviceMeetingResult.h"
#include "model/JoinLiveRequest.h"
#include "model/JoinLiveResult.h"
#include "model/JoinMeetingRequest.h"
#include "model/JoinMeetingResult.h"
#include "model/JoinMeetingInternationalRequest.h"
#include "model/JoinMeetingInternationalResult.h"
#include "model/ListConferenceDevicesRequest.h"
#include "model/ListConferenceDevicesResult.h"
#include "model/ListDeviceIpRequest.h"
#include "model/ListDeviceIpResult.h"
#include "model/ListDevicesRequest.h"
#include "model/ListDevicesResult.h"
#include "model/ListEvaluationsRequest.h"
#include "model/ListEvaluationsResult.h"
#include "model/ListIsvStatisticsRequest.h"
#include "model/ListIsvStatisticsResult.h"
#include "model/ListMembersRequest.h"
#include "model/ListMembersResult.h"
#include "model/ListUsersRequest.h"
#include "model/ListUsersResult.h"
#include "model/ModifyDeviceBackgroundRequest.h"
#include "model/ModifyDeviceBackgroundResult.h"
#include "model/ModifyMeetingPasswordRequest.h"
#include "model/ModifyMeetingPasswordResult.h"
#include "model/ModifyMeetingPasswordInternationalRequest.h"
#include "model/ModifyMeetingPasswordInternationalResult.h"
#include "model/RefreshDeviceScreenCodeRequest.h"
#include "model/RefreshDeviceScreenCodeResult.h"
#include "model/RegisterDeviceRequest.h"
#include "model/RegisterDeviceResult.h"
#include "model/RegisterUemDeviceRequest.h"
#include "model/RegisterUemDeviceResult.h"
#include "model/SendMeetingCommandRequest.h"
#include "model/SendMeetingCommandResult.h"
#include "model/StartLiveRequest.h"
#include "model/StartLiveResult.h"
#include "model/UpdateDeviceHeartBeatRequest.h"
#include "model/UpdateDeviceHeartBeatResult.h"
#include "model/UpdateDeviceStartupPictureRequest.h"
#include "model/UpdateDeviceStartupPictureResult.h"
#include "model/UpdateGonggeLayoutRequest.h"
#include "model/UpdateGonggeLayoutResult.h"
#include "model/UpdateLivePasswordRequest.h"
#include "model/UpdateLivePasswordResult.h"


namespace AlibabaCloud
{
	namespace Aliyuncvc
	{
		class ALIBABACLOUD_ALIYUNCVC_EXPORT AliyuncvcClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::ActiveDeviceResult> ActiveDeviceOutcome;
			typedef std::future<ActiveDeviceOutcome> ActiveDeviceOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ActiveDeviceRequest&, const ActiveDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ActiveDeviceAsyncHandler;
			typedef Outcome<Error, Model::ActiveMeetingResult> ActiveMeetingOutcome;
			typedef std::future<ActiveMeetingOutcome> ActiveMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ActiveMeetingRequest&, const ActiveMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ActiveMeetingAsyncHandler;
			typedef Outcome<Error, Model::BatchCreateDeviceResult> BatchCreateDeviceOutcome;
			typedef std::future<BatchCreateDeviceOutcome> BatchCreateDeviceOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::BatchCreateDeviceRequest&, const BatchCreateDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchCreateDeviceAsyncHandler;
			typedef Outcome<Error, Model::BatchDeleteDevicesResult> BatchDeleteDevicesOutcome;
			typedef std::future<BatchDeleteDevicesOutcome> BatchDeleteDevicesOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::BatchDeleteDevicesRequest&, const BatchDeleteDevicesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> BatchDeleteDevicesAsyncHandler;
			typedef Outcome<Error, Model::CallDeviceResult> CallDeviceOutcome;
			typedef std::future<CallDeviceOutcome> CallDeviceOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CallDeviceRequest&, const CallDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CallDeviceAsyncHandler;
			typedef Outcome<Error, Model::ConferenceToLiveResult> ConferenceToLiveOutcome;
			typedef std::future<ConferenceToLiveOutcome> ConferenceToLiveOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ConferenceToLiveRequest&, const ConferenceToLiveOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ConferenceToLiveAsyncHandler;
			typedef Outcome<Error, Model::CreateDeviceMeetingResult> CreateDeviceMeetingOutcome;
			typedef std::future<CreateDeviceMeetingOutcome> CreateDeviceMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CreateDeviceMeetingRequest&, const CreateDeviceMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateDeviceMeetingAsyncHandler;
			typedef Outcome<Error, Model::CreateEvaluationResult> CreateEvaluationOutcome;
			typedef std::future<CreateEvaluationOutcome> CreateEvaluationOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CreateEvaluationRequest&, const CreateEvaluationOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateEvaluationAsyncHandler;
			typedef Outcome<Error, Model::CreateLiveResult> CreateLiveOutcome;
			typedef std::future<CreateLiveOutcome> CreateLiveOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CreateLiveRequest&, const CreateLiveOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateLiveAsyncHandler;
			typedef Outcome<Error, Model::CreateMeetingResult> CreateMeetingOutcome;
			typedef std::future<CreateMeetingOutcome> CreateMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CreateMeetingRequest&, const CreateMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateMeetingAsyncHandler;
			typedef Outcome<Error, Model::CreateMeetingInternationalResult> CreateMeetingInternationalOutcome;
			typedef std::future<CreateMeetingInternationalOutcome> CreateMeetingInternationalOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CreateMeetingInternationalRequest&, const CreateMeetingInternationalOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateMeetingInternationalAsyncHandler;
			typedef Outcome<Error, Model::CreateUserResult> CreateUserOutcome;
			typedef std::future<CreateUserOutcome> CreateUserOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CreateUserRequest&, const CreateUserOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateUserAsyncHandler;
			typedef Outcome<Error, Model::CreateUserInternationalResult> CreateUserInternationalOutcome;
			typedef std::future<CreateUserInternationalOutcome> CreateUserInternationalOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CreateUserInternationalRequest&, const CreateUserInternationalOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CreateUserInternationalAsyncHandler;
			typedef Outcome<Error, Model::CustomGonggeLayoutResult> CustomGonggeLayoutOutcome;
			typedef std::future<CustomGonggeLayoutOutcome> CustomGonggeLayoutOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CustomGonggeLayoutRequest&, const CustomGonggeLayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CustomGonggeLayoutAsyncHandler;
			typedef Outcome<Error, Model::CustomLayoutResult> CustomLayoutOutcome;
			typedef std::future<CustomLayoutOutcome> CustomLayoutOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::CustomLayoutRequest&, const CustomLayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> CustomLayoutAsyncHandler;
			typedef Outcome<Error, Model::DeleteDeviceResult> DeleteDeviceOutcome;
			typedef std::future<DeleteDeviceOutcome> DeleteDeviceOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::DeleteDeviceRequest&, const DeleteDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteDeviceAsyncHandler;
			typedef Outcome<Error, Model::DeleteLiveResult> DeleteLiveOutcome;
			typedef std::future<DeleteLiveOutcome> DeleteLiveOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::DeleteLiveRequest&, const DeleteLiveOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteLiveAsyncHandler;
			typedef Outcome<Error, Model::DeleteMeetingResult> DeleteMeetingOutcome;
			typedef std::future<DeleteMeetingOutcome> DeleteMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::DeleteMeetingRequest&, const DeleteMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteMeetingAsyncHandler;
			typedef Outcome<Error, Model::DeleteUserResult> DeleteUserOutcome;
			typedef std::future<DeleteUserOutcome> DeleteUserOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::DeleteUserRequest&, const DeleteUserOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DeleteUserAsyncHandler;
			typedef Outcome<Error, Model::EnableLiveSpeakerResult> EnableLiveSpeakerOutcome;
			typedef std::future<EnableLiveSpeakerOutcome> EnableLiveSpeakerOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::EnableLiveSpeakerRequest&, const EnableLiveSpeakerOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EnableLiveSpeakerAsyncHandler;
			typedef Outcome<Error, Model::EndDeviceMeetingResult> EndDeviceMeetingOutcome;
			typedef std::future<EndDeviceMeetingOutcome> EndDeviceMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::EndDeviceMeetingRequest&, const EndDeviceMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EndDeviceMeetingAsyncHandler;
			typedef Outcome<Error, Model::EndLiveResult> EndLiveOutcome;
			typedef std::future<EndLiveOutcome> EndLiveOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::EndLiveRequest&, const EndLiveOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> EndLiveAsyncHandler;
			typedef Outcome<Error, Model::GetAccountInfoResult> GetAccountInfoOutcome;
			typedef std::future<GetAccountInfoOutcome> GetAccountInfoOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetAccountInfoRequest&, const GetAccountInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAccountInfoAsyncHandler;
			typedef Outcome<Error, Model::GetDeviceActiveCodeResult> GetDeviceActiveCodeOutcome;
			typedef std::future<GetDeviceActiveCodeOutcome> GetDeviceActiveCodeOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetDeviceActiveCodeRequest&, const GetDeviceActiveCodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDeviceActiveCodeAsyncHandler;
			typedef Outcome<Error, Model::GetDeviceInfoResult> GetDeviceInfoOutcome;
			typedef std::future<GetDeviceInfoOutcome> GetDeviceInfoOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetDeviceInfoRequest&, const GetDeviceInfoOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDeviceInfoAsyncHandler;
			typedef Outcome<Error, Model::GetDeviceTokenResult> GetDeviceTokenOutcome;
			typedef std::future<GetDeviceTokenOutcome> GetDeviceTokenOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetDeviceTokenRequest&, const GetDeviceTokenOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetDeviceTokenAsyncHandler;
			typedef Outcome<Error, Model::GetMeetingResult> GetMeetingOutcome;
			typedef std::future<GetMeetingOutcome> GetMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetMeetingRequest&, const GetMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetMeetingAsyncHandler;
			typedef Outcome<Error, Model::GetMeetingInternationalResult> GetMeetingInternationalOutcome;
			typedef std::future<GetMeetingInternationalOutcome> GetMeetingInternationalOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetMeetingInternationalRequest&, const GetMeetingInternationalOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetMeetingInternationalAsyncHandler;
			typedef Outcome<Error, Model::GetMeetingMemberResult> GetMeetingMemberOutcome;
			typedef std::future<GetMeetingMemberOutcome> GetMeetingMemberOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetMeetingMemberRequest&, const GetMeetingMemberOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetMeetingMemberAsyncHandler;
			typedef Outcome<Error, Model::GetMembersResult> GetMembersOutcome;
			typedef std::future<GetMembersOutcome> GetMembersOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetMembersRequest&, const GetMembersOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetMembersAsyncHandler;
			typedef Outcome<Error, Model::GetStatisticResult> GetStatisticOutcome;
			typedef std::future<GetStatisticOutcome> GetStatisticOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetStatisticRequest&, const GetStatisticOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetStatisticAsyncHandler;
			typedef Outcome<Error, Model::GetUserResult> GetUserOutcome;
			typedef std::future<GetUserOutcome> GetUserOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::GetUserRequest&, const GetUserOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetUserAsyncHandler;
			typedef Outcome<Error, Model::InviteUserResult> InviteUserOutcome;
			typedef std::future<InviteUserOutcome> InviteUserOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::InviteUserRequest&, const InviteUserOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> InviteUserAsyncHandler;
			typedef Outcome<Error, Model::JoinDeviceMeetingResult> JoinDeviceMeetingOutcome;
			typedef std::future<JoinDeviceMeetingOutcome> JoinDeviceMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::JoinDeviceMeetingRequest&, const JoinDeviceMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> JoinDeviceMeetingAsyncHandler;
			typedef Outcome<Error, Model::JoinLiveResult> JoinLiveOutcome;
			typedef std::future<JoinLiveOutcome> JoinLiveOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::JoinLiveRequest&, const JoinLiveOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> JoinLiveAsyncHandler;
			typedef Outcome<Error, Model::JoinMeetingResult> JoinMeetingOutcome;
			typedef std::future<JoinMeetingOutcome> JoinMeetingOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::JoinMeetingRequest&, const JoinMeetingOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> JoinMeetingAsyncHandler;
			typedef Outcome<Error, Model::JoinMeetingInternationalResult> JoinMeetingInternationalOutcome;
			typedef std::future<JoinMeetingInternationalOutcome> JoinMeetingInternationalOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::JoinMeetingInternationalRequest&, const JoinMeetingInternationalOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> JoinMeetingInternationalAsyncHandler;
			typedef Outcome<Error, Model::ListConferenceDevicesResult> ListConferenceDevicesOutcome;
			typedef std::future<ListConferenceDevicesOutcome> ListConferenceDevicesOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ListConferenceDevicesRequest&, const ListConferenceDevicesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListConferenceDevicesAsyncHandler;
			typedef Outcome<Error, Model::ListDeviceIpResult> ListDeviceIpOutcome;
			typedef std::future<ListDeviceIpOutcome> ListDeviceIpOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ListDeviceIpRequest&, const ListDeviceIpOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListDeviceIpAsyncHandler;
			typedef Outcome<Error, Model::ListDevicesResult> ListDevicesOutcome;
			typedef std::future<ListDevicesOutcome> ListDevicesOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ListDevicesRequest&, const ListDevicesOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListDevicesAsyncHandler;
			typedef Outcome<Error, Model::ListEvaluationsResult> ListEvaluationsOutcome;
			typedef std::future<ListEvaluationsOutcome> ListEvaluationsOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ListEvaluationsRequest&, const ListEvaluationsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListEvaluationsAsyncHandler;
			typedef Outcome<Error, Model::ListIsvStatisticsResult> ListIsvStatisticsOutcome;
			typedef std::future<ListIsvStatisticsOutcome> ListIsvStatisticsOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ListIsvStatisticsRequest&, const ListIsvStatisticsOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListIsvStatisticsAsyncHandler;
			typedef Outcome<Error, Model::ListMembersResult> ListMembersOutcome;
			typedef std::future<ListMembersOutcome> ListMembersOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ListMembersRequest&, const ListMembersOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListMembersAsyncHandler;
			typedef Outcome<Error, Model::ListUsersResult> ListUsersOutcome;
			typedef std::future<ListUsersOutcome> ListUsersOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ListUsersRequest&, const ListUsersOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ListUsersAsyncHandler;
			typedef Outcome<Error, Model::ModifyDeviceBackgroundResult> ModifyDeviceBackgroundOutcome;
			typedef std::future<ModifyDeviceBackgroundOutcome> ModifyDeviceBackgroundOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ModifyDeviceBackgroundRequest&, const ModifyDeviceBackgroundOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyDeviceBackgroundAsyncHandler;
			typedef Outcome<Error, Model::ModifyMeetingPasswordResult> ModifyMeetingPasswordOutcome;
			typedef std::future<ModifyMeetingPasswordOutcome> ModifyMeetingPasswordOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ModifyMeetingPasswordRequest&, const ModifyMeetingPasswordOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyMeetingPasswordAsyncHandler;
			typedef Outcome<Error, Model::ModifyMeetingPasswordInternationalResult> ModifyMeetingPasswordInternationalOutcome;
			typedef std::future<ModifyMeetingPasswordInternationalOutcome> ModifyMeetingPasswordInternationalOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::ModifyMeetingPasswordInternationalRequest&, const ModifyMeetingPasswordInternationalOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> ModifyMeetingPasswordInternationalAsyncHandler;
			typedef Outcome<Error, Model::RefreshDeviceScreenCodeResult> RefreshDeviceScreenCodeOutcome;
			typedef std::future<RefreshDeviceScreenCodeOutcome> RefreshDeviceScreenCodeOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::RefreshDeviceScreenCodeRequest&, const RefreshDeviceScreenCodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RefreshDeviceScreenCodeAsyncHandler;
			typedef Outcome<Error, Model::RegisterDeviceResult> RegisterDeviceOutcome;
			typedef std::future<RegisterDeviceOutcome> RegisterDeviceOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::RegisterDeviceRequest&, const RegisterDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterDeviceAsyncHandler;
			typedef Outcome<Error, Model::RegisterUemDeviceResult> RegisterUemDeviceOutcome;
			typedef std::future<RegisterUemDeviceOutcome> RegisterUemDeviceOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::RegisterUemDeviceRequest&, const RegisterUemDeviceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RegisterUemDeviceAsyncHandler;
			typedef Outcome<Error, Model::SendMeetingCommandResult> SendMeetingCommandOutcome;
			typedef std::future<SendMeetingCommandOutcome> SendMeetingCommandOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::SendMeetingCommandRequest&, const SendMeetingCommandOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> SendMeetingCommandAsyncHandler;
			typedef Outcome<Error, Model::StartLiveResult> StartLiveOutcome;
			typedef std::future<StartLiveOutcome> StartLiveOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::StartLiveRequest&, const StartLiveOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> StartLiveAsyncHandler;
			typedef Outcome<Error, Model::UpdateDeviceHeartBeatResult> UpdateDeviceHeartBeatOutcome;
			typedef std::future<UpdateDeviceHeartBeatOutcome> UpdateDeviceHeartBeatOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::UpdateDeviceHeartBeatRequest&, const UpdateDeviceHeartBeatOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateDeviceHeartBeatAsyncHandler;
			typedef Outcome<Error, Model::UpdateDeviceStartupPictureResult> UpdateDeviceStartupPictureOutcome;
			typedef std::future<UpdateDeviceStartupPictureOutcome> UpdateDeviceStartupPictureOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::UpdateDeviceStartupPictureRequest&, const UpdateDeviceStartupPictureOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateDeviceStartupPictureAsyncHandler;
			typedef Outcome<Error, Model::UpdateGonggeLayoutResult> UpdateGonggeLayoutOutcome;
			typedef std::future<UpdateGonggeLayoutOutcome> UpdateGonggeLayoutOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::UpdateGonggeLayoutRequest&, const UpdateGonggeLayoutOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateGonggeLayoutAsyncHandler;
			typedef Outcome<Error, Model::UpdateLivePasswordResult> UpdateLivePasswordOutcome;
			typedef std::future<UpdateLivePasswordOutcome> UpdateLivePasswordOutcomeCallable;
			typedef std::function<void(const AliyuncvcClient*, const Model::UpdateLivePasswordRequest&, const UpdateLivePasswordOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> UpdateLivePasswordAsyncHandler;

			AliyuncvcClient(const Credentials &credentials, const ClientConfiguration &configuration);
			AliyuncvcClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			AliyuncvcClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~AliyuncvcClient();
			ActiveDeviceOutcome activeDevice(const Model::ActiveDeviceRequest &request)const;
			void activeDeviceAsync(const Model::ActiveDeviceRequest& request, const ActiveDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ActiveDeviceOutcomeCallable activeDeviceCallable(const Model::ActiveDeviceRequest& request) const;
			ActiveMeetingOutcome activeMeeting(const Model::ActiveMeetingRequest &request)const;
			void activeMeetingAsync(const Model::ActiveMeetingRequest& request, const ActiveMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ActiveMeetingOutcomeCallable activeMeetingCallable(const Model::ActiveMeetingRequest& request) const;
			BatchCreateDeviceOutcome batchCreateDevice(const Model::BatchCreateDeviceRequest &request)const;
			void batchCreateDeviceAsync(const Model::BatchCreateDeviceRequest& request, const BatchCreateDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchCreateDeviceOutcomeCallable batchCreateDeviceCallable(const Model::BatchCreateDeviceRequest& request) const;
			BatchDeleteDevicesOutcome batchDeleteDevices(const Model::BatchDeleteDevicesRequest &request)const;
			void batchDeleteDevicesAsync(const Model::BatchDeleteDevicesRequest& request, const BatchDeleteDevicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			BatchDeleteDevicesOutcomeCallable batchDeleteDevicesCallable(const Model::BatchDeleteDevicesRequest& request) const;
			CallDeviceOutcome callDevice(const Model::CallDeviceRequest &request)const;
			void callDeviceAsync(const Model::CallDeviceRequest& request, const CallDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CallDeviceOutcomeCallable callDeviceCallable(const Model::CallDeviceRequest& request) const;
			ConferenceToLiveOutcome conferenceToLive(const Model::ConferenceToLiveRequest &request)const;
			void conferenceToLiveAsync(const Model::ConferenceToLiveRequest& request, const ConferenceToLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ConferenceToLiveOutcomeCallable conferenceToLiveCallable(const Model::ConferenceToLiveRequest& request) const;
			CreateDeviceMeetingOutcome createDeviceMeeting(const Model::CreateDeviceMeetingRequest &request)const;
			void createDeviceMeetingAsync(const Model::CreateDeviceMeetingRequest& request, const CreateDeviceMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateDeviceMeetingOutcomeCallable createDeviceMeetingCallable(const Model::CreateDeviceMeetingRequest& request) const;
			CreateEvaluationOutcome createEvaluation(const Model::CreateEvaluationRequest &request)const;
			void createEvaluationAsync(const Model::CreateEvaluationRequest& request, const CreateEvaluationAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateEvaluationOutcomeCallable createEvaluationCallable(const Model::CreateEvaluationRequest& request) const;
			CreateLiveOutcome createLive(const Model::CreateLiveRequest &request)const;
			void createLiveAsync(const Model::CreateLiveRequest& request, const CreateLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateLiveOutcomeCallable createLiveCallable(const Model::CreateLiveRequest& request) const;
			CreateMeetingOutcome createMeeting(const Model::CreateMeetingRequest &request)const;
			void createMeetingAsync(const Model::CreateMeetingRequest& request, const CreateMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateMeetingOutcomeCallable createMeetingCallable(const Model::CreateMeetingRequest& request) const;
			CreateMeetingInternationalOutcome createMeetingInternational(const Model::CreateMeetingInternationalRequest &request)const;
			void createMeetingInternationalAsync(const Model::CreateMeetingInternationalRequest& request, const CreateMeetingInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateMeetingInternationalOutcomeCallable createMeetingInternationalCallable(const Model::CreateMeetingInternationalRequest& request) const;
			CreateUserOutcome createUser(const Model::CreateUserRequest &request)const;
			void createUserAsync(const Model::CreateUserRequest& request, const CreateUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateUserOutcomeCallable createUserCallable(const Model::CreateUserRequest& request) const;
			CreateUserInternationalOutcome createUserInternational(const Model::CreateUserInternationalRequest &request)const;
			void createUserInternationalAsync(const Model::CreateUserInternationalRequest& request, const CreateUserInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CreateUserInternationalOutcomeCallable createUserInternationalCallable(const Model::CreateUserInternationalRequest& request) const;
			CustomGonggeLayoutOutcome customGonggeLayout(const Model::CustomGonggeLayoutRequest &request)const;
			void customGonggeLayoutAsync(const Model::CustomGonggeLayoutRequest& request, const CustomGonggeLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CustomGonggeLayoutOutcomeCallable customGonggeLayoutCallable(const Model::CustomGonggeLayoutRequest& request) const;
			CustomLayoutOutcome customLayout(const Model::CustomLayoutRequest &request)const;
			void customLayoutAsync(const Model::CustomLayoutRequest& request, const CustomLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			CustomLayoutOutcomeCallable customLayoutCallable(const Model::CustomLayoutRequest& request) const;
			DeleteDeviceOutcome deleteDevice(const Model::DeleteDeviceRequest &request)const;
			void deleteDeviceAsync(const Model::DeleteDeviceRequest& request, const DeleteDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteDeviceOutcomeCallable deleteDeviceCallable(const Model::DeleteDeviceRequest& request) const;
			DeleteLiveOutcome deleteLive(const Model::DeleteLiveRequest &request)const;
			void deleteLiveAsync(const Model::DeleteLiveRequest& request, const DeleteLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteLiveOutcomeCallable deleteLiveCallable(const Model::DeleteLiveRequest& request) const;
			DeleteMeetingOutcome deleteMeeting(const Model::DeleteMeetingRequest &request)const;
			void deleteMeetingAsync(const Model::DeleteMeetingRequest& request, const DeleteMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteMeetingOutcomeCallable deleteMeetingCallable(const Model::DeleteMeetingRequest& request) const;
			DeleteUserOutcome deleteUser(const Model::DeleteUserRequest &request)const;
			void deleteUserAsync(const Model::DeleteUserRequest& request, const DeleteUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DeleteUserOutcomeCallable deleteUserCallable(const Model::DeleteUserRequest& request) const;
			EnableLiveSpeakerOutcome enableLiveSpeaker(const Model::EnableLiveSpeakerRequest &request)const;
			void enableLiveSpeakerAsync(const Model::EnableLiveSpeakerRequest& request, const EnableLiveSpeakerAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EnableLiveSpeakerOutcomeCallable enableLiveSpeakerCallable(const Model::EnableLiveSpeakerRequest& request) const;
			EndDeviceMeetingOutcome endDeviceMeeting(const Model::EndDeviceMeetingRequest &request)const;
			void endDeviceMeetingAsync(const Model::EndDeviceMeetingRequest& request, const EndDeviceMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EndDeviceMeetingOutcomeCallable endDeviceMeetingCallable(const Model::EndDeviceMeetingRequest& request) const;
			EndLiveOutcome endLive(const Model::EndLiveRequest &request)const;
			void endLiveAsync(const Model::EndLiveRequest& request, const EndLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			EndLiveOutcomeCallable endLiveCallable(const Model::EndLiveRequest& request) const;
			GetAccountInfoOutcome getAccountInfo(const Model::GetAccountInfoRequest &request)const;
			void getAccountInfoAsync(const Model::GetAccountInfoRequest& request, const GetAccountInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAccountInfoOutcomeCallable getAccountInfoCallable(const Model::GetAccountInfoRequest& request) const;
			GetDeviceActiveCodeOutcome getDeviceActiveCode(const Model::GetDeviceActiveCodeRequest &request)const;
			void getDeviceActiveCodeAsync(const Model::GetDeviceActiveCodeRequest& request, const GetDeviceActiveCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDeviceActiveCodeOutcomeCallable getDeviceActiveCodeCallable(const Model::GetDeviceActiveCodeRequest& request) const;
			GetDeviceInfoOutcome getDeviceInfo(const Model::GetDeviceInfoRequest &request)const;
			void getDeviceInfoAsync(const Model::GetDeviceInfoRequest& request, const GetDeviceInfoAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDeviceInfoOutcomeCallable getDeviceInfoCallable(const Model::GetDeviceInfoRequest& request) const;
			GetDeviceTokenOutcome getDeviceToken(const Model::GetDeviceTokenRequest &request)const;
			void getDeviceTokenAsync(const Model::GetDeviceTokenRequest& request, const GetDeviceTokenAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetDeviceTokenOutcomeCallable getDeviceTokenCallable(const Model::GetDeviceTokenRequest& request) const;
			GetMeetingOutcome getMeeting(const Model::GetMeetingRequest &request)const;
			void getMeetingAsync(const Model::GetMeetingRequest& request, const GetMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetMeetingOutcomeCallable getMeetingCallable(const Model::GetMeetingRequest& request) const;
			GetMeetingInternationalOutcome getMeetingInternational(const Model::GetMeetingInternationalRequest &request)const;
			void getMeetingInternationalAsync(const Model::GetMeetingInternationalRequest& request, const GetMeetingInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetMeetingInternationalOutcomeCallable getMeetingInternationalCallable(const Model::GetMeetingInternationalRequest& request) const;
			GetMeetingMemberOutcome getMeetingMember(const Model::GetMeetingMemberRequest &request)const;
			void getMeetingMemberAsync(const Model::GetMeetingMemberRequest& request, const GetMeetingMemberAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetMeetingMemberOutcomeCallable getMeetingMemberCallable(const Model::GetMeetingMemberRequest& request) const;
			GetMembersOutcome getMembers(const Model::GetMembersRequest &request)const;
			void getMembersAsync(const Model::GetMembersRequest& request, const GetMembersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetMembersOutcomeCallable getMembersCallable(const Model::GetMembersRequest& request) const;
			GetStatisticOutcome getStatistic(const Model::GetStatisticRequest &request)const;
			void getStatisticAsync(const Model::GetStatisticRequest& request, const GetStatisticAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetStatisticOutcomeCallable getStatisticCallable(const Model::GetStatisticRequest& request) const;
			GetUserOutcome getUser(const Model::GetUserRequest &request)const;
			void getUserAsync(const Model::GetUserRequest& request, const GetUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetUserOutcomeCallable getUserCallable(const Model::GetUserRequest& request) const;
			InviteUserOutcome inviteUser(const Model::InviteUserRequest &request)const;
			void inviteUserAsync(const Model::InviteUserRequest& request, const InviteUserAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			InviteUserOutcomeCallable inviteUserCallable(const Model::InviteUserRequest& request) const;
			JoinDeviceMeetingOutcome joinDeviceMeeting(const Model::JoinDeviceMeetingRequest &request)const;
			void joinDeviceMeetingAsync(const Model::JoinDeviceMeetingRequest& request, const JoinDeviceMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			JoinDeviceMeetingOutcomeCallable joinDeviceMeetingCallable(const Model::JoinDeviceMeetingRequest& request) const;
			JoinLiveOutcome joinLive(const Model::JoinLiveRequest &request)const;
			void joinLiveAsync(const Model::JoinLiveRequest& request, const JoinLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			JoinLiveOutcomeCallable joinLiveCallable(const Model::JoinLiveRequest& request) const;
			JoinMeetingOutcome joinMeeting(const Model::JoinMeetingRequest &request)const;
			void joinMeetingAsync(const Model::JoinMeetingRequest& request, const JoinMeetingAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			JoinMeetingOutcomeCallable joinMeetingCallable(const Model::JoinMeetingRequest& request) const;
			JoinMeetingInternationalOutcome joinMeetingInternational(const Model::JoinMeetingInternationalRequest &request)const;
			void joinMeetingInternationalAsync(const Model::JoinMeetingInternationalRequest& request, const JoinMeetingInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			JoinMeetingInternationalOutcomeCallable joinMeetingInternationalCallable(const Model::JoinMeetingInternationalRequest& request) const;
			ListConferenceDevicesOutcome listConferenceDevices(const Model::ListConferenceDevicesRequest &request)const;
			void listConferenceDevicesAsync(const Model::ListConferenceDevicesRequest& request, const ListConferenceDevicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListConferenceDevicesOutcomeCallable listConferenceDevicesCallable(const Model::ListConferenceDevicesRequest& request) const;
			ListDeviceIpOutcome listDeviceIp(const Model::ListDeviceIpRequest &request)const;
			void listDeviceIpAsync(const Model::ListDeviceIpRequest& request, const ListDeviceIpAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListDeviceIpOutcomeCallable listDeviceIpCallable(const Model::ListDeviceIpRequest& request) const;
			ListDevicesOutcome listDevices(const Model::ListDevicesRequest &request)const;
			void listDevicesAsync(const Model::ListDevicesRequest& request, const ListDevicesAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListDevicesOutcomeCallable listDevicesCallable(const Model::ListDevicesRequest& request) const;
			ListEvaluationsOutcome listEvaluations(const Model::ListEvaluationsRequest &request)const;
			void listEvaluationsAsync(const Model::ListEvaluationsRequest& request, const ListEvaluationsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListEvaluationsOutcomeCallable listEvaluationsCallable(const Model::ListEvaluationsRequest& request) const;
			ListIsvStatisticsOutcome listIsvStatistics(const Model::ListIsvStatisticsRequest &request)const;
			void listIsvStatisticsAsync(const Model::ListIsvStatisticsRequest& request, const ListIsvStatisticsAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListIsvStatisticsOutcomeCallable listIsvStatisticsCallable(const Model::ListIsvStatisticsRequest& request) const;
			ListMembersOutcome listMembers(const Model::ListMembersRequest &request)const;
			void listMembersAsync(const Model::ListMembersRequest& request, const ListMembersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListMembersOutcomeCallable listMembersCallable(const Model::ListMembersRequest& request) const;
			ListUsersOutcome listUsers(const Model::ListUsersRequest &request)const;
			void listUsersAsync(const Model::ListUsersRequest& request, const ListUsersAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ListUsersOutcomeCallable listUsersCallable(const Model::ListUsersRequest& request) const;
			ModifyDeviceBackgroundOutcome modifyDeviceBackground(const Model::ModifyDeviceBackgroundRequest &request)const;
			void modifyDeviceBackgroundAsync(const Model::ModifyDeviceBackgroundRequest& request, const ModifyDeviceBackgroundAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyDeviceBackgroundOutcomeCallable modifyDeviceBackgroundCallable(const Model::ModifyDeviceBackgroundRequest& request) const;
			ModifyMeetingPasswordOutcome modifyMeetingPassword(const Model::ModifyMeetingPasswordRequest &request)const;
			void modifyMeetingPasswordAsync(const Model::ModifyMeetingPasswordRequest& request, const ModifyMeetingPasswordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyMeetingPasswordOutcomeCallable modifyMeetingPasswordCallable(const Model::ModifyMeetingPasswordRequest& request) const;
			ModifyMeetingPasswordInternationalOutcome modifyMeetingPasswordInternational(const Model::ModifyMeetingPasswordInternationalRequest &request)const;
			void modifyMeetingPasswordInternationalAsync(const Model::ModifyMeetingPasswordInternationalRequest& request, const ModifyMeetingPasswordInternationalAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			ModifyMeetingPasswordInternationalOutcomeCallable modifyMeetingPasswordInternationalCallable(const Model::ModifyMeetingPasswordInternationalRequest& request) const;
			RefreshDeviceScreenCodeOutcome refreshDeviceScreenCode(const Model::RefreshDeviceScreenCodeRequest &request)const;
			void refreshDeviceScreenCodeAsync(const Model::RefreshDeviceScreenCodeRequest& request, const RefreshDeviceScreenCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RefreshDeviceScreenCodeOutcomeCallable refreshDeviceScreenCodeCallable(const Model::RefreshDeviceScreenCodeRequest& request) const;
			RegisterDeviceOutcome registerDevice(const Model::RegisterDeviceRequest &request)const;
			void registerDeviceAsync(const Model::RegisterDeviceRequest& request, const RegisterDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterDeviceOutcomeCallable registerDeviceCallable(const Model::RegisterDeviceRequest& request) const;
			RegisterUemDeviceOutcome registerUemDevice(const Model::RegisterUemDeviceRequest &request)const;
			void registerUemDeviceAsync(const Model::RegisterUemDeviceRequest& request, const RegisterUemDeviceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RegisterUemDeviceOutcomeCallable registerUemDeviceCallable(const Model::RegisterUemDeviceRequest& request) const;
			SendMeetingCommandOutcome sendMeetingCommand(const Model::SendMeetingCommandRequest &request)const;
			void sendMeetingCommandAsync(const Model::SendMeetingCommandRequest& request, const SendMeetingCommandAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			SendMeetingCommandOutcomeCallable sendMeetingCommandCallable(const Model::SendMeetingCommandRequest& request) const;
			StartLiveOutcome startLive(const Model::StartLiveRequest &request)const;
			void startLiveAsync(const Model::StartLiveRequest& request, const StartLiveAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			StartLiveOutcomeCallable startLiveCallable(const Model::StartLiveRequest& request) const;
			UpdateDeviceHeartBeatOutcome updateDeviceHeartBeat(const Model::UpdateDeviceHeartBeatRequest &request)const;
			void updateDeviceHeartBeatAsync(const Model::UpdateDeviceHeartBeatRequest& request, const UpdateDeviceHeartBeatAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateDeviceHeartBeatOutcomeCallable updateDeviceHeartBeatCallable(const Model::UpdateDeviceHeartBeatRequest& request) const;
			UpdateDeviceStartupPictureOutcome updateDeviceStartupPicture(const Model::UpdateDeviceStartupPictureRequest &request)const;
			void updateDeviceStartupPictureAsync(const Model::UpdateDeviceStartupPictureRequest& request, const UpdateDeviceStartupPictureAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateDeviceStartupPictureOutcomeCallable updateDeviceStartupPictureCallable(const Model::UpdateDeviceStartupPictureRequest& request) const;
			UpdateGonggeLayoutOutcome updateGonggeLayout(const Model::UpdateGonggeLayoutRequest &request)const;
			void updateGonggeLayoutAsync(const Model::UpdateGonggeLayoutRequest& request, const UpdateGonggeLayoutAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateGonggeLayoutOutcomeCallable updateGonggeLayoutCallable(const Model::UpdateGonggeLayoutRequest& request) const;
			UpdateLivePasswordOutcome updateLivePassword(const Model::UpdateLivePasswordRequest &request)const;
			void updateLivePasswordAsync(const Model::UpdateLivePasswordRequest& request, const UpdateLivePasswordAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			UpdateLivePasswordOutcomeCallable updateLivePasswordCallable(const Model::UpdateLivePasswordRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_ALIYUNCVC_ALIYUNCVCCLIENT_H_

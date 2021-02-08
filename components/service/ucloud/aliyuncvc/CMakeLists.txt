#
# Copyright 2009-2017 Alibaba Cloud All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set(public_header_dir ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(aliyuncvc_public_header 
	include/alibabacloud/aliyuncvc/AliyuncvcClient.h
	include/alibabacloud/aliyuncvc/AliyuncvcExport.h )

set(aliyuncvc_public_header_model 
	include/alibabacloud/aliyuncvc/model/ActiveDeviceRequest.h
	include/alibabacloud/aliyuncvc/model/ActiveDeviceResult.h
	include/alibabacloud/aliyuncvc/model/ActiveMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/ActiveMeetingResult.h
	include/alibabacloud/aliyuncvc/model/BatchCreateDeviceRequest.h
	include/alibabacloud/aliyuncvc/model/BatchCreateDeviceResult.h
	include/alibabacloud/aliyuncvc/model/BatchDeleteDevicesRequest.h
	include/alibabacloud/aliyuncvc/model/BatchDeleteDevicesResult.h
	include/alibabacloud/aliyuncvc/model/CallDeviceRequest.h
	include/alibabacloud/aliyuncvc/model/CallDeviceResult.h
	include/alibabacloud/aliyuncvc/model/ConferenceToLiveRequest.h
	include/alibabacloud/aliyuncvc/model/ConferenceToLiveResult.h
	include/alibabacloud/aliyuncvc/model/CreateDeviceMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/CreateDeviceMeetingResult.h
	include/alibabacloud/aliyuncvc/model/CreateEvaluationRequest.h
	include/alibabacloud/aliyuncvc/model/CreateEvaluationResult.h
	include/alibabacloud/aliyuncvc/model/CreateLiveRequest.h
	include/alibabacloud/aliyuncvc/model/CreateLiveResult.h
	include/alibabacloud/aliyuncvc/model/CreateMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/CreateMeetingResult.h
	include/alibabacloud/aliyuncvc/model/CreateMeetingInternationalRequest.h
	include/alibabacloud/aliyuncvc/model/CreateMeetingInternationalResult.h
	include/alibabacloud/aliyuncvc/model/CreateUserRequest.h
	include/alibabacloud/aliyuncvc/model/CreateUserResult.h
	include/alibabacloud/aliyuncvc/model/CreateUserInternationalRequest.h
	include/alibabacloud/aliyuncvc/model/CreateUserInternationalResult.h
	include/alibabacloud/aliyuncvc/model/CustomGonggeLayoutRequest.h
	include/alibabacloud/aliyuncvc/model/CustomGonggeLayoutResult.h
	include/alibabacloud/aliyuncvc/model/CustomLayoutRequest.h
	include/alibabacloud/aliyuncvc/model/CustomLayoutResult.h
	include/alibabacloud/aliyuncvc/model/DeleteDeviceRequest.h
	include/alibabacloud/aliyuncvc/model/DeleteDeviceResult.h
	include/alibabacloud/aliyuncvc/model/DeleteLiveRequest.h
	include/alibabacloud/aliyuncvc/model/DeleteLiveResult.h
	include/alibabacloud/aliyuncvc/model/DeleteMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/DeleteMeetingResult.h
	include/alibabacloud/aliyuncvc/model/DeleteUserRequest.h
	include/alibabacloud/aliyuncvc/model/DeleteUserResult.h
	include/alibabacloud/aliyuncvc/model/EnableLiveSpeakerRequest.h
	include/alibabacloud/aliyuncvc/model/EnableLiveSpeakerResult.h
	include/alibabacloud/aliyuncvc/model/EndDeviceMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/EndDeviceMeetingResult.h
	include/alibabacloud/aliyuncvc/model/EndLiveRequest.h
	include/alibabacloud/aliyuncvc/model/EndLiveResult.h
	include/alibabacloud/aliyuncvc/model/GetAccountInfoRequest.h
	include/alibabacloud/aliyuncvc/model/GetAccountInfoResult.h
	include/alibabacloud/aliyuncvc/model/GetDeviceActiveCodeRequest.h
	include/alibabacloud/aliyuncvc/model/GetDeviceActiveCodeResult.h
	include/alibabacloud/aliyuncvc/model/GetDeviceInfoRequest.h
	include/alibabacloud/aliyuncvc/model/GetDeviceInfoResult.h
	include/alibabacloud/aliyuncvc/model/GetDeviceTokenRequest.h
	include/alibabacloud/aliyuncvc/model/GetDeviceTokenResult.h
	include/alibabacloud/aliyuncvc/model/GetMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/GetMeetingResult.h
	include/alibabacloud/aliyuncvc/model/GetMeetingInternationalRequest.h
	include/alibabacloud/aliyuncvc/model/GetMeetingInternationalResult.h
	include/alibabacloud/aliyuncvc/model/GetMeetingMemberRequest.h
	include/alibabacloud/aliyuncvc/model/GetMeetingMemberResult.h
	include/alibabacloud/aliyuncvc/model/GetMembersRequest.h
	include/alibabacloud/aliyuncvc/model/GetMembersResult.h
	include/alibabacloud/aliyuncvc/model/GetStatisticRequest.h
	include/alibabacloud/aliyuncvc/model/GetStatisticResult.h
	include/alibabacloud/aliyuncvc/model/GetUserRequest.h
	include/alibabacloud/aliyuncvc/model/GetUserResult.h
	include/alibabacloud/aliyuncvc/model/InviteUserRequest.h
	include/alibabacloud/aliyuncvc/model/InviteUserResult.h
	include/alibabacloud/aliyuncvc/model/JoinDeviceMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/JoinDeviceMeetingResult.h
	include/alibabacloud/aliyuncvc/model/JoinLiveRequest.h
	include/alibabacloud/aliyuncvc/model/JoinLiveResult.h
	include/alibabacloud/aliyuncvc/model/JoinMeetingRequest.h
	include/alibabacloud/aliyuncvc/model/JoinMeetingResult.h
	include/alibabacloud/aliyuncvc/model/JoinMeetingInternationalRequest.h
	include/alibabacloud/aliyuncvc/model/JoinMeetingInternationalResult.h
	include/alibabacloud/aliyuncvc/model/ListConferenceDevicesRequest.h
	include/alibabacloud/aliyuncvc/model/ListConferenceDevicesResult.h
	include/alibabacloud/aliyuncvc/model/ListDeviceIpRequest.h
	include/alibabacloud/aliyuncvc/model/ListDeviceIpResult.h
	include/alibabacloud/aliyuncvc/model/ListDevicesRequest.h
	include/alibabacloud/aliyuncvc/model/ListDevicesResult.h
	include/alibabacloud/aliyuncvc/model/ListEvaluationsRequest.h
	include/alibabacloud/aliyuncvc/model/ListEvaluationsResult.h
	include/alibabacloud/aliyuncvc/model/ListIsvStatisticsRequest.h
	include/alibabacloud/aliyuncvc/model/ListIsvStatisticsResult.h
	include/alibabacloud/aliyuncvc/model/ListMembersRequest.h
	include/alibabacloud/aliyuncvc/model/ListMembersResult.h
	include/alibabacloud/aliyuncvc/model/ListUsersRequest.h
	include/alibabacloud/aliyuncvc/model/ListUsersResult.h
	include/alibabacloud/aliyuncvc/model/ModifyDeviceBackgroundRequest.h
	include/alibabacloud/aliyuncvc/model/ModifyDeviceBackgroundResult.h
	include/alibabacloud/aliyuncvc/model/ModifyMeetingPasswordRequest.h
	include/alibabacloud/aliyuncvc/model/ModifyMeetingPasswordResult.h
	include/alibabacloud/aliyuncvc/model/ModifyMeetingPasswordInternationalRequest.h
	include/alibabacloud/aliyuncvc/model/ModifyMeetingPasswordInternationalResult.h
	include/alibabacloud/aliyuncvc/model/RefreshDeviceScreenCodeRequest.h
	include/alibabacloud/aliyuncvc/model/RefreshDeviceScreenCodeResult.h
	include/alibabacloud/aliyuncvc/model/RegisterDeviceRequest.h
	include/alibabacloud/aliyuncvc/model/RegisterDeviceResult.h
	include/alibabacloud/aliyuncvc/model/RegisterUemDeviceRequest.h
	include/alibabacloud/aliyuncvc/model/RegisterUemDeviceResult.h
	include/alibabacloud/aliyuncvc/model/SendMeetingCommandRequest.h
	include/alibabacloud/aliyuncvc/model/SendMeetingCommandResult.h
	include/alibabacloud/aliyuncvc/model/StartLiveRequest.h
	include/alibabacloud/aliyuncvc/model/StartLiveResult.h
	include/alibabacloud/aliyuncvc/model/UpdateDeviceHeartBeatRequest.h
	include/alibabacloud/aliyuncvc/model/UpdateDeviceHeartBeatResult.h
	include/alibabacloud/aliyuncvc/model/UpdateDeviceStartupPictureRequest.h
	include/alibabacloud/aliyuncvc/model/UpdateDeviceStartupPictureResult.h
	include/alibabacloud/aliyuncvc/model/UpdateGonggeLayoutRequest.h
	include/alibabacloud/aliyuncvc/model/UpdateGonggeLayoutResult.h
	include/alibabacloud/aliyuncvc/model/UpdateLivePasswordRequest.h
	include/alibabacloud/aliyuncvc/model/UpdateLivePasswordResult.h )

set(aliyuncvc_src 
	src/AliyuncvcClient.cc
	src/model/ActiveDeviceRequest.cc
	src/model/ActiveDeviceResult.cc
	src/model/ActiveMeetingRequest.cc
	src/model/ActiveMeetingResult.cc
	src/model/BatchCreateDeviceRequest.cc
	src/model/BatchCreateDeviceResult.cc
	src/model/BatchDeleteDevicesRequest.cc
	src/model/BatchDeleteDevicesResult.cc
	src/model/CallDeviceRequest.cc
	src/model/CallDeviceResult.cc
	src/model/ConferenceToLiveRequest.cc
	src/model/ConferenceToLiveResult.cc
	src/model/CreateDeviceMeetingRequest.cc
	src/model/CreateDeviceMeetingResult.cc
	src/model/CreateEvaluationRequest.cc
	src/model/CreateEvaluationResult.cc
	src/model/CreateLiveRequest.cc
	src/model/CreateLiveResult.cc
	src/model/CreateMeetingRequest.cc
	src/model/CreateMeetingResult.cc
	src/model/CreateMeetingInternationalRequest.cc
	src/model/CreateMeetingInternationalResult.cc
	src/model/CreateUserRequest.cc
	src/model/CreateUserResult.cc
	src/model/CreateUserInternationalRequest.cc
	src/model/CreateUserInternationalResult.cc
	src/model/CustomGonggeLayoutRequest.cc
	src/model/CustomGonggeLayoutResult.cc
	src/model/CustomLayoutRequest.cc
	src/model/CustomLayoutResult.cc
	src/model/DeleteDeviceRequest.cc
	src/model/DeleteDeviceResult.cc
	src/model/DeleteLiveRequest.cc
	src/model/DeleteLiveResult.cc
	src/model/DeleteMeetingRequest.cc
	src/model/DeleteMeetingResult.cc
	src/model/DeleteUserRequest.cc
	src/model/DeleteUserResult.cc
	src/model/EnableLiveSpeakerRequest.cc
	src/model/EnableLiveSpeakerResult.cc
	src/model/EndDeviceMeetingRequest.cc
	src/model/EndDeviceMeetingResult.cc
	src/model/EndLiveRequest.cc
	src/model/EndLiveResult.cc
	src/model/GetAccountInfoRequest.cc
	src/model/GetAccountInfoResult.cc
	src/model/GetDeviceActiveCodeRequest.cc
	src/model/GetDeviceActiveCodeResult.cc
	src/model/GetDeviceInfoRequest.cc
	src/model/GetDeviceInfoResult.cc
	src/model/GetDeviceTokenRequest.cc
	src/model/GetDeviceTokenResult.cc
	src/model/GetMeetingRequest.cc
	src/model/GetMeetingResult.cc
	src/model/GetMeetingInternationalRequest.cc
	src/model/GetMeetingInternationalResult.cc
	src/model/GetMeetingMemberRequest.cc
	src/model/GetMeetingMemberResult.cc
	src/model/GetMembersRequest.cc
	src/model/GetMembersResult.cc
	src/model/GetStatisticRequest.cc
	src/model/GetStatisticResult.cc
	src/model/GetUserRequest.cc
	src/model/GetUserResult.cc
	src/model/InviteUserRequest.cc
	src/model/InviteUserResult.cc
	src/model/JoinDeviceMeetingRequest.cc
	src/model/JoinDeviceMeetingResult.cc
	src/model/JoinLiveRequest.cc
	src/model/JoinLiveResult.cc
	src/model/JoinMeetingRequest.cc
	src/model/JoinMeetingResult.cc
	src/model/JoinMeetingInternationalRequest.cc
	src/model/JoinMeetingInternationalResult.cc
	src/model/ListConferenceDevicesRequest.cc
	src/model/ListConferenceDevicesResult.cc
	src/model/ListDeviceIpRequest.cc
	src/model/ListDeviceIpResult.cc
	src/model/ListDevicesRequest.cc
	src/model/ListDevicesResult.cc
	src/model/ListEvaluationsRequest.cc
	src/model/ListEvaluationsResult.cc
	src/model/ListIsvStatisticsRequest.cc
	src/model/ListIsvStatisticsResult.cc
	src/model/ListMembersRequest.cc
	src/model/ListMembersResult.cc
	src/model/ListUsersRequest.cc
	src/model/ListUsersResult.cc
	src/model/ModifyDeviceBackgroundRequest.cc
	src/model/ModifyDeviceBackgroundResult.cc
	src/model/ModifyMeetingPasswordRequest.cc
	src/model/ModifyMeetingPasswordResult.cc
	src/model/ModifyMeetingPasswordInternationalRequest.cc
	src/model/ModifyMeetingPasswordInternationalResult.cc
	src/model/RefreshDeviceScreenCodeRequest.cc
	src/model/RefreshDeviceScreenCodeResult.cc
	src/model/RegisterDeviceRequest.cc
	src/model/RegisterDeviceResult.cc
	src/model/RegisterUemDeviceRequest.cc
	src/model/RegisterUemDeviceResult.cc
	src/model/SendMeetingCommandRequest.cc
	src/model/SendMeetingCommandResult.cc
	src/model/StartLiveRequest.cc
	src/model/StartLiveResult.cc
	src/model/UpdateDeviceHeartBeatRequest.cc
	src/model/UpdateDeviceHeartBeatResult.cc
	src/model/UpdateDeviceStartupPictureRequest.cc
	src/model/UpdateDeviceStartupPictureResult.cc
	src/model/UpdateGonggeLayoutRequest.cc
	src/model/UpdateGonggeLayoutResult.cc
	src/model/UpdateLivePasswordRequest.cc
	src/model/UpdateLivePasswordResult.cc )

add_library(aliyuncvc ${LIB_TYPE}
	${aliyuncvc_public_header}
	${aliyuncvc_public_header_model}
	${aliyuncvc_src})

set_target_properties(aliyuncvc
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}aliyuncvc
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(aliyuncvc
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_ALIYUNCVC_LIBRARY)
endif()

target_include_directories(aliyuncvc
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(aliyuncvc
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(aliyuncvc
		jsoncpp)
	target_include_directories(aliyuncvc
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(aliyuncvc
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(aliyuncvc
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(aliyuncvc
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(aliyuncvc
		jsoncpp)
endif()

install(FILES ${aliyuncvc_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/aliyuncvc)
install(FILES ${aliyuncvc_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/aliyuncvc/model)
install(TARGETS aliyuncvc
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
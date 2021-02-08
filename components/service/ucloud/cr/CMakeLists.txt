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

set(cr_public_header 
	include/alibabacloud/cr/CrClient.h
	include/alibabacloud/cr/CrExport.h )

set(cr_public_header_model 
	include/alibabacloud/cr/model/CancelRepoBuildRequest.h
	include/alibabacloud/cr/model/CancelRepoBuildResult.h
	include/alibabacloud/cr/model/CreateCollectionRequest.h
	include/alibabacloud/cr/model/CreateCollectionResult.h
	include/alibabacloud/cr/model/CreateNamespaceRequest.h
	include/alibabacloud/cr/model/CreateNamespaceResult.h
	include/alibabacloud/cr/model/CreateNamespaceAuthorizationRequest.h
	include/alibabacloud/cr/model/CreateNamespaceAuthorizationResult.h
	include/alibabacloud/cr/model/CreateRepoRequest.h
	include/alibabacloud/cr/model/CreateRepoResult.h
	include/alibabacloud/cr/model/CreateRepoAuthorizationRequest.h
	include/alibabacloud/cr/model/CreateRepoAuthorizationResult.h
	include/alibabacloud/cr/model/CreateRepoBuildRuleRequest.h
	include/alibabacloud/cr/model/CreateRepoBuildRuleResult.h
	include/alibabacloud/cr/model/CreateRepoSyncTaskRequest.h
	include/alibabacloud/cr/model/CreateRepoSyncTaskResult.h
	include/alibabacloud/cr/model/CreateRepoWebhookRequest.h
	include/alibabacloud/cr/model/CreateRepoWebhookResult.h
	include/alibabacloud/cr/model/CreateUserInfoRequest.h
	include/alibabacloud/cr/model/CreateUserInfoResult.h
	include/alibabacloud/cr/model/CreateUserSourceAccountRequest.h
	include/alibabacloud/cr/model/CreateUserSourceAccountResult.h
	include/alibabacloud/cr/model/DeleteCollectionRequest.h
	include/alibabacloud/cr/model/DeleteCollectionResult.h
	include/alibabacloud/cr/model/DeleteImageRequest.h
	include/alibabacloud/cr/model/DeleteImageResult.h
	include/alibabacloud/cr/model/DeleteNamespaceRequest.h
	include/alibabacloud/cr/model/DeleteNamespaceResult.h
	include/alibabacloud/cr/model/DeleteNamespaceAuthorizationRequest.h
	include/alibabacloud/cr/model/DeleteNamespaceAuthorizationResult.h
	include/alibabacloud/cr/model/DeleteRepoRequest.h
	include/alibabacloud/cr/model/DeleteRepoResult.h
	include/alibabacloud/cr/model/DeleteRepoAuthorizationRequest.h
	include/alibabacloud/cr/model/DeleteRepoAuthorizationResult.h
	include/alibabacloud/cr/model/DeleteRepoBuildRuleRequest.h
	include/alibabacloud/cr/model/DeleteRepoBuildRuleResult.h
	include/alibabacloud/cr/model/DeleteRepoWebhookRequest.h
	include/alibabacloud/cr/model/DeleteRepoWebhookResult.h
	include/alibabacloud/cr/model/DeleteUserSourceAccountRequest.h
	include/alibabacloud/cr/model/DeleteUserSourceAccountResult.h
	include/alibabacloud/cr/model/GetAuthorizationTokenRequest.h
	include/alibabacloud/cr/model/GetAuthorizationTokenResult.h
	include/alibabacloud/cr/model/GetCollectionRequest.h
	include/alibabacloud/cr/model/GetCollectionResult.h
	include/alibabacloud/cr/model/GetImageLayerRequest.h
	include/alibabacloud/cr/model/GetImageLayerResult.h
	include/alibabacloud/cr/model/GetImageManifestRequest.h
	include/alibabacloud/cr/model/GetImageManifestResult.h
	include/alibabacloud/cr/model/GetImageScanRequest.h
	include/alibabacloud/cr/model/GetImageScanResult.h
	include/alibabacloud/cr/model/GetMirrorListRequest.h
	include/alibabacloud/cr/model/GetMirrorListResult.h
	include/alibabacloud/cr/model/GetNamespaceRequest.h
	include/alibabacloud/cr/model/GetNamespaceResult.h
	include/alibabacloud/cr/model/GetNamespaceAuthorizationListRequest.h
	include/alibabacloud/cr/model/GetNamespaceAuthorizationListResult.h
	include/alibabacloud/cr/model/GetNamespaceListRequest.h
	include/alibabacloud/cr/model/GetNamespaceListResult.h
	include/alibabacloud/cr/model/GetRegionRequest.h
	include/alibabacloud/cr/model/GetRegionResult.h
	include/alibabacloud/cr/model/GetRegionListRequest.h
	include/alibabacloud/cr/model/GetRegionListResult.h
	include/alibabacloud/cr/model/GetRepoRequest.h
	include/alibabacloud/cr/model/GetRepoResult.h
	include/alibabacloud/cr/model/GetRepoAuthorizationListRequest.h
	include/alibabacloud/cr/model/GetRepoAuthorizationListResult.h
	include/alibabacloud/cr/model/GetRepoBatchRequest.h
	include/alibabacloud/cr/model/GetRepoBatchResult.h
	include/alibabacloud/cr/model/GetRepoBuildListRequest.h
	include/alibabacloud/cr/model/GetRepoBuildListResult.h
	include/alibabacloud/cr/model/GetRepoBuildLogsRequest.h
	include/alibabacloud/cr/model/GetRepoBuildLogsResult.h
	include/alibabacloud/cr/model/GetRepoBuildRuleListRequest.h
	include/alibabacloud/cr/model/GetRepoBuildRuleListResult.h
	include/alibabacloud/cr/model/GetRepoBuildStatusRequest.h
	include/alibabacloud/cr/model/GetRepoBuildStatusResult.h
	include/alibabacloud/cr/model/GetRepoListRequest.h
	include/alibabacloud/cr/model/GetRepoListResult.h
	include/alibabacloud/cr/model/GetRepoListByNamespaceRequest.h
	include/alibabacloud/cr/model/GetRepoListByNamespaceResult.h
	include/alibabacloud/cr/model/GetRepoSourceRepoRequest.h
	include/alibabacloud/cr/model/GetRepoSourceRepoResult.h
	include/alibabacloud/cr/model/GetRepoSyncTaskRequest.h
	include/alibabacloud/cr/model/GetRepoSyncTaskResult.h
	include/alibabacloud/cr/model/GetRepoSyncTaskListRequest.h
	include/alibabacloud/cr/model/GetRepoSyncTaskListResult.h
	include/alibabacloud/cr/model/GetRepoTagsRequest.h
	include/alibabacloud/cr/model/GetRepoTagsResult.h
	include/alibabacloud/cr/model/GetRepoWebhookRequest.h
	include/alibabacloud/cr/model/GetRepoWebhookResult.h
	include/alibabacloud/cr/model/GetRepoWebhookLogListRequest.h
	include/alibabacloud/cr/model/GetRepoWebhookLogListResult.h
	include/alibabacloud/cr/model/GetSearchRequest.h
	include/alibabacloud/cr/model/GetSearchResult.h
	include/alibabacloud/cr/model/GetSubUserListRequest.h
	include/alibabacloud/cr/model/GetSubUserListResult.h
	include/alibabacloud/cr/model/GetUserInfoRequest.h
	include/alibabacloud/cr/model/GetUserInfoResult.h
	include/alibabacloud/cr/model/GetUserSourceAccountRequest.h
	include/alibabacloud/cr/model/GetUserSourceAccountResult.h
	include/alibabacloud/cr/model/GetUserSourceRepoListRequest.h
	include/alibabacloud/cr/model/GetUserSourceRepoListResult.h
	include/alibabacloud/cr/model/GetUserSourceRepoRefListRequest.h
	include/alibabacloud/cr/model/GetUserSourceRepoRefListResult.h
	include/alibabacloud/cr/model/SearchRepoRequest.h
	include/alibabacloud/cr/model/SearchRepoResult.h
	include/alibabacloud/cr/model/StartImageScanRequest.h
	include/alibabacloud/cr/model/StartImageScanResult.h
	include/alibabacloud/cr/model/StartRepoBuildRequest.h
	include/alibabacloud/cr/model/StartRepoBuildResult.h
	include/alibabacloud/cr/model/StartRepoBuildByRuleRequest.h
	include/alibabacloud/cr/model/StartRepoBuildByRuleResult.h
	include/alibabacloud/cr/model/UpdateNamespaceRequest.h
	include/alibabacloud/cr/model/UpdateNamespaceResult.h
	include/alibabacloud/cr/model/UpdateNamespaceAuthorizationRequest.h
	include/alibabacloud/cr/model/UpdateNamespaceAuthorizationResult.h
	include/alibabacloud/cr/model/UpdateRepoRequest.h
	include/alibabacloud/cr/model/UpdateRepoResult.h
	include/alibabacloud/cr/model/UpdateRepoAuthorizationRequest.h
	include/alibabacloud/cr/model/UpdateRepoAuthorizationResult.h
	include/alibabacloud/cr/model/UpdateRepoBuildRuleRequest.h
	include/alibabacloud/cr/model/UpdateRepoBuildRuleResult.h
	include/alibabacloud/cr/model/UpdateRepoSourceRepoRequest.h
	include/alibabacloud/cr/model/UpdateRepoSourceRepoResult.h
	include/alibabacloud/cr/model/UpdateRepoWebhookRequest.h
	include/alibabacloud/cr/model/UpdateRepoWebhookResult.h
	include/alibabacloud/cr/model/UpdateUserInfoRequest.h
	include/alibabacloud/cr/model/UpdateUserInfoResult.h )

set(cr_src 
	src/CrClient.cc
	src/model/CancelRepoBuildRequest.cc
	src/model/CancelRepoBuildResult.cc
	src/model/CreateCollectionRequest.cc
	src/model/CreateCollectionResult.cc
	src/model/CreateNamespaceRequest.cc
	src/model/CreateNamespaceResult.cc
	src/model/CreateNamespaceAuthorizationRequest.cc
	src/model/CreateNamespaceAuthorizationResult.cc
	src/model/CreateRepoRequest.cc
	src/model/CreateRepoResult.cc
	src/model/CreateRepoAuthorizationRequest.cc
	src/model/CreateRepoAuthorizationResult.cc
	src/model/CreateRepoBuildRuleRequest.cc
	src/model/CreateRepoBuildRuleResult.cc
	src/model/CreateRepoSyncTaskRequest.cc
	src/model/CreateRepoSyncTaskResult.cc
	src/model/CreateRepoWebhookRequest.cc
	src/model/CreateRepoWebhookResult.cc
	src/model/CreateUserInfoRequest.cc
	src/model/CreateUserInfoResult.cc
	src/model/CreateUserSourceAccountRequest.cc
	src/model/CreateUserSourceAccountResult.cc
	src/model/DeleteCollectionRequest.cc
	src/model/DeleteCollectionResult.cc
	src/model/DeleteImageRequest.cc
	src/model/DeleteImageResult.cc
	src/model/DeleteNamespaceRequest.cc
	src/model/DeleteNamespaceResult.cc
	src/model/DeleteNamespaceAuthorizationRequest.cc
	src/model/DeleteNamespaceAuthorizationResult.cc
	src/model/DeleteRepoRequest.cc
	src/model/DeleteRepoResult.cc
	src/model/DeleteRepoAuthorizationRequest.cc
	src/model/DeleteRepoAuthorizationResult.cc
	src/model/DeleteRepoBuildRuleRequest.cc
	src/model/DeleteRepoBuildRuleResult.cc
	src/model/DeleteRepoWebhookRequest.cc
	src/model/DeleteRepoWebhookResult.cc
	src/model/DeleteUserSourceAccountRequest.cc
	src/model/DeleteUserSourceAccountResult.cc
	src/model/GetAuthorizationTokenRequest.cc
	src/model/GetAuthorizationTokenResult.cc
	src/model/GetCollectionRequest.cc
	src/model/GetCollectionResult.cc
	src/model/GetImageLayerRequest.cc
	src/model/GetImageLayerResult.cc
	src/model/GetImageManifestRequest.cc
	src/model/GetImageManifestResult.cc
	src/model/GetImageScanRequest.cc
	src/model/GetImageScanResult.cc
	src/model/GetMirrorListRequest.cc
	src/model/GetMirrorListResult.cc
	src/model/GetNamespaceRequest.cc
	src/model/GetNamespaceResult.cc
	src/model/GetNamespaceAuthorizationListRequest.cc
	src/model/GetNamespaceAuthorizationListResult.cc
	src/model/GetNamespaceListRequest.cc
	src/model/GetNamespaceListResult.cc
	src/model/GetRegionRequest.cc
	src/model/GetRegionResult.cc
	src/model/GetRegionListRequest.cc
	src/model/GetRegionListResult.cc
	src/model/GetRepoRequest.cc
	src/model/GetRepoResult.cc
	src/model/GetRepoAuthorizationListRequest.cc
	src/model/GetRepoAuthorizationListResult.cc
	src/model/GetRepoBatchRequest.cc
	src/model/GetRepoBatchResult.cc
	src/model/GetRepoBuildListRequest.cc
	src/model/GetRepoBuildListResult.cc
	src/model/GetRepoBuildLogsRequest.cc
	src/model/GetRepoBuildLogsResult.cc
	src/model/GetRepoBuildRuleListRequest.cc
	src/model/GetRepoBuildRuleListResult.cc
	src/model/GetRepoBuildStatusRequest.cc
	src/model/GetRepoBuildStatusResult.cc
	src/model/GetRepoListRequest.cc
	src/model/GetRepoListResult.cc
	src/model/GetRepoListByNamespaceRequest.cc
	src/model/GetRepoListByNamespaceResult.cc
	src/model/GetRepoSourceRepoRequest.cc
	src/model/GetRepoSourceRepoResult.cc
	src/model/GetRepoSyncTaskRequest.cc
	src/model/GetRepoSyncTaskResult.cc
	src/model/GetRepoSyncTaskListRequest.cc
	src/model/GetRepoSyncTaskListResult.cc
	src/model/GetRepoTagsRequest.cc
	src/model/GetRepoTagsResult.cc
	src/model/GetRepoWebhookRequest.cc
	src/model/GetRepoWebhookResult.cc
	src/model/GetRepoWebhookLogListRequest.cc
	src/model/GetRepoWebhookLogListResult.cc
	src/model/GetSearchRequest.cc
	src/model/GetSearchResult.cc
	src/model/GetSubUserListRequest.cc
	src/model/GetSubUserListResult.cc
	src/model/GetUserInfoRequest.cc
	src/model/GetUserInfoResult.cc
	src/model/GetUserSourceAccountRequest.cc
	src/model/GetUserSourceAccountResult.cc
	src/model/GetUserSourceRepoListRequest.cc
	src/model/GetUserSourceRepoListResult.cc
	src/model/GetUserSourceRepoRefListRequest.cc
	src/model/GetUserSourceRepoRefListResult.cc
	src/model/SearchRepoRequest.cc
	src/model/SearchRepoResult.cc
	src/model/StartImageScanRequest.cc
	src/model/StartImageScanResult.cc
	src/model/StartRepoBuildRequest.cc
	src/model/StartRepoBuildResult.cc
	src/model/StartRepoBuildByRuleRequest.cc
	src/model/StartRepoBuildByRuleResult.cc
	src/model/UpdateNamespaceRequest.cc
	src/model/UpdateNamespaceResult.cc
	src/model/UpdateNamespaceAuthorizationRequest.cc
	src/model/UpdateNamespaceAuthorizationResult.cc
	src/model/UpdateRepoRequest.cc
	src/model/UpdateRepoResult.cc
	src/model/UpdateRepoAuthorizationRequest.cc
	src/model/UpdateRepoAuthorizationResult.cc
	src/model/UpdateRepoBuildRuleRequest.cc
	src/model/UpdateRepoBuildRuleResult.cc
	src/model/UpdateRepoSourceRepoRequest.cc
	src/model/UpdateRepoSourceRepoResult.cc
	src/model/UpdateRepoWebhookRequest.cc
	src/model/UpdateRepoWebhookResult.cc
	src/model/UpdateUserInfoRequest.cc
	src/model/UpdateUserInfoResult.cc )

add_library(cr ${LIB_TYPE}
	${cr_public_header}
	${cr_public_header_model}
	${cr_src})

set_target_properties(cr
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}cr
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(cr
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_CR_LIBRARY)
endif()

target_include_directories(cr
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(cr
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(cr
		jsoncpp)
	target_include_directories(cr
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(cr
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(cr
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(cr
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(cr
		jsoncpp)
endif()

install(FILES ${cr_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/cr)
install(FILES ${cr_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/cr/model)
install(TARGETS cr
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
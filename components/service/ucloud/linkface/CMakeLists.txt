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

set(linkface_public_header 
	include/alibabacloud/linkface/LinkFaceClient.h
	include/alibabacloud/linkface/LinkFaceExport.h )

set(linkface_public_header_model 
	include/alibabacloud/linkface/model/CreateGroupRequest.h
	include/alibabacloud/linkface/model/CreateGroupResult.h
	include/alibabacloud/linkface/model/DeleteDeviceAllGroupRequest.h
	include/alibabacloud/linkface/model/DeleteDeviceAllGroupResult.h
	include/alibabacloud/linkface/model/DeleteDeviceGroupRequest.h
	include/alibabacloud/linkface/model/DeleteDeviceGroupResult.h
	include/alibabacloud/linkface/model/DeleteFaceRequest.h
	include/alibabacloud/linkface/model/DeleteFaceResult.h
	include/alibabacloud/linkface/model/DeleteGroupRequest.h
	include/alibabacloud/linkface/model/DeleteGroupResult.h
	include/alibabacloud/linkface/model/LinkFaceRequest.h
	include/alibabacloud/linkface/model/LinkFaceResult.h
	include/alibabacloud/linkface/model/QueryAddUserInfoRequest.h
	include/alibabacloud/linkface/model/QueryAddUserInfoResult.h
	include/alibabacloud/linkface/model/QueryAllGroupsRequest.h
	include/alibabacloud/linkface/model/QueryAllGroupsResult.h
	include/alibabacloud/linkface/model/QueryAuthenticationRequest.h
	include/alibabacloud/linkface/model/QueryAuthenticationResult.h
	include/alibabacloud/linkface/model/QueryFaceRequest.h
	include/alibabacloud/linkface/model/QueryFaceResult.h
	include/alibabacloud/linkface/model/QueryGroupUsersRequest.h
	include/alibabacloud/linkface/model/QueryGroupUsersResult.h
	include/alibabacloud/linkface/model/QueryLicensesRequest.h
	include/alibabacloud/linkface/model/QueryLicensesResult.h
	include/alibabacloud/linkface/model/QuerySyncPicScheduleRequest.h
	include/alibabacloud/linkface/model/QuerySyncPicScheduleResult.h
	include/alibabacloud/linkface/model/RegisterFaceRequest.h
	include/alibabacloud/linkface/model/RegisterFaceResult.h
	include/alibabacloud/linkface/model/SearchFaceRequest.h
	include/alibabacloud/linkface/model/SearchFaceResult.h
	include/alibabacloud/linkface/model/SyncFacePicturesRequest.h
	include/alibabacloud/linkface/model/SyncFacePicturesResult.h
	include/alibabacloud/linkface/model/UnlinkFaceRequest.h
	include/alibabacloud/linkface/model/UnlinkFaceResult.h
	include/alibabacloud/linkface/model/UpdateFaceRequest.h
	include/alibabacloud/linkface/model/UpdateFaceResult.h )

set(linkface_src 
	src/LinkFaceClient.cc
	src/model/CreateGroupRequest.cc
	src/model/CreateGroupResult.cc
	src/model/DeleteDeviceAllGroupRequest.cc
	src/model/DeleteDeviceAllGroupResult.cc
	src/model/DeleteDeviceGroupRequest.cc
	src/model/DeleteDeviceGroupResult.cc
	src/model/DeleteFaceRequest.cc
	src/model/DeleteFaceResult.cc
	src/model/DeleteGroupRequest.cc
	src/model/DeleteGroupResult.cc
	src/model/LinkFaceRequest.cc
	src/model/LinkFaceResult.cc
	src/model/QueryAddUserInfoRequest.cc
	src/model/QueryAddUserInfoResult.cc
	src/model/QueryAllGroupsRequest.cc
	src/model/QueryAllGroupsResult.cc
	src/model/QueryAuthenticationRequest.cc
	src/model/QueryAuthenticationResult.cc
	src/model/QueryFaceRequest.cc
	src/model/QueryFaceResult.cc
	src/model/QueryGroupUsersRequest.cc
	src/model/QueryGroupUsersResult.cc
	src/model/QueryLicensesRequest.cc
	src/model/QueryLicensesResult.cc
	src/model/QuerySyncPicScheduleRequest.cc
	src/model/QuerySyncPicScheduleResult.cc
	src/model/RegisterFaceRequest.cc
	src/model/RegisterFaceResult.cc
	src/model/SearchFaceRequest.cc
	src/model/SearchFaceResult.cc
	src/model/SyncFacePicturesRequest.cc
	src/model/SyncFacePicturesResult.cc
	src/model/UnlinkFaceRequest.cc
	src/model/UnlinkFaceResult.cc
	src/model/UpdateFaceRequest.cc
	src/model/UpdateFaceResult.cc )

add_library(linkface ${LIB_TYPE}
	${linkface_public_header}
	${linkface_public_header_model}
	${linkface_src})

set_target_properties(linkface
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}linkface
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(linkface
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_LINKFACE_LIBRARY)
endif()

target_include_directories(linkface
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(linkface
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(linkface
		jsoncpp)
	target_include_directories(linkface
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(linkface
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(linkface
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(linkface
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(linkface
		jsoncpp)
endif()

install(FILES ${linkface_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/linkface)
install(FILES ${linkface_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/linkface/model)
install(TARGETS linkface
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
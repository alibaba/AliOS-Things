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

set(ocr_public_header 
	include/alibabacloud/ocr/OcrClient.h
	include/alibabacloud/ocr/OcrExport.h )

set(ocr_public_header_model 
	include/alibabacloud/ocr/model/DetectCardScreenshotRequest.h
	include/alibabacloud/ocr/model/DetectCardScreenshotResult.h
	include/alibabacloud/ocr/model/GetAsyncJobResultRequest.h
	include/alibabacloud/ocr/model/GetAsyncJobResultResult.h
	include/alibabacloud/ocr/model/RecognizeAccountPageRequest.h
	include/alibabacloud/ocr/model/RecognizeAccountPageResult.h
	include/alibabacloud/ocr/model/RecognizeBankCardRequest.h
	include/alibabacloud/ocr/model/RecognizeBankCardResult.h
	include/alibabacloud/ocr/model/RecognizeBusinessCardRequest.h
	include/alibabacloud/ocr/model/RecognizeBusinessCardResult.h
	include/alibabacloud/ocr/model/RecognizeBusinessLicenseRequest.h
	include/alibabacloud/ocr/model/RecognizeBusinessLicenseResult.h
	include/alibabacloud/ocr/model/RecognizeCharacterRequest.h
	include/alibabacloud/ocr/model/RecognizeCharacterResult.h
	include/alibabacloud/ocr/model/RecognizeChinapassportRequest.h
	include/alibabacloud/ocr/model/RecognizeChinapassportResult.h
	include/alibabacloud/ocr/model/RecognizeDriverLicenseRequest.h
	include/alibabacloud/ocr/model/RecognizeDriverLicenseResult.h
	include/alibabacloud/ocr/model/RecognizeDrivingLicenseRequest.h
	include/alibabacloud/ocr/model/RecognizeDrivingLicenseResult.h
	include/alibabacloud/ocr/model/RecognizeIdentityCardRequest.h
	include/alibabacloud/ocr/model/RecognizeIdentityCardResult.h
	include/alibabacloud/ocr/model/RecognizeLicensePlateRequest.h
	include/alibabacloud/ocr/model/RecognizeLicensePlateResult.h
	include/alibabacloud/ocr/model/RecognizePassportMRZRequest.h
	include/alibabacloud/ocr/model/RecognizePassportMRZResult.h
	include/alibabacloud/ocr/model/RecognizePoiNameRequest.h
	include/alibabacloud/ocr/model/RecognizePoiNameResult.h
	include/alibabacloud/ocr/model/RecognizeQrCodeRequest.h
	include/alibabacloud/ocr/model/RecognizeQrCodeResult.h
	include/alibabacloud/ocr/model/RecognizeStampRequest.h
	include/alibabacloud/ocr/model/RecognizeStampResult.h
	include/alibabacloud/ocr/model/RecognizeTableRequest.h
	include/alibabacloud/ocr/model/RecognizeTableResult.h
	include/alibabacloud/ocr/model/RecognizeTakeoutOrderRequest.h
	include/alibabacloud/ocr/model/RecognizeTakeoutOrderResult.h
	include/alibabacloud/ocr/model/RecognizeTaxiInvoiceRequest.h
	include/alibabacloud/ocr/model/RecognizeTaxiInvoiceResult.h
	include/alibabacloud/ocr/model/RecognizeTrainTicketRequest.h
	include/alibabacloud/ocr/model/RecognizeTrainTicketResult.h
	include/alibabacloud/ocr/model/RecognizeVATInvoiceRequest.h
	include/alibabacloud/ocr/model/RecognizeVATInvoiceResult.h
	include/alibabacloud/ocr/model/RecognizeVINCodeRequest.h
	include/alibabacloud/ocr/model/RecognizeVINCodeResult.h
	include/alibabacloud/ocr/model/RecognizeVerificationcodeRequest.h
	include/alibabacloud/ocr/model/RecognizeVerificationcodeResult.h
	include/alibabacloud/ocr/model/TrimDocumentRequest.h
	include/alibabacloud/ocr/model/TrimDocumentResult.h )

set(ocr_src 
	src/OcrClient.cc
	src/model/DetectCardScreenshotRequest.cc
	src/model/DetectCardScreenshotResult.cc
	src/model/GetAsyncJobResultRequest.cc
	src/model/GetAsyncJobResultResult.cc
	src/model/RecognizeAccountPageRequest.cc
	src/model/RecognizeAccountPageResult.cc
	src/model/RecognizeBankCardRequest.cc
	src/model/RecognizeBankCardResult.cc
	src/model/RecognizeBusinessCardRequest.cc
	src/model/RecognizeBusinessCardResult.cc
	src/model/RecognizeBusinessLicenseRequest.cc
	src/model/RecognizeBusinessLicenseResult.cc
	src/model/RecognizeCharacterRequest.cc
	src/model/RecognizeCharacterResult.cc
	src/model/RecognizeChinapassportRequest.cc
	src/model/RecognizeChinapassportResult.cc
	src/model/RecognizeDriverLicenseRequest.cc
	src/model/RecognizeDriverLicenseResult.cc
	src/model/RecognizeDrivingLicenseRequest.cc
	src/model/RecognizeDrivingLicenseResult.cc
	src/model/RecognizeIdentityCardRequest.cc
	src/model/RecognizeIdentityCardResult.cc
	src/model/RecognizeLicensePlateRequest.cc
	src/model/RecognizeLicensePlateResult.cc
	src/model/RecognizePassportMRZRequest.cc
	src/model/RecognizePassportMRZResult.cc
	src/model/RecognizePoiNameRequest.cc
	src/model/RecognizePoiNameResult.cc
	src/model/RecognizeQrCodeRequest.cc
	src/model/RecognizeQrCodeResult.cc
	src/model/RecognizeStampRequest.cc
	src/model/RecognizeStampResult.cc
	src/model/RecognizeTableRequest.cc
	src/model/RecognizeTableResult.cc
	src/model/RecognizeTakeoutOrderRequest.cc
	src/model/RecognizeTakeoutOrderResult.cc
	src/model/RecognizeTaxiInvoiceRequest.cc
	src/model/RecognizeTaxiInvoiceResult.cc
	src/model/RecognizeTrainTicketRequest.cc
	src/model/RecognizeTrainTicketResult.cc
	src/model/RecognizeVATInvoiceRequest.cc
	src/model/RecognizeVATInvoiceResult.cc
	src/model/RecognizeVINCodeRequest.cc
	src/model/RecognizeVINCodeResult.cc
	src/model/RecognizeVerificationcodeRequest.cc
	src/model/RecognizeVerificationcodeResult.cc
	src/model/TrimDocumentRequest.cc
	src/model/TrimDocumentResult.cc )

add_library(ocr ${LIB_TYPE}
	${ocr_public_header}
	${ocr_public_header_model}
	${ocr_src})

set_target_properties(ocr
	PROPERTIES
	LINKER_LANGUAGE CXX
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}ocr
	)

if(${LIB_TYPE} STREQUAL "SHARED")
	set_target_properties(ocr
		PROPERTIES
		DEFINE_SYMBOL ALIBABACLOUD_OCR_LIBRARY)
endif()

target_include_directories(ocr
	PRIVATE include
		${CMAKE_SOURCE_DIR}/core/include
	)
target_link_libraries(ocr
	core)

if(CMAKE_HOST_WIN32)
	ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
	set(jsoncpp_install_dir ${INSTALL_DIR})
	add_dependencies(ocr
		jsoncpp)
	target_include_directories(ocr
		PRIVATE	${jsoncpp_install_dir}/include)
	target_link_libraries(ocr
		${jsoncpp_install_dir}/lib/jsoncpp.lib)
	set_target_properties(ocr
    	PROPERTIES
    		COMPILE_OPTIONS "/bigobj")
else()
	target_include_directories(ocr
		PRIVATE /usr/include/jsoncpp)
	target_link_libraries(ocr
		jsoncpp)
endif()

install(FILES ${ocr_public_header}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/ocr)
install(FILES ${ocr_public_header_model}
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/alibabacloud/ocr/model)
install(TARGETS ocr
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	)
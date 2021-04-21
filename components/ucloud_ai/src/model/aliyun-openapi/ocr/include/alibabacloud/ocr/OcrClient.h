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

#ifndef ALIBABACLOUD_OCR_OCRCLIENT_H_
#define ALIBABACLOUD_OCR_OCRCLIENT_H_

#include <future>
#include <alibabacloud/core/AsyncCallerContext.h>
#include <alibabacloud/core/EndpointProvider.h>
#include <alibabacloud/core/RpcServiceClient.h>
#include "OcrExport.h"
#include "model/DetectCardScreenshotRequest.h"
#include "model/DetectCardScreenshotResult.h"
#include "model/GetAsyncJobResultRequest.h"
#include "model/GetAsyncJobResultResult.h"
#include "model/RecognizeAccountPageRequest.h"
#include "model/RecognizeAccountPageResult.h"
#include "model/RecognizeBankCardRequest.h"
#include "model/RecognizeBankCardResult.h"
#include "model/RecognizeBusinessCardRequest.h"
#include "model/RecognizeBusinessCardResult.h"
#include "model/RecognizeBusinessLicenseRequest.h"
#include "model/RecognizeBusinessLicenseResult.h"
#include "model/RecognizeCharacterRequest.h"
#include "model/RecognizeCharacterResult.h"
#include "model/RecognizeChinapassportRequest.h"
#include "model/RecognizeChinapassportResult.h"
#include "model/RecognizeDriverLicenseRequest.h"
#include "model/RecognizeDriverLicenseResult.h"
#include "model/RecognizeDrivingLicenseRequest.h"
#include "model/RecognizeDrivingLicenseResult.h"
#include "model/RecognizeIdentityCardRequest.h"
#include "model/RecognizeIdentityCardResult.h"
#include "model/RecognizeLicensePlateRequest.h"
#include "model/RecognizeLicensePlateResult.h"
#include "model/RecognizePassportMRZRequest.h"
#include "model/RecognizePassportMRZResult.h"
#include "model/RecognizePoiNameRequest.h"
#include "model/RecognizePoiNameResult.h"
#include "model/RecognizeQrCodeRequest.h"
#include "model/RecognizeQrCodeResult.h"
#include "model/RecognizeStampRequest.h"
#include "model/RecognizeStampResult.h"
#include "model/RecognizeTableRequest.h"
#include "model/RecognizeTableResult.h"
#include "model/RecognizeTakeoutOrderRequest.h"
#include "model/RecognizeTakeoutOrderResult.h"
#include "model/RecognizeTaxiInvoiceRequest.h"
#include "model/RecognizeTaxiInvoiceResult.h"
#include "model/RecognizeTrainTicketRequest.h"
#include "model/RecognizeTrainTicketResult.h"
#include "model/RecognizeVATInvoiceRequest.h"
#include "model/RecognizeVATInvoiceResult.h"
#include "model/RecognizeVINCodeRequest.h"
#include "model/RecognizeVINCodeResult.h"
#include "model/RecognizeVerificationcodeRequest.h"
#include "model/RecognizeVerificationcodeResult.h"
#include "model/TrimDocumentRequest.h"
#include "model/TrimDocumentResult.h"


namespace AlibabaCloud
{
	namespace Ocr
	{
		class ALIBABACLOUD_OCR_EXPORT OcrClient : public RpcServiceClient
		{
		public:
			typedef Outcome<Error, Model::DetectCardScreenshotResult> DetectCardScreenshotOutcome;
			typedef std::future<DetectCardScreenshotOutcome> DetectCardScreenshotOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::DetectCardScreenshotRequest&, const DetectCardScreenshotOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> DetectCardScreenshotAsyncHandler;
			typedef Outcome<Error, Model::GetAsyncJobResultResult> GetAsyncJobResultOutcome;
			typedef std::future<GetAsyncJobResultOutcome> GetAsyncJobResultOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::GetAsyncJobResultRequest&, const GetAsyncJobResultOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> GetAsyncJobResultAsyncHandler;
			typedef Outcome<Error, Model::RecognizeAccountPageResult> RecognizeAccountPageOutcome;
			typedef std::future<RecognizeAccountPageOutcome> RecognizeAccountPageOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeAccountPageRequest&, const RecognizeAccountPageOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeAccountPageAsyncHandler;
			typedef Outcome<Error, Model::RecognizeBankCardResult> RecognizeBankCardOutcome;
			typedef std::future<RecognizeBankCardOutcome> RecognizeBankCardOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeBankCardRequest&, const RecognizeBankCardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeBankCardAsyncHandler;
			typedef Outcome<Error, Model::RecognizeBusinessCardResult> RecognizeBusinessCardOutcome;
			typedef std::future<RecognizeBusinessCardOutcome> RecognizeBusinessCardOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeBusinessCardRequest&, const RecognizeBusinessCardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeBusinessCardAsyncHandler;
			typedef Outcome<Error, Model::RecognizeBusinessLicenseResult> RecognizeBusinessLicenseOutcome;
			typedef std::future<RecognizeBusinessLicenseOutcome> RecognizeBusinessLicenseOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeBusinessLicenseRequest&, const RecognizeBusinessLicenseOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeBusinessLicenseAsyncHandler;
			typedef Outcome<Error, Model::RecognizeCharacterResult> RecognizeCharacterOutcome;
			typedef std::future<RecognizeCharacterOutcome> RecognizeCharacterOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeCharacterRequest&, const RecognizeCharacterOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeCharacterAsyncHandler;
			typedef Outcome<Error, Model::RecognizeChinapassportResult> RecognizeChinapassportOutcome;
			typedef std::future<RecognizeChinapassportOutcome> RecognizeChinapassportOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeChinapassportRequest&, const RecognizeChinapassportOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeChinapassportAsyncHandler;
			typedef Outcome<Error, Model::RecognizeDriverLicenseResult> RecognizeDriverLicenseOutcome;
			typedef std::future<RecognizeDriverLicenseOutcome> RecognizeDriverLicenseOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeDriverLicenseRequest&, const RecognizeDriverLicenseOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeDriverLicenseAsyncHandler;
			typedef Outcome<Error, Model::RecognizeDrivingLicenseResult> RecognizeDrivingLicenseOutcome;
			typedef std::future<RecognizeDrivingLicenseOutcome> RecognizeDrivingLicenseOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeDrivingLicenseRequest&, const RecognizeDrivingLicenseOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeDrivingLicenseAsyncHandler;
			typedef Outcome<Error, Model::RecognizeIdentityCardResult> RecognizeIdentityCardOutcome;
			typedef std::future<RecognizeIdentityCardOutcome> RecognizeIdentityCardOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeIdentityCardRequest&, const RecognizeIdentityCardOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeIdentityCardAsyncHandler;
			typedef Outcome<Error, Model::RecognizeLicensePlateResult> RecognizeLicensePlateOutcome;
			typedef std::future<RecognizeLicensePlateOutcome> RecognizeLicensePlateOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeLicensePlateRequest&, const RecognizeLicensePlateOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeLicensePlateAsyncHandler;
			typedef Outcome<Error, Model::RecognizePassportMRZResult> RecognizePassportMRZOutcome;
			typedef std::future<RecognizePassportMRZOutcome> RecognizePassportMRZOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizePassportMRZRequest&, const RecognizePassportMRZOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizePassportMRZAsyncHandler;
			typedef Outcome<Error, Model::RecognizePoiNameResult> RecognizePoiNameOutcome;
			typedef std::future<RecognizePoiNameOutcome> RecognizePoiNameOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizePoiNameRequest&, const RecognizePoiNameOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizePoiNameAsyncHandler;
			typedef Outcome<Error, Model::RecognizeQrCodeResult> RecognizeQrCodeOutcome;
			typedef std::future<RecognizeQrCodeOutcome> RecognizeQrCodeOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeQrCodeRequest&, const RecognizeQrCodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeQrCodeAsyncHandler;
			typedef Outcome<Error, Model::RecognizeStampResult> RecognizeStampOutcome;
			typedef std::future<RecognizeStampOutcome> RecognizeStampOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeStampRequest&, const RecognizeStampOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeStampAsyncHandler;
			typedef Outcome<Error, Model::RecognizeTableResult> RecognizeTableOutcome;
			typedef std::future<RecognizeTableOutcome> RecognizeTableOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeTableRequest&, const RecognizeTableOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeTableAsyncHandler;
			typedef Outcome<Error, Model::RecognizeTakeoutOrderResult> RecognizeTakeoutOrderOutcome;
			typedef std::future<RecognizeTakeoutOrderOutcome> RecognizeTakeoutOrderOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeTakeoutOrderRequest&, const RecognizeTakeoutOrderOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeTakeoutOrderAsyncHandler;
			typedef Outcome<Error, Model::RecognizeTaxiInvoiceResult> RecognizeTaxiInvoiceOutcome;
			typedef std::future<RecognizeTaxiInvoiceOutcome> RecognizeTaxiInvoiceOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeTaxiInvoiceRequest&, const RecognizeTaxiInvoiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeTaxiInvoiceAsyncHandler;
			typedef Outcome<Error, Model::RecognizeTrainTicketResult> RecognizeTrainTicketOutcome;
			typedef std::future<RecognizeTrainTicketOutcome> RecognizeTrainTicketOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeTrainTicketRequest&, const RecognizeTrainTicketOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeTrainTicketAsyncHandler;
			typedef Outcome<Error, Model::RecognizeVATInvoiceResult> RecognizeVATInvoiceOutcome;
			typedef std::future<RecognizeVATInvoiceOutcome> RecognizeVATInvoiceOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeVATInvoiceRequest&, const RecognizeVATInvoiceOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeVATInvoiceAsyncHandler;
			typedef Outcome<Error, Model::RecognizeVINCodeResult> RecognizeVINCodeOutcome;
			typedef std::future<RecognizeVINCodeOutcome> RecognizeVINCodeOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeVINCodeRequest&, const RecognizeVINCodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeVINCodeAsyncHandler;
			typedef Outcome<Error, Model::RecognizeVerificationcodeResult> RecognizeVerificationcodeOutcome;
			typedef std::future<RecognizeVerificationcodeOutcome> RecognizeVerificationcodeOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::RecognizeVerificationcodeRequest&, const RecognizeVerificationcodeOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> RecognizeVerificationcodeAsyncHandler;
			typedef Outcome<Error, Model::TrimDocumentResult> TrimDocumentOutcome;
			typedef std::future<TrimDocumentOutcome> TrimDocumentOutcomeCallable;
			typedef std::function<void(const OcrClient*, const Model::TrimDocumentRequest&, const TrimDocumentOutcome&, const std::shared_ptr<const AsyncCallerContext>&)> TrimDocumentAsyncHandler;

			OcrClient(const Credentials &credentials, const ClientConfiguration &configuration);
			OcrClient(const std::shared_ptr<CredentialsProvider> &credentialsProvider, const ClientConfiguration &configuration);
			OcrClient(const std::string &accessKeyId, const std::string &accessKeySecret, const ClientConfiguration &configuration);
			~OcrClient();
			DetectCardScreenshotOutcome detectCardScreenshot(const Model::DetectCardScreenshotRequest &request)const;
			void detectCardScreenshotAsync(const Model::DetectCardScreenshotRequest& request, const DetectCardScreenshotAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			DetectCardScreenshotOutcomeCallable detectCardScreenshotCallable(const Model::DetectCardScreenshotRequest& request) const;
			GetAsyncJobResultOutcome getAsyncJobResult(const Model::GetAsyncJobResultRequest &request)const;
			void getAsyncJobResultAsync(const Model::GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			GetAsyncJobResultOutcomeCallable getAsyncJobResultCallable(const Model::GetAsyncJobResultRequest& request) const;
			RecognizeAccountPageOutcome recognizeAccountPage(const Model::RecognizeAccountPageRequest &request)const;
			void recognizeAccountPageAsync(const Model::RecognizeAccountPageRequest& request, const RecognizeAccountPageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeAccountPageOutcomeCallable recognizeAccountPageCallable(const Model::RecognizeAccountPageRequest& request) const;
			RecognizeBankCardOutcome recognizeBankCard(const Model::RecognizeBankCardRequest &request)const;
			void recognizeBankCardAsync(const Model::RecognizeBankCardRequest& request, const RecognizeBankCardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeBankCardOutcomeCallable recognizeBankCardCallable(const Model::RecognizeBankCardRequest& request) const;
			RecognizeBusinessCardOutcome recognizeBusinessCard(const Model::RecognizeBusinessCardRequest &request)const;
			void recognizeBusinessCardAsync(const Model::RecognizeBusinessCardRequest& request, const RecognizeBusinessCardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeBusinessCardOutcomeCallable recognizeBusinessCardCallable(const Model::RecognizeBusinessCardRequest& request) const;
			RecognizeBusinessLicenseOutcome recognizeBusinessLicense(const Model::RecognizeBusinessLicenseRequest &request)const;
			void recognizeBusinessLicenseAsync(const Model::RecognizeBusinessLicenseRequest& request, const RecognizeBusinessLicenseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeBusinessLicenseOutcomeCallable recognizeBusinessLicenseCallable(const Model::RecognizeBusinessLicenseRequest& request) const;
			RecognizeCharacterOutcome recognizeCharacter(const Model::RecognizeCharacterRequest &request)const;
			void recognizeCharacterAsync(const Model::RecognizeCharacterRequest& request, const RecognizeCharacterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeCharacterOutcomeCallable recognizeCharacterCallable(const Model::RecognizeCharacterRequest& request) const;
			RecognizeChinapassportOutcome recognizeChinapassport(const Model::RecognizeChinapassportRequest &request)const;
			void recognizeChinapassportAsync(const Model::RecognizeChinapassportRequest& request, const RecognizeChinapassportAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeChinapassportOutcomeCallable recognizeChinapassportCallable(const Model::RecognizeChinapassportRequest& request) const;
			RecognizeDriverLicenseOutcome recognizeDriverLicense(const Model::RecognizeDriverLicenseRequest &request)const;
			void recognizeDriverLicenseAsync(const Model::RecognizeDriverLicenseRequest& request, const RecognizeDriverLicenseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeDriverLicenseOutcomeCallable recognizeDriverLicenseCallable(const Model::RecognizeDriverLicenseRequest& request) const;
			RecognizeDrivingLicenseOutcome recognizeDrivingLicense(const Model::RecognizeDrivingLicenseRequest &request)const;
			void recognizeDrivingLicenseAsync(const Model::RecognizeDrivingLicenseRequest& request, const RecognizeDrivingLicenseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeDrivingLicenseOutcomeCallable recognizeDrivingLicenseCallable(const Model::RecognizeDrivingLicenseRequest& request) const;
			RecognizeIdentityCardOutcome recognizeIdentityCard(const Model::RecognizeIdentityCardRequest &request)const;
			void recognizeIdentityCardAsync(const Model::RecognizeIdentityCardRequest& request, const RecognizeIdentityCardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeIdentityCardOutcomeCallable recognizeIdentityCardCallable(const Model::RecognizeIdentityCardRequest& request) const;
			RecognizeLicensePlateOutcome recognizeLicensePlate(const Model::RecognizeLicensePlateRequest &request)const;
			void recognizeLicensePlateAsync(const Model::RecognizeLicensePlateRequest& request, const RecognizeLicensePlateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeLicensePlateOutcomeCallable recognizeLicensePlateCallable(const Model::RecognizeLicensePlateRequest& request) const;
			RecognizePassportMRZOutcome recognizePassportMRZ(const Model::RecognizePassportMRZRequest &request)const;
			void recognizePassportMRZAsync(const Model::RecognizePassportMRZRequest& request, const RecognizePassportMRZAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizePassportMRZOutcomeCallable recognizePassportMRZCallable(const Model::RecognizePassportMRZRequest& request) const;
			RecognizePoiNameOutcome recognizePoiName(const Model::RecognizePoiNameRequest &request)const;
			void recognizePoiNameAsync(const Model::RecognizePoiNameRequest& request, const RecognizePoiNameAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizePoiNameOutcomeCallable recognizePoiNameCallable(const Model::RecognizePoiNameRequest& request) const;
			RecognizeQrCodeOutcome recognizeQrCode(const Model::RecognizeQrCodeRequest &request)const;
			void recognizeQrCodeAsync(const Model::RecognizeQrCodeRequest& request, const RecognizeQrCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeQrCodeOutcomeCallable recognizeQrCodeCallable(const Model::RecognizeQrCodeRequest& request) const;
			RecognizeStampOutcome recognizeStamp(const Model::RecognizeStampRequest &request)const;
			void recognizeStampAsync(const Model::RecognizeStampRequest& request, const RecognizeStampAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeStampOutcomeCallable recognizeStampCallable(const Model::RecognizeStampRequest& request) const;
			RecognizeTableOutcome recognizeTable(const Model::RecognizeTableRequest &request)const;
			void recognizeTableAsync(const Model::RecognizeTableRequest& request, const RecognizeTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeTableOutcomeCallable recognizeTableCallable(const Model::RecognizeTableRequest& request) const;
			RecognizeTakeoutOrderOutcome recognizeTakeoutOrder(const Model::RecognizeTakeoutOrderRequest &request)const;
			void recognizeTakeoutOrderAsync(const Model::RecognizeTakeoutOrderRequest& request, const RecognizeTakeoutOrderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeTakeoutOrderOutcomeCallable recognizeTakeoutOrderCallable(const Model::RecognizeTakeoutOrderRequest& request) const;
			RecognizeTaxiInvoiceOutcome recognizeTaxiInvoice(const Model::RecognizeTaxiInvoiceRequest &request)const;
			void recognizeTaxiInvoiceAsync(const Model::RecognizeTaxiInvoiceRequest& request, const RecognizeTaxiInvoiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeTaxiInvoiceOutcomeCallable recognizeTaxiInvoiceCallable(const Model::RecognizeTaxiInvoiceRequest& request) const;
			RecognizeTrainTicketOutcome recognizeTrainTicket(const Model::RecognizeTrainTicketRequest &request)const;
			void recognizeTrainTicketAsync(const Model::RecognizeTrainTicketRequest& request, const RecognizeTrainTicketAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeTrainTicketOutcomeCallable recognizeTrainTicketCallable(const Model::RecognizeTrainTicketRequest& request) const;
			RecognizeVATInvoiceOutcome recognizeVATInvoice(const Model::RecognizeVATInvoiceRequest &request)const;
			void recognizeVATInvoiceAsync(const Model::RecognizeVATInvoiceRequest& request, const RecognizeVATInvoiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeVATInvoiceOutcomeCallable recognizeVATInvoiceCallable(const Model::RecognizeVATInvoiceRequest& request) const;
			RecognizeVINCodeOutcome recognizeVINCode(const Model::RecognizeVINCodeRequest &request)const;
			void recognizeVINCodeAsync(const Model::RecognizeVINCodeRequest& request, const RecognizeVINCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeVINCodeOutcomeCallable recognizeVINCodeCallable(const Model::RecognizeVINCodeRequest& request) const;
			RecognizeVerificationcodeOutcome recognizeVerificationcode(const Model::RecognizeVerificationcodeRequest &request)const;
			void recognizeVerificationcodeAsync(const Model::RecognizeVerificationcodeRequest& request, const RecognizeVerificationcodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			RecognizeVerificationcodeOutcomeCallable recognizeVerificationcodeCallable(const Model::RecognizeVerificationcodeRequest& request) const;
			TrimDocumentOutcome trimDocument(const Model::TrimDocumentRequest &request)const;
			void trimDocumentAsync(const Model::TrimDocumentRequest& request, const TrimDocumentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context = nullptr) const;
			TrimDocumentOutcomeCallable trimDocumentCallable(const Model::TrimDocumentRequest& request) const;
	
		private:
			std::shared_ptr<EndpointProvider> endpointProvider_;
		};
	}
}

#endif // !ALIBABACLOUD_OCR_OCRCLIENT_H_

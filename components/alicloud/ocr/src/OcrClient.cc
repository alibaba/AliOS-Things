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

#include <alibabacloud/ocr/OcrClient.h>
#include <alibabacloud/core/SimpleCredentialsProvider.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Location;
using namespace AlibabaCloud::Ocr;
using namespace AlibabaCloud::Ocr::Model;

namespace
{
	const std::string SERVICE_NAME = "ocr";
}

OcrClient::OcrClient(const Credentials &credentials, const ClientConfiguration &configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(credentials), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentials, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "ocr");
}

OcrClient::OcrClient(const std::shared_ptr<CredentialsProvider>& credentialsProvider, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, credentialsProvider, configuration)
{
	auto locationClient = std::make_shared<LocationClient>(credentialsProvider, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "ocr");
}

OcrClient::OcrClient(const std::string & accessKeyId, const std::string & accessKeySecret, const ClientConfiguration & configuration) :
	RpcServiceClient(SERVICE_NAME, std::make_shared<SimpleCredentialsProvider>(accessKeyId, accessKeySecret), configuration)
{
	auto locationClient = std::make_shared<LocationClient>(accessKeyId, accessKeySecret, configuration);
	endpointProvider_ = std::make_shared<EndpointProvider>(locationClient, configuration.regionId(), SERVICE_NAME, "ocr");
}

OcrClient::~OcrClient()
{}

OcrClient::DetectCardScreenshotOutcome OcrClient::detectCardScreenshot(const DetectCardScreenshotRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return DetectCardScreenshotOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return DetectCardScreenshotOutcome(DetectCardScreenshotResult(outcome.result()));
	else
		return DetectCardScreenshotOutcome(outcome.error());
}

void OcrClient::detectCardScreenshotAsync(const DetectCardScreenshotRequest& request, const DetectCardScreenshotAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, detectCardScreenshot(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::DetectCardScreenshotOutcomeCallable OcrClient::detectCardScreenshotCallable(const DetectCardScreenshotRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<DetectCardScreenshotOutcome()>>(
			[this, request]()
			{
			return this->detectCardScreenshot(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::GetAsyncJobResultOutcome OcrClient::getAsyncJobResult(const GetAsyncJobResultRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return GetAsyncJobResultOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return GetAsyncJobResultOutcome(GetAsyncJobResultResult(outcome.result()));
	else
		return GetAsyncJobResultOutcome(outcome.error());
}

void OcrClient::getAsyncJobResultAsync(const GetAsyncJobResultRequest& request, const GetAsyncJobResultAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, getAsyncJobResult(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::GetAsyncJobResultOutcomeCallable OcrClient::getAsyncJobResultCallable(const GetAsyncJobResultRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<GetAsyncJobResultOutcome()>>(
			[this, request]()
			{
			return this->getAsyncJobResult(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeAccountPageOutcome OcrClient::recognizeAccountPage(const RecognizeAccountPageRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeAccountPageOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeAccountPageOutcome(RecognizeAccountPageResult(outcome.result()));
	else
		return RecognizeAccountPageOutcome(outcome.error());
}

void OcrClient::recognizeAccountPageAsync(const RecognizeAccountPageRequest& request, const RecognizeAccountPageAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeAccountPage(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeAccountPageOutcomeCallable OcrClient::recognizeAccountPageCallable(const RecognizeAccountPageRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeAccountPageOutcome()>>(
			[this, request]()
			{
			return this->recognizeAccountPage(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeBankCardOutcome OcrClient::recognizeBankCard(const RecognizeBankCardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeBankCardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeBankCardOutcome(RecognizeBankCardResult(outcome.result()));
	else
		return RecognizeBankCardOutcome(outcome.error());
}

void OcrClient::recognizeBankCardAsync(const RecognizeBankCardRequest& request, const RecognizeBankCardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeBankCard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeBankCardOutcomeCallable OcrClient::recognizeBankCardCallable(const RecognizeBankCardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeBankCardOutcome()>>(
			[this, request]()
			{
			return this->recognizeBankCard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeBusinessCardOutcome OcrClient::recognizeBusinessCard(const RecognizeBusinessCardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeBusinessCardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeBusinessCardOutcome(RecognizeBusinessCardResult(outcome.result()));
	else
		return RecognizeBusinessCardOutcome(outcome.error());
}

void OcrClient::recognizeBusinessCardAsync(const RecognizeBusinessCardRequest& request, const RecognizeBusinessCardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeBusinessCard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeBusinessCardOutcomeCallable OcrClient::recognizeBusinessCardCallable(const RecognizeBusinessCardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeBusinessCardOutcome()>>(
			[this, request]()
			{
			return this->recognizeBusinessCard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeBusinessLicenseOutcome OcrClient::recognizeBusinessLicense(const RecognizeBusinessLicenseRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeBusinessLicenseOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeBusinessLicenseOutcome(RecognizeBusinessLicenseResult(outcome.result()));
	else
		return RecognizeBusinessLicenseOutcome(outcome.error());
}

void OcrClient::recognizeBusinessLicenseAsync(const RecognizeBusinessLicenseRequest& request, const RecognizeBusinessLicenseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeBusinessLicense(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeBusinessLicenseOutcomeCallable OcrClient::recognizeBusinessLicenseCallable(const RecognizeBusinessLicenseRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeBusinessLicenseOutcome()>>(
			[this, request]()
			{
			return this->recognizeBusinessLicense(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeCharacterOutcome OcrClient::recognizeCharacter(const RecognizeCharacterRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeCharacterOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeCharacterOutcome(RecognizeCharacterResult(outcome.result()));
	else
		return RecognizeCharacterOutcome(outcome.error());
}

void OcrClient::recognizeCharacterAsync(const RecognizeCharacterRequest& request, const RecognizeCharacterAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeCharacter(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeCharacterOutcomeCallable OcrClient::recognizeCharacterCallable(const RecognizeCharacterRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeCharacterOutcome()>>(
			[this, request]()
			{
			return this->recognizeCharacter(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeChinapassportOutcome OcrClient::recognizeChinapassport(const RecognizeChinapassportRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeChinapassportOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeChinapassportOutcome(RecognizeChinapassportResult(outcome.result()));
	else
		return RecognizeChinapassportOutcome(outcome.error());
}

void OcrClient::recognizeChinapassportAsync(const RecognizeChinapassportRequest& request, const RecognizeChinapassportAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeChinapassport(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeChinapassportOutcomeCallable OcrClient::recognizeChinapassportCallable(const RecognizeChinapassportRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeChinapassportOutcome()>>(
			[this, request]()
			{
			return this->recognizeChinapassport(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeDriverLicenseOutcome OcrClient::recognizeDriverLicense(const RecognizeDriverLicenseRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeDriverLicenseOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeDriverLicenseOutcome(RecognizeDriverLicenseResult(outcome.result()));
	else
		return RecognizeDriverLicenseOutcome(outcome.error());
}

void OcrClient::recognizeDriverLicenseAsync(const RecognizeDriverLicenseRequest& request, const RecognizeDriverLicenseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeDriverLicense(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeDriverLicenseOutcomeCallable OcrClient::recognizeDriverLicenseCallable(const RecognizeDriverLicenseRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeDriverLicenseOutcome()>>(
			[this, request]()
			{
			return this->recognizeDriverLicense(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeDrivingLicenseOutcome OcrClient::recognizeDrivingLicense(const RecognizeDrivingLicenseRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeDrivingLicenseOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeDrivingLicenseOutcome(RecognizeDrivingLicenseResult(outcome.result()));
	else
		return RecognizeDrivingLicenseOutcome(outcome.error());
}

void OcrClient::recognizeDrivingLicenseAsync(const RecognizeDrivingLicenseRequest& request, const RecognizeDrivingLicenseAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeDrivingLicense(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeDrivingLicenseOutcomeCallable OcrClient::recognizeDrivingLicenseCallable(const RecognizeDrivingLicenseRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeDrivingLicenseOutcome()>>(
			[this, request]()
			{
			return this->recognizeDrivingLicense(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeIdentityCardOutcome OcrClient::recognizeIdentityCard(const RecognizeIdentityCardRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeIdentityCardOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeIdentityCardOutcome(RecognizeIdentityCardResult(outcome.result()));
	else
		return RecognizeIdentityCardOutcome(outcome.error());
}

void OcrClient::recognizeIdentityCardAsync(const RecognizeIdentityCardRequest& request, const RecognizeIdentityCardAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeIdentityCard(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeIdentityCardOutcomeCallable OcrClient::recognizeIdentityCardCallable(const RecognizeIdentityCardRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeIdentityCardOutcome()>>(
			[this, request]()
			{
			return this->recognizeIdentityCard(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeLicensePlateOutcome OcrClient::recognizeLicensePlate(const RecognizeLicensePlateRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeLicensePlateOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeLicensePlateOutcome(RecognizeLicensePlateResult(outcome.result()));
	else
		return RecognizeLicensePlateOutcome(outcome.error());
}

void OcrClient::recognizeLicensePlateAsync(const RecognizeLicensePlateRequest& request, const RecognizeLicensePlateAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeLicensePlate(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeLicensePlateOutcomeCallable OcrClient::recognizeLicensePlateCallable(const RecognizeLicensePlateRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeLicensePlateOutcome()>>(
			[this, request]()
			{
			return this->recognizeLicensePlate(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizePassportMRZOutcome OcrClient::recognizePassportMRZ(const RecognizePassportMRZRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizePassportMRZOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizePassportMRZOutcome(RecognizePassportMRZResult(outcome.result()));
	else
		return RecognizePassportMRZOutcome(outcome.error());
}

void OcrClient::recognizePassportMRZAsync(const RecognizePassportMRZRequest& request, const RecognizePassportMRZAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizePassportMRZ(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizePassportMRZOutcomeCallable OcrClient::recognizePassportMRZCallable(const RecognizePassportMRZRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizePassportMRZOutcome()>>(
			[this, request]()
			{
			return this->recognizePassportMRZ(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizePoiNameOutcome OcrClient::recognizePoiName(const RecognizePoiNameRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizePoiNameOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizePoiNameOutcome(RecognizePoiNameResult(outcome.result()));
	else
		return RecognizePoiNameOutcome(outcome.error());
}

void OcrClient::recognizePoiNameAsync(const RecognizePoiNameRequest& request, const RecognizePoiNameAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizePoiName(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizePoiNameOutcomeCallable OcrClient::recognizePoiNameCallable(const RecognizePoiNameRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizePoiNameOutcome()>>(
			[this, request]()
			{
			return this->recognizePoiName(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeQrCodeOutcome OcrClient::recognizeQrCode(const RecognizeQrCodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeQrCodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeQrCodeOutcome(RecognizeQrCodeResult(outcome.result()));
	else
		return RecognizeQrCodeOutcome(outcome.error());
}

void OcrClient::recognizeQrCodeAsync(const RecognizeQrCodeRequest& request, const RecognizeQrCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeQrCode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeQrCodeOutcomeCallable OcrClient::recognizeQrCodeCallable(const RecognizeQrCodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeQrCodeOutcome()>>(
			[this, request]()
			{
			return this->recognizeQrCode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeStampOutcome OcrClient::recognizeStamp(const RecognizeStampRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeStampOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeStampOutcome(RecognizeStampResult(outcome.result()));
	else
		return RecognizeStampOutcome(outcome.error());
}

void OcrClient::recognizeStampAsync(const RecognizeStampRequest& request, const RecognizeStampAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeStamp(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeStampOutcomeCallable OcrClient::recognizeStampCallable(const RecognizeStampRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeStampOutcome()>>(
			[this, request]()
			{
			return this->recognizeStamp(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeTableOutcome OcrClient::recognizeTable(const RecognizeTableRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeTableOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeTableOutcome(RecognizeTableResult(outcome.result()));
	else
		return RecognizeTableOutcome(outcome.error());
}

void OcrClient::recognizeTableAsync(const RecognizeTableRequest& request, const RecognizeTableAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeTable(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeTableOutcomeCallable OcrClient::recognizeTableCallable(const RecognizeTableRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeTableOutcome()>>(
			[this, request]()
			{
			return this->recognizeTable(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeTakeoutOrderOutcome OcrClient::recognizeTakeoutOrder(const RecognizeTakeoutOrderRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeTakeoutOrderOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeTakeoutOrderOutcome(RecognizeTakeoutOrderResult(outcome.result()));
	else
		return RecognizeTakeoutOrderOutcome(outcome.error());
}

void OcrClient::recognizeTakeoutOrderAsync(const RecognizeTakeoutOrderRequest& request, const RecognizeTakeoutOrderAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeTakeoutOrder(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeTakeoutOrderOutcomeCallable OcrClient::recognizeTakeoutOrderCallable(const RecognizeTakeoutOrderRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeTakeoutOrderOutcome()>>(
			[this, request]()
			{
			return this->recognizeTakeoutOrder(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeTaxiInvoiceOutcome OcrClient::recognizeTaxiInvoice(const RecognizeTaxiInvoiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeTaxiInvoiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeTaxiInvoiceOutcome(RecognizeTaxiInvoiceResult(outcome.result()));
	else
		return RecognizeTaxiInvoiceOutcome(outcome.error());
}

void OcrClient::recognizeTaxiInvoiceAsync(const RecognizeTaxiInvoiceRequest& request, const RecognizeTaxiInvoiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeTaxiInvoice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeTaxiInvoiceOutcomeCallable OcrClient::recognizeTaxiInvoiceCallable(const RecognizeTaxiInvoiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeTaxiInvoiceOutcome()>>(
			[this, request]()
			{
			return this->recognizeTaxiInvoice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeTrainTicketOutcome OcrClient::recognizeTrainTicket(const RecognizeTrainTicketRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeTrainTicketOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeTrainTicketOutcome(RecognizeTrainTicketResult(outcome.result()));
	else
		return RecognizeTrainTicketOutcome(outcome.error());
}

void OcrClient::recognizeTrainTicketAsync(const RecognizeTrainTicketRequest& request, const RecognizeTrainTicketAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeTrainTicket(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeTrainTicketOutcomeCallable OcrClient::recognizeTrainTicketCallable(const RecognizeTrainTicketRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeTrainTicketOutcome()>>(
			[this, request]()
			{
			return this->recognizeTrainTicket(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeVATInvoiceOutcome OcrClient::recognizeVATInvoice(const RecognizeVATInvoiceRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeVATInvoiceOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeVATInvoiceOutcome(RecognizeVATInvoiceResult(outcome.result()));
	else
		return RecognizeVATInvoiceOutcome(outcome.error());
}

void OcrClient::recognizeVATInvoiceAsync(const RecognizeVATInvoiceRequest& request, const RecognizeVATInvoiceAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeVATInvoice(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeVATInvoiceOutcomeCallable OcrClient::recognizeVATInvoiceCallable(const RecognizeVATInvoiceRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeVATInvoiceOutcome()>>(
			[this, request]()
			{
			return this->recognizeVATInvoice(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeVINCodeOutcome OcrClient::recognizeVINCode(const RecognizeVINCodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeVINCodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeVINCodeOutcome(RecognizeVINCodeResult(outcome.result()));
	else
		return RecognizeVINCodeOutcome(outcome.error());
}

void OcrClient::recognizeVINCodeAsync(const RecognizeVINCodeRequest& request, const RecognizeVINCodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeVINCode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeVINCodeOutcomeCallable OcrClient::recognizeVINCodeCallable(const RecognizeVINCodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeVINCodeOutcome()>>(
			[this, request]()
			{
			return this->recognizeVINCode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::RecognizeVerificationcodeOutcome OcrClient::recognizeVerificationcode(const RecognizeVerificationcodeRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return RecognizeVerificationcodeOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return RecognizeVerificationcodeOutcome(RecognizeVerificationcodeResult(outcome.result()));
	else
		return RecognizeVerificationcodeOutcome(outcome.error());
}

void OcrClient::recognizeVerificationcodeAsync(const RecognizeVerificationcodeRequest& request, const RecognizeVerificationcodeAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, recognizeVerificationcode(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::RecognizeVerificationcodeOutcomeCallable OcrClient::recognizeVerificationcodeCallable(const RecognizeVerificationcodeRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<RecognizeVerificationcodeOutcome()>>(
			[this, request]()
			{
			return this->recognizeVerificationcode(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}

OcrClient::TrimDocumentOutcome OcrClient::trimDocument(const TrimDocumentRequest &request) const
{
	auto endpointOutcome = endpointProvider_->getEndpoint();
	if (!endpointOutcome.isSuccess())
		return TrimDocumentOutcome(endpointOutcome.error());

	auto outcome = makeRequest(endpointOutcome.result(), request);

	if (outcome.isSuccess())
		return TrimDocumentOutcome(TrimDocumentResult(outcome.result()));
	else
		return TrimDocumentOutcome(outcome.error());
}

void OcrClient::trimDocumentAsync(const TrimDocumentRequest& request, const TrimDocumentAsyncHandler& handler, const std::shared_ptr<const AsyncCallerContext>& context) const
{
	auto fn = [this, request, handler, context]()
	{
		handler(this, request, trimDocument(request), context);
	};

	asyncExecute(new Runnable(fn));
}

OcrClient::TrimDocumentOutcomeCallable OcrClient::trimDocumentCallable(const TrimDocumentRequest &request) const
{
	auto task = std::make_shared<std::packaged_task<TrimDocumentOutcome()>>(
			[this, request]()
			{
			return this->trimDocument(request);
			});

	asyncExecute(new Runnable([task]() { (*task)(); }));
	return task->get_future();
}


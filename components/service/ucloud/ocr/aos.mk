# component name
NAME := ocr
# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := add aliyun ocr ai open api.
# source files and the folder of internal include files

$(NAME)_SOURCES += src/OcrClient.cc \
                src/model/DetectCardScreenshotRequest.cc \
                src/model/DetectCardScreenshotResult.cc \
                src/model/GetAsyncJobResultRequest.cc \
                src/model/GetAsyncJobResultResult.cc \
                src/model/RecognizeAccountPageRequest.cc \
                src/model/RecognizeAccountPageResult.cc \
                src/model/RecognizeBankCardRequest.cc \
                src/model/RecognizeBankCardResult.cc \
                src/model/RecognizeBusinessCardRequest.cc \
                src/model/RecognizeBusinessCardResult.cc \
                src/model/RecognizeBusinessLicenseRequest.cc \
                src/model/RecognizeBusinessLicenseResult.cc \
                src/model/RecognizeCharacterRequest.cc \
                src/model/RecognizeCharacterResult.cc \
                src/model/RecognizeChinapassportRequest.cc \
                src/model/RecognizeChinapassportResult.cc \
                src/model/RecognizeDriverLicenseRequest.cc \
                src/model/RecognizeDriverLicenseResult.cc \
                src/model/RecognizeDrivingLicenseRequest.cc \
                src/model/RecognizeDrivingLicenseResult.cc \
                src/model/RecognizeIdentityCardRequest.cc \
                src/model/RecognizeIdentityCardResult.cc \
                src/model/RecognizeLicensePlateRequest.cc \
                src/model/RecognizeLicensePlateResult.cc \
                src/model/RecognizePassportMRZRequest.cc \
                src/model/RecognizePassportMRZResult.cc \
                src/model/RecognizePoiNameRequest.cc \
                src/model/RecognizePoiNameResult.cc \
                src/model/RecognizeQrCodeRequest.cc \
                src/model/RecognizeQrCodeResult.cc \
                src/model/RecognizeStampRequest.cc \
                src/model/RecognizeStampResult.cc \
                src/model/RecognizeTableRequest.cc \
                src/model/RecognizeTableResult.cc \
                src/model/RecognizeTakeoutOrderRequest.cc \
                src/model/RecognizeTakeoutOrderResult.cc \
                src/model/RecognizeTaxiInvoiceRequest.cc \
                src/model/RecognizeTaxiInvoiceResult.cc \
                src/model/RecognizeTrainTicketRequest.cc \
                src/model/RecognizeTrainTicketResult.cc \
                src/model/RecognizeVATInvoiceRequest.cc \
                src/model/RecognizeVATInvoiceResult.cc \
                src/model/RecognizeVINCodeRequest.cc \
                src/model/RecognizeVINCodeResult.cc \
                src/model/RecognizeVerificationcodeRequest.cc \
                src/model/RecognizeVerificationcodeResult.cc \
                src/model/TrimDocumentRequest.cc \
                src/model/TrimDocumentResult.cc
            
$(NAME)_INCLUDES += ocr/include
GLOBAL_INCLUDES += ocr/include


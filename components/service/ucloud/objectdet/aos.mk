# component name
NAME := objectdet
# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := add aliyun open api include ai vision and so on.
# source files and the folder of internal include files

$(NAME)_COMPONENTS += mbedtls jsoncpp osal_posix oss


#face and body AI detection
$(NAME)_SOURCES += ./src/objectdet.cc \
                ./src/ObjectdetClient.cc \
                ./src/model/ClassifyVehicleInsuranceRequest.cc \
                ./src/model/ClassifyVehicleInsuranceResult.cc \
                ./src/model/DetectMainBodyRequest.cc \
                ./src/model/DetectMainBodyResult.cc \
                ./src/model/DetectObjectRequest.cc \
                ./src/model/DetectObjectResult.cc \
                ./src/model/DetectTransparentImageRequest.cc \
                ./src/model/DetectTransparentImageResult.cc \
                ./src/model/DetectVehicleRequest.cc \
                ./src/model/DetectVehicleResult.cc \
                ./src/model/DetectWhiteBaseImageRequest.cc \
                ./src/model/DetectWhiteBaseImageResult.cc \
                ./src/model/GenerateVehicleRepairPlanRequest.cc \
                ./src/model/GenerateVehicleRepairPlanResult.cc \
                ./src/model/GetVehicleRepairPlanRequest.cc \
                ./src/model/GetVehicleRepairPlanResult.cc \
                ./src/model/RecognizeVehicleDamageRequest.cc \
                ./src/model/RecognizeVehicleDamageResult.cc \
                ./src/model/RecognizeVehicleDashboardRequest.cc \
                ./src/model/RecognizeVehicleDashboardResult.cc \
                ./src/model/RecognizeVehiclePartsRequest.cc \
                ./src/model/RecognizeVehiclePartsResult.cc

$(NAME)_INCLUDES += ./include
GLOBAL_INCLUDES += ./include

GLOBAL_DEFINES += UCLOUD_AI_OBJECTDET_CONFIG
GLOBAL_DEFINES += USE_CRYPTO_MBEDTLS USE_AOS_TIME_POSIX_API

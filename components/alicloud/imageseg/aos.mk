# component name
NAME := imageseg
# component information
$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := add aliyun open api include ai vision and so on.
# source files and the folder of internal include files

#image segment detection
$(NAME)_SOURCES += ./src/imageseg.cc \
                ./src/ImagesegClient.cc \
                ./src/model/ChangeSkyRequest.cc \
                ./src/model/ChangeSkyResult.cc \
                ./src/model/GetAsyncJobResultRequest.cc \
                ./src/model/GetAsyncJobResultResult.cc \
                ./src/model/ParseFaceRequest.cc \
                ./src/model/ParseFaceResult.cc \
                ./src/model/RefineMaskRequest.cc \
                ./src/model/RefineMaskResult.cc \
                ./src/model/SegmentAnimalRequest.cc \
                ./src/model/SegmentAnimalResult.cc \
                ./src/model/SegmentBodyRequest.cc \
                ./src/model/SegmentBodyResult.cc \
                ./src/model/SegmentClothRequest.cc \
                ./src/model/SegmentClothResult.cc \
                ./src/model/SegmentCommodityRequest.cc \
                ./src/model/SegmentCommodityResult.cc \
                ./src/model/SegmentCommonImageRequest.cc \
                ./src/model/SegmentCommonImageResult.cc \
                ./src/model/SegmentFaceRequest.cc \
                ./src/model/SegmentFaceResult.cc \
                ./src/model/SegmentFoodRequest.cc \
                ./src/model/SegmentFoodResult.cc \
                ./src/model/SegmentFurnitureRequest.cc \
                ./src/model/SegmentFurnitureResult.cc \
                ./src/model/SegmentHDBodyRequest.cc \
                ./src/model/SegmentHDBodyResult.cc \
                ./src/model/SegmentHDCommonImageRequest.cc \
                ./src/model/SegmentHDCommonImageResult.cc \
                ./src/model/SegmentHDSkyRequest.cc \
                ./src/model/SegmentHDSkyResult.cc \
                ./src/model/SegmentHairRequest.cc \
                ./src/model/SegmentHairResult.cc \
                ./src/model/SegmentHeadRequest.cc \
                ./src/model/SegmentHeadResult.cc \
                ./src/model/SegmentLogoRequest.cc \
                ./src/model/SegmentLogoResult.cc \
                ./src/model/SegmentSceneRequest.cc \
                ./src/model/SegmentSceneResult.cc \
                ./src/model/SegmentSkinRequest.cc \
                ./src/model/SegmentSkinResult.cc \
                ./src/model/SegmentSkyRequest.cc \
                ./src/model/SegmentSkyResult.cc \
                ./src/model/SegmentVehicleRequest.cc \
                ./src/model/SegmentVehicleResult.cc

$(NAME)_INCLUDES += ./include
GLOBAL_INCLUDES += ./include

GLOBAL_DEFINES += UCLOUD_AI_IMAGESEG_CONFIG
